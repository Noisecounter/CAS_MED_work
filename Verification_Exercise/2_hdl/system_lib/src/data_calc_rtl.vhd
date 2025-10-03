-----------------------------------------------------
-- Project : altera
-----------------------------------------------------
-- File    : data_calc_rtl.vhd
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

entity data_calc is
  generic(
    G_AWIDTH : natural := 10;
    G_DWIDTH : natural := 8
    );
  port(
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

-- Declarations

end data_calc;


architecture rtl of data_calc is
  signal op0_i    : std_ulogic_vector (G_DWIDTH-1 downto 0);
  signal op1_i    : std_ulogic_vector (G_DWIDTH-1 downto 0);
  signal count    : unsigned (G_AWIDTH-1 downto 0);
  signal data_sel : std_ulogic;
begin
  -- Input Mux
  op0_i <= data0_1 when data_sel = '0' else data2_1;
  op1_i <= data1_1 when data_sel = '0' else data3_1;

  p_calc : process(rst_80, clk_80)
  begin
    if rst_80 = '1' then
      res       <= (others => '0');
      count     <= (others => '0');
      res_count <= (others => '0');
      res_en    <= '0';
      op0       <= (others => '0');
      op1       <= (others => '0');
      data_sel  <= '0';
    elsif rising_edge(clk_80) then
      if data_en_1 = '1' then
        op0      <= op0_i;
        op1      <= op1_i;
        res      <= std_ulogic_vector(unsigned(op0_i) * unsigned(op1_i));
        count    <= count + 1;
        res_en   <= '1';
        data_sel <= not data_sel;
      else
        res_en <= '0';
      end if;
      res_count <= std_ulogic_vector(count);
    end if;
  end process p_calc;
  
end architecture rtl;
