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

#include <memory>
#include <vector>

#include "msm_module_name.h"
#include "msm_port_sock_base.h"
#include "msm_proc_handle_base.h"
#include "msm_senstive.h"
#include "msm_thread_handle.h"

//Forward declare msm_debugger_env
namespace msm_debugger
{
class msm_debugger_env;
}
namespace msm_core
{

class msm_spawn_options; //!< Forward declaration
/**
 * @brief class for asc modules
 */
class msm_module: public msm_object
{
    friend class msm_senstive;
public:
	msm_module(const msm_module_name& module_name) ;
	virtual ~msm_module();
/**
	 * @brief Function which can be overloaded by msm_modules to perform
	 * cleanup activities when simulation is about to end
	 */
	virtual void end_of_simulation() {};    

	//TODO Add end_of_elaboration call . Need to change the order in msm_core::elaborate()
	virtual void before_end_of_elaboration() {} ;



protected:
	msm_senstive sensitive; //!< Member variable to register sensitivity to procs aptly
	msm_core::msm_proc_handles_per_module __m_proc_handles; //!< Contains all the proc handles of this module
	 //TODO : Make above var as private

	/*
	 * Making spawn function to be friend of msm_module,
	 * so that it can access protected variables
	 */
	template<class T>
	friend msm_proc_handle_base* msm_spawn_method(T object,
	        const char *proc_name, const msm_spawn_options *opt_p = nullptr);

	/*
	 * Making spawn function to be friend of msm_module,
	 * so that it can access protected variables
	 */
	template<class T>
	friend msm_proc_handle_base* msm_spawn_thread(T object,
	        const char *proc_name, const msm_spawn_options *opt_p = nullptr);

};

extern std::shared_ptr<msm_proc_handle_base> g_last_registerd_proc; //!<Declaration of method to store last registered process

/**
 * @brief function to get the vector of all registered modules
 * so far,
 * @return returns vector of pointers for modules
 */
std::vector<msm_module*> get_msm_modules();

/**
 * @brief function to get the last registered module
 * @return returns last registerd module
 */
msm_module* get_last_registerd_module();


/** MACRO OVERLOAD for backward compatiablity **/

#define GET_MACRO(_1,_2,NAME,...) NAME

#define MSM_HAS_PROCESS(user_module_name) \
	typedef user_module_name MSM_CURRENT_USER_MODULE; \
	friend class msm_debugger::msm_debugger_env;

#define MSM_METHOD1(user_method_name) \
    msm_core::g_last_registerd_proc = std::make_shared<msm_core::msm_method_handle<decltype(std::bind(&MSM_CURRENT_USER_MODULE::user_method_name, this))> > (\
	        std::bind(&MSM_CURRENT_USER_MODULE::user_method_name, this), #user_method_name)

#define MSM_METHOD2(user_method_name, scheduler) \
    __m_proc_handles.register_proc_handle_with_clk(scheduler , std::make_shared<msm_core::msm_method_handle<decltype(std::bind(&MSM_CURRENT_USER_MODULE::user_method_name, this))> > (\
	        std::bind(&MSM_CURRENT_USER_MODULE::user_method_name, this), #user_method_name))

#define MSM_THREAD1(user_method_name) \
    msm_core::g_last_registerd_proc = std::make_shared<msm_core::msm_thread_handle<decltype(std::bind(&MSM_CURRENT_USER_MODULE::user_method_name, this, std::placeholders::_1))> > (\
			std::bind(&MSM_CURRENT_USER_MODULE::user_method_name, this, std::placeholders::_1), #user_method_name)

#define MSM_THREAD2(user_method_name, scheduler) \
    __m_proc_handles.register_proc_handle_with_clk(scheduler , std::make_shared<msm_core::msm_thread_handle<decltype(std::bind(&MSM_CURRENT_USER_MODULE::user_method_name, this, std::placeholders::_1))> > (\
			std::bind(&MSM_CURRENT_USER_MODULE::user_method_name, this, std::placeholders::_1), #user_method_name))

#define LEGACY_MSM_THREAD_CREATOR(class_name, thread_func_name) \
    std::make_shared<msm_core::msm_thread_handle<decltype(std::bind(&class_name::thread_func_name, this, std::placeholders::_1))> > (\
			std::bind(&class_name::thread_func_name, this, std::placeholders::_1), #thread_func_name)

#define MSM_METHOD(...) GET_MACRO(__VA_ARGS__, MSM_METHOD2, MSM_METHOD1)(__VA_ARGS__)
#define MSM_THREAD(...) GET_MACRO(__VA_ARGS__, MSM_THREAD2, MSM_THREAD1)(__VA_ARGS__)

/**
 * @brief Enabling the sensitivity of the current method
 * only to the given channel
 * @param channel is the only channel to which method becomes
 *  sensitive
 */
void next_trigger(msm_core::msm_prim_channel &channel);

/**
 * @brief Enabling the sensitivity of the current method
 * only to the given port
 * @param port is the only port to which method becomes
 *  sensitive
 */
template <class T>
void next_trigger(msm_core::msm_port_export_base<T> &port)
{
    msm_prim_channel *channel = dynamic_cast<msm_prim_channel*>(port.get_interface());
    if (channel)
    {
        next_trigger(*channel);
    }
    else
    {
        MSM_KERNEL_ERROR("failed to run next_trigger in MSM");
    }
}

/**
 * @brief Blocking the current thread and making it to wait
 * only on the given channel
 * @param channel is the channel to which thread will unblock
 * @param MSM_THREAD_ARGUMENT -> Pass MSM_THREAD_CASCADE
 */
void wait(msm_core::msm_prim_channel& channel, MSM_THREAD_ARGUMENT);

/**
 * @brief Blocking the current thread and making it to wait
 * on the static sensitivity of the thread
 * @param MSM_THREAD_ARGUMENT -> Pass MSM_THREAD_CASCADE
 */
void wait(MSM_THREAD_ARGUMENT);

} /* namespace msm_core */
