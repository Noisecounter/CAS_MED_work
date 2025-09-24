-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

entity prescaler is
    port (
        clk     : in std_ulogic;
        reset_n : in std_ulogic;
        p1hz    : out std_ulogic;
        p1khz   : out std_ulogic
--        led0            : out std_ulogic
    );
end prescaler;


architecture rtl of prescaler is
  -- For synthesis
  constant C_MAX1_SYN : natural := 124999; --125 MHz > 1 KHz / 124999
  constant C_MAX2_SYN : natural := 999;   -- 1 KHz > 1 Hz  / 1000
  -- For simulation only
  constant C_MAX1_SIM : natural := 4;
  constant C_MAX2_SIM : natural := 9;

  signal count1       : natural range 0 to C_MAX1_SYN;
  signal count2       : natural range 0 to C_MAX2_SYN;
begin

    prescale_reg : process (clk, reset_n)
        variable v_max1 : natural;
        variable v_max2 : natural;
    begin
        if rising_edge (clk) then
            -- default value
            p1khz <= '0';
            p1hz  <= '0';
            v_max1 := C_MAX1_SYN;
            v_max2 := C_MAX2_SYN;
            -- synthesis translate_off
            v_max1 := C_MAX1_SIM;
            v_max2 := C_MAX2_SIM;
            -- synthesis translate_on
            -- cascaded counter
            if count1 = v_max1 then
                -- set p1khz
                p1khz  <= '1';
                count1 <= 0;
                if count2 = v_max2 then
                    -- set enable
                    p1hz   <= '1';
                    count2 <= 0;
                else
                    count2 <= count2 + 1;
                end if;
            else
                count1 <= count1 + 1;
            end if;
            
            -- reset
            if reset_n = '0' then
                count1 <= 0;
                count2 <= 0;
                p1khz  <= '0';
                p1hz   <= '0';
            end if;
        end if;
    end process;
    
-- led0 <= reset_n;

end architecture rtl;