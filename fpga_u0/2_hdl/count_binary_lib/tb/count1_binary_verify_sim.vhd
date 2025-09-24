-----------------------------------------------------
-- Project : count_binary
-----------------------------------------------------
-- File    : count1_binary_verify_sim.vhd
-- Library : count_binary_lib
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

entity count1_binary_verify is
  port(
    binary_out : in  std_ulogic_vector (3 downto 0);
    clk        : out std_ulogic;
    reset_n    : out std_ulogic
    );

-- Declarations

end entity count1_binary_verify;

architecture sim of count1_binary_verify is
  constant c_clk_cycle : time := 20 ns;
  signal   sim_end     : boolean := false;
begin

  p_clock : process
  begin
    while (not sim_end) loop
      clk <= '0';
      wait for c_clk_cycle/2;
      clk <= '1';
      wait for c_clk_cycle/2;
    end loop;
    clk <= '0';
    report "Process p_clock stopped";
    wait;
  end process p_clock;

  p_reset : process
  begin
    -- Apply reset
    reset_n <= '0';
    wait for 3*c_clk_cycle;
    -- Remove reset
    reset_n <= '1';
    report "Process p_reset stopped";
    wait;
  end process p_reset;

  p_control : process
  begin
    wait until binary_out = x"F";
    report "Counter reached 0xF first time, restart in 2 seconds";
    wait until binary_out = x"F";
    report "Counter reached 0xF second time, restart in 2 seconds";
    wait until binary_out = x"3";
    report "Counter restarted second time, stopp simulation";
    sim_end <= true;
    report "Process p_control stopped";
    wait;
  end process p_control;

end architecture sim;

