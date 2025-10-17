// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#ifndef _NOCHbmMcXtlm_h_
#define _NOCHbmMcXtlm_h_

#include <systemc>
#include "properties.h"
#include <memory>
#include <string>
#include <map>
#include <set>
#include "ClockDomainRef.h"
#include "NOCCoreXtlm.h"
#include "XSCLogger.h"

namespace SimHrdIP {
namespace FuncSim  {
    class HBMWrapperTop;
}
}

class NOCCoreClockImpl;

class NocHbmRegCfgExtract {
public:
	NocHbmRegCfgExtract(const std::string& filename);
	using NameValueMap = std::map<std::string, uint32_t>;

	const std::vector<NameValueMap> chNumToAttr;
};

class NOCHbmMcXtlm : public sc_core::sc_module
{
	public:
	    SC_HAS_PROCESS(NOCHbmMcXtlm);
	    NOCHbmMcXtlm(sc_core::sc_module_name, const xsc::common_cpp::properties& _properties );
	    ~NOCHbmMcXtlm();

	    std::vector<SimHrdIP::FuncSim::HBMWrapperTop*> m_HbmModel;

	    static bool mIsHbmMcBindingComplete;
	    static bool mIsHbmMcInstantationComplete;
	    //! \brief sc ports
	    sc_core::sc_in_clk              mc_clk0;       // stack 0 controller clock
	    sc_core::sc_in_clk              mc_clk1;       // stack 1 controller clock
	    
		template <typename T> using sc_optional_in = sc_core::sc_port
		<sc_core::sc_signal_in_if<bool>, 1,
		sc_core::SC_ZERO_OR_MORE_BOUND>;  
	    sc_optional_in<bool>            mc_rst_n;
		
	    SimHrdIP::FuncSim::XSCIPLogger* mLogger;

	    virtual void before_end_of_elaboration();
	    virtual void end_of_elaboration();  
	    virtual void start_of_simulation();
	    virtual void end_of_simulation();
	    void handleReset();
	private:
	   void getPhyName(const std::string& hdlName, const std::string& sNocAttrFile); 
	   void getRegFromStackFile(const std::string& sStackAttrFile);
	   void clkSetting();
	private:
	    // HDL simulation context
	    std::string                                 m_hdlAttrFile;
	    std::string                                 m_hdlCompName;
	    //
	    std::string                                 m_physName;
	    SimHrdIP::FuncSim::ClockDomain*             m_pFuncDfiDomain0 =nullptr;
	    SimHrdIP::FuncSim::ClockDomain*             m_pFuncDfiDomain1 =nullptr;
	    SimHrdIP::NocCfgHdlAttrExtract* m_hdlAttrExt = nullptr;
	    //NocHbmRegCfgExtract* m_chAttrExt = nullptr;
	    std::unique_ptr<sc_core::sc_clock>   m_dfiInternalClk0;
	    std::unique_ptr<sc_core::sc_clock>   m_dfiInternalClk1;
	    NOCCoreClockImpl* m_coreClockImpl=nullptr;
	    std::vector<std::pair<std::string, std::string>> m_CompName2PhyName;
	    bool mStackActive[2]= { false,false};
};

#endif
