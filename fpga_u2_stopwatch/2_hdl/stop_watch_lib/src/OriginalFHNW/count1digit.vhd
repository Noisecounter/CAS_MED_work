-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

entity count1digit is
    port (
        reset_n : in  std_ulogic;
        clk     : in  std_ulogic;
        run     : in  std_ulogic;
        enable  : in  std_ulogic;
        lap     : in  std_ulogic;
        init    : in  std_ulogic;
        digit   : out std_ulogic_vector(6 downto 0)
    );
end entity count1digit;

--
architecture rtl of count1digit is
    -- LED Assignment: 6:0                            gfedcba
    constant C_0 : std_ulogic_vector (6 downto 0) := "0111111";
    constant C_1 : std_ulogic_vector (6 downto 0) := "0000110";
    constant C_2 : std_ulogic_vector (6 downto 0) := "1011011";
    constant C_3 : std_ulogic_vector (6 downto 0) := "1001111";
    constant C_4 : std_ulogic_vector (6 downto 0) := "1100110";
    constant C_5 : std_ulogic_vector (6 downto 0) := "1101101";
    constant C_6 : std_ulogic_vector (6 downto 0) := "1111101";
    constant C_7 : std_ulogic_vector (6 downto 0) := "0000111";
    constant C_8 : std_ulogic_vector (6 downto 0) := "1111111";
    constant C_9 : std_ulogic_vector (6 downto 0) := "1101111";
    constant C_R : std_ulogic_vector (6 downto 0) := "0001000";
    
    signal reg_value : natural range 0 to 9;
    signal cnt_value : natural range 0 to 9;
begin

    -----------------------------------------------------
    -- Sequential Process: 
    -----------------------------------------------------
    p_count_reg : process (clk, reset_n)
    begin
        if rising_edge(clk) then
            -- Initialization
            if init = '1' then                   
                cnt_value <= 0;
                reg_value <= 0;
                
            -- counting
            elsif run = '1' and enable = '1' then  
                if cnt_value = 9 then
                    cnt_value <= 0;
                else
                    cnt_value <= cnt_value + 1;
                end if;
            end if;

            -- Lap handling
            if lap = '0' then
                reg_value <= cnt_value;
            end if;       
            
            -- reset
            if reset_n = '0' then
                cnt_value <= 0;                        
                reg_value <= 0;
            end if;
        end if;
    end process p_count_reg;
    
    -----------------------------------------------------
    -- Combinational PROCESS:
    -----------------------------------------------------
    p_sevenseg_comb : process (reg_value)
    begin
        case reg_value is
            when 0      => digit <= C_0;
            when 1      => digit <= C_1;
            when 2      => digit <= C_2;
            when 3      => digit <= C_3;
            when 4      => digit <= C_4;
            when 5      => digit <= C_5;
            when 6      => digit <= C_6;
            when 7      => digit <= C_7;
            when 8      => digit <= C_8;
            when 9      => digit <= C_9;
            when others => digit <= C_R;
        end case;
    end process p_sevenseg_comb;

end architecture rtl;
