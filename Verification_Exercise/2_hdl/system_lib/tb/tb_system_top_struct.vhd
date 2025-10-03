-----------------------------------------------------
-- Project : system
-----------------------------------------------------
-- File    : tb_system_top_symbol.vhd
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

entity tb_system_top is
    generic(
        G_AWIDTH : natural := 10;
        G_DWIDTH : natural := 8
    );
end entity tb_system_top;

architecture struct of tb_system_top is

    -- Architecture declarations

    -- Internal signal declarations
    signal clk_40  : std_ulogic;
    signal clk_50  : std_ulogic := '0';
    signal clk_80  : std_ulogic;
    signal data0   : std_ulogic_vector(C_DWIDTH-1 downto 0);
    signal data1   : std_ulogic_vector(C_DWIDTH-1 downto 0);
    signal data2   : std_ulogic_vector(C_DWIDTH-1 downto 0);
    signal data3   : std_ulogic_vector(C_DWIDTH-1 downto 0);
    signal data_en : std_ulogic;
    signal locked  : std_ulogic;
    signal raddr   : std_ulogic_vector(9 downto 0);
    signal rdata   : std_ulogic_vector(4*C_DWIDTH-1 downto 0);
    signal rst     : std_ulogic := '0';

    -- Component Declarations
    component system_top
        generic (
            G_AWIDTH : natural := 10;
            G_DWIDTH : natural := 8
        );
        port (
            rst     : in  std_ulogic := '0';
            clk_50  : in  std_ulogic := '0';
            clk_40  : out std_ulogic;
            clk_80  : out std_ulogic;
            data0   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
            data1   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
            data2   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
            data3   : in  std_ulogic_vector (G_DWIDTH-1 downto 0);
            data_en : in  std_ulogic;
            locked  : out std_ulogic;
            raddr   : in  std_ulogic_vector (9 downto 0);
            rdata   : out std_ulogic_vector (4*G_DWIDTH-1 downto 0)
        );
    end component system_top;
    component tb_verify_system_top
        port (
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
    end component tb_verify_system_top;

begin

    -- Instance port mappings.
    i0_system_top : system_top
        generic map (
            G_AWIDTH => C_AWIDTH,
            G_DWIDTH => C_DWIDTH
        )
        port map (
            data0   => data0,
            data1   => data1,
            data2   => data2,
            data3   => data3,
            data_en => data_en,
            clk_40  => clk_40,
            clk_80  => clk_80,
            locked  => locked,
            clk_50  => clk_50,
            rst     => rst,
            rdata   => rdata,
            raddr   => raddr
        );
    i0_tb_verify_system_top : tb_verify_system_top
        port map (
            clk_40  => clk_40,
            clk_80  => clk_80,
            locked  => locked,
            rdata   => rdata,
            clk_50  => clk_50,
            data0   => data0,
            data1   => data1,
            data2   => data2,
            data3   => data3,
            data_en => data_en,
            raddr   => raddr,
            rst     => rst
        );

end architecture struct;
