-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

entity condition is
    port(
        reset_n      : in  std_ulogic;
        clk          : in  std_ulogic;
        p1khz        : in  std_ulogic;
        start_stop   : in  std_ulogic;
        lap_init     : in  std_ulogic;
        start_stop_p : out std_ulogic;
        lap_init_p   : out std_ulogic
    );
end condition;


architecture rtl of condition is
    constant C_DEB_LEN        : natural := 4;
    signal start_stop_sync1   : std_ulogic;
    signal start_stop_sync2   : std_ulogic;
    signal start_stop_d1      : std_ulogic;
    signal start_stop_d2      : std_ulogic;
    signal lap_init_sync1     : std_ulogic;
    signal lap_init_sync2     : std_ulogic;
    signal lap_init_d1        : std_ulogic;
    signal lap_init_d2        : std_ulogic;
begin

    -----------------------------------------------------
    -- Synchronize and debounce start_stop
    -- 1. synchronize with double stage synchronizer
    -- 2. shift in new value after 1 ms
    -- 3. debounce: rising edge immediately, falling edge debounced
    -- 4. delayed debounced button by one clock cycle
    -- 5. puls for positive edge of debounced button
    -----------------------------------------------------
    p_start_stop : process (clk, reset_n)    
        variable shift_reg : std_ulogic_vector(C_DEB_LEN-1 downto 0);    
    begin
        if rising_edge (clk) then
            -- 1
            start_stop_sync1 <= start_stop;
            start_stop_sync2 <= start_stop_sync1;
            -- 2
            if p1khz = '1' then
                shift_reg := shift_reg(shift_reg'high-1 downto 0) & start_stop_sync2;
            end if;
            -- 3
            if start_stop_sync2 = '1' then
                start_stop_d1 <= '1';           -- button pressed
            elsif shift_reg = std_ulogic_vector(to_unsigned(0, C_DEB_LEN)) then
                start_stop_d1 <= '0';           -- button released
            end if;
            -- 4
            start_stop_d2 <= start_stop_d1;

            -- reset
            if reset_n = '0' then
                start_stop_sync1 <= '0';
                start_stop_sync2 <= '0';
                start_stop_d1    <= '0';
                start_stop_d2    <= '0';
                shift_reg     := (others => '0');
            end if;
        end if;
    end process p_start_stop;

      -- 5
      start_stop_p <= start_stop_d1 and not start_stop_d2;

    -----------------------------------------------------
    -- Debounce lap_init
    -----------------------------------------------------
    p_lap_init : process (clk, reset_n)
        variable shift_reg : std_ulogic_vector(C_DEB_LEN-1 downto 0);
    begin
        if rising_edge (clk) then
            -- 1
            lap_init_sync1 <= lap_init;
            lap_init_sync2 <= lap_init_sync1;
            -- 1
            if p1khz = '1' then
                shift_reg := shift_reg(shift_reg'high-1 downto 0) & lap_init_sync2;
            end if;
            -- 2
            if lap_init_sync2 = '1' then
                lap_init_d1 <= '1';             -- button pressed
            elsif shift_reg = std_ulogic_vector(to_unsigned(0, C_DEB_LEN)) then
                lap_init_d1 <= '0';             -- button released
            end if;
            -- 3
            lap_init_d2 <= lap_init_d1;
            if reset_n = '0' then
                lap_init_d1 <= '0';
                lap_init_d2 <= '0';
                lap_init_sync1 <= '0';
                lap_init_sync2 <= '0';
                shift_reg   := (others => '0');
            end if;
        end if;
    end process p_lap_init;

  -- 4
  lap_init_p <= lap_init_d1 and not lap_init_d2;


end architecture rtl;

