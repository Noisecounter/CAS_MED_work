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

constant c_end_sim  : integer  := 50000;
signal p_stim_done : std_logic := '0';
    
begin

    p_clk_and_rst : process

    variable counter : integer := 0;

    begin
-- Reset Sequenz    
        rst <= '1';
        wait for 20ns;
        rst <= '0';
        wait for 5ns;
-- Start der Wiederholungssequenz
    while counter < c_end_sim loop
        clk_50 <= '0';
        WAIT FOR 5ns ;
        clk_50 <= '1';
        WAIT FOR 10ns; 
        clk_50 <= '0';
        WAIT FOR 5ns ;
        counter := counter + 1;
    end loop;
        clk_50 <= '0';
        report "End of Simulation";
        wait;
-- Ende der Wiederholungssequenz
    end process p_clk_and_rst; 
    
    p_stim : process
        
    variable counter : integer := 0;
    
    begin
    
    wait until locked = '1';
    
    while counter <= 3 loop
        wait until rising_edge (clk_40);
        
            case counter is
                when 0 =>
                    data_en <= '0';
                    data0 <= x"00";
                    data1 <= x"00";
                    data2 <= x"00";
                    data3 <= x"00";
                when 1 =>
                    data_en <= '1';
                    data0 <= x"07";
                    data1 <= x"01";
                    data2 <= x"09";
                    data3 <= x"01";
                when 2 =>
                    data_en <= '1';
                    data0 <= x"07";
                    data1 <= x"02";
                    data2 <= x"09";
                    data3 <= x"02";
                when 3 =>
                    data_en <= '0';
                    data0 <= x"07";
                    data1 <= x"00";
                    data2 <= x"09";
                    data3 <= x"00";
            end case;
            counter := + 1;
     
    end loop;
   
    p_stim_done <= '1';
    report "Process p_wirite finished";
    wait;
    
    end process p_stim;
    
    p_read : process
    
    begin
    
    wait until locked = '1';
    wait until p_stim_done = '1';
    wait until rising_edge (clk_40);
        
   -- Adresse setzen und rdata prüfen
        raddr <= "0000000000";                  --entspricht raddr 0
        assert rdata = x"07010007";
            report "raddr 0 stimmt nicht mit expected rdata überrein"
            severity error;
            
        raddr <= "0000000001";                  --entspricht raddr 1
        assert rdata = x"09010009";
            report "raddr 1 stimmt nicht mit expected rdata überrein"
            severity error;
        
        raddr <= "0000000010";                  --entspricht raddr 2
        assert rdata = x"00702000E";
            report "raddr 2 stimmt nicht mit expected rdata überrein"
            severity error;
            
        raddr <= "0000000011";                  --entspricht raddr 3
        assert rdata = x"09020012";
            report "raddr 3 stimmt nicht mit expected rdata überrein"
            severity error;
            
        raddr <= "0000001000";                  --entspricht raddr 8
        assert rdata = x"00000000";
            report "raddr 8 stimmt nicht mit expected rdata überrein"
            severity error;

   
    report "Process p_read finished";
    wait;
    end process p_read;    

end architecture sim;
