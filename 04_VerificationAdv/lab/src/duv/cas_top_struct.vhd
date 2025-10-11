library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

entity cas_top is
  port( 
    rst_n    : in  std_ulogic;
    clk      : in  std_ulogic;
    spi_cs_n : in  std_ulogic;
    spi_sclk : in  std_ulogic;
    spi_si   : in  std_ulogic;
    spi_so   : out std_ulogic
  );
end entity cas_top;


architecture struct of cas_top is

  signal base_addr           : std_ulogic_vector(c_cmd_addr_bits-1 downto 0);
  signal base_addr_rdy       : std_ulogic;
  signal cmd_err_rd_addr     : integer range 0 to 2**c_cmd_addr_bits-1;
  signal cmd_opr_err_rd_addr : integer range 0 to 2**c_cmd_addr_bits-1;
  signal cmd_status          : t_cmd_status;
  signal spi_access_done     : std_ulogic;
  signal rd_data             : std_ulogic_vector(c_csr_data_bits-1 downto 0);
  signal rd_data_rdy         : std_ulogic;
  signal rd_req              : std_ulogic;
  signal reg_settings        : t_reg_settings;
  signal rx_nr_bytes         : std_ulogic_vector(c_cmd_dbn_bits-1 downto 0);
  signal wr_data             : std_ulogic_vector(c_csr_data_bits-1 downto 0);
  signal wr_req              : std_ulogic;


  component registers
  port (
    base_addr           : in  std_ulogic_vector (c_cmd_addr_bits-1 downto 0);
    base_addr_rdy       : in  std_ulogic;
    clk                 : in  std_ulogic;
    cmd_err_rd_addr     : in  integer range 0 to 2**c_cmd_addr_bits-1;
    cmd_opr_err_rd_addr : in  integer range 0 to 2**c_cmd_addr_bits-1;
    cmd_status          : in  t_cmd_status;
    spi_access_done     : in  std_ulogic;
    rd_req              : in  std_ulogic;
    rst_n               : in  std_ulogic;
    rx_nr_bytes         : in  std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    wr_data             : in  std_ulogic_vector (c_csr_data_bits-1 downto 0);
    wr_req              : in  std_ulogic;
    rd_data             : out std_ulogic_vector (c_csr_data_bits-1 downto 0);
    rd_data_rdy         : out std_ulogic;
    reg_settings        : out t_reg_settings 
  );
  end component registers;
  component serial_if
  port (
    clk                 : in  std_ulogic;
    spi_cs_n            : in  std_ulogic;
    spi_sclk            : in  std_ulogic;
    spi_si              : in  std_ulogic;
    rd_data             : in  std_ulogic_vector (c_csr_data_bits-1 downto 0);
    rd_data_rdy         : in  std_ulogic;
    reg_settings        : in  t_reg_settings ;
    rst_n               : in  std_ulogic;
    base_addr           : out std_ulogic_vector (c_cmd_addr_bits-1 downto 0);
    base_addr_rdy       : out std_ulogic;
    cmd_err_rd_addr     : out integer range 0 to 2**c_cmd_addr_bits-1;
    cmd_opr_err_rd_addr : out integer range 0 to 2**c_cmd_addr_bits-1;
    cmd_status          : out t_cmd_status ;
    spi_access_done     : out std_ulogic;
    spi_so              : out std_ulogic;
    rd_req              : out std_ulogic;
    rx_nr_bytes         : out std_ulogic_vector (c_cmd_dbn_bits-1 downto 0);
    wr_data             : out std_ulogic_vector (c_csr_data_bits-1 downto 0);
    wr_req              : out std_ulogic 
  );
  end component serial_if;

begin

  i0_registers : registers
    port map (
      base_addr           => base_addr,
      base_addr_rdy       => base_addr_rdy,
      clk                 => clk,
      cmd_err_rd_addr     => cmd_err_rd_addr,
      cmd_opr_err_rd_addr => cmd_opr_err_rd_addr,
      cmd_status          => cmd_status,
      spi_access_done     => spi_access_done,
      rd_req              => rd_req,
      rst_n               => rst_n,
      rx_nr_bytes         => rx_nr_bytes,
      wr_data             => wr_data,
      wr_req              => wr_req,
      rd_data             => rd_data,
      rd_data_rdy         => rd_data_rdy,
      reg_settings        => reg_settings
    );
  i0_serial_if : serial_if
    port map (
      clk                 => clk,
      spi_cs_n            => spi_cs_n,
      spi_sclk            => spi_sclk,
      spi_si              => spi_si,
      rd_data             => rd_data,
      rd_data_rdy         => rd_data_rdy,
      reg_settings        => reg_settings,
      rst_n               => rst_n,
      base_addr           => base_addr,
      base_addr_rdy       => base_addr_rdy,
      cmd_err_rd_addr     => cmd_err_rd_addr,
      cmd_opr_err_rd_addr => cmd_opr_err_rd_addr,
      cmd_status          => cmd_status,
      spi_access_done     => spi_access_done,
      spi_so              => spi_so,
      rd_req              => rd_req,
      rx_nr_bytes         => rx_nr_bytes,
      wr_data             => wr_data,
      wr_req              => wr_req
    );

end architecture struct;
