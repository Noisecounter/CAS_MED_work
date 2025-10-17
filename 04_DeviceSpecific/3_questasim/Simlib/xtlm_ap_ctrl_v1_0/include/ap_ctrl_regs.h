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
#include <new>

namespace xsc
{
    struct ap_ctrl_regs
    {
        enum REG_ADDR {
            CTRL_ADDR = 0x0,
            GIE_ADDR  = 0x4,
            IE_ADDR   = 0x8,
            IS_ADDR   = 0xc,
            AR_ADDR   = 0x10,
            MIC_ADDR  = 0x14,
            MOC_ADDR  = 0x18
        };

        union CTRL_REG
        {
            unsigned int value;
            struct
            {
                unsigned int ap_start         :1; //! Asserted by host, cleared by kernel
                unsigned int ap_done          :1; //! Asserted by kernel when it has finished
                unsigned int ap_idle          :1; //! Asserted when it is idle
                unsigned int ap_ready         :1; //! Asserted when it is done with input data
                unsigned int ap_continue      :1; //! Asserted by host
                unsigned int reserved0        :2; //! Bits 5 & 6 reserved
                unsigned int auto_restart     :1; //! Autorestart status
                unsigned int reserved1        :1; //! Bit 8 reserved
                // FUTURE: Below bits are now deprecated
                unsigned int mbox_input_ctrl  :1; //! Asserted by host, cleared by saxilite
                unsigned int mbox_output_ctrl :1; //! Asserted by host, cleared by saxilite
            } BITS;
        };

        union GIE_REG
        {
            unsigned int value;
            struct
            {
                unsigned int GIE       :1; //! Global Interrupt Enable Bit
                unsigned int reserved0 :31; //! Remaining Bits reserved
            } BITS;
        };

        union IE_REG
        {
            unsigned int value;
            struct
            {
                unsigned int ap_done          :1; //! ap_done Interrupt Enable Bit
                unsigned int ap_ready         :1; //! ap_ready Interrupt Enable Bit
                unsigned int ap_idle          :1; //! ap_idle Interrupt Enable Bit
                unsigned int mbox_input_ctrl  :1; //! mbox_input_ctrl Interrupt Enable Bit
                unsigned int mbox_output_ctrl :1; //! mbox_output_ctrl Interrupt Enable Bit
                unsigned int reserved0        :27; //! Remaining Bits reserved
            } BITS;
        };

        union IS_REG
        {
            unsigned int value;
            struct
            {
                unsigned int ap_done          :1; //! ap_done Interrupt Status Bit
                unsigned int ap_ready         :1; //! ap_ready Interrupt Status Bit
                unsigned int ap_idle          :1; //! ap_idle Interrupt Status Bit
                unsigned int mbox_input_ctrl  :1; //! mbox_input_ctrl Interrupt Status Bit
                unsigned int mbox_output_ctrl :1; //! mbox_output_ctrl Interrupt Status Bit
                unsigned int reserved0        :27; //! Remaining Bits reserved
            } BITS;
        };

        union AR_REG
        {
            unsigned int value;
            struct
            {
                unsigned int ap_start_counter :32; //! auto_restart counter
            } BITS;
        };

        union MIC_REG
        {
            unsigned int value;
            struct
            {
                unsigned int write     :1; //! Asserted by host
                unsigned int ack       :1; //! Asserted on request received, cleared on request serviced
                unsigned int reserved0 :30; //! Remaining Bits reserved
            } BITS;
        };

        union MOC_REG
        {
            unsigned int value;
            struct
            {
                unsigned int read      :1; //! Asserted by host
                unsigned int ack       :1; //! Asserted on request received, cleared on request serviced
                unsigned int reserved0 :30; //! Remaining Bits reserved
            } BITS;
        };

        ap_ctrl_regs(unsigned char* const reg_mem)
            : CTRL(*(new (reg_mem + CTRL_ADDR) CTRL_REG()))
            , GIE(*(new (reg_mem + GIE_ADDR) GIE_REG()))
            , IE(*(new (reg_mem + IE_ADDR) IE_REG()))
            , IS(*(new (reg_mem + IS_ADDR) IS_REG()))
            , AR(*(new (reg_mem + AR_ADDR) AR_REG()))
            , MIC(*(new (reg_mem + MIC_ADDR) MIC_REG()))
            , MOC(*(new (reg_mem + MOC_ADDR) MOC_REG()))
        {
        }

        //! Declare Kernel Regs...
        CTRL_REG& CTRL;
        GIE_REG&  GIE;
        IE_REG&   IE;
        IS_REG&   IS;
        AR_REG&   AR;
        MIC_REG&  MIC;
        MOC_REG&  MOC;
    };
}
