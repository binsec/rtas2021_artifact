/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_MPC107_HH__
#define __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_MPC107_HH__

/*
 * ---------> systemc connections
 * =========> service connections
 *
 *
 *                  /-----------\
 * [port            |           |   [non intrusive connections 
 *  connections]    |           |    provided by the module]
 *                  |           | 
 *   from           |           |    from  
 *  system-bus      |           |    cpu
 * (bus_port)       |     M     | (memory_export)
 * ---------------->|           |<=================
 *                  |     P     |
 *  to ram          |           |    to ram
 *  (ram_port)      |     C     |  (ram_export)
 * <----------------|           |=================>
 *                  |     1     |
 *  to rom          |           |    to rom
 *  (rom_port)      |     0     |  (rom_export)
 * <----------------|           |=================>
 *                  |     7     |
 *                  |           | 
 *  to pci bus      |           |
 *  (pci_port)      |     M     |
 * <----------------|     O     |
 *                  |     D     |
 *                  |     U     |
 *                  |     L     |
 *                  |     E     |
 *                  |           |
 *                  |           |
 *                  |           |
 *                  |           |
 *                  \-----------/
 */
 
#include <systemc>
#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/util/garbage_collector/garbage_collector.hh"
#include "unisim/component/tlm/message/snooping_fsb.hh"
#include "unisim/component/tlm/message/memory.hh"
#include "unisim/component/tlm/message/pci.hh"
#include "unisim/component/tlm/message/interrupt.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/cxx/chipset/mpc107/address_maps.hh"
#include "unisim/component/cxx/chipset/mpc107/address_map_entry.hh"
#include "unisim/component/cxx/chipset/mpc107/config_regs.hh"
#include "unisim/component/cxx/chipset/mpc107/pci_controller.hh"
#include "unisim/component/cxx/chipset/mpc107/dma/dma.hh"
#include "unisim/component/cxx/chipset/mpc107/dma/dma_client_interface.hh"
#include "unisim/component/cxx/chipset/mpc107/atu/atu.hh"
#include "unisim/component/tlm/chipset/mpc107/epic/epic.hh"
#include <list>

namespace unisim {
namespace component {
namespace tlm {
namespace chipset {
namespace mpc107 {
	
using std::list;
using unisim::kernel::tlm::TlmSendIf;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::ResponseListener;
using unisim::kernel::tlm::AdvancedResponseListener;
using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceImport;
using unisim::util::garbage_collector::Pointer;
using unisim::component::tlm::message::SnoopingFSBRequest;
using unisim::component::tlm::message::SnoopingFSBResponse;
using unisim::component::tlm::message::MemoryRequest;
using unisim::component::tlm::message::MemoryResponse;
using unisim::component::tlm::message::PCIRequest;
using unisim::component::tlm::message::PCIResponse;
using unisim::component::tlm::message::InterruptRequest;
using unisim::service::interfaces::Memory;
using unisim::component::cxx::chipset::mpc107::AddressMap;
using unisim::component::cxx::chipset::mpc107::AddressMapEntry;
using unisim::component::cxx::chipset::mpc107::PCIController;
using unisim::component::cxx::chipset::mpc107::ConfigurationRegister;
using unisim::component::cxx::chipset::mpc107::ConfigurationRegisters;
using unisim::component::cxx::chipset::mpc107::atu::ATU;
using unisim::component::cxx::chipset::mpc107::dma::DMA;
using unisim::component::cxx::chipset::mpc107::dma::DMAClientInterface;
using unisim::component::tlm::chipset::mpc107::epic::EPIC;
		
template <class PHYSICAL_ADDR, 
		uint32_t MAX_TRANSACTION_DATA_SIZE,
		class PCI_ADDR,
		uint32_t MAX_PCI_TRANSACTION_DATA_SIZE,
		bool DEBUG = false>
class MPC107 :
	public sc_core::sc_module,
	public TlmSendIf<SnoopingFSBRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>, 
					 SnoopingFSBResponse<MAX_TRANSACTION_DATA_SIZE> >,
	public TlmSendIf<PCIRequest<PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE>,
					PCIResponse<MAX_PCI_TRANSACTION_DATA_SIZE> >,
	public AdvancedResponseListener<MemoryRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>,
							MemoryResponse<MAX_TRANSACTION_DATA_SIZE>,
							PCIRequest<PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE>,
							PCIResponse<MAX_PCI_TRANSACTION_DATA_SIZE> >,
	public AdvancedResponseListener<MemoryRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>,
							MemoryResponse<MAX_TRANSACTION_DATA_SIZE>,
							SnoopingFSBRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE>,
							SnoopingFSBResponse<MAX_TRANSACTION_DATA_SIZE> >,
	public DMAClientInterface<PHYSICAL_ADDR>,
	public Service<Memory<PHYSICAL_ADDR> >,
	public Client<Memory<PHYSICAL_ADDR> > {
private:
	typedef SnoopingFSBRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE> ReqType;
	typedef Pointer<ReqType> PReqType;
	typedef SnoopingFSBResponse<MAX_TRANSACTION_DATA_SIZE> RspType;
	typedef Pointer<RspType> PRspType;
	typedef TlmMessage<ReqType, RspType> MsgType;
	typedef Pointer<MsgType> PMsgType;
	typedef MemoryRequest<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE> MemReqType;
	typedef Pointer<MemReqType> PMemReqType;
	typedef MemoryResponse<MAX_TRANSACTION_DATA_SIZE> MemRspType;
	typedef Pointer<MemRspType> PMemRspType;
	typedef TlmMessage<MemReqType, MemRspType> MemMsgType;
	typedef Pointer<MemMsgType> PMemMsgType;
	typedef PCIRequest<PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE> PCIReqType;
	typedef Pointer<PCIReqType> PPCIReqType;
	typedef PCIResponse<MAX_PCI_TRANSACTION_DATA_SIZE> PCIRspType;
	typedef Pointer<PCIRspType> PPCIRspType;
	typedef TlmMessage<PCIReqType, PCIRspType> PCIMsgType;
	typedef Pointer<PCIMsgType> PPCIMsgType;

	typedef InterruptRequest IRQType;
	typedef Pointer<IRQType> PIRQType;

	template <typename ADDRESS>
	struct MemoryClient : unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >
	{
		MemoryClient(const char *name, unisim::kernel::Object *parent) : unisim::kernel::Object(name, parent), unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >(name, parent) {}
	};
	
	MemoryClient<PCI_ADDR> pci_memory_client;
public:
	SC_HAS_PROCESS(MPC107);

	/* Module ports declaration */
	/** Input port for incomming requests from the system bus */
	sc_core::sc_export<TlmSendIf<ReqType, RspType> > slave_port;
	/** 
	 * Output port to send transactions to the system bus 
	 * (needed to inform of pci requests to the memory) */
	sc_core::sc_port<TlmSendIf<ReqType, RspType> > master_port;
	/** Input port to listen for pci requests */ 
	sc_core::sc_export<TlmSendIf<PCIReqType, PCIRspType> > pci_slave_port;
	/** Output port to send memory requests to the ram */
	sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > ram_master_port;
	/** Output port to send memory requests to the rom */
	sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > rom_master_port;
	/** Output port to send memory requests to the extended ram */
	sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > erom_master_port;
	/** Output port to send pci requests to devices (probably through a bus) */
	sc_core::sc_port<TlmSendIf<PCIReqType, PCIRspType> > pci_master_port;
	/** Input port for the incomming interruptions
	 * (to be directly attached to the EPIC) */
	sc_core::sc_export<TlmSendIf<IRQType> > *irq_slave_port[unisim::component::tlm::chipset::mpc107::epic::EPIC<PHYSICAL_ADDR, DEBUG>::NUM_IRQS];
	/** output port for the outgoing interruptions 
	 * (to be directly attached to the EPIC) */
	sc_core::sc_port<TlmSendIf<IRQType> > irq_master_port;
	/** output port for the outgoing soft reset interruption
	 * (to be directly attached to the EPIC) */
	sc_core::sc_port<TlmSendIf<IRQType> > soft_reset_master_port;
		
	/* Service/client exports/imports declaration */
	ServiceExport<Memory<PHYSICAL_ADDR> > memory_export;
	ServiceImport<Memory<PHYSICAL_ADDR> > ram_import;
	ServiceImport<Memory<PHYSICAL_ADDR> > rom_import;
	ServiceImport<Memory<PHYSICAL_ADDR> > erom_import;
	ServiceImport<Memory<PCI_ADDR> > pci_import;
		
	MPC107(const sc_core::sc_module_name &name, Object *parent = 0);
	virtual ~MPC107();
	
	/* Initialization methods of the service */
	virtual void Reset();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
		
	/* Methods to implement for Service<MemoryInterface<PHYSICAL_ADDR> > */
	virtual void ResetMemory();
	virtual bool ReadMemory(PHYSICAL_ADDR addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(PHYSICAL_ADDR addr, const void *buffer, uint32_t size);

	/* Interface of the MPC107 to the local bus */
	virtual bool Send(const PMsgType &message);
	/* Interface of the MPC107 to the PCI bus (MPC107 as slave device) */
	virtual bool Send(const PPCIMsgType &message);
	
	/**
	 * Method required by ResponseListener for memory requests comming from the PCI bus.
	 * Called each time a request using the ResponseListener Send method receives a response. In this
	 *   case is for requests comming through the PCI bus that must be send to the memory system. 
	 * 
	 * @param msg the message to send
	 * @param port the port which should be used to send the message
	 * @param orig_pci_msg the original pci message
	 * @param who_pci_master_port the port which received the original message
	 */
	void ResponseReceived(const PMemMsgType &msg, 
						sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > &port,
						const PPCIMsgType &orig_pci_msg,
						sc_core::sc_export<TlmSendIf<PCIReqType, PCIRspType> > &who_pci_master_port);

	/**
	 * Method required by ResponseListener of memory requests comming from the FSB bus.
	 * Called each time a request using the ResponseListener Send method receives a response. In this
	 *   case is for requests comming through the FSB bus that must be send to the memory system. 
	 * 
	 * @param msg the message to send
	 * @param port the port which should be used to send the message
	 * @param orig_fsb_msg the original pci message
	 * @param who_fsb_master_port the port which received the original message
	 */
	void ResponseReceived(const PMemMsgType &msg, 
						sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > &port,
						const PMsgType &orig_fsb_msg,
						sc_core::sc_export<TlmSendIf<ReqType, RspType> > &who_fsb_master_port);

	/* Methods required by the DMA controller to handle PCI and local memory space accesses */
	virtual void DMARead(PHYSICAL_ADDR addr,
		unsigned int size,
		unsigned int channel);
	virtual void DMAWrite(PHYSICAL_ADDR addr,
		Pointer<uint8_t> &data,
		unsigned int size,
		unsigned int channel);
	virtual void DMAPCIRead(PHYSICAL_ADDR addr,
		unsigned int size,
		unsigned int channel);
	virtual void DMAPCIWrite(PHYSICAL_ADDR addr,
		Pointer<uint8_t> &data,
		unsigned int size,
		unsigned int channel);

protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
private:
	/** DMA channels */
	DMA<PHYSICAL_ADDR, DEBUG> dma;
	class DMAMsgType {
	public:
		PMsgType msg;
		unsigned int channel;
	};
	class DMAPCIMsgType {
	public:
		PPCIMsgType msg;
		unsigned int channel;
	};
	typedef Pointer<DMAMsgType> PDMAMsgType;
	typedef Pointer<DMAPCIMsgType> PDMAPCIMsgType;
	/* list of DMA local memory requests */
	list<PDMAMsgType> dma_req_list;
	/* list of DMA PCI requests */
	list<PDMAPCIMsgType> dma_pci_req_list;
	/* Threads to handle DMA requests */
	void DispatchDMALocalMemoryAccess();
	void DispatchDMAPCIAccess();
	/* Events to handle DMA threads */
	sc_core::sc_event dispatchDMALocalMemoryAccessEvent;
	sc_core::sc_event dispatchDMAPCIAccessEvent;
	
	/**
	 * ATU controller */
	ATU<PHYSICAL_ADDR, PCI_ADDR, DEBUG> atu;
	
	/**
	 * EPIC Interrupt controller */
	EPIC<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE, DEBUG> epic;
	/** 
	 * Output port to send transactions to the EPIC 
	 * (this is a private, not visible port ) */
	sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > epic_master_port;
	/**
	 * Memory Import to connect to the epic to get access to 
	 * the memory service */
	ServiceImport<Memory<PHYSICAL_ADDR> > epic_memory_import;
	/**
	 * Output port to notify the sdram clock (the sdram cycle time) 
	 *   to internal components, i.e., EPIC */
	sc_core::sc_out<uint64_t> sdram_master_port;
	/**
	 * Signal to connect the sdram_outport to the EPIC sdram_inport
	 */
	sc_core::sc_signal<uint64_t> sdram_to_epic_sig;
	
	/* the configuration registers */
	ConfigurationRegisters config_regs;

	/* the pci controller class that handles request to the pci bus */
	PCIController<PHYSICAL_ADDR, MAX_TRANSACTION_DATA_SIZE,
				PCI_ADDR, MAX_PCI_TRANSACTION_DATA_SIZE, DEBUG> pci_controller;
	/* list of pci requests generated by the system bus */
	list<PPCIMsgType> bus_pci_req_list;
	/* list of the requests queued for the pci that need a response */
	list<PMsgType> req_list;
	/* event to control the dispatch of pci requests */
	sc_core::sc_event pci_dispatch_event;
	/* process to dispatch pending pci requests */
	void PCIDispatch();
	/* process to dispatch pending local memory requests */
	void LocalMemoryDispatch();

	/* CONFIG_ADDR register */
	uint32_t config_addr;

	static const PHYSICAL_ADDR local_memory_orig = 0x0; // local memory base address
	AddressMap<PHYSICAL_ADDR, PCI_ADDR, DEBUG> addr_map;
	/* Reset values */
	/** Indicates if the mpc107 is running in host mode (true) or agent mode (false) */
	bool host_mode;
	/** Indicates if the address map to use: true = A address map, false = B address map */
	bool a_address_map;
	/** 
	 * Determines the bit bus size of rom chip 2 and 3, and DRAM: true = 32bits, false = 64bits.
	 * Default: true (32 bits) */
	bool memory_32bit_data_bus_size;
	/** Determines if the rom chip 0 has a data bus size of 8bits or the same than the DRAM */
	bool rom0_8bit_data_bus_size;
	/** Determines if the rom chip 1 has a data bus size of 8bits or the same than the DRAM */
	bool rom1_8bit_data_bus_size;
	
	unsigned int frequency;
	uint64_t sdram_cycle_time;
	sc_core::sc_time cycle_time;
	
	/* Parameters for reset values */
	Parameter<bool> param_host_mode;
	Parameter<bool> param_a_address_map;
	Parameter<bool> param_memory_32bit_data_bus_size;
	Parameter<bool> param_rom0_8bit_data_bus_size;
	Parameter<bool> param_rom1_8bit_data_bus_size;
	Parameter<unsigned int> param_frequency;
	Parameter<uint64_t> param_sdram_cycle_time;
	
	/**
	 * Translates a pci message to a fsb message.
	 * 
	 * @param pci_msg the pci message to translate.
	 * 
	 * @return the translated fsb message.
	 */ 
	PMsgType ConverttoFSBMsg(const PPCIMsgType &pci_msg, int size_done, int size);
	/**
	 * Translates a pci message to a memory message.
	 * 
	 * @param pci_msg the pci message to translate.
	 * 
	 * @return the translated memory message.
	 */ 
	PMemMsgType ConverttoMemMsg(const PPCIMsgType &pci_msg, int size_done, int size);
	/**
	 * Translates a fsb message to a memory message.
	 * 
	 * @param pci_msg the fsb message to translate.
	 * 
	 * @return the translated memory message.
	 */ 
	PMemMsgType ConverttoMemMsg(const PMsgType &fsb_msg);
	
	/** 
 	 * This method simply queues messages coming from PCI to be send to the memory system. 
	 *   It notifies the dispatcher if necessary.
 	 * 
	 * @param pci_msg   the pci message to send
	 * @param out_port  the memory port that should be used
	 * 
	 * @return true on succes, false otherwise
	 */
	bool SendPCItoMemory(const PPCIMsgType &pci_msg, sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > &out_port);

	/** 
 	 * This method simply queues messages coming from FSB to be send to the memory system. 
	 *   It notifies the dispatcher if necessary.
 	 * 
	 * @param fsb_msg   the pci message to send
	 * @param out_port  the memory port that should be used
	 * 
	 * @return true on success, false otherwise
	 */
	bool SendFSBtoMemory(const PMsgType &fsb_msg, sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > &out_port);

	/**
	 * This method transform a request received in the FSB to a ATU request and updates
	 *   the address mapping if necessary. Returns true if suceeds.
	 * 
	 * @param fsb_msg   the message received from the FSB
	 * 
	 * @return true on success, false otherwise
	 */
	bool SendFSBtoATU(const PMsgType &fsb_msg);
	
	/**
	 * This method performs a read memory on the ATU registers. Returns true if succeeds.
	 * 
	 * @param addr   the address of the requested register
	 * @param buffer the buffer with the register data
	 * @param size   the size of the request
	 * 
	 * @return true on success, false otherwise
	 */
	bool ReadMemorytoATU(PHYSICAL_ADDR addr, void *buffer, uint32_t size);
	
	/**
	 * This method performs a write memory on the ATU registers. Returns true if succeeds.
	 * 
	 * @param addr   the address of the targeted register
	 * @param buffer the buffer containing the data to write into the register
	 * @param size   the size of the request
	 * 
	 * @return true on success, false otherwise
	 */
	bool WriteMemorytoATU(PHYSICAL_ADDR addr, const void *buffer, uint32_t size);
	
	class PCItoMemoryReqType {
	public:
		PPCIMsgType pci_msg;
		sc_core::sc_port<TlmSendIf<MemReqType, MemRspType> > *out_port;
	};
	/** list of pci requests to send to the memory system */
	list<PCItoMemoryReqType *> pci_req_list;
	/** list of free pci requests to send to the memory system */
	list<PCItoMemoryReqType *> free_pci_req_list;
	/**
	 * Thread to handle incomming pci requests that need to be send to the
	 * memory system. The PowerPC architecture requires that this requests is
	 * snooped by all the processors, so it will be sent to all the processors
	 * and memory system (if necessary).
	 */
	void DispatchPCIReq();
	/** even to handle the DispatchPCIReq thread */
	sc_core::sc_event dispatch_pci_req_ev;

	/**
	 * Function to show debug information from a bus request.
	 * 
	 * @param req  the pci message to visualize
	 */
	void DEBUG_BUS_REQ(const PReqType &req);
	/**
	 * Function to show debug information from a pci request.
	 * 
	 * @param req  the pci message to visualize
	 */
	void DEBUG_PCI_REQ(const PPCIReqType &req);
	
	/**
	 * Setup Memory export.
	 * @return true on success.
	 */
	bool SetupMemory();
};

} // end of namespace unisim
} // end of namespace component
} // end of namespace tlm
} // end of namespace chipset
} // end of namespace mpc107

#endif // __UNISIM_COMPONENT_TLM_CHIPSET_MPC107_MPC107_HH__
