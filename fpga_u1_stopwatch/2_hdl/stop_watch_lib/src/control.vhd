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
        led0            : out std_ulogic
        );
end control;


architecture fsm of control is                                  -- fsm "finite state machine"
    type state_type is (stopped_s, run_s, lap_s);
    
    signal current_state    : state_type;
    signal next_state       : state_type;
    
begin

    sequential : process (clk)
    begin
        if rising_edge (clk) then
            
           if reset_n = '0' then
                current_state   <= stopped_s;
                init            <= '1';
           end if;
            
            if start_stop_p = '1' then
                case current_state is
                    when stopped_s  => current_state    <= run_s;
                    when run_s      => current_state    <= stopped_s;
                    when lap_s      => current_state    <= stopped_s;
                end case;
            end if;
            
            if lap_init_p = '1' then
                case current_state is
                    when stopped_s  => current_state    <= stopped_s;
                    when run_s      => current_state    <= lap_s;
                    when lap_s      => current_state    <= run_s;
                end case;
            
            else
            
                if current_state = stopped_s then
                    run     <= '0';
                    lap     <= '0';
                    init    <= '0';
                end if;
                
                if current_state = run_s then
                    run     <= '1';
                    lap     <= '0';
                    init    <= '0';
                end if;
                
                if current_state = lap_s then
                    run     <= '1';
                    lap     <= '1';
                    init    <= '0';
                end if;
            end if;
                if (current_state = stopped_s) and (lap_init_p = '1') then
                    init <= '1';
            end if;
        end if;

    end process sequential;
    
led0 <= init;

end architecture fsm;