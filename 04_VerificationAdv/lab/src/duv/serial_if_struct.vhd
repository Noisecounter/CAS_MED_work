library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

entity serial_if is
  port( 
    clk                 : in  std_ulogic;
    spi_cs_n            : in  std_ulogic;
    spi_sclk            : in  std_ulogic;
    spi_si              : in  std_ulogic;
    rd_data             : in  std_ulogic_vector (c_csr_data_bits-1 downto 0);
    rd_data_rdy         : in  std_ulogic;
    reg_settings        : in  t_reg_settings;
    rst_n               : in  std_ulogic;
    base_addr           : out std_ulogic_vector (c_cmd_addr_bits-1 downto 0);
    base_addr_rdy       : out std_ulogic;
    cmd_err_rd_addr     : out integer range 0 to 2**c_cmd_addr_bits-1;
    cmd_opr_err_rd_addr : out integer range 0 to 2**c_cmd_addr_bits-1;
    cmd_status          : out t_cmd_status;
    spi_access_done     : out std_ulogic;
    spi_so              : out std_ulogic;
    rd_req              : out std_ulogic;
    rx_nr_bytes         : out std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    wr_data             : out std_ulogic_vector (c_csr_data_bits-1 downto 0);
    wr_req              : out std_ulogic
  );
end entity serial_if;


architecture struct of serial_if is

  signal base_addr_rdy_i             : std_ulogic;
  signal clear_fifo                  : std_ulogic;
  signal data_rd_req                 : std_ulogic;
  signal data_wr                     : std_ulogic_vector(c_csr_data_bits-1 downto 0);
  signal data_wr_req                 : std_ulogic;
  signal early_rd_req                : std_ulogic;
  signal fifo_rd_data                : std_ulogic_vector(c_csr_data_bits-1 downto 0);
  signal frame_fifo_level            : unsigned(c_frame_fifo_level_bits-1 downto 0);
  signal rd_wrn_i                    : std_ulogic;
  signal spi_addr_rdy                : std_ulogic;
  signal spi_early_rd_req            : std_ulogic;
  signal spi_frame_calc_crc          : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);
  signal spi_frame_clear_fifo        : std_ulogic;
  signal spi_frame_data_wr           : std_ulogic_vector(c_csr_data_bits-1 downto 0);
  signal spi_frame_done              : std_ulogic;
  signal spi_frame_fifo_rd_req       : std_ulogic;
  signal spi_frame_fifo_wr_req       : std_ulogic;
  signal spi_frame_update_tx_crc_1st : std_ulogic;
  signal spi_frame_update_tx_crc_2nd : std_ulogic;
  signal spi_frame_wr_crc            : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);
  signal spi_rd_data                 : std_ulogic_vector(c_csr_data_bits-1 downto 0);
  signal spi_rd_data_rdy             : std_ulogic;
  signal spi_rd_req                  : std_ulogic;
  signal spi_rd_wrn                  : std_ulogic;
  signal spi_slave_addr_rdy          : std_ulogic;
  signal spi_slave_base_addr         : std_ulogic_vector(c_cmd_addr_bits-1 downto 0);
  signal spi_slave_busy              : std_ulogic;
  signal spi_slave_cmd_crc           : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);
  signal spi_slave_echo_crc          : std_ulogic_vector(c_cmd_crc_bits-1 downto 0);
  signal spi_slave_nr_bytes          : std_ulogic_vector(c_cmd_dbn_bits-1 downto 0);
  signal spi_slave_rd_cmd_crc_err    : std_ulogic;
  signal spi_slave_rd_cmd_crc_rdy    : std_ulogic;
  signal spi_slave_rd_data           : std_ulogic_vector(c_csr_data_bits-1 downto 0);
  signal spi_slave_rd_req            : std_ulogic;
  signal spi_slave_rd_wrn            : std_ulogic;
  signal spi_slave_status            : std_ulogic_vector(c_cmd_status_bits-1 downto 0);
  signal spi_slave_update_status     : std_ulogic;
  signal spi_slave_wr_addr_rdy       : std_ulogic;
  signal spi_slave_wr_req            : std_ulogic;
  signal spi_wr_req                  : std_ulogic;
  signal wr_req_i                    : std_ulogic;
  signal base_addr_internal          : std_ulogic_vector (c_cmd_addr_bits-1 downto 0);
  signal cmd_status_internal         : t_cmd_status;
  signal spi_access_done_internal    : std_ulogic;
  signal rx_nr_bytes_internal        : std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);

  component cmd_status_ctrl
  port (
    base_addr           : in  std_ulogic_vector (c_cmd_addr_bits-1 downto 0);
    base_addr_rdy_i     : in  std_ulogic;
    clk                 : in  std_ulogic;
    early_rd_req        : in  std_ulogic;
    fifo_rd_data        : in  std_ulogic_vector (c_csr_data_bits-1 downto 0);
    spi_access_done     : in  std_ulogic;
    rd_wrn_i            : in  std_ulogic;
    reg_settings        : in  t_reg_settings ;
    rst_n               : in  std_ulogic;
    rx_nr_bytes         : in  std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    wr_req_i            : in  std_ulogic;
    base_addr_rdy       : out std_ulogic;
    cmd_err_rd_addr     : out integer range 0 to 2**c_cmd_addr_bits-1;
    cmd_opr_err_rd_addr : out integer range 0 to 2**c_cmd_addr_bits-1;
    cmd_status          : out t_cmd_status;
    wr_data             : out std_ulogic_vector (c_csr_data_bits-1 downto 0);
    wr_req              : out std_ulogic 
  );
  end component cmd_status_ctrl;
  component rx_frame_fifo
  port (
    clear_fifo       : in  std_ulogic;
    clk              : in  std_ulogic;
    data_rd_req      : in  std_ulogic;
    data_wr          : in  std_ulogic_vector (c_csr_data_bits-1 downto 0);
    data_wr_req      : in  std_ulogic;
    rst_n            : in  std_ulogic;
    fifo_rd_data     : out std_ulogic_vector (c_csr_data_bits-1 downto 0);
    frame_fifo_level : out unsigned (c_frame_fifo_level_bits-1 downto 0)
  );
  end component rx_frame_fifo;
  component spi_com
  port (
    clk                         : in  std_ulogic;
    cmd_status                  : in  t_cmd_status;
    frame_fifo_level            : in  unsigned (c_frame_fifo_level_bits-1 downto 0);
    reg_settings                : in  t_reg_settings;
    rst_n                       : in  std_ulogic;
    spi_frame_calc_crc          : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    spi_frame_update_tx_crc_1st : in  std_ulogic;
    spi_frame_update_tx_crc_2nd : in  std_ulogic;
    spi_frame_wr_crc            : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    spi_rd_data                 : in  std_ulogic_vector (c_csr_data_bits-1 downto 0);
    spi_rd_data_rdy             : in  std_ulogic;
    spi_slave_addr_rdy          : in  std_ulogic;
    spi_slave_base_addr         : in  std_ulogic_vector (c_cmd_addr_bits-1 downto 0);
    spi_slave_busy              : in  std_ulogic;
    spi_slave_cmd_crc           : in  std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    spi_slave_nr_bytes          : in  std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    spi_slave_rd_cmd_crc_err    : in  std_ulogic;
    spi_slave_rd_cmd_crc_rdy    : in  std_ulogic;
    spi_slave_rd_req            : in  std_ulogic;
    spi_slave_rd_wrn            : in  std_ulogic;
    spi_slave_update_status     : in  std_ulogic;
    spi_slave_wr_addr_rdy       : in  std_ulogic;
    spi_slave_wr_req            : in  std_ulogic;
    spi_addr_rdy                : out std_ulogic;
    spi_early_rd_req            : out std_ulogic;
    spi_frame_clear_fifo        : out std_ulogic;
    spi_frame_done              : out std_ulogic;
    spi_frame_fifo_rd_req       : out std_ulogic;
    spi_frame_fifo_wr_req       : out std_ulogic;
    spi_rd_req                  : out std_ulogic;
    spi_rd_wrn                  : out std_ulogic;
    spi_slave_echo_crc          : out std_ulogic_vector (c_cmd_crc_bits-1 downto 0);
    spi_slave_rd_data           : out std_ulogic_vector (c_csr_data_bits-1 downto 0);
    spi_slave_status            : out std_ulogic_vector (c_cmd_status_bits-1 downto 0);
    spi_wr_req                  : out std_ulogic 
  );
  end component spi_com;
  component spi_slave
  generic (
    g_addr_bits : integer := 8;
    g_data_bits : integer := 8
  );
  port (
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
  end component spi_slave;


begin
  spi_access_done_internal  <= spi_frame_done;
  rx_nr_bytes_internal      <= spi_slave_nr_bytes;
  data_wr_req               <= spi_frame_fifo_wr_req;
  data_wr                   <= spi_frame_data_wr;
  clear_fifo                <= spi_frame_clear_fifo;
  data_rd_req               <= spi_frame_fifo_rd_req;
  base_addr_internal        <= spi_slave_base_addr;
  base_addr_rdy_i           <= spi_addr_rdy;
  rd_wrn_i                  <= spi_rd_wrn;
  wr_req_i                  <= spi_wr_req;
  rd_req                    <= spi_rd_req;
  early_rd_req              <= spi_early_rd_req;
  spi_rd_data               <= rd_data;
  spi_rd_data_rdy           <= rd_data_rdy;

  i0_cmd_status_ctrl : cmd_status_ctrl
    port map (
      base_addr           => base_addr_internal,
      base_addr_rdy_i     => base_addr_rdy_i,
      clk                 => clk,
      early_rd_req        => early_rd_req,
      fifo_rd_data        => fifo_rd_data,
      spi_access_done     => spi_access_done_internal,
      rd_wrn_i            => rd_wrn_i,
      reg_settings        => reg_settings,
      rst_n               => rst_n,
      rx_nr_bytes         => rx_nr_bytes_internal,
      wr_req_i            => wr_req_i,
      base_addr_rdy       => base_addr_rdy,
      cmd_err_rd_addr     => cmd_err_rd_addr,
      cmd_opr_err_rd_addr => cmd_opr_err_rd_addr,
      cmd_status          => cmd_status_internal,
      wr_data             => wr_data,
      wr_req              => wr_req
    );
  i0_rx_frame_fifo : rx_frame_fifo
    port map (
      clear_fifo       => clear_fifo,
      clk              => clk,
      data_rd_req      => data_rd_req,
      data_wr          => data_wr,
      data_wr_req      => data_wr_req,
      rst_n            => rst_n,
      fifo_rd_data     => fifo_rd_data,
      frame_fifo_level => frame_fifo_level
    );
  i0_spi_com : spi_com
    port map (
      clk                         => clk,
      cmd_status                  => cmd_status_internal,
      frame_fifo_level            => frame_fifo_level,
      reg_settings                => reg_settings,
      rst_n                       => rst_n,
      spi_frame_calc_crc          => spi_frame_calc_crc,
      spi_frame_update_tx_crc_1st => spi_frame_update_tx_crc_1st,
      spi_frame_update_tx_crc_2nd => spi_frame_update_tx_crc_2nd,
      spi_frame_wr_crc            => spi_frame_wr_crc,
      spi_rd_data                 => spi_rd_data,
      spi_rd_data_rdy             => spi_rd_data_rdy,
      spi_slave_addr_rdy          => spi_slave_addr_rdy,
      spi_slave_base_addr         => spi_slave_base_addr,
      spi_slave_busy              => spi_slave_busy,
      spi_slave_cmd_crc           => spi_slave_cmd_crc,
      spi_slave_nr_bytes          => spi_slave_nr_bytes,
      spi_slave_rd_cmd_crc_err    => spi_slave_rd_cmd_crc_err,
      spi_slave_rd_cmd_crc_rdy    => spi_slave_rd_cmd_crc_rdy,
      spi_slave_rd_req            => spi_slave_rd_req,
      spi_slave_rd_wrn            => spi_slave_rd_wrn,
      spi_slave_update_status     => spi_slave_update_status,
      spi_slave_wr_addr_rdy       => spi_slave_wr_addr_rdy,
      spi_slave_wr_req            => spi_slave_wr_req,
      spi_addr_rdy                => spi_addr_rdy,
      spi_early_rd_req            => spi_early_rd_req,
      spi_frame_clear_fifo        => spi_frame_clear_fifo,
      spi_frame_done              => spi_frame_done,
      spi_frame_fifo_rd_req       => spi_frame_fifo_rd_req,
      spi_frame_fifo_wr_req       => spi_frame_fifo_wr_req,
      spi_rd_req                  => spi_rd_req,
      spi_rd_wrn                  => spi_rd_wrn,
      spi_slave_echo_crc          => spi_slave_echo_crc,
      spi_slave_rd_data           => spi_slave_rd_data,
      spi_slave_status            => spi_slave_status,
      spi_wr_req                  => spi_wr_req
    );
  i0_spi_slave : spi_slave
    generic map (
      g_addr_bits => c_cmd_addr_bits,
      g_data_bits => c_csr_data_bits
    )
    port map (
      cs_n           => spi_cs_n,
      echo_crc       => spi_slave_echo_crc,
      rd_data        => spi_slave_rd_data,
      rst_n          => rst_n,
      sck            => spi_sclk,
      si             => spi_si,
      status         => spi_slave_status,
      addr_rdy       => spi_slave_addr_rdy,
      base_addr      => spi_slave_base_addr,
      busy           => spi_slave_busy,
      calc_crc       => spi_frame_calc_crc,
      cmd_crc        => spi_slave_cmd_crc,
      nr_of_bytes    => spi_slave_nr_bytes,
      rd_cmd_crc_err => spi_slave_rd_cmd_crc_err,
      rd_cmd_crc_rdy => spi_slave_rd_cmd_crc_rdy,
      rd_req         => spi_slave_rd_req,
      rd_wrn         => spi_slave_rd_wrn,
      so             => spi_so,
      update_crc_1st => spi_frame_update_tx_crc_1st,
      update_crc_2nd => spi_frame_update_tx_crc_2nd,
      update_status  => spi_slave_update_status,
      wr_addr_rdy    => spi_slave_wr_addr_rdy,
      wr_crc         => spi_frame_wr_crc,
      wr_data        => spi_frame_data_wr,
      wr_req         => spi_slave_wr_req
    );

  base_addr        <= base_addr_internal;
  cmd_status       <= cmd_status_internal;
  spi_access_done  <= spi_access_done_internal;
  rx_nr_bytes      <= rx_nr_bytes_internal;

end architecture struct;
