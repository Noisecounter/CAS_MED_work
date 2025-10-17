// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
// (c) Copyright(C) 2013 - 2022 by Xilinx, Inc. All rights reserved.
//
// This file contains confidential and proprietary information
// of Xilinx, Inc. and is protected under U.S. and
// international copyright and other intellectual property
// laws.
//
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
//
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
//
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.

#pragma once

#include <tlm.h>
#include <systemc.h>
#include "msm.h"

using namespace msm_core;
using namespace msm_trace;
namespace msm_adapters
{
template<class T>
class sc2msm_fifo_in_port : public sc_core::sc_module
{
    SC_HAS_PROCESS(sc2msm_fifo_in_port);
public:
    sc2msm_fifo_in_port(msm_core::msm_module_name name, sc_core::sc_module_name sc_name) :
        sc_module(sc_name), msm_output("msm_output"), sc_input("sc_input"),
        clk("clk"),  msm_fifo_channel(name)
    {
        sc_core::sc_object* ptr = this;
	    
		SC_METHOD(sc2msm_updater);
        sensitive << sc_input;
		
		SC_METHOD(ready_updater);
        sensitive << clk.pos();
        dont_initialize();

		sc_input(fifo_open);
	    msm_output(msm_fifo_channel);
	}
    virtual ~sc2msm_fifo_in_port() = default;
    bool data_driven{};
	msm_fifo_in<T> msm_output; //!<MSM Socket (to be used with MSM AXIS payload)
    msm_fifo<T> msm_fifo_channel;
	std::shared_ptr<T> data_ptr;
	T sc_read;
	sc_core::sc_in<bool> clk;
	sc_core::sc_fifo_in<T> sc_input; //!<AXIS XTLM Socket for SystemC
	sc_core::sc_fifo<T> fifo_open;
private:
	    /**
     * @brief Method to read SystemC data and update on to the msm_signal
     */
    void sc2msm_updater()
    {
        if ( sc_input.num_available() && msm_output.num_available() ) { 
	    	sc_input->nb_read(sc_read);
			std::shared_ptr<T> data_ptr = std::make_shared<T>();
			*data_ptr = sc_read;
			msm_fifo_channel.nb_write(data_ptr); //Null checking of m_msm_intf is done before start of the sim
        	msm_core::sync_update();
        	DEBUG_MSM_ADAPTER( this->name() << " read sc_in port and updated MSM interface to " \
        	        << sc_read <<" @ " << sc_core::sc_time_stamp());
			
			data_driven = true;
		} else {
			data_driven = false;
		}
    }

	void ready_updater()
	{
		
        if ( sc_input.num_available() && msm_output.num_available() && !data_driven ) { 
	    	sc_input->nb_read(sc_read);
			std::shared_ptr<T> data_ptr = std::make_shared<T>();
			*data_ptr = sc_read;
			msm_fifo_channel.nb_write(data_ptr); //Null checking of m_msm_intf is done before start of the sim
        	msm_core::sync_update();
        	DEBUG_MSM_ADAPTER( this->name() << " read sc_in port and updated MSM interface to " \
        	        << sc_read <<" @ " << sc_core::sc_time_stamp());
		}
	
	}

};

} /* namespace msm_adapters */
