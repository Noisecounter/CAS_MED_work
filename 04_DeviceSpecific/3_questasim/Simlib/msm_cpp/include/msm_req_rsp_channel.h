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

#include "msm.h"
#include "msm_fifo.h"
#include "msm_put_get_imp.h"

namespace msm_core {

template < typename REQ , typename RSP ,
     typename REQ_CHANNEL = msm_fifo<REQ> ,
     typename RSP_CHANNEL = msm_fifo<RSP> >

class msm_req_rsp_channel : public msm_module
{
public:
  // uni-directional slave interface

  msm_export< msm_fifo_get_if< REQ > > get_request_export;
  msm_export< msm_fifo_put_if< RSP > > put_response_export;

  // uni-directional master interface

  msm_export< msm_fifo_put_if< REQ > > put_request_export;
  msm_export< msm_fifo_get_if< RSP > > get_response_export;

  // master / slave interfaces

  msm_export< msm_master_if< REQ , RSP > > master_export;
  msm_export< msm_slave_if< REQ , RSP > > slave_export;

  msm_req_rsp_channel( const msm_module_name module_name ,
           int req_size = 1 , int rsp_size = 1 ) :
    msm_module( module_name  ) ,
    get_request_export("get_request_export"),
	put_response_export("get_request_export"),
	put_request_export("get_request_export"),
	get_response_export("get_response_export"),
	master_export("master_export"),
	slave_export("slave_export"),
	request_fifo("request_fifo", req_size ) ,
    response_fifo("response_fifo", rsp_size ), 
    master( request_fifo , response_fifo ) ,
    slave( request_fifo , response_fifo )
  {

    bind_exports();

  }
 ~msm_req_rsp_channel() = default;
private:
  void bind_exports() {

    put_request_export( request_fifo );
    get_request_export( request_fifo );

    put_response_export( response_fifo );
    get_response_export( response_fifo );

    master_export( master );
    slave_export( slave );

  }

//protected:
public:
  REQ_CHANNEL request_fifo;
  RSP_CHANNEL response_fifo;

  msm_master_imp< REQ , RSP > master;
  msm_slave_imp< REQ , RSP > slave;
};

} // namespace msm_core

