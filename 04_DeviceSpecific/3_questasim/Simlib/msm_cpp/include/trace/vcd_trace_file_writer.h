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
#include <fstream>
#include <list>
#include <queue>
#include <string>
#include <utility>

#include "msm_trace_obj_base.h"
#include "vcd_trace_object.h"
#include "msm_port.h"


namespace msm_trace
{

/**
 * @class Class for handling VCD file writing. This class primarily focuses on file operations and
 * not on the trace generation. VCD trace generation is off loaded to vcd_trace_objects so as to
 * enable vcd trace generation using multiple threads. File IO has to be done from single thread,
 * so as to retain data integrity.
 */
class vcd_trace_file_writer
{
public:
    /**
     * @brief Constructor
     * @param file_name is the name of the VCD file to be generated
     */
    vcd_trace_file_writer(const std::string file_name, bool type);
    virtual ~vcd_trace_file_writer();

    /**
     * @brief API to register trace information and get unique id for the trace
     * @param trace_name trace object name
     * @param width width of the trace object
     * @return returns unique trace id for the given trace
     */
    std::string register_trace_and_get_id(std::string trace_name, unsigned int width);

    /**
     * @brief API to finish trace registration. No further registration of traces after calling this
     *  API. Complete the header and initial dump.
     */
    void finish_trace_registration();

    /**
     * @brief An API to dump generated traces to file. This API handles buffered trace data over
     * multiple clock cycles
     * @param buffered_traces is buffered trace date over multiple cycles
     */
    void dump_traces(std::queue<std::pair<uint64_t, std::list<std::string> > > buffered_traces);

    /**
     * @brief An API to dump generated traces to file. This API handles trace data generated in
     * single clock cycle
     * @param cycle_trace_data is the trace data generated in single cycle
     */
    void dump_traces( std::pair<uint64_t, std::list<msm_trace::msm_trace_obj_base*> > each_cycle);

    /**
     * @brief An API to add comments into the vcd file
     * @param comment is the comment string to be added
     */
    void write_comment(const std::string& comment);

    /**
     * @brief An explicit API to flush the data to file
     */
    void flush();

    void dump_init_data ();

private:
    const std::string m_file_name; //!< Name of the VCD file
    std::ofstream m_vcd_fs; //!< File stream handle for performing file operations
    uint64_t unique_id; //!< Unique ID generated for each of the trace registration
    bool m_scope_header_written = false; //!< Check for if scope header is written during register trace

    std::list<std::string> m_trace_data; //!< Trace data generated only for init dump..//TODO need to be removed
};
//TODO : Need to create msm_trace_file base class (which should be generic for any type of tracing
// in the infra). This is a workaround until then
using msm_trace_file = vcd_trace_file_writer;

/**
 * @brief an API to create VCD trace file instance
 * @param file_name is the name of the VCD file
 * @return returns the pointer to the vcd_trace_file_writer
 */
vcd_trace_file_writer* msm_create_vcd_trace_file(std::string file_name, bool type = true);

/**
 * @brief an API to get the vcd_trace_file in the infra (only 1 is supported)
 * @return returns the pointer to vcd_trace file
 */
vcd_trace_file_writer* get_vcd_trace_file(); //TODO : Deprecate this, need to support multiple files

/**
 * @brief an API to register trace generation of the given object (C++ data types)
 * @tparam T is the data type of the trace to be generated
 * @param tf is the trace_file pointer
 * @param trace_inst is the instance of the object for which trace need to be generated
 * @param name is the of the trace
 * @param width is width of the trace instance in bits. This is evaluated by the compiler, but user
 * can override
 */
template<class T>
void msm_trace(vcd_trace_file_writer* tf, T& trace_inst, std::string name,
        unsigned int width = __msm_trace_obj_size_in_bits<T>::value)
{
    name = msm_core::normalize_the_name_across_simaulators(name);
    auto unique_id = tf->register_trace_and_get_id(name, width);
    auto trace_obj = new vcd_trace_object<const T&>(trace_inst, unique_id);
}

//For Ports
template<class T>
void msm_trace(vcd_trace_file_writer* tf, msm_core::msm_in<T>& ports, std::string name,
        unsigned int width = __msm_trace_obj_size_in_bits<T>::value)
{
    name = msm_core::normalize_the_name_across_simaulators(name);
    auto unique_id = tf->register_trace_and_get_id(name, width);
    auto trace_obj = new vcd_trace_object_msm<const T&>(ports.m_interface->read(), unique_id);
    ports.m_interface->m_trace_obj.push_back(trace_obj);
}

template<class T>
void msm_trace(vcd_trace_file_writer* tf, msm_core::msm_out<T>& ports, std::string name,
        unsigned int width = __msm_trace_obj_size_in_bits<T>::value)
{
    name = msm_core::normalize_the_name_across_simaulators(name);
    auto unique_id = tf->register_trace_and_get_id(name, width);
    auto trace_obj = new vcd_trace_object_msm<const T&>(ports.m_interface->read(), unique_id);
    ports.m_interface->m_trace_obj.push_back(trace_obj);
}

/**
 *
 * @brief an API to register trace generation of the given object (msm_signal type)
 * @tparam T is the data type of the trace to be generated
 * @param tf is the trace_file pointer
 * @param intf is the msm_signal for which trace need to be generated
 * @param width is with of the msm_interace. This is evaluated by the compiler, but user can override
 */
template<class T>
void msm_trace(vcd_trace_file_writer* tf, msm_core::msm_signal<T>& intf, std::string name,
        unsigned int width = __msm_trace_obj_size_in_bits<T>::value)
{
    name = msm_core::normalize_the_name_across_simaulators(name);
    auto unique_id  = tf->register_trace_and_get_id(name, width);
    auto trace_obj = new vcd_trace_object_msm<const T&>(intf.read(), unique_id);
    intf.m_trace_obj.push_back(trace_obj);
}


} /* namespace msm_trace */
