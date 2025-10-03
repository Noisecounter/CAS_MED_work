-----------------------------------------------------
-- Project : system
-----------------------------------------------------
-- File    : system_top_symbol.vhd
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
library IEEE;
use IEEE.STD_LOGIC_1164.all;

entity system_top is
  generic(
    G_AWIDTH : natural := 10;
    G_DWIDTH : natural := 8
    );
  port(
    rst     : in  std_ulogic := '0';
    clk_50  : in  std_ulogic := '0';
    clk_40  : out std_ulogic;
    clk_80  : out std_ulogic;
    data0   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
    data1   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
    data2   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
    data3   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
    data_en : in  std_ulogic;
    locked  : out std_ulogic;
    raddr   : in  std_ulogic_vector (9 downto 0);
    rdata   : out std_ulogic_vector (4*G_DWIDTH-1 downto 0)
    );
end entity system_top;

architecture struct of system_top is

  -- Architecture declarations

  -- Internal signal declarations
  signal data0_1   : std_ulogic_vector(G_DWIDTH-1 downto 0);
  signal data1_1   : std_ulogic_vector(G_DWIDTH-1 downto 0);
  signal data2_1   : std_ulogic_vector(G_DWIDTH-1 downto 0);
  signal data3_1   : std_ulogic_vector(G_DWIDTH-1 downto 0);
  signal res       : std_ulogic_vector(2*G_DWIDTH-1 downto 0);
  signal data_en_1 : std_ulogic;
  signal res_count : std_ulogic_vector(G_AWIDTH-1 downto 0);
  signal rst_80    : std_ulogic := '0';
  signal rst_40    : std_ulogic;
  signal waddr     : std_ulogic_vector(G_AWIDTH-1 downto 0);
  signal wen       : std_ulogic;
  signal wdata     : std_ulogic_vector(4*G_DWIDTH-1 downto 0);
  signal res_en    : std_ulogic;
  signal op0       : std_ulogic_vector(G_DWIDTH-1 downto 0);
  signal op1       : std_ulogic_vector(G_DWIDTH-1 downto 0);

  -- Implicit buffer signal declarations
  signal clk_40_internal : std_ulogic;
  signal clk_80_internal : std_ulogic;
  signal locked_internal : std_ulogic;


  -- Component Declarations
  component data_calc
    generic (
      G_AWIDTH : natural := 10;
      G_DWIDTH : natural := 8
      );
    port (
      clk_80    : in  std_ulogic;
      data0_1   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data1_1   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data2_1   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data3_1   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data_en_1 : in  std_ulogic;
      rst_80    : in  std_ulogic;
      op0       : out std_ulogic_vector (G_DWIDTH-1 downto 0);
      op1       : out std_ulogic_vector (G_DWIDTH-1 downto 0);
      res       : out std_ulogic_vector (2*G_DWIDTH-1 downto 0);
      res_count : out std_ulogic_vector (G_AWIDTH-1 downto 0);
      res_en    : out std_ulogic
      );
  end component data_calc;
  component data_sync
    generic (
      G_DWIDTH : natural := 8
      );
    port (
      rst_40    : in  std_ulogic;
      clk_40    : in  std_ulogic;
      data0     : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data1     : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data2     : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data3     : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      data_en   : in  std_ulogic;
      data0_1   : out std_ulogic_vector (G_DWIDTH-1 downto 0);
      data1_1   : out std_ulogic_vector (G_DWIDTH-1 downto 0);
      data2_1   : out std_ulogic_vector (G_DWIDTH-1 downto 0);
      data3_1   : out std_ulogic_vector (G_DWIDTH-1 downto 0);
      data_en_1 : out std_ulogic
      );
  end component data_sync;
  component ime_reset
    port (
      clk      : in  std_logic;
      rst_in_n : in  std_logic;
      rst      : out std_logic;
      rst_n    : out std_logic
      );
  end component ime_reset;
  component my_pll
    port (
      refclk   : in  std_logic := '0';
      rst      : in  std_logic := '0';
      locked   : out std_logic;
      outclk_0 : out std_logic;
      outclk_1 : out std_logic
      );
  end component my_pll;
  component ram_dual
    generic (
      G_AWIDTH : natural := 10;
      G_DWIDTH : natural := 8
      );
    port (
      raddr : in  std_ulogic_vector (G_AWIDTH-1 downto 0);
      rclk  : in  std_ulogic;
      waddr : in  std_ulogic_vector (G_AWIDTH-1 downto 0);
      wclk  : in  std_ulogic;
      wdata : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
      wen   : in  std_ulogic;
      rdata : out std_ulogic_vector (G_DWIDTH-1 downto 0)
      );
  end component ram_dual;

begin
  waddr <= res_count;
  wen   <= res_en;
  wdata <= op0 & op1 & res;


  -- Instance port mappings.
  i0_data_calc : data_calc
    generic map (
      G_AWIDTH => G_AWIDTH,
      G_DWIDTH => G_DWIDTH
      )
    port map (
      clk_80    => clk_80_internal,
      data0_1   => data0_1,
      data1_1   => data1_1,
      data2_1   => data2_1,
      data3_1   => data3_1,
      data_en_1 => data_en_1,
      rst_80    => rst_80,
      op0       => op0,
      op1       => op1,
      res       => res,
      res_count => res_count,
      res_en    => res_en
      );
  i0_data_sync : data_sync
    generic map (
      G_DWIDTH => G_DWIDTH
      )
    port map (
      rst_40    => rst_40,
      clk_40    => clk_40_internal,
      data0     => data0,
      data1     => data1,
      data2     => data2,
      data3     => data3,
      data_en   => data_en,
      data0_1   => data0_1,
      data1_1   => data1_1,
      data2_1   => data2_1,
      data3_1   => data3_1,
      data_en_1 => data_en_1
      );
  i0_ime_reset : ime_reset
    port map (
      clk      => clk_40_internal,
      rst_in_n => locked_internal,
      rst_n    => open,
      rst      => rst_40
      );
  i1_ime_reset : ime_reset
    port map (
      clk      => clk_80_internal,
      rst_in_n => locked_internal,
      rst_n    => open,
      rst      => rst_80
      );
  i0_my_pll : my_pll
    port map (
      refclk   => clk_50,
      rst      => rst,
      outclk_0 => clk_40_internal,
      outclk_1 => clk_80_internal,
      locked   => locked_internal
      );
  i0_ram_dual : ram_dual
    generic map (
      G_AWIDTH => G_AWIDTH,
      G_DWIDTH => 4*G_DWIDTH
      )
    port map (
      wclk  => clk_80_internal,
      waddr => waddr,
      wen   => wen,
      wdata => wdata,
      rclk  => clk_40_internal,
      raddr => raddr,
      rdata => rdata
      );

  -- Implicit buffered output assignments
  clk_40 <= clk_40_internal;
  clk_80 <= clk_80_internal;
  locked <= locked_internal;

end architecture struct;
