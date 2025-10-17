// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#ifndef _NOCCoreXtlm_h_
#define _NOCCoreXtlm_h_

#include <systemc>
#include "properties.h"
#include <string>
#include <map>
#include <memory>
#include "ClockDomainRef.h"
#include "NOCComponentBase.h"
#include "NPPSocketWrapper.h"
#include "NOCTopoExtract.h"
#include "NOCCfgRegExtract.h"
#include "XSCLogger.h"

class NOCCoreClockImpl;

class NOCCoreXtlm : public sc_core::sc_module {
    public:
	SC_HAS_PROCESS(NOCCoreXtlm);
	NOCCoreXtlm(sc_core::sc_module_name, const xsc::common_cpp::properties& );
	NOCCoreXtlm();
	~NOCCoreXtlm();
	static SimHrdIP::FuncSim::NPPSocketWrapper* getNPPSocket(std::string comp_name, std::string port_name);
	static std::map< std::string, std::vector< std::pair < std::string, SimHrdIP::FuncSim::NOCComponentBase* > > > compType2compNamePtr;
	static std::map< std::string, std::vector< std::pair < std::string, SimHrdIP::FuncSim::NPPSocketWrapper* > > > compName2PortNamePtr;
	void execute_clk();
	void handleReset();

    private:
	SimHrdIP::FuncSim::NOCComponentBase* getNOCModel(std::string nm);
	virtual void end_of_elaboration();  
	virtual void start_of_simulation();
	virtual void end_of_simulation();
	void create(::SimHrdIP::NocTopoExtract::CompByType comp2Type, bool isStream);
	void connect(::SimHrdIP::NocTopoExtract::LinkEndPoints);
	void registerStrConfig(const std::string& compName, const char* regValueStr);
	bool ifBind(std::string comp, unsigned int port);
	bool isBound(std::string srcComp, std::string srcPort);
	void fillMasterSlavePortConnection(std::string srcComp, std::string srcPort, std::string dstComp, std::string dstPort);


    public:
	sc_core::sc_in_clk       noc_clk;
	sc_core::sc_in<bool>     noc_rst_n;
	SimHrdIP::FuncSim::XSCIPLogger* mLogger = nullptr;


    private:
	std::unique_ptr<sc_core::sc_clock>   m_nocInternalClk;
	std::string deviceFile;
	std::string sysAddrFile;
	std::string registerFile;
	std::map<std::pair<std::string, unsigned int>, bool> compPortBind;
	NOCCoreClockImpl* m_coreClockImpl;
};

#endif
