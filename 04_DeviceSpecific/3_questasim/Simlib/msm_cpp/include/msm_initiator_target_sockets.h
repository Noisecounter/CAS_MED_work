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
#include <memory>

#include "msm_debug.h"
#include "msm_interface.h"
#include "msm_module_name.h"
#include "msm_port_sock_base.h"
#include "msm_port.h"

namespace msm_core
{
//Forward declaration of msm_init_socket
template<class FW_IF, class BW_IF> class msm_aximm_initiator_socket;

template<class FW_IF, class BW_IF> class msm_aximm_target_socket;

template<class FW_IF, class BW_IF>
class msm_initiator_target_socket_base : public msm_port_sock_base 
{
	public:

	msm_initiator_target_socket_base(msm_module_name& sock_name) : 
			msm_port_sock_base(sock_name) {
	}
 	    /**
     * @brief API to register process handle
     * @param proc is the MSM process registered as sensitive to this channel
     */
    void register_msm_proc_sensitive_to_channel(std::shared_ptr<msm_proc_handle_base> proc) override
    {
        //msm_prim_channel* channel = dynamic_cast<msm_prim_channel*>(m_interface);
        //if (channel)
        //{
        //    channel->register_msm_proc_sensitive_to_channel(proc);
        //}
        //else
        //{
        //    m_procs_sensitive.push_back(proc);
        //}
    }

       /**
     * @brief API to check if the interface is bound or not.
     */
    bool is_bound() const override
    {
        //The current is considered to be bound, if it's bound to interface
        //return (m_interface != nullptr);
		return 1;
	}
	
};



/**
 * @class msm_aximm_target_socket target socket
 * @tparam T is payload data type
 */
template<class FW_IF, class BW_IF>
class msm_aximm_target_socket : public msm_initiator_target_socket_base<FW_IF, BW_IF>
{
public:
    /*!
     * @brief Constructor
     * @param sock_name sock_name is instance name of the socket
     */
    msm_aximm_target_socket(msm_module_name sock_name) : msm_initiator_target_socket_base<FW_IF, BW_IF>(sock_name),
		socket_port(sock_name.get_base_name()+ std::string("_port")), socket_export(sock_name.get_base_name()+ std::string("_export"))
	{
	}
    virtual ~msm_aximm_target_socket() = default;
	
	//Implements bind functionality
	virtual void bind(FW_IF &intf) {
		socket_export.bind(intf);
	}

    void operator ()(FW_IF &intf)
    {
        bind(intf);
    }
 	void bind(msm_aximm_target_socket<FW_IF, BW_IF> &child_socket) {
		socket_port.bind(child_socket.socket_port);
		socket_export.bind(child_socket.socket_export);
	}
	
	void bind(msm_aximm_initiator_socket<FW_IF, BW_IF> &init_socket) {
		init_socket.socket_port.bind(socket_export);
		socket_port.bind(init_socket.socket_export);
	}
    virtual void operator() (msm_aximm_target_socket<FW_IF, BW_IF>& child_socket)
    {
        bind(child_socket);
    }

    /**
     * @brief parentheses operator to bind target socket to initiator socket at same level.
     * @param init_soceket is a initiator_socket to bind
     */
    virtual void operator() (msm_aximm_initiator_socket<FW_IF,BW_IF>& init_socket)
    {
        bind(init_socket);
    }
	BW_IF* operator->()
    {
        return socket_port.m_interface;
    }
	msm_port<BW_IF> socket_port;
	msm_export<FW_IF> socket_export;

};

/**
 * @class msm_aximm_target_socket target socket
 * @tparam T is payload data type
 */
template<class FW_IF, class BW_IF>
class msm_aximm_initiator_socket : public msm_initiator_target_socket_base<FW_IF, BW_IF>
{
public:
    /*!
     * @brief Constructor
     * @param sock_name sock_name is instance name of the socket
     */
    msm_aximm_initiator_socket(msm_module_name sock_name) : msm_initiator_target_socket_base<FW_IF, BW_IF>(sock_name),
		socket_port(sock_name.get_base_name()+ std::string("_port")), socket_export(sock_name.get_base_name()+std::string("_export"))
	{
	}
    virtual ~msm_aximm_initiator_socket() = default;
 	
	//Implements bind functionality
	virtual void bind(BW_IF &intf) {
		socket_export.bind(intf);
	}

    void operator ()(BW_IF &intf)
    {
        bind(intf);
	}
	void bind(msm_aximm_target_socket<FW_IF, BW_IF> &child_socket) {
		socket_port.bind(child_socket.socket_export);
		child_socket.socket_port.bind(socket_export);
	}
	
	void bind(msm_aximm_initiator_socket<FW_IF, BW_IF> &init_socket) {
		init_socket.socket_port.bind(socket_port);
		init_socket.socket_export.bind(socket_export);
	}
	  virtual void operator() (msm_aximm_target_socket<FW_IF, BW_IF>& child_socket)
    {
        bind(child_socket);
    }

    /**
     * @brief parentheses operator to bind target socket to initiator socket at same level.
     * @param init_soceket is a initiator_socket to bind
     */
    virtual void operator() (msm_aximm_initiator_socket<FW_IF, BW_IF>& init_socket)
    {
        bind(init_socket);
    }
	
	FW_IF* operator->()
    {
        return socket_port.m_interface;
    }

	msm_port<FW_IF> socket_port;
	msm_export<BW_IF> socket_export;
};
} /* namespace msm_core */

