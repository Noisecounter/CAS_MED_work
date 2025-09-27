library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
    
entity count_show_display is
    port (
        clk         : in std_ulogic;
        reset_n     : in std_ulogic;
        enable_0    : in std_ulogic;
        run         : in std_ulogic;
        lap         : in std_ulogic;                       -- 1: Freeze output, 0: Output counter value
        init        : in std_ulogic;
        c_digit_lo  : out std_ulogic_vector (6 downto 0);
        c_digit_hi  : out std_ulogic_vector (6 downto 0);
        s_digit_lo  : out std_ulogic_vector (6 downto 0);
        s_digit_hi  : out std_ulogic_vector (6 downto 0)
        );
end count_show_display;

architecture structural of count_show_display is

signal enable_1     : std_ulogic;           -- übertrag auf nächste Stelle
signal enable_2     : std_ulogic;           
signal enable_3     : std_ulogic;           
       

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