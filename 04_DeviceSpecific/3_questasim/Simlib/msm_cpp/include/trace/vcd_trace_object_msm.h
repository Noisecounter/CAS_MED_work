// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
// (c) Copyright(C) 2013 - 2023 by Xilinx, Inc. All rights reserved.
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

#include <algorithm>
#include <bitset>
#include <string>
#include <vector>

#include "msm_trace_obj_base.h"

namespace msm_trace
{
class vcd_trace_object_base_msm: public msm_trace_obj_base
{
public:
    vcd_trace_object_base_msm();
    virtual ~vcd_trace_object_base_msm();
    /**
     * @brief A pure virtual API to check if data got updated.
     * @return returns if the object got updated or not
     */
    virtual bool is_updated() const = 0;

    /**
     * @brief A pure virtual API to get trace dump
     * @return returns the string containing trace dump
     */
    virtual std::string get_dump() = 0;

};

/**
 * @brief API to get the trace objects pointers of msm_cpp
 * @return returns vector containing trace vcd object pointers of MSM
 */
const std::vector<vcd_trace_object_base_msm*>& get_vcd_msm_trace_objects();

/**
 * @brief Generic template class for various data types to be traced (MSM)
 * @tparam T is type of the data to be traced
 * @tparam N is the width in bits of the data type to be traced. This is auto evaluated based on the
 * type of data by the compiler. But provision is given for overriding by the user.
 */

template<class T, unsigned int N = __msm_trace_obj_size_in_bits<T>::value>
class vcd_trace_object_msm: public vcd_trace_object_base_msm
{
public:
    vcd_trace_object_msm(T &trace_inst, std::string identifier) :
            updated_value_ref(trace_inst), m_var_identifier(identifier)
    {
    }
    virtual ~vcd_trace_object_msm() = default;

    bool is_updated() const override
    {
        //This API for msm objects isn't expected to be used as the kernel will call
        //get dump only if the channel got updated.
        //Hence returning true alway    s
        return true;
    }

    /**
     * @brief A pure virtual function to generate trace dump.
     * Generation of trace and getting the dump has been separated so as to
     * accomodate delta cycle trace generation. Trace may be generated multiple times
     * in exec cycle and / or delta cycle
     */
    void generate_dump() override
    {
        //TODO : Memory Optimization. The memory is allocated every time. Instead reuse memory
        if (updated_value_ref != 0)
        {
            m_vcd_dump = std::bitset<N>(updated_value_ref).to_string() + " " + m_var_identifier;
            //TODO : Need leading zero optimization
            //Remove the leading zeros for optimization purpose
            m_vcd_dump.erase(0, std::min(m_vcd_dump.find_first_not_of('0'), m_vcd_dump.size() - 1));
        }
        else
            m_vcd_dump = "0 " + m_var_identifier;

        //Set the trace generation sate to generated.
        set_trace_generation_at_cur_time();
    }

    /**
     * @brief A pure virtual API to get trace dump. (Implements base class interface)
     * @return returns the string containing trace dump
     */
    std::string get_dump() override
    {
        //Reset the trace generation state.
        reset_trace_generation_at_cur_time();
        return m_vcd_dump;
    }

private:
    T &updated_value_ref;
    std::string m_vcd_dump; //!< VCD Dump string
    const std::string m_var_identifier; //!< trace identifier in VCD format as per VCD spec
};
}
