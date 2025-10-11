-------------------------------------------------------------
-- Description : implements TC-0003
--   TC0003.1: Write register 0x006 with CRC-error
--   TC0003.2: Read register 0x006
-------------------------------------------------------------

architecture tc_0003 of tb_ctrl is

  constant c_tb_tc     : string := "tc_0003";
  constant c_inst_name : string := PathTail(tc_0003'PATH_NAME);

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
    ---------------------------------------------------------------------------    
    -- TC-0003.1: Write 0x006 by SPI
    ---------------------------------------------------------------------------    
    tb_ts <= 1;
    wait for 0 ns;
    write(output,"###" & lf);
    write(output,"### ts = " & to_string(tb_ts)&"                                                            at " & to_string(now) & lf);

    v_tx_nr_of_bytes := 1;
    v_tx_addr        := std_ulogic_vector(to_unsigned(6, v_tx_addr'length));  

    v_tx_data        := (others => (others => '0'));
    v_tx_data(0)     := X"AB";

    v_echo_exp.status     := (others => '0');
    v_echo_exp.reg_data   := (others => (others => '0'));
    v_echo_exp.check_echo := true;
    
    v_echo_exp.status(c_cmd_crc_err_bit) := '1'; -- 2 = received cmd crc error

    spi_write(sclk_period  => 250 ns,   -- 4 Mbit/s
              addr         => v_tx_addr,
              data         => v_tx_data,
              set_crc_err  => true,     -- <====
              data_nr      => std_ulogic_vector(to_unsigned(v_tx_nr_of_bytes-1, c_cmd_dbn_bits)),
              echo_exp     => v_echo_exp,
              batch_mode   => g_batch_mode,
              spi_tx_ack_n => spi_cmd_ack_n,
              spi_tx_ctrl  => spi_model_ctrl);

    wait for 100*c_clk_period;
    ---------------------------------------------------------------------------    
    ---------------------------------------------------------------------------    
    -- TC-0003.2:Read back 0x006 by SPI
    ---------------------------------------------------------------------------    
    tb_ts <= 2;
    wait for 0 ns;
    write(output,"###" & lf);
    write(output,"### TS = " & to_string(tb_ts)&"                                                            at " & to_string(now) & lf);
    v_tx_nr_of_bytes := 1;
    v_tx_addr        := std_ulogic_vector(to_unsigned(6, v_tx_addr'LENGTH));  
    
    v_echo_exp.status     := (others => '0');
    v_echo_exp.reg_data   := (others => (others => '0'));
    v_echo_exp.check_echo := true;

    v_echo_exp.reg_data(0) := X"EF";  -- still old value due to crc-error
    
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


end architecture tc_0003;

