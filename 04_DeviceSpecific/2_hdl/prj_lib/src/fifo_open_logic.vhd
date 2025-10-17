-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

library olo;
    use olo.all;

entity fifo_open_logic is



    port(
        -- Control Interface
        clk         : in    std_logic;
        rst_n       : in    std_logic;
        -- Write side
        wr_ena      : in    std_logic;
        wr_data     : in    std_logic_vector(31 downto 0);
        wr_full     : out   std_logic;
        -- Read side
        rd_ack      : in    std_logic;
        rd_data     : out   std_logic_vector(31 downto 0);
        rd_empty    : out   std_logic;
        -- Handle "not implemented" case
        implemented : out   std_logic
    );
end fifo_open_logic;


architecture rtl of fifo_open_logic is

        signal  ResetInv  : STD_LOGIC   := '1';

begin
    -- Change to '1' when the FIFO is implemented
    implemented <= '1';
    -- Add Implementation

    i_fifo : entity olo.olo_base_fifo_sync 

        generic map (
                    Width_g => 32,
                    Depth_g => 1024
                    )

        port map    (
                    Clk         => clk,
                    Rst         => ResetInv,
                    In_Valid    => wr_ena,
                    In_Data     => wr_data,
                    Out_Ready   => rd_ack,
                    Out_Data    => rd_data,
                    Empty       => rd_empty,
                    Full        => wr_full
                    );

        ResetInv <= not rst_n;
    
end architecture rtl;

