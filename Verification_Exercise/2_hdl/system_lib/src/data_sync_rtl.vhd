-----------------------------------------------------
-- Project : altera
-----------------------------------------------------
-- File    : data_mux_rtl.vhd
-- Library : altera_lib
-- Author  : michael.pichler@fhnw.ch
-- Company : Institute of Microelectronics (IME) FHNW
-- Copyright(C) IME
-----------------------------------------------------
-- Description :
-----------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity data_sync is
  generic(
    G_DWIDTH : natural := 8
    );
  port(
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

-- Declarations

end data_sync;


architecture rtl of data_sync is
begin

  p_sync : process(rst_40, clk_40)
  begin
    if rst_40 = '1' then
      data0_1   <= (others => '0');
      data1_1   <= (others => '0');
      data2_1   <= (others => '0');
      data3_1   <= (others => '0');
      data_en_1 <= '0';
    elsif rising_edge(clk_40) then
      -- input synchronization
      data0_1   <= data0;
      data1_1   <= data1;
      data2_1   <= data2;
      data3_1   <= data3;
      data_en_1 <= data_en;
    end if;
  end process p_sync;
  
end architecture rtl;
