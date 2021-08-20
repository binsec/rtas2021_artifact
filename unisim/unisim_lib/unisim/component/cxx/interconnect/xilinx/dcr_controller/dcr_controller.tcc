/*
 *  Copyright (c) 2010-2011,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_TCC__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_TCC__

#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace dcr_controller {

using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebugInfo;

template <class CONFIG>
DCRController<CONFIG>::DCRController(const char *name, Object *parent)
	: Object(name, parent, "A Device Control Register bus controller")
	, logger(*this)
	, indirect_mode_address_register(0)
	, dcr_controller_status_and_control_register(DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_RESET_VALUE)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
}

template <class CONFIG>
DCRController<CONFIG>::~DCRController()
{
}

template <class CONFIG>
bool DCRController<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
bool DCRController<CONFIG>::IsMappedDCR(uint32_t dcrn) const
{
	return (dcrn >= DCR_CONTROLLER_BASEADDR) && (dcrn < (DCR_CONTROLLER_BASEADDR + CONFIG::DCR_CONTROLLER_LOCATIONS));
}

template <class CONFIG>
int DCRController<CONFIG>::Route(uint32_t dcrn)
{
	if(dcrn < DCR_CONTROLLER_BASEADDR) return CONFIG::EXTERNAL_SLAVE_NUM;
	if(dcrn >= (DCR_CONTROLLER_BASEADDR + CONFIG::DCR_CONTROLLER_LOCATIONS)) return CONFIG::EXTERNAL_SLAVE_NUM;
	
	uint32_t offset = dcrn - DCR_CONTROLLER_BASEADDR;
	if((offset >= CONFIG::APU_LOW) && (offset <= CONFIG::APU_HIGH)) return GetSlaveInterface(CONFIG::APU_SLAVE_NUM);
	if((offset >= CONFIG::MCI_LOW) && (offset <= CONFIG::MCI_HIGH)) return GetSlaveInterface(CONFIG::MCI_SLAVE_NUM);
	if((offset >= CONFIG::CROSSBAR_LOW) && (offset <= CONFIG::CROSSBAR_HIGH)) return GetSlaveInterface(CONFIG::CROSSBAR_SLAVE_NUM);
	if((offset >= CONFIG::PLBS0_LOW) && (offset <= CONFIG::PLBS0_HIGH)) return GetSlaveInterface(CONFIG::PLBS0_SLAVE_NUM);
	if((offset >= CONFIG::PLBS1_LOW) && (offset <= CONFIG::PLBS1_HIGH)) return GetSlaveInterface(CONFIG::PLBS1_SLAVE_NUM);
	if((offset >= CONFIG::PLBM_LOW) && (offset <= CONFIG::PLBM_HIGH)) return GetSlaveInterface(CONFIG::PLBM_SLAVE_NUM);
	if((offset >= CONFIG::DMAC0_LOW) && (offset <= CONFIG::DMAC0_HIGH)) return GetSlaveInterface(CONFIG::DMAC0_SLAVE_NUM);
	if((offset >= CONFIG::DMAC1_LOW) && (offset <= CONFIG::DMAC1_HIGH)) return GetSlaveInterface(CONFIG::DMAC1_SLAVE_NUM);
	if((offset >= CONFIG::DMAC2_LOW) && (offset <= CONFIG::DMAC2_HIGH)) return GetSlaveInterface(CONFIG::DMAC2_SLAVE_NUM);
	if((offset >= CONFIG::DMAC3_LOW) && (offset <= CONFIG::DMAC3_HIGH)) return GetSlaveInterface(CONFIG::DMAC3_SLAVE_NUM);

	return -1;
}

template <class CONFIG>
int DCRController<CONFIG>::GetMasterInterface(unsigned int num_master) const
{
	return num_master;
}

template <class CONFIG>
int DCRController<CONFIG>::GetSlaveInterface(unsigned int num_slave) const
{
	return CONFIG::NUM_MASTERS + num_slave;
}

template <class CONFIG>
bool DCRController<CONFIG>::IsMasterInterface(int intf) const
{
	return intf < (int) CONFIG::NUM_MASTERS;
}

template <class CONFIG>
bool DCRController<CONFIG>::IsSlaveInterface(int intf) const
{
	return intf >= (int) CONFIG::NUM_MASTERS;
}

template <class CONFIG>
unsigned int DCRController<CONFIG>::GetSlave(int intf) const
{
	return intf - CONFIG::NUM_MASTERS;
}

template <class CONFIG>
unsigned int DCRController<CONFIG>::GetMaster(int intf) const
{
	return intf;
}

template <class CONFIG>
void DCRController<CONFIG>::Read(uint32_t dcrn, uint32_t& value)
{
	uint32_t offset = dcrn - DCR_CONTROLLER_BASEADDR;
	
	switch(offset)
	{
		case CONFIG::INDIRECT_MODE_ADDRESS_REGISTER:
			value = GetIndirectModeAddressRegister();
			break;
		case CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER:
			value = GetDCRControllerStatusAndControlRegister();
			break;
		default:
			logger << DebugWarning << "DCR register 0x" << std::hex << dcrn << std::dec << " is reserved or unmapped" << EndDebugWarning;
			value = 0;
	}
	
	if(IsVerbose())
	{
		logger << DebugInfo << "Read 0x" << std::hex << value << std::dec << " from DCR #0x" << std::hex << dcrn << std::dec << EndDebugInfo;
	}
}

template <class CONFIG>
void DCRController<CONFIG>::Write(unsigned int num_master, uint32_t dcrn, uint32_t value)
{
	if(IsVerbose())
	{
		logger << DebugInfo;
		switch(num_master)
		{
			case CONFIG::C440_MASTER_NUM:
				logger << "C440";
				break;
			case CONFIG::XM_MASTER_NUM:
				logger << "XM";
				break;
			default:
				logger << "?";
		}
		logger << " master writing 0x" << std::hex << value << std::dec << " into DCR #0x" << std::hex << dcrn << std::dec << EndDebugInfo;
	}

	uint32_t offset = dcrn - DCR_CONTROLLER_BASEADDR;
	
	switch(offset)
	{
		case CONFIG::INDIRECT_MODE_ADDRESS_REGISTER:
			SetIndirectModeAddressRegister(num_master, value);
			break;
		case CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER:
			SetDCRControllerStatusAndControlRegister(num_master, value);
			break;
		default:
			logger << DebugWarning << "DCR register 0x" << std::hex << dcrn << std::dec << " is reserved or unmapped" << EndDebugWarning;
	}
}

template <class CONFIG>
void DCRController<CONFIG>::SetIndirectModeAddressRegister(unsigned int num_master, uint32_t value)
{
	indirect_mode_address_register = value & CONFIG::INDIRECT_MODE_ADDRESS_REGISTER_MASK;
	
	if(dcr_controller_status_and_control_register & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK)
	{
		uint32_t old_dcr_controller_status_and_control_register = dcr_controller_status_and_control_register;
		switch(num_master)
		{
			case CONFIG::C440_MASTER_NUM:
				// autolock for C440 master
				dcr_controller_status_and_control_register = dcr_controller_status_and_control_register | CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK;
				break;
			case CONFIG::XM_MASTER_NUM:
				// autolock for XM master
				dcr_controller_status_and_control_register = dcr_controller_status_and_control_register | CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK;
				break;
		}
		if(IsVerbose())
		{
			LogDCRControllerStatusAndControlRegister(old_dcr_controller_status_and_control_register, dcr_controller_status_and_control_register);
		}
	}
}

template <class CONFIG>
void DCRController<CONFIG>::SetDCRControllerStatusAndControlRegister(unsigned int num_master, uint32_t value)
{
	uint32_t old_dcr_controller_status_and_control_register = dcr_controller_status_and_control_register;
	
	switch(num_master)
	{
		case CONFIG::C440_MASTER_NUM:
			dcr_controller_status_and_control_register = 
				(dcr_controller_status_and_control_register & ~CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_WRITE_MASK) |
				(value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_WRITE_MASK);
			if(value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK)
			{
				dcr_controller_status_and_control_register = dcr_controller_status_and_control_register & ~CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK;
			}
			break;
		case CONFIG::XM_MASTER_NUM:
			dcr_controller_status_and_control_register = 
				(dcr_controller_status_and_control_register & ~CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_WRITE_MASK) |
				(value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_WRITE_MASK);
			if(value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK)
			{
				dcr_controller_status_and_control_register = dcr_controller_status_and_control_register & ~CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK;
			}
			break;
	}
	
	if(IsVerbose())
	{
		LogDCRControllerStatusAndControlRegister(old_dcr_controller_status_and_control_register, dcr_controller_status_and_control_register);
	}
}

template <class CONFIG>
void DCRController<CONFIG>::SetDCRControllerStatusAndControlRegister_C440_TIMEOUT()
{
	uint32_t old_dcr_controller_status_and_control_register = dcr_controller_status_and_control_register;
	dcr_controller_status_and_control_register = dcr_controller_status_and_control_register | CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK;
	if(IsVerbose())
	{
		LogDCRControllerStatusAndControlRegister(old_dcr_controller_status_and_control_register, dcr_controller_status_and_control_register);
	}
}

template <class CONFIG>
void DCRController<CONFIG>::SetDCRControllerStatusAndControlRegister_XM_TIMEOUT()
{
	uint32_t old_dcr_controller_status_and_control_register = dcr_controller_status_and_control_register;
	dcr_controller_status_and_control_register = dcr_controller_status_and_control_register | CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK;
	if(IsVerbose())
	{
		LogDCRControllerStatusAndControlRegister(old_dcr_controller_status_and_control_register, dcr_controller_status_and_control_register);
	}
}

template <class CONFIG>
void DCRController<CONFIG>::ResetDCRControllerStatusAndControlRegister_C440_ALOCK()
{
	uint32_t old_dcr_controller_status_and_control_register = dcr_controller_status_and_control_register;
	dcr_controller_status_and_control_register = dcr_controller_status_and_control_register & ~CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK;
	if(IsVerbose())
	{
		LogDCRControllerStatusAndControlRegister(old_dcr_controller_status_and_control_register, dcr_controller_status_and_control_register);
	}
}

template <class CONFIG>
void DCRController<CONFIG>::ResetDCRControllerStatusAndControlRegister_XM_ALOCK()
{
	uint32_t old_dcr_controller_status_and_control_register = dcr_controller_status_and_control_register;
	dcr_controller_status_and_control_register = dcr_controller_status_and_control_register & ~CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK;
	if(IsVerbose())
	{
		LogDCRControllerStatusAndControlRegister(old_dcr_controller_status_and_control_register, dcr_controller_status_and_control_register);
	}
}

template <class CONFIG>
void DCRController<CONFIG>::ResetALOCK(unsigned int num_master)
{
	switch(num_master)
	{
		case CONFIG::C440_MASTER_NUM:
			// release autolock for C440 master
			ResetDCRControllerStatusAndControlRegister_C440_ALOCK();
			break;
		case CONFIG::XM_MASTER_NUM:
			// release autolock for XM master
			ResetDCRControllerStatusAndControlRegister_XM_ALOCK();
			break;
	}
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetIndirectModeAddressRegister() const
{
	return indirect_mode_address_register & CONFIG::INDIRECT_MODE_ADDRESS_REGISTER_MASK;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister() const
{
	return dcr_controller_status_and_control_register & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_MASK;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_C440_LOCK() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_OFFSET;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_C440_ALOCK() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_OFFSET;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_XM_LOCK() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_OFFSET;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_XM_ALOCK() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_OFFSET;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_AUTOLOCK() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_OFFSET;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_XM_ASYN() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_OFFSET;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_XS_ASYN() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_OFFSET;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_XM_TOWAIT() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_OFFSET;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_C440_TIMEOUT() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_OFFSET;
}

template <class CONFIG>
uint32_t DCRController<CONFIG>::GetDCRControllerStatusAndControlRegister_XM_TIMEOUT() const
{
	return (GetDCRControllerStatusAndControlRegister() & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK) >> CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_OFFSET;
}

template <class CONFIG>
void DCRController<CONFIG>::LogDCRControllerStatusAndControlRegister(uint32_t old_value, uint32_t new_value)
{
	if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_MASK) != (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_MASK))
	{
		logger << DebugInfo << "DCR Controller Status And Control Register: C440_LOCK C440_ALOCK XM_LOCK XM_ALOCK AUTOLOCK XM_ASYN XS_ASYN XM_TOWAIT C440_TIMEOUT XM_TIMEOUT" << EndDebugInfo;
		logger << DebugInfo << "                                             "
							<< "    " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_MASK) ? '1' : '0')
							<< "         " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK) ? '1' : '0')
							<< "        " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK) ? '1' : '0')
							<< "        " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK) ? '1' : '0')
							<< "        " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK) ? '1' : '0')
							<< "       " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_MASK) ? '1' : '0')
							<< "       " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_MASK) ? '1' : '0')
							<< "        " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK) ? '1' : '0')
							<< "           " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK) ? '1' : '0')
							<< "           " << ((new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK) ? '1' : '0')
			<< EndDebugInfo;
		
		logger << EndDebugInfo;
		logger << DebugInfo << "DCR Controller Status And Control Register recent changes:" << std::endl;
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_MASK))
		{
			logger << "  - processor is locking DCR bus" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_LOCK_MASK))
		{
			logger << "  - processor is unlocking DCR bus" << std::endl;
		}
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK))
		{
			logger << "  - processor is auto-locking DCR bus" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_ALOCK_MASK))
		{
			logger << "  - processor is auto-unlocking DCR bus" << std::endl;
		}
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK))
		{
			logger << "  - external master is locking DCR bus" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_LOCK_MASK))
		{
			logger << "  - external master is unlocking DCR bus" << std::endl;
		}
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK))
		{
			logger << "  - external master is auto-locking DCR bus" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ALOCK_MASK))
		{
			logger << "  - external master is auto-unlocking DCR bus" << std::endl;
		}
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK))
		{
			logger << "  - enabling DCR bus auto-locking" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK))
		{
			logger << "  - disabling DCR bus auto-locking" << std::endl;
		}
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_MASK))
		{
			logger << "  - external DCR master interface enters asynchronous mode" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_ASYN_MASK))
		{
			logger << "  - external DCR master interface enters synchronous mode" << std::endl;
		}
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_MASK))
		{
			logger << "  - external DCR slave interface enters asynchronous mode" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XS_ASYN_MASK))
		{
			logger << "  - external DCR slave interface enters synchronous mode" << std::endl;
		}
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK))
		{
			logger << "  - enabling external DCR master time-out wait support" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TOWAIT_MASK))
		{
			logger << "  - disabling external DCR master time-out wait support" << std::endl;
		}
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK))
		{
			logger << "  - processor DCR master access time-out occurs" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_C440_TIMEOUT_MASK))
		{
			logger << "  - processor DCR master access time-out status bit reseted" << std::endl;
		}
		if(!(old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK) && (new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK))
		{
			logger << "  - external DCR master access time-out occurs" << std::endl;
		}
		if((old_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK) && !(new_value & CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_XM_TIMEOUT_MASK))
		{
			logger << "  - external DCR master access time-out occurs status bit reseted" << std::endl;
		}
		logger << EndDebugInfo;
	}
}

} // end of namespace dcr_controller
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_TCC__
