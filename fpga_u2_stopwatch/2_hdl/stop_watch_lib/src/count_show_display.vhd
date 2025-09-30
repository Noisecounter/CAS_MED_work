library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
    
    use work.stop_watch_pkg.all;
    
entity count_show_display is
    generic (
            g_high : natural := c_num_digits
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

signal enable_1     : std_ulogic;           -- carry over to the next digit
signal enable_2     : std_ulogic;           -- carry over to the next digit
signal enable_3     : std_ulogic;           -- carry over to the next digit
       

begin

U1: entity work.count1digit
    port map (  clk             => clk,
                reset_n         => reset_n,
                enable          => enable_0,
                lap             => lap,
                run             => run,
                init            => init,
                digit           => c_digit_lo
            );
            
U2: entity work.count1digit
    port map (  clk             => clk,
                reset_n         => reset_n,
                enable          => enable_1,
                lap             => lap,
                run             => run,
                init            => init,
                digit           => c_digit_hi             
            );
            
U3: entity work.count1digit
    port map (  clk             => clk,
                reset_n         => reset_n,
                enable          => enable_2,
                lap             => lap,
                run             => run,
                init            => init,
                digit           => s_digit_lo
            );
            
U4: entity work.count1digit
    port map (  clk             => clk,
                reset_n         => reset_n,
                enable          => enable_3,
                lap             => lap,
                run             => run,
                init            => init,
                digit           => s_digit_hi          
            );
            
end structural;