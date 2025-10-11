library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

entity spi_com is
  port( 
    clk                         : in  std_ulogic;
    cmd_status                  : in  t_cmd_status;
    frame_fifo_level            : in  unsigned (c_frame_fifo_level_bits-1 downto 0);
    reg_settings                : in  t_reg_settings;
    rst_n                       : in  std_ulogic;
    spi_frame_calc_crc          : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    spi_frame_update_tx_crc_1st : in  std_ulogic;
    spi_frame_update_tx_crc_2nd : in  std_ulogic;
    spi_frame_wr_crc            : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    spi_rd_data                 : in  std_ulogic_vector (c_csr_data_bits-1 downto 0);
    spi_rd_data_rdy             : in  std_ulogic;
    spi_slave_addr_rdy          : in  std_ulogic;
    spi_slave_base_addr         : in  std_ulogic_vector (c_cmd_addr_bits-1 downto 0);
    spi_slave_busy              : in  std_ulogic;
    spi_slave_cmd_crc           : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    spi_slave_nr_bytes          : in  std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    spi_slave_rd_cmd_crc_err    : in  std_ulogic;
    spi_slave_rd_cmd_crc_rdy    : in  std_ulogic;
    spi_slave_rd_req            : in  std_ulogic;
    spi_slave_rd_wrn            : in  std_ulogic;
    spi_slave_update_status     : in  std_ulogic;
    spi_slave_wr_addr_rdy       : in  std_ulogic;
    spi_slave_wr_req            : in  std_ulogic;
    spi_addr_rdy                : out std_ulogic;
    spi_early_rd_req            : out std_ulogic;
    spi_frame_clear_fifo        : out std_ulogic;
    spi_frame_done              : out std_ulogic;
    spi_frame_fifo_rd_req       : out std_ulogic;
    spi_frame_fifo_wr_req       : out std_ulogic;
    spi_rd_req                  : out std_ulogic;
    spi_rd_wrn                  : out std_ulogic;
    spi_slave_echo_crc          : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    spi_slave_rd_data           : out std_ulogic_vector (c_csr_data_bits-1 downto 0);
    spi_slave_status            : out std_ulogic_vector (c_cmd_status_bits-1 downto 0);
    spi_wr_req                  : out std_ulogic
  );
end entity spi_com ;


architecture rtl of spi_com is
  signal addr_rdy_ff           : std_ulogic_vector(1 to 3);
  signal wr_addr_rdy_ff        : std_ulogic_vector(1 to 3);  -- for spi_master_arbiter
  signal wr_req_ff             : std_ulogic_vector(1 to 3);
  signal rd_cmd_crc_ff         : std_ulogic_vector(1 to 3);
  signal rd_req_ff             : std_ulogic_vector(1 to 3);
  signal busy_ff               : std_ulogic_vector(1 to 3);
  signal update_ff             : std_ulogic_vector(1 to 3);
  signal wr_cmd_crc_rdy        : std_ulogic;
  signal rd_cmd_crc_rdy_sync   : std_ulogic;
  signal wr_addr_flag          : std_ulogic;
  signal spi_update_status     : std_ulogic;
  signal spi_update_crc        : std_ulogic;
  signal spi_slave_done        : std_ulogic;
  signal spi_slave_status_i    : std_ulogic_vector(c_cmd_status_bits-1 downto 0);  -- due to intelquartus vhdl-2008 problem
  signal spi_slave_echo_crc_i  : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);     -- due to intelquartus vhdl-2008 problem
  signal spi_slave_rd_data_i   : std_ulogic_vector(c_csr_data_bits-1 downto 0);    -- due to intelquartus vhdl-2008 problem

  signal spi_early_rd_cnt      : unsigned(c_cmd_dbn_bits-1 downto 0);              -- number of read-bytes with auto-inc = 0..15
  signal spi_cmd_rdy           : std_ulogic;
  signal clear_fifo            : std_ulogic;
  signal wr_cmd_crc_err        : std_ulogic;
  signal rd_cmd_crc_err_stored : std_ulogic;
  
  signal wr_cmd_crc_ok_flag    : std_ulogic;
  signal wait_empty_flag       : std_ulogic;
 
  signal update_crc_1st_ff     : std_ulogic_vector(1 to 3);
  signal update_crc_2nd_ff     : std_ulogic_vector(1 to 3);
  signal update_crc_1st_sync   : std_ulogic;
  signal update_crc_2nd_sync   : std_ulogic;
  signal final_wr_crc          : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);
 
  signal nvm_busy_d            : std_ulogic;  -- synchronous to clk, 1 ff is enough
  signal nvm_busy_done         : std_ulogic;
 
begin

  spi_slave_status                <= spi_slave_status_i;    -- due to intelquartus vhdl-2008 problem
  spi_slave_echo_crc( 7 downto 0) <= spi_slave_echo_crc_i(15 downto 8);  -- high byte transmitted first
  spi_slave_echo_crc(15 downto 8) <= spi_slave_echo_crc_i( 7 downto 0);  -- low byte transmitted second
  spi_cmd_rdy                     <= spi_addr_rdy;
  spi_slave_rd_data_i             <= spi_rd_data when rd_cmd_crc_err_stored = '0' else (others => '0');  -- due to intelquartus vhdl-2008 problem
  
  p_slave_rd_data : process(rst_n, clk)
  begin
    if rst_n = '0' then
      spi_slave_rd_data <= (others => '0');
    elsif rising_edge(clk) then
      if spi_slave_done = '1' then      
        spi_slave_rd_data <= (others => '0');      -- reset
      elsif spi_rd_data_rdy = '1' then  
        spi_slave_rd_data <= spi_slave_rd_data_i;  -- set
      end if;
    end if;
  end process p_slave_rd_data;

  spi_frame_clear_fifo <= clear_fifo;
  
 -------------------------------------------------------------
  p_sync : process(rst_n, clk)
  begin
    if rst_n = '0' then
      addr_rdy_ff       <= (others => '0');
      wr_addr_rdy_ff    <= (others => '0');
      wr_req_ff         <= (others => '0');
      rd_cmd_crc_ff     <= (others => '0');
      rd_req_ff         <= (others => '0');
      busy_ff           <= (others => '0');
      update_ff         <= (others => '0');
      update_crc_1st_ff <= (others => '0');
      update_crc_2nd_ff <= (others => '0');
      wr_cmd_crc_rdy    <= '0';
      nvm_busy_d        <= '0';
    elsif rising_edge(clk) then
      addr_rdy_ff       <= spi_slave_addr_rdy       & addr_rdy_ff    (1 to 2);
      wr_addr_rdy_ff    <= spi_slave_wr_addr_rdy    & wr_addr_rdy_ff (1 to 2);
      wr_req_ff         <= spi_slave_wr_req         & wr_req_ff      (1 to 2);
      rd_cmd_crc_ff     <= spi_slave_rd_cmd_crc_rdy & rd_cmd_crc_ff  (1 to 2);
      rd_req_ff         <= spi_slave_rd_req         & rd_req_ff      (1 to 2);
      busy_ff           <= spi_slave_busy           & busy_ff        (1 to 2);
      update_ff         <= spi_slave_update_status  & update_ff      (1 to 2);
      update_crc_1st_ff <= spi_frame_update_tx_crc_1st & update_crc_1st_ff  (1 to 2);
      update_crc_2nd_ff <= spi_frame_update_tx_crc_2nd & update_crc_2nd_ff  (1 to 2);
      wr_cmd_crc_rdy    <= update_crc_2nd_sync;
    end if;
  end process p_sync;

  -- rising-edge-detection:
  spi_addr_rdy          <= addr_rdy_ff      (2) and not(addr_rdy_ff      (3));
  spi_rd_wrn            <= addr_rdy_ff      (2) and not(addr_rdy_ff      (3)) and spi_slave_rd_wrn;
  spi_frame_fifo_wr_req <= wr_req_ff        (2) and not(wr_req_ff        (3));  -- store received data
  spi_rd_req            <= rd_req_ff        (2) and not(rd_req_ff        (3));
  rd_cmd_crc_rdy_sync   <= rd_cmd_crc_ff    (2) and not(rd_cmd_crc_ff    (3));

  update_crc_1st_sync   <= update_crc_1st_ff(2) and not(update_crc_1st_ff(3));
  update_crc_2nd_sync   <= update_crc_2nd_ff(2) and not(update_crc_2nd_ff(3));

  -- falling-edge-detection:
  spi_slave_done        <= not(busy_ff      (2)) and    busy_ff          (3);
  spi_frame_done        <= spi_slave_done;
  
  spi_update_status     <= not(update_ff    (2)) and    update_ff        (3);

  clear_fifo            <= '1' when (wr_cmd_crc_rdy = '1'    and wr_cmd_crc_err = '1'          ) or  -- immediately in case of a crc-err
                                    (spi_slave_done = '1'    and wr_cmd_crc_ok_flag = '0'      ) or  -- reset fifo always at the end of an access (no crc-rdy due to cs_n too short)
                                    (spi_slave_done = '1'    and frame_fifo_level = 0          ) or  -- end of an access (no crc-err and fifo empty)
                                    (wait_empty_flag = '1'   and frame_fifo_level = 0          ) or  -- pulse at end of wait_empty_flag (if no crc-err, wait until empty)
                                    (spi_update_status = '1' and cmd_status.cmd_opr_error = '1')     -- writing to page2 of nv-ram (read-only)
                                                                                                 else '0'; 
                                                                                                                                                                                      
  -- moved from "spi_store_data" due to e-342, request only
  -- (might not work with sclk = 4 mbit/s)
  -- for crc-err: final crc must be zero
  p_crc : process(rst_n, clk)
  begin
    if rst_n = '0' then
      final_wr_crc <= (others => '0');
    elsif rising_edge(clk) then
      if spi_slave_done = '1' then            -- reset
        final_wr_crc <= (others => '0');
      elsif update_crc_1st_sync = '1' then 
        final_wr_crc <= next_crc16_d8(spi_frame_wr_crc(15 downto 8), spi_frame_calc_crc);  -- init, valid byte is always left-bound (shifted in from left)
      elsif update_crc_2nd_sync = '1' then
        final_wr_crc <= next_crc16_d8(spi_frame_wr_crc(15 downto 8), final_wr_crc      );  -- valid byte is always left-bound (shifted in from left)        
      end if;
    end if;
  end process p_crc;

  wr_cmd_crc_err <= '1' when final_wr_crc /= x"0000" else '0';


  p_wr_cmd_crc_ok_flag : process(rst_n, clk)
  begin
    if rst_n = '0' then
      wr_cmd_crc_ok_flag <= '0';
    elsif rising_edge(clk) then
      if wr_cmd_crc_rdy = '1' and wr_cmd_crc_err = '0' then 
        wr_cmd_crc_ok_flag <= '1';       
      elsif spi_slave_done = '1' and frame_fifo_level = 0 then
        wr_cmd_crc_ok_flag <= '0';       
      end if;
    end if;
  end process p_wr_cmd_crc_ok_flag;


  p_wait_empty_flag : process(rst_n, clk)
  begin
    if rst_n = '0' then
      wait_empty_flag <= '0';
    elsif rising_edge(clk) then
      if spi_slave_done = '1' and frame_fifo_level /= 0 and wr_cmd_crc_ok_flag = '1' then
        wait_empty_flag <= '1'; 
      elsif frame_fifo_level = 0 then       
        wait_empty_flag <= '0'; 
      end if;
    end if;
  end process p_wait_empty_flag;


  p_early_rd_cnt : process(rst_n, clk)
  begin
    if rst_n = '0' then
      spi_early_rd_cnt <= (others => '0');
    elsif rising_edge(clk) then
      if spi_cmd_rdy = '1' and spi_slave_rd_wrn = c_read then  -- only read, "spi_cmd_rdy" identical to "spi_addr_rdy"
        spi_early_rd_cnt <= unsigned(spi_slave_nr_bytes);
      else
        if spi_early_rd_cnt /= 0 then
          spi_early_rd_cnt <= spi_early_rd_cnt-1;
        end if;
      end if;
    end if;
  end process p_early_rd_cnt;


  p_early_rd_req : process(rst_n, clk)
  begin
    if rst_n = '0' then
      spi_early_rd_req <= '0';
    elsif rising_edge(clk) then
      if (spi_cmd_rdy = '1' and spi_slave_rd_wrn = c_read) or spi_early_rd_cnt /= 0 then 
        spi_early_rd_req <= '1';
      else
        spi_early_rd_req <= '0';
      end if;
    end if;
  end process p_early_rd_req;


  p_wr_addr_flag : process(rst_n, clk)
  begin
    if rst_n = '0' then
      wr_addr_flag <= '0';
    elsif rising_edge(clk) then
      if (wr_addr_rdy_ff(2) and not(wr_addr_rdy_ff(3))) = '1' then  -- rising-edge
        wr_addr_flag <= '1';
      elsif wr_cmd_crc_rdy = '1' then 
        wr_addr_flag <= '0';
      end if;
    end if;
  end process p_wr_addr_flag;
  
  
  -- read from frame-fifo
  p_frame_data_rd_req : process(rst_n, clk)
  begin
    if rst_n = '0' then
      spi_frame_fifo_rd_req <= '0';
    elsif rising_edge(clk) then
      if (wr_cmd_crc_rdy = '1' and wr_cmd_crc_err = '0') or      -- there was no crc-err
         (nvm_busy_done = '1' and frame_fifo_level /= 0)   then  -- nv-mem access and not finished yet
        spi_frame_fifo_rd_req <= '1';
      elsif spi_frame_fifo_rd_req = '1' and      -- already reading
            frame_fifo_level = 1           then  -- equals 1 to have fifo-level = 0 after read
        spi_frame_fifo_rd_req <= '0';
      end if;
    end if;
  end process p_frame_data_rd_req;
  
  
  -- write frame-rd-data to registers:
  p_wr_req : process(rst_n, clk)
  begin
    if rst_n = '0' then
      spi_wr_req <= '0';
    elsif rising_edge(clk) then
      spi_wr_req <= spi_frame_fifo_rd_req;
    end if;
  end process p_wr_req;
  
  -------------------------------------------------------------
  
  p_send_data: process(rst_n, clk)
  begin
    if rst_n = '0' then
      spi_slave_status_i  <= (others => '0');
    elsif rising_edge(clk) then
      if spi_update_status = '1'  then          -- update status-bits after sending echo-cmd-low
        spi_slave_status_i(c_cmd_op_err_bit) <= cmd_status.cmd_opr_error;
--         spi_slave_status_i(c_cmd_reject_bit) <= cmd_status.cmd_reject;
        spi_slave_status_i(c_cmd_err_bit   ) <= cmd_status.cmd_error;
      end if;
          
      if (wr_cmd_crc_rdy = '1' and wr_cmd_crc_err = '1') or 
         (rd_cmd_crc_rdy_sync = '1' and spi_slave_rd_cmd_crc_err = '1')   then
        spi_slave_status_i(c_cmd_crc_err_bit) <= '1';
      end if;

      if spi_slave_done = '1'  then                    
        spi_slave_status_i  <= (others => '0');  -- clear all
      end if;

    end if;
  end process p_send_data;
  
  -------------------------------------------------------------

  p_delay : process(rst_n, clk)
  begin
    if rst_n = '0' then
      spi_update_crc <= '0';
    elsif rising_edge(clk) then
      spi_update_crc <= spi_update_status;
    end if;
  end process p_delay;
  
  p_calc_echo_crc : process(rst_n, clk)
    variable v_crc : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);
  begin
    if rst_n = '0' then
      spi_slave_echo_crc_i <= (others => '0');
    elsif rising_edge(clk) then
      if spi_update_crc = '1' then 
--         v_crc                := next_crc16_d8(spi_slave_status_i( 7 downto 0), spi_slave_cmd_crc);  -- init crc with low byte
--         spi_slave_echo_crc_i <= next_crc16_d8(spi_slave_status_i(15 downto 8), v_crc);  
        spi_slave_echo_crc_i <= next_crc16_d8(spi_slave_status_i, spi_slave_cmd_crc); 
        
      elsif spi_rd_data_rdy = '1' then
        spi_slave_echo_crc_i <= next_crc16_d8(spi_slave_rd_data_i, spi_slave_echo_crc_i);  
      
      end if;
    end if;
  end process p_calc_echo_crc;
     
  p_store_rd_cmd_crc_error : process(rst_n, clk)
  begin
    if rst_n = '0' then    
      rd_cmd_crc_err_stored <= '0';
    elsif rising_edge(clk) then
      if spi_slave_done = '1' then                                             -- reset
        rd_cmd_crc_err_stored <= '0';
      elsif rd_cmd_crc_rdy_sync = '1' and spi_slave_rd_cmd_crc_err = '1' then  -- set
        rd_cmd_crc_err_stored <= '1';
      end if;      
    end if;
  end process p_store_rd_cmd_crc_error;
  

end architecture rtl;

