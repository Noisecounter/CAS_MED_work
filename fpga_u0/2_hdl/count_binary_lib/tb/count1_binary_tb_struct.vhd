-----------------------------------------------------
-- Project : count_binary
-----------------------------------------------------
-- File    : count1_binary_tb_symbol.vhd
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
  use ieee.numeric_std.all;
  use ieee.std_logic_1164.all;

library count_binary_lib;

entity count1_binary_tb is
  generic(
    g_prescale_cnt_max : natural := 4
    );
end entity count1_binary_tb;


architecture struct of count1_binary_tb is

  -- Internal signal declarations
  signal binary_out : std_ulogic_vector(3 downto 0);
  signal clk        : std_ulogic;
  signal reset_n    : std_ulogic;


begin

  -- Instance port mappings.
  
  -- Note: The keyword ENTITY here is not used to declare an entity but to
  --       ... use an entity. Here a block of the type count1_binary_verify is used
  --       ... and its ports are connected to signals.
  --       ... The theory for this is maybe yet to be discussed in the course but hey,
  --       ... if we would discuss all theory first, you would not have any labs for the 
  --       ... first four days ;-)
  i_verify : entity count_binary_lib.count1_binary_verify
    port map (
      binary_out => binary_out,
      clk        => clk,
      reset_n    => reset_n
      );
  i_dut : entity count_binary_lib.count_binary
    generic map (
      g_prescale_cnt_max => g_prescale_cnt_max
      )
    port map (
      clk        => clk,
      reset_n    => reset_n,
      binary_out => binary_out
      );

end architecture struct;
