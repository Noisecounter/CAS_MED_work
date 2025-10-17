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
#include "sc2msm_base.h"

using namespace msm_core;
using namespace msm_trace;
namespace msm_adapters
{
template<class T>
class sc2msm_fifo_port_export : public sc2msm_base
{
  SC_HAS_PROCESS(sc2msm_fifo_port_export);
  public:
  sc2msm_fifo_port_export(sc_core::sc_module_name name) :
    sc2msm_base(name), msm_output("msm_output"), sc_input("sc_input"),
    clk("clk") , msm_fifo_channel("msm_fifo_channel")
  {
    sc_core::sc_object* ptr = this;

    SC_THREAD(ready_updater);
    sensitive << clk.pos();
    dont_initialize();

    SC_METHOD(sc2msm_updater);
    sensitive << sc_input;
    dont_initialize();

    msm_output(msm_fifo_channel);
  }

  virtual ~sc2msm_fifo_port_export() = default;

  msm_core::msm_port<msm_fifo_get_if<T> > msm_output; //!<MSM Socket (to be used with MSM AXIS payload)
  sc_core::sc_port<tlm::tlm_fifo_get_if<T>> sc_input; //!<AXIS XTLM Socket for SystemC
  sc_core::sc_in<bool> clk; //!< Clock input

  void set_bulk_process(bool enabled) {
    if (enabled)
      msm_fifo_channel.nb_bound(0xFFFFFF);
    bulk_process = enabled;
  }

  private:
  T sc_read;
  msm_fifo<T> msm_fifo_channel;
  bool data_driven = false; //!< Variable to know if the data is driven in the current posedge or not
  bool bulk_process = false; //!< Variable to process all the incoming data and forward it in a single clock cycle

  //Implementation of pure virtual function of indirect base msm2sc_scnc
  void sc2msm_updater()
  {
    DEBUG_MSM_ADAPTER((static_cast<msm_core::msm_object*>(this))->name()  << " sc_fifo " << sc_input->nb_can_get() << " @ " << sc_core::sc_time_stamp() );
    if( msm_fifo_channel.nb_can_put() && sc_input->nb_can_get())
    {
      do {
        sc_input->nb_get(sc_read);
        std::shared_ptr<T> data_ptr = std::make_shared<T>();
        *data_ptr = sc_read;
        DEBUG_MSM_ADAPTER(
            (static_cast<msm_core::msm_object*>(this))->name() << " read systemc socket and updated MSM interface " << " to " << *aie_axis_payload << " @ " << sc_core::sc_time_stamp());
        msm_fifo_channel.nb_put(data_ptr);
      } while(bulk_process && msm_fifo_channel.nb_can_put() && sc_input->nb_can_get());
      msm_core::sync_update();
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
      wait();
      if (!data_driven) {
        if(msm_fifo_channel.nb_can_put() && sc_input->nb_can_get())
        {
          do {
            sc_input->nb_get(sc_read);
            std::shared_ptr<T> data_ptr = std::make_shared<T>();
            *data_ptr = sc_read;
            msm_fifo_channel.nb_put(data_ptr);
          } while(bulk_process && msm_fifo_channel.nb_can_put() && sc_input->nb_can_get());
          msm_core::sync_update();
        }
      }
    }
  }
};

} /* namespace msm_adapters */
