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
class msm_aximm_initiator_rd_socket_handler: public msm_prim_channel , public msm_init_aximm_read_if<T>
{
public:

	 msm_aximm_initiator_rd_socket_handler(const msm_module_name interface_name, std::string impl_level="TRANSACTION", const unsigned int max_fifo_depth=1) :
		msm_prim_channel(interface_name), msm_init_aximm_read_if<T>(interface_name), msm_interface_base(interface_name),
		beat_available("rd_beat_available"), addr_sampled("addr_sampled"), init_rd_socket("init_rd_socket"), trans_sampled("trans_sampled"), data_available("data_available"),
		IMPL_LEVEL(impl_level), TARGET_IMPL_LEVEL("")
		
	 {
			 init_rd_socket.bind(*this);
	 }
	 
	 virtual ~msm_aximm_initiator_rd_socket_handler() = default;
     
	 /*Pure Virtual Function of Interface*/
	 virtual void  send_beat_data(std::shared_ptr<T> &trans);
	 virtual void  send_data(std::shared_ptr<T> &trans);
	 virtual void  send_addr_ack();
 	 virtual void  send_addr_busy();
     virtual void  send_trans_ack();
 	 virtual void  send_trans_busy();

	 /*Helper functions for HANDSHAKE level*/
	 std::shared_ptr<T> get_beat_data( );
	 std::shared_ptr<T> get_data( );
	 std::shared_ptr<T> peek_beat_data();
	 bool is_last_beat();
	 bool is_data_available();
	 bool is_addr_ch_ready();
	 bool is_slave_ready();
	 void reset();
	 std::string IMPL_LEVEL, TARGET_IMPL_LEVEL;
	 msm_core::msm_signal<bool> beat_available, addr_sampled, trans_sampled, data_available;
	 msm_core::msm_aximm_initiator_socket<msm_targ_aximm_read_if<T>,msm_init_aximm_read_if<T> > init_rd_socket;
     /**
	 * @brief Implements update behavior
	 */
	 private:
	 bool update() override;
	 std::queue<std::shared_ptr<T>> new_rd_addr_value;	//!< 1 address channel. 1 request can be served at a time.
	 std::queue<std::shared_ptr<T>> updated_rd_addr_value;	//!< Updated values in the fifo
	 std::queue<std::shared_ptr<T>> new_rd_data_value;	//!< New values to be updated
	 std::queue<std::shared_ptr<T>> updated_rd_data_value;	//!< Updated values in the fifo
     std::atomic<int> update_requested = 0; //!< To make sure we request update only once using
     std::map<std::shared_ptr<T>, int> num_reads;	
	 std::shared_ptr<T> trans_in_progress = nullptr;
	 unsigned int max_fifo_depth=1;
	 bool addr_channel_available=true, transaction_channel_available=true;
};

} /* namespace msm_core */
template<class T>
inline bool msm_core::msm_aximm_initiator_rd_socket_handler<T>::update()
{
    bool ret_value = false;
	if (!new_rd_data_value.empty() && (updated_rd_data_value.size() < max_fifo_depth))
    {
        while(updated_rd_data_value.size() < max_fifo_depth && (!new_rd_data_value.empty()))
	    {
            updated_rd_data_value.push(new_rd_data_value.front());
            new_rd_data_value.pop();
	    }
        ret_value = true;
	}
    update_requested = 0;
    return ret_value;
}
/******************************************************
 * *******************HANDSHAKE level***************
 ******************************************************/

/**send beat data at handhshake level. Called by the slave handler to send new rd beat data*/
template<class T>
inline void msm_core::msm_aximm_initiator_rd_socket_handler<T>::send_beat_data(std::shared_ptr<T> &trans)
{
	  if ( trans_in_progress == nullptr ) {
	  	trans_in_progress = trans;
		num_reads[trans] = trans->get_burst_length();
	  }
	   num_reads[trans] -= 1;
	  //HANDSHAKE to HANDSHAKE level
	  //Transaction to HANDSHAKE level
	  if ( IMPL_LEVEL == "HANDSHAKE" ) {
	  	new_rd_data_value.push(trans);
	  	beat_available.write(!beat_available.read());
	  	init_rd_socket->send_data_busy();
	  }
	  //when init is at TRANSACTION LEVEL and TARGT is at HANDSHAKE level
	  if ( IMPL_LEVEL == "TRANSACTION" ) {
	  	    if ( num_reads[trans_in_progress] == 0 ) {
	  			new_rd_data_value.push(trans);
				data_available.write(!data_available.read()); //when last data beat happens, notify initiator
				init_rd_socket->send_data_busy();
			} else  {
	  			init_rd_socket->send_beat_ack(); //Keep sending acknowledgement, when last comes then notify init that data has arrived
			}
	  }
	  TARGET_IMPL_LEVEL = "HANDSHAKE";
	  if ( update_requested++ == 0 ) {
		  request_update();
	  }
}
/**get new beat data at HANDSHAKE level. Called by the init handler**/
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_initiator_rd_socket_handler<T>::get_beat_data( )
{
	
	auto trans = updated_rd_data_value.front();
 	if ( IMPL_LEVEL == "HANDSHAKE" && TARGET_IMPL_LEVEL == "TRANSACTION" ) {
			if ( num_reads[trans_in_progress] == 0 ) {
	  			new_rd_data_value.push(trans);
				beat_available.write(!beat_available.read()); //
				init_rd_socket->send_data_ack();
			} else {
	  			new_rd_data_value.push(trans);
				beat_available.write(!beat_available.read()); //
			}
			num_reads[trans] -= 1;
	}
	if ( trans != NULL ) {
		updated_rd_data_value.pop();
		if ( IMPL_LEVEL == "HANDSHAKE" ) {
			init_rd_socket->send_beat_ack();
		}
		if ( IMPL_LEVEL == "TRANSACTION" ) {
			init_rd_socket->send_data_ack();
		}
	}

	return trans;  	  
}
/**Check last beat at HANDSHAKE level. Called by the init handler**/
template<class T>
inline bool msm_core::msm_aximm_initiator_rd_socket_handler<T>::is_last_beat()
{
	bool status = true;
	if ( num_reads[trans_in_progress] == 0 ) {
		 num_reads.erase(trans_in_progress);
		 trans_in_progress = nullptr;
		 status = true;
	} else {
		 status = false;
	}
	 return status;
}
/**Return current beat data. Called by the Init handler**/
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_initiator_rd_socket_handler<T>::peek_beat_data()
{
	return updated_rd_data_value.front();
}
/**addr phase Completed. Addr ack received by slave**/
template<class T>
inline void msm_core::msm_aximm_initiator_rd_socket_handler<T>::send_addr_ack()
{
	addr_channel_available=true;
	addr_sampled.write(!addr_sampled.read());
}
/**When addr phase is in progress. Called by slave handler**/
template<class T>
inline void msm_core::msm_aximm_initiator_rd_socket_handler<T>::send_addr_busy()
{
	addr_channel_available=false;
}
/**addr phase Completed. Addr ack received by slave**/
template<class T>
inline bool msm_core::msm_aximm_initiator_rd_socket_handler<T>::is_addr_ch_ready()
{
	return addr_channel_available; 
}

/******************************************************
 ********************TRANSACTION level***************
 ******************************************************/

/**Called by slave handler at TRANSACTION level. To send Rd data to init handler***/
template<class T>
inline void msm_core::msm_aximm_initiator_rd_socket_handler<T>::send_data(std::shared_ptr<T> &trans)
{
	  if ( IMPL_LEVEL == "HANDSHAKE" ) {
	  	if ( trans_in_progress == nullptr ) {
	  		trans_in_progress = trans;
			num_reads[trans] = trans->get_burst_length();
	  	 }
	  	 num_reads[trans] -= 1;
	  	 beat_available.write(!beat_available.read());
	  }
	  new_rd_data_value.push(trans);
	  if ( IMPL_LEVEL == "TRANSACTION" ) {
	  	    data_available.write(!data_available.read());
	  } 
	  init_rd_socket->send_data_busy();
	  TARGET_IMPL_LEVEL = "TRANSACTION";
	  if ( update_requested++ == 0 ) {
		  request_update();
	  }
}

/**Called by slave handler at TRANSACTION level. To send new Rd data to init handler***/
template<class T>
inline std::shared_ptr<T> msm_core::msm_aximm_initiator_rd_socket_handler<T>::get_data( )
{
	auto trans = updated_rd_data_value.front();
	if ( trans != NULL ) {
		updated_rd_data_value.pop();
		if ( trans_in_progress == nullptr ) {
			init_rd_socket->send_data_ack(); //send this ack only when both init and target are at transaction level
		}
		if ( TARGET_IMPL_LEVEL == "HANDSHAKE" ) {
			init_rd_socket->send_beat_ack(); //If initiator is TRANSACTION LEVEL and Target is at HANDSHAKE level
			is_last_beat(); //trans in progress should be null because initiataor have populated trans_in_progress
		}
	}
	return trans;  	  
}
/**Called by init handler at TRANSACTION level to check rd data is available or not**/
template<class T>
inline bool msm_core::msm_aximm_initiator_rd_socket_handler<T>::is_data_available()
{
	return (!updated_rd_data_value.empty());
}
/**called by init handler at TRANSACTION level to check slave is ready**/
template<class T>
inline bool msm_core::msm_aximm_initiator_rd_socket_handler<T>::is_slave_ready()
{
	return transaction_channel_available;
}
/**When transaction phase is in progress. Called by slave handler at TRANSACTION level.**/
template<class T>
inline void msm_core::msm_aximm_initiator_rd_socket_handler<T>::send_trans_busy()
{
	transaction_channel_available=false;
}
/**When transaction phase is completed. Called by slave handler at TRANSACTION level.**/
template<class T>
inline void msm_core::msm_aximm_initiator_rd_socket_handler<T>::send_trans_ack()
{
	transaction_channel_available=true;
	trans_sampled.write(!trans_sampled.read());
}
template<class T>
inline void msm_core::msm_aximm_initiator_rd_socket_handler<T>::reset()
{
	addr_channel_available=true;
	transaction_channel_available=true;
	num_reads.clear();
}
