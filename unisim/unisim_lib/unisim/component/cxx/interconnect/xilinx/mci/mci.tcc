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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_MCI_MCI_TCC__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_MCI_MCI_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace mci {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
MCI<CONFIG>::MCI(const char *name, Object *parent)
	: Object(name, parent, "A Memory Controller Interface (MCI)")
	, Service<Memory<typename CONFIG::ADDRESS> >(name, parent)
	, Client<Memory<typename CONFIG::ADDRESS> >(name, parent)
	, memory_export("memory-export", this)
	, memory_import("memory-import", this)
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	Reset();
}

template <class CONFIG>
MCI<CONFIG>::~MCI()
{
}

template <class CONFIG>
void MCI<CONFIG>::Reset()
{
	mi_rowconflict_mask = 0;
	mi_bankconflict_mask = 0;
	mi_control = CONFIG::MI_CONTROL_RESET_VALUE;
}

template <class CONFIG>
void MCI<CONFIG>::ResetMemory()
{
	Reset();
}

template <class CONFIG>
bool MCI<CONFIG>::ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size)
{
	return memory_import->ReadMemory(addr, buffer, size);
}

template <class CONFIG>
bool MCI<CONFIG>::WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size)
{
	return memory_import->WriteMemory(addr, buffer, size);
}

template <class CONFIG>
uint32_t MCI<CONFIG>::ReadDCR(unsigned int dcrn)
{
	uint32_t value;

	switch(dcrn)
	{
		case CONFIG::MI_ROWCONFLICT_MASK: value = GetMI_ROWCONFLICT_MASK(); break;
		case CONFIG::MI_BANKCONFLICT_MASK: value = GetMI_BANKCONFLICT_MASK(); break;
		case CONFIG::MI_CONTROL: value = GetMI_CONTROL(); break;
		default:
			logger << DebugWarning << "Reading an unmapped DCR #0x" << std::hex << dcrn << std::dec << EndDebugWarning;
			return 0;
	}
	
	if(IsVerbose())
	{
		logger << DebugInfo << "Read 0x" << std::hex << value << std::dec << " from DCR #0x" << std::hex << dcrn << std::dec << EndDebugInfo;
	}
	return value;
}

template <class CONFIG>
void MCI<CONFIG>::WriteDCR(unsigned int dcrn, uint32_t value)
{
	if(IsVerbose())
	{
		logger << DebugInfo << "Writing 0x" << std::hex << value << std::dec << " into DCR #0x" << std::hex << dcrn << std::dec << EndDebugInfo;
	}
	switch(dcrn)
	{
		case CONFIG::MI_ROWCONFLICT_MASK: SetMI_ROWCONFLICT_MASK(value); break;
		case CONFIG::MI_BANKCONFLICT_MASK: SetMI_BANKCONFLICT_MASK(value); break;
		case CONFIG::MI_CONTROL: SetMI_CONTROL(value); break;
		default:
			logger << DebugWarning << "Writing an unmapped or read only DCR #0x" << std::hex << dcrn << std::dec << EndDebugWarning;
	}
}

template <class CONFIG>
bool MCI<CONFIG>::IsMappedDCR(uint32_t dcrn) const
{
	return (dcrn >= CONFIG::DCR_LOW) && (dcrn <= CONFIG::DCR_HIGH);
}

template <class CONFIG>
const char *MCI<CONFIG>::GetInterfaceName(typename MCI<CONFIG>::Interface intf) const
{
	switch(intf)
	{
		case IF_MASTER_MCI: return "IF_MASTER_MCI";
		case IF_SLAVE_MCI: return "IF_SLAVE_MCI";
		case IF_DCR: return "IF_DCR";
	}
	return "?";
}

template <class CONFIG>
bool MCI<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
uint32_t MCI<CONFIG>::GetMI_ROWCONFLICT_MASK() const
{
	return mi_rowconflict_mask;
}

template <class CONFIG>
uint32_t MCI<CONFIG>::GetMI_BANKCONFLICT_MASK() const
{
	return mi_bankconflict_mask;
}

template <class CONFIG>
uint32_t MCI<CONFIG>::GetMI_CONTROL() const
{
	return mi_control;
}

template <class CONFIG>
uint32_t MCI<CONFIG>::GetMI_CONTROL_ENABLE() const
{
	return (GetMI_CONTROL() & CONFIG::MI_CONTROL_ENABLE_MASK) >> CONFIG::MI_CONTROL_ENABLE_OFFSET;
}

template <class CONFIG>
void MCI<CONFIG>::SetMI_ROWCONFLICT_MASK(uint32_t value)
{
	mi_rowconflict_mask = value;
}

template <class CONFIG>
void MCI<CONFIG>::SetMI_BANKCONFLICT_MASK(uint32_t value)
{
	mi_bankconflict_mask = value;
}

template <class CONFIG>
void MCI<CONFIG>::SetMI_CONTROL(uint32_t value)
{
	uint32_t old_mi_control = mi_control;
	uint32_t new_mi_control = (old_mi_control & ~CONFIG::MI_CONTROL_WRITE_MASK) | (value & CONFIG::MI_CONTROL_WRITE_MASK);
	
	if(IsVerbose())
	{
		LogMI_CONTROL(old_mi_control, new_mi_control);
	}
	
	mi_control = new_mi_control;
}

template <class CONFIG>
void MCI<CONFIG>::LogMI_CONTROL(uint32_t old_value, uint32_t new_value)
{
	logger << DebugInfo << "MI_CONTROL: ENABLE ROWCONFLICTHOLDENABLE BANKCONFLICTHOLDENABLE DIRECTIONCONFLICTHOLDENABLE AUTOHOLDDURATION 2_3_CLOCK_RATIO_MODE OVERLAPRDWR BURSTWIDTH BURSTLENGTH WDD RMW PLB_PRIORITY_ENABLE PIPELINED_READ_ENABLE PIPELINED_WRITE_ENABLE" << EndDebugInfo;
	logger << DebugInfo;
	logger << "              " << ((new_value & CONFIG::MI_CONTROL_ENABLE_MASK) >> CONFIG::MI_CONTROL_ENABLE_OFFSET);
	logger << "                " << ((new_value & CONFIG::MI_CONTROL_ROWCONFLICTHOLDENABLE_MASK) >> CONFIG::MI_CONTROL_ROWCONFLICTHOLDENABLE_OFFSET);
	logger << "                    " << ((new_value & CONFIG::MI_CONTROL_BANKCONFLICTHOLDENABLE_MASK) >> CONFIG::MI_CONTROL_BANKCONFLICTHOLDENABLE_OFFSET);
	logger << "                         " << ((new_value & CONFIG::MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_MASK) >> CONFIG::MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_OFFSET);
	logger << "                     " << ((new_value & CONFIG::MI_CONTROL_AUTOHOLDDURATION_MASK) >> CONFIG::MI_CONTROL_AUTOHOLDDURATION_OFFSET);
	logger << "                  " << ((new_value & CONFIG::MI_CONTROL_2_3_CLOCK_RATIO_MODE_MASK) >> CONFIG::MI_CONTROL_2_3_CLOCK_RATIO_MODE_OFFSET);
	logger << "               " << ((new_value & CONFIG::MI_CONTROL_OVERLAPRDWR_MASK) >> CONFIG::MI_CONTROL_OVERLAPRDWR_OFFSET);
	logger << "          " << ((new_value & CONFIG::MI_CONTROL_BURSTWIDTH_MASK) >> CONFIG::MI_CONTROL_BURSTWIDTH_OFFSET);
	logger << "           " << ((new_value & CONFIG::MI_CONTROL_BURSTLENGTH_MASK) >> CONFIG::MI_CONTROL_BURSTLENGTH_OFFSET);
	logger << "       " << ((new_value & CONFIG::MI_CONTROL_WDD_MASK) >> CONFIG::MI_CONTROL_WDD_OFFSET);
	logger << "   " << ((new_value & CONFIG::MI_CONTROL_RMW_MASK) >> CONFIG::MI_CONTROL_RMW_OFFSET);
	logger << "             " << ((new_value & CONFIG::MI_CONTROL_PLB_PRIORITY_ENABLE_MASK) >> CONFIG::MI_CONTROL_PLB_PRIORITY_ENABLE_OFFSET);
	logger << "                    " << ((new_value & CONFIG::MI_CONTROL_PIPELINED_READ_ENABLE_MASK) >> CONFIG::MI_CONTROL_PIPELINED_READ_ENABLE_OFFSET);
	logger << "                    " << ((new_value & CONFIG::MI_CONTROL_PIPELINED_WRITE_ENABLE_MASK) >> CONFIG::MI_CONTROL_PIPELINED_WRITE_ENABLE_OFFSET);
	logger << EndDebugInfo;
	logger << DebugInfo << "MI_CONTROL register recent changes:" << std::endl;
	if(!(old_value & CONFIG::MI_CONTROL_ENABLE_MASK) && (new_value & CONFIG::MI_CONTROL_ENABLE_MASK))
	{
		logger << " - Enabling MCI" << std::endl;
	}
	if((old_value & CONFIG::MI_CONTROL_ENABLE_MASK) && !(new_value & CONFIG::MI_CONTROL_ENABLE_MASK))
	{
		logger << " - Disabling MCI" << std::endl;
	}
	if(!(old_value & CONFIG::MI_CONTROL_ROWCONFLICTHOLDENABLE_MASK) && (new_value & CONFIG::MI_CONTROL_ROWCONFLICTHOLDENABLE_MASK))
	{
		logger << " - Enabling row conflict hold" << std::endl;
	}
	if((old_value & CONFIG::MI_CONTROL_ROWCONFLICTHOLDENABLE_MASK) && !(new_value & CONFIG::MI_CONTROL_ROWCONFLICTHOLDENABLE_MASK))
	{
		logger << " - Disabling row conflict hold" << std::endl;
	}
	if(!(old_value & CONFIG::MI_CONTROL_BANKCONFLICTHOLDENABLE_MASK) && (new_value & CONFIG::MI_CONTROL_BANKCONFLICTHOLDENABLE_MASK))
	{
		logger << " - Enabling bank conflict hold" << std::endl;
	}
	if((old_value & CONFIG::MI_CONTROL_BANKCONFLICTHOLDENABLE_MASK) && !(new_value & CONFIG::MI_CONTROL_BANKCONFLICTHOLDENABLE_MASK))
	{
		logger << " - Disabling bank conflict hold" << std::endl;
	}
	if(!(old_value & CONFIG::MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_MASK) && (new_value & CONFIG::MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_MASK))
	{
		logger << " - Enabling direction conflict hold" << std::endl;
	}
	if((old_value & CONFIG::MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_MASK) && !(new_value & CONFIG::MI_CONTROL_DIRECTIONCONFLICTHOLDENABLE_MASK))
	{
		logger << " - Disabling direction conflict hold" << std::endl;
	}
	if((old_value ^ new_value) & CONFIG::MI_CONTROL_AUTOHOLDDURATION_MASK)
	{
		logger << " - Changing auto hold duration to " << (2 + ((new_value & CONFIG::MI_CONTROL_ENABLE_MASK) >> CONFIG::MI_CONTROL_AUTOHOLDDURATION_OFFSET)) << " cycles" << std::endl;
	}
	if((old_value ^ new_value) & CONFIG::MI_CONTROL_2_3_CLOCK_RATIO_MODE_MASK)
	{
		logger << " - Changing 2:3 clock ratio mode to " << (((new_value & CONFIG::MI_CONTROL_ENABLE_MASK) >> CONFIG::MI_CONTROL_2_3_CLOCK_RATIO_MODE_OFFSET) ? "fractional": "integer") << " ratio of MCI clock to the embedded processor block interconnect clock" << std::endl;
	}
	if(!(old_value & CONFIG::MI_CONTROL_OVERLAPRDWR_MASK) && (new_value & CONFIG::MI_CONTROL_OVERLAPRDWR_MASK))
	{
		logger << " - Enabling read/write overlaping" << std::endl;
	}
	if((old_value & CONFIG::MI_CONTROL_OVERLAPRDWR_MASK) && !(new_value & CONFIG::MI_CONTROL_OVERLAPRDWR_MASK))
	{
		logger << " - Disabling read/write overlaping" << std::endl;
	}
	if((old_value ^ new_value) & CONFIG::MI_CONTROL_BURSTWIDTH_MASK)
	{
		logger << " - Changing burst width to ";
		switch((new_value & CONFIG::MI_CONTROL_BURSTWIDTH_MASK) >> CONFIG::MI_CONTROL_BURSTWIDTH_OFFSET)
		{
			case 0: logger << " 128";
			case 1: logger << " 64";
			case 2: logger << " ?";
			case 3: logger << " 32";
		}
		logger << "-bit data per clock" << std::endl;
	}
	if((old_value ^ new_value) & CONFIG::MI_CONTROL_BURSTLENGTH_MASK)
	{
		logger << " - Changing burst length to " << (1 << ((new_value & CONFIG::MI_CONTROL_BURSTLENGTH_MASK) >> CONFIG::MI_CONTROL_BURSTLENGTH_OFFSET)) << std::endl;
	}
	if((old_value ^ new_value) & CONFIG::MI_CONTROL_WDD_MASK)
	{
		logger << " - Changing write data delay to " << ((new_value & CONFIG::MI_CONTROL_WDD_MASK) >> CONFIG::MI_CONTROL_WDD_OFFSET) << " cycles" << std::endl;
	}
	if(!(old_value & CONFIG::MI_CONTROL_RMW_MASK) && (new_value & CONFIG::MI_CONTROL_RMW_MASK))
	{
		logger << " - Enabling RNW" << std::endl;
	}
	if((old_value & CONFIG::MI_CONTROL_RMW_MASK) && !(new_value & CONFIG::MI_CONTROL_RMW_MASK))
	{
		logger << " - Disabling RNW" << std::endl;
	}
	if(!(old_value & CONFIG::MI_CONTROL_PLB_PRIORITY_ENABLE_MASK) && (new_value & CONFIG::MI_CONTROL_PLB_PRIORITY_ENABLE_MASK))
	{
		logger << " - Enabling PLB Priority" << std::endl;
	}
	if((old_value & CONFIG::MI_CONTROL_PLB_PRIORITY_ENABLE_MASK) && !(new_value & CONFIG::MI_CONTROL_PLB_PRIORITY_ENABLE_MASK))
	{
		logger << " - Disabling PLB Priority" << std::endl;
	}
	if(!(old_value & CONFIG::MI_CONTROL_PIPELINED_READ_ENABLE_MASK) && (new_value & CONFIG::MI_CONTROL_PIPELINED_READ_ENABLE_MASK))
	{
		logger << " - Enabling pipelined read commands" << std::endl;
	}
	if((old_value & CONFIG::MI_CONTROL_PIPELINED_READ_ENABLE_MASK) && !(new_value & CONFIG::MI_CONTROL_PIPELINED_READ_ENABLE_MASK))
	{
		logger << " - Disabling pipelined read commands" << std::endl;
	}
	if(!(old_value & CONFIG::MI_CONTROL_PIPELINED_WRITE_ENABLE_MASK) && (new_value & CONFIG::MI_CONTROL_PIPELINED_WRITE_ENABLE_MASK))
	{
		logger << " - Enabling pipelined write commands" << std::endl;
	}
	if((old_value & CONFIG::MI_CONTROL_PIPELINED_WRITE_ENABLE_MASK) && !(new_value & CONFIG::MI_CONTROL_PIPELINED_WRITE_ENABLE_MASK))
	{
		logger << " - Disabling pipelined write commands" << std::endl;
	}
	logger << EndDebugInfo;
}


} // end of namespace mci
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_MCI_MCI_TCC__
