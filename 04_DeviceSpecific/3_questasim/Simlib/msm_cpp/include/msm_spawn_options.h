// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
// (c) Copyright(C) 2013 - 2023 by Xilinx, Inc. All rights reserved.
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

#include <vector>

namespace msm_core
{
class msm_port_sock_base;
class msm_scheduler_base;
class msm_prim_channel;
class msm_proc_handle_base;
} /* namespace msm_core */


namespace msm_core
{
/**
 * @class class for setting options for spawning MSM threads/methods
 */
class msm_spawn_options
{
public:
    msm_spawn_options() = default;

    //!< Destructor
    ~msm_spawn_options();

    /**
     * @brief API to set the port sensitive to spawning process
     * @param port_base is the port which will be added to sensitivity list
     */
    void set_sensitivity(msm_port_sock_base* port_base);

    /**
     * @brief API to set the channel sensitive to spawning process
     * @param port_base is the channel which will be added to sensitivity list
     */
    void set_sensitivity(msm_prim_channel* channel);

    /**
     * @brief API to set the clock pos/neg edge sensitive to spawning process
     * @param port_base is the clock edge which will be added to sensitivity list
     */
    void set_sensitivity(msm_scheduler_base* scheduler);

    /*
     * Disable Copy operations
     */
    msm_spawn_options(const msm_spawn_options&) = delete;
    const msm_spawn_options& operator = (const msm_spawn_options& ) = delete;

private:
    std::vector<msm_prim_channel*> m_channels; //!< List of channels which are sensitive
    std::vector<msm_port_sock_base*> m_ports; //!< List of ports which are sensitive
    std::vector<msm_scheduler_base*> m_schedulers; //!< List of clock edges marked for sensitive

    /*
	 * Making spawn function to be friend of msm_module,
	 * so that it can access protected variables
	 */
	template<class T>
	friend msm_proc_handle_base* msm_spawn_method(T object,
	        const char *proc_name, const msm_spawn_options *opt_p);

	/*
	 * Making spawn function to be friend of msm_module,
	 * so that it can access protected variables
	 */
	template<class T>
	friend msm_proc_handle_base* msm_spawn_thread(T object,
	        const char *proc_name, const msm_spawn_options *opt_p);
};

} /* namespace msm_core */
