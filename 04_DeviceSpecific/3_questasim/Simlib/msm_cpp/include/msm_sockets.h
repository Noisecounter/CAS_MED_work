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
#include "msm_object.h"
#include "msm_port_sock_base.h"
#include "msm_proc_handle_base.h"
#include "msm_transport_dbg_handle.h"

namespace msm_core
{

//Forward declaration of msm_init_socket
template<class T> class msm_init_socket;

/**
 * @class msm_target_socket target socket
 * @tparam T is payload data type
 */
template<class T>
class msm_targ_socket: public msm_socket_base
{
public:
    /*!
     * @brief Constructor
     * @param sock_name sock_name is instance name of the socket
     */
    msm_targ_socket(msm_module_name sock_name);
    virtual ~msm_targ_socket() = default;

    /*!
     * @brief API to check if transaction is available to sample from the initiator
     * @return returns whether transaction is available or not
     */
    bool is_transaction_available();

    /*!
     * @brief API to sample the transaction from the initiator when available.
     * This is expected to be called after checking is_transaction_available
     * @return returns the payload request from the initiator
     */
    std::shared_ptr<T> sample_transaction();

    /**
     * @brief an API to peek the transaction
     * @return returns a pointer to transaction (in read-only format)
     */
    const std::shared_ptr<const T> peek_transaction();

    /*!
     * @brief API to apply back pressure to the initiator. Initiator isn't expected to send data
     * from next clock cycle.
     * For those who are aware with AXI Stream, the functionality is similar to de-asserting TREADY
     * signal on AXI Stream interface
     * @param ready_val value to be set. true-> accepts transactions, false-> otherwise
     */
    void set_ready(bool ready_val);

    /*!
     * @brief API to check if initiator is ready or not (to send response to initiator)
     * @return returns whether initiator is ready or not
     */
    bool is_init_ready();

    /*!
     * @brief API to transport given response to the initiator
     * @param payload is the transaction payload containing response
     */
    void transport(std::shared_ptr<T> payload);

    /*!
     * @brief API to hierarchically bind target socket.
     * @param socket is child socket
     */
    virtual void bind(msm_targ_socket<T> &child_socket);

    /**
     * @brief parentheses operator overload for bind operation
     * @param child_socket is child socket to bind
     */
    virtual void operator() (msm_targ_socket<T>& child_socket)
    {
        bind(child_socket);
    }

    /**
     * @brief API to check if the interface is bound or not, implements pure virtual function
     */
    virtual bool is_bound() const override
    {
        if (m_child_hier_socket && m_bw_interface)
            MSM_KERNEL_FATAL(
                    this->name() << " has binding issue . Please check hierarchy from source to destination" << " FW IF " << m_bw_interface->name() << " it's child " << m_child_hier_socket->name());

        if (m_bw_interface)
        {
            MSM_KERNEL_DEBUG(this->name() << " is bound with " << m_bw_interface->name());
        }
        else if (m_child_hier_socket)
        {
            MSM_KERNEL_DEBUG(this->name() << " is bound with " << m_child_hier_socket->name());
        }
        else
            MSM_KERNEL_DEBUG(this->name() << " isn't bound");
        return m_bw_interface || m_child_hier_socket;
    }

    /**
     * @brief API to register transport debug functionality. Actual implementation should be done by
     * user in the registered function
     * @param module_ptr is the pointer of msm_moudle
     * @param func_ptr is the function pointer of the transport_dbg functionality
     */
    template<class INSTANCE>
    void register_transport_dbg(unsigned int (INSTANCE::*transport_dbg)(std::shared_ptr<T>),
            INSTANCE *inst_ptr, msm_module_name name)
    {
        auto func_ptr = std::function<unsigned int(INSTANCE*, std::shared_ptr<T>)>(transport_dbg);
        m_fw_interface.register_transport_dbg(
                std::move(
                        std::make_unique<msm_core::msm_transport_dbg_handle<INSTANCE*, T> >(
                                func_ptr, inst_ptr, name)));
    }
//	virtual void register_transport_dbg(std::unique_ptr<msm_transport_dbg_base<T> > transport_dbg_handle)
//	{
//		m_fw_interface.register_transport_dbg(transport_dbg_handle);
//	}

public: //TODO make private :

    /**
     * @brief API to set bw_interface. This API sets BW interface by considering hierarchical connections
     * @param bw_interface is the BW interface to set
     */
    void set_bw_interface(msm_sock_interface<T> *bw_interface)
    {
        if (m_child_hier_socket)
        {
            m_child_hier_socket->set_bw_interface(bw_interface);
            return;
        }

        m_bw_interface = bw_interface;
    }

    /**
     * @brief API to get fw_interface. This API gets FW interface by considering hierarchical connections
     * @return returns FW interface
     */
    msm_sock_interface<T>* get_fw_interface()
    {
        if (m_child_hier_socket)
        {
            return m_child_hier_socket->get_fw_interface();
        }
        return &m_fw_interface;
    }

    friend class msm_init_socket<T> ; //Declaring initiator socket as friend to access private members
    friend class msm_adapters::msm2aie_axis<T>;  //Making adapter friend class , so that the given
    //adapter can access the socket interface
    msm_targ_socket<T> *m_child_hier_socket;
    msm_sock_interface<T> *m_bw_interface; //!< Pointer to BW interface (implemented by initiator)
    msm_sock_interface<T> m_fw_interface; //!< Instance of FW interface (initiator will have reference)

};

#define REGISTER_TRANPORT_DBG(transport_dbg_func, target_socket) \
	target_socket.register_transport_dbg(&MSM_CURRENT_USER_MODULE::transport_dbg_func, this, #transport_dbg_func)
/**
 * @class msm_init_socket is initiator socket
 * @tparam T is payload data type
 */
template<class T>
class msm_init_socket: public msm_socket_base
{
public:

    /*!
     * @brief Constructor
     * @param sock_name sock_name is instance name of the socket
     */
    msm_init_socket(msm_module_name sock_name);
    virtual ~msm_init_socket() = default;

    /*!
     * @brief API to check whether the target is ready to accept payload or not
     * @return true/false
     */
    bool is_target_ready();

    /*!
     * @brief API to apply back pressure to the target. Target isn't expected to send response
     * from next clock cycle.
     * For those who are aware with AXI Stream, the functionality is similar to de-asserting TREADY
     * signal on AXI Stream interface
     * @param ready_val value to be set. true-> accepts transactions, false-> otherwise
     */
    void set_ready(bool ready_val);

    /*!
     * @brief API to transport payload to the target
     * @param payload is payload to be transported to the target
     */
    void transport(std::shared_ptr<T> payload);

    /*!
     * @brief API to check if the transaction is available to sample (from target)
     * @return true/false
     */
    bool is_transaction_available();

    /*!
     * @brief API to sample_transaction from target
     * @return returns sampled transaction
     */
    std::shared_ptr<T> sample_transaction();

    /**
     * @brief an API to peek the transaction
     * @return returns a pointer to transaction (in read-only format)
     */
    const std::shared_ptr<const T> peek_transaction();
    /*!
     * @brief API to bind target socket
     * @param targ_socket is target socket to be bound to the current initiator
     */
    virtual void bind(msm_targ_socket<T> &targ_socket);

    /*!
     * @brief API to make hierarchical binding with the parent initiator socket.
     * @param parent_hier_socket is the parent initiator socket
     */
    virtual void bind(msm_init_socket<T> &parent_hier_socket);

    /**
     * @brief parentheses operator overload for bind functionality
     * @param targ_socket the target socket to bind
     */
    virtual void operator() (msm_targ_socket<T> &targ_socket)
    {
        bind(targ_socket);
    }

    /**
     * @brief parentheses operator overload for bind functionality
     * @param parent_hier_socket is the parent initiator socket to bind
     */
    virtual void operator() (msm_init_socket<T> &parent_hier_socket)
    {
        bind(parent_hier_socket);
    }

    /**
     * @brief API to make debug transport calls (Backdoor access).
     * This will not advance simulation clocks
     * @param payload is a shared_ptr of the transaction
     * @return returns number of bytes updated/modified/read.
     */
    unsigned int transport_dbg(std::shared_ptr<T> &payload)
    {
        if (m_fw_interface)
            return m_fw_interface->execute_transport_dbg(payload);
        else
            MSM_KERNEL_FATAL(this->name() << ": transport_dbg called when the socket isn't bound!");
        return 0;
    }

    /**
     * @brief API to check if the interface is bound or not, implements pure virtual function
     */
    virtual bool is_bound() const override
    {
        if(m_fw_interface && m_child_hier_socket)
            MSM_KERNEL_FATAL(
                    this->name() << " has binding issue . Please check hierarchy from source to destination" << " FW IF " << m_fw_interface->name() << " it's child " << m_child_hier_socket->name());

        if (m_fw_interface)
        {
            MSM_KERNEL_DEBUG(this->name() << " is bound with intf " << m_fw_interface->name());
        }
        else if (m_child_hier_socket)
        {
            MSM_KERNEL_DEBUG(this->name() << " is bound with port" << m_child_hier_socket->name());
        }
        else
            MSM_KERNEL_DEBUG(this->name() << " isn't bound");
        return m_fw_interface || m_child_hier_socket;
    }

    /**
     * @brief Get the forward interface pointer to which this socket is bound
     * @return returns the forward interface pointer
     */
    msm_sock_interface<T>* get_fw_interface() const
    {
        return m_fw_interface;
    }

public: // TODO - Make it private
    friend class msm_targ_socket<T> ;

    msm_sock_interface<T> m_bw_interface; //!<Instance of BW interface, initiator will have pointer
    msm_sock_interface<T> *m_fw_interface; //!<Pointer of the FW interface (implemented by initiator)
    msm_init_socket<T> *m_child_hier_socket; //!< Hierarchical pointer for child socket
};

template<class T>
inline bool msm_core::msm_init_socket<T>::is_target_ready()
{
    //When target has valid data it's not ready to accept new data
    return m_fw_interface->is_ready();
}

template<class T>
inline bool msm_core::msm_init_socket<T>::is_transaction_available()
{
    //When backward interface has valid data
    return m_bw_interface.is_valid();
}

template<class T>
inline void msm_core::msm_init_socket<T>::transport(std::shared_ptr<T> payload)
{
    //Send the data to forward interface
    m_fw_interface->write(payload);
}

template<class T>
inline std::shared_ptr<T> msm_core::msm_init_socket<T>::sample_transaction()
{
    return m_bw_interface.read();
}

template<class T>
inline const std::shared_ptr<const T> msm_init_socket<T>::peek_transaction()
{
    return m_bw_interface.peek();
}

template<class T>
inline msm_init_socket<T>::msm_init_socket(msm_module_name sock_name) :
        msm_socket_base(sock_name), m_fw_interface(nullptr), m_bw_interface("bw_intf"), m_child_hier_socket(
                nullptr)
{
}

template<class T>
inline msm_core::msm_targ_socket<T>::msm_targ_socket(msm_module_name sock_name) :
        msm_socket_base(sock_name), m_bw_interface(nullptr), m_fw_interface("fw_intf"), m_child_hier_socket(
                nullptr)
{
}

template<class T>
inline bool msm_core::msm_targ_socket<T>::is_init_ready()
{
    return m_bw_interface->is_ready();
}

template<class T>
inline bool msm_core::msm_targ_socket<T>::is_transaction_available()
{
    return m_fw_interface.is_valid();
}

template<class T>
inline void msm_core::msm_targ_socket<T>::transport(std::shared_ptr<T> payload)
{
    m_bw_interface->write(payload);
}

template<class T>
inline std::shared_ptr<T> msm_core::msm_targ_socket<T>::sample_transaction()
{
    return m_fw_interface.read();
}

template<class T>
inline const std::shared_ptr<const T> msm_targ_socket<T>::peek_transaction()
{
    return m_fw_interface.peek();
}

template<class T>
inline void msm_targ_socket<T>::set_ready(bool ready_val)
{
    m_fw_interface.set_ready(ready_val);
}

template<class T>
inline void msm_core::msm_targ_socket<T>::bind(msm_targ_socket<T> &child_socket)
{
    if(this == & child_socket)
        MSM_KERNEL_FATAL(this->name() << " bound to itself " );

    MSM_KERNEL_DEBUG( "Parent " << this->name() << " Child " << child_socket.name() );
    //Add the received child socket as child to current socket. This establishes hierarchy
    m_child_hier_socket = &child_socket;
}

template<class T>
inline void msm_core::msm_init_socket<T>::bind(msm_init_socket<T> &parent_hier_socket)
{
    if(this == & parent_hier_socket)
        MSM_KERNEL_FATAL(this->name() << " bound to itself " );
    MSM_KERNEL_DEBUG("Parent " << parent_hier_socket.name() << " Child " <<  this->name() );
    //Add the current socket as child socket to the parent socket. This establishes hierarchy
    parent_hier_socket.m_child_hier_socket = this;
}

template<class T>
inline void msm_core::msm_init_socket<T>::set_ready(bool ready_val)
{
    m_bw_interface.set_ready(ready_val);
}

template<class T>
inline void msm_core::msm_init_socket<T>::bind(msm_targ_socket<T> &targ_socket)
{
    //If this socket has child socket (i.e., hierarchical socket) then bind the target with child
    if (m_child_hier_socket)
    {
        MSM_KERNEL_DEBUG(this->name() << "hierarchically binding to " << targ_socket.name() );
        m_child_hier_socket->bind(targ_socket);
    }
    else
    {
        MSM_KERNEL_DEBUG(this->name() <<  " trying to get interface of " << targ_socket.name() );
        //If this is not hierarchical socket then bind target to the current socket
        m_fw_interface = targ_socket.get_fw_interface();
        targ_socket.set_bw_interface(&m_bw_interface);
    }
}

} /* namespace msm_core */

