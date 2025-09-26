library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

entity control is
    port(
        clk             : in  std_ulogic;                       -- 125 MHz clock
        reset_n         : in  std_ulogic;                       -- Synchronous low-active reset
        start_stop_p    : in  std_ulogic;                       -- Conditioned start_stop
        lap_init_p      : in  std_ulogic;                       -- Conditioned lap_init
        run             : out std_ulogic;                       -- 1: Increment counter, 0: Keep current counter value
        lap             : out std_ulogic;                       -- 1: Freeze output, 0: Output counter value
        init            : out std_ulogic;                       -- 1: Reset counter to zero
        led0            : out std_ulogic                        -- LED0 on Zybo board for debugging
        );
end control;


architecture fsm of control is                                  -- fsm "finite state machine"
    type state_type is (stopped_s, run_s, lap_s);               -- Def. new Datatype "state_type" with only 3 possible values "stopped_s, run_s, und lap_s"
    
    signal current_state    : state_type;                       -- Def. new Signal "current_state" which transports the value of "state_type"
--    signal next_state       : state_type;
    
begin

    -----------------------------------------------------
    -- Process sensitiv to clk
    -----------------------------------------------------

    sequential : process (clk)
    begin
        if rising_edge (clk) then
            
            if start_stop_p = '1' then                                  -- If start_stop btn pressed
                case current_state is
                    when stopped_s  => current_state    <= run_s;       -- In stoped, current_state gets run_s
                        if current_state = run_s then
                            run     <= '1';
                            lap     <= '0';
                            init    <= '0';
                        end if;
                            
                    when run_s      => current_state    <= stopped_s;   -- In run, current_state gets stopped_s
                        if current_state = stopped_s then
                            run     <= '0';
                            lap     <= '0';
                            init    <= '0';
                        end if;
                       
                    when lap_s      => current_state    <= stopped_s;   -- In lap, current_state gets stopped_s
                        if current_state = stopped_s then
                            run     <= '0';
                            lap     <= '0';
                            init    <= '0';  
                        end if;
                                                
                end case;
            end if;
            
            if lap_init_p = '1' then                                    -- If lap_init is pressed
                case current_state is
                    when stopped_s  => current_state    <= stopped_s;   -- In stoped, current_state gets stopped_s    
                    when run_s      => current_state    <= lap_s;       -- In run, current_state gets lap_s 
                    when lap_s      => current_state    <= run_s;       -- In lap, current_state gets run_s 
                end case;
            
            
            
            
            
            
            
            else
            
                if current_state = stopped_s then                       -- Output of Statemachine in stopped
                    run     <= '0';
                    lap     <= '0';
                    init    <= '0';
                end if;
                
                if current_state = run_s then                           -- Output of Statemachine in run
                    run     <= '1';
                    lap     <= '0';
                    init    <= '0';
                end if;
                
                if current_state = lap_s then                           --  Output of Statemachine in lap
                    run     <= '1';
                    lap     <= '1';
                    init    <= '0';
                end if;
            end if; 
                if (current_state = stopped_s) and (lap_init_p = '1') then  -- Set init to 1 if in stopped state and lap_init is pressed
                    init <= '1';
            end if;

          if reset_n = '0' then                                        -- Reset handling of reset_n
                current_state   <= stopped_s;
                init            <= '1';
           end if;            
        end if;

    end process sequential;
    
led0 <= init;

end architecture fsm;