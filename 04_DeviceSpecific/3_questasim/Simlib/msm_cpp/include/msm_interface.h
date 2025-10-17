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
#include <functional>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

#include "msm_debug.h"
#include "msm_module_name.h"
#include "msm_transport_dbg_handle.h"
#include "trace/vcd_trace_object_msm.h"
/*
 * Forward declaration for MSM adapters, so that adapters can be made friend of interfaces to
 * access private functions of interface
 */
namespace msm_adapters
{
template<class > class sc2msm;
template<class > class msm2aie_axis;
}

namespace msm_core
{

/** for post update phase ** HACK FOR COMBINATIONAL LOGIC.. NEED TO BE REMOVED */
class post_update_callback_base
{
public:
    post_update_callback_base() = default;
    virtual ~post_update_callback_base() = default;

    /**
     * @brief an abstract interface to execute the process
     */
    virtual void execute() = 0;
};

template<class MODULE_TYPE>
class post_update_callback: public post_update_callback_base
{
    MODULE_TYPE *m_instance_ptr;
    std::function<void(MODULE_TYPE*)> m_func_ptr;
public:
    post_update_callback(std::function<void(MODULE_TYPE*)> func_ptr, MODULE_TYPE *instance_ptr) :
            m_func_ptr(func_ptr), m_instance_ptr(instance_ptr)
    {
        if (!instance_ptr)
            MSM_KERNEL_FATAL("Post update call back Received null pointer");
    }

    void execute() override
    {
        //Call the registered function with given pointer
        m_func_ptr(m_instance_ptr);
    }
};

class msm_module;
//Forward declaration

/**
 * @brief base class for creating interfaces.
 * Custome interface class should derived from this class
 */

class msm_interface_base: public msm_object
{

public:

    /**

     * @brief Constructor

     */

    msm_interface_base(const msm_module_name &interface_name) :
            msm_object(interface_name)
    {
    }
    ;

    virtual ~msm_interface_base() = default;

};

class msm_proc_handle_base;
//!< Forward declaration
/**
 * @brief base class for the interfaces (without template)
 * An abstract base class
 */
class msm_prim_channel: public msm_object
{
public:
    /**
     * @brief a pure virtual function which will be invoked for after every
     * execution cycle
     */
    virtual bool update() = 0;

    /**
     * @brief API to register process handle
     * @param proc is the MSM process registered as sensitive to this channel
     */
    void register_msm_proc_sensitive_to_channel(std::shared_ptr<msm_proc_handle_base> proc);
    /**
     * @brief Constructor
     */
    msm_prim_channel(const msm_module_name &interface_name);

    /**
     * @brief Virtual destructor
     */
    virtual ~msm_prim_channel();

    /**
     * @brief API to get procs (static) sensitive to current channel
     * @return
     */
    const std::vector<std::shared_ptr<msm_proc_handle_base> >& get_procs_sensitive_to_channel() const
    {
        return m_procs_sensitive_to_channel;
    }

    /**
     * @brief API to get procs (dynamically blocked) senstive to current channel
     * @return
     */
    const std::vector<std::shared_ptr<msm_proc_handle_base> > get_procs_blocked_on_channel() const
    {
        return m_procs_blocked_on_channel;
    }

    /**
     * @brief API to set proc to be blocked on the channel
     * @param blocked_proc is the proc which is blocked on the current channel
     */
    void set_proc_blocked_on_channel(std::shared_ptr<msm_proc_handle_base> blocked_proc)
    {
        m_procs_blocked_on_channel.push_back(blocked_proc);
    }

    /**
     * @brief API to clear blocked procs
     */
    void clear_blocked_procs()
    {
        m_procs_blocked_on_channel.clear();
    }

    post_update_callback_base *m_post_update_base; //TODO: Remove this

    std::vector<msm_trace::vcd_trace_object_base_msm*> m_trace_obj;
protected:
    /**
     * @brief an API for scheduler to queue an update request for the current primitive channel
     */
    void request_update();

private:

    std::vector<std::shared_ptr<msm_proc_handle_base>> m_procs_sensitive_to_channel; //!< List of process sensitive to current channel
    std::vector<std::shared_ptr<msm_proc_handle_base>> m_procs_blocked_on_channel; //!< List of process blocked on the current channel
};

/**
 * @brief A registry of all the msm_interface(s) in the design.
 * We would like to use this register to update the interface(s)
 * @return returns a copy of global interface registry
 */
std::vector<msm_prim_channel*> get_msm_prim_channels();

/**
 * @brief an API to for update the interface from a thread out side of MSM kernel. The host
 * thread will perform update request instead of MSM. It has to be noted that host thread should
 * call this API only when MSM kernel is not active.
 * Should be called from single thread
 */
void sync_update();

template<class T>
class msm_sock_interface: public msm_prim_channel
{
public:

    msm_sock_interface(msm_module_name sock_intf) :
            msm_prim_channel(sock_intf), before_update(nullptr), after_update(nullptr), valid_before(
            false), valid_after(false), ready_before(true), ready_after(true), m_transport_dbg_proc(
                    nullptr), update_requested(0)
    {
    }
    /**
     * @brief implementation of pure virtual function for updating interface
     */
    bool update() override;

    /**
     * @brief an API to write to the interface
     * @param payload is shared_ptr for the payload to be updated
     */
    void write(std::shared_ptr<T> &payload);

    /**
     * @brief an API to read the interface
     * @return returns shared_ptr for payload on the itnerface
     */
    std::shared_ptr<T>& read();

    /**
     * @brief an API to peek/snoop the interface (this will not change the state of the interface)
     * @return returns pointer to interface data (read only)
     */
    const std::shared_ptr<const T> peek();

    /**
     * @brief an API to check if the payload on the interface is valid
     * @return true -> for valid , false-> invalid payload
     */
    bool is_valid()
    {
        return valid_after;
    }

    /**
     * @brief an API to check if the interface is ready to accept new payload
     * @return true -> can accept the payload, false-> can't accept
     */
    bool is_ready()
    {
        return ready_after;
    }

    /**
     * @brief an API to set if the interface can accept new payload in NEXT clock cycle
     */
    void set_ready(bool ready_val)
    {
        ready_before = ready_val;
    }

    /**
     * @brief API to register transport debug function of an msm_module. Only member functions of
     * an msm_module class can be registered as transport_dbg function
     * @param module_ptr is the pointer of associated msm_module
     * @param func_ptr is the function pointer of transport_dbg function
     */
    void register_transport_dbg(std::unique_ptr<msm_transport_dbg_base<T>> transport_dbg_hadle)
    {
        m_transport_dbg_proc = std::move(transport_dbg_hadle);
    }
    /**
     * @brief API for the initiator socket to execute debug transport function
     * @param payload is the transaction payload
     * @return returns number of bytes updated/read
     */
    unsigned int execute_transport_dbg(std::shared_ptr<T> &payload)
    {
        //If a transport_dbg function is registered call the same
        if (m_transport_dbg_proc)
            return m_transport_dbg_proc->execute(payload);
        else
        {
            MSM_REPORT_INFO(
                    this->name() << " received transport_dbg call, but no transport_dbg is registered");
            // else the default implementation is no bytes have been changed
            return 0;
        }
    }

private:
    std::shared_ptr<T> before_update; //!< shared_ptr for the payload which is updated by socket
    std::shared_ptr<T> after_update; //!< shared_ptr for the payload after update phase of msm_infra executed
    bool valid_before;	//!<valid before update phase, The valid ready pair mimics AXIS interface,
    bool valid_after; //!< valid after update phase is executed
    bool ready_before; //!< ready before update phase executed
    bool ready_after; //!< ready after update phase is executed
    std::atomic<int> update_requested; //!< To make sure we request update only once using
    // read-modify-write operations, so that mutex locks can be avoided

    friend msm_adapters::msm2aie_axis<T>; // Making adapter class as friend so that, adapter has access to
    //internal state of the interface and updates the same. This will avoid clock cycle delay ,
    //and helps in maintaining clock cycle accurate transactor /adapter
    std::unique_ptr<msm_transport_dbg_base<T> > m_transport_dbg_proc; //!< Pointer to transport_dbg_handler
};

} /* namespace msm_core */

template<class T>
inline bool msm_core::msm_sock_interface<T>::update()
{

    if (valid_before && ready_before)
    {
        valid_after = valid_before;
        after_update = before_update;
        valid_before = false;
    }
    ready_after = ready_before;

    if (valid_after)
        ready_before = false;

    update_requested = 0; // Reset the update request count
    return true;
}

template<class T>
inline void msm_core::msm_sock_interface<T>::write(std::shared_ptr<T> &payload)
{
    valid_before = true;
    before_update = payload;

    if (update_requested++ == 0) // To make sure only one request is placed
    {
        request_update();
    }

}

template<class T>
inline std::shared_ptr<T>& msm_core::msm_sock_interface<T>::read()
{
    valid_after = false;
    ready_before = true;
    if (update_requested++ == 0) // To make sure only one request is placed
        request_update();
    return after_update;
}

template<class T>
inline const std::shared_ptr<const T> msm_core::msm_sock_interface<T>::peek()
{
    return after_update;
}
