library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

entity registers is
  port( 
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
end entity registers ;


architecture rtl of registers is
  signal addr           : integer range 0 to 2**12-1;
  signal mem            : t_mem;   
  signal reg_settings_i : t_reg_settings;
  signal wr_req_i       : std_ulogic;  -- write only to reg if no cmd-opr-, cmd-error or cmd-reject
begin
  wr_req_i <= wr_req when cmd_status.cmd_error     = '0' and 
                          cmd_status.cmd_opr_error = '0'    else '0';
                
  ------------------------------------------------------ 
  -- addresses
  ------------------------------------------------------  
  p_config_reg_addr : process(rst_n, clk)
  begin
    if rst_n = '0' then
      addr <= 0;
    elsif rising_edge(clk) then
      if base_addr_rdy = '1' then 
        addr <= to_integer(unsigned(base_addr));
      elsif wr_req_i = '1' or rd_req = '1' then 
        addr <= addr+1;
      end if;
    end if;
  end process p_config_reg_addr;

  ------------------------------------------------------  
  gen_store_mem : for i in c_csr_start_addr to c_csr_end_addr generate 
      p_store_mem : process(rst_n, clk)
      begin
        if rst_n = '0' then
           mem(i) <= c_csr_config(i).reset_val;
        elsif rising_edge(clk) then
          if addr <= c_csr_end_addr then                             -- addr within range
            if wr_req_i = '1' and c_csr_config(addr).reg_wr = '1' then  -- reg is writeable and cmd is ok
              if i = addr then
                mem(i) <= wr_data;
              end if;
            end if;
          end if;
        end if;
      end process p_store_mem;
  end generate gen_store_mem;
  ------------------------------------------------------  
  p_read_reg : process(rst_n, clk)
  begin
    if rst_n = '0' then
      rd_data <= (others => '0');
    elsif rising_edge(clk) then
      if rd_req = '1' then
        if addr <= c_csr_end_addr then                    -- addr within range
          if c_csr_config(addr).reg_rd = '1'                                   and      -- reg is readable
             ( cmd_status.cmd_opr_error = '0' or 
              (cmd_status.cmd_opr_error = '1' and addr < cmd_opr_err_rd_addr)) and      -- only until cmd-opr-error
             ( cmd_status.cmd_error     = '0' or 
              (cmd_status.cmd_error     = '1' and addr < cmd_err_rd_addr)    )    then  -- only until cmd-error
            rd_data <= mem(addr);
          else
            rd_data <= (others => '0');
          end if;
        else
          rd_data <= (others => '0');
        end if;
      end if;
    end if;
  end process p_read_reg;
  ------------------------------------------------------  
  p_rd_data_rdy : process(rst_n, clk)
  begin
    if rst_n = '0' then
      rd_data_rdy <= '0';
    elsif rising_edge(clk) then
      rd_data_rdy <= rd_req;
    end if;
  end process p_rd_data_rdy;
  ------------------------------------------------------  
  reg_settings <= reg_settings_i;  -- due to quartus synthesis problems

  reg_settings_i.reg_000_settings <= mem(c_addr_version);
  reg_settings_i.reg_001_settings <= mem(c_addr_001);
  reg_settings_i.reg_002_settings <= mem(c_addr_002);
  reg_settings_i.reg_003_settings <= mem(c_addr_003);
  reg_settings_i.reg_004_settings <= mem(c_addr_004);
  reg_settings_i.reg_005_settings <= mem(c_addr_005);
  reg_settings_i.reg_006_settings <= mem(c_addr_006);
  reg_settings_i.reg_007_settings <= mem(c_addr_007);
  reg_settings_i.reg_008_settings <= mem(c_addr_008);
  reg_settings_i.reg_009_settings <= mem(c_addr_009);
  reg_settings_i.reg_00A_settings <= mem(c_addr_00A);
  reg_settings_i.reg_00B_settings <= mem(c_addr_00B);
  reg_settings_i.reg_00C_settings <= mem(c_addr_00C);
  reg_settings_i.reg_00D_settings <= mem(c_addr_00D);
  reg_settings_i.reg_00E_settings <= mem(c_addr_00E);
  reg_settings_i.reg_00F_settings <= mem(c_addr_00F);
  
end architecture rtl;

