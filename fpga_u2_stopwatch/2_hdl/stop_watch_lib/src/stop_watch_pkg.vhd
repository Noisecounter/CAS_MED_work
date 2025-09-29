LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

PACKAGE constants IS
CONSTANT c_max1_syn:        integer := 124999;
CONSTANT c_max2_syn:        integer := 9;

CONSTANT c_max1_sim:        integer := 4;
CONSTANT c_max2_sim:        integer := 4;

CONSTANT c_num_digits:      natural range 0 to 7    := 4;                   -- Anzahl geünschter Digits
TYPE digit_array_t is array (0 to c_num_digits) of natural range 0 to 9;    -- Definition des Array-Type mit Stellen 0 bis c_num_digits

CONSTANT c_max_value_syn:   digit_array_t := (5,9,9,9);                     -- Wertebereich der einzelnen Digits f. Synthese
CONSTANT c_max_value_sim:   digit_array_t := (2,2,2,2);                     -- Wertebereich der einzelnen Digits f. Simulation

END constants;