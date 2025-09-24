# create library
vlib stop_watch_lib

# compile
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/prescaler.vhd
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/condition.vhd
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/control.vhd
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/count1digit.vhd
vcom -work stop_watch_lib ../2_hdl/stop_watch_lib/src/stop_watch.vhd

