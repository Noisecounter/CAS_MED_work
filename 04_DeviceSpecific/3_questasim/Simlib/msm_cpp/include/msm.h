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
#include "msm_module_name.h"
#include "msm_object.h"
#include "msm_interface.h"
#include "msm_port.h"
#include "msm_module.h"
#include "msm_sockets.h"
#include "static_scheduler.h"
#include "msm_fifo.h"
#include "msm_signal.h"
#include "msm_req_rsp_channel.h"
#include "msm_fifo_ports.h"
#include "msm_transport_dbg_handle.h"
#include "msm2sc_sync.h"
#include "msm_clock.h"
#include "msm_global_apis.h"
#include "msm_mutex.h"
#include "msm_spawn.h"
#include "msm_targ_rd_aximm_handler.h"
#include "msm_init_rd_aximm_handler.h"
#include "msm_init_wr_aximm_handler.h"
#include "msm_targ_wr_aximm_handler.h"
#include "msm_initiator_target_sockets.h"
#include "msm_aximm_target_rd_stub.h"
#include "msm_aximm_target_wr_stub.h"
#include "msm_aximm_initiator_wr_stub.h"
#include "msm_aximm_initiator_rd_stub.h"
//Utils
#include "msm_vector.h"
//#include "vcd_trace_file_writer.h"

#include "msm_thread_handle.h"
#include "msm_global_time.h"
#include "msm_method_handle.h"

//TRACE
#include "trace/vcd_trace_object.h"
#include "trace/vcd_trace_file_writer.h"



