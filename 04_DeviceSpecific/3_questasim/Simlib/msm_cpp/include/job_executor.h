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

#include <list>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "msm_debug.h"
#include "msm_proc_handle_base.h"
#include "trace/msm_trace_obj_base.h"

namespace msm_trace
{
class msm_trace_obj_base;
//!< Forward declaration
} /* namespace msm_trace */

namespace msm_core
{

class msm_method_base;
class static_scheduler;
class job_executor
{
public:
    job_executor(static_scheduler *scheduler);
    virtual ~job_executor();

    /**
     * @brief API to enqueue msm_process in the given scheduler queue
     * @param method_handle is an handle for msm_method
     */
    virtual void enqueue_msm_process(msm_proc_handles_per_clk_domain_per_module *method_handle);

    /**
     * @brief API to set the msm_process for execution when triggered
     */
    virtual void set_process_for_execution();

    /**
     * @brief API to enqueue msm trace objects
     * @param trace_obj is the msm trace object pointer to be enqueued
     */
    virtual void enqueue_trace_objs(msm_trace::msm_trace_obj_base *trace_obj);

    /**
     * @brief API to set the interfaces for execution when triggered
     */
    void set_interfaces_for_execution();

    /**
     * @brief API to set the msm_traces for execution and generate traces when triggered
     */
    void set_traces_for_generation();

    /**
     * @brief an API to request scheduler thread to stop the execution loop
     * the scheduler no longer runs , once it stops execution
     */
    void stop_request();

    /**
     * @brief an API to get the trace data generated in the current trace generation phase
     * @return
     */
    std::list<msm_trace::msm_trace_obj_base*>& get_trace_obj_list()
    {
        return m_trace_obj_list;
    }

    std::set<std::shared_ptr<msm_proc_handle_base> >& get_comb_process()
    {
        return m_comb_process;
    }

private:
    static_scheduler *m_scheduler; //!< associated scheduler with the current executor
    std::thread m_thread;		//!< thread for executing jobs
    std::vector<msm_proc_handles_per_clk_domain_per_module*> m_msm_proc_queue; //!<msm_process job queue

    std::vector<msm_trace::msm_trace_obj_base*> m_trace_obj_queue; //!< msm_trace_obj queue

    std::list<msm_trace::msm_trace_obj_base*> m_trace_obj_list; //!< Trace data generated //TODO : Should we use vector ?

    enum class queue_type
    {
        MSM_PROCESS, INTERFACE, TRACE_OBJS
    }; //Queue types to be executed

    queue_type m_selected_queue; //!< Selected queue for execution

    void executer_thread();		//!< thread which execute jobs

    bool can_thread_execute(); //!< API to check if the spawned thread can execute or wait

    void execute_process(); //!< Function to execute process

    void update_interfaces(); //!< Function to Update interfaces

    void generate_traces(); //!< Function to generate trace

    bool is_stop_req;	//!< Variable to know if job executor's stop request is initiated
    bool is_selected_ex_done; //!< Status representing if selected queue execution is done or not

    std::set<std::shared_ptr<msm_proc_handle_base> > m_comb_process; //!< Set of combinational process to be executed-
    //-which became active because of current cycle execution

#ifdef KERNEL_SCHEDULE_STATS
	//Define this macro for kernel profiling information
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
	const unsigned int m_stats_freq = 1000;
	unsigned int m_sample_count = 0;
	uint64_t m_differntial_per_sample = 0;
	void start_time_measurement()
	{
	    m_start_time = std::chrono::high_resolution_clock::now();
	}
	void end_time_measurement()
	{
	    m_differntial_per_sample += std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - m_start_time).count();

	    if(m_stats_freq == m_sample_count)
	    {
	        MSM_REPORT_INFO(" thread " << std::this_thread::get_id() << " Size " << m_msm_proc_queue.size() << "  Consumed " << m_differntial_per_sample );
	        m_differntial_per_sample = 0;
	        m_sample_count = 0;
	    }
	    m_sample_count++;
	}
#endif

};

} /* namespace msm_core */
