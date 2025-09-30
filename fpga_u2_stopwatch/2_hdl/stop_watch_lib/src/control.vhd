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
        init            : out std_ulogic                        -- 1: Reset counter to zero
--        led0            : out std_ulogic                        -- LED0 on Zybo board for debugging
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
            
            case current_state is
                when stopped_s  =>  run     <= '0';
                                    init    <= '0';
                                    lap     <= '0';
                                    
                                    if start_stop_p = '1' then
                                        current_state  <= run_s;
                                    elsif lap_init_p = '1' then 
                                        init <= '1';
                                    end if;
                                    
                when run_s      =>  run     <= '1';
                                    init    <= '0';
                                    lap     <= '0';
                                    
                                    if start_stop_p = '1' then
                                        current_state  <= stopped_s;
                                    elsif lap_init_p = '1' then 
                                        current_state <= lap_s;
                                    end if;
                                    
                when lap_s     =>   run     <= '1';
                                    init    <= '0';
                                    lap     <= '1';
                                    
                                    if lap_init_p = '1' then
                                        current_state <= run_s;
                                    elsif start_stop_p = '1' then
                                        current_state <= stopped_s;
                                    end if;
                                when others => null;
                            end case;
                                            
                                if reset_n = '0' then
                                    current_state <= stopped_s;
                                    init <= '1';
                                end if;
                            end if;
                                
  
    end process sequential;
    
-- led0 <= init;

end architecture fsm;