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

#include "msm2sc_base.h"
#include <tlm.h>
#include <systemc.h>

namespace msm_adapters
{
template<class T>
class msm2aie_axis : public msm2sc_base, public sc_core::sc_module
{
    SC_HAS_PROCESS(msm2aie_axis);
public:
    msm2aie_axis(msm_core::msm_module_name name, sc_core::sc_module_name sc_name) :
        msm2sc_base(name), sc_module(sc_name), msm_input("msm_input"), sc_output("sc_output"),
        sc_channel("sc_channel"), clk("clk")
    {
        sc_core::sc_object* ptr = this;

        SC_THREAD(ready_updater);
        sensitive << clk.pos();
        dont_initialize();

        sc_output(sc_channel);
    }
    virtual ~msm2aie_axis() = default;

    msm_core::msm_targ_socket<T> msm_input; //!<MSM Socket (to be used with MSM AXIS payload)
    sc_core::sc_port<tlm::tlm_fifo_get_if<T>> sc_output; //!<AXIS XTLM Socket for SystemC
    sc_core::sc_in<bool> clk; //!< Clock input
private:

    bool data_driven = false; //!< Variable to know if the data is driven in the current posedge or not
    tlm::tlm_fifo<T> sc_channel;

    //Implementation of pure virtual function of indirect base msm2sc_scnc
    void msm2sc_sync_handler() override
    {
            DEBUG_MSM_ADAPTER((static_cast<msm_core::msm_object*>(this))->name() << " msm " << msm_input.is_transaction_available() << " sc_fifo " << sc_channel.nb_can_put() << " @ " << sc_core::sc_time_stamp() );
        if(msm_input.is_transaction_available() && sc_channel.nb_can_put())
        {
            auto aie_axis_payload = msm_input.sample_transaction();
                DEBUG_MSM_ADAPTER(
                        (static_cast<msm_core::msm_object*>(this))->name() << " read msm_in socket and updated SystemC interface " << " to " << *aie_axis_payload << " @ " << sc_core::sc_time_stamp());

            sc_channel.put(*aie_axis_payload);
            data_driven= true;
        }
        else
            data_driven = false;
    }

    /**
     * @brief Thread to update ready status of the socket. The thread updates after delta cycle of
     * posedge of the clock . This is needed to get the updated status of FIFO and the same needs
     * to be updated before posedge of next clock cycle.
     */
    void ready_updater()
    {
        while(true)
        {
            wait(SC_ZERO_TIME);
            msm_input.get_fw_interface()->ready_after = sc_channel.nb_can_put();
            if(msm_input.is_transaction_available() && sc_channel.nb_can_put())
            {
                auto aie_axis_payload = msm_input.sample_transaction();
                sc_channel.put(*aie_axis_payload);
                DEBUG_MSM_ADAPTER(
                        (static_cast<msm_core::msm_object*>(this))->name() << " read msm_in  after delta socket and updated SystemC interface " << " to " << *aie_axis_payload << " @ " << sc_core::sc_time_stamp());
            }
            wait();
        }
    }
};

} /* namespace msm_adapters */
