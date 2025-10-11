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

entity tb_ctrl is
  generic( 
    g_log_file   : string;
    g_batch_mode : boolean
  );
  port( 
    clk            : in  std_ulogic;
    rst_n          : in  std_ulogic;
    spi_cmd_ack_n  : in  std_ulogic;
    clk_ctrl       : out t_clk_ctrl       := c_clk_ctrl_init;
    spi_model_ctrl : out t_spi_model_ctrl := c_spi_model_init
  );
end entity tb_ctrl ;
