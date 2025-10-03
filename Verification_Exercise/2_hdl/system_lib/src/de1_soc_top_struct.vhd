-----------------------------------------------------
-- Project : system
-----------------------------------------------------
-- File    : de1_soc_top_symbol.vhd
-- Library : system_lib
-- Author  : michael.pichler@fhnw.ch
-- Company : Institute of Microelectronics (IME) FHNW
-- Copyright(C) IME
-----------------------------------------------------
-- Last commit:
--   $Author::                                      $
--      $Rev::                                      $
--     $Date::             $
-----------------------------------------------------
-- Description :
-----------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity de1_soc_top is
  port(
    clk_50  : in  std_logic := '0';
    data0   : in  std_ulogic_vector (7 downto 0);
    data1   : in  std_ulogic_vector (7 downto 0);
    data2   : in  std_ulogic_vector (7 downto 0);
    data3   : in  std_ulogic_vector (7 downto 0);
    data_en : in  std_ulogic;
    raddr   : in  std_ulogic_vector (9 downto 0);
    rst     : in  std_logic := '0';
    clk_40  : out std_logic;
    clk_80  : out std_logic;
    locked  : out std_logic;
    rdata   : out std_ulogic_vector (31 downto 0)
    );

-- Declarations

end de1_soc_top;
-----------------------------------------------------
-- Project : system
-----------------------------------------------------
-- File    : de1_soc_top_struct.vhd
-- Library : system_lib
-- Author  : michael.pichler@fhnw.ch
-- Company : Institute of Microelectronics (IME) FHNW
-- Copyright(C) IME
-----------------------------------------------------
-- Last commit:
--   $Author::                                      $
--      $Rev::                                      $
--     $Date::             $
-----------------------------------------------------
-- Description :
-----------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library system_lib;

architecture struct of de1_soc_top is

  -- Architecture declarations

  -- Internal signal declarations


  -- Component Declarations
  component system_top
    generic (
      G_AWIDTH : natural := 10;
      G_DWIDTH : natural := 8
      );
    port (
      data0   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data1   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data2   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data3   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data_en : in  std_ulogic;
      clk_40  : out std_ulogic;
      clk_80  : out std_ulogic;
      locked  : out std_ulogic;
      clk_50  : in  std_ulogic := '0';
      rst     : in  std_ulogic := '0';
      rdata   : out std_ulogic_vector (4*G_DWIDTH-1 downto 0);
      raddr   : in  std_ulogic_vector (9 downto 0)
      );
  end component;

  -- Optional embedded configurations
  -- pragma synthesis_off
  for all : system_top use entity system_lib.system_top;
  -- pragma synthesis_on


begin

  -- Instance port mappings.
  i0_system_top : system_top
    generic map (
      G_AWIDTH => 10,
      G_DWIDTH => 8
      )
    port map (
      data0   => data0,
      data1   => data1,
      data2   => data2,
      data3   => data3,
      data_en => data_en,
      clk_40  => clk_40,
      clk_80  => clk_80,
      locked  => locked,
      clk_50  => clk_50,
      rst     => rst,
      rdata   => rdata,
      raddr   => raddr
      );

end struct;
