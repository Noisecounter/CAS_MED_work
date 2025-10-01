LIBRARY ieee;
USE ieee.std_logic_1164.ALL;

PACKAGE stop_watch_pkg IS
    
    CONSTANT c_max1_syn:        natural := 124999;                              -- Teilt 125 MHz in 1 kHz (Prescaler)
    CONSTANT c_max2_syn:        natural := 9;                                   -- Teilt 1 kHz in 100 Hz (Prescaler)

    CONSTANT c_max1_sim:        natural := 4;                                   -- Teiler für Simulation (Prescaler)
    CONSTANT c_max2_sim:        natural := 4;                                   -- Teiler für Simulation (Prescaler)

    CONSTANT c_num_digits:      natural range 0 to 7    := 3;                   -- Anzahl geünschter Digits
    TYPE digit_array_t is array (0 to c_num_digits) of natural range 0 to 9;    -- Definition des Array-Type mit Stellen 0 bis c_num_digits

    CONSTANT c_max_value_syn:   digit_array_t := (5,9,9,9);                     -- Wertebereich der einzelnen Digits f. Synthese
    CONSTANT c_max_value_sim:   digit_array_t := (2,2,2,2);                     -- Wertebereich der einzelnen Digits f. Simulation
    
    function f_select1 (g_sim : boolean; c_max1_sim : natural; c_max1_syn: natural) return natural;
    function f_select2 (g_sim : boolean; c_max_value_sim : digit_array_t; c_max_value_syn : digit_array_t) return digit_array_t;
    
END stop_watch_pkg;

package body stop_watch_pkg is

    function f_select1 (g_sim : boolean; c_max1_sim : natural; c_max1_syn: natural) return natural is
    begin
        if g_sim then
            return c_max1_sim;
        else
            return c_max1_syn;
        end if;
    end function;
    
    function f_select2 (g_sim : boolean; c_max_value_sim : digit_array_t; c_max_value_syn : digit_array_t) return digit_array_t is
    begin
        if g_sim then
            return c_max_value_sim;
        else
            return c_max_value_syn;
        end if;
    end function;
    
end stop_watch_pkg;