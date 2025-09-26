#-----------------------------------------------------
# Copyright (c) FHNW 2021
#-----------------------------------------------------

# restart the simulation 
restart -f -nowave

# Add signals & variables to the wave window
#-----------------------------------------------------
add wave -divider Inputs
add wave -in *

add wave -divider Internals
add wave -internal *

add wave -divider Outputs
add wave -out *

# Force inputs
#-----------------------------------------------------
#   reset, clock 
force reset_n 1 0 ns, 0 20 ns, 1 40 ns
force clk     1 0 ns, 0 4 ns -r 8 ns
run 1 ns

#Apply default input
force digit_lo 7'h01 0
force digit_hi 7'h10 0
run 0.2 us

#change lo input
force digit_lo 7'h02 0
run 0.2 us

#change hi input
force digit_hi 7'h20 0
run 0.2 us

#   it's done
echo "pmod_ssd DONE!"
