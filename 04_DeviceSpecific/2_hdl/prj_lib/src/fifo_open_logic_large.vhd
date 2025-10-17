-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;


entity fifo_open_logic_large is
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
        -- Handle "not implemented" case
        implemented : out   std_logic
    );
end fifo_open_logic_large;


architecture rtl of fifo_open_logic_large is
begin
    -- Change to '1' when the FIFO is implemented
    implemented <= '0';
    -- Add Implementation
    
end architecture rtl;
