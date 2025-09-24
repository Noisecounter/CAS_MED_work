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
#   init
force reset_n      1 0 ns, 0 20 ns, 1 40 ns
force clk          1 0 ns, 0 10 ns -r 20 ns
run 1 ns
force start_stop   0 0 ns
force lap_init     0 0 ns
run 5 us

#   s_stop + start_stop -> s_run
force start_stop 0 0 ns, 1 200 ns, 0 400 ns
run 5 us

#   s_run + start_stop -> s_stop
force start_stop 0 0 ns, 1 200 ns, 0 400 ns
run 5 us

#   s_stop + lap_init -> s_init -> s_stop
force lap_init   0 0 ns, 1 200 ns, 0 400 ns
run 5 us

#   s_stop + start_stop -> s_run
force start_stop 0 0 ns, 1 200 ns, 0 400 ns
run 6 us

#   s_run + lap_init -> s_meantime
force lap_init   0 0 ns, 1 200 ns, 0 400 ns
run 6 us

#   s_meantime + lap_init -> s_run
force lap_init   0 0 ns, 1 200 ns, 0 400 ns
run 6 us

#   s_run + lap_init -> s_meantime
force lap_init   0 0 ns, 1 200 ns, 0 400 ns
run 6 us

#   s_meantime + start_stop -> s_stop
force start_stop 0 0 ns, 1 200 ns, 0 400 ns
run 6 us

#   s_stop + lap_init -> s_init -> s_stop
force lap_init   0 0 ns, 1 200 ns, 0 400 ns
run 6 us

#   it's done
echo "stop_watch DONE!"
