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
            s_digit_sel : out std_ulogic_vector (6 downto 0);
            s_digit     : out std_ulogic_vector (6 downto 0);
            c_digit_sel : out std_ulogic_vector (6 downto 0);
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
                enable_0        => p100hz
            );            
                        
end structural;