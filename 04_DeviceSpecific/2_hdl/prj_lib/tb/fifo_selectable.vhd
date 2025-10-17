-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

entity fifo_selectable is
    port(
        -- Control Interface
        clk         : in    std_logic;
        rst_n       : in    std_logic;
        -- Write side
        wr_ena      : in    std_logic;
        wr_data     : in    std_logic_vector(31 downto 0);
        wr_full     : out   std_logic;
        -- Read sie
        rd_ack      : in    std_logic;
        rd_data     : out   std_logic_vector(31 downto 0);
        rd_empty    : out   std_logic;
        -- TB interface
        sel         : in    std_logic_vector(2 downto 0); -- [2]:1=large/0=small, [1:0]:"00"=psi_common,"01"=macro,"10"=generator,"11"=primitive
        implemented : out   std_logic
    );
end fifo_selectable;


architecture struct of fifo_selectable is
    
    -- Type to FIFO
    type to_fifo_r is record
        wr_ena      : std_logic;
        wr_data     : std_logic_vector(31 downto 0);
        rd_ack      : std_logic;
    end record;
    type to_fifo_a is array (0 to 7) of to_fifo_r;
    constant to_fifo_init_c : to_fifo_r := ('0', (others => '0'), '0');

    -- Type from FIFO
    type from_fifo_r is record
        wr_full     : std_logic;
        rd_data     : std_logic_vector(31 downto 0);
        rd_empty    : std_logic;
        impl        : std_logic;
    end record;
    type from_fifo_a is array (0 to 7) of from_fifo_r;
    constant from_fifo_init_c : from_fifo_r := ('0', (others => '0'), '0', '0');

    -- Signals
    signal from_fifo_s  : from_fifo_a   := (others => from_fifo_init_c);
    signal to_fifo_s    : to_fifo_a     := (others => to_fifo_init_c);
    signal sel_int    : natural range 0 to 7;

begin

    -- Switch logic
    sel_int         <= to_integer(unsigned(sel));
    implemented     <= from_fifo_s(sel_int).impl;
    rd_empty        <= from_fifo_s(sel_int).rd_empty;
    rd_data         <= from_fifo_s(sel_int).rd_data;
    wr_full         <= from_fifo_s(sel_int).wr_full;
    p_assign : process(sel, wr_ena, wr_data, rd_ack)
    begin
        to_fifo_s <= (others => to_fifo_init_c);
        to_fifo_s(sel_int).wr_ena   <= wr_ena;
        to_fifo_s(sel_int).wr_data  <= wr_data;
        to_fifo_s(sel_int).rd_ack   <= rd_ack;
    end process;
    

    -- Instances
    i_open_logic_small : entity work.fifo_open_logic
        port map (
            clk         => clk,
            rst_n       => rst_n,
            wr_ena      => to_fifo_s(0).wr_ena,
            wr_data     => to_fifo_s(0).wr_data,
            wr_full     => from_fifo_s(0).wr_full,
            rd_ack      => to_fifo_s(0).rd_ack,
            rd_data     => from_fifo_s(0).rd_data,
            rd_empty    => from_fifo_s(0).rd_empty,
            implemented => from_fifo_s(0).impl
        );
    i_macro_small : entity work.fifo_macro
        port map (
            clk         => clk,
            rst_n       => rst_n,
            wr_ena      => to_fifo_s(1).wr_ena,
            wr_data     => to_fifo_s(1).wr_data,
            wr_full     => from_fifo_s(1).wr_full,
            rd_ack      => to_fifo_s(1).rd_ack,
            rd_data     => from_fifo_s(1).rd_data,
            rd_empty    => from_fifo_s(1).rd_empty,
            implemented => from_fifo_s(1).impl
        );
    i_generator_small : entity work.fifo_generator
        port map (
            clk         => clk,
            rst_n       => rst_n,
            wr_ena      => to_fifo_s(2).wr_ena,
            wr_data     => to_fifo_s(2).wr_data,
            wr_full     => from_fifo_s(2).wr_full,
            rd_ack      => to_fifo_s(2).rd_ack,
            rd_data     => from_fifo_s(2).rd_data,
            rd_empty    => from_fifo_s(2).rd_empty,
            implemented => from_fifo_s(2).impl
        );
    i_primitive_small : entity work.fifo_primitive
        port map (
            clk         => clk,
            rst_n       => rst_n,
            wr_ena      => to_fifo_s(3).wr_ena,
            wr_data     => to_fifo_s(3).wr_data,
            wr_full     => from_fifo_s(3).wr_full,
            rd_ack      => to_fifo_s(3).rd_ack,
            rd_data     => from_fifo_s(3).rd_data,
            rd_empty    => from_fifo_s(3).rd_empty,
            implemented => from_fifo_s(3).impl
        );
    i_open_logic_large : entity work.fifo_open_logic_large
        port map (
            clk         => clk,
            rst_n       => rst_n,
            wr_ena      => to_fifo_s(4).wr_ena,
            wr_data     => to_fifo_s(4).wr_data,
            wr_full     => from_fifo_s(4).wr_full,
            rd_ack      => to_fifo_s(4).rd_ack,
            rd_data     => from_fifo_s(4).rd_data,
            rd_empty    => from_fifo_s(4).rd_empty,
            implemented => from_fifo_s(4).impl
        );
    i_macro_large : entity work.fifo_macro_large
        port map (
            clk         => clk,
            rst_n       => rst_n,
            wr_ena      => to_fifo_s(5).wr_ena,
            wr_data     => to_fifo_s(5).wr_data,
            wr_full     => from_fifo_s(5).wr_full,
            rd_ack      => to_fifo_s(5).rd_ack,
            rd_data     => from_fifo_s(5).rd_data,
            rd_empty    => from_fifo_s(5).rd_empty,
            implemented => from_fifo_s(5).impl
        );
    i_generator_large : entity work.fifo_generator_large
        port map (
            clk         => clk,
            rst_n       => rst_n,
            wr_ena      => to_fifo_s(6).wr_ena,
            wr_data     => to_fifo_s(6).wr_data,
            wr_full     => from_fifo_s(6).wr_full,
            rd_ack      => to_fifo_s(6).rd_ack,
            rd_data     => from_fifo_s(6).rd_data,
            rd_empty    => from_fifo_s(6).rd_empty,
            implemented => from_fifo_s(6).impl
        );
    i_primitive_large : entity work.fifo_primitive_large
        port map (
            clk         => clk,
            rst_n       => rst_n,
            wr_ena      => to_fifo_s(7).wr_ena,
            wr_data     => to_fifo_s(7).wr_data,
            wr_full     => from_fifo_s(7).wr_full,
            rd_ack      => to_fifo_s(7).rd_ack,
            rd_data     => from_fifo_s(7).rd_data,
            rd_empty    => from_fifo_s(7).rd_empty,
            implemented => from_fifo_s(7).impl
        );

    


end architecture;

