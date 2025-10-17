// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689


#ifndef SIM_QDMA_CORE_H_
#define SIM_QDMA_CORE_H_

#include "xtlm.h"
#include <fstream>
#include <list>
#include <sys/types.h>
#include "xcl_api.h"
#include "sk_client.h"
#include "sim_qdma_helper.h"
#include "sim_qdma_base.h"
#include "report_handler.h"
#include <queue>
#include <map>
#include <sstream>
#include <tuple>
#include <vector>
#include "unix_socket.h"
#include "rpc_messages.pb.h"

namespace qdma {
enum qdma_intf {
	QDMA_CTRL = 0, QDMA_MM = 1, QDMA_MAUX = 2, QDMA_MSTR = 3 ,QDMA_SSTR=4, QDMA_S_MM = 5
};

typedef struct qdma_itf_prop {
	uint64_t addr_width;
	uint64_t data_width;
	uint64_t user_width;
	uint64_t id_width;
	bool support_burst;
	bool support_backdoor_access;
	bool enable;
	qdma_itf_prop() {
		addr_width = 32;
		data_width = 32;
		user_width = 32;
		id_width = 32;
		support_burst = true;
		support_backdoor_access = true;
		enable = true;
	}
} qdma_itf_prop_type;

typedef struct stream_nbl_req {
	uint32_t       rid;
	unsigned char* ptr;
	uint32_t       size;
	bool           iseot;
	bool           isprocessed;
} stream_nbl_req_type;

}
class sim_qdma_core :public sc_module,public sim_qdma_base {
public :
	sim_qdma_core(sc_core::sc_module_name ,xsc::common_cpp::properties& ,xsc::common_cpp::report_handler *,std::vector<PF_BAR_st_type*> *bar_info_ptr);
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
	
    SC_HAS_PROCESS(sim_qdma_core);
	sc_core::sc_in_clk   m_axi_mm_aclk;
	sc_core::sc_in_clk   m_axictrl_aclk;
	sc_core::sc_in<bool> m_axictrl_aresetn;
	sc_core::sc_in<bool> m_aximm_aresetn;
	sc_core::sc_in_clk   m_aximm_aux_aclk;
	sc_core::sc_in<bool> m_aximm_aux_aresetn;
	sc_core::sc_in_clk   m_axis_aclk;
	sc_core::sc_in<bool> m_axis_aresetn;
	sc_core::sc_in_clk   s_axis_aclk;
	sc_core::sc_in<bool> s_axis_aresetn;
	sc_core::sc_in_clk   s_axi_mm_aclk;
	sc_core::sc_in<bool> s_aximm_aresetn;
    sc_core::sc_in<sc_bv<16> > usr_irq_req;
    sc_core::sc_out<sc_bv<16> > usr_irq_ack;
	// Callbacks to be invoked by underlying handler
	bool xclWrite(AddressSpace space, uint64_t offset, const void *hostBuf,size_t size,uint32_t PFDEV_ID=0, uint32_t BAR_ID=0);
	bool xclRead(AddressSpace space, uint64_t offset, void *hostBuf,size_t size,uint32_t PFDEV_ID=0, uint32_t BAR_ID=0);
	void ddrRead(unsigned int ddrNum, uint64_t offset, void* dest,unsigned int size);
    unsigned int xclCreateQueue(queue_context &q_ctx);
    bool     xclWriteQueue(uint64_t q_handle, const void *src, size_t size,unsigned int req_id,bool eot,bool isnonblocking );
    uint64_t xclReadQueue(uint64_t q_handle, void *dest, size_t size ,unsigned int req_id,bool eot,bool isnonblocking );
    bool xclPollCompletion(unsigned int req_id,std::vector<std::tuple<void*,uint32_t,bool> >&);
    virtual bool xclDestroyQueue(uint64_t q_handle);
    bool xclAllocDeviceBuffer(uint64_t base_address, uint64_t size,std::string& buffer_filename);
    bool xclFreeDeviceBuffer(uint64_t base_address);
   bool xclImportBO(std::string sFileName,uint64_t offset,uint64_t size) ;
   bool xclCopyBO(uint64_t src_handle,std::string dst_filename,uint64_t size, uint64_t src_offset,uint64_t dst_offset) ;
    void setEnableKernel2XrtCommunication(bool);
    void init_helper();
    std::vector<PF_BAR_size_type*> bar_info;

    void xclCloseQ2X();
	~sim_qdma_core();
private :

	xsc::common_cpp::report_handler*           m_report_handler;
	xtlm::xtlm_aximm_initiator_rd_socket_util* ini_ctrl_rd_socket_util;
	xtlm::xtlm_aximm_initiator_wr_socket_util* ini_ctrl_wr_socket_util;
	xtlm::xtlm_aximm_initiator_rd_socket_util* ini_mm_rd_socket_util;
	xtlm::xtlm_aximm_initiator_wr_socket_util* ini_mm_wr_socket_util;
	xtlm::xtlm_aximm_initiator_rd_socket_util* ini_mm_aux_rd_socket_util;
	xtlm::xtlm_aximm_initiator_wr_socket_util* ini_mm_aux_wr_socket_util;
	xtlm::xtlm_axis_initiator_socket_util*     ini_stm_stream_socket_util;
	xtlm::xtlm_axis_target_socket_util*        tar_stm_stream_socket_util;
	xtlm::xtlm_aximm_target_rd_socket_util* target_mm_rd_socket_util;
	xtlm::xtlm_aximm_target_wr_socket_util* target_mm_wr_socket_util;

	size_t methodProcessRdCommand     (qdma::qdma_intf intf, uint64_t offset, const void *hostBuf,size_t size);
	size_t methodProcessWrCommand     (qdma::qdma_intf intf, uint64_t offset, const void *hostBuf,size_t size);
	size_t methodProcessIniStreamQueue(qdma::qdma_intf intf,uint64_t offset, const void *hostBuf,size_t size);
	size_t methodProcessTargetStreamRequest(qdma::qdma_intf intf);
	void methodProcessWrQueue (uint64_t q_handle);
	void methodProcessRdQueue ();

	void end_of_elaboration();

	void read_properties(xsc::common_cpp::properties& properties);
	size_t send_dbg_transport_rd_call(qdma::qdma_intf intf,uint64_t offset, const void *hostBuf,size_t size);
	size_t send_dbg_transport_wr_call(qdma::qdma_intf intf,uint64_t offset, const void *hostBuf,size_t size);

	bool                                       m_is_aux_enable;
	bool                                       m_is_stm_enable;
	std::stringstream 						   m_ss;

	xtlm::xtlm_aximm_mem_manager               			   m_mem_manager;
	xtlm::xtlm_axis_mem_manager               			   m_ms_manager;
	std::map<qdma::qdma_intf, qdma::qdma_itf_prop*>        m_itf_prop_map;

	uint32_t                                       m_axi_wr_tbuf_no_serviced;
	uint32_t                                       m_axi_rd_tbuf_no_serviced;
	unsigned char*                                 m_xcl_read_req_data_ptr;
	unsigned char*                                 m_xcl_write_req_data_ptr;
	size_t                                         m_read_req_copied_size;
	size_t                                     	   m_write_req_copied_size;
	qdma::qdma_intf                            	   m_activeIntf;
	std::map<uint64_t,std::queue<std::pair<unsigned char,bool> >* > m_stream_host_rdq;
	std::map<uint64_t,std::deque<qdma::stream_nbl_req_type>* >m_stream_pend_rreq;
	std::map<uint64_t,std::deque<qdma::stream_nbl_req_type>* >m_stream_pend_wreq;
	std::map<uint64_t,queue_context >              m_stream_host_qctx;
	std::map<uint64_t,uint64_t >                   m_stream_flowid_to_qid;
	std::map<uint64_t,std::queue<unsigned char>* > m_stream_host_wdq;
	uint64_t                                       m_queue_counter;

    void target_mm_wr_req_method();
    void target_mm_rd_req_method();
    void target_mm_wr_res_method();
    void target_mm_rd_res_method();
	std::queue<xtlm::aximm_payload*> target_mm_req_queue,target_mm_wr_res_queue,target_mm_rd_res_queue;    
    void msg_to_host();
    void write_to_host(xtlm::aximm_payload*);
    void read_from_host(xtlm::aximm_payload*);
    sc_core::sc_event msg_to_host_ev;
    uint16_t old_irq_vector;
    void send_irq_tohost();
    bool EnableKernel2XrtCommunication;

    unix_socket *k2h_socket;
    bool irq2xrt(uint16_t);
    bool mmcall2xrt(bool rd_wr, uint64_t addr, void *data, uint32_t size);
    sc_core::sc_event send_wr_resp,send_rd_data;

    void fetching_BAR_info();
    uint64_t appending_BAR_value(uint64_t offset, uint32_t PFDEV_ID, uint32_t BAR_ID);
};

//Global Functions to support XSim GDB debug of Kernel
void registerDdrRead();
extern "C" void WEAKFUNC iki_register_DDR_read(int (*fp)(int, int));
int ddrRead(int ddrNum, int offset);
DPI_LINKER_DECL
extern "C" size_t get_sc_time_stamp();
DPI_LINKER_DECL
double get_sc_time_insec();
DPI_LINKER_DECL
void weak_sc_stop();

#endif /* SIM_QDMA_CORE_H_ */


