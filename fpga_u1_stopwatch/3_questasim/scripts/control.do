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
#   reset, clock and enable 
force reset_n      1 0 ns, 0 20 ns, 1 40 ns
force clk          1 0 ns, 0 10 ns -r 20 ns
run 1 ns
force start_stop_p 0 0 ns
force lap_init_p 0 0 ns
run 1 us

#   s_stop + start_stop_p -> s_run
force start_stop_p 0 0 ns, 1 20 ns, 0 40 ns
run 1 us

#   s_run + start_stop_p -> s_stop
force start_stop_p 0 0 ns, 1 20 ns, 0 40 ns
run 1 us

#   s_stop + lap_init_p -> s_init -> s_stop
force lap_init_p   0 0 ns, 1 20 ns, 0 40 ns
run 1 us

#   s_stop + start_stop_p -> s_run
force start_stop_p 0 0 ns, 1 20 ns, 0 40 ns
run 1 us

#   s_run + lap_init_p -> s_meantime
force lap_init_p   0 0 ns, 1 20 ns, 0 40 ns
run 1 us

#   s_meantime + lap_init_p -> s_run
force lap_init_p   0 0 ns, 1 20 ns, 0 40 ns
run 1 us

#   s_run + lap_init_p -> s_meantime
force lap_init_p   0 0 ns, 1 20 ns, 0 40 ns
run 1 us

#   s_meantime + start_stop_p -> s_stop
force start_stop_p 0 0 ns, 1 20 ns, 0 40 ns
run 1 us

#   s_stop + lap_init_p -> s_init -> s_stop
force lap_init_p   0 0 ns, 1 20 ns, 0 40 ns
run 1 us

#   it's done
echo "control_stopwatch DONE!"
