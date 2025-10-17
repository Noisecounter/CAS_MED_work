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

#include "msm_interface.h"
#include "msm_module_name.h"
namespace msm_core {
template<class T>
class msm_init_aximm_write_if : public virtual msm_interface_base
{ 
public: 
	virtual void send_resp(std::shared_ptr<T>& trans) =0;
    virtual void send_addr_ack()=0;
	virtual void send_addr_busy()=0;
    virtual void send_trans_ack()=0;
	virtual void send_trans_busy()=0;
	virtual void send_data_ack()=0;
	virtual void send_data_busy()=0;
	msm_init_aximm_write_if(const msm_module_name &nm) : msm_interface_base(nm) {}
}; 
template<class T>
class msm_targ_aximm_write_if : public virtual msm_interface_base
{ 
public: 
    virtual void send_addr_ch(std::shared_ptr<T> &trans) =0;
	virtual void send_beat_data(std::shared_ptr<T> &trans)=0;
	virtual void send_resp_ack()=0;
	virtual void send_transaction(std::shared_ptr<T> &trans)=0;
	msm_targ_aximm_write_if(const msm_module_name &nm) : msm_interface_base(nm) {}
};
template<class T>
class msm_init_aximm_read_if : public virtual msm_interface_base
{ 
public: 
	virtual void send_beat_data(std::shared_ptr<T> &trans)=0;
    virtual void send_addr_ack()=0;
	virtual void send_addr_busy()=0;
 	virtual void send_trans_ack()=0;
	virtual void send_trans_busy()=0;
	virtual void send_data(std::shared_ptr<T> &trans)=0;
	msm_init_aximm_read_if(const msm_module_name &nm) : msm_interface_base(nm) {}
};
template<class T>
class msm_targ_aximm_read_if : public virtual msm_interface_base
{ 
public: 
	virtual void send_addr(std::shared_ptr<T>& trans)=0;
	virtual void send_data_ack()=0;
	virtual void send_beat_ack()=0;
	virtual void send_data_busy()=0;
	virtual void send_transaction(std::shared_ptr<T> &trans)=0;
	msm_targ_aximm_read_if(const msm_module_name &nm) : msm_interface_base(nm) {}
};

} // namespace msm_core

