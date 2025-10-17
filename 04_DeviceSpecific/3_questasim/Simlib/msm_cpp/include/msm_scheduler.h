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

#include <cstdint>
#include <functional>
#include <set>
#include <string>

#include "msm_exec_abstract_scheduler.h"
#include "msm_time.h"
#include "msm_update_abstract_scheduler.h"
#include "trace/msm_trace_abstract_scheduler.h"

namespace msm_core
{

class msm_scheduler_base: public msm_exec_abstract_scheduler,
        public msm_update_abstract_scheduler,
        public msm_trace_abstract_scheduler
{
public:
    /**
     * @brief constructor
     * @param schduler_instance is the instance name of the scheduler
     * @param num_threads no of threads requested for scheduling.
     * Default is 0,which represents maximum available cores
     */
    //TODO: time_offset isn't yet supported
    msm_scheduler_base(const std::string &scheduler_instance, const msm_time time_period,
            const unsigned int num_threads, const msm_time time_offset);
    virtual ~msm_scheduler_base();

    /**
     * @brief an API to execute methods and update interfaces. It is recommended to use only when
     * there is single scheduler. When there are multiple schedulers this shouldn't be used.
     * Instead execute_methods of all the schedulers called first then update_interfaces should
     * be called for all the schedulers
     */
    virtual void execute_and_update()
    {
        execute_methods();
        update_interfaces();
        exec_combinational_delta_cycle();

        //Adding tracing also in the simulation cycle
        check_trace_interval_and_generate();

	//Call debug handler only if this scheduler is posedge
	//search name if "pos" is present  at the end
	if(m_name.rfind("pos") == m_name.size() - 3)		
	        debugger_handler();
        //Update the simulation time
//        update_sim_time_by_clk_period(); TODO -> Enable it again by considering multiple clocks and offset
    }

    /**
     * @brief API to update MSM data to SystemC
     */
    virtual void msm_to_sysc_update() = 0;

    /**
     * @brief an API to get all the schedulers in the design
     * @return
     */
    static const std::set<msm_scheduler_base*>& get_schedulers()
    {
        return s_schedulers;
    }

    /**
     * @brief Name of the scheduler
     * @return returns name of the scheduler
     */
    const std::string& name() const
    {
        return m_name;
    }

    /**
     * @brief API to get number of std::threads used by scheduler
     * @return number of std::threads spawned by the scheduler
     */
    virtual const unsigned int get_num_threads() const = 0;

    /**
     * @brief API to get the number of simulation cycles e
     * @return return number of clock cycles (positive edges)  executed
     */
    uint64_t get_num_sim_cycle_count() const
    {
        return m_num_sim_cycles_completed;
    }

    /**
     * @brief API to check if the current cycle is an execution phase of the given scheduler
     * @return returns true if it's an execute cycle, false otherwise (could be delta / trace
     * SYSC <-> MSM synch phases etc
     */ 
    virtual bool is_execute_cycle() const = 0;

    static void msm_debugger_before_end_of_elaborate()
    {
        if(msm_debugger_before_end_of_elaborate_cb)
            msm_debugger_before_end_of_elaborate_cb();
    }
    
protected:
    const std::string m_name; 		 //!< Instance name
    static std::set<msm_scheduler_base*> s_schedulers; //!< List of all the schedulers in the design
    const msm_time m_time_period; //!< Time period of the scheduler
    uint64_t m_num_sim_cycles_completed; //!< Number of simulation cycles executed
    msm_time m_cur_time; //!< Current simulation time

    /**
     * @brief API to update the simulation time by concrete implementation
     */
    void update_sim_time_by_clk_period();

    /**
     * @brief API to handle debug infrastructure
     */
    void debugger_handler();

    inline static std::function<void()> msm_debugger_cb {}; //! MSM debug function call back
    inline static std::function<void()> msm_debugger_before_end_of_elaborate_cb{}; //! MSM debug function call back for before end of elaborate

    /**
     * @brief API to get callback function from debugger lib
     * @return
     */
    std::function<void()> get_call_back_function_from_debugger_lib();
};

} /* namespace msm_core */
