-----------------------------------------------------
-- Project :
-----------------------------------------------------
-- File    : ram_dual.vhd
-- Library :
-- Author  : michael.pichler@fhnw.ch
-- Company : Institute of Microelectronics (IME) FHNW
-- Copyright(C) IME
-----------------------------------------------------
-- Description :
-----------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ram_dual is
  generic (
    G_AWIDTH : natural := 10;
    G_DWIDTH : natural := 8
    );
  port (
    -- write port
    wclk  : in  std_ulogic;
    waddr : in  std_ulogic_vector(G_AWIDTH-1 downto 0);
    wen   : in  std_ulogic;
    wdata : in  std_ulogic_vector(G_DWIDTH-1 downto 0);
    -- read port
    rclk  : in  std_ulogic;
    raddr : in  std_ulogic_vector(G_AWIDTH-1 downto 0);
    rdata : out std_ulogic_vector(G_DWIDTH-1 downto 0)
    );
end ram_dual;

architecture rtl of ram_dual is

  -- Constants

  -- Build a 2-D array type for the RAM
  subtype t_word is std_ulogic_vector(G_DWIDTH-1 downto 0);
  type    t_ram is array(0 to 2**G_AWIDTH-1) of t_word;

  -- Declare the RAM signal.
  signal ram     : t_ram;
  signal waddr_i : natural range 0 to G_AWIDTH**2-1;
  signal raddr_i : natural range 0 to G_AWIDTH**2-1;
  
begin

  -- Internal address
  waddr_i <= to_integer(unsigned(waddr));
  raddr_i <= to_integer(unsigned(raddr));

  p_wr : process(wclk)
  begin
    if rising_edge(wclk) then
      if(wen = '1') then
        ram(waddr_i) <= wdata;
      end if;
    end if;
  end process p_wr;

  p_rd : process(rclk)
  begin
    if rising_edge(rclk) then
      rdata <= ram(raddr_i);
    end if;
  end process p_rd;
  
end rtl;
