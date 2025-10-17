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

#include <atomic>
#include <condition_variable>
#include <deque>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "msm_interface.h"
#include "msm_proc_handle_base.h"
#include "msm_scheduler.h"
#include "msm_time.h"

namespace msm_core
{

class job_executor;
//!< Forward declaration of job executor
class msm_prim_channel;
//!<Forward declaration of primitive channel

class static_scheduler: public msm_scheduler_base
{
public:
    static_scheduler(const std::string name, const msm_time time_period,
            const unsigned int num_threads = 0, const msm_time time_offset = msm_time(0, MSM_FS));

    virtual ~static_scheduler();

    /**
     * @brief API to register an msm_module (with all the process) with given scheduler
     * @param method_handle is an unique_ptr to msm_method
     */
    virtual void register_msm_process_per_clk(
            msm_proc_handles_per_clk_domain_per_module &method_handle) override;

    /**
     * @brief API to schedule msm methods for the first time.
     */
    virtual void init_sched_msm_methods() override;

    /**
     * @brief API to schedule trace objects with the given scheduler
     * @param trace_object is the trace object to be scheduled
     */
    virtual void schedule_trace_objects(msm_trace::msm_trace_obj_base &trace_object) override;

    /**
     * @brief an API to execute, execute_cycle API of all the scheduled msm_methods
     */
    virtual void execute_methods() override;

    /**
     * @brief an API to execute, update API of all the scheduled interface(s)
     */
    virtual void update_interfaces() override;

    /**
     * @brief an API to generate traces of all trace objects
     */
    virtual void generate_traces() override;

    /**
     * @brief API to generate C++ traces
     * @return returns list of trace objects which got changed
     */
    std::list<msm_trace::msm_trace_obj_base*> generate_cpp_traces();

    /**
     * @brief API to update MSM data to SystemC
     */
    virtual void msm_to_sysc_update() override;

    /**
     * @brief API to execute delta cycle araising out of combinational
     */
    virtual void exec_combinational_delta_cycle() override;
    /**
     * @brief API to get number of std::threads used by scheduler
     * @return number of std::threads spawned by the scheduler
     */
    virtual const unsigned int get_num_threads() const override
    {
        return m_num_threads;
    }

    /**
     * @brief API to check if the current cycle is an execution phase of the given scheduler
     * @return returns true if it's an execute cycle, false otherwise (could be delta / trace
     * SYSC <-> MSM synch phases etc
     */ 
    bool is_execute_cycle() const override
    {
        return m_execute_triggered;
    }
private:

    /**
     * @brief API to run delta cycle of the scheduler
     */
    void run_delta_cycle();

    /**
     * @brief API to determine if delta cycle execution is required or not
     * @return true / false
     */
    bool is_delta_cycle_required();

    /**
     * @brief A blocking call, which blocks scheduler to wait until all the
     * scheduled jobs are completed
     */
    void trigger_executors_and_wait_for_completion();
    const unsigned int m_num_threads;   //!< Number of actual threads used
    std::vector<std::unique_ptr<job_executor>> m_executors; //!<Executors. forced to choose unique ptr
    // as executor is non movable - non-copyable type
    unsigned int m_method_count; //!< Total number of methods scheduled
    unsigned int m_intf_count;  //!< Total number of interfaces scheduled
    unsigned int m_trace_obj_count; //!<Total number of trace objects

    /*
     * scheduler <-> Executors multi-thread communication protocol
     *
     * Notifies to start --------------> Executors start executing
     *                          .
     *                          .
     *                          .
     *                          .
     * Receives done     <-------------- Each thread notifies that it has done it's job
     *
     * Following are member variables to implement above protocol
     */

    std::mutex m_start_mutex;           //!< Mutex associated with start signal
    std::mutex m_done_mutex;            //!< Mutex associated with done signal
    std::condition_variable m_start_cv; //!< Start condition variable
    std::condition_variable m_done_cv;  //!< Done condition variable
    std::atomic<unsigned int> m_num_done_threads;   //!< Number of threads which are in done state.
    unsigned int m_max_delta_count = 0;     //!< Maximum delta cycle count
    std::unordered_set<const msm_prim_channel*> m_interface_set; //!<Set of interface scheduled
    bool m_execute_triggered = false;  //!< maintains state of execute cycle
    std::list<msm_trace::msm_trace_obj_base*> m_merged_trace_obj_list;

    //making executor as friend class
    friend class job_executor;

    std::deque<msm_proc_handles_per_clk_domain_per_module*> m_msm_methods_list; //!< List of modules with proc in given lock domain
    std::set<std::shared_ptr<msm_proc_handle_base>> m_combinational_delta_process;
    //Write traces //TODO need to removed from here
    void fetch_trace_from_executors_n_write();
    bool m_is_trace_dumped_at_current_time = false; //!< to check if trace is dumped at current time
};

} /* namespace msm_core */

