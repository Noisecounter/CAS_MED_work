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

#include <map>
#include <memory>
#include <vector>

#include "msm_interface.h"
#include "msm_module_name.h"
#include "msm_scheduler.h"

namespace msm_core
{
class msm_prim_channel;
class msm_proc_handle_base: public msm_object
{
    bool m_proc_disabled = false;
public:
    msm_proc_handle_base(msm_module_name &name) :
            msm_object(name)
    {
    }
    virtual ~msm_proc_handle_base() = default;

    /**
     * @brief an abstract interface to execute the process
     */
    virtual void execute() = 0;

    /**
     * @brief API to enable the execution if the given channel matches with
     * the channel with which current process is blocked
     * @param channel is the channel which is trying to unblock the process
     * @return true if it's unblocked / false if it's blocked
     */
    void enable()
    {
        m_proc_disabled = false;
    }

    /**
     * @brief API to block the current process with given channel
     * @param channel is the channel which will block current process
     */
    void disable()
    {
        m_proc_disabled = true;
    }

    /**
     * @brief API to check if the execution is disabled for current process
     * @return true if the process is disabled , false if not
     */
    bool is_exec_disabled() const
    {
        return m_proc_disabled;
    }

};

/**
 * @brief API to get the current MSM process in execution in the
 * given Operating System thread
 * @return returns the current MSM thread in given OS thread
 */
std::shared_ptr<msm_proc_handle_base> msm_get_current_process_in_os_thread();

/**
 * @class Base class for all MSM_METHOD
 */
class msm_method_base: public msm_proc_handle_base
{
public:

    msm_method_base(msm_module_name &name) :
            msm_proc_handle_base(name)
    {
    }
    virtual ~msm_method_base() = default;
};

/**
 * @class Base class for all MSM_THREAD
 */
class msm_thread_base: public msm_proc_handle_base
{
public:

    msm_thread_base(msm_module_name &name) :
            msm_proc_handle_base(name)
    {

    }
    virtual ~msm_thread_base() = default;
};

/**
 * @class Class to register the MSM process (threads/methods) of a given clock domain
 * The class will aggregate all the process of the module which belong to same clock domain.
 * This class will be used for scheduling by multi threaded kernel so as to avoid data race
 * within the given module
 */
class msm_proc_handles_per_clk_domain_per_module
{
public:

    /**
     * @brief Function which executes registered processes
     */
    void execute();

    /**
     * @brief Function which is used to register process
     * @param proc_handle
     */
    void register_msm_proc_handle(std::shared_ptr<msm_proc_handle_base> proc_handle);

private:
    std::vector<std::shared_ptr<msm_proc_handle_base>> m_processs; //!< List of MSM process registered
};

class msm_scheduler_base;
//!<Forward declaration

/**
 * @class class which aggregates multiple clock domain process in the given msm_module
 */
class msm_proc_handles_per_module
{
public:
    /**
     * @brief API to register msm_proc_handles_per_clk_domain_per_module with the given clock domain
     * @param clk is the clock domain to be registered with
     * @param proc_handle is the unique pointer for msm_proc_handles_per_clk_domain_per_module
     */
    void register_proc_handle_with_clk(msm_scheduler_base *clk,
            std::shared_ptr<msm_proc_handle_base> proc_handle);

private:
    std::map<msm_scheduler_base*, std::shared_ptr<msm_proc_handles_per_clk_domain_per_module> > m_all_process; //!< Map of clock domains with associated process

};
}
