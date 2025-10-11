library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library std;
use std.textio.all;

library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

library osvvm;
context osvvm.osvvmcontext;

package tb_cas_pkg is

  -------------------------------------------------------------
  -- global
  constant c_clk_period    : time := 65.104 ns;  -- t =  65.104 ns -> f = 15.36 mhz: e-358
  constant c_comment_char  : character := '#';
  constant c_break_char    : character := '-';
  constant c_log_path      : string := "./logs/sim_logs/";
  constant c_log_tc_debug  : boolean := false;  -- if true, logging is enabled
  constant c_log_tc_info   : boolean := true;  -- if true, logging is enabled
  constant c_log_spi_debug : boolean := false;  -- if true, logging is enabled
  constant c_log_spi_info  : boolean := true;  -- if true, logging is enabled
  constant c_log_prefix    : string := "###      ";
  constant c_alrt_prefix   : string := "###      ";
  constant c_rpt_prefix    : string := "### ";
  -------------------------------------------------------------


  -------------------------------------------------------------
  -- external clock
  -------------------------------------------------------------
  type t_clk_ctrl is record
    clk_enable    : boolean;
    clk_period    : time;
    clk_rise_time : time;  -- absolut within clk_ext_period
    clk_high_time : time;
  end record;

  constant c_clk_ctrl_init : t_clk_ctrl := (clk_enable    => false,
                                            clk_period    => c_clk_period,
                                            clk_rise_time => c_clk_period/2,
                                            clk_high_time => c_clk_period/2);
  -------------------------------------------------------------


  -------------------------------------------------------------
  -- spi
  -------------------------------------------------------------
  type t_spi_tx_data is array (0 to c_frame_fifo_depth-1) of std_ulogic_vector(c_csr_data_bits-1 downto 0);  -- contains data to send with auto-inc
  type t_spi_rx_data is array (0 to 2**c_cmd_dbn_bits-1 ) of std_ulogic_vector(c_csr_data_bits-1 downto 0);
  type t_exp_spi_echo_data is record 
    check_echo : boolean;
    status     : std_ulogic_vector(c_cmd_status_bits-1 downto 0);
    reg_data   : t_spi_rx_data;
  end record;
  ------------------------------------------------------------------------------  
  type t_spi_echo_data is array (0 to (c_cmd_bits+c_cmd_status_bits+c_cmd_crc_bits)/8+c_frame_fifo_depth-1) of std_ulogic_vector(c_csr_data_bits-1 downto 0); 
  type t_spi_state is (idle, addr_bits, dbn_bits, rd_wr_bit, data_bits, crc_bits, echo_bits, done);  
  type t_spi_msg   is (ok, err);
  type t_spi_rd_wr is (rd, wr);

  type t_spi_model_ctrl is record
    cmd_rdy     : std_ulogic;
    sclk_period : time;
    spi_op      : t_spi_rd_wr;
    addr        : std_ulogic_vector(c_cmd_addr_bits-1 downto 0);
    data        : t_spi_tx_data;
    data_nr     : std_ulogic_vector(c_cmd_dbn_bits-1 downto 0);  -- number of bytes to write/read
    echo_exp    : t_exp_spi_echo_data;  -- expected echo
    set_crc_err : boolean;
  end record;
  -------------------------------------------------------------
  constant c_spi_model_init : t_spi_model_ctrl := (cmd_rdy     => '0',
                                                   sclk_period => 250 ns,           -- 4 Mbit/s
                                                   spi_op      => wr,
                                                   addr        => (others => 'X'),
                                                   data        => (others => (others => 'X')),
                                                   data_nr     => (others => '0'),  -- number of bytes to write/read
                                                   echo_exp    => (check_echo => true,
                                                                   status     => (others => '0') ,
                                                                   reg_data   => (others => (others => '0'))),
                                                   set_crc_err => false);
  -------------------------------------------------------------
	procedure spi_write(constant sclk_period  : in  time;
                      constant addr         : in  std_ulogic_vector(c_cmd_addr_bits-1 downto 0);
                      constant data         : in  t_spi_tx_data;
                      constant data_nr      : in  std_ulogic_vector(c_cmd_dbn_bits-1 downto 0);  -- number of bytes to write
                      constant echo_exp     : in  t_exp_spi_echo_data;
                      constant set_crc_err  : in  boolean := false;
                      constant batch_mode   : in  boolean := false;
                      signal   spi_tx_ack_n : in  std_ulogic;
                      signal   spi_tx_ctrl  : out t_spi_model_ctrl);
  -------------------------------------------------------------
	procedure spi_read (constant sclk_period  : in  time;
                      constant addr         : in  std_ulogic_vector(c_cmd_addr_bits-1 downto 0);
                      constant data_nr      : in  std_ulogic_vector(c_cmd_dbn_bits-1 downto 0);  -- number of bytes to read
                      constant echo_exp     : in  t_exp_spi_echo_data;
                      constant set_crc_err  : in  boolean := false;
                      constant batch_mode   : in  boolean := false;
                      signal   spi_rx_ack_n : in  std_ulogic;
                      signal   spi_rx_ctrl  : out t_spi_model_ctrl);
  -------------------------------------------------------------
end package tb_cas_pkg;

-------------------------------------------------------------
-------------------------------------------------------------
-------------------------------------------------------------

package body tb_cas_pkg is

  -------------------------------------------------------------
	procedure spi_write(constant sclk_period  : in  time;
                      constant addr         : in  std_ulogic_vector(c_cmd_addr_bits-1 downto 0);
                      constant data         : in  t_spi_tx_data;
                      constant data_nr      : in  std_ulogic_vector(c_cmd_dbn_bits-1 downto 0);  -- number of bytes to write
                      constant echo_exp     : in  t_exp_spi_echo_data;
                      constant set_crc_err  : in  boolean := false;
                      constant batch_mode   : in  boolean := false;
                      signal   spi_tx_ack_n : in  std_ulogic;
                      signal   spi_tx_ctrl  : out t_spi_model_ctrl) is
  begin
    spi_tx_ctrl.sclk_period <= sclk_period;
    spi_tx_ctrl.spi_op      <= wr;
    spi_tx_ctrl.addr        <= addr;
    spi_tx_ctrl.data        <= data;
    spi_tx_ctrl.data_nr     <= data_nr;
    spi_tx_ctrl.echo_exp    <= echo_exp;
    spi_tx_ctrl.set_crc_err <= set_crc_err;

    if not(batch_mode) then 
      if unsigned(data_nr) = 0 then 
        write(output,"###    write reg 0x" & to_hstring(addr) & " by spi...                                      at " & to_string(now) & lf);
      else
        write(output,"###    write reg 0x" & to_hstring(addr) & " to 0x" &to_hstring(unsigned(addr)+(unsigned(data_nr))) &" by spi...                             at " & to_string(now) & lf);    
      end if;
    end if;

    -- requests a transaction from the test initiation to the model
    RequestTransaction(rdy => spi_tx_ctrl.cmd_rdy, ack => spi_tx_ack_n);
  end procedure spi_write;
  -------------------------------------------------------------


  -------------------------------------------------------------
	procedure spi_read (constant sclk_period  : in  time;
                      constant addr         : in  std_ulogic_vector(c_cmd_addr_bits-1 downto 0);
                      constant data_nr      : in  std_ulogic_vector(c_cmd_dbn_bits-1 downto 0);  -- number of bytes to read
                      constant echo_exp     : in  t_exp_spi_echo_data;
                      constant set_crc_err  : in  boolean := false;
                      constant batch_mode   : in  boolean := false;
                      signal   spi_rx_ack_n : in  std_ulogic;
                      signal   spi_rx_ctrl  : out t_spi_model_ctrl) is
  begin
    spi_rx_ctrl.sclk_period <= sclk_period;
    spi_rx_ctrl.spi_op      <= rd;
    spi_rx_ctrl.addr        <= addr;
    spi_rx_ctrl.data_nr     <= data_nr;
    spi_rx_ctrl.echo_exp    <= echo_exp;
    spi_rx_ctrl.set_crc_err <= set_crc_err;

    if not(batch_mode) then 
      if unsigned(data_nr) = 0 then 
        write(output,"###    read reg 0x" & to_hstring(addr) & " by spi...                                       at " & to_string(now) & lf);
      else
        write(output,"###    read reg 0x" & to_hstring(addr) & " to 0x" &to_hstring(unsigned(addr)+(unsigned(data_nr))) &" by spi...                              at " & to_string(now) & lf);    
      end if;
    end if;

    -- requests a transaction from the test initiation to the model
    RequestTransaction(rdy => spi_rx_ctrl.cmd_rdy, ack => spi_rx_ack_n);
  end procedure spi_read;
  -------------------------------------------------------------


end package body tb_cas_pkg;
