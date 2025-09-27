-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity control is
    port(
        reset_n      : in  std_ulogic;
        clk          : in  std_ulogic;
        start_stop_p : in  std_ulogic;
        lap_init_p   : in  std_ulogic;
        run          : out std_ulogic;
        lap          : out std_ulogic;
        init         : out std_ulogic
    );
end control;


architecture fsm of control is

  type STATE_TYPE is (
    s_stop,
    s_run,
    s_init,
    s_lap
    );

  -- Declare current and next state signals
  signal state : STATE_TYPE;

begin

    fsm : process(clk, reset_n)
    begin
        if rising_edge(clk) then
            case state is
                ------------------------------------
                when s_stop =>
                    -- Outputs
                    run  <= '0';
                    init <= '0';
                    lap  <= '0';
                    -- Transitions
                    if start_stop_p = '1' then
                        state <= s_run;
                    elsif lap_init_p = '1' then
                        state <= s_init;
                    end if;       
                ------------------------------------
                when s_run =>
                    -- Outputs
                    run  <= '1';
                    init <= '0';
                    lap  <= '0';
                    -- Transitions
                    if start_stop_p = '1' then
                        state <= s_stop;
                    elsif lap_init_p = '1' then
                        state <= s_lap;
                    end if;       
                ------------------------------------
                when s_init =>
                    -- Outputs
                    run  <= '0';
                    init <= '1';
                    lap  <= '0';
                    -- Transitions
                    state <= s_stop;   
                ------------------------------------
                when s_lap =>
                    -- Outputs
                    run  <= '1';
                    init <= '0';
                    lap  <= '1';
                    -- Transitions
                    if lap_init_p = '1' then
                        state <= s_run;
                    elsif start_stop_p = '1' then
                        state <= s_stop;
                    end if;    
                ------------------------------------
                when others => null;    
            end case;
            if reset_n = '0' then
                state <= s_stop;
            end if;
        end if;
    end process;
  
end fsm;
