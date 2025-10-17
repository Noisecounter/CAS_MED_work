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

#include <vector>

#include "msm_interface.h"
#include "msm_module_name.h"
#include "msm_object.h"
#include "msm_debug.h"
#include "msm_signal.h"
#include "msm_port_sock_base.h"

/*
 * Forward declaration for MSM adapters, so that adapters can call private functions
 * of the port by making adapter as friend of ports
 */
namespace msm_adapters
{
template<class > class sc2msm;
}

namespace msm_core
{
template<class IF>
class msm_port : public msm_port_export_base<IF>
{
public:
    
	 msm_port(const msm_module_name &port_name) :
            msm_port_export_base<IF>(port_name)
    {
    }

    virtual ~msm_port() = default;
};

template<class IF>
class msm_export : public msm_port_export_base<IF>
{
public:
    msm_export(const msm_module_name &port_name) :
            msm_port_export_base<IF>(port_name)
    {
	}

    virtual ~msm_export() = default;
};

template<class T>
class msm_out: public msm_port<msm_signal<T> >
{
public:
    msm_out(const msm_module_name &port_name) :
            msm_port<msm_signal<T> >(port_name)
    {
	}
    void write(const T &value) {
     return (*this)->write(value);
    }
    const T& read() {
     return (*this)->read();
    }



};

template<class T>
class msm_in: public msm_port<msm_signal<T> >
{
public:
    msm_in(const msm_module_name &port_name) :
            msm_port<msm_signal<T> >(port_name)
    {
	}
    const T& read() {
     return (*this)->read();
    }
    void write(const T &value) {
     return (*this)->write(value);
    }
};

} /* namespace msm_core */
