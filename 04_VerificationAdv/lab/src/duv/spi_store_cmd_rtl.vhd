library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

entity spi_store_cmd is
  generic( 
    g_addr_bits : integer := 8
  );
  port( 
    cs_n           : in  std_ulogic;
    rst_n          : in  std_ulogic;
    sck            : in  std_ulogic;
    si             : in  std_ulogic;
    addr_rdy       : out std_ulogic;
    base_addr      : out std_ulogic_vector (g_addr_bits-1 downto 0);
    busy           : out std_ulogic;
    cmd            : out std_ulogic_vector (c_cmd_bits-1 downto 0);
    cmd_crc        : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    cmd_rdy        : out std_ulogic;
    nr_of_bytes    : out std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    rd_cmd_crc_err : out std_ulogic;
    rd_cmd_crc_rdy : out std_ulogic;
    rd_cmd_end     : out std_ulogic;
    read           : out std_ulogic;
    write          : out std_ulogic
  );
end entity spi_store_cmd ;


architecture rtl of spi_store_cmd is

  signal cmd_bit_cnt  : integer range c_cmd_bits-1 downto 0;  
  signal crc_bit_cnt  : integer range c_cmd_crc_bits-1 downto 0;  
  signal final_rd_crc : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);  
  signal rd_cmd_crc   : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);  
  signal rd_wrn       : std_ulogic;  
  signal update_crc   : std_ulogic;  

  type t_state is (
    idle,
    last_cmd,
    keep_cmd,
    store_addr,
    store_nr_bytes,
    store_crc,
    last_crc,
    eval_crc
  );

  signal current_state      : t_state;
  signal next_state         : t_state;
  signal cmd_cld            : std_ulogic_vector (c_cmd_bits-1 downto 0);
  signal cmd_rdy_cld        : std_ulogic ;
  signal rd_cmd_crc_rdy_cld : std_ulogic ;
  signal rd_cmd_end_cld     : std_ulogic ;

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
      cmd_cld <= (others => '0');
      cmd_rdy_cld <= '0';
      rd_cmd_crc_rdy_cld <= '0';
      rd_cmd_end_cld <= '0';
      cmd_bit_cnt <= c_cmd_bits-1;
      crc_bit_cnt <= c_cmd_crc_bits-1;
      rd_cmd_crc <= (others => '0');
      rd_wrn <= '0';
    elsif (rst_n = '0') then
      current_state <= idle;
      cmd_cld <= (others => '0');
      cmd_rdy_cld <= '0';
      rd_cmd_crc_rdy_cld <= '0';
      rd_cmd_end_cld <= '0';
      cmd_bit_cnt <= c_cmd_bits-1;
      crc_bit_cnt <= c_cmd_crc_bits-1;
      rd_cmd_crc <= (others => '0');
      rd_wrn <= '0';
    elsif (rising_edge(sck)) then
      current_state <= next_state;

      case next_state is
        when idle => 
          cmd_bit_cnt <= c_cmd_bits-1;
          cmd_cld <= (others => '0');
          cmd_rdy_cld <= '0';
          crc_bit_cnt <= c_cmd_crc_bits-1;
          rd_cmd_crc_rdy_cld <= '0';
          rd_wrn <= '0';
          rd_cmd_end_cld <= '0';
          rd_cmd_crc <= (others => '0');
        when store_addr => 
          cmd_cld <= si & cmd_cld(cmd_cld'high downto 1);  -- lsb first
          cmd_bit_cnt <= cmd_bit_cnt - 1;
        when store_nr_bytes => 
          cmd_cld <= si & cmd_cld(cmd_cld'high downto 1);  -- lsb first
          cmd_bit_cnt <= cmd_bit_cnt - 1;
        when last_cmd => 
          cmd_cld <= si & cmd_cld(cmd_cld'high downto 1);  -- lsb first
          cmd_rdy_cld <= '1';
          rd_wrn <= si;
        when store_crc => 
          cmd_rdy_cld <= '0';
          rd_cmd_crc <= si & rd_cmd_crc(rd_cmd_crc'high downto 1);  -- msbyte first, lsb first
          crc_bit_cnt <= crc_bit_cnt - 1;
        when last_crc => 
          rd_cmd_crc <= si & rd_cmd_crc(rd_cmd_crc'high downto 1);
          rd_cmd_end_cld <= '1';
          crc_bit_cnt <= c_cmd_crc_bits-1;
        when eval_crc => 
          rd_cmd_crc_rdy_cld <= '1';
          rd_cmd_end_cld <= '0';
        when keep_cmd => 
          cmd_rdy_cld <= '0';
          rd_cmd_crc_rdy_cld <= '0';
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
        next_state <= store_addr;
      when store_addr => 
        if (cmd_bit_cnt = c_cmd_dbn_bits) then 
          next_state <= store_nr_bytes;
        else
          next_state <= store_addr;
        end if;
      when store_nr_bytes => 
        if (cmd_bit_cnt = 0) then 
          next_state <= last_cmd;
        else
          next_state <= store_nr_bytes;
        end if;
      when last_cmd => 
        if (read = '1') then 
          next_state <= store_crc;
        else
          next_state <= keep_cmd;
        end if;
      when store_crc => 
        if (crc_bit_cnt = 0) then 
          next_state <= last_crc;
        else
          next_state <= store_crc;
        end if;
      when last_crc => 
        next_state <= eval_crc;
      when eval_crc => 
        next_state <= keep_cmd;
      when keep_cmd => 
        next_state <= keep_cmd;
      when others =>
        next_state <= idle;
    end case;
  end process p_nextstate;
 
  cmd            <= cmd_cld;
  cmd_rdy        <= cmd_rdy_cld;
  rd_cmd_crc_rdy <= rd_cmd_crc_rdy_cld;
  rd_cmd_end     <= rd_cmd_end_cld;
  base_addr      <= cmd_cld(g_addr_bits-1 downto 0);
  nr_of_bytes    <= cmd_cld(14 downto c_cmd_addr_bits);
  addr_rdy       <= cmd_rdy_cld;
  read           <= rd_wrn;  -- continous til end
  write          <= not(rd_wrn) and cmd_rdy_cld;
  busy           <= not(cs_n);
  
  -- for subsequent blocks
  p_cmd_crc : process(rst_n, cs_n, sck)
    variable v_crc : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);  -- needed because rx-cmd_cld is transmitted low byte first
  begin
    if cs_n = '1' then
      cmd_crc <= (others => '0');
    elsif rst_n = '0' then
      cmd_crc <= (others => '0');
    elsif rising_edge(sck) then
      if cmd_rdy_cld = '1' then
        v_crc   := next_crc16_d8(cmd_cld(7 downto 0), x"ff_ff");  -- init crc with low byte
        cmd_crc <= next_crc16_d8(cmd_cld(15 downto 8), v_crc);
      end if;
    end if;
  end process p_cmd_crc;
  
  p_update_crc : process(rst_n, cs_n, sck)
  begin
    if cs_n = '1' then
      update_crc <= '0';
    elsif rst_n = '0' then
      update_crc <= '0';
    elsif rising_edge(sck) then
      if crc_bit_cnt = c_cmd_crc_bits/2 or      -- msb of crc-word
         crc_bit_cnt = 0               then  -- lsb of crc-word
        update_crc <= '1';
      else 
        update_crc <= '0';
      end if;
    end if;
  end process p_update_crc;
  
  -- for crc-err: final crc (only rd-cmd_cld = no data) must be zero
  p_crc : process(rst_n, cs_n, sck)
    variable v_crc : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);  -- needed because crc is available only as word (msb first)
  begin
    if cs_n = '1' then
      final_rd_crc <= (others => '0');
    elsif rst_n = '0' then
      final_rd_crc <= (others => '0');
    elsif rising_edge(sck) then
      if cmd_rdy_cld = '1' then
        v_crc        := next_crc16_d8(cmd_cld( 7 downto 0), x"ff_ff");  -- init crc with low byte (lsb first)
        final_rd_crc <= next_crc16_d8(cmd_cld(15 downto 8), v_crc);
      elsif update_crc = '1' then
        final_rd_crc <= next_crc16_d8(rd_cmd_crc(15 downto 8), final_rd_crc);  -- valid byte is always left-bound (shifted in from left)
      end if;
    end if;
  end process p_crc;
  
  rd_cmd_crc_err <= '1' when final_rd_crc /= x"0000" else '0';

end architecture rtl;

