-----------------------------------------------------
-- Project : system
-----------------------------------------------------
-- File    : tb_verify_system_top_sim.vhd
-- Library : system_tb_lib
-- Author  : michael.pichler@fhnw.ch
-- Company : FHNW - ISE
-- Copyright(C) ISE
-----------------------------------------------------
-- Description :
-----------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use ieee.std_logic_textio.all;

library system_tb_lib;
use system_tb_lib.system_top_pkg.all;


library std;
use std.TEXTIO.all;

entity tb_verify_system_top is
    
--    generic (
--         C_DWIDTH   : natural := 8;
--         C_AWIDTH   : natural := 10 
--         );
         
    port(
        clk_40  : in  std_ulogic;
        clk_80  : in  std_ulogic;
        locked  : in  std_ulogic;
        rdata   : in  std_ulogic_vector (4*C_DWIDTH-1 downto 0);
        clk_50  : out std_ulogic;
        data0   : out std_ulogic_vector (C_DWIDTH-1 downto 0);
        data1   : out std_ulogic_vector (C_DWIDTH-1 downto 0);
        data2   : out std_ulogic_vector (C_DWIDTH-1 downto 0);
        data3   : out std_ulogic_vector (C_DWIDTH-1 downto 0);
        data_en : out std_ulogic;
        raddr   : out std_ulogic_vector (9 downto 0);
        rst     : out std_ulogic
    );
end tb_verify_system_top;


architecture sim of tb_verify_system_top is

    -- Record fuer lese / schreibe Port
    signal in_port  : t_in_port;
    signal rd_port  : t_rd_port;
    
    -- Kontrollsignale
    signal stim_done    : boolean := false;
    signal rd_done      : boolean := false;
 
begin

    -- Verbindungen zwischen ports und records
    
    -- fuer in_port
    data_en <= in_port.data_en;
    data0   <= in_port.data0;
    data1   <= in_port.data1;
    data2   <= in_port.data2;
    data3   <= in_port.data3;
    
    -- fuer rd_port
    raddr           <= rd_port.raddr;
    rd_port.rdata   <= rdata;
    rd_port.clk     <= clk_40;
    
    -- -------------------------------------------------------
    -- Stimuli for clock and reset
    -- -------------------------------------------------------
    p_clk_and_rst : process

--    variable counter : integer := 0;
    begin
        -- Reset Sequenz    
        rst <= transport '1', '0' after 10 *c_cycle;
        
        -- Clock Erzeugung
        while (not rd_done) loop
            clk_50 <= '1' after c_cycle/2,
                '0' after c_cycle;
            wait for c_cycle;
        end loop;
        
        -- Prozess beenden
        report "Clock & Reset Prozess beendet";
        wait for 200 ns;
        wait;
    end process p_clk_and_rst;
    
    -- -------------------------------------------------------
    -- Stimulate inputs
    -- -------------------------------------------------------
    p_stim : process
        file f_stim             : text open read_mode is c_stim;
        variable l_stim         : line;
        variable v_in_port      : t_in_port;
        variable v_count_old    : natural;
    begin
    -- Initialisierung
    in_port <= (0, '0', (others => '0'), (others => '0'), (others => '0'), (others => '0'));
    wait until locked = '1';        -- warte bis PLL locked
    wait until rising_edge (clk_40);-- synchronisiere auf 40 MHz Clock
    wait for c_delay;               -- Stimuliere Inputs nach rising edge (propagation delay aus system_top_pkg)
    -- Stimuliere Inhalt aus Datei f_stim
    while not endfile(f_stim) loop
        v_count_old := v_in_port.count;
        readline (f_stim, l_stim);
        read (l_stim, v_in_port.count);
        read (l_stim, v_in_port.data_en);
        hread (l_stim, v_in_port.data0);
        hread (l_stim, v_in_port.data1);
        hread (l_stim, v_in_port.data2);
        hread (l_stim, v_in_port.data3);
        wait for (v_in_port.count - v_count_old) * c_cycle_40;
        in_port <= v_in_port;  -- Variable "v_in_port" wird auf Signal in_port geschrieben 
    end loop;
    file_close(f_stim);

    -- beende Prozess
    report "Stimuli Prozess beendet";
    stim_done <= true;
    wait;
    end process p_stim;
    
    -- -------------------------------------------------------
    -- Read from memory
    -- ------------------------------------------------------- 
    p_read : process
        file f_exp          : text open read_mode is c_exp;
        file f_dump         : text open WRITE_MODE is c_dump;
        constant c_header   : STRING := " Sim-Time Addr Expected Actual";
        variable l_read     : LINE;
        variable l_result   : LINE;
        variable addr       : STD_ULOGIC_VECTOR (9 downto 0);
        variable exp_data   : STD_ULOGIC_VECTOR (31 downto 0);
    begin
        -- Initialisierung
        -- rd_port <= ('Z', (others => '0'), (others => 'Z'));
        --write (l_result, string'("  Sim-Time   Addr  Expected    Actual"));
        WRITE (l_result, c_header);
        WRITELINE (f_dump, l_result);

        wait until stim_done = TRUE;            -- Warte bis Schreiben beendet
        wait until rising_edge(rd_port.clk);    -- Sync auf clock_40

        -- Lese Speicher und vergleiche mit erwarteten Werten
        while not ENDFILE(f_exp) loop
            READLINE (f_exp, l_read);
            HREAD (l_read, addr);
            HREAD (l_read, exp_data);
            rd_mem (rd_port, addr,exp_data,l_result);
            -- Schreibe result-Puffer in Datei
            WRITELINE (f_dump, l_result);
        end loop;
        FILE_CLOSE(f_exp);
        FILE_CLOSE(f_dump);

        -- Beende Prozess
        report "Lese Prozess beendet";
        rd_done <= TRUE;
        wait;
        
    end process p_read;
    
end architecture sim;