// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#ifndef _NOCSlaveUnitXtlm_h_
#define _NOCSlaveUnitXtlm_h_

#include <map>
#include "NOCCfgRegExtract.h"
#include "ClockDomainRef.h"
#include "NOCCoreXtlm.h"
#include "utils/xtlm_aximm_target_stub.h"
#include "utils/xtlm_axis_target_stub.h"
#include "XSCLogger.h"
#include "NOCBackdoorObserver.h"
#include "NOCBackdoorHandler.h"
#define NSU_REG_MODE_SELECT 0x00000108
#define NSU2_REG_MODE_SELECT 0x0000010C
#define NSU_REG_SRC 0x00000100

namespace SimHrdIP {
namespace FuncSim {
class NOCNSUWrapperTop;
}
}

class NOCSlaveUnitXtlm : public sc_module ,
    public SimHrdIP::FuncSim::NOCBackdoorObserver

{
	public:
	    SC_HAS_PROCESS(NOCSlaveUnitXtlm);
	    NOCSlaveUnitXtlm(sc_core::sc_module_name, const xsc::common_cpp::properties& _properties );
	    ~NOCSlaveUnitXtlm();

        SimHrdIP::FuncSim::NOCComponentBase* m_NsuModel;

	    //! \brief sc ports
	    sc_core::sc_in_clk              aclk;
	    template <typename T> using sc_optional_in = sc_core::sc_port
		<sc_core::sc_signal_in_if<bool>, 1,
		sc_core::SC_ZERO_OR_MORE_BOUND>;  

	    sc_optional_in<bool>            aresetn;
	    SimHrdIP::FuncSim::XSCIPLogger* mLogger;
	    xtlm::xtlm_aximm_initiator_socket arr_socket; // AR/R: Read  channels of AXIMM
	    xtlm::xtlm_aximm_initiator_socket awb_socket; // AW/B: Write channels of AXIMM
	    xtlm::xtlm_axis_initiator_socket  at_socket;  // AT  : Transfer channel of AXIS
	    std::unique_ptr<xtlm::xtlm_aximm_target_stub> m_arr_tieoff;
	    std::unique_ptr<xtlm::xtlm_aximm_target_stub> m_awb_tieoff;
	    std::unique_ptr<xtlm::xtlm_axis_target_stub > m_at_tieoff;
	    virtual void before_end_of_elaboration();
	    virtual void end_of_elaboration();  
	    virtual void start_of_simulation();
	    virtual void end_of_simulation();

        void setBurstLength( xtlm::aximm_payload& txn );
		virtual void b_transport(unsigned int id,xtlm::aximm_payload& pkt, sc_time&);
		virtual unsigned int transport_dbg(unsigned int id, xtlm::aximm_payload& pkt);
		virtual void b_transport(unsigned int id,xtlm::axis_payload& pkt, sc_time&);
		virtual unsigned int transport_dbg(unsigned int id,xtlm::axis_payload& pkt);

		void handleReset();
	private:
	    std::string getPhyName(const std::string& hdlName, const std::string& sNocAttrFile); 
	    // HDL simulation context
	    std::string                                 m_hdlAttrFile;
	    std::string                                 m_hdlCompName;
	    //
	    std::string                                 m_physName;
	    uint32_t                                    m_axiDataWidth;
	    SimHrdIP::FuncSim::ClockDomain*             m_pFuncAxiDomain =nullptr;
	    SimHrdIP::NocCfgHdlAttrExtract* m_hdlAttrExt = nullptr;
	    NOCCoreClockImpl* m_coreClockImpl = nullptr;
        bool isStream;

};

#endif
