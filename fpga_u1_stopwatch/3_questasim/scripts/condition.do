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
#   without bouncing
force reset_n    1 0 ns, 0 20 ns, 1 40 ns
force clk        1 0 ns, 0 10 ns -r 20 ns
run 1 ns
force p1khz      0 0 ns, 1 80 ns -r 100 ns
force start_stop 0 0 ns, 1 2 us, 0 3 us
force lap_init   0 0 ns, 1 2 us, 0 3 us
run 8 us

# start_stop with bouncing while pressing the button
force start_stop 1 0 ns, 0 100 ns
run 200 ns

force start_stop 1 0 ns, 0 200 ns
run 300 ns

force start_stop 1 0 ns, 0 300 ns
run 400 ns
force start_stop 1

# lap_init with bouncing while pressing the button
force lap_init   1 0 ns, 0 100 ns
run 200 ns

force lap_init   1 0 ns, 0 200 ns
run 300 ns

force lap_init   1 0 ns, 0 300 ns
run 400 ns
force lap_init   1

force lap_init   1 0 ns
run 3 us

#   with bouncing while releasing the button
force start_stop 0 0 ns, 1 100 ns
force lap_init   0 0 ns, 1 100 ns
run 200 ns

force start_stop 0 0 ns, 1 200 ns
force lap_init   0 0 ns, 1 200 ns
run 300 ns

force start_stop 0 0 ns, 1 300 ns
force lap_init   0 0 ns, 1 300 ns
run 400 ns

force start_stop 0 0 ns
force lap_init   0 0 ns
run 4 us

#   it's done
echo "debounce DONE!"
