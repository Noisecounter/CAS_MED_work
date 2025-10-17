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

#include <set>

#include "msm_interface.h"
#include "msm_module_name.h"

namespace msm_core
{

/**
 * @brief Abstract port base class for all port types (port or socket)
 */
class msm_port_sock_base: public msm_object
{
public:

    /**
     * @brief API to check if the interface is bound or not.
     */
    virtual bool is_bound() const = 0;

    msm_port_sock_base(const msm_module_name &port_name);
    virtual ~msm_port_sock_base();

    /**
     * @brief API to register process handle
     * @param proc is the MSM process registered as sensitive to this channel
     */
    virtual void register_msm_proc_sensitive_to_channel(std::shared_ptr<msm_proc_handle_base> proc) = 0;
};
template<class IF>
class msm_port_export_base: public msm_port_sock_base
{
public:

    msm_port_export_base(const msm_module_name &port_name) :
            msm_port_sock_base(port_name), m_interface(nullptr)
    {
        m_hierarchical_port.resize(0);
    }
    ;

    virtual ~msm_port_export_base() = default;

    /**
     * @brief API to check if the interface is bound or not.
     */
    bool is_bound() const override
    {
        //The current is considered to be bound, if it's bound to interface
        return (m_interface != nullptr);
    }

    const IF* operator ->() const
    {
        if (m_interface == 0)
        {
            MSM_KERNEL_DEBUG(this->name() << "port is not bound" );
        }
        return m_interface;
    }

    IF* operator ->()
    {
        if (m_interface == 0)
        {
            MSM_KERNEL_DEBUG(this->name() << "port is not bound" );
        }
        return m_interface;
    }

    /**
     * @brief API to get reference of interface pointer.
     * @return
     */
    IF* get_interface() const
    {
        return m_interface;
    }
    /**
     * @brief bind functionality for binding with parent port
     * @param parent_hier_port is the hierarchical port of the parent
     */
    virtual void bind(msm_port_export_base<IF> &parent_hier_port)
    {
        parent_hier_port.m_hierarchical_port.push_back(this);

        if (parent_hier_port.m_interface)
        {
            this->bind(*parent_hier_port.m_interface);
        }
        else if (m_interface)
        {
            parent_hier_port.bind(*m_interface);
        }
        
        for (auto &proc : m_procs_sensitive) {
            parent_hier_port.register_msm_proc_sensitive_to_channel(proc);
        }

    }

    //Implements bind functionality
    virtual void bind(IF &intf)
    {
        if (m_interface && (m_interface != &intf))
        {
            MSM_KERNEL_FATAL(this->name() << " is already bound with interface ");
        }

        //At this point, It's a valid interface, we need to bind the interface to the current port
        //and to it's child port based on whether it's non-hierarchical port or not
        if (!m_hierarchical_port.empty())
        {
            for (auto hier_port : m_hierarchical_port)
            {
                hier_port->bind(intf);
            }
        }

        m_interface = &intf;
        //MSM_KERNEL_DEBUG(this->name() << " is binding with " << intf.name() );
        //Add sensitivity to interface, if any
        for (auto &proc : m_procs_sensitive)
        {
            msm_prim_channel *channel = dynamic_cast<msm_prim_channel*>(m_interface);
            if (channel)
                channel->register_msm_proc_sensitive_to_channel(proc);
        }
        m_procs_sensitive.clear();
    }

    /**
     * @brief operator overload for bind functionality
     * @param parent_hier_port is the parent hierarchical port to be bind
     */
    virtual void operator ()(msm_port_export_base<IF> &parent_hier_port)
    {
        bind(parent_hier_port);
    }

    void operator ()(IF &intf)
    {
        bind(intf);
    }

    /**
     * @brief API to register process handle
     * @param proc is the MSM process registered as sensitive to this channel
     */
    void register_msm_proc_sensitive_to_channel(std::shared_ptr<msm_proc_handle_base> proc) override
    {
        msm_prim_channel* channel = dynamic_cast<msm_prim_channel*>(m_interface);
        if (channel)
        {
            channel->register_msm_proc_sensitive_to_channel(proc);
        }
        else
        {
            m_procs_sensitive.push_back(proc);
        }
    }

private:
    std::vector<msm_port_export_base<IF>*> m_hierarchical_port; //!< Pointer to to hierarchical port
    std::vector<std::shared_ptr<msm_proc_handle_base> > m_procs_sensitive; //!< List of process sensitive to the port

public:
    IF *m_interface; //!< Interface bound to port

};

/**
 * @class Abstract base class for all the port types
 */
class msm_port_base: public msm_port_sock_base
{
public:
//	/**
//	 * @brief API to bind port with an interface
//	 * @param intf is the interface to bound
//	 */
//	virtual void bind(msm_interface_base &intf) = 0;
//
//    /**
//     * @brief parentheses operator overload interface binding
//     * @param intf is the interface to bound
//     */
//    virtual void operator ()(msm_interface_base &intf)
//    {
//        bind(intf);
//    }

    msm_port_base(const msm_module_name &port_name);
    virtual ~msm_port_base();
};

/**
 * @class Abstract base class for all the socket types
 */
class msm_socket_base: public msm_port_sock_base
{
public:
    msm_socket_base(const msm_module_name &sock_name);
    virtual ~msm_socket_base();

    /**
     * @brief API to register process handle
     * @param proc is the MSM process registered as sensitive to this channel
     */
    void register_msm_proc_sensitive_to_channel(std::shared_ptr<msm_proc_handle_base> proc) override
    {
        MSM_REPORT_ERROR("Sensitivity support isn't enabled for MSM sockets");
    }
};

class msm_fifo_base: public msm_port_sock_base
{
public:

    /**
     * @brief API to bind FIFO port with an FIFO interface
     * @param intf is the interface to bound
     */
    virtual void bind(msm_prim_channel &intf) = 0;

    msm_fifo_base(const msm_module_name &fifo_name);
    virtual ~msm_fifo_base();
};
//============================================================================//
//                  Port book keeping & helper function(s)
//============================================================================//

/**
 * @brief API to get ASC Ports and sockets in the design
 */
const std::set<msm_port_sock_base*>& get_msm_ports_and_sockets();

/**
 * @brief API to get ASC Ports in the design
 */
const std::set<msm_port_base*>& get_msm_ports();

/**
 * @brief API to get ASC sockets in the design
 */
const std::set<msm_socket_base*>& get_msm_sockets();

/**
 * @brief API to get ASC FIFO ports in the design
 */
const std::set<msm_fifo_base*>& get_msm_fifo_ports();

/**
 * @brief an API to check whether all the port(s) are bound or not
 * @return
 */
bool check_port_and_socket_bindings();

} /* namespace msm_core */
