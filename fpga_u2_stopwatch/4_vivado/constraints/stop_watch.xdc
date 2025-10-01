
## Pins to use
set_property PACKAGE_PIN K17 [get_ports {clk}]

set_property PACKAGE_PIN T16 [get_ports {reset_n}]
set_property PACKAGE_PIN K18 [get_ports {start_stop}]
set_property PACKAGE_PIN P16 [get_ports {lap_init}]

set_property PACKAGE_PIN W14 [get_ports {s_digit[0]}]
set_property PACKAGE_PIN Y14 [get_ports {s_digit[1]}]
set_property PACKAGE_PIN T12 [get_ports {s_digit[2]}]
set_property PACKAGE_PIN U12 [get_ports {s_digit[3]}]
set_property PACKAGE_PIN U14 [get_ports {s_digit[4]}]
set_property PACKAGE_PIN U15 [get_ports {s_digit[5]}]
set_property PACKAGE_PIN V17 [get_ports {s_digit[6]}]

set_property PACKAGE_PIN V18 [get_ports {s_digit_sel}]

set_property PACKAGE_PIN V15 [get_ports {c_digit[0]}]
set_property PACKAGE_PIN W15 [get_ports {c_digit[1]}]
set_property PACKAGE_PIN T11 [get_ports {c_digit[2]}]
set_property PACKAGE_PIN T10 [get_ports {c_digit[3]}]
set_property PACKAGE_PIN T14 [get_ports {c_digit[4]}]
set_property PACKAGE_PIN T15 [get_ports {c_digit[5]}]
set_property PACKAGE_PIN P14 [get_ports {c_digit[6]}]

set_property PACKAGE_PIN R14 [get_ports {c_digit_sel}]

set_property PACKAGE_PIN M14 [get_ports {led0}]

## I/O standardc_digitVCMOS
set_property IOSTANDARD LVCMOS33 [get_ports *]

## Timing Constraints, 8.00 ns corresponds to 125 MHz
create_clock -add -name clk -period 8.00 [get_ports {clk}]

## Vivado treats all ports as clock synchronous. Let the tool know all ports are asynchronous
set_false_path -from [get_ports {reset_n}]
set_false_path -from [get_ports {start_stop}]
set_false_path -from [get_ports {lap_init}]
set_false_path -to [get_ports {s_digit[*]}]
## set_false_path directive simply disables all timing analysis

