library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library std;
use std.textio.all;

library work;
use work.tb_cas_pkg.all;

library osvvm;
context osvvm.osvvmcontext;


entity tb_cas_top is
end entity tb_cas_top;

architecture struct of tb_cas_top is

  signal clk            : std_ulogic;
  signal clk_ctrl       : t_clk_ctrl;
  signal rst_n          : std_ulogic;
  signal spi_cmd_ack_n  : std_ulogic;
  signal spi_cs_n       : std_ulogic;
  signal spi_sclk       : std_ulogic;
  signal spi_si         : std_ulogic;
  signal spi_so         : std_ulogic;
  signal spi_model_ctrl : t_spi_model_ctrl;

  component cas_top
  port (
    rst_n    : in  std_ulogic ;
    clk      : in  std_ulogic ;
    spi_cs_n : in  std_ulogic ;
    spi_sclk : in  std_ulogic ;
    spi_si   : in  std_ulogic ;
    spi_so   : out std_ulogic 
  );
  end component cas_top;
  
  component clk_rst
  port (
    clk_ctrl : in  t_clk_ctrl := c_clk_ctrl_init;
    clk      : out std_ulogic := '0';
    rst_n    : out std_ulogic := '0'
  );
  end component clk_rst;
  
  component model_spi
  port (
    clk        : in  std_ulogic ;
    model_ctrl : in  t_spi_model_ctrl := c_spi_model_init;
    rst_n      : in  std_ulogic ;
    spi_si     : in  std_ulogic ;
    cmd_ack_n  : out std_ulogic ;
    spi_cs_n   : out std_ulogic ;
    spi_sclk   : out std_ulogic ;
    spi_so     : out std_ulogic 
  );
  end component model_spi;
  
  component tb_ctrl
  generic (
    g_log_file   : string;
    g_batch_mode : boolean
  );
  port (
    clk            : in  std_ulogic ;
    rst_n          : in  std_ulogic ;
    spi_cmd_ack_n  : in  std_ulogic ;
    clk_ctrl       : out t_clk_ctrl       := c_clk_ctrl_init;
    spi_model_ctrl : out t_spi_model_ctrl := c_spi_model_init
  );
  end component tb_ctrl;

  -- optional embedded configurations
  for all : tb_ctrl use entity work.tb_ctrl(tc_0002);
  
begin

  i0_cas_top : cas_top
    port map (
      clk      => clk,
      spi_cs_n => spi_cs_n,
      spi_sclk => spi_sclk,
      spi_si   => spi_so,
      rst_n    => rst_n,
      spi_so   => spi_si
    );
  i0_clk_rst : clk_rst
    port map (
      clk_ctrl => clk_ctrl,
      clk      => clk,
      rst_n    => rst_n
    );
  i0_model_spi : model_spi
    port map (
      clk        => clk,
      model_ctrl => spi_model_ctrl,
      rst_n      => rst_n,
      spi_si     => spi_si,
      cmd_ack_n  => spi_cmd_ack_n,
      spi_cs_n   => spi_cs_n,
      spi_sclk   => spi_sclk,
      spi_so     => spi_so
    );
  i0_tb_ctrl : tb_ctrl
    generic map (
      g_log_file   => "",
      g_batch_mode => false
    )
    port map (
      clk            => clk,
      rst_n          => rst_n,
      spi_cmd_ack_n  => spi_cmd_ack_n,
      clk_ctrl       => clk_ctrl,
      spi_model_ctrl => spi_model_ctrl
    );

end architecture struct;
