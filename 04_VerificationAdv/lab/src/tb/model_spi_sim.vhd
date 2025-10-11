library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library std;
use std.textio.all;

library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;
use work.tb_cas_pkg.all;

library osvvm;
context osvvm.osvvmcontext;

entity model_spi is
  port( 
    clk        : in  std_ulogic;
    model_ctrl : in  t_spi_model_ctrl;
    rst_n      : in  std_ulogic;
    spi_si     : in  std_ulogic;
    cmd_ack_n  : out std_ulogic;
    spi_cs_n   : out std_ulogic;
    spi_sclk   : out std_ulogic;
    spi_so     : out std_ulogic
  );
end entity model_spi;

architecture sim of model_spi is

  constant c_inst_name : string := PathTail(model_spi'path_name);
  signal model_id      : AlertLogIDType;
  signal activity      : t_spi_state     := idle;
  signal echo_data     : t_spi_echo_data := (others => (others => '0'));
  signal tx_cmd        : std_ulogic_vector(c_cmd_bits-1 downto 0) := (others => '0');

begin

  model_id <= GetAlertLogID(c_inst_name);

  -------------------------------------------------------------
  p_spi : process
    variable v_data_nr  : u_unsigned(c_cmd_dbn_bits-1 downto 0)  := (others => '0');
    variable v_tx_cmd   : std_ulogic_vector(c_cmd_bits-1 downto 0) := (others => '0');
    variable v_crc      : std_ulogic_vector(c_cmd_crc_bits-1 downto 0) := (others => '1');
    variable v_byte_cnt : integer    := 0;
    variable v_rd_wrn   : std_ulogic := c_write;
    variable v_echo_len : integer    := 0;
  begin
    spi_cs_n <= '1';
    spi_sclk <= '0';
    spi_so   <= '0';
    
    -- suspends a model until a transaction is requested via RequestTransaction
    WaitForTransaction(clk => clk, rdy => model_ctrl.cmd_rdy, ack => cmd_ack_n);    
    -------------------------------------------------------------
    v_data_nr := u_unsigned(model_ctrl.data_nr);
    if model_ctrl.spi_op = wr then
       v_rd_wrn := c_write;
    else
       v_rd_wrn := c_read;
    end if;
    v_tx_cmd := v_rd_wrn & std_ulogic_vector(v_data_nr) & model_ctrl.addr;
    log(model_id, "v_tx_cmd = 0x"&to_hstring(v_tx_cmd), debug, c_log_spi_debug);
    tx_cmd   <= v_tx_cmd;
    wait for 0 ns;

    -- crc calculation
    v_crc := next_crc16_d8(v_tx_cmd( 7 downto 0), X"FF_FF");  -- cmd low byte first
    v_crc := next_crc16_d8(v_tx_cmd(15 downto 8), v_crc);     -- cmd high byte second

    if v_rd_wrn = c_write then 
      for i in 0 to to_integer(v_data_nr) loop
        v_crc := next_crc16_d8(model_ctrl.data(i), v_crc);
      end loop;
    end if;
    log(model_id, "v_crc = 0x"&to_hstring(v_crc), debug, c_log_spi_debug);

    if model_ctrl.set_crc_err then
      v_crc := not(v_crc);  -- generate a checksum error
    end if;
    
    log(model_id, "### tx-cmd : 0x"&to_hstring(v_tx_cmd)&"                                  ", info, c_log_spi_info);
    if v_tx_cmd(15) = c_read then
      log(model_id, "###   tx-cmd(15)    = '1'    (rd-cmd)                ", info, c_log_spi_info);
    else 
      log(model_id, "###   tx-cmd(15)    = '0'    (wr-cmd)                ", info, c_log_spi_info);
    end if;
    log(model_id,   "###   tx-cmd(14:12) = 0x"&to_hstring(v_tx_cmd(14 downto 12))&"    (+1 = number of bytes)  ", info, c_log_spi_info);
    log(model_id,   "###   tx-cmd(11: 0) = 0x"&to_hstring(v_tx_cmd(11 downto 0)) &"  (start-addr)            ", info, c_log_spi_info);            

    if v_rd_wrn = c_write then 
      for i in 0 to to_integer(v_data_nr) loop
        log(model_id, "### tx-data(" & to_string(i) & ") : 0x" & to_hstring(model_ctrl.data(i))&"                                ", info, c_log_spi_info);
      end loop;
    end if;
    log(model_id, "### tx-crc: 0x"&to_hstring(v_crc)&"                                   ", info, c_log_spi_info);

    -------------------------------------
    wait until rising_edge(clk);
    wait for c_clk_period/4;
    -------------------------------------
    -- so = addr (lsb first)
    activity <= addr_bits;
    spi_cs_n <= '0';
    for i in model_ctrl.addr'reverse_range loop  -- cmd low byte, lsb first
      spi_sclk <= '0';
      spi_so   <= model_ctrl.addr(i);
      v_byte_cnt := v_byte_cnt+1;
      wait for model_ctrl.sclk_period/2;
      spi_sclk <= '1' when spi_cs_n = '0' else '0';
      wait for model_ctrl.sclk_period/2;
    end loop;
    -- so = nr of bytes
    activity <= dbn_bits;
    for i in model_ctrl.data_nr'reverse_range loop 
      spi_sclk <= '0';
      spi_so   <= v_data_nr(i);
      v_byte_cnt := v_byte_cnt+1;
      wait for model_ctrl.sclk_period/2;
      spi_sclk <= '1' when spi_cs_n = '0' else '0';
      wait for model_ctrl.sclk_period/2;
    end loop;
    -- so = '0' = write / so = '1' = read
    activity <= rd_wr_bit;
    spi_sclk <= '0';
    spi_so   <= v_rd_wrn;
    v_byte_cnt := v_byte_cnt+1;
    wait for model_ctrl.sclk_period/2;
    spi_sclk <= '1' when spi_cs_n = '0' else '0';
    wait for model_ctrl.sclk_period/2;
    if v_rd_wrn = c_write then     
      -- so = data
      activity    <= data_bits;
      for j in 0 to to_integer(v_data_nr) loop
        for i in 0 to c_csr_data_bits-1 loop
          spi_sclk <= '0';
          spi_so   <= model_ctrl.data(j)(i);
          v_byte_cnt := v_byte_cnt+1;
          wait for model_ctrl.sclk_period/2;
          spi_sclk <= '1' when spi_cs_n = '0' else '0';
          wait for model_ctrl.sclk_period/2;
        end loop;
      end loop;
    end if;
    -- so = crc, high byte first, lsb first
    activity <= crc_bits;
    for i in v_crc(15 downto 8)'reverse_range loop
      spi_sclk <= '0';
      spi_so   <= v_crc(i);
      v_byte_cnt := v_byte_cnt+1;
      wait for model_ctrl.sclk_period/2;
      spi_sclk <= '1' when spi_cs_n = '0' else '0';
      wait for model_ctrl.sclk_period/2;
    end loop;    
    for i in v_crc( 7 downto 0)'reverse_range loop
      spi_sclk <= '0';
      spi_so   <= v_crc(i);
      v_byte_cnt := v_byte_cnt+1;
      wait for model_ctrl.sclk_period/2;
      spi_sclk <= '1' when spi_cs_n = '0' else '0';
      wait for model_ctrl.sclk_period/2;
    end loop;    
    -- si = echo, low byte first, lsb first 
    activity <= echo_bits;
    if v_rd_wrn = c_write then     
      v_echo_len := (c_cmd_bits+c_cmd_status_bits+c_cmd_crc_bits)/c_csr_data_bits;
    else
      v_echo_len := (c_cmd_bits+c_cmd_status_bits+c_cmd_crc_bits)/c_csr_data_bits+to_integer(v_data_nr)+1;  -- +1 in case data_nr=8 --> model_ctrl.data_nr="000"
    end if;
    for j in 0 to v_echo_len-1 loop
      for i in 0 to c_csr_data_bits-1 loop  -- lsb first
        spi_sclk <= '0';
        spi_so   <= '0';  -- drive dummy data
        v_byte_cnt := v_byte_cnt+1;
        wait for model_ctrl.sclk_period/2;
        spi_sclk     <= '1' when spi_cs_n = '0' else '0';
        echo_data(j)(i) <= spi_si;  -- store read data
        log(model_id, "echo_data("&to_string(j)&")("&to_string(i)&") = "&to_string(spi_si), debug, c_log_spi_debug);
        wait for model_ctrl.sclk_period/2;
      end loop;
    end loop;
    -- end
    activity <= done;
    spi_sclk <= '0';
    spi_so   <= '0';
    v_byte_cnt := 0;
    wait for model_ctrl.sclk_period/2;
    activity <= idle;
    spi_cs_n <= '1';
    wait for model_ctrl.sclk_period/2;  -- wait half a period before next spi access
  end process p_spi;
  -------------------------------------------------------------


  -------------------------------------------------------------
  p_check_spi_echo : process(activity)
    variable v_rd_cmd    : boolean := false;
    variable v_rd_data   : integer := 0;  -- number of received read-data
    variable v_crc       : std_ulogic_vector(c_cmd_crc_bits-1 downto 0) := (others => '0');
    variable v_crc_final : std_ulogic_vector(c_cmd_crc_bits-1 downto 0) := (others => '0');       
  begin
    if activity = done then
      if model_ctrl.echo_exp.check_echo then  -- separate line to avoid synthesis warning:

        if echo_data(1)(7) = c_read then
          v_rd_cmd := true;
        else
          v_rd_cmd := false;
        end if;

        -- check 1st and 2nd byte: cmd
        ------------------------------
        if echo_data(1) & echo_data(0) = tx_cmd then
          log(model_id, "### echo-cmd ok : 0x"&to_hstring(echo_data(1) & echo_data(0))&"                             ", info, c_log_spi_info);
          if echo_data(1)(7) = '1' then
            log(model_id, "###   echo-cmd(15)    = '1'    (rd-cmd)              ", info, c_log_spi_info);
          else
            log(model_id, "###   echo-cmd(15)    = '0'    (wr-cmd)              ", info, c_log_spi_info);
          end if;
          log(model_id, "###   echo-cmd(14:12) = 0x"&to_hstring(echo_data(1)(6 downto 4))&"    (+1 = number of bytes)"             , info, c_log_spi_info);
          log(model_id, "###   echo-cmd(11: 0) = 0x"&to_hstring(echo_data(1)(3 downto 0) & echo_data(0))&"  (start-addr)          ", info, c_log_spi_info);
        end if;
        AffirmIf(model_id, echo_data(1) & echo_data(0) = tx_cmd, "###   echo-cmd wrong : expected 0x"&to_hstring(tx_cmd)&" / received 0x"&to_hstring(echo_data(1) & echo_data(0)));

        -- check 3rd byte: status
        ---------------------------------
        log(model_id, "### echo-status : 0x"&to_hstring(echo_data(2))&"                               ", info, c_log_spi_info);
        if echo_data(2)(7) = '1' then
          log(model_id, "###   echo-status(7)  = '1'    (status-bit(7) set)", info, c_log_spi_info);
        end if;
        if echo_data(2)(6) = '1' then
          log(model_id, "###   echo-status(6)  = '1'    (status-bit(6) set)", info, c_log_spi_info);
        end if;
        if echo_data(2)(5) = '1' then
          log(model_id, "###   echo-status(5)  = '1'    (status-bit(5) set)", info, c_log_spi_info);
        end if;
        if echo_data(2)(4) = '1' then
          log(model_id, "###   echo-status(4)  = '1'    (status-bit(4) set)", info, c_log_spi_info);
        end if;
        if echo_data(2)(3) = '1' then
          log(model_id, "###   echo-status(3)  = '1'    (status-bit(3) set)", info, c_log_spi_info);
        end if;
        if echo_data(2)(2) = '1' then
          log(model_id, "###   echo-status(2)  = '1'    (cmd-crc-error)       ", info, c_log_spi_info);
        end if;
        if echo_data(2)(1) = '1' then
          log(model_id, "###   echo-status(1)  = '1'    (cmd-error : reg does not exist)", info, c_log_spi_info);
        end if;
        if echo_data(2)(0) = '1' then
          log(model_id, "###   echo-status(0)  = '1'    (cmd-operation-error : write read-only-reg / read wr-only-reg) ", info, c_log_spi_info);
        end if;
        AffirmIf(model_id, echo_data(2)(2) = model_ctrl.echo_exp.status(2), "###   echo-status( 2) : cmd-crc-error                  wrong: "&to_string(echo_data(2)(2))&" instead of "&to_string(model_ctrl.echo_exp.status(2)));
        AffirmIf(model_id, echo_data(2)(1) = model_ctrl.echo_exp.status(1), "###   echo-status( 1) : cmd-error (reg does not exist) wrong: "&to_string(echo_data(2)(1))&" instead of "&to_string(model_ctrl.echo_exp.status(1)));
        AffirmIf(model_id, echo_data(2)(0) = model_ctrl.echo_exp.status(0), "###   echo-status( 0) : cmd-reject                     wrong: "&to_string(echo_data(2)(0))&" instead of "&to_string(model_ctrl.echo_exp.status(0)));

        -- check read-bytes if read-cmd
        -------------------------------
        if v_rd_cmd then         -- read-command
          v_rd_data := to_integer(unsigned(echo_data(1)(6 downto 4)))+1;  -- for n = (cmd(14:12)+1 bytes
          for i in 0 to v_rd_data-1 loop
            log(model_id, "### echo-data("&to_string(i)&") : 0x"&to_hstring(echo_data(i+3)), info, c_log_spi_info);
            if model_ctrl.echo_exp.reg_data(i) /= "--------" then
              AffirmIf(model_id,  echo_data(i+3) = model_ctrl.echo_exp.reg_data(i), "###   echo-data("&to_string(i)&") wrong: 0x"&to_hstring(echo_data(i+4))&" instead of 0x"&to_hstring(model_ctrl.echo_exp.reg_data(i)));
            end if;
          end loop;
        end if;

        -- check 2nd last and last byte: crc (msb transmitted first)
        ------------------------------------------------------------
        v_crc := next_crc16_d8(echo_data(0), X"FF_FF");   -- init crc
        if v_rd_cmd then         -- read-command
          for i in 1 to (c_cmd_bits+c_cmd_status_bits)/c_csr_data_bits+v_rd_data-1 loop
            v_crc := next_crc16_d8(echo_data(i), v_crc);
            log(model_id, "rd: v_crc("&to_string(i)&") = 0x"&to_hstring(v_crc), debug, c_log_spi_debug);
          end loop;
          v_crc_final := next_crc16_d8(echo_data((c_cmd_bits+c_cmd_status_bits)/c_csr_data_bits+v_rd_data  ), v_crc      );  -- update crc with high byte
          v_crc_final := next_crc16_d8(echo_data((c_cmd_bits+c_cmd_status_bits)/c_csr_data_bits+v_rd_data+1), v_crc_final);  -- update crc with low byte  --> must be zero
 
        else                     -- write-command
          for i in 1 to (c_cmd_bits+c_cmd_status_bits)/c_csr_data_bits-1 loop
            v_crc := next_crc16_d8(echo_data(i), v_crc);
            log(model_id, "wr: v_crc("&to_string(i)&") = 0x"&to_hstring(v_crc), debug, c_log_spi_debug);
          end loop;
          v_crc_final := next_crc16_d8(echo_data((c_cmd_bits+c_cmd_status_bits)/c_csr_data_bits  ), v_crc      );  -- update crc with high byte
          v_crc_final := next_crc16_d8(echo_data((c_cmd_bits+c_cmd_status_bits)/c_csr_data_bits+1), v_crc_final);  -- update crc with low byte  --> must be zero
        end if;
        log(model_id, "v_crc_final = 0x"&to_hstring(v_crc_final), debug, c_log_spi_debug);
 
        if v_crc_final = X"0000" then
          log(model_id, "### echo-crc ok : 0x"&to_hstring(v_crc)&"                             ", info, c_log_spi_info);
        end if;
        AffirmIf(model_id, v_crc_final = X"0000", "###   echo-crc wrong : received 0x"&to_hstring(v_crc));
      end if;
    end if;

  end process p_check_spi_echo;
  
  
end architecture sim;

