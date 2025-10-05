-----------------------------------------------------
-- Project : system
-----------------------------------------------------
-- File    : system_top_pkg.vhd
-- Library : system_tb_lib
-- Author  : michael.pichler@fhnw.ch
-- Company : FHNW - ISE
-- Copyright(C) ISE
-----------------------------------------------------
-- Description :
-----------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use std.textio.all;
use ieee.std_logic_textio.all;

package system_top_pkg is
    -- -------------------------------------------------------
    -- Global Constants & Types
    -- -------------------------------------------------------
    constant C_DWIDTH   : integer := 8;
    constant C_AWIDTH   : natural := 10;
    
    constant c_stim     : string := "./f_stim.txt"; -- Content with input stimuli
    constant c_exp      : string := "./f_exp.txt";  -- Content with expected values
    constant c_dump     : string := "./f_dump.txt"; -- Dumpfile with exp. und actual values
    
    constant c_delay    : time := 1 ns;     -- Propagation Delay
    
    constant c_cycle    : time := 20 ns;    -- 50 MHz
    constant c_cycle_40 : time := 25 ns:    -- 40 MHz
    constant c_cycle_50 : time := 12.5 ns;  -- 80 MHz
    
--    constant c_start_stim   : time := 20*c_cycle;
--    constant c_start_read   : time := 40*c_cycle;
--    constant c_end_sim      : time := 60*c_cycle;    
  
-- Record für Speicher write port
    type t_in_port is record
        count   : naturl;
        data_en : std_ulogic;
        data0   : std_ulogic_vector (C_DWITH-1 downto 0);
        data1   : std_ulogic_vector (C_DWITH-1 downto 0);
        data2   : std_ulogic_vector (C_DWITH-1 downto 0);
    end record;
    
-- Record für Speicher lese port
    type t_rd_port is record
        clk     : std_logic;
        raddr   : std_ulogic_vector (9 downto 0);
        rdata   : std_ulogic_vector (31 downto 0);
    end record;
    
    -- -------------------------------------------------------
    -- Procedure to read data from a memory
    -- -------------------------------------------------------
    procedure rd_mem
        (   signal rd_port      : inout t_rd_port;
            variable addr       : in std_ulogic_vector (9 downto 0);
            variable exp_data   : in std_ulogic_vector (31 downto 0);
            variable l_result   : inout line
        );
        
end system_top_pkg;

package body system_top_pkg is
    -- -------------------------------------------------------
    -- Procedure to read data from a memory
    -- -------------------------------------------------------
    procedure rd_mem
        (   signal rd_port      : inout t_rd_port;
            variable addr       : in std_ulogic_vector (9 downto 0);
            variable exp_data   : in std_ulogic_vector( 31 downto 0);
            variable l_result   : inout line
        ) is
    begin
    
        rd_port.raddr <= transport addr after c_delay;  -- lese an Adresse addr
        -- Datenbus 'Z' wegen mode inout
        -- rd_port.data <= transport (others => 'Z') after c_delay;
        -- rd_port.clock <= 'Z';
        wait until rising_edge (rd_port.clk);
        wait until rising_edge (rd_port.clk);
        -- Fülle Line Puffer
        write (l_result, NOW, right, 10);
        hwrite (l_result, addr, right, 6);
        hwrite (l_result, exp_data, right, 10);
        hwrite (l_result. rd_port.rdata, right, 10);
        -- Monitor: Vergleiche aktuelle Daten mit erwarteten Daten
        if rd_port.rdata /= exp_data then
            -- Ausgabe zu STDOUT (Puffer behält seine Daten)  
            report l_result.all
            severity error;
            -- Füge "!!!" hinzu für einfache debugging
            write (l_result, string'("!!!"));
        end if;
    end rd_mem;        
            
end system_top_pkg;
