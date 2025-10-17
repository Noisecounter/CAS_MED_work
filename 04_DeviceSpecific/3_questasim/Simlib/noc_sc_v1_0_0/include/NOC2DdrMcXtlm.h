// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#ifndef _NOC2DdrMcXtlm_h_
#define _NOC2DdrMcXtlm_h_

#include "properties.h"
#include "xtlm.h"
#include <memory>
#include <string>
#include <map>
#include "ClockDomainRef.h"
#include "NOCCoreXtlm.h"
#include "XSCLogger.h"
#include <DDR5WrapperTop.h>


class NOCCoreClockImpl;

class NocDdrRegCfgExtract {
public:
	NocDdrRegCfgExtract(const std::string& filename);
	using NameValueMap = std::map<std::string, uint32_t>;

	const NameValueMap chNumToAttr;
};

class NOC2DdrMcXtlm : public sc_core::sc_module {
public:

  SC_HAS_PROCESS(NOC2DdrMcXtlm);
  NOC2DdrMcXtlm( sc_core::sc_module_name nm, const xsc::common_cpp::properties& _properties );
  ~NOC2DdrMcXtlm();
  
  //! \brief sc ports
  template <typename T> using sc_optional_in = sc_core::sc_port
                                               <sc_core::sc_signal_in_if<bool>, 1,
                                                sc_core::SC_ZERO_OR_MORE_BOUND>;  
  
  sc_core::sc_in_clk              mc_clk;       // controller clock
  sc_optional_in<bool>            mc_rst_n;
  //This is a test signal that is enabled only in debug mode. 
  //We are adding this signal to view ddrmc_clk frequency in 
  //waveforms. To enable viewing the signal environment
  //variable DBG_NOC_CLK_FREQ must be set.
  sc_core::sc_signal<bool> test_ddrmc_clk;

  virtual void before_end_of_elaboration();  
  virtual void end_of_elaboration();  
  virtual void start_of_simulation();
  virtual void end_of_simulation();
  void handleReset();
private:

  void getPhyNameAndSetRegs();
  void getRegFromStackFile(const std::string& sStackAttrFile);
  void initialize();
  
  SimHrdIP::FuncSim::DDR5WrapperTop* 	      mDDRWrapper =nullptr;
  SimHrdIP::FuncSim::ClockDomain*             mDDRDomain  =nullptr;
  std::string                                 m_hdlAttrFile;
  std::string                                 m_hdlCompName;
  std::string                                 mCompName;
  std::unique_ptr<sc_core::sc_clock>   m_mcIntClk;
  NOCCoreClockImpl* m_coreClockImpl = nullptr;
  SimHrdIP::FuncSim::XSCIPLogger* mLogger = nullptr;
  SimHrdIP::NocCfgHdlAttrExtract* m_hdlAttrExt = nullptr;
  //NocDdrRegCfgExtract* m_chAttrExt = nullptr;

};

#endif
