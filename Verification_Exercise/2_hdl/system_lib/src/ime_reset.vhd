--------------------------------------------------------------------------------
-- Project : IME-IP
--------------------------------------------------------------------------------
-- File    : ime_reset.vhd
-- Library : ime_lib
-- Author  : michael.pichler@fhnw.ch
-- Company : Institute of Microelectronics (IME) FHNW
--------------------------------------------------------------------------------
-- Description : Global Reset generation
--------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity ime_reset is
  port (
    clk      : in  std_logic;
    rst_in_n : in  std_logic;
    rst_n    : out std_logic;
    rst      : out std_logic
    );
end ime_reset;

architecture rtl of ime_reset is
  signal shift_reg : std_logic_vector(1 downto 0) := "00";
begin

  -- Reset Generation
  --   assert ssynchronously
  --   deassert synchronously
  p_reset : process (rst_in_n, clk)
  begin
    if rst_in_n = '0' then
      shift_reg <= "00";
    elsif rising_edge(clk) then
      shift_reg <= shift_reg(0) & '1';
    end if;
  end process p_reset;

  rst_n <= shift_reg(1);
  rst   <= not shift_reg(1);

end rtl;
