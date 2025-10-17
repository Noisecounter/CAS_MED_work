// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#pragma once

#include <iomanip>

#include "xtlm.h"
#include "xtlm_ap_ctrl.h"
#include "report_handler.h"
#include "func_emu_mm.h"

extern "C" {

    void __tlm_load_generic(void *tlm_inst, char *addr, char *data, size_t size);
    void __tlm_store_generic(void *tlm_inst, char *addr, char *data, size_t size);

}

namespace xsc {
namespace func {

class xtlm_ap_ctrl : public xsc::xtlm_ap_ctrl
{

  public:
    xtlm_ap_ctrl(sc_module_name name);

    //! Mapping functions for kernel args and its mmap interface
    void set_mm_if(uint64_t addr, xsc::func::mm::access_if *&mm_if);
    int get_mm_if(uint64_t addr, xsc::func::mm::access_if *&mm_if);

    char* get_m_name() { return m_name; }
    xsc::common_cpp::report_handler* get_logger() { return m_log; }

  protected:
    std::map< uint64_t, xsc::func::mm::access_if*, std::greater<uint64_t> > mm_if_map;
};

class xtlm_ap_ctrl_none : public xsc::xtlm_ap_ctrl_none
{
  public:
    xtlm_ap_ctrl_none(sc_module_name name);
};
 
typedef xtlm_ap_ctrl xtlm_ap_ctrl_hs;
typedef xtlm_ap_ctrl xtlm_ap_ctrl_chain;

}
}

