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
#include <deque>

#include "msm_interface.h"
#include "msm_fifo_if.h"
#include "msm_module_name.h"
//#include "msm_thread_handle.h"

namespace msm_core
{

template<class T>
class msm_fifo: public virtual msm_prim_channel , public virtual msm_fifo_get_if<T>, public virtual msm_fifo_put_if<T>, public virtual msm_fifo_in_if<T>, public virtual msm_fifo_out_if<T>
{
public:

	 msm_fifo(const msm_module_name &interface_name, const unsigned int max_fifo_depth=1) :
		msm_prim_channel(interface_name), msm_fifo_get_if<T>(interface_name), msm_fifo_put_if<T>(interface_name),
		msm_fifo_in_if<T>(interface_name), msm_fifo_out_if<T>(interface_name),
		msm_interface_base(interface_name), max_fifo_depth(max_fifo_depth)
	 {
	 }
	
	 
	 virtual ~msm_fifo() = default;
	
     /**
	 * @brief API to check if the FIFO is full or not
	 * @return returns true when FIFO becomes full
	 */
	bool is_full() const
	{
		bool ret_val=false;
		if (updated_values_fifo_size + new_values.size() >= max_fifo_depth) {
			ret_val=true;
		}
		return ret_val;
	}

	/**
	 * @brief API to check if the FIFO to read is empty
	 * @return returns if there's no data in the FIFO for read in the current cycle
	 */
	bool empty() const
	{
		return updated_values.empty();
	}
    int size() const {
        return updated_values.size();
    }
    int num_available() const;
	int used();
	bool nb_bound( unsigned int new_size );
	//read/write interface
	std::shared_ptr<T>  read(MSM_THREAD_ARGUMENT);
    std::shared_ptr<T>  nb_read();
	void write( const std::shared_ptr<T> data, MSM_THREAD_ARGUMENT);
    virtual bool nb_write( const std::shared_ptr<T> data );
	
    // msm get interface
   // void get(std::shared_ptr<T> val_ , MSM_THREAD_ARGUMENT);
	std::shared_ptr<T>  nb_get();
	std::shared_ptr<T> get(MSM_THREAD_ARGUMENT );
	bool nb_can_get( ) const;

    //// msm peek interface
	std::shared_ptr<T> peek( MSM_THREAD_ARGUMENT ) const;
	std::shared_ptr<T> nb_peek() const;
	std::shared_ptr<T> nb_peek(int n) const;
    bool nb_can_peek( ) const ; 

    //// msm put interface
	void put( const std::shared_ptr<T> data , MSM_THREAD_ARGUMENT);
    bool nb_put( const std::shared_ptr<T> data );
    bool nb_can_put() const; 
	unsigned int updated_values_fifo_size=0;

private:
	
     /**
	 * @brief Implements update behavior
	 */
	bool update() override;
	unsigned int nb_peek_count{0};
	std::queue<std::shared_ptr<T>> new_values;	//!< New values to be updated
	//std::queue<std::shared_ptr<T>> updated_values;	//!< Updated values in the fifo
	std::deque<std::shared_ptr<T>> updated_values;	//!< Updated values in the fifo
	unsigned int max_fifo_depth;	//!< Maximum configured depth
    std::atomic<int> update_requested = 0; //!< To make sure we request update only once using
	
};

} /* namespace msm_core */

template<class T>
inline bool msm_core::msm_fifo<T>::update()
{
    bool ret_value = false;
    //Move the new values to udpated_values FIFO
    if (!new_values.empty() && (updated_values.size() < max_fifo_depth))
    {
        while(updated_values.size() < max_fifo_depth && (!new_values.empty()))
	    {
            //updated_values.push(new_values.front());
            updated_values.push_back(new_values.front());
            new_values.pop();
	    }
        ret_value = true;
	}
	updated_values_fifo_size = updated_values.size();
    update_requested = 0;
    return ret_value;
}


//template <class T>
//inline void msm_core::msm_fifo<T>::get(std::shared_ptr<T> val_, MSM_THREAD_ARGUMENT)
//{
//  while ( updated_values.empty() ) {
//  	 MSM_WAIT;
//  }
//
//  //in a single clock cycle multiple get cycle should not be allowed
//  //making it similar to HW
//  if ( get_count > 0 ) {
//  	 return;
//  }
// // val_ = updated_values.front();
//  memcpy(val_.get(), updated_values.front().get(), sizeof(T)); 
//  updated_values.pop();
//  get_count++; 
//
//  request_update();
//}
template <class T>
inline std::shared_ptr<T> msm_core::msm_fifo<T>::get(MSM_THREAD_ARGUMENT)
{
  while ( updated_values.empty() ) {
  	 MSM_WAIT;
  }

  //in a single clock cycle multiple get cycle should not be allowed
  //making it similar to HW
  auto  val_ = updated_values.front();
  updated_values.pop_front();

  if ( update_requested++ == 0 ) {
  	request_update();
  }
  return val_;

}
// non-blocking read
template <class T>
inline std::shared_ptr<T> msm_core::msm_fifo<T>::nb_get()
{

  if( updated_values.empty() ) {
    return nullptr;
  }

  //val_ = updated_values.front();
  auto val = updated_values.front();
  updated_values.pop_front();
  if ( update_requested++ == 0 ) {
	  request_update(); //request and update to kernel for updating the data structure
  }
  return val;
}

template <class T>
inline bool msm_core::msm_fifo<T>::nb_can_get( ) const {
  return !updated_values.empty();
}


template <class T>
inline void msm_core::msm_fifo<T>::put( const std::shared_ptr<T> val_, MSM_THREAD_ARGUMENT )
{
   while ( is_full() ) {
	   MSM_WAIT;
   }
   
   	  new_values.push(val_);

  	  if ( update_requested++ == 0 ) {
		  request_update();
	  }
}

template <class T>
inline
bool msm_core::msm_fifo<T>::nb_put( const std::shared_ptr<T> val_ )
{

     if (is_full())
        return false;

    //Push the elements to new_value queue
	new_values.push(val_);

    if ( update_requested++ == 0 ) {
       request_update();
    }
	return true;
}

template <class T >
inline
bool msm_core::msm_fifo<T>::nb_can_put( ) const {

  return !is_full();

}

template <class T >
inline 
std::shared_ptr<T> msm_core::msm_fifo<T>::peek(MSM_THREAD_ARGUMENT) const {

  while( empty() ) {
	 MSM_WAIT;
  }
 return updated_values.front();

}

template < typename T>
inline
std::shared_ptr<T>
msm_core::msm_fifo<T>::nb_peek() const {

  if ( empty() ) {
     return nullptr;
  }
  return updated_values.front();

}
template < typename T>
inline
std::shared_ptr<T>
msm_core::msm_fifo<T>::nb_peek(int n) const {

  if ( empty() || n >= updated_values.size() || n < 0) {
     return nullptr;
  }
  return updated_values[n];

}

template< typename T >
inline
bool
msm_core::msm_fifo<T>::nb_can_peek( ) const
{
	  return !empty();
}

template<class T>
inline std::shared_ptr<T> msm_core::msm_fifo<T>::read(MSM_THREAD_ARGUMENT)
{
	 return get(MSM_THREAD_CASCADE);
}
template<class T>
inline std::shared_ptr<T> msm_core::msm_fifo<T>::nb_read()
{
	 return nb_get();
}
template<class T>
inline bool msm_core::msm_fifo<T>::nb_write(const std::shared_ptr<T> data)
{
	return nb_put(data);	
}

template<class T>
inline void  msm_core::msm_fifo<T>::write(const std::shared_ptr<T> data, MSM_THREAD_ARGUMENT)
{
	put(data, MSM_THREAD_CASCADE);	
}
template<class T>
inline int msm_core::msm_fifo<T>::used()
{
    return updated_values.size(); 
}

template<class T>
inline int msm_core::msm_fifo<T>::num_available() const
{
    return updated_values.size(); 
}
template < typename T>
inline bool msm_core::msm_fifo<T>::nb_bound( unsigned int new_size ) 
{
   
    bool ret = true;
   
    if( static_cast<int>( new_size ) < used() ) {
   
      new_size = used();
      ret = false;
   
    }
   
    max_fifo_depth = new_size;
    return ret;
   
}

