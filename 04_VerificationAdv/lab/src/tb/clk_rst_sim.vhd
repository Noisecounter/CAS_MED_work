library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library work;
use work.tb_cas_pkg.all;

entity clk_rst is
  port( 
    clk_ctrl : in  t_clk_ctrl := c_clk_ctrl_init;
    clk      : out std_ulogic := '0';
    rst_n    : out std_ulogic := '0'
  );
end entity clk_rst ;


architecture sim of clk_rst is
begin

  rst_n <= transport '0', '1' after 10*c_clk_period;
  
  ----------------------------
  -- clock clk_osc
  ----------------------------
  p_clk: process
    begin
    clk <= '0';
    wait for c_clk_period;
    while clk_ctrl.clk_enable loop
      wait for clk_ctrl.clk_rise_time;
      clk <= '1';
      wait for clk_ctrl.clk_high_time;
      clk <= '0';
      wait for clk_ctrl.clk_period    -
               clk_ctrl.clk_rise_time -
               clk_ctrl.clk_high_time;
    end loop;
    wait; -- for ever
  end process p_clk;

  
end architecture sim;
