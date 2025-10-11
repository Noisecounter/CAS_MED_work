-------------------------------------------------------------
-- Description : implements TC-0001
--   TC-0001.1: All RD and RD/WR Register default values are read
--   TC-0001.2: All WR registers get a new value
--   TC-0001.3: All WR registers are read back
-------------------------------------------------------------

architecture tc_0001 of tb_ctrl is

  constant c_tb_tc     : string := "tc_0001";
  constant c_inst_name : string := PathTail(tc_0001'path_name);

  signal tb_ts : integer := 0;

begin

  p_logging : process
  begin
    if g_batch_mode then 
      TranscriptOpen(c_log_path & g_log_file, append_mode);
      -- SetTranscriptMirror(true);  -- print to console and file
    end if;
    wait ;
  end process p_logging;

  -------------------------------------------------------------
  p_sequence : process
    variable v_tx_nr_of_bytes : integer range 1 to 2**c_cmd_dbn_bits := 1;
    variable v_tx_addr        : std_ulogic_vector(c_cmd_addr_bits-1 downto 0) := (others => '0'); 
    ---------------------------
    variable v_tx_data        : t_spi_tx_data := (others => (others => '0'));
    variable v_echo_exp       : t_exp_spi_echo_data;
  begin
    write(output, "###-----------------------------------------------------------------------------" & lf);
    write(output, "### Start of simulation of: "&c_tb_tc & lf);
    write(output, "###-----------------------------------------------------------------------------" & lf);

    SetAlertLogName(c_tb_tc);
    SetAlertLogOptions(LogPrefix    => c_log_prefix);
    SetAlertLogOptions(AlertPrefix  => c_alrt_prefix);
    SetAlertLogOptions(ReportPrefix => c_rpt_prefix);
    if not(g_batch_mode) then 
      SetAlertStopCount(GetAlertLogID(c_inst_name), error, 1);  -- stop after 1 alert error
    end if;
    
    clk_ctrl            <= c_clk_ctrl_init;
    clk_ctrl.clk_enable <= true;
    wait for 0 ns;

   	wait until rising_edge(rst_n); 
    ClearAlerts;
    
    ---------------------------------------------------------------------------    
    -- TC-0001.1: Read register 0x000..0x007 by SPI (default-values)
    ---------------------------------------------------------------------------    
    tb_ts <= 1;
    wait for 0 ns;
    write(output,"###" & lf);
    write(output,"### TS = " & to_string(tb_ts)&"                                                            at " & to_string(now) & lf);

    v_tx_nr_of_bytes := 8;
    v_tx_addr        := std_ulogic_vector(to_unsigned(0, v_tx_addr'LENGTH));  
    
    v_echo_exp.status     := (others => '0');
    v_echo_exp.reg_data   := (others => (others => '0'));
    v_echo_exp.check_echo := true;

    v_echo_exp.reg_data(0) := X"A5";  -- reg 0x000 = version
    v_echo_exp.reg_data(1) := X"00";  -- reg 0x001
    v_echo_exp.reg_data(2) := X"00";  -- reg 0x002
    v_echo_exp.reg_data(3) := X"00";  -- reg 0x003
    v_echo_exp.reg_data(4) := X"00";  -- reg 0x004
    v_echo_exp.reg_data(5) := X"00";  -- reg 0x005
    v_echo_exp.reg_data(6) := X"EF";  -- reg 0x006
    v_echo_exp.reg_data(7) := X"00";  -- reg 0x007
    
    spi_read(sclk_period  => 250 ns,   -- 4 Mbit/s
             addr         => v_tx_addr,
             data_nr      => std_ulogic_vector(to_unsigned(v_tx_nr_of_bytes-1, c_cmd_dbn_bits)),
             echo_exp     => v_echo_exp,
             batch_mode   => g_batch_mode,
             spi_rx_ack_n => spi_cmd_ack_n,
             spi_rx_ctrl  => spi_model_ctrl);

    wait for 100*c_clk_period;
    ---------------------------------------------------------------------------    
    -- Read register 0x008..0x00F by SPI (default-values)
    ---------------------------------------------------------------------------    
    v_tx_nr_of_bytes := 8;
    v_tx_addr        := std_ulogic_vector(to_unsigned(8, v_tx_addr'LENGTH));  
    
    v_echo_exp.status     := (others => '0');
    v_echo_exp.reg_data   := (others => (others => '0'));
    v_echo_exp.check_echo := true;

    v_echo_exp.reg_data(0) := X"0B";  -- reg 0x008
    v_echo_exp.reg_data(1) := X"10";  -- reg 0x009
    v_echo_exp.reg_data(2) := X"FF";  -- reg 0x00A
    v_echo_exp.reg_data(3) := X"00";  -- reg 0x00B
    v_echo_exp.reg_data(4) := X"00";  -- reg 0x00C
    v_echo_exp.reg_data(5) := X"00";  -- reg 0x00D
    v_echo_exp.reg_data(6) := X"00";  -- reg 0x00E
    v_echo_exp.reg_data(7) := X"00";  -- reg 0x00F
    
    spi_read(sclk_period  => 250 ns,   -- 4 Mbit/s
             addr         => v_tx_addr,
             data_nr      => std_ulogic_vector(to_unsigned(v_tx_nr_of_bytes-1, c_cmd_dbn_bits)),
             echo_exp     => v_echo_exp,
             batch_mode   => g_batch_mode,
             spi_rx_ack_n => spi_cmd_ack_n,
             spi_rx_ctrl  => spi_model_ctrl);

    wait for 100*c_clk_period;
    ---------------------------------------------------------------------------    
    ---------------------------------------------------------------------------    
    -- TC-0001.2: Write register 0x001..0x008 by SPI
    ---------------------------------------------------------------------------    
    tb_ts <= 2;
    wait for 0 ns;
    write(output,"###" & lf);
    write(output,"### TS = " & to_string(tb_ts)&"                                                            at " & to_string(now) & lf);

    v_tx_nr_of_bytes := 8;
    v_tx_addr        := std_ulogic_vector(to_unsigned(1, v_tx_addr'LENGTH));  

    v_tx_data        := (others => (others => '0'));
    v_tx_data(0)     := X"10";  
    v_tx_data(1)     := X"21";  
    v_tx_data(2)     := X"32";
    v_tx_data(3)     := X"43";
    v_tx_data(4)     := X"54";
    v_tx_data(5)     := X"65";
    v_tx_data(6)     := X"76";
    v_tx_data(7)     := X"87";
    v_echo_exp.status     := (others => '0');
    v_echo_exp.reg_data   := (others => (others => '0'));
    v_echo_exp.check_echo := true;

    spi_write(sclk_period  => 250 ns,   -- 4 mbit/s
              addr         => v_tx_addr,
              data         => v_tx_data,
              data_nr      => std_ulogic_vector(to_unsigned(v_tx_nr_of_bytes-1, c_cmd_dbn_bits)),
              echo_exp     => v_echo_exp,
              batch_mode   => g_batch_mode,
              spi_tx_ack_n => spi_cmd_ack_n,
              spi_tx_ctrl  => spi_model_ctrl);

    wait for 100*c_clk_period;
    ---------------------------------------------------------------------------    
    -- Write register 0x009..0x00F by SPI
    ---------------------------------------------------------------------------    
    v_tx_nr_of_bytes := 7;
    v_tx_addr        := std_ulogic_vector(to_unsigned(9, v_tx_addr'LENGTH));  

    v_tx_data        := (others => (others => '0'));
    v_tx_data(0)     := X"A9";
    v_tx_data(1)     := X"BA";
    v_tx_data(2)     := X"CB";
    v_tx_data(3)     := X"DC";
    v_tx_data(4)     := X"ED";
    v_tx_data(5)     := X"FE";
    v_tx_data(6)     := X"0F";

    v_echo_exp.status     := (others => '0');
    v_echo_exp.reg_data   := (others => (others => '0'));
    v_echo_exp.check_echo := true;

    spi_write(sclk_period  => 250 ns,   -- 4 mbit/s
              addr         => v_tx_addr,
              data         => v_tx_data,
              data_nr      => std_ulogic_vector(to_unsigned(v_tx_nr_of_bytes-1, c_cmd_dbn_bits)),
              echo_exp     => v_echo_exp,
              batch_mode   => g_batch_mode,
              spi_tx_ack_n => spi_cmd_ack_n,
              spi_tx_ctrl  => spi_model_ctrl);

    wait for 100*c_clk_period;
    ---------------------------------------------------------------------------    
    -- TC-0001.3:  Read register 0x001..0x008 by SPI 
    ---------------------------------------------------------------------------    
    tb_ts <= 3;
    wait for 0 ns;
    write(output,"###" & lf);
    write(output,"### TS = " & to_string(tb_ts)&"                                                            at " & to_string(now) & lf);

    v_tx_nr_of_bytes := 8;
    v_tx_addr        := std_ulogic_vector(to_unsigned(1, v_tx_addr'LENGTH));  
    
    v_echo_exp.status     := (others => '0');
    v_echo_exp.reg_data   := (others => (others => '0'));
    v_echo_exp.check_echo := true;

    v_echo_exp.reg_data(0) := X"10";  -- reg 0x001
    v_echo_exp.reg_data(1) := X"21";  -- reg 0x002
    v_echo_exp.reg_data(2) := X"32";  -- reg 0x003
    v_echo_exp.reg_data(3) := X"43";  -- reg 0x004
    v_echo_exp.reg_data(4) := X"54";  -- reg 0x005
    v_echo_exp.reg_data(5) := X"65";  -- reg 0x006
    v_echo_exp.reg_data(6) := X"76";  -- reg 0x007
    v_echo_exp.reg_data(7) := X"87";  -- reg 0x008
    
    spi_read(sclk_period  => 250 ns,   -- 4 Mbit/s
             addr         => v_tx_addr,
             data_nr      => std_ulogic_vector(to_unsigned(v_tx_nr_of_bytes-1, c_cmd_dbn_bits)),
             echo_exp     => v_echo_exp,
             batch_mode   => g_batch_mode,
             spi_rx_ack_n => spi_cmd_ack_n,
             spi_rx_ctrl  => spi_model_ctrl);

    wait for 100*c_clk_period;


    ---------------------------------------------------------------------------    
    -- Read register 0x009..0x00F by SPI 
    ---------------------------------------------------------------------------    
    
    v_tx_nr_of_bytes := 8;
    v_tx_addr        := std_ulogic_vector(to_unsigned(9, v_tx_addr'LENGTH));  
    
    v_echo_exp.status     := (others => '0');
    v_echo_exp.reg_data   := (others => (others => '0'));
    v_echo_exp.check_echo := true;

    v_echo_exp.reg_data(0) := X"A9";  -- reg 0x009
    v_echo_exp.reg_data(1) := X"BA";  -- reg 0x00A
    v_echo_exp.reg_data(2) := X"CB";  -- reg 0x00B
    v_echo_exp.reg_data(3) := X"DC";  -- reg 0x00C
    v_echo_exp.reg_data(4) := X"ED";  -- reg 0x00D
    v_echo_exp.reg_data(5) := X"FE";  -- reg 0x00E
    v_echo_exp.reg_data(6) := X"0F";  -- reg 0x00F
    --v_echo_exp.reg_data(7) := X"00";  -- reg 0x00F
    
    spi_read(sclk_period  => 250 ns,   -- 4 Mbit/s
             addr         => v_tx_addr,
             data_nr      => std_ulogic_vector(to_unsigned(v_tx_nr_of_bytes-1, c_cmd_dbn_bits)),
             echo_exp     => v_echo_exp,
             batch_mode   => g_batch_mode,
             spi_rx_ack_n => spi_cmd_ack_n,
             spi_rx_ctrl  => spi_model_ctrl);

    wait for 100*c_clk_period;

    ---------------------------------------------------------------------------    
    -- End of simulation
    ---------------------------------------------------------------------------    
    wait for 100*c_clk_period;
    wait until rising_edge(clk);
    clk_ctrl.clk_enable <= false;
            
    write(output, "###" & lf);
    write(output, "### End of simulation" & lf);
    write(output, "### Summary: " & lf);
    SetTranscriptMirror(true);  -- print to console and file: report in terminal when running batch
    ReportAlerts;

    wait for 10 ns;
    wait;
    
  end process p_sequence;
  ---------------------------------------------------------------------------    


end architecture tc_0001;

