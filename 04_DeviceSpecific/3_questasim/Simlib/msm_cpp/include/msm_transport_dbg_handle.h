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

#include <functional>

#include "msm_module_name.h"
#include "msm_object.h"
#include "msm_proc_handle_base.h"

namespace msm_core
{

template <class PAYLOAD>
class msm_transport_dbg_base: public msm_object
{
public:
	/**
	 * @brief an abstract interface to execute method
	 */
	virtual unsigned int execute(std::shared_ptr<PAYLOAD>& payload) = 0;

	msm_transport_dbg_base(msm_module_name &name) :
			msm_object(name)
	{
	}
	virtual ~msm_transport_dbg_base() = default;
};

template<class MODULE_INSTANCE, class PAYLOAD>
class msm_transport_dbg_handle: public msm_transport_dbg_base<PAYLOAD>
{
	MODULE_INSTANCE m_instance_ptr;
	std::function<unsigned int(MODULE_INSTANCE, std::shared_ptr<PAYLOAD>)> m_func_ptr;
public:
	msm_transport_dbg_handle(
			std::function<unsigned int(MODULE_INSTANCE, std::shared_ptr<PAYLOAD>)> func_ptr,
			MODULE_INSTANCE instance_ptr, msm_module_name& name) :
			msm_transport_dbg_base<PAYLOAD>(name), m_func_ptr(func_ptr), m_instance_ptr(
					instance_ptr)
	{
		if (!instance_ptr)
			MSM_KERNEL_FATAL("Transport debug " << this->name() << " Received null pointer");
	}
	virtual ~msm_transport_dbg_handle() = default;

	unsigned int execute(std::shared_ptr<PAYLOAD>& payload) override
	{
		//Call the registered function with given pointer
		return m_func_ptr(m_instance_ptr, payload);
	}
};

} /* namespace msm_core */
