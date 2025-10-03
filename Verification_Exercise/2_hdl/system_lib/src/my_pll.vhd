LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY my_pll IS
	PORT
	(
		locked   : OUT STD_LOGIC;
		outclk_0 : OUT STD_LOGIC := '0';	-- 40 MHz
		outclk_1 : OUT STD_LOGIC := '0';	-- 80 MHz
		refclk   : IN  STD_LOGIC;
		rst      : IN  STD_LOGIC
	);
END my_pll;

ARCHITECTURE RTL OF my_pll IS
	constant c_cycle : time := 12.5 ns;
	signal clk_1    : std_ulogic;
	signal locked_s : std_ulogic := '0';
	signal count    : natural range 0 to 3;
BEGIN
	p_pll_cmb : process
	begin
		clk_1 <= '0';
		wait for 0 ns;
		wait until rising_edge(refclk);

		l_main : while (rst = '0') loop
			clk_1 <= '1', '0' after c_cycle/2;
			wait for c_cycle;
		end loop l_main;
	end process p_pll_cmb;

	p_locked : process (refclk, rst)
	begin
		if (rst = '1') then
			locked <= '0';
			count  <= 0;
		elsif rising_edge(refclk) then
			if (count < 3) then
				count <= count + 1;
			else
				locked <= '1', '0' after 5*c_cycle;
			end if;
		end if;
	end process p_locked;

	p_sync : process (clk_1)
	begin
		if rising_edge(clk_1) then
			locked_s <= locked;
			if locked_s = '1' then
				outclk_0 <= not outclk_0;
				outclk_1 <= '1', '0' after c_cycle/2;
			else
				outclk_0 <= '0';
				outclk_1 <= '0';
			end if;
		end if;
	end process p_sync;

END RTL; --my_pll
