-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

library std;

entity fifo_tb is
end fifo_tb;


architecture sim of fifo_tb is
    -- print procedure
    procedure print (arg : in string := "") is
    begin
          std.textio.write(std.textio.output, arg & LF);
    end procedure;

    -- DUT signals
    signal clk          : std_logic                     := '0';
    signal rst_n        : std_logic                     := '0';
    signal wr_ena       : std_logic                     := '0';
    signal wr_data      : std_logic_vector(31 downto 0) := (others => '0');
    signal wr_full      : std_logic                     := '0';
    signal rd_ack       : std_logic                     := '0';
    signal rd_data      : std_logic_vector(31 downto 0) := (others => '0');
    signal rd_empty     : std_logic                     := '0';
    signal sel          : std_logic_vector(2 downto 0)  := (others => '0');
    signal implemented  : std_logic                     := '0';

    -- TB signals
    signal TbRunning    : boolean                       := true;

    -- Constants
    constant Iterations_c           : natural := 1;
    constant SuppressFullCheck_c    : boolean := true;
    
begin

    p_test : process
        variable size_v : natural;
    begin
        for test_case in 0 to 7 loop
            -- Print
            case test_case is
                when 0 => print("*** open logic (1k) ***");
                when 1 => print("*** macro (1k) ***");
                when 2 => print("*** generator (1k) ***");
                when 3 => print("*** primitive (1k) ***");
                when 4 => print("*** open logic (4k) ***");
                when 5 => print("*** macro (4k) ***");
                when 6 => print("*** generator (4k) ***");
                when 7 => print("*** primitive (4k) ***");
                when others => null;
            end case;
            if test_case < 4 then
                size_v := 1024;
            else
                size_v := 4096;
            end if;
            -- Select instance
            sel <= std_logic_vector(to_unsigned(test_case, sel'length));
            wait for 1 ns;
            -- Only run case if implemented
            if implemented = '1' then
                -- Reset
                wait until rising_edge(clk);
                rst_n <= '0';
                wait for 10 us;
                wait until rising_edge(clk);
                rst_n <= '1';
                wait for 1 us;
                wait until rising_edge(clk);
                -- Fill and read iterations
                for i in 0 to Iterations_c-1 loop
                    print("- Iteration " & integer'image(i));
                    -- Fill (slowly)
                    for word in 0 to size_v-1 loop
                        wait until rising_edge(clk);
                        assert wr_full = '0' report "write: wr_full set before FIFO should be full" severity error;
                        wait until rising_edge(clk);
                        wr_data(31 downto 16)   <= std_logic_vector(to_unsigned(i, 16));
                        wr_data(15 downto 0)    <= std_logic_vector(to_unsigned(word, 16));
                        wr_ena                  <= '1';
                        wait until rising_edge(clk);
                        wr_ena                  <= '0';
                        wr_data                 <= (others => '0');
                        wait until rising_edge(clk);
                        wait until rising_edge(clk);
                        wait until rising_edge(clk);
                        wait until rising_edge(clk);
                        wait until rising_edge(clk);
                        assert rd_empty = '0' report "write: rd_empty set when FIFO not empty" severity error;
                    end loop;
                    assert (wr_full = '1') or SuppressFullCheck_c report "write: wr_full not set after write burst" severity error;
                    -- Read (slowly)
                    for word in 0 to size_v-1 loop
                        wait until rising_edge(clk);
                        assert unsigned(rd_data(31 downto 16)) = i report "read: wrong rd_data(31 downto 16) / iteration" severity error;
                        assert unsigned(rd_data(15 downto 0)) = word report "read: wrong rd_data(15 downto 0) / word" severity error;
                        assert rd_empty = '0' report "read: rd_empty set when FIFO not empty" severity error;
                        rd_ack <= '1';
                        wait until rising_edge(clk);
                        rd_ack <= '0';
                        wait until rising_edge(clk);
                        wait until rising_edge(clk);
                        wait until rising_edge(clk);
                        wait until rising_edge(clk);
                        wait until rising_edge(clk);
                        assert wr_full = '0' report "read: wr_full set when FIFO not full" severity error;
                    end loop;
                    assert rd_empty = '1' report "read: rd_empty not set after read burst" severity error;
                end loop;
            else
                print("SKIPPED! (not implemented)");
            end if;
        end loop;
        TbRunning <= false;
        wait;
    end process;

    p_clk : process
    begin
        while TbRunning loop
            clk <= '0';
            wait for 5 ns;
            clk <= '1';
            wait for 5 ns;
        end loop;
        wait;
    end process;

    i_dut : entity work.fifo_selectable
        port map (
            clk         => clk,
            rst_n       => rst_n,
            wr_ena      => wr_ena,
            wr_data     => wr_data,
            wr_full     => wr_full,
            rd_ack      => rd_ack,
            rd_data     => rd_data,
            rd_empty    => rd_empty,
            sel         => sel,
            implemented => implemented
        );

       


end architecture;

