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

#include "msm_fifo_if.h"

namespace msm_core {

template < typename PUT_DATA , typename GET_DATA>
class msm_put_get_imp :
  private virtual msm_put_if< PUT_DATA > ,
  private virtual msm_get_peek_if< GET_DATA >
{
public:
  msm_put_get_imp( msm_put_if<PUT_DATA> &p ,
       msm_get_peek_if<GET_DATA> &g ) :
    put_fifo( p ) , get_fifo( g ) {}

  // put interface
  void put( const std::shared_ptr<PUT_DATA> t , MSM_THREAD_ARGUMENT) { put_fifo.put( t , MSM_THREAD_CASCADE); }
  bool nb_put( const std::shared_ptr<PUT_DATA> t ) { return put_fifo.nb_put( t ); }
  bool nb_can_put( ) const {
    return put_fifo.nb_can_put();
  }

  // get interface
  std::shared_ptr<GET_DATA> get(MSM_THREAD_ARGUMENT) { return get_fifo.get(MSM_THREAD_CASCADE); }
  //void get(std::shared_ptr<GET_DATA> t , MSM_THREAD_ARGUMENT) { return get_fifo.get(t, MSM_THREAD_CASCADE); }
  std::shared_ptr<GET_DATA>  nb_get( ) { return get_fifo.nb_get(); }
  bool nb_can_get( ) const {
    return get_fifo.nb_can_get();
  }

  // peek interface
  std::shared_ptr<GET_DATA> peek(MSM_THREAD_ARGUMENT) const { return get_fifo.peek(MSM_THREAD_CASCADE); } 
  std::shared_ptr<GET_DATA>  nb_peek( ) const { return get_fifo.nb_peek(); }
  bool nb_can_peek() const {
    return get_fifo.nb_can_peek();
  }

    /**
     * @brief Implements update behavior
     */
private:
  msm_put_if<PUT_DATA> &put_fifo;
  msm_get_peek_if<GET_DATA> &get_fifo;
};

template < typename REQ , typename RSP >
class msm_master_imp :
  private msm_put_get_imp< REQ , RSP > ,
  public virtual msm_master_if< REQ , RSP >
{
public:

  msm_master_imp( msm_put_if<REQ> &req , msm_get_peek_if<RSP> &rsp ) : 
	msm_master_if<REQ, RSP>("msm_master_if"),
    msm_interface_base("msm_interface_base"),
 	msm_put_get_imp<REQ,RSP>( req , rsp ) {}

};

template < typename REQ , typename RSP >
class msm_slave_imp :
  private msm_put_get_imp< RSP , REQ > ,
  public virtual msm_slave_if< REQ , RSP >
{
public:

  msm_slave_imp( msm_get_peek_if<REQ> &req , msm_put_if<RSP> &rsp ) :  
	msm_slave_if< REQ , RSP >("msm_slave_if"),
	msm_interface_base("msm_interface_base"),
    msm_put_get_imp<RSP,REQ>( rsp  , req ) {}

};

} // namespace msm_core 

