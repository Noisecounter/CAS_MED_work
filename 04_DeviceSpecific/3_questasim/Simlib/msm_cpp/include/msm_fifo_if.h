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

#include "msm_thread_handle.h"

namespace msm_core {

template <typename T> 
class msm_fifo_in_if : public virtual msm_interface_base
{ 
public: 

    // get the number of available samples 
    virtual int num_available() const = 0; 
    msm_fifo_in_if(const msm_module_name &nm) : msm_interface_base(nm) {}
	// blocking read 
    virtual  std::shared_ptr<T> read(MSM_THREAD_ARGUMENT ) = 0; 
	
    // non-blocking read 
    virtual std::shared_ptr<T> nb_read( ) = 0; 

    // disabled 
    //msm_fifo_in_if( const msm_fifo_in_if<T>& ); 
    //msm_fifo_in_if<T>& operator = ( const msm_fifo_in_if<T>& ); 
}; 

template <typename T> 
class msm_fifo_out_if : public virtual msm_interface_base 
{ 
public: 

	 virtual bool nb_write( const std::shared_ptr<T> t ) = 0; 
	 virtual void write( const std::shared_ptr<T> t , MSM_THREAD_ARGUMENT) = 0;

    virtual int num_available() const = 0;

    msm_fifo_out_if(const msm_module_name &nm) : msm_interface_base(nm) {}

    // disabled 
    //msm_fifo_out_if( const msm_fifo_out_if<T>& ); 
    //msm_fifo_out_if<T>& operator = ( const msm_fifo_out_if<T>& ); 
}; 

template < typename T >
class msm_get_if {
public:
  
  virtual std::shared_ptr<T> get(MSM_THREAD_ARGUMENT)=0;
  //virtual void get(std::shared_ptr<T>  t, MSM_THREAD_ARGUMENT )=0;
  virtual std::shared_ptr<T>  nb_get( ) = 0;
  virtual bool nb_can_get( ) const = 0;
};

template < typename T >
class msm_put_if {
  public:
  virtual void put( const std::shared_ptr<T> data , MSM_THREAD_ARGUMENT)=0;
  virtual bool nb_put( const std::shared_ptr<T> t ) = 0;
  virtual bool nb_can_put() const = 0;
};

template < typename T >
class msm_peek_if {

  public:
  virtual std::shared_ptr<T> peek( MSM_THREAD_ARGUMENT) const = 0;
  virtual std::shared_ptr<T>  nb_peek( ) const = 0;
  virtual bool nb_can_peek( ) const = 0;
};

// get_peek interfaces
template < typename T >
class msm_get_peek_if :
  public virtual msm_get_if<T> ,
  public virtual msm_peek_if<T> {
};


template < typename T >
class msm_fifo_put_if :
  public virtual msm_put_if<T>,
  public virtual msm_interface_base {
    public:
  msm_fifo_put_if(const msm_module_name &nm) : msm_interface_base(nm) {}

  };

template < typename T >
class msm_fifo_get_if :
  public virtual msm_get_peek_if<T>,
  public virtual msm_interface_base  {

  public:
  msm_fifo_get_if(const msm_module_name &nm) : msm_interface_base(nm) {}

  };

  // combined

template < typename REQ , typename RSP >
class msm_master_if :
  public virtual msm_put_if< REQ > ,
  public virtual msm_get_peek_if< RSP > ,
  public virtual msm_interface_base {
  
  public:
  msm_master_if(const msm_module_name &nm) : msm_interface_base(nm) {}

  };

template < typename REQ , typename RSP >
class msm_slave_if :
  public virtual msm_put_if< RSP > ,
  public virtual msm_get_peek_if< REQ > ,
  public virtual msm_interface_base {
  

  public:
  msm_slave_if(const msm_module_name &nm) : msm_interface_base(nm) {}

  };

} // namespace msm_core

