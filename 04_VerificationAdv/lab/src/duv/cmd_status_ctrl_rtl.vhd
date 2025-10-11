library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
library work;
use work.cas_func_pkg.all;
use work.cas_pkg.all;

entity cmd_status_ctrl is
  port( 
    base_addr           : in  std_ulogic_vector (c_cmd_addr_bits-1 downto 0);
    base_addr_rdy_i     : in  std_ulogic;
    clk                 : in  std_ulogic;
    early_rd_req        : in  std_ulogic;
    fifo_rd_data        : in  std_ulogic_vector (c_csr_data_bits-1 downto 0);
    spi_access_done     : in  std_ulogic;
    rd_wrn_i            : in  std_ulogic;
    reg_settings        : in  t_reg_settings;
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

-- declarations

end entity cmd_status_ctrl ;


architecture rtl of cmd_status_ctrl is
 
  signal calc_addr       : integer range 0 to 2**c_cmd_addr_bits-1;
  signal hl_cmd          : std_ulogic;
  signal base_addr_rdy_d : std_ulogic;
  signal fifo_rd_data_d  : std_ulogic_vector (c_csr_data_bits-1 downto 0);
  signal wr_req_d        : std_ulogic;

begin

  p_calc_addr: process(rst_n, clk)
  begin
    if rst_n = '0' then
      calc_addr <= 0;
    elsif rising_edge(clk) then
      if base_addr_rdy_i = '1' then 
        calc_addr <= to_integer(unsigned(base_addr));
      elsif wr_req_i = '1' or early_rd_req = '1' then
        calc_addr <= calc_addr+1;
      end if;
    end if;
  end process p_calc_addr;


  ------------------------------------------------------  
  -- cmd-error 
  --   - register does not exist
  p_cmd_error : process(rst_n, clk)
  begin
    if rst_n = '0' then
      cmd_status.cmd_error <= '0';
      cmd_err_rd_addr      <= 0;
    elsif rising_edge(clk) then
      if spi_access_done = '1' then                       -- reset
        cmd_status.cmd_error <= '0';
        cmd_err_rd_addr      <= 0;
      elsif base_addr_rdy_i = '1' then                     -- new cmd
        if unsigned(base_addr) > c_csr_end_addr then         -- not within reg address range 
--           if cmd_status.cmd_opr_error = '0'         and      -- cmd-opr-error not already set
--              cmd_status.cmd_reject    = '0'            then  -- cmd-reject not already set
          if cmd_status.cmd_opr_error = '0'         then        -- cmd-opr-error not already set
 
            cmd_status.cmd_error <= '1';
          end if;
        end if;
      end if;
    end if;
  end process p_cmd_error;  
  ------------------------------------------------------  


--   ------------------------------------------------------  
--   -- cmd-reject: 
--   p_cmd_reject : process(rst_n, clk)
--   begin
--     if rst_n = '0' then
--       cmd_status.cmd_reject <= '0';
--     elsif rising_edge(clk) then
--       if spi_access_done = '1' then
--         cmd_status.cmd_reject <= '0';
--       end if;
--     end if;
--   end process p_cmd_reject;
--   ------------------------------------------------------  


  ------------------------------------------------------  
  -- cmd-opr-error:
  --  - write of a rd-only register
  --  - read of a wr-only register
  p_cmd_opr_error : process(rst_n, clk)
  begin
    if rst_n = '0' then
      cmd_status.cmd_opr_error <= '0';
      cmd_opr_err_rd_addr      <= 0;
    elsif rising_edge(clk) then
      if spi_access_done = '1' then                     -- reset
        cmd_status.cmd_opr_error <= '0';
        cmd_opr_err_rd_addr      <= 0;
      else 
        if wr_req_i = '1' then                           -- wr-cmd
          if c_csr_config(calc_addr).reg_wr = '0' and      -- reg is not writeable
--              cmd_status.cmd_error  = '0'          and      -- cmd-error not already set
--              cmd_status.cmd_reject = '0'             then  -- cmd-reject not already set
             cmd_status.cmd_error  = '0'             then  -- cmd-error not already set

            cmd_status.cmd_opr_error <= '1';

          end if;
        elsif early_rd_req = '1' then                    -- rd-cmd
          if calc_addr <= c_csr_end_addr         then    -- within reg address range
            if c_csr_config(calc_addr).reg_rd = '0'  and      -- reg is not readable
--                cmd_status.cmd_error  = '0'           and      -- cmd-error not already set
--                cmd_status.cmd_reject = '0'              then  -- cmd-reject not already set
               cmd_status.cmd_error  = '0'              then  -- cmd-error not already set
 
              cmd_status.cmd_opr_error <= '1';

              if cmd_status.cmd_opr_error = '0' then   -- store only the first one
                cmd_opr_err_rd_addr <= calc_addr;      -- store addr, where cmd-opr-error occurs: skip next addressea
              end if;
 
            end if;
          end if;
        end if;
      end if;
    end if;
  end process p_cmd_opr_error;  
  ------------------------------------------------------  


  ------------------------------------------------------  
  p_delay : process(rst_n, clk)
  begin
    if rst_n = '0' then
      base_addr_rdy_d <= '0';
      fifo_rd_data_d  <= (others => '0');
      wr_req_d        <= '0';
    elsif rising_edge(clk) then
      base_addr_rdy_d <= base_addr_rdy_i;
      fifo_rd_data_d  <= fifo_rd_data;
      wr_req_d        <= wr_req_i;
    end if;
  end process p_delay;  


  base_addr_rdy <= base_addr_rdy_d; 
  wr_req        <= wr_req_d;
  wr_data       <= fifo_rd_data_d;
  ------------------------------------------------------  


end architecture rtl;

