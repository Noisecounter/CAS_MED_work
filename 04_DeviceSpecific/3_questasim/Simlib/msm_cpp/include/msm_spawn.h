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

#include <memory>

#include "msm_interface.h"
#include "msm_method_handle.h"
#include "msm_module.h"
#include "msm_port_sock_base.h"
#include "msm_senstive.h"
#include "msm_spawn_options.h"

namespace msm_core
{
class msm_spawn_options;
} /* namespace msm_core */

namespace msm_core
{

extern std::shared_ptr<msm_proc_handle_base> g_last_registerd_proc; //!<Declaration of method to store last registered process

/**
 * @brief API to spawn the MSM_METHOD dynamically. Much of the usage is similar to
 * SystemC, except that methods in MSM can only be spawned only during construction/
 * before the elaboration
 *
 * @tparam T is the method type to be registered as method (will be inferred by the
 * compiler, user need not to worry
 * @param object is the method function, ideally using std::bind
 * @param proc_name is the name of the spawning process
 * @param opt_p is additional options to make the spawning thread sensitive
 * @return returns process handle for created method
 */
template<typename T>
msm_proc_handle_base* msm_spawn_method(T object, const char *proc_name,
        const msm_spawn_options *opt_p)
{
    g_last_registerd_proc = std::make_shared<msm_core::msm_method_handle<T>>(object, proc_name);
    if (opt_p)
    {
        msm_module *last_module = get_last_registerd_module();
        for (auto channels : opt_p->m_channels)
            last_module->sensitive << *channels;
        for (auto ports : opt_p->m_ports)
            last_module->sensitive << *ports;
        for (auto clks : opt_p->m_schedulers)
            last_module->sensitive << clks;
    }
    return g_last_registerd_proc.get();
}

/**
 * @brief API to spawn the MSM_THREAD dynamically. Much of the usage is similar to
 * SystemC, except that methods in MSM can only be spawned only during construction/
 * before the elaboration
 *
 * @tparam T is the thread type to be registered as thread (will be inferred by the
 * compiler, user need not to worry
 * @param object is the thread function, ideally using std::bind
 * @param proc_name is the name of the spawning process
 * @param opt_p is additional options to make the spawning thread sensitive
 * @return returns process handle for created thread
 */
template<typename T>
msm_proc_handle_base* msm_spawn_thread(T object, const char *proc_name,
        const msm_spawn_options *opt_p)
{
    g_last_registerd_proc = std::make_shared<msm_core::msm_thread_handle<T>>(object, proc_name);
    if (opt_p)
    {
        msm_module *last_module = get_last_registerd_module();
        for (auto channels : opt_p->m_channels)
            last_module->sensitive << *channels;
        for (auto ports : opt_p->m_ports)
            last_module->sensitive << *ports;
        for (auto clks : opt_p->m_schedulers)
            last_module->sensitive << clks;
    }
    return g_last_registerd_proc.get();
}
}

