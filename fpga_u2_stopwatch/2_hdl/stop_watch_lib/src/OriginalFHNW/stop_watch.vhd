-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.std_logic_arith.all;

entity stop_watch is
    port(
        clk        : in  std_ulogic;
        lap_init   : in  std_ulogic;
        reset_n    : in  std_ulogic; -- reset synchronization is omitted for simplicity reasons
        start_stop : in  std_ulogic;
        sec_digit  : out std_ulogic_vector (6 downto 0)
    );
end stop_watch;


architecture struct of stop_watch is

    -- Internal signal declarations
    signal init         : std_ulogic;
    signal lap          : std_ulogic;
    signal lap_init_p   : std_ulogic;
    signal p1hz         : std_ulogic;
    signal p1khz        : std_ulogic;
    signal run          : std_ulogic;
    signal start_stop_p : std_ulogic;

begin

    -- Instance port mappings.
    i_control : entity work.control
        port map (
            reset_n      => reset_n,
            clk          => clk,
            start_stop_p => start_stop_p,
            lap_init_p   => lap_init_p,
            run          => run,
            lap          => lap,
            init         => init
        );
      
    i_count1digit : entity work.count1digit
        port map (
            reset_n => reset_n,
            clk     => clk,
            run     => run,
            enable  => p1hz,
            lap     => lap,
            init    => init,
            digit   => sec_digit
        );
      
    i_condition : entity work.condition
        port map (
            reset_n      => reset_n,
            clk          => clk,
            p1khz        => p1khz,
            start_stop   => start_stop,
            lap_init     => lap_init,
            start_stop_p => start_stop_p,
            lap_init_p   => lap_init_p
        );
      
    i_prescaler : entity work.prescaler
        port map (
            clk     => clk,
            reset_n => reset_n,
            p1hz    => p1hz,
            p1khz   => p1khz
        );

end struct;
