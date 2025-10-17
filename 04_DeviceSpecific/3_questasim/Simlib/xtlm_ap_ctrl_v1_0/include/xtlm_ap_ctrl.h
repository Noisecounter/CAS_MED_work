// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
/*  (c) Copyright 1995 - 2021 Xilinx, Inc. All rights reserved.

 This file contains confidential and proprietary information
 of Xilinx, Inc. and is protected under U.S. and
 international copyright and other intellectual property
 laws.

 DISCLAIMER
 This disclaimer is not a license and does not grant any
 rights to the materials distributed herewith. Except as
 otherwise provided in a valid license issued to you by
 Xilinx, and to the maximum extent permitted by applicable
 law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
 WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
 AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
 BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
 INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
 (2) Xilinx shall not be liable (whether in contract or tort,
 including negligence, or under any other theory of
 liability) for any loss or damage of any kind or nature
 related to, arising under or in connection with these
 materials, including for any direct, or any indirect,
 special, incidental, or consequential loss or damage
 (including loss of data, profits, goodwill, or any type of
 loss or damage suffered as a result of any action brought
 by a third party) even if such damage or loss was
 reasonably foreseeable or Xilinx had been advised of the
 possibility of the same.

 CRITICAL APPLICATIONS
 Xilinx products are not designed or intended to be fail-
 safe, or for use in any application requiring fail-safe
 performance, such as life-support or safety devices or
 systems, Class III medical devices, nuclear facilities,
 applications related to the deployment of airbags, or any
 other applications that could lead to death, personal
 injury, or severe property or environmental damage
 (individually and collectively, "Critical
 Applications"). Customer assumes the sole risk and
 liability of any use of Xilinx products in Critical
 Applications, subject only to applicable laws and
 regulations governing limitations on product liability.

 THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
 PART OF THIS FILE AT ALL TIMES.      */

#pragma once

#include <sstream>
#include <iomanip>
#include "xtlm.h"
#include "report_handler.h"

#define ADD_MEMORY_IF_ARG(if_name, offset, size) add_karg(if_name,  offset, size)
#define ADD_STREAM_IF_ARG(if_name, offset, size) add_karg(if_name,  offset, size)
#define ADD_SCALAR_ARG(arg_name,   offset, size) add_karg(arg_name, offset, size)

#include "ap_ctrl_regs.h"

namespace xsc
{

class xtlm_ap_ctrl : public sc_core::sc_module, public ap_ctrl_regs
{
    enum {
        REG_MEM_SIZE = 256
    };

    public:

        SC_HAS_PROCESS(xtlm_ap_ctrl);

        xtlm_ap_ctrl(sc_module_name name);
        ~xtlm_ap_ctrl();

        sc_in<bool> ap_rst_n;
        sc_in<bool> ap_clk;

        //! s_axi_control interface for kernel register access...
        xtlm::xtlm_aximm_target_socket*  s_axi_control_rd_socket;
        xtlm::xtlm_aximm_target_socket*  s_axi_control_wr_socket;

    protected:

        //! Bit value chages which triggers actual kernel (derived class) activity
        virtual void ap_start();
        virtual void ap_continue();

        //! The below bits are updated by the kernel based on the status of activity
        void ap_done();
        void ap_idle();
        void ap_ready();

        //! Autorestart-status and mbox sync for next kernel iteration
        bool ap_restart();
        bool ap_restart_status();

        //! Auto-restart support
        void set_auto_restart_counter();
        void set_auto_restart_counter(uint32_t count);

        //! Mailbox support
        virtual void sync_hw_mbox();
        virtual void sync_sw_mbox();

        //! Add Kernel arguments
        void add_karg(xtlm::xtlm_aximm_master_if* if_name, unsigned int offset, unsigned int size) {}
        void add_karg(xtlm::xtlm_axis_master_if*  if_name, unsigned int offset, unsigned int size) {}
        void add_karg(xtlm::xtlm_axis_slave_if*   if_name, unsigned int offset, unsigned int size) {}
        void add_karg(unsigned int& arg_name, unsigned int offset, unsigned int size) {}

        unsigned char reg_mem[REG_MEM_SIZE];
        uint64_t*     kernel_args;
        uint64_t      auto_restart_counter;
        sc_event      ev_ap_start;
        sc_event      ev_ap_continue;
        sc_event      ev_ap_restart;
        sc_event      ev_ap_mbox;

        bool mbox_in_enabled = false;
        bool mbox_out_enabled = false;
        bool counted_ar_enabled = false;

        xtlm::xtlm_aximm_target_wr_socket_util*  s_axi_control_wr_util;
        xtlm::xtlm_aximm_target_rd_socket_util*  s_axi_control_rd_util;

        char* m_name;
        xsc::common_cpp::report_handler* m_log;
        std::stringstream m_ss;

    private:
        
        void kernel_config_write();
        void kernel_status_read();
        void kernel_mbox_sync();
        void write_CTRL(unsigned int data);
        void write_GIE(unsigned int data);
        void write_IE(unsigned int data);
        void write_IS(unsigned int data);
        void write_AR(unsigned int data);
        void write_MIC(unsigned int data);
        void write_MOC(unsigned int data);

        //! Interrupt generation and clearing...
        void generate_interrupt();
        void clear_interrupt();
};

class xtlm_ap_ctrl_none : public sc_core::sc_module
{
    public:
        SC_HAS_PROCESS(xtlm_ap_ctrl_none);

        xtlm_ap_ctrl_none(sc_module_name name);
        ~xtlm_ap_ctrl_none();

        //! Add Kernel arguments
        void add_karg(xtlm::xtlm_axis_master_if*  if_name, unsigned int offset, unsigned int size) {}
        void add_karg(xtlm::xtlm_axis_slave_if*   if_name, unsigned int offset, unsigned int size) {}

        sc_in<bool>   ap_rst_n;
        sc_in<bool>   ap_clk;

    protected:
        bool ap_restart_status();

        char* m_name;
        xsc::common_cpp::report_handler* m_log;
        std::stringstream m_ss;

};

typedef xtlm_ap_ctrl xtlm_ap_ctrl_hs;
typedef xtlm_ap_ctrl xtlm_ap_ctrl_chain;

} //! name_space

