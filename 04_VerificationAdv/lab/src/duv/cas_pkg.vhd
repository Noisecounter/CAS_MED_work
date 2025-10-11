library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;

package cas_pkg is

  ------------------------------------------------------------------------------
  -- global constant declarations
  ------------------------------------------------------------------------------
  constant c_clk_freq       : integer := 15360000;  -- 15.36 mhz
  constant c_csr_addr_bits  : integer :=  15;
  constant c_csr_data_bits  : integer :=   8;
  constant c_version        : std_ulogic_vector(c_csr_data_bits-1 downto 0) := x"A5";
  ------------------------------------------------------------------------------
  
  ------------------------------------------------------------------------------
  -- asic constant declarations
  ------------------------------------------------------------------------------
  -- register
  ------------------------------------------------------------------------------   
  constant c_csr_start_addr : integer :=  0;
  constant c_csr_end_addr   : integer := 15; 
  
  type t_mem is array (c_csr_start_addr to c_csr_end_addr) of std_ulogic_vector(c_csr_data_bits-1 downto 0);  -- for registers
  
  constant c_addr_version   : integer := 16#000#;  -- read-only
  constant c_addr_001       : integer := 16#001#;
  constant c_addr_002       : integer := 16#002#;
  constant c_addr_003       : integer := 16#003#;
  constant c_addr_004       : integer := 16#004#;
  constant c_addr_005       : integer := 16#005#;
  constant c_addr_006       : integer := 16#006#;
  constant c_addr_007       : integer := 16#007#;
  constant c_addr_008       : integer := 16#008#;
  constant c_addr_009       : integer := 16#009#;
  constant c_addr_00A       : integer := 16#00A#;
  constant c_addr_00B       : integer := 16#00B#;
  constant c_addr_00C       : integer := 16#00C#;
  constant c_addr_00D       : integer := 16#00D#;
  constant c_addr_00E       : integer := 16#00E#;
  constant c_addr_00F       : integer := 16#00F#;

  type t_csr_property is record
    reg_wr    : std_ulogic;                                     -- is writable
    reg_rd    : std_ulogic;                                     -- is readable
    reset_val : std_ulogic_vector(c_csr_data_bits-1 downto 0);  -- reset/power-up value
  end record;
  
  type t_csr_property_array is array (c_csr_start_addr to c_csr_end_addr) of t_csr_property;
  
  -- wr     : writable
  -- rd     : readable
  -- rst-val: reset/power-up register value
  constant c_csr_config : t_csr_property_array := 
  -- addr                wr    rd    rst-val
    (c_addr_version => ('0',   '1',  c_version),
        c_addr_001     => ('1',   '1',  x"00"),
        c_addr_002     => ('1',   '1',  X"00"),
        c_addr_003     => ('1',   '1',  X"00"),
        c_addr_004     => ('1',   '1',  X"00"),
        c_addr_005     => ('1',   '1',  X"00"),
        c_addr_006     => ('1',   '1',  X"EF"),
        c_addr_007     => ('1',   '1',  X"00"),
        c_addr_008     => ('1',   '1',  X"0B"),
        c_addr_009     => ('1',   '1',  X"10"),
        c_addr_00A     => ('1',   '1',  X"FF"),
        c_addr_00B     => ('1',   '1',  X"00"),
        c_addr_00C     => ('1',   '1',  X"00"),
        c_addr_00D     => ('1',   '1',  X"00"),
        c_addr_00E     => ('1',   '1',  X"00"),
        c_addr_00F     => ('1',   '1',  X"00"));
  ------------------------------------------------------------------------------

  
  ------------------------------------------------------------------------------
  -- SPI interface
  ---------------------------------
  constant c_cmd_bits              : integer := 16;
  constant c_cmd_status_bits       : integer := 8;
  constant c_cmd_addr_bits         : integer := 12;  -- size of base-addr-bits field within command
  constant c_cmd_dbn_bits          : integer := 3;   -- size of data-bytes field within command    
  constant c_cmd_crc_bits          : integer := 16;

  constant c_frame_fifo_depth      : integer := 2**c_cmd_dbn_bits;  -- to store received data until crc is checked
  constant c_frame_fifo_level_bits : integer := c_cmd_dbn_bits;

  constant c_write                 : std_ulogic := '0';
  constant c_read                  : std_ulogic := '1';
  
  -- status-word (within echo)
  ----------------------------
  constant c_cmd_op_err_bit        : integer := 0;  -- received cmd operation error i.e. writing to read-only-reg
  constant c_cmd_err_bit           : integer := 1;  -- received cmd unknown command or not supported
  constant c_cmd_crc_err_bit       : integer := 2;  -- received cmd frame has a checksum error

  -- protocol-errors
  ------------------
  type t_cmd_status is record
    cmd_error     : std_ulogic;
    cmd_opr_error : std_ulogic;
  end record;  

  -- signals from registers (control and config)
  ----------------------------------------------
  type t_reg_settings is record
     reg_000_settings : std_ulogic_vector(7 downto 0);
     reg_001_settings : std_ulogic_vector(7 downto 0);
     reg_002_settings : std_ulogic_vector(7 downto 0);
     reg_003_settings : std_ulogic_vector(7 downto 0);
     reg_004_settings : std_ulogic_vector(7 downto 0);
     reg_005_settings : std_ulogic_vector(7 downto 0);
     reg_006_settings : std_ulogic_vector(7 downto 0);
     reg_007_settings : std_ulogic_vector(7 downto 0);
     reg_008_settings : std_ulogic_vector(7 downto 0);
     reg_009_settings : std_ulogic_vector(7 downto 0);
     reg_00A_settings : std_ulogic_vector(7 downto 0);
     reg_00B_settings : std_ulogic_vector(7 downto 0);
     reg_00C_settings : std_ulogic_vector(7 downto 0);
     reg_00D_settings : std_ulogic_vector(7 downto 0);
     reg_00E_settings : std_ulogic_vector(7 downto 0);
     reg_00F_settings : std_ulogic_vector(7 downto 0);
  end record;
    
end package cas_pkg;

-------------------------------------------------------------

package body cas_pkg is
end package body cas_pkg;
