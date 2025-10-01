-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

use work.stop_watch_pkg.all;

entity pmod_ssd is
    
    generic (
        g_clk_per_digit : positive :=2
            );
    
    port (
        reset_n     : in  std_ulogic;
        clk         : in  std_ulogic;
        digit_lo    : in  std_ulogic_vector(6 downto 0);
        digit_hi    : in  std_ulogic_vector(6 downto 0);
        pmod_digit  : out std_ulogic_vector(6 downto 0);
        pmod_sel    : out std_ulogic
    );
end entity;

--
architecture rtl of pmod_ssd is
    
    signal sel : std_ulogic;
    signal cnt : natural range 0 to g_clk_per_digit-1;
    
begin

    -----------------------------------------------------
    -- Sequential Process: 
    -----------------------------------------------------
    p_seq : process (clk, reset_n)
    begin
        if rising_edge(clk) then
            -- Increment counter
            if cnt = 0 then
                sel <= not sel;
                cnt <= g_clk_per_digit-1;
            else
                cnt <= cnt - 1;
            end if;    
                    
            -- Forward one or the other digit
            pmod_sel <= sel;
            if sel = '1' then
                pmod_digit <= digit_hi;
            else
                pmod_digit <= digit_lo;
            end if;
            
            -- reset
            if reset_n = '0' then
                cnt         <= g_clk_per_digit-1;
                sel         <= '0';
                pmod_digit  <= (others => '0');
            end if;
        end if;
    end process;

end architecture rtl;