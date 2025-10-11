library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

package cas_func_pkg is
  
  ------------------------------------------------------------------------------
  -- function declarations
  ------------------------------------------------------------------------------
  ------------------------------------------------------------------------------
  function log2ceil (n : natural) return natural;
  ------------------------------------------------------------------------------
  

  ------------------------------------------------------------------------------
  -- see https://www.easics.be/webtools/crctool
  ------------------------------------------------------------------------------
  -- copyright (c) 1999-2008 easics nv.
  -- this source file may be used and distributed without restriction
  -- provided that this copyright statement is not removed from the file
  -- and that any derivative work contains the original copyright notice
  -- and the associated disclaimer.
  --
  -- this source file is provided "as is" and without any express
  -- or implied warranties, including, without limitation, the implied
  -- warranties of merchantibility and fitness for a particular purpose.
  --
  -- purpose : synthesizable crc function
  --   * polynomial: x^16 + x^12 + x^5 + 1
  --   * data width: 8
  --
  -- info : tools@easics.be
  --        http://www.easics.com
  ------------------------------------------------------------------------------
  -- polynomial: x^16 + x^12 + x^5 + 1
  -- data width: 8
  -- convention: the first serial bit is d[7]
  function next_crc16_d8(data : std_ulogic_vector( 7 downto 0);
                         crc  : std_ulogic_vector(15 downto 0)) return std_ulogic_vector;
  ------------------------------------------------------------------------------
  
  
  ------------------------------------------------------------------------------
  function to_hex(vec : in std_ulogic_vector(3 downto 0)) return std_ulogic_vector; 
  ------------------------------------------------------------------------------

-------------------------------------------------------------

end package cas_func_pkg;

-------------------------------------------------------------
-------------------------------------------------------------
-------------------------------------------------------------

package body cas_func_pkg is
  
  ------------------------------------------------------------------------------
  -- function bodies
  ------------------------------------------------------------------------------
  ------------------------------------------------------------------------------
  function log2ceil(n : natural) return natural is
    variable v_nr_bit : unsigned(31 downto 0);
  begin  
    if n = 0 then
      return 1;
    else    
      v_nr_bit := to_unsigned(n-1,32);
      for i in 31 downto 0 loop
        if v_nr_bit(i) = '1' then
         return i+1;
        end if;
      end loop;
      return 0;  -- will never happen
    end if;
  end log2ceil;
  ------------------------------------------------------------------------------
  
  
  ------------------------------------------------------------------------------
  -- polynomial: x^16 + x^12 + x^5 + 1
  -- data width: 8
  -- convention: the first serial bit is d[7]
  function next_crc16_d8(data : std_ulogic_vector( 7 downto 0);
                         crc  : std_ulogic_vector(15 downto 0)) return std_ulogic_vector is

    variable d:      std_ulogic_vector( 7 downto 0);
    variable c:      std_ulogic_vector(15 downto 0);
    variable newcrc: std_ulogic_vector(15 downto 0);

  begin
    d := data;
    c := crc;

    newcrc(0) := d(4) xor d(0) xor c(8) xor c(12);
    newcrc(1) := d(5) xor d(1) xor c(9) xor c(13);
    newcrc(2) := d(6) xor d(2) xor c(10) xor c(14);
    newcrc(3) := d(7) xor d(3) xor c(11) xor c(15);
    newcrc(4) := d(4) xor c(12);
    newcrc(5) := d(5) xor d(4) xor d(0) xor c(8) xor c(12) xor c(13);
    newcrc(6) := d(6) xor d(5) xor d(1) xor c(9) xor c(13) xor c(14);
    newcrc(7) := d(7) xor d(6) xor d(2) xor c(10) xor c(14) xor c(15);
    newcrc(8) := d(7) xor d(3) xor c(0) xor c(11) xor c(15);
    newcrc(9) := d(4) xor c(1) xor c(12);
    newcrc(10) := d(5) xor c(2) xor c(13);
    newcrc(11) := d(6) xor c(3) xor c(14);
    newcrc(12) := d(7) xor d(4) xor d(0) xor c(4) xor c(8) xor c(12) xor c(15);
    newcrc(13) := d(5) xor d(1) xor c(5) xor c(9) xor c(13);
    newcrc(14) := d(6) xor d(2) xor c(6) xor c(10) xor c(14);
    newcrc(15) := d(7) xor d(3) xor c(7) xor c(11) xor c(15);
    return newcrc;
  end next_crc16_d8;
  ------------------------------------------------------------------------------


  -------------------------------------------------------------------------------------
  -- to convert 4-bit vector into a hex-coded-value for a 7-segment-display:
  function to_hex(vec : in std_ulogic_vector(3 downto 0)) return std_ulogic_vector is 
    variable v_tmp : std_ulogic_vector(6 downto 0);
    variable v_hex : std_ulogic_vector(6 downto 0);
  begin
    case vec is
      when x"0"   => v_tmp := "0000001";
      when x"1"   => v_tmp := "1001111";
      when x"2"   => v_tmp := "0010010";
      when x"3"   => v_tmp := "0000110";
      when x"4"   => v_tmp := "1001100";
      when x"5"   => v_tmp := "0100100";
      when x"6"   => v_tmp := "0100000";
      when x"7"   => v_tmp := "0001111";
      when x"8"   => v_tmp := "0000000";
      when x"9"   => v_tmp := "0000100";
      when x"a"   => v_tmp := "0001000";
      when x"b"   => v_tmp := "1100000";
      when x"c"   => v_tmp := "0110001";
      when x"d"   => v_tmp := "1000010";
      when x"e"   => v_tmp := "0110000";
      when x"f"   => v_tmp := "0111000";
      when others => v_tmp := "1111110";  -- all dark except g
    end case;
    for i in 6 downto 0 loop
      v_hex(6-i) := v_tmp(i);
    end loop;
    return v_hex;
  end function to_hex;
  -------------------------------------------------------------------------------------
  
end package body cas_func_pkg;
