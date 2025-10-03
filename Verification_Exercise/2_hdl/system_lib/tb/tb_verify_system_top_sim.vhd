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
    
begin

end architecture sim;
