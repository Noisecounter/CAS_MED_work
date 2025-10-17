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

#include "sc2msm_base.h"

namespace msm_adapters
{

template <class T >
class aie_axis2msm_init : public sc2msm_base
{
    SC_HAS_PROCESS(aie_axis2msm_init);
public:
    aie_axis2msm_init(sc_core::sc_module_name name) :
        sc2msm_base(name), sc_input("sc_input"), msm_output("msm_output"),
        m_msm_intf(nullptr), clk("clk")
    {
        SC_THREAD(sc2msm_updater);

        SC_METHOD(dirty_cleaner); 
        sensitive << clk.pos(); 
//        sc_input(m_tlm_1_if);

    }
    virtual ~aie_axis2msm_init() = default;

    //Export
    sc_port<tlm::tlm_fifo_get_if<T>> sc_input;
    msm_core::msm_init_socket<T> msm_output;
    sc_core::sc_in<bool> clk;
private:
    //TLM_1_IF m_tlm_1_if;
    bool dirty = false; 

    void dirty_cleaner()
    {
                m_msm_intf->update(); //Explicitly call the update of the msm interface, so that
        if(dirty && msm_output.is_target_ready())
        {
            auto msm_payload = std::make_shared<T>();
            sc_input->nb_get(*msm_payload);
            dirty = false; 
        }
    }

    void sc2msm_updater()
    {
        //TODO : Convert this THREAD to METHOD
        while (true)
        {
            //We are reading from fifo after delta cycle, so that we can get the data written in
            //last posedge
            if (sc_input->nb_can_get())
            {
                
                m_msm_intf->update(); //Explicitly call the update of the msm interface, so that
                while(!msm_output.is_target_ready()) {
                    sc_core::wait(clk.posedge_event());
                    sc_core::wait(SC_ZERO_TIME);
                }
                auto msm_payload = std::make_shared<T>();
                if(dirty)
                {
                    sc_input->nb_get(*msm_payload);
                    dirty = false;
                }
                *msm_payload = sc_input->peek();
                msm_output.transport(msm_payload);
                m_msm_intf->update(); //Explicitly call the update of the msm interface, so that
                dirty = true;
                //msm_core::sync_update();
                //updated data is available for MSM in the next clock cycle.
                DEBUG_MSM_ADAPTER(  " Adapter updating data " << *msm_payload <<  " " <<sc_core::sc_time_stamp());
            } else {
                sc_core::wait((dynamic_cast<tlm::tlm_fifo<T>*>(sc_input.get_interface()))->ok_to_get());
            }
        }

    }

    void ready_updater();

    /*
     * To fetch the interface of the msm_output and null checking
     */
    virtual void end_of_elaboration() override
    {
        m_msm_intf = msm_output.get_fw_interface();
        if (m_msm_intf == nullptr)
            throw std::runtime_error(
                    "MSM interface of " + std::string(this->name()) + " isn't bound");
    }

    msm_core::msm_sock_interface<T> *m_msm_intf; //!< MSM initiator socket to send AXIS txns

};

} /* namespace msm_adapters */
