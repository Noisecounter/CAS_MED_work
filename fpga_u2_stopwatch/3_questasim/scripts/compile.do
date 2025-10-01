# Create library
vlib stop_watch_lib

# Compile files
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/stop_watch_pkg.vhd 
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/stop_watch.vhd
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/prescaler.vhd
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/condition.vhd 
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/control.vhd 
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/count_show_display.vhd
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/count1digit.vhd  
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/pmod_ssd.vhd  
 
 


