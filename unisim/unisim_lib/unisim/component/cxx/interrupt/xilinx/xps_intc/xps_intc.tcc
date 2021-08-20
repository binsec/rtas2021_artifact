/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_COMPONENT_CXX_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_TCC__
#define __UNISIM_COMPONENT_CXX_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_TCC__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <string.h>

namespace unisim {
namespace component {
namespace cxx {
namespace interrupt {
namespace xilinx {
namespace xps_intc {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
XPS_IntC<CONFIG>::XPS_IntC(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Memory<typename CONFIG::MEMORY_ADDR> >(name, parent)
	, memory_export("memory-export", this)
	, logger(*this)
	, isr(0)
	, ier(0)
	, ivr(0xffffffffUL)
	, mer(0)
	, verbose(false)
	, c_baseaddr(CONFIG::C_BASEADDR)
	, c_highaddr(CONFIG::C_HIGHADDR)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_c_baseaddr("c-baseaddr", this, c_baseaddr, "Base address (C_BASEADDR design parameter)")
	, param_c_highaddr("c-highaddr", this, c_highaddr, "High address (C_HIGHADDR design parameter)")
{
	param_c_baseaddr.SetMutable(false);
	param_c_highaddr.SetMutable(false);

	intr[0] = 0;
	intr[1] = 0;

	std::stringstream sstr_description;
	sstr_description << "This module implements a Xilinx XPS Interrupt Controller (v2.01a). It has the following characteristics:" << std::endl;
	sstr_description << "PLB data width: " << CONFIG::C_SPLB_DWITH << " bits" << std::endl;
	sstr_description << "Number of interrupt inputs: " << CONFIG::C_NUM_INTR_INPUTS << " interrupt inputs" << std::endl;
	sstr_description << "IPR support: " << (CONFIG::C_HAS_IPR ? "yes" : "no") << std::endl;
	sstr_description << "SIE support: " << (CONFIG::C_HAS_SIE ? "yes" : "no") << std::endl;
	sstr_description << "CIE support: " << (CONFIG::C_HAS_CIE ? "yes" : "no") << std::endl;
	sstr_description << "IVR support: " << (CONFIG::C_HAS_IVR ? "yes" : "no") << std::endl;
	
	sstr_description << "Ouput: active on ";
	if(CONFIG::C_IRQ_IS_LEVEL)
	{
		// Level
		sstr_description << (CONFIG::C_IRQ_ACTIVE ? "high" : "low") << " level";
	}
	else
	{
		// Edge
		sstr_description << (CONFIG::C_IRQ_ACTIVE ? "rising" : "falling") << " edge";
	}
	sstr_description << std::endl;
	unsigned int irq;
	for(irq = 0; irq < CONFIG::C_NUM_INTR_INPUTS; irq++)
	{
		sstr_description << "input #" << irq << " capture mode: ";
		uint32_t mask = 1UL << irq;
		if(CONFIG::C_KIND_OF_INTR & mask)
		{
			// Edge
			if(CONFIG::C_KIND_OF_EDGE & mask)
			{
				sstr_description << "rising";
				// Rising
			}
			else
			{
				// Falling
				sstr_description << "falling";
			}
			sstr_description << " edge";
		}
		else
		{
			// Level
			if(CONFIG::C_KIND_OF_LVL & mask)
			{
				// High
				sstr_description << "high";
			}
			else
			{
				// Low
				sstr_description << "low";
			}
			sstr_description << " level";
		}
		sstr_description << std::endl;
	}
	
	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG>
XPS_IntC<CONFIG>::~XPS_IntC()
{
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::BeginSetup()
{
	Reset();
	return true;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::Reset()
{
	isr = 0;
	ier = 0;
	ivr = 0xffffffffUL;
	mer = 0;
	intr[0] = 0;
	intr[1] = 0;
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const
{
	return (addr >= c_baseaddr) && ((addr + size - 1) <= c_highaddr);
}

template <class CONFIG>
void XPS_IntC<CONFIG>::ResetMemory()
{
	Reset();
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size)
{
	if(size > 0)
	{
		if(!IsMapped(addr, size)) return false;
		uint8_t *dst = (uint8_t *) buffer;
		
		do
		{
			uint32_t value;
			Read(addr, value);
			if(size >= 4)
			{
				memcpy(dst, &value, 4);
				size -= 4;
				dst += 4;
				addr += 4;
			}
			else
			{
				memcpy(dst, &value, size);
				size = 0;
			}
		}
		while(size);
	}
	return true;
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size)
{
	if(size > 0)
	{
		if(!IsMapped(addr, size)) return false;
		const uint8_t *src = (const uint8_t *) buffer;
		
		do
		{
			uint32_t value;
			if(size >= 4)
			{
				memcpy(&value, src, 4);
				size -= 4;
				src += 4;
				addr += 4;
			}
			else
			{
				Read(addr, value);
				memcpy(&value, src, size);
				size = 0;
			}
			Write(addr, value);
		}
		while(size);
	}
	return true;
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
bool XPS_IntC<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
void XPS_IntC<CONFIG>::Read(typename CONFIG::MEMORY_ADDR addr, uint32_t& value)
{
	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;
	uint32_t reg_value;
	switch(offset)
	{
		case CONFIG::ISR: reg_value = GetISR(); break;
		case CONFIG::IPR: reg_value = GetIPR(); break;
		case CONFIG::IER: reg_value = GetIER(); break;
		case CONFIG::IVR: reg_value = GetIVR(); break;
		case CONFIG::MER: reg_value = GetMER(); break;
		default: reg_value = 0;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "Reading 0x" << std::hex << reg_value << " from register 0x" << offset << std::dec << " (";
		switch(offset)
		{
			case CONFIG::ISR: logger << "isr"; break;
			case CONFIG::IPR: logger << "ipr"; break;
			case CONFIG::IER: logger << "ier"; break;
			case CONFIG::IVR: logger << "ivr"; break;
			case CONFIG::MER: logger << "mer"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}

	value = unisim::util::endian::Host2BigEndian(reg_value);
}

template <class CONFIG>
void XPS_IntC<CONFIG>::Write(typename CONFIG::MEMORY_ADDR addr, uint32_t value)
{
	uint32_t reg_value = unisim::util::endian::BigEndian2Host(value);

	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;

	if(IsVerbose())
	{
		logger << DebugInfo << "Writing 0x" << std::hex << reg_value << " in register 0x" << offset << std::dec << " (";
		switch(offset)
		{
			case CONFIG::ISR: logger << "isr"; break;
			case CONFIG::IER: logger << "ier"; break;
			case CONFIG::IAR: logger << "iar"; break;
			case CONFIG::SIE: logger << "sie"; break;
			case CONFIG::CIE: logger << "cie"; break;
			case CONFIG::MER: logger << "mer"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}
	
	switch(offset)
	{
		case CONFIG::ISR: if(!GetMER_HIE()) SetISR(reg_value); break;
		case CONFIG::IER: SetIER(reg_value); break;
		case CONFIG::IAR: SetIAR(reg_value); break;
		case CONFIG::SIE: SetSIE(reg_value); break;
		case CONFIG::CIE: SetCIE(reg_value); break;
		case CONFIG::MER: SetMER(reg_value); break;
		default: break;
	}
}

template <class CONFIG>
uint32_t XPS_IntC<CONFIG>::GetISR() const
{
	return isr & MASK;
}

template <class CONFIG>
uint32_t XPS_IntC<CONFIG>::GetIPR() const
{
	return CONFIG::C_HAS_IPR ? (GetISR() & GetIER()) : 0;
}

template <class CONFIG>
uint32_t XPS_IntC<CONFIG>::GetIER() const
{
	return ier & MASK;
}

template <class CONFIG>
uint32_t XPS_IntC<CONFIG>::GetIVR() const
{
	return CONFIG::C_HAS_IVR ? ivr : 0;
}

template <class CONFIG>
uint32_t XPS_IntC<CONFIG>::GetMER() const
{
	return mer & CONFIG::MER_MASK;
}

template <class CONFIG>
uint32_t XPS_IntC<CONFIG>::GetMER_ME() const
{
	return (GetMER() & CONFIG::MER_ME_MASK) >> CONFIG::MER_ME_OFFSET;
}

template <class CONFIG>
uint32_t XPS_IntC<CONFIG>::GetMER_HIE() const
{
	return (GetMER() & CONFIG::MER_HIE_MASK) >> CONFIG::MER_HIE_OFFSET;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetISR(uint32_t value)
{
	isr = value & MASK;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetIER(uint32_t value)
{
	ier = value & MASK;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetIAR(uint32_t value)
{
	isr = (isr & ~value) & MASK;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetSIE(uint32_t value)
{
	if(CONFIG::C_HAS_SIE)
	{
		SetIER(GetIER() | value);
	}
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetCIE(uint32_t value)
{
	if(CONFIG::C_HAS_CIE)
	{
		SetIER(GetIER() & ~value);
	}
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetMER(uint32_t value)
{
	// Note: only transition of MER[HIE] from 0 to 1 is allowed
	// Programmer must reset the controller to make it go back 0
	mer = (value & ~CONFIG::MER_ME_MASK) | (value & CONFIG::MER_MASK);
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetInterruptInput(unsigned int irq, bool level)
{
	uint32_t mask = 1 << irq;
	intr[1] = level ? intr[0] | mask : intr[0] & ~mask;
}

template <class CONFIG>
void XPS_IntC<CONFIG>::DetectInterruptInput()
{
	if(GetMER_HIE()) // Hardware interrupts enabled ?
	{
		if(IsVerbose())
		{
			logger << DebugInfo << "Hardware interrupts enabled" << EndDebugInfo;
			logger << DebugInfo << "Detecting interrupt input" << EndDebugInfo;
		}
		unsigned int irq;
		for(irq = 0; irq < CONFIG::C_NUM_INTR_INPUTS; irq++)
		{
			uint32_t mask = 1 << irq;
			// Interrupt detection
			if(CONFIG::C_KIND_OF_INTR & mask)
			{
				// Edge
				if(CONFIG::C_KIND_OF_EDGE & mask)
				{
					// Rising
					if(!(intr[0] & mask) && (intr[1] & mask))
					{
						if(IsVerbose())
						{
							logger << DebugInfo << "IRQ #" << irq << " rising edge detected" << EndDebugInfo;
						}
						SetISR(GetISR() | mask);
					}
				}
				else
				{
					// Falling
					if((intr[0] & mask) && !(intr[1] & mask))
					{
						if(IsVerbose())
						{
							logger << DebugInfo << "IRQ #" << irq << " falling edge detected" << EndDebugInfo;
						}
						SetISR(GetISR() | mask);
					}
				}
			}
			else
			{
				// Level
				if(CONFIG::C_KIND_OF_LVL & mask)
				{
					// High
					if(intr[1] & mask)
					{
						if(IsVerbose())
						{
							logger << DebugInfo << "IRQ #" << irq << " high level detected" << EndDebugInfo;
						}
						SetISR(GetISR() | mask);
					}
				}
				else
				{
					// Low
					if(!(intr[1] & mask))
					{
						if(IsVerbose())
						{
							logger << DebugInfo << "IRQ #" << irq << " low level detected" << EndDebugInfo;
						}
						SetISR(GetISR() | mask);
					}
				}
			}
		}
	}
	else
	{
		if(IsVerbose())
		{
			logger << DebugInfo << "Software interrupts enabled" << EndDebugInfo;
		}
	}
	
	intr[1] = intr[0];
}

template <class CONFIG>
void XPS_IntC<CONFIG>::GenerateRequest()
{
	if(GetMER_ME())
	{
		if(IsVerbose() && (GetISR() & GetIER()))
		{
			logger << DebugInfo << "Pending IRQs: ISR=0x" << std::hex << GetISR() << ", IER=0x" << GetIER() << std::dec << EndDebugInfo;
		}
		unsigned int irq = 0;
		if(unisim::util::arithmetic::BitScanForward(irq, isr & ier))
		{
			// Pending IRQ, Request generation
			if(IsVerbose())
			{
				logger << DebugInfo << "Generating request for IRQ #" << irq << EndDebugInfo;
			}
			if(CONFIG::C_HAS_IVR)
			{
				ivr = irq;
			}
			if(CONFIG::C_IRQ_IS_LEVEL)
			{
				// Level
				SetOutputLevel(CONFIG::C_IRQ_ACTIVE);
			}
			else
			{
				// Edge
				SetOutputEdge(CONFIG::C_IRQ_ACTIVE);
			}
			
			return;
		}
	}

	if(IsVerbose())
	{
		logger << DebugInfo << "No pending IRQ: ISR=0x" << std::hex << GetISR() << ", IER=0x" << GetIER() << std::dec << EndDebugInfo;
	}
	
	// no pending IRQ
	if(CONFIG::C_HAS_IVR)
	{
		ivr = 0;
	}
	if(CONFIG::C_IRQ_IS_LEVEL)
	{
		// Level
		SetOutputLevel(!CONFIG::C_IRQ_ACTIVE);
	}
	else
	{
		// Edge
		SetOutputEdge(!CONFIG::C_IRQ_ACTIVE);
	}
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetOutputLevel(bool level)
{
}

template <class CONFIG>
void XPS_IntC<CONFIG>::SetOutputEdge(bool final_level)
{
}

} // end of namespace xps_intc
} // end of namespace xilinx
} // end of namespace interrupt
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
