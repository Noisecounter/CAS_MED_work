LIBRARY ieee;
    USE ieee.std_logic_1164.ALL;
    USE ieee.numeric_std.ALL;

entity count1digit is
    generic(
        g_high : integer := 9
    );


    port(
        clk             : in  std_ulogic;                       -- 125 MHz clock
        reset_n         : in  std_ulogic;                       -- Synchronous low-active reset
        enable          : in  std_ulogic;                       -- Clock enabl, p1hz from prescaler, act. f. 1 clock cycle every second.
        run             : in  std_ulogic;                       -- 1: Increment counter, 0: Keep current counter value
        lap             : in  std_ulogic;                       -- 1: Freeze output, 0: Output counter value
        init            : in  std_ulogic;                       -- 1: Reset counter to zero
        digit           : out std_ulogic_vector (6 downto 0);   -- 7-segment control value output
        ena_out         : out std_ulogic                        -- übertrag auf nächste Stelle
--        led0            : out std_ulogic
        );
end count1digit;


architecture rtl of count1digit is

    signal lapdisplay   : integer range 0 to 9;                 -- Holds lap time
    signal counter      : integer range 0 to 9;                 -- Counts the enable pulses (p1hz)
    signal reg_value    : natural range 0 to g_high;
    signal cnt_value    : natural range 0 to g_high;

    CONSTANT C_0: std_ulogic_vector (6 downto 0) := "0111111";  -- Display 0 "gfedcba"
    CONSTANT C_1: std_ulogic_vector (6 downto 0) := "0000110";  -- Display 1 "gfedcba"
    CONSTANT C_2: std_ulogic_vector (6 downto 0) := "1011011";  -- Display 2 "gfedcba"
    CONSTANT C_3: std_ulogic_vector (6 downto 0) := "1001111";  -- Display 3 "gfedcba"
    CONSTANT C_4: std_ulogic_vector (6 downto 0) := "1100110";  -- Display 4 "gfedcba"
    CONSTANT C_5: std_ulogic_vector (6 downto 0) := "1101101";  -- Display 5 "gfedcba"
    CONSTANT C_6: std_ulogic_vector (6 downto 0) := "1111101";  -- Display 6 "gfedcba"
    CONSTANT C_7: std_ulogic_vector (6 downto 0) := "0000111";  -- Display 7 "gfedcba"
    CONSTANT C_8: std_ulogic_vector (6 downto 0) := "1111111";  -- Display 8 "gfedcba"
    CONSTANT C_9: std_ulogic_vector (6 downto 0) := "1101111";  -- Display 9 "gfedcba"

begin

    -----------------------------------------------------
    -- Process sensitiv to clk
    -- Counts the enable pulses from prescaler if run = 1
    -- If counter is equal to 9, then go back to 0
    -- If init = 1, then go back to 0
    -----------------------------------------------------
    p_CountSeconds : process (clk)
    begin
    
        if rising_edge (clk) then
            if run = '1' and enable ='1' then
                    ena_out <= '0';
                if  counter = g_high then
                    counter <= 0;
                    ena_out <= '1';
                else
                    counter <= counter + 1;
                end if;
            end if;
            
            if init = '1' then
          --      digit       <= "000000";
          --      lapdisplay  <= 0;
                counter     <= 0;
            end if;
        end if;
        
    end process p_CountSeconds; 
    
    -----------------------------------------------------
    -- Process sensitiv to clk
    -- If lap is 0, then counter is displayed   
    -----------------------------------------------------
    p_LapDisplay : process (clk)
    begin
                       
        if rising_edge (clk) then
            if lap = '0' then
                lapdisplay  <= counter;
            end if;
         --     else lapdisplay <= lapdisplay; -- unnötig,
         --     VHDL erzeugt Flip-Flop das den Wert so lange speichert, bis er überschrieben wird.
        end if;
        
    end process p_LapDisplay;

-- 7 segment mapping
with lapdisplay select
        digit <=    c_0 when 0,
                    c_1 when 1,
                    c_2 when 2,
                    c_3 when 3,
                    c_4 when 4,
                    c_5 when 5,
                    c_6 when 6,
                    c_7 when 7,
                    c_8 when 8,
                    c_9 when 9,
                    c_0 when others;
    
end architecture rtl;