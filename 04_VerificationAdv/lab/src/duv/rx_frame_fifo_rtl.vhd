library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

entity rx_frame_fifo is
  port( 
    clear_fifo       : in  std_ulogic;
    clk              : in  std_ulogic;
    data_rd_req      : in  std_ulogic;
    data_wr          : in  std_ulogic_vector (c_csr_data_bits-1 downto 0);
    data_wr_req      : in  std_ulogic;
    rst_n            : in  std_ulogic;
    fifo_rd_data     : out std_ulogic_vector (c_csr_data_bits-1 downto 0);
    frame_fifo_level : out unsigned (c_frame_fifo_level_bits-1 downto 0)
  );
end entity rx_frame_fifo ;


architecture rtl of rx_frame_fifo is
  type t_fifo is array (0 to c_frame_fifo_depth-1) of std_ulogic_vector(c_csr_data_bits-1 downto 0);
  signal rd_ptr : unsigned(c_frame_fifo_level_bits-1 downto 0);
  signal wr_ptr : unsigned(c_frame_fifo_level_bits-1 downto 0);
  signal mem    : t_fifo;
begin

  p_wr_ptr : process(rst_n, clk)
  begin
    if rst_n = '0' then
      wr_ptr <= (others => '0');
    elsif rising_edge(clk) then
      if clear_fifo = '1' then
        wr_ptr <= (others => '0');
      else 
        if data_wr_req = '1' and data_rd_req = '0' then
          wr_ptr <= wr_ptr+1;
        end if;
      end if;
    end if;
  end process p_wr_ptr;


  p_rd_ptr : process(rst_n, clk)
  begin
    if rst_n = '0' then
      rd_ptr <= (others => '0');
    elsif rising_edge(clk) then
      if clear_fifo = '1' then
        rd_ptr <= (others => '0');
      else 
        if data_rd_req = '1' and data_wr_req = '0' then
          rd_ptr <= rd_ptr+1;
        end if;
      end if;
    end if;
  end process p_rd_ptr;


  p_fifo_level : process(rst_n, clk)
  begin
    if rst_n = '0' then
      frame_fifo_level <= (others => '0');
    elsif rising_edge(clk) then
      if clear_fifo = '1' then
        frame_fifo_level <= (others => '0');
      elsif data_wr_req = '1' and data_rd_req = '0' then
        frame_fifo_level <= frame_fifo_level+1;
      elsif data_rd_req = '1' and data_wr_req = '0' then
        frame_fifo_level <= frame_fifo_level-1;
      end if;
    end if;
  end process p_fifo_level;
  
  
  p_fifo : process(rst_n, clk)
  begin
    if rst_n = '0' then
      mem          <= (others => (others => '0'));
      fifo_rd_data <= (others => '0');
    elsif rising_edge(clk) then
      if data_wr_req = '1' and data_rd_req = '0' then
        mem(to_integer(wr_ptr)) <= data_wr;
      elsif data_rd_req = '1' and data_wr_req = '0' then
        fifo_rd_data <= mem(to_integer(rd_ptr));
      end if;
    end if;
  end process p_fifo;
  
  
end architecture rtl;

