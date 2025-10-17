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


namespace msm_core
{
class msm_proc_handles_per_clk_domain_per_module;
} /* namespace msm_core */

namespace msm_core
{

class msm_exec_abstract_scheduler
{
public:
    msm_exec_abstract_scheduler() = default;
    virtual ~msm_exec_abstract_scheduler() = default;

    /**
	 * @brief API to register an msm_process of a module with given scheduler
	 * @param method_handle is an unique_ptr to msm_method
	 */
	virtual void register_msm_process_per_clk(msm_proc_handles_per_clk_domain_per_module& method_handle) = 0;

	/**
	 * @brief API to schedule msm methods for the first time.
	 * This API will be called from elaboration. It's upto the scheduler to make use or ignore
	 * this API (with dummy implementation)
	 */
	virtual void init_sched_msm_methods() = 0;

	/**
	 * @brief an API to execute, execute_cycle API of all the scheduled msm_modules
	 */
	virtual void execute_methods() = 0;

	/**
	 * @brief API to execute delta cycle araising out of combinational
	 */
	virtual void exec_combinational_delta_cycle() = 0;
};

} /* namespace msm_core */
