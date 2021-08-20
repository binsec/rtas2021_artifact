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
 
#ifndef __UNISIM_COMPONENT_TLM_PCI_BUS_BUS_HH__
#define __UNISIM_COMPONENT_TLM_PCI_BUS_BUS_HH__

#include <list>
#include <unisim/kernel/tlm/tlm.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/component/tlm/message/pci.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/service/interfaces/memory.hh>

namespace unisim {
namespace component {
namespace tlm {
namespace pci {
namespace bus {

using namespace std;
using unisim::kernel::tlm::TlmMessage;
using unisim::kernel::tlm::TlmSendIf;
using unisim::component::tlm::message::PCIRequest;
using unisim::component::tlm::message::PCIResponse;
using unisim::util::garbage_collector::Pointer;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::util::endian::LittleEndian2Host;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using unisim::component::cxx::pci::PCISpace;
using unisim::component::cxx::pci::TransactionType;


template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE,
  		unsigned int NUM_MASTERS,
  		unsigned int NUM_TARGETS,
  		unsigned int NUM_MAPPINGS,
  		bool DEBUG = false> 
class Bus:public sc_core::sc_module,
  		public TlmSendIf<PCIRequest<ADDRESS_TYPE, MAX_DATA_SIZE>,
  						PCIResponse<MAX_DATA_SIZE> >,
  		public Service<unisim::service::interfaces::Memory<ADDRESS_TYPE> >,
  		public Client<unisim::service::interfaces::Memory<ADDRESS_TYPE> > {
public:
	typedef PCIRequest < ADDRESS_TYPE, MAX_DATA_SIZE > ReqType;
	typedef PCIResponse < MAX_DATA_SIZE > RspType;

private:
	static const uint32_t BAR_IO_MASK = 0x3;
	static const uint32_t BAR_MEM_MASK = 0xF;
	static const uint32_t NUM_BARS = 6;
	static const uint32_t VALUE_READ = 1;
	static const uint32_t SIZE_REQUESTED = 2;
	static const uint32_t SIZE_READ = 3;
	static const uint32_t STABLE = 4;

	typedef Pointer < ReqType > PReqType;
	typedef Pointer < RspType > PRspType;
	typedef TlmMessage < ReqType, RspType > MsgType;
	typedef Pointer < MsgType > PMsgType;
	typedef TlmSendIf < ReqType, RspType > TlmSendIfType;

	//routing stuff
  	class DevReg {
  	public:
  	    uint8_t state;
    	int size;
    	uint32_t original_value;
    	ADDRESS_TYPE orig;
    	ADDRESS_TYPE end;
    	unisim::component::cxx::pci::PCISpace addr_type;
	};

	class DeviceRequest {
	public:
		DeviceRequest() : message(0), device(0) {}
	
		PMsgType message;
		unsigned int device;
	};
	
	list<DeviceRequest *> device_request_list;
	list<DeviceRequest *> device_request_free_list;
	sc_core::sc_event device_dispatch_event;

	unsigned int num_mappings;	
	DevReg *devmap[NUM_TARGETS][NUM_BARS];
	int portToDevNumberMap[NUM_TARGETS];
	int devNumberToPortMap[32];

	int decode (ADDRESS_TYPE addr);
	unsigned int findDeviceByAddRange (ADDRESS_TYPE addr, unisim::component::cxx::pci::PCISpace addr_type);
	void updateDevMap (PReqType &req, PRspType &rsp);

	// debug stuff
	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
	
	// routing stuff
	ADDRESS_TYPE base_address[NUM_MAPPINGS];
	ADDRESS_TYPE size[NUM_MAPPINGS];
	uint32_t device_number[NUM_MAPPINGS];
	uint32_t target_port[NUM_MAPPINGS];
	uint32_t register_number[NUM_MAPPINGS];
	PCISpace addr_type[NUM_MAPPINGS];

	Parameter<unsigned int> param_num_mappings;
	ParameterArray<ADDRESS_TYPE> param_base_address;
	ParameterArray<ADDRESS_TYPE> param_size;
	ParameterArray<uint32_t> param_device_number;
	ParameterArray<uint32_t> param_target_port;
	ParameterArray<uint32_t> param_register_number;
	ParameterArray<PCISpace> param_addr_type;
	
	// frequency variables
	unsigned int frequency;
	sc_core::sc_time cycle_time;
	Parameter<unsigned int> param_frequency;


public:

	sc_core::sc_export<TlmSendIfType> *input_port[NUM_MASTERS];
	sc_core::sc_port<TlmSendIfType> *output_port[NUM_TARGETS];

	ServiceExport<unisim::service::interfaces::Memory<ADDRESS_TYPE> > *memory_export[NUM_MASTERS];
	ServiceImport<unisim::service::interfaces::Memory<ADDRESS_TYPE> > *memory_import[NUM_TARGETS];

	SC_HAS_PROCESS(Bus);

	Bus(const sc_core::sc_module_name &name, Object *parent = 0):
		Object(name, parent, "PCI bus"),
		sc_core::sc_module(name),
		Service<unisim::service::interfaces::Memory<ADDRESS_TYPE> >(name, parent),
		Client<unisim::service::interfaces::Memory<ADDRESS_TYPE> >(name, parent),
		num_mappings(0),
		logger(*this),
		verbose(false),
		param_verbose("verbose", this, verbose, "enable/disable verbosity"),
		param_num_mappings("num-mappings", this, num_mappings, "total number of address mappings"),
		param_base_address("base-address", this, base_address, NUM_MAPPINGS, "mapping: base address of mapped device"),
		param_size("size", this, size, NUM_MAPPINGS, "mapping: size in bytes of mapped device"),
		param_device_number("device-number", this, device_number, NUM_MAPPINGS, "mapping: device number"),
		param_target_port("target-port", this, target_port, NUM_MAPPINGS, "mapping: target port number"),
		param_register_number("register-number", this, register_number, NUM_MAPPINGS, "mapping: BAR offset in PCI device configuration space"),
		param_addr_type("addr-type", this, addr_type, NUM_MAPPINGS, "mapping: address space type"),
		frequency(0),
		param_frequency("frequency", this, frequency, "frequency in Mhz")
{
		param_size.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
		param_target_port.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
		param_num_mappings.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
		param_frequency.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

		for(unsigned int i = 0; i < NUM_MASTERS; i++){
			stringstream s, r;
			s << "input_port[" << i << "]";
			input_port[i] =
				new sc_core::sc_export<TlmSendIfType>(s.str().c_str());
			(*input_port[i]) (*this);
			r << "memory_export[" << i << "]";
			memory_export[i] =
				new ServiceExport<unisim::service::interfaces::Memory<ADDRESS_TYPE> >(r.str().c_str(), this);
		}

		for (unsigned int i = 0; i < NUM_TARGETS; i++) {
			stringstream s, r;
			s << "output_port[" << i << "]";
			output_port[i] =
				new sc_core::sc_port<TlmSendIfType>(s.str().c_str());
			r << "memory_import[" << i << "]";
			memory_import[i] =
				new ServiceImport<unisim::service::interfaces::Memory<ADDRESS_TYPE> >(r.str().c_str(), this);
		}

		for(unsigned int i = 0; i < NUM_MASTERS; i++){
			for (unsigned int j = 0; j < NUM_TARGETS; j++) {
				memory_export[i]->SetupDependsOn(*memory_import[j]);
			}
		}

		for (unsigned int i = 0; i < NUM_TARGETS; i++) {
			for (unsigned int j = 0; j < NUM_BARS; j++) {
				devmap[i][j] = NULL;
			}
		}
		
		SC_THREAD(DeviceDispatch);
	}

	~Bus() {
		for(unsigned int i = 0; i < NUM_MASTERS; i++){
			if (input_port[i]) delete input_port[i];
	  		if (memory_export[i]) delete memory_export[i];
		}

		for (unsigned int i = 0; i < NUM_TARGETS; i++) {
			if (output_port[i]) delete output_port[i];
			if (memory_import[i]) delete memory_import[i];
		}

		for (unsigned int i = 0; i < num_mappings; i++) {
			int regNumber = (register_number[i] - 16)/ 4;
			DevReg *devReg = devmap[target_port[i]][regNumber];
			if(devReg) delete devReg;
		}
		
		while (!device_request_free_list.empty()) {
	  		DeviceRequest *device_request = device_request_free_list.front();
      		device_request_free_list.pop_front();
      		if (device_request) {
      			delete device_request;
      		}
		}

		while (!device_request_list.empty()) {
	  		DeviceRequest *device_request = device_request_list.front();
      		device_request_list.pop_front();
      		if (device_request) {
      			delete device_request;
      		}
		}
	}
	
	virtual bool BeginSetup()
	{
		Reset ();

//		for (unsigned int i = 0; i < NUM_MAPPINGS; i++) {
//			int regNumber = (register_number[i] - 16)/ 4;
//			DevReg *devReg = devmap[target_port[i]][regNumber];
//			if(!devReg) {
//				devReg = new DevReg();
//				devmap[target_port[i]][regNumber] = devReg;
//			}
//			devReg->orig = base_address[i];
//			devReg->size = size[i];
//			devReg->end = devReg->orig + devReg->size -1;
//			devReg->addr_type = (full_system::pci::PCISpace) addr_type[i];
//			
//			portToDevNumberMap[target_port[i]] = device_number[i];
//			portToDevNumberMap[device_number[i]] = target_port[i];
//			 
//		}
		
		for (unsigned int i = 0; i < num_mappings; i++) {
			int regNumber = (register_number[i] - 16)/ 4;
			DevReg *devReg = devmap[target_port[i]][regNumber];
			if(devReg == NULL) {
				devReg = new DevReg();
				devmap[target_port[i]][regNumber] = devReg;
			}
			devReg->orig = base_address[i];
			devReg->size = size[i];
			devReg->end = devReg->orig + devReg->size -1;
			devReg->addr_type = (unisim::component::cxx::pci::PCISpace) addr_type[i];

			if(unlikely(DEBUG && verbose)) {
				logger << DebugInfo
												 << "Mapping " << target_port[i] << " with "
												 << " orig = 0x" << std::hex << devReg->orig << std::dec
												 << ", end = 0x" << std::hex << devReg->end << std::dec
												 << ", size = " << devReg->size
												 << " and address type = ";
				switch(devReg->addr_type) {
				case unisim::component::cxx::pci::SP_MEM:
					logger << "SP_MEM";
					break;
				case unisim::component::cxx::pci::SP_IO:
					logger << "SP_IO";
					break;
				case unisim::component::cxx::pci::SP_CONFIG:
					logger << "SP_CONFIG";
					break;
				}
				logger << "(" << devReg->addr_type << ")" << std::endl << EndDebugInfo;
			}
			portToDevNumberMap[target_port[i]] = device_number[i];
			portToDevNumberMap[device_number[i]] = target_port[i];
			 
		}
		
		return true;
	}

	virtual bool SetupMemory()
	{
    	for(unsigned int i = 0; i < NUM_TARGETS; i++)
			if(!memory_import[i])
				return false;
			
		return true;
	}

	virtual bool Setup(ServiceExportBase *srv_export)
	{
		for(unsigned int i = 0; i < NUM_MASTERS; i++)
		{
			if(srv_export == memory_export[i]) return SetupMemory();
		}
		
		logger << DebugError << "Internal error" << EndDebugError;
		
		return false;
	}

	virtual bool EndSetup() {
			
		if(!frequency) {
			if(unlikely(verbose))
				logger << DebugError << "SERVICE_SETUP_ERROR("
					<< __FUNCTION__ << ":"
					<< __FILE__ << ":"
					<< __LINE__ << "): frequency parameter has not been set" 
					<< std::endl << EndDebugError;
			return false;
		}
		cycle_time = sc_core::sc_time(1.0 / (double) frequency, sc_core::SC_US);
		
		return true;
    }

	virtual bool Send (const PMsgType &message) {
		const PReqType& req = message->req;
		unsigned int device;
		bool ret = true;

		if(req->space == unisim::component::cxx::pci::SP_CONFIG) {
			device = decode (req->addr);
      	} else {
				device = findDeviceByAddRange (req->addr, req->space);
		}

		if(device >= NUM_TARGETS) {
			if(req->type == unisim::component::cxx::pci::TT_READ) {
				if(!message->HasResponseEvent()) {
					if(unlikely(verbose))
						logger << DebugError
							<< "Received a read request without response event ("
							<< __FUNCTION__ << ":"<< __FILE__ << ":" << __LINE__ << ")" << std::endl
							<< EndDebugError;
					Object::Stop(-1);
				}

	    		PRspType res = new(res) RspType();
				switch(req->size) {
				case sizeof(uint32_t): {
					uint32_t p = 0xFFFFFFFF;
					memcpy(res->read_data, &p, 4);
					message->SetResponse(res);
					message->GetResponseEvent()->notify(sc_core::SC_ZERO_TIME);
					if(unlikely(DEBUG && verbose))
						logger << DebugInfo
							<< "Answering 0xFFFFFFFF to addr: " 
							<< std::hex << req->addr << std::dec
							<< " (device = " << device << ")"
							<< std::endl << EndDebugInfo;
		  			return true;
		  			break;
					}
				case sizeof(uint16_t): {
					uint16_t p = 0xFFFF;
					memcpy(res->read_data, &p, 2);
					message->SetResponse(res);
					message->GetResponseEvent()->notify(sc_core::SC_ZERO_TIME);
					if(unlikely(DEBUG && verbose))
						logger << DebugInfo
							<< "Answering 0xFFFF to addr: " 
							<< std::hex << req->addr << std::dec
							<< " (device = " << device << ")"
							<< std::endl << EndDebugInfo;
		  			return true;
		  			break;
					}
				case sizeof (uint8_t): {
					uint8_t p = 0xFF;
					memcpy(res->read_data, &p, 1);
					message->SetResponse(res);
					message->GetResponseEvent()->notify(sc_core::SC_ZERO_TIME);
					if(unlikely(DEBUG && verbose))
						logger << DebugInfo
							<< "Answering 0xFF to addr: " 
							<< std::hex << req->addr << std::dec
							<< " (device = " << device << ")"
							<< std::endl << EndDebugInfo;
		  			return true;
		  			break;
					}
				default: {
					if(unlikely(verbose))
						logger << DebugError
							<< "Invalid access size(?) for PCI configspace! ("
							<< __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ")" << std::endl
							<< EndDebugError;
					Object::Stop(-1);
					return true;
					}
	      		}
	  		}
      	} else {
			if(unlikely(DEBUG && verbose))
				logger << DebugInfo
					<< "putting received message to device " << device
					<< " in the message list to be sent" << std::endl
					<< EndDebugInfo;

      		DeviceRequest *device_request;
      		if(device_request_free_list.empty()) {
      			device_request = new DeviceRequest();
      			device_request_free_list.push_back(device_request);
      		}
      		device_request = device_request_free_list.front();
      		device_request_free_list.pop_front();
      		device_request->message = message;
      		device_request->device = device;

			if(device_request_list.empty()) {
				sc_core::sc_time delay;
				sc_dt::uint64 ui_delay = sc_core::sc_time_stamp().value();
				ui_delay = ui_delay % cycle_time.value();
				delay = sc_core::sc_time(ui_delay, false);
	   			device_dispatch_event.notify(delay);
				//std::cerr << "Send: delay= " << delay << ")" << endl;
			}
	   		
	   		device_request_list.push_back(device_request);
			//std::cerr << "Send: queue size= " << device_request_list.size() << ")" << endl;
   			
   			return true;		
	   	}
	   	
	   	return ret;
  	}
  	
  	void DeviceDispatch() {
  		DeviceRequest *device_request;
  		while(1) {
  			wait(device_dispatch_event);
  			if(device_request_list.empty()) continue;
  			device_request = device_request_list.front();
  			
  			PMsgType& message = device_request->message;
			sc_core::sc_event bus_event;
			unsigned int device = device_request->device;
			if(unlikely(DEBUG && verbose))
				logger << DebugInfo
					<< "sending message to device: " << device << std::endl
					<< EndDebugInfo;
   			if(message->HasResponseEvent())
				message->PushResponseEvent(bus_event);
			
      		while(!(*output_port[device])->Send(message)) {
      			if(unlikely(DEBUG && verbose))
      				logger << DebugInfo
      					<< "message not accepted by device: " << device
      					<< ", retrying later" << std::endl
      					<< EndDebugInfo;
      			wait(cycle_time);
      			if(unlikely(DEBUG && verbose))
      				logger << DebugInfo
      					<< "retrying to send message to device: " << device << std::endl
      					<< EndDebugInfo;
      		}
      		
      		if(unlikely(DEBUG && verbose))
      			logger << DebugInfo
      				<< "message to device " << device
      				<< " succesfully sent" << std::endl
      				<< EndDebugInfo;
 			sc_core::sc_time delay;
      		if(message->HasResponseEvent()) {
      			
      			if(unlikely(DEBUG && verbose))
      				logger << DebugInfo
      					<< "waiting response from device: " << device << std::endl
      					<< EndDebugInfo;
      			wait(bus_event);
      			if(unlikely(DEBUG && verbose))
      				logger << DebugInfo
      					<< " received response from device: " << device << std::endl
      					<< EndDebugInfo;
    	  		//TODO: Pau I THINK WE SHOULd be adding another event to the event stack and wait on that event!
    	  		
	 			if(!message->rsp) {
	 				if(unlikely(verbose))
	 					logger << DebugError
	 						<< "received a response for a pci message without response field" << std::endl
	 						<< EndDebugError;
					Object::Stop(-1);
		 		}
		 		PReqType& req = message->req;
	 			PRspType& rsp = message->rsp;
	 			updateDevMap(req, rsp);
	 			
	 			message->PopResponseEvent();
	 			if(unlikely(DEBUG && verbose))
	 				logger << DebugInfo
	 					<< "notifying response received from device: " << device << std::endl
	 					<< EndDebugInfo;
				sc_dt::uint64 ui_delay = sc_core::sc_time_stamp().value();
				ui_delay = ui_delay % cycle_time.value();
				delay = sc_core::sc_time(ui_delay, false);
	 			message->GetResponseEvent()->notify(delay);
	 			delay += cycle_time;
      		} else {
      			delay = cycle_time;
      		}
      		device_request_list.pop_front();
			//std::cerr << "Disp: before device_request->message = 0 (queue size= " << device_request_list.size() << ")" << endl;
			device_request->message = 0;
      		device_request_free_list.push_back(device_request);
      		if(!device_request_list.empty())
			{
				//std::cerr << "Disp: delay=" << delay << endl;
      			device_dispatch_event.notify(delay);
			}
  		}
  	}

	virtual void Reset() {
		for (unsigned int i = 0; i < NUM_TARGETS; i++) 
			for (unsigned int j = 0; j < NUM_BARS; j++) 
				devmap[i][j] = NULL;
	}

	virtual void ResetMemory() {
		this->Reset();

		for(unsigned int i = 0; i < NUM_TARGETS; i++)
			if(*memory_import[i]) (*memory_import[i])->ResetMemory();
	}

	virtual bool ReadMemory(ADDRESS_TYPE addr, void *buffer, uint32_t size)
	{
		unsigned int device = findDeviceByAddRange(addr, unisim::component::cxx::pci::SP_MEM);
		if(device >= NUM_TARGETS) return false;
		if(!(*memory_import[device])) return false;

		return (*memory_import[device])->ReadMemory(addr, buffer, size);
	}

	virtual bool WriteMemory(ADDRESS_TYPE addr, const void *buffer, uint32_t size)
	{
		unsigned int device = findDeviceByAddRange(addr, unisim::component::cxx::pci::SP_MEM);
		if(device >= NUM_TARGETS) return false;
		if(!(*memory_import[device])) return false;

		return (*memory_import[device])->WriteMemory(addr, buffer, size);
	}
};



//private functions
template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE,
	unsigned int NUM_MASTERS, unsigned int NUM_TARGETS, unsigned int NUM_MAPPINGS, bool DEBUG> 
int Bus<ADDRESS_TYPE, MAX_DATA_SIZE, NUM_MASTERS, NUM_TARGETS, NUM_MAPPINGS, DEBUG>
::decode (ADDRESS_TYPE daddr) {
	return (daddr >> 11) & 0x1F;
}

template<class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE,
	unsigned int NUM_MASTERS, unsigned int NUM_TARGETS, unsigned int NUM_MAPPINGS, bool DEBUG> 
unsigned int Bus<ADDRESS_TYPE, MAX_DATA_SIZE, NUM_MASTERS, NUM_TARGETS, NUM_MAPPINGS, DEBUG>
::findDeviceByAddRange (ADDRESS_TYPE paddr, unisim::component::cxx::pci::PCISpace addr_type) {
	unsigned int device = 0xff;
	for (unsigned int i = 0; i < NUM_TARGETS; i++) {
		for (unsigned int j = 0; j < NUM_BARS; j++) {
			if (devmap[i][j] != NULL) {
				if(paddr >= devmap[i][j]->orig) {
					if(paddr <= devmap[i][j]->end) {
						if(addr_type == devmap[i][j]->addr_type) {
							device = i;
							break;
						}
					}
				}
			}
		}
	}
	return device;
}

template < class ADDRESS_TYPE, uint32_t MAX_DATA_SIZE,
  unsigned int NUM_MASTERS,
  unsigned int NUM_TARGETS, unsigned int NUM_MAPPINGS, bool DEBUG> 
void Bus < ADDRESS_TYPE, MAX_DATA_SIZE, NUM_MASTERS, NUM_TARGETS, NUM_MAPPINGS, DEBUG>::
updateDevMap (PReqType &req, PRspType &res) {
	ADDRESS_TYPE offset = req->addr & 0xFF;

//#define PCI0_BASE_ADDR0               0x10    // Base ADDRESS_TYPEess 0               rw
//#define PCI0_BASE_ADDR1               0x14    // Base ADDRESS_TYPEess 1               rw
//#define PCI0_BASE_ADDR2               0x18    // Base ADDRESS_TYPEess 2               rw
//#define PCI0_BASE_ADDR3               0x1C    // Base ADDRESS_TYPEess 3               rw
//#define PCI0_BASE_ADDR4               0x20    // Base ADDRESS_TYPEess 4               rw
//#define PCI0_BASE_ADDR5               0x24    // Base ADDRESS_TYPEess 5               rw

	if(0x10 <= offset && offset <= 0x24
		&& req->size == sizeof (uint32_t)) {
		uint8_t device = (req->addr >> 11) & 0x1F;
		uint8_t barNum = (offset - 16) / 4;
		DevReg *reg;
		
		reg = devmap[device][barNum];

		if(req->type == unisim::component::cxx::pci::TT_WRITE && 
			req->space == unisim::component::cxx::pci::SP_CONFIG) {

			uint32_t write_data;
			memcpy(&write_data, req->write_data, sizeof(uint32_t));
			write_data = LittleEndian2Host(write_data);
			if (write_data == 0xffffffff) {
				//requesting bar size
				if(unlikely(DEBUG && verbose))
					logger << DebugInfo
						<< "requesting bar size dev " << device
						<< " reg " << barNum << std::endl
						<< EndDebugInfo; 
				reg->state = SIZE_REQUESTED;
			} else if (write_data != 0x00000000) {
				//writing bar address
				uint32_t bar_mask;
				//We check the bit that indicates if it's for io or mem
				if (reg->original_value & 0x1)
					bar_mask = BAR_IO_MASK;
				else
					bar_mask = BAR_MEM_MASK;

				ADDRESS_TYPE newValue =
						(write_data & ~bar_mask) |
						(reg->original_value & bar_mask);

				if (newValue & ~bar_mask) {
					if(unlikely(DEBUG && verbose))
						logger << DebugInfo
							<< "writing bar address dev " << device
							<< " reg " << barNum << " data: "
							<< std::hex << write_data << std::dec << std::endl
							<< EndDebugInfo;

					if (reg && reg->size > 1) {
						reg->orig = newValue;
						reg->end = newValue + req->size - 1;
						reg->state = STABLE;
					}
				}
			}
		} else if(req->type ==unisim::component::cxx::pci::TT_READ && 
			req->space == unisim::component::cxx::pci::SP_CONFIG) {
			if(reg != NULL && reg->state == SIZE_REQUESTED) {
#if 0
				//reading size
				uint32_t bar_mask;
				//We check the bit that indicates if it's for io or mem
				if (reg->original_value & 0x1) 
					bar_mask = BAR_IO_MASK;
				else
					bar_mask = BAR_MEM_MASK;
#endif
				if(unlikely(DEBUG && verbose))
					logger << DebugInfo
						<< "reading size dev " << device << " reg "
						<< barNum << std::endl
						<< EndDebugInfo;
				uint32_t read_data;
				memcpy(&read_data, res->read_data, sizeof(uint32_t));
				read_data = LittleEndian2Host(read_data);
				reg->size = (~read_data) + 1;
				reg->state = SIZE_READ;
			} else {
				if(reg == NULL) {
					reg = new DevReg ();
					reg->state = STABLE;
					devmap[device][barNum] = reg;
				}
				if(reg->state == STABLE) {
					//first step, reading original value of register
					if(unlikely(DEBUG && verbose))
						logger << DebugInfo
							<< "reading original value dev " << device
							<< " reg " << barNum << std::endl
							<< EndDebugInfo;
					//reg->original_value = *(uint32_t *) res->read_data;
					uint32_t original_value;
					memcpy(&original_value, res->read_data, sizeof(uint32_t));
					reg->original_value = LittleEndian2Host(original_value);
					
					reg->state = VALUE_READ;
				}
			}
		}
	}
}

}				// end of namespace bus 
}				// end of namespace pci
}				// end of namespace tlm
}				// end of namespace component
}				// end of namespace unisim

#endif
