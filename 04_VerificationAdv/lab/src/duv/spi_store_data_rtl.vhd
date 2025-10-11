library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

entity spi_store_data is
  generic( 
    g_data_bits : integer := 8
  );
  port( 
    cmd_crc        : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    cs_n           : in  std_ulogic;
    nr_of_bytes    : in  std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    rst_n          : in  std_ulogic;
    sck            : in  std_ulogic;
    si             : in  std_ulogic;
    write          : in  std_ulogic;
    calc_crc       : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    update_crc_1st : out std_ulogic;
    update_crc_2nd : out std_ulogic;
    wr_cmd_end     : out std_ulogic;
    wr_crc         : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    wr_data        : out std_ulogic_vector (g_data_bits-1 downto 0);
    wr_req         : out std_ulogic
  );
end entity spi_store_data ;


architecture rtl of spi_store_data is

  signal crc_bit_cnt  : integer range c_cmd_crc_bits-1 downto 0;  
  signal data_bit_cnt : integer range g_data_bits-1 downto 0;  
  signal data_cnt     : unsigned(c_cmd_dbn_bits-1 downto 0);  
  signal wr_cmd_crc   : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);  
  signal wr_data_i    : std_ulogic_vector(g_data_bits-1 downto 0);  -- due to intelquartus vhdl-2008 problem
  signal wr_data_rdy  : std_ulogic;  

  type t_state is (
    idle,
    store_data,
    last_data,
    store_crc,
    last_crc
  );

  signal current_state       : t_state;
  signal next_state          : t_state;
  signal store_data_busy_cld : std_ulogic ;
  signal wr_cmd_end_cld      : std_ulogic ;

begin

  -----------------------------------------------------------------
  p_clocked : process ( 
    sck,
    cs_n,
    rst_n
  )
  -----------------------------------------------------------------
  begin
    if (cs_n = '1') then
      current_state <= idle;
      wr_cmd_end_cld <= '0';
      crc_bit_cnt <= c_cmd_crc_bits-1;
      data_bit_cnt <= g_data_bits-1;
      wr_cmd_crc <= (others => '0');
      wr_data_i <= (others => '0');
      wr_data_rdy <= '0';
    elsif (rst_n = '0') then
      current_state <= idle;
      wr_cmd_end_cld <= '0';
      crc_bit_cnt <= c_cmd_crc_bits-1;
      data_bit_cnt <= g_data_bits-1;
      wr_cmd_crc <= (others => '0');
      wr_data_i <= (others => '0');
      wr_data_rdy <= '0';
    elsif (rising_edge(sck)) then
      current_state <= next_state;

      case next_state is
        when idle => 
          data_bit_cnt <= g_data_bits-1;
          crc_bit_cnt <= c_cmd_crc_bits-1;
          wr_data_i <= (others => '0');
          wr_cmd_crc <= (others => '0');
          wr_data_rdy <= '0';
          wr_cmd_end_cld <= '0';
        when store_data => 
          wr_data_i <= si & wr_data_i(wr_data_i'high downto 1);  -- lsb first
          data_bit_cnt <= data_bit_cnt - 1;
          wr_data_rdy <= '0';
        when last_data => 
          wr_data_i <= si & wr_data_i(wr_data_i'high downto 1);
          data_bit_cnt <= g_data_bits-1;
          wr_data_rdy <= '1';
        when store_crc => 
          wr_data_rdy <= '0';
          wr_cmd_crc <= si & wr_cmd_crc(wr_cmd_crc'high downto 1);  -- msbyte first, lsb first
          crc_bit_cnt <= crc_bit_cnt - 1;
        when last_crc => 
          wr_cmd_crc <= si & wr_cmd_crc(wr_cmd_crc'high downto 1);
          crc_bit_cnt <= c_cmd_crc_bits-1;
          wr_cmd_end_cld <= '1';
        when others =>
          null;
      end case;
    end if;
  end process p_clocked;
 
  -----------------------------------------------------------------
  p_nextstate : process (all)
  -----------------------------------------------------------------
  begin
    case current_state is
      when idle => 
        if (write = '1') then 
          next_state <= store_data;
        else
          next_state <= idle;
        end if;
      when store_data => 
        if (data_bit_cnt = 0) then 
          next_state <= last_data;
        else
          next_state <= store_data;
        end if;
      when last_data => 
        if (data_cnt = unsigned(nr_of_bytes)) then 
          next_state <= store_crc;
        else
          next_state <= store_data;
        end if;
      when store_crc => 
        if (crc_bit_cnt = 0) then 
          next_state <= last_crc;
        else
          next_state <= store_crc;
        end if;
      when last_crc => 
        next_state <= idle;
      when others =>
        next_state <= idle;
    end case;
  end process p_nextstate;

  wr_cmd_end <= wr_cmd_end_cld;
  wr_req     <= wr_data_rdy when data_cnt <= unsigned(nr_of_bytes) else '0';
  wr_data    <= wr_data_i;  -- due to intelquartus vhdl-2008 problem
  
  p_data_cnt : process(rst_n, cs_n, sck)
  begin
    if cs_n = '1' then
      data_cnt <= (others => '0');
    elsif rst_n = '0' then
      data_cnt <= (others => '0');
    elsif rising_edge(sck) then
      if wr_data_rdy = '1' then
        data_cnt <= data_cnt+1;
      end if;
    end if;
  end process p_data_cnt;
  
  p_update_crc : process(rst_n, cs_n, sck)
  begin
    if cs_n = '1' then
      update_crc_1st <= '0';
      update_crc_2nd <= '0';
    elsif rst_n = '0' then
      update_crc_1st <= '0';
      update_crc_2nd <= '0';
    elsif rising_edge(sck) then
      if    crc_bit_cnt = c_cmd_crc_bits/2 then  -- msb of crc-word
        update_crc_1st <= '1';
        update_crc_2nd <= '0';
      elsif crc_bit_cnt = 0  then                -- lsb of crc-word
        update_crc_1st <= '0';
        update_crc_2nd <= '1';
      else 
        update_crc_1st <= '0';
        update_crc_2nd <= '0';
      end if;
    end if;
  end process p_update_crc;
  
  p_crc : process(rst_n, cs_n, sck)
  begin
    if cs_n = '1' then
      calc_crc <= (others => '0');
    elsif rst_n = '0' then
      calc_crc <= (others => '0');
    elsif rising_edge(sck) then
      if wr_data_rdy = '1' then   -- each data-byte
        if data_cnt = 0 then        -- first data-byte
          calc_crc <= next_crc16_d8(wr_data_i, cmd_crc);  -- init
        else                      
          calc_crc <= next_crc16_d8(wr_data_i, calc_crc); 
        end if;
      end if;
    end if;
  end process p_crc;
  
  wr_crc <= wr_cmd_crc;

end architecture rtl;

