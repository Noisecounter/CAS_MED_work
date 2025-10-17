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
class msm_aximm_target_wr_socket_handler: public virtual msm_prim_channel , public virtual msm_targ_aximm_write_if<T>
{
public:

	 msm_aximm_target_wr_socket_handler(const msm_module_name interface_name, std::string impl_level="TRANSACTION", const unsigned int max_fifo_depth=1) :
		msm_prim_channel(interface_name), msm_targ_aximm_write_if<T>(interface_name), msm_interface_base(interface_name), 
		addr_available("wr_addr_available"), beat_available("wr_beat_available") , resp_sampled("resp_sampled"), targ_wr_socket("targ_wr_socket"), transaction_available("transaction_available")
		, IMPL_LEVEL(impl_level), INIT_IMPL_LEVEL("")
	 {
				targ_wr_socket.bind(*this);
	 }
	 
	 virtual ~msm_aximm_target_wr_socket_handler() = default;
     
	 /*Pure Virtual Function of Interface*/
	 virtual void send_addr_ch(std::shared_ptr<T> &trans);
	 virtual void send_beat_data(std::shared_ptr<T> &trans);
	 virtual void send_resp_ack();
	 virtual void send_transaction(std::shared_ptr<T> &trans);
	 bool is_data_ch_available();
	 bool is_addr_ch_available();
     bool is_beat_available();
     bool is_trans_available();
	 msm_core::msm_signal<bool> addr_available, beat_available, resp_sampled, transaction_available;
	 std::shared_ptr<T> get_beat_data();
	 std::shared_ptr<T> get_addr_ch();
     std::shared_ptr<T> get_transaction();
	 std::shared_ptr<T> peek_addr_ch();
	 std::shared_ptr<T> peek_beat_data();
	 bool is_last_beat();
	 bool is_master_ready();
	 void reset();
	 msm_core::msm_aximm_target_socket<msm_targ_aximm_write_if<T>, msm_init_aximm_write_if<T>> targ_wr_socket;

     /**
	 * @brief Implements update behavior
	 */
    private:
	bool update() override;
	unsigned int nb_peek_count{0};
	std::queue<std::shared_ptr<T>> new_wr_addr_value;	//!< 1 address channel. 1 request can be served at a time.
	std::queue<std::shared_ptr<T>> updated_wr_addr_value;	//!< Updated values in the fifo
	std::queue<std::shared_ptr<T>> new_wr_data_value;	//!< New values to be updated
	std::queue<std::shared_ptr<T>> updated_wr_data_value;	//!< Updated values in the fifo
    std::queue<std::shared_ptr<T>> new_wr_trans_value;	//!< 1 address channel. 1 request can be served at a time.
	std::queue<std::shared_ptr<T>> updated_wr_trans_value;	//!< Updated values in the fifo
	std::atomic<int> update_requested = 0; //!< To make sure we request update only once using
    std::map<std::shared_ptr<T>, int> num_writes;	
	unsigned int max_fifo_depth=1;
	bool resp_channel_available=true;
	std::string IMPL_LEVEL, INIT_IMPL_LEVEL;
	std::shared_ptr<T> trans_in_progress;

};

} /* namespace msm_core */

template<class T>
inline bool msm_core::msm_aximm_target_wr_socket_handler<T>::update()
{
    bool ret_value = false;
	if (!new_wr_data_value.empty() && (updated_wr_data_value.size() < max_fifo_depth))
    {
        while(updated_wr_data_value.size() < max_fifo_depth && (!new_wr_data_value.empty()))
	    {
            updated_wr_data_value.push(new_wr_data_value.front());
            new_wr_data_value.pop();
	    }
        ret_value = true;
	}
    if (!new_wr_addr_value.empty() && (updated_wr_addr_value.size() < max_fifo_depth))
    {
        while(updated_wr_addr_value.size() < max_fifo_depth && (!new_wr_addr_value.empty()))
	    {
            updated_wr_addr_value.push(new_wr_addr_value.front());
            new_wr_addr_value.pop();
	    }
        ret_value = true;
	}
    if (!new_wr_trans_value.empty() && (updated_wr_trans_value.size() < max_fifo_depth))
    {
        while(updated_wr_trans_value.size() < max_fifo_depth && (!new_wr_trans_value.empty()))
	    {
            updated_wr_trans_value.push(new_wr_trans_value.front());
            new_wr_trans_value.pop();
	    }
        ret_value = true;
	}

	update_requested = 0;
    return ret_value;
}
//called by init. ARVALID
template<class T>
inline void msm_core::msm_aximm_target_wr_socket_handler<T>::send_addr_ch(std::shared_ptr<T> &trans)
{
	if ( trans != NULL ) {
    	new_wr_addr_value.push(trans);
    	addr_available.write(!addr_available.read());
		targ_wr_socket->send_addr_busy();
		if ( IMPL_LEVEL == "TRANSACTION" ) {
			new_wr_trans_value.push(trans);
    		transaction_available.write(!transaction_available.read());
		}
		 INIT_IMPL_LEVEL = "HANDSHAKE";
	}
	if ( update_requested++ == 0 ) {
         request_update();
    }

}
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_target_wr_socket_handler<T>::get_addr_ch()
{
    auto trans = updated_wr_addr_value.front();
    if ( trans != NULL ) {
        updated_wr_addr_value.pop();
    	targ_wr_socket->send_addr_ack();
		if ( IMPL_LEVEL == "HANDSHAKE" && INIT_IMPL_LEVEL == "TRANSACTION" ) {
			if ( num_writes[trans_in_progress] == 0 ) {
	  			new_wr_data_value.push(trans);
				beat_available.write(!beat_available.read()); //
				targ_wr_socket->send_trans_ack();
			} else {
	  			new_wr_data_value.push(trans);
				beat_available.write(!beat_available.read()); //
			}
			num_writes[trans] -= 1;
		}

	}
    return trans;
}
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_target_wr_socket_handler<T>::peek_addr_ch()
{
		     return updated_wr_addr_value.front(); //in the update phase only the next addr can come  
}
template<class T>
inline bool msm_core::msm_aximm_target_wr_socket_handler<T>::is_addr_ch_available()
{
		     //return (updated_wr_addr_value.front() != nullptr); //in the update phase only the next addr can come  
		     return (!updated_wr_addr_value.empty()); //in the update phase only the next addr can come  
}
template<class T>
inline void msm_core::msm_aximm_target_wr_socket_handler<T>::send_beat_data(std::shared_ptr<T>& trans)
{
	//If Init is at handshake level and target is at transaction level,in get_transaction itself all the work is done
	//give ACK to INIT without doing anything
	if ( IMPL_LEVEL == "TRANSACTION" ) {
			targ_wr_socket->send_data_ack();
			return;
	}
	if ( trans_in_progress == nullptr ) {
		trans_in_progress = trans;
	    num_writes[trans] = trans->get_burst_length();
	}
	num_writes[trans] -= 1;
	if (num_writes[trans] == 0) {
		new_wr_data_value.push(trans); //in the update phase only the next addr can come  
		beat_available.write(!beat_available.read());
		targ_wr_socket->send_data_busy();
	}
	if ( update_requested++ == 0 ) {
         request_update();
    }
}
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_target_wr_socket_handler<T>::get_beat_data()
{
		    auto trans = updated_wr_data_value.front();
			updated_wr_data_value.pop();
			targ_wr_socket->send_data_ack();
			if ( IMPL_LEVEL == "HANDSHAKE" && INIT_IMPL_LEVEL == "TRANSACTION" ) {
				if ( num_writes[trans_in_progress] == 0 ) {
	  				new_wr_data_value.push(trans);
					beat_available.write(!beat_available.read()); //
					targ_wr_socket->send_trans_ack();
				} else {
	  				new_wr_data_value.push(trans);
					beat_available.write(!beat_available.read()); //
				}
				num_writes[trans] -= 1;
			}
			return trans; //in the update phase only the next addr can come  
}
template<class T>
inline bool msm_core::msm_aximm_target_wr_socket_handler<T>::is_data_ch_available()
{
		     //return (updated_wr_data_value.front() != nullptr); //in the update phase only the next addr can come  
		     return (!updated_wr_data_value.empty()); //in the update phase only the next addr can come  
}
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_target_wr_socket_handler<T>::peek_beat_data()
{
		     return updated_wr_data_value.front(); //in the update phase only the next addr can come  
}
template<class T>
inline bool msm_core::msm_aximm_target_wr_socket_handler<T>::is_last_beat()
{
	bool status = true;
	if ( num_writes[trans_in_progress] == 0 ) {
	   num_writes.erase(trans_in_progress);
	   trans_in_progress = nullptr;
		status = true;
	} else {
		status = false;
	}
	return status;
}
template<class T>
inline bool msm_core::msm_aximm_target_wr_socket_handler<T>::is_trans_available()
{
    return !updated_wr_trans_value.empty();
}
template<class T>
inline bool msm_core::msm_aximm_target_wr_socket_handler<T>::is_beat_available()
{
     return !updated_wr_data_value.empty(); //in the update phase only the next addr can come  
}
template<class T>
inline bool msm_core::msm_aximm_target_wr_socket_handler<T>::is_master_ready()
{
             return resp_channel_available; //in the update phase only the next addr can come  
}

template<class T>
inline void msm_core::msm_aximm_target_wr_socket_handler<T>::send_resp_ack()
{
		resp_sampled.write(!resp_sampled.read());
		resp_channel_available=true;  
}
//called by init. ARVALID
template<class T>
inline void msm_core::msm_aximm_target_wr_socket_handler<T>::send_transaction(std::shared_ptr<T> &trans)
{
	if ( trans != nullptr ) {
		 if ( IMPL_LEVEL == "TRANSACTION" ) {
		 	new_wr_trans_value.push(trans);
		 	transaction_available.write(!transaction_available.read());
		 }
		//when init is at TRANSACTION level and target is at HANDSHAKE level then init call will land here 
		 if ( IMPL_LEVEL == "HANDSHAKE" ) {
			if ( trans_in_progress == nullptr ) {
	  			trans_in_progress = trans;
				num_writes[trans] = trans->get_burst_length();
	  	 	}
	  	 	num_writes[trans] -= 1;
		 	new_wr_addr_value.push(trans);
			addr_available.write(!addr_available.read()); 
		 }
		 INIT_IMPL_LEVEL = "TRANSACTION";
		 targ_wr_socket->send_trans_busy();
	}
	if ( update_requested++ == 0 ) {
		request_update();
	}
}
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_target_wr_socket_handler<T>::get_transaction()
{
    auto trans = updated_wr_trans_value.front();
	if ( trans != NULL ) {
		updated_wr_trans_value.pop();
		if ( INIT_IMPL_LEVEL == "TRANSACTION" ) {
			targ_wr_socket->send_trans_ack();
		}
		if ( INIT_IMPL_LEVEL == "HANDSHAKE" ) {
			targ_wr_socket->send_addr_ack();
		}
	}
		return trans;
}

template<class T>
inline void msm_core::msm_aximm_target_wr_socket_handler<T>::reset()
{
	resp_channel_available=true;	
	num_writes.clear();
}
