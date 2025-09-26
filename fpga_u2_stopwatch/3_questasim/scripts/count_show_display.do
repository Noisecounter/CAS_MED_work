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
#   reset, clock and p1khz 
force reset_n 1 0 ns, 0 20 ns, 1 40 ns
force clk     1 0 ns, 0 10 ns -r 20 ns
run 1 ns
force enable  0 0 ns, 1 80 ns -r 100 ns
force run     0 0 ns
force lap     0 0 ns
force init    0 0 ns
run 1 us

#   check start and stop
force run     1 0 ns, 0 19.2 us
run 20 us

#   check lap while running
force run     1 0 ns, 0 2200 ns
force lap     0 0 ns, 1 700 ns, 0 1200 ns
run 3 us

#   init
force init    1 0 ns, 0 20 ns
run 1 us

#   check stop while lap is displayed
force run     1 0 ns, 0 700 ns
force lap     0 0 ns, 1 400 ns, 0 700 ns
run 1 us

#   init again
force init    1 0 ns, 0 20 ns
run 1 us

echo "To simulate the whole range enter: resume"
pause

#   run again and check the whole range
force run     1 0 ns, 0 219 us
run 220 us

#   it's done
echo "Do you see the change from 22:22:22 to 00:00:00?"
