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

#include <functional>
#include <string>
#include <vector>

#include "msm_module_name.h"
#include "msm_object.h"

namespace msm_core
{

/**
 * @class container class for msm_objects to mimic vector behavior
 */
template <class T>
class msm_vector : public msm_object
{
    typedef std::vector<T*> storage_type; //!< typedef for Standard vector container to store pointers of class type T

//    We will add iterator support at a later point in time
//    typedef typename storage_type::iterator iterator; //!<
//    typedef typename storage_type::const_iterator const_iterator;

public:
    /**
     * @brief Constructor for msm_vector
     * @param prefix is the prefix name for the elements / msm_objects in the vector
     * @param size is the number of elements in the constructor
     */
    msm_vector(const std::string prefix, unsigned int size=0) : msm_object(msm_module_name(prefix))
    {
        init_function(size, create_element);
    }

    /**
     * @brief Constructor for msm_vector
     * @param prefix is the prefix name for the elements / msm_objects in the vector
     * @param size is the number of elements in the constructor
     * @param creator the user defined allocator function for the element construction
     */
    msm_vector(const std::string prefix, unsigned int size,
            std::function<T* (const std::string&, const unsigned int&)> creator): msm_object(msm_module_name(prefix))
    {
        init_function(size, creator);
    }

    /**
     * @brief Destructor to clear/deallocate the elements
     */
    ~msm_vector()
    {
        clear();
    }

    /*
     * Some operator overloads for ease of use
     */
    T& operator[] (unsigned int i)
    {
        return *(m_vec_ptrs.at(i));
    }

    T& at(unsigned int i)
    {
        return *(m_vec_ptrs.at(i));
    }

    const T& operator[] (unsigned int i) const
    {
        return *(m_vec_ptrs.at(i));
    }

    const T& at(unsigned int i) const
    {
        return *(m_vec_ptrs.at(i));
    }

    const unsigned int size() const
    {
        return m_vec_ptrs.size();
    }

    /* We will add iterator support at a later point in time
    iterator begin()
    {
        return m_vec_ptrs.begin();
    }

    iterator end()
    {
        return m_vec_ptrs.end();
    }

    const_iterator begin() const
    {
        return m_vec_ptrs.begin();
    }
    const_iterator end() const
    {
        return m_vec_ptrs.end();

    }
    */
  /**
     * @brief API to initialiaze the vector
     * @param n is the number of elements to be created
     */
    void init(unsigned int n)
    {
        //Reserve the memory
        init_function(n, create_element);
    }


private:

    storage_type m_vec_ptrs; //!< Standard vector container to store pointers of class type T

    /**
     * @brief Default function for creating element when user hasn't specified allocator
     * @param prefix prefix is the prefix for each of the object in the current vector
     * @param index index is the index number of the element (starts with 0)
     * @return returns element of the type T
     */
    static T* create_element(const std::string prefix, unsigned int index)
    {
        std::string element_name = prefix + "_" + std::to_string(index);
        return new T (element_name);
    }

    /**
     * @brief API to clear / destroy the elements in the container
     */
    void clear()
    {
        for(auto elem : m_vec_ptrs)
            delete elem;
    }

    /**
     * @brief API to initialiaze the vector
     * @param n is the number of elements to be created
     * @param creator is the creator function / allocator for element
     */
    void init_function(unsigned int n, std::function<T* (const std::string, unsigned int)> creator)
    {
        //Reserve the memory
        m_vec_ptrs.reserve(n);

        try {

            for(unsigned int i = 0; i < n ; i++)
            {
                m_vec_ptrs.push_back(creator(this->basename(), i));
            }
        }
        catch(...)
        {
            clear();
            throw;
        }
    }

};

} /* namespace msm_core */
