-----------------------------------------------------
-- Copyright (c) FHNW 2021
-----------------------------------------------------

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
force reset_n 1 0 ns, 0 20 ns, 1 40 ns
force clk     1 0 ns, 0 10 ns -r 20 ns
run 8 us

#   it's done
echo "prescaler DONE!"
