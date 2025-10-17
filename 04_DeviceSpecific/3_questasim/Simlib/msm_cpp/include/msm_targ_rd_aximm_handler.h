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
#include "msm_initiator_target_sockets.h"
namespace msm_core
{

template<class T>
class msm_aximm_target_rd_socket_handler: public virtual msm_prim_channel , public virtual msm_targ_aximm_read_if<T>
{
public:

	 msm_aximm_target_rd_socket_handler(const msm_module_name interface_name, std::string impl_level="TRANSACTION", const unsigned int max_fifo_depth=1) :
		msm_prim_channel(interface_name), msm_targ_aximm_read_if<T>(interface_name),
		msm_interface_base(interface_name), beat_sampled("rd_beat_sampled"), addr_available("rd_addr_available"), targ_rd_socket("targ_rd_socket"), transaction_available("transaction_available"), data_sampled("data_sampled"), IMPL_LEVEL(impl_level), INIT_IMPL_LEVEL("")
	 {
	 	targ_rd_socket.bind(*this);
	 }
	
	 
	 virtual ~msm_aximm_target_rd_socket_handler() = default;
	 /***********************************************
	  * Pure Virtual Functions of Interface should be 
	  * called by sockets
	  * **********************************************/ 
	 
	 virtual void send_addr(std::shared_ptr<T>& trans); //for HANDSHAKE
	 virtual void send_transaction(std::shared_ptr<T>& trans); //for TRANSACTION
	 
	 //Internal functions called by init handler
	 virtual void send_data_ack();
	 virtual void send_beat_ack();
	 virtual void send_data_busy();

	 /*Helper functions for channel*/
	 bool is_trans_available(); //for TRANSACTION level
	 std::shared_ptr<T> get_addr_ch(); //for HANDSHAKE level
	 std::shared_ptr<T> get_transaction(); //for HANDSHAE level
	 std::shared_ptr<T> peek_addr_ch(); //for HANDSHAKE level
	 bool is_data_ch_ready();  //for HANDSHAKE level
     bool is_addr_ch_available(); //for HANDSHAKE level
     bool is_master_ready();
	 void reset();
	 std::string IMPL_LEVEL, INIT_IMPL_LEVEL;
     msm_core::msm_signal<bool> beat_sampled, addr_available, transaction_available, data_sampled;
	 msm_core::msm_aximm_target_socket<msm_targ_aximm_read_if<T>, msm_init_aximm_read_if<T>> targ_rd_socket;
	
     /**
	 * @brief Implements update behavior
	 */
    private:
	bool update() override;
	std::queue<std::shared_ptr<T>> new_rd_addr_value;	//!< 1 address channel. 1 request can be served at a time.
	std::queue<std::shared_ptr<T>> updated_rd_addr_value;	//!< Updated values in the fifo
	std::queue<std::shared_ptr<T>> new_rd_trans_value;	//!< 1 address channel. 1 request can be served at a time.
	std::queue<std::shared_ptr<T>> updated_rd_trans_value;	//!< Updated values in the fifo
	std::atomic<int> update_requested = 0; //!< To make sure we request update only once using
	bool data_channel_available=true;
	unsigned int max_fifo_depth =1;
};

} /* namespace msm_core */

template<class T>
inline bool msm_core::msm_aximm_target_rd_socket_handler<T>::update()
{
    bool ret_value = false;
    //Move the new values to udpated_values FIFO
    if (!new_rd_addr_value.empty() && (updated_rd_addr_value.size() < max_fifo_depth))
    {
        while(updated_rd_addr_value.size() < max_fifo_depth && (!new_rd_addr_value.empty()))
	    {
            updated_rd_addr_value.push(new_rd_addr_value.front());
            new_rd_addr_value.pop();
	    }
        ret_value = true;
	}
      //Move the new values to udpated_values FIFO
    if (!new_rd_trans_value.empty() && (updated_rd_trans_value.size() < max_fifo_depth))
    {
        while(updated_rd_trans_value.size() < max_fifo_depth && (!new_rd_trans_value.empty()))
	    {
            updated_rd_trans_value.push(new_rd_trans_value.front());
            new_rd_trans_value.pop();
	    }
        ret_value = true;
	}
	update_requested = 0;
    return ret_value;
}
/*************************************************
 **************HANDSHAKE LEVEL*******************
 ************************************************/

/**Called by init handler at HANDSHAKE level to send new addr**/
template<class T>
inline void msm_core::msm_aximm_target_rd_socket_handler<T>::send_addr(std::shared_ptr<T> &trans)
{
	if ( trans != nullptr ) {
		 if ( IMPL_LEVEL == "HANDSHAKE" ) {
		 	new_rd_addr_value.push(trans);
		 	addr_available.write(!addr_available.read());
		 	targ_rd_socket->send_addr_busy();
		 } else if  ( IMPL_LEVEL == "TRANSACTION" ) {
		 	new_rd_trans_value.push(trans);
		 	transaction_available.write(!transaction_available.read());
		 	targ_rd_socket->send_trans_busy();
		 }
		 INIT_IMPL_LEVEL = "HANDSHAKE";
	}
	if ( update_requested++ == 0 ) {
		request_update();
	}

}
/**Called by slave handler at HANDSHAKE level to get new addr**/
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_target_rd_socket_handler<T>::get_addr_ch()
{
    auto trans = updated_rd_addr_value.front();
	if ( trans != NULL ) {
		updated_rd_addr_value.pop();
		if ( INIT_IMPL_LEVEL == "HANDSHAKE" ) {
			targ_rd_socket->send_addr_ack();
		}
		if ( INIT_IMPL_LEVEL == "TRANSACTION") {
			targ_rd_socket->send_trans_ack(); //When init is at TRANSACTION level and Slave is at HANDSHAKE level, it need to get a notification that trans phase is completed. Next is data phase
		}
	}
	return trans;
}
/**Called by slave handler at HANDSHAKE level to peek latest addr**/
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_target_rd_socket_handler<T>::peek_addr_ch()
{
    auto trans = updated_rd_addr_value.front();
	return trans;
}

/**Called by slave handler at HANDSHAKE level to to check next addr can be received**/
template<class T>
inline bool msm_core::msm_aximm_target_rd_socket_handler<T>::is_addr_ch_available()
{
	return (!updated_rd_addr_value.empty()); 
}

/**Called by init handler at HANDSHAKE level to to check next beat can be received**/
template<class T>
inline void msm_core::msm_aximm_target_rd_socket_handler<T>::send_beat_ack()
{
	data_channel_available=true;
	beat_sampled.write(!beat_sampled.read());
}
/*************************************************
 **************TRANSACTION LEVEL*******************
 ************************************************/
/**Called by Initiator at TRANSACTION level to send new transaction**/
template<class T>
inline void msm_core::msm_aximm_target_rd_socket_handler<T>::send_transaction(std::shared_ptr<T> &trans)
{
	if ( trans != nullptr ) {
		 if ( IMPL_LEVEL == "HANDSHAKE") {
		     //when the initiator is at TRANSACTION level and target is at HANDSHAKE level
			 new_rd_addr_value.push(trans);
		     addr_available.write(!addr_available.read());
		 } else if ( IMPL_LEVEL == "TRANSACTION" ) {
		 	 new_rd_trans_value.push(trans);
		 	 transaction_available.write(!transaction_available.read());
		 }
		 INIT_IMPL_LEVEL = "TRANSACTION";
		 targ_rd_socket->send_trans_busy();
	}
	if ( update_requested++ == 0 ) {
		request_update();
	}
}
/**Called by slave handler at TRANSACTION level to get new transaction**/
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_target_rd_socket_handler<T>::get_transaction()
{
    auto trans = updated_rd_trans_value.front();
	if ( trans != NULL ) {
		updated_rd_trans_value.pop();
		if ( INIT_IMPL_LEVEL == "TRANSACTION" ) {
			targ_rd_socket->send_trans_ack();
		}
		if ( INIT_IMPL_LEVEL == "HANDSHAKE") {
		 targ_rd_socket->send_addr_ack(); //ack for initiator which is on handshake level and target on transaction level
	    }
	}
	return trans;
}
/**Called by slave handler at TRANSACTION level to check new transaction is available or not**/
template<class T>
inline bool msm_core::msm_aximm_target_rd_socket_handler<T>::is_trans_available()
{
    return !updated_rd_trans_value.empty();
}
/**Called by init handler at TRANSACTION level to to check next data can be received**/
template<class T>
inline void msm_core::msm_aximm_target_rd_socket_handler<T>::send_data_ack()
{
	data_channel_available=true;
	data_sampled.write(!data_sampled.read());
}
/**called by init handler at TRANSACTION level when data is in progress**/
template<class T>
inline void msm_core::msm_aximm_target_rd_socket_handler<T>::send_data_busy()
{
	data_channel_available=false;
}
/**Common for both TRANSACTION and HANDSHAKE level**/
template<class T>
inline bool msm_core::msm_aximm_target_rd_socket_handler<T>::is_data_ch_ready()
{
	return data_channel_available;
}
/**Common for both TRANSACTION and HANDSHAKE level**/
template<class T>
inline bool msm_core::msm_aximm_target_rd_socket_handler<T>::is_master_ready()
{
    return data_channel_available; 
}
//RESET the local variables
template<class T>
inline void msm_core::msm_aximm_target_rd_socket_handler<T>::reset()
{
	data_channel_available=true;
}
