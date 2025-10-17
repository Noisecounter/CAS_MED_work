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

#include "msm_proc_handle_base.h"
#include "msm_debug.h"

#include <boost/coroutine2/all.hpp>

namespace msm_core
{

#define MSM_THREAD_ARGUMENT boost::coroutines2::coroutine< void >::push_type& yield
#define MSM_WAIT yield()
#define MSM_THREAD_INST msm_core::msm_thread_handle<MSM_CURRENT_USER_MODULE>
#define MSM_THREAD_CASCADE yield

typedef void msm_thread;

/**
 * @brief MSM thread handler class. This class maintains state of the MSM thread
 * @tparam T is the msm_module type in which thread is used
 */
template<class T>
class msm_thread_handle: public msm_thread_base
{
    T m_bounded_thread; //!< Member variable to store binded function
public:
    /**
     * @brief Constructor of the MSM_thread
     * @param msm_thread_func is the function pointer of msm_module function
     * @param instance_ptr is the instance pointer of the msm_module
     * @param name is the name of the thread object (msm_object)
     */
    msm_thread_handle(T bounded_thread, msm_core::msm_module_name name) :
            msm_thread_base(name), m_bounded_thread(bounded_thread), m_coro(std::bind(m_bounded_thread, std::placeholders::_1))
    {
    }
    virtual ~msm_thread_handle() = default;

    /**
     * Implements pure virtual function of the msm_process
     */
    void execute() override
    {
        //Execute the method if it's not disabled
        if (!is_exec_disabled() && m_coro)
            m_coro();
    }
private:
    boost::coroutines2::coroutine<void>::pull_type m_coro; //!< Boost coroutine instance
};

} /* namespace msm_core */
