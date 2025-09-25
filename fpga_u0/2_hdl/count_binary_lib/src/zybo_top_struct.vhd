-----------------------------------------------------
-- Project : count_binary
-----------------------------------------------------
-- File    : zybo_top_symbol.vhd
-- Library : count_binary_lib
-- Author  : michael.pichler@fhnw.ch
-- Company : Institute of Microelectronics (IME) FHNW
-- Copyright(C) IME
-----------------------------------------------------
-- Last commit: Michael Rast
--   $Author::                                      $
--      $Rev::                                      $
--     $Date::             $
-----------------------------------------------------
-- Description :
-----------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

LIBRARY count_binary_lib;

ENTITY zybo_top IS
  PORT( 
    btn_0   : IN     std_ulogic;
    sysclk  : IN     std_ulogic;
    led     : OUT    std_ulogic_vector (3 DOWNTO 0)
  );

END ENTITY zybo_top ;
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ARCHITECTURE struct OF zybo_top IS

  -- Internal signal declarations
  SIGNAL binary_out : std_ulogic_vector(3 DOWNTO 0);
  SIGNAL clk        : std_ulogic;
  SIGNAL reset_n    : std_ulogic;

BEGIN
  -- Architecture concurrent statements
  -- HDL Embedded Text Block 2 IOs1
  -- IOs 1
  clk     <= sysclk;
  reset_n <= not btn_0;
  led      <= binary_out;


  -- Instance port mappings.
  i_inst : ENTITY count_binary_lib.count_binary
    GENERIC MAP (
      g_prescale_cnt_max => 12_500_000-1 -- zybo clock input is 125 MHz
    )
    PORT MAP (
      clk        => clk,
      reset_n    => reset_n,
      binary_out => binary_out
    );

END ARCHITECTURE struct;
