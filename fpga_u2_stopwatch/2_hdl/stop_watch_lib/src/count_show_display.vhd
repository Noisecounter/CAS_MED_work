library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
    
    use work.stop_watch_pkg.all;
    
entity count_show_display is

    generic (
--            g_high  : integer := 9;
            g_sim   : boolean := false                      -- Schalter simulation / synthese 
            );

    port (
        clk         : in std_ulogic;                        -- 125 MHz clock
        reset_n     : in std_ulogic;                        -- Synchronous low-active reset
        enable_0    : in std_ulogic;                        -- Active for 1 clk cycle (ferquencie from prescaler)
        run         : in std_ulogic;                        -- 1: Increment counter, 0: Keep current counter value
        lap         : in std_ulogic;                        -- 1: Freeze output, 0: Output counter value
        init        : in std_ulogic;                        -- 1: Reset counter to zero
        c_digit_lo  : out std_ulogic_vector (6 downto 0);   -- 7-segment 1/100 s value output
        c_digit_hi  : out std_ulogic_vector (6 downto 0);   -- 7-segment 1/10 s value output
        s_digit_lo  : out std_ulogic_vector (6 downto 0);   -- 7-segment 1 s value output
        s_digit_hi  : out std_ulogic_vector (6 downto 0)    -- 7-segment 10 s value output
        );
end count_show_display;

architecture structural of count_show_display is

signal carry_out    : std_ulogic;           -- carry_out from count1digit
signal enable_1     : std_ulogic;           -- carry over to the next digit
signal enable_2     : std_ulogic;           -- carry over to the next digit
signal enable_3     : std_ulogic;           -- carry over to the next digit

-- Funktion aus stop_watch_pkg
-- g_sim true/false entscheidet welches array an c_max_values uebergeben wird.
constant c_max_values : digit_array_t := f_select2(g_sim, c_max_value_sim, c_max_value_syn); 

begin

U1: entity work.count1digit

    generic map ( g_high => c_max_values(3) -- uebergibt wert an der 4. position des Arrays an g_high (man zählt die Stellen von 0 bis 3)
                                            -- Für Synthese wäre das die rechte Stelle von 5,9,9,(9)
                )
                
    port map (  clk             => clk,
                reset_n         => reset_n,
                enable          => enable_0,    -- "enable0" wird an count1digit auf den input "enable" verknüpft
                lap             => lap,
                run             => run,
                init            => init,
                digit           => c_digit_lo,  -- Output "digit" wird an "c_digit_lo" verknüpft
                carry_out       => enable_1     -- Output "carry_out" wird an "enable_1" verknüpft.
            );
            
U2: entity work.count1digit

    generic map ( g_high => c_max_values(2) -- uebergibt wert an der 3. position des Arrays an g_high (man zählt die Stellen von 0 bis 3)
                                            -- Für Synthese wäre das die zweit letzte Stelle von 5,9,(9),9
                )
                
    port map (  clk             => clk,
                reset_n         => reset_n,
                enable          => enable_1,
                lap             => lap,
                run             => run,
                init            => init,
                digit           => c_digit_hi,
                carry_out       => enable_2             
            );
            
U3: entity work.count1digit

    generic map ( g_high => c_max_values(1) -- uebergibt wert an der 2. position des Arrays an g_high (man zählt die Stellen von 0 bis 3)
                                            -- Für Synthese wäre das 5,(9),9,9
                )
                
    port map (  clk             => clk,
                reset_n         => reset_n,
                enable          => enable_2,
                lap             => lap,
                run             => run,
                init            => init,
                digit           => s_digit_lo,
                carry_out       => enable_3
            );
            
U4: entity work.count1digit

    generic map ( g_high => c_max_values(0) -- uebergibt wert an der 1. position des Arrays an g_high (man zählt die Stellen von 0 bis 3)
                                            -- Für Synthese wäre das (5),9,9,9
                                            -- g_high bekommt 5 und somit zählt die forderste Stelle nur bis 5
                )
                
    port map (  clk             => clk,
                reset_n         => reset_n,
                enable          => enable_3,
                lap             => lap,
                run             => run,
                init            => init,
                digit           => s_digit_hi          
            );
            
end structural;