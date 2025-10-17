// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689


#ifndef SIM_QDMA_H_
#define SIM_QDMA_H_

#include "xtlm.h"
#include <fstream>
#include <list>
#include <sys/types.h>
#include "xcl_api.h"
#include "sk_client.h"
#include "sim_qdma_helper.h"
#include "sim_qdma_base.h"
#include "report_handler.h"
#include "sim_qdma_core.h"
#include <sstream>
#include <tuple>
#include <vector>


class sim_qdma :public sc_module,public sim_qdma_base {
public :
	sim_qdma(sc_core::sc_module_name name,xsc::common_cpp::properties&,std::vector<PF_BAR_st_type*> *bar_info_ptr=0);
	SC_HAS_PROCESS(sim_qdma);
	//Exposed Interfaces
	xtlm::xtlm_aximm_initiator_socket* initiator_ctrl_rd_socket;
	xtlm::xtlm_aximm_initiator_socket* initiator_ctrl_wr_socket;

	xtlm::xtlm_aximm_initiator_socket* initiator_mm_rd_socket;
	xtlm::xtlm_aximm_initiator_socket* initiator_mm_wr_socket;

	xtlm::xtlm_aximm_initiator_socket* initiator_mm_aux_rd_socket;
	xtlm::xtlm_aximm_initiator_socket* initiator_mm_aux_wr_socket;

	xtlm::xtlm_axis_initiator_socket*  initiator_stm_stream_socket;
	xtlm::xtlm_axis_target_socket*     target_stm_stream_socket;
    
    xtlm::xtlm_aximm_target_socket* target_mm_rd_socket;
	xtlm::xtlm_aximm_target_socket* target_mm_wr_socket;

	sc_core::sc_in_clk   m_aximm_aclk;
	sc_core::sc_in_clk   m_axictrl_aclk;
	sc_core::sc_in<bool> m_axictrl_aresetn;
	sc_core::sc_in<bool> m_aximm_aresetn;
	sc_core::sc_in_clk   m_aximm_aux_aclk;
	sc_core::sc_in<bool> m_aximm_aux_aresetn;
	sc_core::sc_in_clk   m_axis_aclk;
	sc_core::sc_in<bool> m_axis_aresetn;
	sc_core::sc_in_clk   s_axis_aclk;
	sc_core::sc_in<bool> s_axis_aresetn;
	sc_core::sc_in_clk   s_aximm_aclk;
	sc_core::sc_in<bool> s_aximm_aresetn;
    sc_core::sc_in<sc_bv<16> > usr_irq_req;
    sc_core::sc_out<sc_bv<16> > usr_irq_ack;

	// Callbacks to be invoked by underlying handler
	bool xclWrite(AddressSpace space, uint64_t offset, const void *hostBuf,size_t size,uint32_t PFDEV_ID=0, uint32_t BAR_ID=0);
	bool xclRead(AddressSpace space, uint64_t offset, void *hostBuf,size_t size,uint32_t PFDEV_ID=0, uint32_t BAR_ID=0);
    unsigned int xclCreateQueue(queue_context &q_ctx);
    bool     xclWriteQueue(uint64_t q_handle, const void *src, size_t size,unsigned int req_id,bool eot,bool isnonblocking );
    uint64_t xclReadQueue(uint64_t q_handle, void *dest, size_t size ,unsigned int req_id,bool eot,bool isnonblocking );
    bool xclDestroyQueue(uint64_t q_handle);
    bool xclPollCompletion(unsigned int req_id,std::vector<std::tuple<void*,uint32_t,bool> >&);
    bool xclAllocDeviceBuffer(uint64_t base_address, uint64_t size,std::string& buffer_filename);
    bool xclFreeDeviceBuffer(uint64_t base_address);
    bool xclImportBO(std::string sFileName,uint64_t offset,uint64_t size) ;
    bool xclCopyBO(uint64_t src_handle,std::string dst_filename,uint64_t size, uint64_t src_offset,uint64_t dst_offset) ;
    void xclCloseQ2X();

	static sim_qdma* getInstance() {      	  return m_instance; }
	// Infrastructure used by XSIM for DDR backdoor access
	void ddrRead(unsigned int ddrNum, uint64_t offset, void* dest,unsigned int size);
	~sim_qdma();
private :

	xsc::common_cpp::report_handler*              m_report_handler;
	sim_qdma_core*                                m_sim_qdma_core;
	sim_qdma_helper*                              m_sim_qdma_helper;
	sc_signal<bool> m_aximm_aux_aclk_signal;
	sc_signal<bool> m_aximm_aux_aresetn_signal;
	sc_signal<bool> s_axis_aclk_signal;
	sc_signal<bool> s_axis_aresetn_signal;
	sc_signal<bool> m_axis_aclk_signal;
	sc_signal<bool> m_axis_aresetn_signal;
	sc_signal<bool> s_aximm_aclk_signal;
	sc_signal<bool> s_aximm_aresetn_signal;
	void initModel();
	std::stringstream m_ss;
	void simulate_single_cycle();
	void end_of_elaboration();
	void start_of_simulation();
	void calculate_clk_period();
	sc_core::sc_time m_clk_period;
	//Singleton instance
	static sim_qdma *m_instance;
};

//Global Functions to support XSim GDB debug of Kernel
void registerDdrRead();
extern "C" void WEAKFUNC iki_register_DDR_read(int (*fp)(int, int));
int ddrRead(int ddrNum, int offset);
DPI_LINKER_DECL
extern "C" size_t get_sc_time_stamp();
DPI_LINKER_DECL
double get_sc_time_insec();

#endif /* sim_qdma_H_ */


