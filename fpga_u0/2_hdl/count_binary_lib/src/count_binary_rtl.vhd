-----------------------------------------------------
-- Project : count_binary
-----------------------------------------------------
-- File    : count_binary_rtl.vhd
-- Library : count_binary_lib
-- Author  : michael.pichler@fhnw.ch
-- Company : Institute of Microelectronics (IME) FHNW
-- Copyright(C) IME
-----------------------------------------------------
-- Last commit:
--   $Author:: Michael Rast                         $
--      $Rev::                                      $
--     $Date::             $
-----------------------------------------------------
-- Description :
-----------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity count_binary is
  generic (
    g_prescale_cnt_max : natural := 12_500_000-1
    );
  port (
    clk        : in  std_ulogic;
    reset_n    : in  std_ulogic;
    binary_out : out std_ulogic_vector(3 downto 0)
    );
end entity count_binary;

architecture rtl of count_binary is
  constant c_binary_cnt_max : natural := 15;
  constant c_break_cnt_max  : natural := 19;
  signal   prescale_cnt     : natural range 0 to g_prescale_cnt_max;  -- 125 MHz to 10 Hz
  signal   binary_cnt       : natural range 0 to c_binary_cnt_max;  -- Counting Range
  signal   break_cnt        : natural range 0 to c_break_cnt_max;  -- Counting Break (2 s)
  signal   p_10hz           : std_ulogic;

begin
  -- prescale the clock to approximatly 10 Hz
  p_prescale : process (clk, reset_n)
  begin
    if (reset_n = '0') then
      p_10hz       <= '0';
      prescale_cnt <= 0;
    elsif rising_edge(clk) then
      if prescale_cnt < g_prescale_cnt_max then
        p_10hz       <= '0';
        prescale_cnt <= prescale_cnt + 1;
      else
        p_10hz       <= '1';
        prescale_cnt <= 0;
      end if;
      
    end if;
  end process p_prescale;

  -- count from 0 to c_count_max
  p_count : process (clk, reset_n)
  begin
    if (reset_n = '0') then
      binary_cnt <= 0;
      break_cnt  <= 0;
    elsif rising_edge(clk) then
      if p_10hz = '1' then
        if binary_cnt < c_binary_cnt_max then
          binary_cnt <= binary_cnt + 1;
        else
          if break_cnt < c_break_cnt_max then
            break_cnt <= break_cnt + 1;
          else
            binary_cnt <= 0;
            break_cnt  <= 0;
          end if;
        end if;
      end if;
    end if;
  end process p_count;

  -- display the value of binary_cnt on the red leds
  binary_out <= std_ulogic_vector(to_unsigned(binary_cnt, binary_out'length));
end architecture rtl;

