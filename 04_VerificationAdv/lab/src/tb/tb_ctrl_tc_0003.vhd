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





end architecture tc_0003;

