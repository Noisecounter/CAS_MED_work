library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
 
entity count1digit is
    port(
        clk          : in  std_ulogic;
        reset_n        : in  std_ulogic;
        enable   : in  std_ulogic;
        init     : in  std_ulogic;
        run : in std_ulogic;
        lap   : in std_ulogic;
        digit : out std_ulogic_vector (6 downto 0)
        );
end count1digit;
 
architecture rtl of count1digit is
     CONSTANT C_0: std_ulogic_vector (6 downto 0) := "0111111";
     CONSTANT C_1: std_ulogic_vector (6 downto 0) := "0000110";
     CONSTANT C_2: std_ulogic_vector (6 downto 0) := "1011011";
     CONSTANT C_3: std_ulogic_vector (6 downto 0) := "1001111";
     CONSTANT C_4: std_ulogic_vector (6 downto 0) := "1100110";
     CONSTANT C_5: std_ulogic_vector (6 downto 0) := "1101101";
     CONSTANT C_6: std_ulogic_vector (6 downto 0) := "1111101";
     CONSTANT C_7: std_ulogic_vector (6 downto 0) := "0000111";
     CONSTANT C_8: std_ulogic_vector (6 downto 0) := "1111111";
     CONSTANT C_9: std_ulogic_vector (6 downto 0) := "1101111";    
     signal counter: integer range 0 to 9;
     signal hold: integer range 0 to 9;
begin
 
    -- Counter
    p_count : process (clk)        
    begin
        if rising_edge (clk) then
            if enable = '1' and run = '1' then            
                if counter = 9 then
                    counter <= 0;
                else
                    counter <= counter + 1;
                end if;                     
            end if;
            -- reset
            if reset_n = '0' or init = '1' then
                counter <= 0;
            end if;
        end if;
    end process;
    -- Hold
    p_hold : process (clk)        
    begin
        if rising_edge (clk) then
            if lap = '0' then
                hold <= counter; 
            end if;
            -- reset
            if reset_n = '0' or init = '1' then
            end if;
        end if;
    end process;
    -- 7 segment mapping
    with hold select
        digit <= c_0 when 0,
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



 
end;
