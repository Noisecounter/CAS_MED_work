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

#include <cstdint>
#include <stdexcept>
#include <string>

namespace msm_core
{

/**
 * @enum time unit of the simulation time
 */
enum msm_time_unit
{
    MSM_FS = 0,/**< MSM_FS */
    MSM_PS,    /**< MSM_PS */
    MSM_NS,    /**< MSM_NS */
    MSM_US,    /**< MSM_US */
    MSM_MS,    /**< MSM_MS */
    MSM_SEC    /**< MSM_SEC */
};

/**
 * @brief API to convert string to msm_time_unit
 * @param str_time_unit is the time unit in string
 * @return returns time unit
 */
msm_time_unit string_to_msm_time_unit(const std::string str_time_unit);

/**
 * @class msm_time class to represent time in the msm_infrastructure
 */
class msm_time
{
public:
    /**
     * @brief constructor
     * @param time_val of the simulation with given time unit
     * @param msm_time_unit time unit for the given time value
     */
    msm_time(double time_val = 0, msm_time_unit msm_time_unit = msm_time_unit::MSM_NS);

    /**
     * @brief time unit of the msm_time instance
     * @return returns time unit
     */
    msm_time_unit get_time_unit() const;

    /**
     * @brief get the time value relative to time resolution of the instance
     * @return the time value relative to time resolution of the instance
     */
    double get_time_val() const;

    /**
     * @brief API to return the
     * @return returns time in seconds
     */
    double to_seconds() const;

    /**
     * @brief API to return the time in string format with units
     */
    std::string to_string() const;

    /*
     * += operator overload, Increments LHS by RHS time
     */
    msm_time& operator +=(const msm_time &rhs_time);

    /*
     * Relational operator overloads
     */
    bool operator == (const msm_time &rhs_time) const;
    bool operator != (const msm_time &rhs_time) const;
    bool operator <  (const msm_time &rhs_time) const;
    bool operator <= (const msm_time &rhs_time) const;
    bool operator >  (const msm_time &rhs_time) const;
    bool operator >= (const msm_time &rhs_time) const;
private:
    double m_time_val; //!< Time value w.r.t to given time units
    msm_time_unit m_time_unit; //!< Time unit
};

} /* namespace msm_core */
