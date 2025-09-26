#Pinout
set_property PACKAGE_PIN K17 [get_ports {clk}];
set_property PACKAGE_PIN T16 [get_ports {reset_n}];
set_property PACKAGE_PIN K18 [get_ports {start_stop}];
set_property PACKAGE_PIN P16 [get_ports {lap_init}];
set_property PACKAGE_PIN W14 [get_ports {sec_digit[0]}];
set_property PACKAGE_PIN Y14 [get_ports {sec_digit[1]}];
set_property PACKAGE_PIN T12 [get_ports {sec_digit[2]}];
set_property PACKAGE_PIN U12 [get_ports {sec_digit[3]}];
set_property PACKAGE_PIN U14 [get_ports {sec_digit[4]}];
set_property PACKAGE_PIN U15 [get_ports {sec_digit[5]}];
set_property PACKAGE_PIN V17 [get_ports {sec_digit[6]}];

#IO standard
set_property IOSTANDARD LVCMOS33 [get_ports *];

#Clock frequency
create_clock -name clk -add -period 8.00 [get_ports { clk }];

#I/O Timings
set_false_path -from [get_ports {reset_n}]
set_false_path -from [get_ports {start_stop}]
set_false_path -from [get_ports {lap_init}]
set_false_path -to   [get_ports {sec_digit[*]}]

