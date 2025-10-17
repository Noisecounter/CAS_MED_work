// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
// (c) Copyright(C) 2013 - 2021 by Xilinx, Inc. All rights reserved.
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

#include "xtlm.h"
#include <systemc>
#include <vector>
#include <queue>
#include <set>
#include "axis2ipc_socket.h"
#include "ipc2axis_socket.h"

namespace xtlm
{

class sim_ipc_axis_multi_intf_impl: public sc_core::sc_module
{

public:
	SC_HAS_PROCESS(sim_ipc_axis_multi_intf_impl);
	sim_ipc_axis_multi_intf_impl(sc_core::sc_module_name name,
			xsc::common_cpp::properties &ppts);

	sc_core::sc_vector<xtlm::xtlm_axis_initiator_socket> m_initiator_sockets;
	sc_core::sc_vector<xtlm::xtlm_axis_target_socket> m_target_sockets;

	sc_core::sc_in<bool> clk;
	sc_core::sc_in<bool> aresetn;

	//!< Function to check if external proc is connected or not
	bool is_external_proc_connected() const;
private:

	/*!
	 * @brief Function to get IPI instance name in different simulators
	 * @param s is hierarchical name
	 * @return returns in instance name
	 */
	std::string get_ipi_name(std::string s);

	/*!
	 * @brief Method to store the transaction from IPC to buffer
	 */
	void ipc_array_to_payload_buffer();

	/*!
	 * @brief Method to Send the transactions from buffer to IPC
	 */
	void payload_array_to_ipc();

	/*!
	 * @brief Method to send data from AXIS to IPC
	 */
	void axi_stream2ipc_send();

	/*!
	 * @brief Method to initiate transactions from the buffer
	 */
	void init_txns_from_buffer();

	/*!
	 * @brief Function to check if the payload is control packet
	 * @param payload
	 * @return True/False
	 */
	bool is_timing_control_packet(xtlm::axis_payload *payload);

	/*!
	 * @brief Method to handle barrier transaction and push them to Queue
	 * @param payload
	 * @param intf_index is the index of the interface
	 */
	void process_barrier_to_queue(xtlm::axis_payload *payload,
			unsigned int intf_index);
	/*!
	 * @brief This method handles barriers transactions which are in the Queue
	 * and sends them on TLM interfaces when barrier condition is met
	 * @param payload
	 * @return
	 */
	bool handle_barrier(xtlm::axis_payload *payload);

	/*!
	 * @brief Function to initiate give transaction , delay pair on the interface
	 * if the previous transfer on is completed (on given interface)
	 * @param txn_pair is the pair of payload and delay
	 * @param intf_index is the index on which transaction should be initiated
	 * @return True - if transaction is initiated, False - otherwise
	 */
	bool send_txn_pair_on_intf(
			std::pair<xtlm::axis_payload*, unsigned int> txn_pair,
			unsigned int intf_index);

	/**!
	 * @brief Method to calculate clock period
	 */
	void calculate_clock_period();

	std::vector<unsigned int> m_init_if_data_width; //!<Initiator IF data widths
	std::vector<unsigned int> m_targ_if_data_width; //!<Target IF data widths
	std::vector<std::vector<std::pair<xtlm::axis_payload*, unsigned int> > > m_axis2ipc_frames;
	sc_core::sc_vector<xtlm::xtlm_axis_initiator_socket_util>
		m_initiator_socket_utils; //!<Initiator Socket Util Vector
	sc_core::sc_vector<xtlm::xtlm_axis_target_socket_util>
		m_target_socket_utils; //!<Target Socket Util Vector
	std::vector<std::queue<std::pair<xtlm::axis_payload*, unsigned int> > >
		m_buffered_payloads; //!< Buffer to store payloads received from the IPC
	xsc::ipc2axis_frame_socket* m_ipc2axis; //!< IPC2AXIS Frame Socket
	xsc::axis2ipc_frame_socket* m_axis2ipc; //!< AXIS2IPC Frame Socket
	sc_core::sc_event ev_payloads_pushed_to_buffer; //!< Event to notify
	xsc::common_cpp::report_handler m_logger; //!< Logger module
	std::map<unsigned int, std::set<unsigned int> > m_barrier_intf_set; //!< Map to store barriered interface associated to key
	sc_core::sc_time m_clock_period;
	unsigned int m_init_reset_cycles;
	sc_core::sc_event m_out_of_reset;
	std::string m_instance_name; //!< Instance name of the IP
};
}
