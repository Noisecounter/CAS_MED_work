library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

entity spi_send_echo is
  generic( 
    g_addr_bits : integer := 8;
    g_data_bits : integer := 8
  );
  port( 
    cmd           : in  std_ulogic_vector (c_cmd_bits-1 downto 0);
    cmd_rdy       : in  std_ulogic;
    cs_n          : in  std_ulogic;
    echo_crc      : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    rd_cmd_end    : in  std_ulogic;
    rd_data       : in  std_ulogic_vector (g_data_bits-1 downto 0);
    read          : in  std_ulogic;
    rst_n         : in  std_ulogic;
    sck           : in  std_ulogic;
    status        : in  std_ulogic_vector (c_cmd_status_bits-1 downto 0);
    wr_cmd_end    : in  std_ulogic;
    rd_req        : out std_ulogic;
    so            : out std_ulogic;
    update_status : out std_ulogic
  );
end entity spi_send_echo ;

architecture rtl of spi_send_echo is

  signal cmd_int       : std_ulogic_vector(c_cmd_bits-1 downto 0);  
  signal data_bit_cnt  : integer range g_data_bits-1 downto 0;  
  signal echo_crc_int  : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);  
  signal rd_data_int_0 : std_ulogic_vector(g_data_bits-1 downto 0);  
  signal rd_data_int_1 : std_ulogic_vector(g_data_bits-1 downto 0);  
  signal rd_data_sel   : std_ulogic;  
  signal reg_len_cnt   : unsigned(c_cmd_dbn_bits-1 downto 0);  
  signal status_int    : std_ulogic_vector(c_cmd_status_bits-1 downto 0);  

  type t_state is (
    idle,
    store_cmd,
    echo_cmd_lsb,
    echo_cmd_lo,
    echo_cmd_hi,
    echo_stat,
    send_db0,
    send_dbn,
    echo_crc_hi,
    echo_crc_lo
  );
 
  signal current_state     : t_state;
  signal next_state        : t_state;
  signal rd_req_cld        : std_ulogic ;
  signal so_cld            : std_ulogic ;
  signal update_status_cld : std_ulogic ;

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
      rd_req_cld <= '0';
      so_cld <= '0';
      update_status_cld <= '0';
      cmd_int <= (others => '0');
      data_bit_cnt <= g_data_bits-1;
      echo_crc_int <= (others => '0');
      rd_data_int_0 <= (others => '0');
      rd_data_int_1 <= (others => '0');
      rd_data_sel <= '0';
      reg_len_cnt <= (others => '0');
      status_int <= (others => '0');
    elsif (rst_n = '0') then
      current_state <= idle;
      -- default reset values
      rd_req_cld <= '0';
      so_cld <= '0';
      update_status_cld <= '0';
      cmd_int <= (others => '0');
      data_bit_cnt <= g_data_bits-1;
      echo_crc_int <= (others => '0');
      rd_data_int_0 <= (others => '0');
      rd_data_int_1 <= (others => '0');
      rd_data_sel <= '0';
      reg_len_cnt <= (others => '0');
      status_int <= (others => '0');
    elsif (falling_edge(sck)) then
      current_state <= next_state;

      case next_state is
        when idle => 
          data_bit_cnt <= g_data_bits-1;
          so_cld <= '0';
          cmd_int <= (others => '0');
          status_int <= (others => '0');
          rd_data_int_0 <= (others => '0');
          rd_data_int_1 <= (others => '0');
          rd_data_sel <= '0';
          echo_crc_int <= (others => '0');
          reg_len_cnt <= (others => '0');
          update_status_cld <= '0';
        when store_cmd => 
          cmd_int <= cmd;
          if read = '1' then  -- read at least one byte
            reg_len_cnt <= unsigned(cmd(14 downto c_cmd_addr_bits));
          end if;
        when echo_cmd_lsb => 
          so_cld <= cmd_int(0);
          cmd_int <= '0' & cmd_int(c_cmd_bits-1 downto 1);
        when echo_cmd_lo => 
          if data_bit_cnt = 0 then
            data_bit_cnt <= g_data_bits-1;
          else
            data_bit_cnt <= data_bit_cnt - 1;
          end if;
          so_cld <= cmd_int(0);
          cmd_int <= '0' & cmd_int(c_cmd_bits-1 downto 1);
        when echo_cmd_hi => 
          if data_bit_cnt = 0 then
            data_bit_cnt <= g_data_bits-1;
          else
            data_bit_cnt <= data_bit_cnt - 1;
          end if;
          so_cld <= cmd_int(0);
          cmd_int <= '0' & cmd_int(c_cmd_bits-1 downto 1);
          if data_bit_cnt <= 2 then
            -- synchonisation in spi_a_com need ~3+clk-cycles = 195.3 ns
            -- on falling-edge of update_status.
            -- spi-a-sck (max) = 4mbit = 250 ns. so activate update_status
            -- at latest one sck-scyle before status is send
            update_status_cld <= '0';
          else
            update_status_cld <= '1';  
          end if;
          status_int <= status;
        when echo_stat => 
          if data_bit_cnt = 0 then   -- read first register
            if read = '1' then
              rd_req_cld <= '1';
            end if;
          else
            rd_req_cld <= '0';
          end IF;
          if data_bit_cnt = 0 then
            data_bit_cnt <= g_data_bits-1;
            echo_crc_int <= echo_crc;
           elsif data_bit_cnt = 1 then
            if read = '1' then
              rd_data_int_0 <= rd_data;
            end if;
            if read = '1' and reg_len_cnt /= 0 then  -- more than 1 rd-byte
              rd_data_sel <= not(rd_data_sel);
            end if;
            data_bit_cnt <= data_bit_cnt - 1;
          else
            data_bit_cnt <= data_bit_cnt - 1;
          end if;
          so_cld <= status_int(0);
          status_int <= '0' & status_int(c_cmd_status_bits-1 downto 1);
        when send_db0 => 
          if data_bit_cnt = 0 then
            data_bit_cnt <= g_data_bits-1;
            echo_crc_int <= echo_crc;
            if reg_len_cnt /= 0 then  -- more than 1 rd-byte
              rd_req_cld <= '1';
            end if;
          elsif data_bit_cnt = 1 then
            rd_data_int_1 <= rd_data;
            rd_data_sel <= not(rd_data_sel);
            data_bit_cnt <= data_bit_cnt - 1;
          elsif data_bit_cnt = g_data_bits-1 then
            rd_req_cld <= '0';
            data_bit_cnt <= data_bit_cnt - 1;
          else
            data_bit_cnt <= data_bit_cnt - 1;
          end if;
          so_cld <= rd_data_int_0(0);
          rd_data_int_0 <= '0' & rd_data_int_0(g_data_bits -1 downto 1);
        when send_dbn => 
          if data_bit_cnt = 0 then
            data_bit_cnt <= g_data_bits-1;
            echo_crc_int <=  echo_crc;
            if reg_len_cnt /= 1 then 
--             if reg_len_cnt /= 0 then 
              rd_req_cld <= '1';
            end if;
            reg_len_cnt <= reg_len_cnt-1;
          elsif data_bit_cnt = 1 then
            if rd_data_sel = '0' then
              rd_data_int_0 <= rd_data;
            else
              rd_data_int_1 <= rd_data;
            end if;
            rd_data_sel <= not(rd_data_sel);
            data_bit_cnt <= data_bit_cnt - 1;
          elsif data_bit_cnt = g_data_bits-1 then
            rd_req_cld <= '0';
            data_bit_cnt <= data_bit_cnt - 1;
          else
            data_bit_cnt <= data_bit_cnt - 1;
          end if;
          if rd_data_sel = '1' then
            so_cld <= rd_data_int_0(0);
            rd_data_int_0 <= '0' & rd_data_int_0(g_data_bits -1 downto 1);
          else
            so_cld <= rd_data_int_1(0);
            rd_data_int_1 <= '0' & rd_data_int_1(g_data_bits -1 downto 1);
          end if;
        when echo_crc_hi => 
          if data_bit_cnt = 0 then
            data_bit_cnt <= g_data_bits-1;
          else
            data_bit_cnt <= data_bit_cnt - 1;
          end if;
          so_cld <= echo_crc_int(0);
          echo_crc_int <= '0' & echo_crc_int(c_cmd_crc_bits-1 downto 1);
        when echo_crc_lo => 
          if data_bit_cnt = 0 then
            data_bit_cnt <= g_data_bits-1;
          else
            data_bit_cnt <= data_bit_cnt - 1;
          end if;
          so_cld <= echo_crc_int(0);
          echo_crc_int <= '0' & echo_crc_int(c_cmd_crc_bits-1 downto 1);
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
        if (cmd_rdy = '1') then 
          next_state <= store_cmd;
        else
          next_state <= idle;
        end if;
      when store_cmd => 
        if (wr_cmd_end = '1' or
            rd_cmd_end = '1') then 
          next_state <= echo_cmd_lsb;
        else
          next_state <= store_cmd;
        end if;
      when echo_cmd_lsb => 
        next_state <= echo_cmd_lo;
      when echo_cmd_lo => 
        if (data_bit_cnt = 0) then 
          next_state <= echo_cmd_hi;
        else
          next_state <= echo_cmd_lo;
        end if;
      when echo_cmd_hi => 
        if (data_bit_cnt = 0) then 
          next_state <= echo_stat;
        else
          next_state <= echo_cmd_hi;
        end if;
      when echo_stat => 
        if (data_bit_cnt = 0 and read = '1') then 
          next_state <= send_db0;
        elsif (data_bit_cnt = 0) then 
          next_state <= echo_crc_hi;
        else
          next_state <= echo_stat;
        end if;
      when send_db0 => 
        if (data_bit_cnt = 0 and reg_len_cnt = 0) then 
          next_state <= echo_crc_hi;
        elsif (data_bit_cnt = 0) then 
          next_state <= send_dbn;
        else
          next_state <= send_db0;
        end if;
      when send_dbn => 
        if (data_bit_cnt = 0 and reg_len_cnt = 0) then 
          next_state <= echo_crc_hi;
        else
          next_state <= send_dbn;
        end if;
      when echo_crc_hi => 
        if (data_bit_cnt = 0) then 
          next_state <= echo_crc_lo;
        else
          next_state <= echo_crc_hi;
        end if;
      when echo_crc_lo => 
        next_state <= echo_crc_lo;
      when others =>
        next_state <= idle;
    end case;
  end process p_nextstate;
 
  rd_req        <= rd_req_cld;
  so            <= so_cld;
  update_status <= update_status_cld;

end architecture rtl;

