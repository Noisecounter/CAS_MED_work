# ----------------------------------------
# Initialize variables
# if ![info exists TOP_LEVEL_NAME] { 
  set TOP_LEVEL_NAME "tb_system_top"
  set SRC "../2_hdl/system_lib/src"
  set TB  "../2_hdl/system_lib/tb"
# }

# ----------------------------------------
# Compile the design files in correct order
alias com {
  echo "\[exec\] com"
  vlib -dirpath ./work work
  vmap system_lib work
  vmap system_tb_lib work

  vcom -2008 -work work $SRC/ime_reset.vhd
  vcom -2008 -work work $SRC/my_pll.vhd
  vcom -2008 -work work $SRC/data_sync_rtl.vhd
  vcom -2008 -work work $SRC/data_calc_rtl.vhd
  vcom -2008 -work work $SRC/ram_dual.vhd
  vcom -2008 -work work $SRC/system_top_struct.vhd
  
  vcom -2008 -work work $TB/system_top_pkg.vhd
  # vcom -2008 -work work $TB/tb_verify_system_top_sim1.vhd
  vcom -2008 -work work $TB/tb_verify_system_top_sim.vhd
  vcom -2008 -work work $TB/tb_system_top_struct.vhd
}

# ----------------------------------------
# Elaborate the top level design with novopt option
alias ld {
  echo "\[exec\] elab"
  eval vsim -L work $TOP_LEVEL_NAME -voptargs=+acc
}

# ----------------------------------------
# Simulate
alias sim {
  echo "\[exec\] sim"
  do scripts/sim.do
}

# ----------------------------------------
# Compile, elaborate and run simulation
alias do_all "
  com
  ld
  sim
"

# ----------------------------------------
# Print out user commmand line aliases
alias h {
  echo "List Of Command Line Aliases"
  echo
  echo "com             -- Compile the design files in correct order"
  echo "ld              -- Elaborate the top level design with novopt option"
  echo "sim             -- Run simulation"
  echo
  echo "TOP_LEVEL_NAME  -- $TOP_LEVEL_NAME"
}
h
