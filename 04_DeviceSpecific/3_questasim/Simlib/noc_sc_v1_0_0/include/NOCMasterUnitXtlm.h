// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
#ifndef _NOCMasterUnitXtlm_h_
#define _NOCMasterUnitXtlm_h_

#include <systemc>
#include "properties.h"
#include "xtlm.h"
#include "xtlm_interfaces/xtlm_aximm_protocol_type.h"
#include <memory>
#include <string>
#include <map>
#include "NOCCfgRegExtract.h"
#include "ClockDomainRef.h"
#include "NOCCoreXtlm.h"
#include "utils/xtlm_aximm_initiator_stub.h"
#include "utils/xtlm_axis_initiator_stub.h"
#include "XSCLogger.h"
#include "NOCComponentBase.h"

#define NMU_REG_MODE_SELECT 0x000002C8
namespace SimHrdIP {
namespace FuncSim {
class NMUWrapperTop;
class NOCComponentBase;
}
}
class NOCCoreClockImpl;

class NOCMasterUnitXtlm : public sc_core::sc_module/*, public tlm::tlm_fw_transport_if<xtlm::xtlm_aximm_protocol_types>, public tlm::tlm_bw_transport_if<xtlm::xtlm_aximm_protocol_types>*/
{
	public:
	    SC_HAS_PROCESS(NOCMasterUnitXtlm);
	    NOCMasterUnitXtlm(sc_core::sc_module_name, const xsc::common_cpp::properties& _properties );
	    ~NOCMasterUnitXtlm();

	    SimHrdIP::FuncSim::NOCComponentBase* m_NmuModel;

	    //! \brief sc ports
	    sc_core::sc_in_clk              aclk;
	    template <typename T> using sc_optional_in = sc_core::sc_port
		<sc_core::sc_signal_in_if<bool>, 1,
		sc_core::SC_ZERO_OR_MORE_BOUND>;  

	    SimHrdIP::FuncSim::XSCIPLogger* mLogger;
	    sc_optional_in<bool>            aresetn;
	    xtlm::xtlm_aximm_target_socket  arr_socket;   // AR/R: Read  channels of AXIMM
	    xtlm::xtlm_aximm_target_socket  awb_socket;   // AW/B: Write channels of AXIMM
	    xtlm::xtlm_axis_target_socket   at_socket;    // AT  : Transfer channel of AXIS
	    std::unique_ptr<xtlm::xtlm_aximm_initiator_stub> m_arr_tieoff;
	    std::unique_ptr<xtlm::xtlm_aximm_initiator_stub> m_awb_tieoff;
	    std::unique_ptr<xtlm::xtlm_axis_initiator_stub > m_at_tieoff;

	    //xtlm::xtlm_aximm_initiator_socket  int_arr_socket;   // AR/R: Read  channels of AXIMM
	    std::unique_ptr< xtlm::xtlm_axis_target_socket_util> m_st_util;
	    //xtlm::xtlm_aximm_initiator_socket  int_awb_socket;   // AW/B: Write channels of AXIMM
	    //xtlm::xtlm_axis_initiator_socket  int_at_socket;   // AW/B: Write channels of AXIMM
	    //xtlm::xtlm_aximm_target_rd_socket_util m_rd_util;
	    //xtlm::xtlm_aximm_target_wr_socket_util m_wr_util;
	    //xtlm::xtlm_aximm_initiator_socket  int_awb_socket;   // AW/B: Write channels of AXIMM
	    //xtlm::xtlm_axis_initiator_socket  int_at_socket;   // AW/B: Write channels of AXIMM

	//    std::unique_ptr< xtlm::xtlm_aximm_target_rd_socket_util>  m_rd_util;
	//    std::unique_ptr< xtlm::xtlm_aximm_target_wr_socket_util > m_wr_util;
	//    std::unique_ptr< xtlm::xtlm_axis_target_socket_util> m_st_util;
	    /*
	       virtual tlm::tlm_sync_enum nb_transport_fw(
	       xtlm::aximm_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t);
	       virtual void b_transport( xtlm::aximm_payload& trans, sc_time& delay );
	       virtual bool get_direct_mem_ptr(xtlm::aximm_payload& trans, tlm::tlm_dmi& dmi_data){}
	       virtual unsigned int transport_dbg(xtlm::aximm_payload& trans){}
	       virtual tlm::tlm_sync_enum nb_transport_bw(
	       xtlm::aximm_payload& trans,
	       tlm::tlm_phase& phase,
	       sc_core::sc_time& t);
	       virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range){}
	       */

	    virtual void before_end_of_elaboration();
	    virtual void end_of_elaboration();  
	    virtual void start_of_simulation();
	    virtual void end_of_simulation();
		void handleReset();
        //sc_signal< sc_bv<12> > NMU_WR_USR_DST;
        //sc_signal< sc_bv<12> > NMU_RD_USR_DST;
        //sc_signal< sc_bv<3> > NMUHBM_WR_USR_DST;
        //sc_signal< sc_bv<3> > NMUHBM_RD_USR_DST;
        //sc_signal< sc_bv<4> > NOC2NMU_WR_USR_DST;
        //sc_signal< sc_bv<4> > NOC2NMU_RD_USR_DST;
        //sc_signal< bool >      NMU_WR_DEST_MODE;
        //sc_signal< bool >      NMU_RD_DEST_MODE;
	private:
	    std::string getPhyName(const std::string& hdlName, const std::string& sNocAttrFile); 
	private:
	    // HDL simulation context
	    std::string                                 m_hdlAttrFile;
	    std::string                                 m_hdlCompName;
	    //
	    std::string                                 m_physName;
	    uint32_t                                    m_axiDataWidth;
	    SimHrdIP::FuncSim::ClockDomain*             m_pFuncAxiDomain = nullptr;
	    SimHrdIP::NocCfgHdlAttrExtract* m_hdlAttrExt = nullptr;
	    NOCCoreClockImpl* m_coreClockImpl = nullptr;
};

#endif
