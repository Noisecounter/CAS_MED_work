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

#include <bitset>
#include <string>

namespace msm_trace
{

/**
 * @class Base class for all types of traces in MSM infra
 */
class msm_trace_obj_base
{
public:
    msm_trace_obj_base()
    {
        m_atleast_a_trace_generated = true;
    }
    virtual ~msm_trace_obj_base() = default;

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

    /**
     * @brief A pure virtual function to generate trace dump.
     * Generation of trace and getting the dump has been separated so as to
     * accomodate delta cycle trace generation. Trace may be generated multiple times
     * in exec cycle and / or delta cycle
     */
    virtual void generate_dump() = 0;

    /**
     * @brief an API to know if the trace is already generated at curren time
     * @return true/false accordingly
     */
    bool is_trace_generated_at_current_time() const
    {
        return m_trace_generated_at_cur_time;
    }

    static bool is_atleast_a_trace_obj_created()
    {
        return m_atleast_a_trace_generated;
    }
protected:
    /**
     * @brief API to reset the trace generation state to not generated state
     */
    void reset_trace_generation_at_cur_time()
    {
        m_trace_generated_at_cur_time = false;
    }

    /**
     * @brief API to set the trace generation state to generated state
     */
    void set_trace_generation_at_cur_time()
    {
        m_trace_generated_at_cur_time = true;
    }

private:
    bool m_trace_generated_at_cur_time = true; //!< State of the trace generation
    inline static bool m_atleast_a_trace_generated = false;
};

/*
 * Helper metaprogramming constructs to derive the size of data type in bits
 */
//For generic data types, consider size as size in bytes
template<typename T> struct __msm_trace_obj_size_in_bits
{
    constexpr static unsigned int const value = sizeof(T) * 8;
};

//Specialize the size for bool data type as 1 bit for SystemC support
template<> struct __msm_trace_obj_size_in_bits<const bool&>
{
    constexpr static unsigned int const value = 1;
};

//Specialize the size for bool data type as 1 bit for MSM support
template<> struct __msm_trace_obj_size_in_bits<bool&>
{
    constexpr static unsigned int const value = 1;
};

template<> struct __msm_trace_obj_size_in_bits<const bool>
{
    constexpr static unsigned int const value = 1;
};

template<> struct __msm_trace_obj_size_in_bits<bool>
{
    constexpr static unsigned int const value = 1;
};

//For std::bitset<N> data type consider size as the size of the bitset for SystemC Support
template<size_t N> struct __msm_trace_obj_size_in_bits<const std::bitset<N>&>
{
    constexpr static unsigned int const value = N;
};

//For std::bitset<N> data type consider size as the size of the bitset for MSM Support
template<size_t N> struct __msm_trace_obj_size_in_bits<std::bitset<N>&>
{
    constexpr static unsigned int const value = N;
};

//For std::bitset<N> data type consider size as the size of the bitset for SystemC Support
template<size_t N> struct __msm_trace_obj_size_in_bits<const std::bitset<N>>
{
    constexpr static unsigned int const value = N;
};

//For std::bitset<N> data type consider size as the size of the bitset for MSM Support
template<size_t N> struct __msm_trace_obj_size_in_bits<std::bitset<N> >
{
    constexpr static unsigned int const value = N;
};

} /* namespace msm_trace */
