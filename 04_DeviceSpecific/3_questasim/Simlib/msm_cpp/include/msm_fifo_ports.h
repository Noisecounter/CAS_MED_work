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

#include "msm_fifo.h"
#include "msm_module_name.h"
#include "msm_object.h"
#include "msm_port_sock_base.h"

namespace msm_core
{
/**
 * @class An FIFO out port
 * @tparam template param T is a type of payloads on the port
 */
#if 0
template<class T>
class msm_fifo_out: public msm_fifo_base , public msm_port<msm_fifo_out_if<T> >
{
public:
	msm_fifo_out(const msm_module_name port_name) :
			msm_fifo_base(port_name), (*this)(nullptr), msm_port<msm_fifo_out_if<T> >(port_name), m_hierarchical_port(nullptr)
	{
	}

	//Implements bind functionality
	/**
	 * @brief API to bind FIFO port with an FIFO interface
	 * @param intf is the interface to bound
	 */
	void bind(msm_prim_channel &intf) override
	{
		auto interface = dynamic_cast<msm_fifo<T>*>(&intf);
		if (!interface)
			MSM_KERNEL_ERROR(
					this->name() + " is trying to bind with " + intf.name()
							+ " which isn't compatible");

		//At this point, It's a valid interface, we need to bind the interface to the current port
		//or it's child port based on whether it's non-hierarchical port or not
		if (m_hierarchical_port)
			m_hierarchical_port->bind(intf);
		else
			(*this) = interface;
	}

	/**
	 * @brief Overloaded bind function for hierarchical binding
	 * @param parent_hier_port is the parent port to which current port need to bound hierarchically
	 */
	void bind(msm_fifo_out<T> &parent_hier_port)
	{
		parent_hier_port.m_hierarchical_port = this;
	}

	bool is_bound() const override
	{
		//The current is considered to be bound, if it's bound to interface or to an hierarchical port
		return ((*this) != nullptr) || (m_hierarchical_port != nullptr);
	}

	/**
	 * @brief API to write the payload to underneath interface
	 * @param payload_ptr is the shared_ptr of payload to be written to FIFO
	 * @return Returns if write is successful or not
	 */
	bool write(std::shared_ptr<T> payload_ptr)
	{
		if ((*this))
			return (*this)->write(payload_ptr);
		else
			MSM_KERNEL_ERROR(this->name() << " trying to write to a null interface!");
		return false;
	}

	/**
	 * @brief API to check if Underneath FIFO is full or not
	 * @return returns true if FIFO is full else false
	 */
	const bool is_full() const
	{
		if ((*this))
			return (*this)->is_full();
		else
			MSM_KERNEL_ERROR(this->name() << " trying to read status from unbound port!");
	}
private:
	msm_fifo<T> *(*this);
	msm_fifo_out<T> *m_hierarchical_port;
};
#endif
/**
 * @class An FIFO in port
 * @tparam template param T is a type of payloads on the port
 */
template<class T>
class msm_fifo_in: public virtual msm_port<msm_fifo<T> > 
{
public:
	msm_fifo_in(const msm_module_name &port_name) :
		 msm_port<msm_fifo<T> >(port_name)
	{
	}

	virtual ~msm_fifo_in() = default;

	/**
	 * @brief API to bind FIFO port with an FIFO interface
//	 * @param intf is the interface to bound
//	 */
//	void bind(msm_prim_channel &intf) override
//	{
//		auto interface = dynamic_cast<msm_fifo<T>*>(&intf);
//		if (!interface)
//			MSM_KERNEL_ERROR(
//					this->name() + " is trying to bind with " + intf.name()
//							+ " which isn't compatible");
//		//At this point, It's a valid interface, we need to bind the interface to the current port
//		//or it's child port based on whether it's non-hierarchical port or not
//		if (m_hierarchical_port)
//			m_hierarchical_port->bind(intf);
//		else
//			(*this) = interface;
//	}

	/**
	 * @brief Overloaded bind function for hierarchical binding
	 * @param child_port is the child port to which current port need to bound hierarchically
	 */
//	void bind(msm_fifo_in<T>& child_port)
//	{
//		m_hierarchical_port = & child_port;
//	}

	/**
	 * @brief API to read data from underneath FIFO
	 * @return returns shared_ptr of payload from FIFO
	 */
	const std::shared_ptr<T> read(MSM_THREAD_ARGUMENT)
	{
		return (*this)->read(MSM_THREAD_CASCADE);
	}
	void  read ( const std::shared_ptr<T>& val , MSM_THREAD_ARGUMENT )
	{
		return (*this)->read(val, MSM_THREAD_CASCADE);
	}
    int num_available() const {
		return (*this)->num_available();

	}
    int nb_read(std::shared_ptr<T> &val_) {
		return (*this)->nb_read(val_);
	}

	/**
	 * @brief API to check if the FIFO is empty
	 * @return return true if the FIFO to read is empty else false
	 */
	const bool empty() const
	{
		return (*this)->empty();
	}

};

} /* namespace msm_core */
