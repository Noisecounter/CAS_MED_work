library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

    use work.stop_watch_pkg.all;
    
entity stop_watch is

    generic (
            g_sim   : boolean := true
            );

    port    (
            clk         : in std_ulogic;
            reset_n     : in std_ulogic;
            start_stop  : in std_ulogic;
            lap_init    : in std_ulogic;
            s_digit_sel : out std_ulogic;
            s_digit     : out std_ulogic_vector (6 downto 0);
            c_digit_sel : out std_ulogic;
            c_digit     : out std_ulogic_vector (6 downto 0);
            led0        : out std_ulogic
            );
            
end stop_watch;

architecture structural of stop_watch is

signal start_stop_p : std_ulogic;           -- conditioned start_stop
signal lap_init_p   : std_ulogic;           -- conditioned lap_init
signal p1khz        : std_ulogic;           -- 1 kHz from prescaler
signal p100hz       : std_logic;            -- 1 Hz from prescaler
signal run          : std_logic;            -- run from control
signal lap          : std_ulogic;           -- lap from control
signal init         : std_ulogic;           -- init from control
signal c_digit_lo   : std_ulogic_vector (6 downto 0);   -- 7-segment 1/100 s value output
signal c_digit_hi   : std_ulogic_vector (6 downto 0);   -- 7-segment 1/10 s value output
signal s_digit_lo   : std_ulogic_vector (6 downto 0);   -- 7-segment 1 s value output
signal s_digit_hi   : std_ulogic_vector (6 downto 0);    -- 7-segment 10 s value output


begin

U1: entity work.prescaler
    port map (  clk             => clk,
                reset_n         => reset_n,
                p100hz          => p100hz,
                p1khz           => p1khz
--                led0            => led0 
            );
            
U2: entity work.condition
    port map (  clk             => clk,
                reset_n         => reset_n,
                p1khz           => p1khz,
                start_stop      => start_stop,
                lap_init        => lap_init,
                start_stop_p    => start_stop_p,
                lap_init_p      => lap_init_p
            );
            
U3: entity work.control
    port map (  clk             => clk,
                reset_n         => reset_n,
                start_stop_p    => start_stop_p,
                lap_init_p      => lap_init_p,
                run             => run,
                init            => init,
                lap             => lap
--                led0            => led0
            );
            
U4: entity work.count_show_display
    port map (  clk             => clk,
                reset_n         => reset_n,
                run             => run,
                lap             => lap,
                init            => init,
                enable_0        => p100hz,
                c_digit_lo      => c_digit_lo,
                c_digit_hi      => c_digit_hi,
                s_digit_lo      => s_digit_lo,
                s_digit_hi      => s_digit_hi
            );
            
U5: entity work.pmod_ssd
    port map (  clk             => clk,
                reset_n         => reset_n,
                digit_hi        => s_digit_hi,
                digit_lo        => s_digit_lo,
                pmod_digit      => s_digit,
                pmod_sel        => s_digit_sel
            );
            
U6: entity work.pmod_ssd
    port map (  clk             => clk,
                reset_n         => reset_n,
                digit_hi        => c_digit_hi,
                digit_lo        => c_digit_lo,
                pmod_digit      => c_digit,
                pmod_sel        => c_digit_sel
            );                        
                        
end structural;