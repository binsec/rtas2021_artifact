/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <assert.h>

#include <unisim/component/tlm2/processor/hcs12x/s12xmmc.hh>
#include <unisim/component/tlm2/processor/hcs12x/tlm_types.hh>
#include <unisim/util/converter/convert.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace processor {
namespace hcs12x {


S12XMMC::S12XMMC(const sc_module_name& name, S12MPU_IF *_mpu, Object *parent) :
	Object(name, parent)
	, sc_module(name)
	, MMC(name, _mpu, parent)
	, unisim::kernel::Client<TrapReporting>(name, parent)

	, trap_reporting_import("trap_reporting_import", this)
	, init_socket("init-socket")

	, busSemaphore()
	, busSemaphore_event()

	, memoryMapStr("")
	, param_memoryMapStr("memory-map", this, memoryMapStr)

{

	tlm2_btrans_time = sc_time((double)0, SC_PS);

	mmc_trans = payloadFabric.allocate();

}


S12XMMC::~S12XMMC() {

	mmc_trans->release();

	for (unsigned int i=0; i <memoryMap.size(); i++) {
		if (memoryMap[i]) { delete memoryMap[i]; memoryMap[i] = NULL; }
	}

}


bool S12XMMC::accessBus(physical_address_t addr, MMC_DATA *buffer, tlm::tlm_command cmd) {

	bool find = false;

	for (unsigned int i=0; i <memoryMap.size(); i++) {
		if ((addr >= memoryMap[i]->start_address) && (addr <= memoryMap[i]->end_address)) {
			find = true;

//			tlm::tlm_generic_payload* mmc_trans = payloadFabric.allocate();

			mmc_trans->acquire();

			mmc_trans->set_data_ptr( (unsigned char *)buffer->buffer );

			mmc_trans->set_data_length( buffer->data_size );
			mmc_trans->set_streaming_width( buffer->data_size );

			mmc_trans->set_byte_enable_ptr( 0 );
			mmc_trans->set_dmi_allowed( false );
			mmc_trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

			if (cmd == tlm::TLM_READ_COMMAND) {
				mmc_trans->set_command( tlm::TLM_READ_COMMAND );

//				memset(buffer->buffer, 0, buffer->data_size);

			} else {
				mmc_trans->set_command( tlm::TLM_WRITE_COMMAND );
			}

			mmc_trans->set_address( addr & 0x7FFFFF);

			init_socket[memoryMap[i]->module_index]->b_transport( *mmc_trans, tlm2_btrans_time );

//			tlm::tlm_response_status response = mmc_trans->get_response_status();

			if (mmc_trans->is_response_error()) { return (false); }

			mmc_trans->release();

			break;
		}
	}


	return (find);

}


void S12XMMC::xgate_access(MMC::ACCESS accessType, MMC_DATA *buffer) {

	address_t logicalAddress = buffer->logicalAddress;
	tlm::tlm_command cmd = ((accessType == MMC::WRITE)? tlm::TLM_WRITE_COMMAND: tlm::TLM_READ_COMMAND);

	bool find = false;
	if (inherited::version.compare("V3") == 0) {
		for (unsigned int i=0; (i<inherited::MMC_MEMMAP_SIZE) && !find; i++) {
			find = (inherited::MMC_REGS_ADDRESSES[i] == logicalAddress);
		}
	} else if (inherited::version.compare("V4") == 0) {
		for (unsigned int i=0; (i<=inherited::PPAGE) && !find; i++) {
			find = (inherited::MMC_REGS_ADDRESSES[i] == logicalAddress);
		}
	}

	if (find) {
		if (inherited::mpu) {
			inherited::mpu->assertInterrupt();
		}
	} else {
		physical_address_t addr = inherited::getXGATEPhysicalAddress((address_t) logicalAddress);

		bool result = true;
		if (inherited::mpu) {
			result = inherited::mpu->validate(TOWNER::XGATE, addr, buffer->data_size, (cmd == tlm::TLM_WRITE_COMMAND), buffer->isExecute);
		}

		if (result) {
			while (!busSemaphore.lock(TOWNER::XGATE)) {
				wait(busSemaphore_event);
			}

			find = accessBus(addr, buffer, cmd);

			if (busSemaphore.unlock(TOWNER::XGATE)) {
				busSemaphore_event.notify();
			}

			if (!find) {

				if (debug_enabled) {
					std::cerr << "WARNING: S12XMMC::XGATE => Device at 0x" << std::hex << logicalAddress << " Not present in the emulated platform." << std::dec << std::endl;
				}
				if (cmd == tlm::TLM_WRITE_COMMAND) {
					if (debug_enabled) {
						std::cerr << "Unable to WRITE size= " << buffer->data_size << "  value= 0x" << std::hex << *((sc_dt::uint64 *) buffer->buffer) << std::endl;
					}
				} else {
					memset(buffer->buffer, 0, buffer->data_size);
				}

				if (trap_reporting_import && debug_enabled) {
					trap_reporting_import->ReportTrap(*this);
				}
			}
		}

	}

}

void S12XMMC::cpu_access(MMC::ACCESS accessType, MMC_DATA *buffer) {

	address_t logicalAddress = buffer->logicalAddress;
	tlm::tlm_command cmd = ((accessType == MMC::WRITE)? tlm::TLM_WRITE_COMMAND: tlm::TLM_READ_COMMAND);

	bool find = false;
	if (inherited::version.compare("V3") == 0) {
		for (unsigned int i=0; (i<inherited::MMC_MEMMAP_SIZE) && !find; i++) {
			find = (inherited::MMC_REGS_ADDRESSES[i] == logicalAddress);
		}
	} else if (inherited::version.compare("V4") == 0) {
		for (int i=0; (i<=inherited::PPAGE) && !find; i++) {
			find = (inherited::MMC_REGS_ADDRESSES[i] == logicalAddress);
		}
	}

	if (find) {
		if (cmd == tlm::TLM_READ_COMMAND) {
			memset(buffer->buffer, 0, buffer->data_size);
			inherited::read(logicalAddress, buffer->buffer, buffer->data_size);
		} else {
			inherited::write(logicalAddress, buffer->buffer, buffer->data_size);
		}

	} else {

		physical_address_t addr = inherited::getCPU12XPhysicalAddress((address_t) logicalAddress, buffer->type, buffer->isGlobal);

		bool result = true;
		if (inherited::mpu) {
			result = inherited::mpu->validate(TOWNER::CPU12X, addr, buffer->data_size, (cmd == tlm::TLM_WRITE_COMMAND), buffer->isExecute);
		}

		if (result) {
			while (!busSemaphore.lock(TOWNER::CPU12X)) {
				wait(busSemaphore_event);
			}

			find = accessBus(addr, buffer, cmd);

			if (busSemaphore.unlock(TOWNER::CPU12X)) {
				busSemaphore_event.notify();
			}

			if (!find) {

//				std::cerr << "WARNING: S12XMMC::CPU12X => Device at 0x" << std::hex << logicalAddress << " Not present in the emulated platform." << std::dec << std::endl;
				if (cmd == tlm::TLM_WRITE_COMMAND) {
//					std::cerr << "Unable to WRITE size= " << buffer->data_size << "  value= 0x" << std::hex << *((sc_dt::uint64 *) buffer->buffer) << std::endl;
				} else {
					memset(buffer->buffer, 0, buffer->data_size);
				}

//				if (trap_reporting_import) {
//					trap_reporting_import->ReportTrap(*this);
//				}
			}
		}

	}


}

bool S12XMMC::BeginSetup() {

	vector<string> result;
	stringSplit(memoryMapStr, ";", result);
	vector<string> oneMemoryMapEntrySegments;
	for (unsigned int i = 0; i < result.size(); i++) {
		stringSplit(result[i], ",", oneMemoryMapEntrySegments);
		MemoryMapEntry *oneMemoryMapEntry = new MemoryMapEntry();
		std::stringstream ss;
		ss << std::dec << oneMemoryMapEntrySegments[0] << " " << std::hex << oneMemoryMapEntrySegments[1] << " " << std::hex << oneMemoryMapEntrySegments[2] << std::dec;
		ss >> std::dec >> oneMemoryMapEntry->module_index >> std::hex >> oneMemoryMapEntry->start_address >> std::hex >> oneMemoryMapEntry->end_address;

		ss.str(std::string());

		memoryMap.push_back(oneMemoryMapEntry);
		oneMemoryMapEntrySegments.clear();
	}
	result.clear();

	return (inherited::BeginSetup());
}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


