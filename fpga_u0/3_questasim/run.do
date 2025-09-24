# Create count_binary_lib library
vlib count_binary_lib

# Compile files
vcom -work count_binary_lib ../2_hdl/count_binary_lib/src/count_binary_rtl.vhd  
vcom -work count_binary_lib ../2_hdl/count_binary_lib/tb/count1_binary_verify_sim.vhd  
vcom -work count_binary_lib ../2_hdl/count_binary_lib/tb/count1_binary_tb_struct.vhd

# This is an awkward quirk of modelsim: By default designs get optimized and many signals are then
# not visible. With the vopt call, this can be fixed.
vopt -debugdb -work count_binary_lib -o count1_binary_tb_debug count_binary_lib.count1_binary_tb

# Run simulation
vsim count_binary_lib.count1_binary_tb_debug
