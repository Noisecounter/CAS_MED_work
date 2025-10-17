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

#include "msm_signal.h"

namespace msm_core
{
template<class T>
class msm_signal_in_if: public virtual msm_interface_base
{    
	public:
	msm_signal_in_if(const msm_module_name &port_name) : msm_interface_base(port_name) {
	}
	virtual ~msm_signal_in_if() = default;

	/*!
     * @brief API to write given data to interface
     * @param data is data to be updated on the interface
     */

     virtual const T& read() const =0;
};

template<class T>
class msm_signal_out_if: public virtual msm_interface_base
{    
	public:
	msm_signal_out_if(const msm_module_name &port_name) : msm_interface_base(port_name) {
	}
	virtual ~msm_signal_out_if() = default;

	/*!
     * @brief API to read data from interface
     * @return returns data on the interface
     */
     virtual void write(const T &data) =0;
};
template<class T>
class msm_signal_inout_if : public virtual msm_signal_out_if<T>, public virtual msm_signal_in_if<T>, public virtual msm_interface_base
{
	public:
	msm_signal_inout_if(const msm_module_name &port_name) : msm_signal_out_if<T>(port_name) , msm_signal_in_if<T>(port_name), msm_interface_base(port_name) {
	}
	virtual ~msm_signal_inout_if() = default;
};

template<class T>
class msm_signal: public virtual msm_signal_inout_if<T>, public msm_prim_channel
{
public:

	/**
	 * @brief API to read the updated value of the interface
	 * @return returns updated interface value
	 */
	const T& read() const;

	/**
	 * @brief API to write data to the interface
	 * @param data is data to write to the interface
	 */
	void write(const T &data);

	/**
	 * @brief Constructor
	 * @param interface_name
	 */
	msm_signal(const msm_module_name &interface_name, T init_value = T());

	virtual ~msm_signal() = default;

	/**
	 * @brief Operator overload for assignment with interface value
	 * @param data is the value of the interface to be updated
	 */
    msm_signal& operator=(const T &data)
    {
        this->write(data);
        return *this;
    }

    /**
     * @brief conversion to bool for != and implicit if/while statement bool conversions
     */
    explicit operator bool() const
    {
        // != operator is used to handle std::bitset<N> cases.
        // As bitset is not providing bool operator
        return updated_value != 0;
    }

    /**
     * @brief operator overload for equality checking. Checks the given values with the updated
     * interface value
     * @param data is the data to be compared with
     * @return true/false -> equal / not equal
     */
    bool operator ==(const T &data) const
    {
        return updated_value == data;
    }

    /**
     * @brief Operator overload for inequality check
     * @param data is data to be compared against
     * @return true/false -> equal / not equal
     */
    bool operator !=(const T &data) const
    {
        return updated_value != data;
    }

    //TODO Add private here
	/**
	 * @brief Implements update behavior
	 */
	bool update() override;

	friend class msm_adapters::sc2msm<T>;

	T value;
	T updated_value;
};

//Operator overload to support std::cout
template<class T>
std::ostream& operator <<(std::ostream &os, const msm_signal<T> &intf)
{
    os << intf.read();
    return os;
}

template<class T>
inline bool msm_core::msm_signal<T>::update()
{
    if (updated_value == value)
        return false;

    MSM_KERNEL_DEBUG(this->name() << " Updating from " << updated_value << " to " << value);
    updated_value = std::move(value);
    return true;
}

template<class T>
inline const T& msm_core::msm_signal<T>::read() const
{
	return updated_value;

}

template<class T>
inline void msm_core::msm_signal<T>::write(const T &data)
{
    //TODO : Multiple writers to same channel isn't allowed. Should be handled in infra.
    //Probably during port binding
	value = data;
	request_update();
}

template<class T>
inline msm_core::msm_signal<T>::msm_signal(const msm_module_name &interface_name, T init_value) :
		msm_prim_channel(interface_name), msm_signal_inout_if<T>(interface_name), msm_signal_in_if<T>(interface_name), msm_signal_out_if<T>(interface_name),
		msm_interface_base(interface_name), value(init_value), updated_value(init_value)
{
}


}
