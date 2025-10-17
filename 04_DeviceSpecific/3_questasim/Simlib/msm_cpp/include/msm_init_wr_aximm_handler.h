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
#include <queue>

#include "msm_interface.h"
#include "msm_aximm_if.h"
#include "msm_module_name.h"
#include "msm_signal.h"
namespace msm_core
{
template<class T>
class msm_aximm_initiator_wr_socket_handler: public virtual msm_prim_channel , public virtual msm_init_aximm_write_if<T>
{
public:

	 msm_aximm_initiator_wr_socket_handler(const msm_module_name interface_name, const unsigned int max_fifo_depth=1) :
		msm_prim_channel(interface_name), msm_init_aximm_write_if<T>(interface_name),
		msm_interface_base(interface_name), resp_available("wr_resp_available"), addr_sampled("addr_sampled"), beat_sampled("beat_sampled") , init_wr_socket("init_wr_socket"), trans_sampled("trans_sampled"), IMPL_LEVEL("TRANSACTION"), TARG_IMPL_LEVEL("")

	 {
	 	init_wr_socket.bind(*this);
	 }
	
	 /*Pure Virtual Function of Interface*/ 
	 virtual ~msm_aximm_initiator_wr_socket_handler() = default;
	 virtual void send_resp(std::shared_ptr<T>& trans);
     virtual void send_addr_ack();
	 virtual void send_addr_busy();
	 virtual void send_data_ack();
	 virtual void send_data_busy();
     virtual void send_trans_ack();
	 virtual void send_trans_busy();
	 
     /*Helper functions */
	 bool is_addr_ch_ready();
	 bool is_data_ch_ready();
	 bool is_resp_available();	
	 std::shared_ptr<T> get_resp();
	 std::shared_ptr<T>& peek_resp();
	 bool is_slave_ready();
	 void reset();
     msm_core::msm_aximm_initiator_socket<msm_targ_aximm_write_if<T>,msm_init_aximm_write_if<T> > init_wr_socket;
	 msm_core::msm_signal<bool> resp_available, beat_sampled, addr_sampled, trans_sampled;
     /**
	 * @brief Implements update behavior
	 */
	private:
    bool update() override;
	std::queue<std::shared_ptr<T>> new_wr_rsp_value;	//!< 1 address channel. 1 request can be served at a time.
	std::queue<std::shared_ptr<T>> updated_wr_rsp_value;	//!< Updated values in the fifo
    std::atomic<int> update_requested = 0; //!< To make sure we request update only once using
	unsigned int max_fifo_depth =1;
	bool addr_channel_available=true, data_channel_available=true, trans_channel_available=true;
	std::string IMPL_LEVEL, TARG_IMPL_LEVEL;
};

} /* namespace msm_core */

template<class T>
inline bool msm_core::msm_aximm_initiator_wr_socket_handler<T>::update()
{
    bool ret_value = false;
    //Move the new values to udpated_values FIFO
    if (!new_wr_rsp_value.empty() && (updated_wr_rsp_value.size() < max_fifo_depth))
    {
        while(updated_wr_rsp_value.size() < max_fifo_depth && (!new_wr_rsp_value.empty()))
	    {
            updated_wr_rsp_value.push(new_wr_rsp_value.front());
            new_wr_rsp_value.pop();
	    }
        ret_value = true;
	}
    update_requested = 0;
    return ret_value;
}

template<class T>
inline void msm_core::msm_aximm_initiator_wr_socket_handler<T>::send_resp(std::shared_ptr<T>& trans)
{
    if ( trans != nullptr ) {
	new_wr_rsp_value.push(trans); //in the update phase only the next addr can come  
	resp_available.write(!resp_available.read());
    }
    if ( update_requested++ == 0 ) {
    	request_update();
    }
}

template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_initiator_wr_socket_handler<T>::get_resp()
{
	auto ptr = updated_wr_rsp_value.front();
	if ( ptr != nullptr ) {
         updated_wr_rsp_value.pop();
		 init_wr_socket->send_resp_ack();
	}
    return ptr; //in the update phase only the next addr can come  
}
template<class T>
inline std::shared_ptr<T>& msm_core::msm_aximm_initiator_wr_socket_handler<T>::peek_resp()
{
    return updated_wr_rsp_value.front(); //in the update phase only the next addr can come  
}	
template<class T>
bool msm_core::msm_aximm_initiator_wr_socket_handler<T>::is_resp_available()
{
    return !updated_wr_rsp_value.empty();
}

template<class T>
inline void msm_core::msm_aximm_initiator_wr_socket_handler<T>::send_addr_ack()
{
		addr_channel_available=true;  
		addr_sampled.write(!addr_sampled.read());   
}
template<class T>
inline void msm_core::msm_aximm_initiator_wr_socket_handler<T>::send_addr_busy()
{
		addr_channel_available=false;  
}
/******Tx addr handling***********************************/
template<class T>
inline bool msm_core::msm_aximm_initiator_wr_socket_handler<T>::is_slave_ready()
{
    return trans_channel_available; //need to handle for handshake
}

template<class T>
inline void msm_core::msm_aximm_initiator_wr_socket_handler<T>::send_trans_ack()
{
		trans_channel_available=true;  
		trans_sampled.write(!trans_sampled.read());   
}
template<class T>
inline void msm_core::msm_aximm_initiator_wr_socket_handler<T>::send_trans_busy()
{
		trans_channel_available=false;  
}

template<class T>
inline void msm_core::msm_aximm_initiator_wr_socket_handler<T>::send_data_ack()
{
		beat_sampled.write(!beat_sampled.read());
		data_channel_available=true;  
}
template<class T>
inline void msm_core::msm_aximm_initiator_wr_socket_handler<T>::send_data_busy()
{
		data_channel_available=false;  
}
template<class T>
inline bool msm_core::msm_aximm_initiator_wr_socket_handler<T>::is_addr_ch_ready()
{
			 return addr_channel_available; //in the update phase only the next addr can come  
}
template<class T>
inline bool msm_core::msm_aximm_initiator_wr_socket_handler<T>::is_data_ch_ready()
{
			 return data_channel_available; //in the update phase only the next addr can come  
}
template<class T>
inline void msm_core::msm_aximm_initiator_wr_socket_handler<T>::reset()
{
	addr_channel_available=true;
	data_channel_available=true;
}
