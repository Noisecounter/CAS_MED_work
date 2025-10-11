library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_pkg.all;

entity spi_slave is
  generic( 
    g_addr_bits : integer := 8;
    g_data_bits : integer := 8
  );
  port( 
    cs_n           : in  std_ulogic;
    echo_crc       : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    rd_data        : in  std_ulogic_vector (g_data_bits-1 downto 0);
    rst_n          : in  std_ulogic;
    sck            : in  std_ulogic;
    si             : in  std_ulogic;
    status         : in  std_ulogic_vector (c_cmd_status_bits-1 downto 0);
    addr_rdy       : out std_ulogic;
    base_addr      : out std_ulogic_vector (g_addr_bits-1 downto 0);
    busy           : out std_ulogic;
    calc_crc       : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    cmd_crc        : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    nr_of_bytes    : out std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    rd_cmd_crc_err : out std_ulogic;
    rd_cmd_crc_rdy : out std_ulogic;
    rd_req         : out std_ulogic;
    rd_wrn         : out std_ulogic;
    so             : out std_ulogic;
    update_crc_1st : out std_ulogic;
    update_crc_2nd : out std_ulogic;
    update_status  : out std_ulogic;
    wr_addr_rdy    : out std_ulogic;
    wr_crc         : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    wr_data        : out std_ulogic_vector (g_data_bits-1 downto 0);
    wr_req         : out std_ulogic
  );
end entity spi_slave;

architecture struct of spi_slave is

  signal cmd                  : std_ulogic_vector(c_cmd_bits-1 downto 0);
  signal cmd_rdy              : std_ulogic;
  signal rd_cmd_end           : std_ulogic;
  signal read                 : std_ulogic;
  signal wr_cmd_end           : std_ulogic;
  signal write                : std_ulogic;
  signal cmd_crc_internal     : std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
  signal nr_of_bytes_internal : std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);

  component spi_send_echo
  generic (
    g_addr_bits : integer := 8;
    g_data_bits : integer := 8
  );
  port (
    cmd           : in  std_ulogic_vector (c_cmd_bits-1 downto 0);
    cmd_rdy       : in  std_ulogic ;
    cs_n          : in  std_ulogic ;
    echo_crc      : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    rd_cmd_end    : in  std_ulogic ;
    rd_data       : in  std_ulogic_vector (g_data_bits-1 downto 0);
    read          : in  std_ulogic ;
    rst_n         : in  std_ulogic ;
    sck           : in  std_ulogic ;
    status        : in  std_ulogic_vector (c_cmd_status_bits-1 downto 0);
    wr_cmd_end    : in  std_ulogic ;
    rd_req        : out std_ulogic ;
    so            : out std_ulogic ;
    update_status : out std_ulogic 
  );
  end component spi_send_echo;
  component spi_store_cmd
  generic (
    g_addr_bits : integer := 8
  );
  port (
    cs_n           : in  std_ulogic ;
    rst_n          : in  std_ulogic ;
    sck            : in  std_ulogic ;
    si             : in  std_ulogic ;
    addr_rdy       : out std_ulogic ;
    base_addr      : out std_ulogic_vector (g_addr_bits-1 downto 0);
    busy           : out std_ulogic ;
    cmd            : out std_ulogic_vector (c_cmd_bits-1 downto 0);
    cmd_crc        : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    cmd_rdy        : out std_ulogic ;
    nr_of_bytes    : out std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    rd_cmd_crc_err : out std_ulogic ;
    rd_cmd_crc_rdy : out std_ulogic ;
    rd_cmd_end     : out std_ulogic ;
    read           : out std_ulogic ;
    write          : out std_ulogic 
  );
  end component spi_store_cmd;
  component spi_store_data
  generic (
    g_data_bits : integer := 8
  );
  port (
    cmd_crc        : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    cs_n           : in  std_ulogic ;
    nr_of_bytes    : in  std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    rst_n          : in  std_ulogic ;
    sck            : in  std_ulogic ;
    si             : in  std_ulogic ;
    write          : in  std_ulogic ;
    calc_crc       : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    update_crc_1st : out std_ulogic ;
    update_crc_2nd : out std_ulogic ;
    wr_cmd_end     : out std_ulogic ;
    wr_crc         : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    wr_data        : out std_ulogic_vector (g_data_bits-1 downto 0);
    wr_req         : out std_ulogic 
  );
  end component spi_store_data;

begin
  rd_wrn      <= read;
  wr_addr_rdy <= write;                             

  i0_spi_send_echo : spi_send_echo
    generic map (
      g_addr_bits => g_addr_bits,
      g_data_bits => g_data_bits
    )
    port map (
      cmd           => cmd,
      cmd_rdy       => cmd_rdy,
      cs_n          => cs_n,
      echo_crc      => echo_crc,
      rd_cmd_end    => rd_cmd_end,
      rd_data       => rd_data,
      read          => read,
      rst_n         => rst_n,
      sck           => sck,
      status        => status,
      wr_cmd_end    => wr_cmd_end,
      rd_req        => rd_req,
      so            => so,
      update_status => update_status
    );
  i0_spi_store_cmd : spi_store_cmd
    generic map (
      g_addr_bits => g_addr_bits
    )
    port map (
      cs_n           => cs_n,
      rst_n          => rst_n,
      sck            => sck,
      si             => si,
      addr_rdy       => addr_rdy,
      base_addr      => base_addr,
      busy           => busy,
      cmd            => cmd,
      cmd_crc        => cmd_crc_internal,
      cmd_rdy        => cmd_rdy,
      nr_of_bytes    => nr_of_bytes_internal,
      rd_cmd_crc_err => rd_cmd_crc_err,
      rd_cmd_crc_rdy => rd_cmd_crc_rdy,
      rd_cmd_end     => rd_cmd_end,
      read           => read,
      write          => write
    );
  i0_spi_store_data : spi_store_data
    generic map (
      g_data_bits => g_data_bits
    )
    port map (
      cmd_crc        => cmd_crc_internal,
      cs_n           => cs_n,
      nr_of_bytes    => nr_of_bytes_internal,
      rst_n          => rst_n,
      sck            => sck,
      si             => si,
      write          => write,
      calc_crc       => calc_crc,
      update_crc_1st => update_crc_1st,
      update_crc_2nd => update_crc_2nd,
      wr_cmd_end     => wr_cmd_end,
      wr_crc         => wr_crc,
      wr_data        => wr_data,
      wr_req         => wr_req
    );

  cmd_crc     <= cmd_crc_internal;
  nr_of_bytes <= nr_of_bytes_internal;

end architecture struct;
