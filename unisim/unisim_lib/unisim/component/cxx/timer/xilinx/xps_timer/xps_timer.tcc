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

#ifndef __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_TIMER_XPS_TIMER_TCC__
#define __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_TIMER_XPS_TIMER_TCC__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <string.h>

namespace unisim {
namespace component {
namespace cxx {
namespace timer {
namespace xilinx {
namespace xps_timer {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
XPS_Timer<CONFIG>::XPS_Timer(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Memory<typename CONFIG::MEMORY_ADDR> >(name, parent)
	, memory_export("memory-export", this)
	, logger(*this)
	, verbose(false)
	, toggle(0)
	, read_idx(0)
	, write_idx(~0)
	, tcr0_roll_over(false)
	, tcr1_roll_over(false)
	, num_tcr0_roll_over(0)
	, num_tcr1_roll_over(0)
	, num_timer0_generate_interrupts(0)
	, num_timer1_generate_interrupts(0)
	, num_timer0_generate_interrupt_losses(0)
	, num_timer1_generate_interrupt_losses(0)
	, num_timer0_captures(0)
	, num_timer1_captures(0)
	, num_timer0_old_capture_losses(0)
	, num_timer1_old_capture_losses(0)
	, num_timer0_new_capture_losses(0)
	, num_timer1_new_capture_losses(0)
	, c_baseaddr(CONFIG::C_BASEADDR)
	, c_highaddr(CONFIG::C_HIGHADDR)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_c_baseaddr("c-baseaddr", this, c_baseaddr, "Base address (C_BASEADDR design parameter)")
	, param_c_highaddr("c-highaddr", this, c_highaddr, "High address (C_HIGHADDR design parameter)")
	, stat_num_tcr0_roll_over("num-tcr0-roll-over", this, num_tcr0_roll_over, "Number of timer/counter 0 roll over")
	, stat_num_tcr1_roll_over("num-tcr1-roll-over", this, num_tcr1_roll_over, "Number of timer/counter 1 roll over")
	, stat_num_timer0_generate_interrupts("num-timer0-generate-interrupts", this, num_timer0_generate_interrupts, "Number of timer 0 generate interrupts")
	, stat_num_timer1_generate_interrupts("num-timer1-generate-interrupts", this, num_timer1_generate_interrupts, "Number of timer 1 generate interrupts")
	, stat_num_timer0_generate_interrupt_losses("num-timer0-generate-interrupt-losses", this, num_timer0_generate_interrupt_losses, "Number of timer 0 generate interrupt losses")
	, stat_num_timer1_generate_interrupt_losses("num-timer1-generate-interrupt-losses", this, num_timer1_generate_interrupt_losses, "Number of timer 1 generate interrupt losses")
	, stat_num_timer0_captures("num-timer0-captures", this, num_timer0_captures, "Number of timer 0 captures")
	, stat_num_timer1_captures("num-timer1-captures", this, num_timer1_captures, "Number of timer 1 captures")
	, stat_num_timer0_old_capture_losses("num-timer0-old-capture-losses", this, num_timer0_old_capture_losses, "Number of timer 0 old capture losses")
	, stat_num_timer1_old_capture_losses("num-timer1-old-capture-losses", this, num_timer1_old_capture_losses, "Number of timer 1 old capture losses")
	, stat_num_timer0_new_capture_losses("num-timer0-new-capture-losses", this, num_timer0_new_capture_losses, "Number of timer 0 new capture losses")
	, stat_num_timer1_new_capture_losses("num-timer1-new-capture-losses", this, num_timer1_new_capture_losses, "Number of timer 1 new capture losses")
	, stat_num_timer0_capture_losses("num-timer0-capture-losses", this, "+", &stat_num_timer0_old_capture_losses, &stat_num_timer0_new_capture_losses, "Number of timer 0 capture losses")
	, stat_num_timer1_capture_losses("num-timer1-capture-losses", this, "+", &stat_num_timer1_old_capture_losses, &stat_num_timer1_new_capture_losses, "Number of timer 1 capture losses")
{
	param_c_baseaddr.SetMutable(false);
	param_c_highaddr.SetMutable(false);

	tcsr0[0] = 0;
	tcsr0[1] = 0;
	tlr0[0] = 0;
	tlr0[1] = 0;
	tcr0[0] = 0;
	tcr0[1] = 0;
	tcsr1[0] = 0;
	tcsr1[1] = 0;
	tlr1[0] = 0;
	tlr1[1] = 0;
	tcr1[0] = 0;
	tcr1[1] = 0;
	
	std::stringstream sstr_description;
	sstr_description << "This module implements a Xilinx XPS Timer/Counter (v1.02a). It has the following characteristics:" << std::endl;
	sstr_description << "PLB data width: " << CONFIG::C_SPLB_DWITH << " bits" << std::endl;
	sstr_description << "Width of the counters: " << CONFIG::C_COUNT_WIDTH << " bits" << std::endl;
	sstr_description << "One timer only: " << (CONFIG::C_ONE_TIMER_ONLY ? "yes" : "no") << std::endl;
	sstr_description << "CaptureTrig0 assertion level: " << (CONFIG::C_TRIG0_ASSERT ? "high" : "low") << std::endl;
	sstr_description << "CaptureTrig1 assertion level: " << (CONFIG::C_TRIG1_ASSERT ? "high" : "low") << std::endl;
	sstr_description << "GenerateOut0 assertion level: " << (CONFIG::C_GEN0_ASSERT ? "high" : "low") << std::endl;
	sstr_description << "GenerateOut0 assertion level: " << (CONFIG::C_GEN1_ASSERT ? "high" : "low") << std::endl;
	
	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG>
XPS_Timer<CONFIG>::~XPS_Timer()
{
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::BeginSetup()
{
	Reset();
	return true;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::Reset()
{
	read_idx = 0;
	write_idx = ~0;
	toggle = 0;
	tcsr0[0] = 0;
	tcsr0[1] = 0;
	tlr0[0] = 0;
	tlr0[1] = 0;
	tcr0[0] = 0;
	tcr0[1] = 0;
	tcsr1[0] = 0;
	tcsr1[1] = 0;
	tlr1[0] = 0;
	tlr1[1] = 0;
	tcr1[0] = 0;
	tcr1[1] = 0;
	tcr0_roll_over = false;
	tcr1_roll_over = false;
	num_tcr0_roll_over = 0;
	num_tcr1_roll_over = 0;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::ResetMemory()
{
	Reset();
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size)
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
bool XPS_Timer<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size)
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
bool XPS_Timer<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
template <typename T>
void XPS_Timer<CONFIG>::Read(typename CONFIG::MEMORY_ADDR addr, T& value)
{
	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;
	uint32_t reg_value;
	switch(sizeof(T))
	{
		case 1:
			switch(offset)
			{
				case CONFIG::TCSR0: reg_value = GetTCSR0() >> 24; break;
				case CONFIG::TLR0: reg_value = GetTLR0() >> 24; break;
				case CONFIG::TCR0: reg_value = GetTCR0() >> 24; break;
				case CONFIG::TCSR1: reg_value = GetTCSR1() >> 24; break;
				case CONFIG::TLR1: reg_value = GetTLR1() >> 24; break;
				case CONFIG::TCR1: reg_value = GetTCR1() >> 24; break;
				default: reg_value = 0; break;
			}
			break;
		case 2:
			switch(offset)
			{
				case CONFIG::TCSR0: reg_value = GetTCSR0() >> 16; break;
				case CONFIG::TLR0: reg_value = GetTLR0() >> 16; break;
				case CONFIG::TCR0: reg_value = GetTCR0() >> 16; break;
				case CONFIG::TCSR1: reg_value = GetTCSR1() >> 16; break;
				case CONFIG::TLR1: reg_value = GetTLR1() >> 16; break;
				case CONFIG::TCR1: reg_value = GetTCR1() >> 16; break;
				default: reg_value = 0; break;
			}
			break;
		case 4:
			switch(offset)
			{
				case CONFIG::TCSR0: reg_value = GetTCSR0(); break;
				case CONFIG::TLR0: reg_value = GetTLR0(); break;
				case CONFIG::TCR0: reg_value = GetTCR0(); break;
				case CONFIG::TCSR1: reg_value = GetTCSR1(); break;
				case CONFIG::TLR1: reg_value = GetTLR1(); break;
				case CONFIG::TCR1: reg_value = GetTCR1(); break;
				default: reg_value = 0; break;
			}
			break;
		default: reg_value = 0; break;
	}
	
	if(verbose)
	{
		logger << DebugInfo << "Reading 0x" << std::hex << reg_value << " from register 0x" << offset << std::dec << " (";
		switch(offset)
		{
			case CONFIG::TCSR0: logger << "tcsr0"; break;
			case CONFIG::TLR0: logger << "tlr0"; break;
			case CONFIG::TCR0: logger << "tcr0"; break;
			case CONFIG::TCSR1: logger << "tcsr1"; break;
			case CONFIG::TLR1: logger << "tlr1"; break;
			case CONFIG::TCR1: logger << "tcr1"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}
	value = unisim::util::endian::Host2BigEndian(reg_value);
}

template <class CONFIG>
template <typename T>
void XPS_Timer<CONFIG>::Write(typename CONFIG::MEMORY_ADDR addr, T value)
{
	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;
	uint32_t reg_value = unisim::util::endian::BigEndian2Host(value);
	if(verbose)
	{
		logger << DebugInfo << "Writing 0x" << std::hex << reg_value << " in register 0x" << offset << std::dec << " (";
		switch(offset)
		{
			case CONFIG::TCSR0: logger << "tcsr0"; break;
			case CONFIG::TLR0: logger << "tlr0"; break;
			case CONFIG::TCSR1: logger << "tcsr1"; break;
			case CONFIG::TLR1: logger << "tlr1"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}

	switch(sizeof(T))
	{
		case 1:
			switch(offset)
			{
				case CONFIG::TCSR0: SetTCSR0((GetTCSR0() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::TLR0: SetTLR0((GetTLR0() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::TCSR1: SetTCSR1((GetTCSR1() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::TLR1: SetTLR1((GetTLR1() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				default: break;
			}
			break;
		case 2:
			switch(offset)
			{
				case CONFIG::TCSR0: SetTCSR0((GetTCSR0() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::TLR0: SetTLR0((GetTLR0() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::TCSR1: SetTCSR1((GetTCSR1() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::TLR1: SetTLR1((GetTLR1() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				default: break;
			}
			break;
		case 4:
			switch(offset)
			{
				case CONFIG::TCSR0: SetTCSR0(reg_value); break;
				case CONFIG::TLR0: SetTLR0(reg_value); break;
				case CONFIG::TCSR1: SetTCSR1(reg_value); break;
				case CONFIG::TLR1: SetTLR1(reg_value); break;
				default: break;
			}
			break;
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::LogTCSR()
{
	uint32_t old_tcsr0 = tcsr0[(read_idx >> TCSR0_IDX_SHIFT) & 1];
	uint32_t old_tcsr1 = tcsr1[(read_idx >> TCSR1_IDX_SHIFT) & 1];
	uint32_t new_tcsr0 = tcsr0[(write_idx >> TCSR0_IDX_SHIFT) & 1];
	uint32_t new_tcsr1 =  tcsr1[(write_idx >> TCSR1_IDX_SHIFT) & 1];
	
	if(((old_tcsr0 & CONFIG::TCSR0_MASK) != (new_tcsr0 & CONFIG::TCSR0_MASK)) || ((old_tcsr1 & CONFIG::TCSR1_MASK) != (new_tcsr1 & CONFIG::TCSR1_MASK)))
	{
		// either TCSR0 or TCSR1 have changed
		logger << DebugInfo << "TCSR: ENALL PWMA0 T0INT ENT0 EINT0 LOAD0 ARHT0 CAPT0 GENT0 UDT0 MDT0";
		if(!CONFIG::C_ONE_TIMER_ONLY) logger << " PWMB0 T1INT ENT1 EINT1 LOAD1 ARHT1 CAPT1 GENT1 UDT1 MDT1";
		logger << EndDebugInfo;
		logger << DebugInfo << "        " << ((new_tcsr0 & CONFIG::TCSR0_ENALL_MASK) ? '1':'0')
		       << "     " << ((new_tcsr0 & CONFIG::TCSR0_PWMA0_MASK) ? '1':'0')
		       << "     " << ((new_tcsr0 & CONFIG::TCSR0_T0INT_MASK) ? '1':'0')
		       << "     " << ((new_tcsr0 & CONFIG::TCSR0_ENT0_MASK) ? '1':'0')
		       << "    " << ((new_tcsr0 & CONFIG::TCSR0_ENIT0_MASK) ? '1':'0')
		       << "     " << ((new_tcsr0 & CONFIG::TCSR0_LOAD0_MASK) ? '1':'0')
		       << "     " << ((new_tcsr0 & CONFIG::TCSR0_ARHT0_MASK) ? '1':'0')
		       << "     " << ((new_tcsr0 & CONFIG::TCSR0_CAPT0_MASK) ? '1':'0')
		       << "     " << ((new_tcsr0 & CONFIG::TCSR0_GENT0_MASK) ? '1':'0')
		       << "    " << ((new_tcsr0 & CONFIG::TCSR0_UDT0_MASK) ? '1':'0')
		       << "    " << ((new_tcsr0 & CONFIG::TCSR0_MDT0_MASK) ? '1':'0');
		if(!CONFIG::C_ONE_TIMER_ONLY)
		{
			logger << "     " << ((new_tcsr1 & CONFIG::TCSR1_PWMB0_MASK) ? '1':'0')
		           << "     " << ((new_tcsr1 & CONFIG::TCSR1_T1INT_MASK) ? '1':'0')
		           << "    " << ((new_tcsr1 & CONFIG::TCSR1_ENT1_MASK) ? '1':'0')
		           << "     " << ((new_tcsr1 & CONFIG::TCSR1_ENIT1_MASK) ? '1':'0')
		           << "     " << ((new_tcsr1 & CONFIG::TCSR1_LOAD1_MASK) ? '1':'0')
		           << "     " << ((new_tcsr1 & CONFIG::TCSR1_ARHT1_MASK) ? '1':'0')
		           << "     " << ((new_tcsr1 & CONFIG::TCSR1_CAPT1_MASK) ? '1':'0')
		           << "     " << ((new_tcsr1 & CONFIG::TCSR1_GENT1_MASK) ? '1':'0')
		           << "    " << ((new_tcsr1 & CONFIG::TCSR1_UDT1_MASK) ? '1':'0')
		           << "    " << ((new_tcsr1 & CONFIG::TCSR1_MDT1_MASK) ? '1':'0');
		}
		logger << EndDebugInfo;
		logger << DebugInfo << "TCSR recent changes:" << std::endl;
		if(!(old_tcsr0 & CONFIG::TCSR0_ENALL_MASK) && !(old_tcsr1 & CONFIG::TCSR1_ENALL_MASK) && (new_tcsr0 & CONFIG::TCSR0_ENALL_MASK) && (new_tcsr1 & CONFIG::TCSR1_ENALL_MASK))
		{
			logger << " - enabling all timers" << std::endl;
		}
		if(!CONFIG::C_ONE_TIMER_ONLY && CONFIG::C_GEN0_ASSERT && CONFIG::C_GEN1_ASSERT)
		{
			if((!(old_tcsr0 & CONFIG::TCSR0_PWMA0_MASK) || (old_tcsr0 & CONFIG::TCSR0_MDT0_MASK) ||
			    !(old_tcsr1 & CONFIG::TCSR1_PWMB0_MASK) || (old_tcsr1 & CONFIG::TCSR1_MDT1_MASK)) &&
			   (new_tcsr0 & CONFIG::TCSR0_PWMA0_MASK) &&
			   (new_tcsr1 & CONFIG::TCSR1_PWMB0_MASK) &&
			   !(new_tcsr0 & CONFIG::TCSR0_MDT0_MASK) &&
			   !(new_tcsr1 & CONFIG::TCSR1_MDT1_MASK))
			{
				logger << " - enabling pulse width modulation" << std::endl;
			}
			if((old_tcsr0 & CONFIG::TCSR0_PWMA0_MASK) && !(old_tcsr0 & CONFIG::TCSR0_MDT0_MASK) &&
			   (old_tcsr1 & CONFIG::TCSR1_PWMB0_MASK) && !(old_tcsr1 & CONFIG::TCSR1_MDT1_MASK) &&
			   (!(new_tcsr0 & CONFIG::TCSR0_PWMA0_MASK) ||
			    !(new_tcsr1 & CONFIG::TCSR1_PWMB0_MASK) ||
			    (new_tcsr0 & CONFIG::TCSR0_MDT0_MASK) ||
			    (new_tcsr1 & CONFIG::TCSR1_MDT1_MASK)))
			{
				logger << " - disabling pulse width modulation" << std::endl;
			}
		}
	
		if((old_tcsr0 & CONFIG::TCSR0_MASK) != (new_tcsr0 & CONFIG::TCSR0_MASK))
		{
			if(!(old_tcsr0 & CONFIG::TCSR0_T0INT_MASK) && (new_tcsr0 & CONFIG::TCSR0_T0INT_MASK))
			{
				logger << " - timer 0 interrupt has occured" << std::endl;
			}
			if((old_tcsr0 & CONFIG::TCSR0_T0INT_MASK) && !(new_tcsr0 & CONFIG::TCSR0_T0INT_MASK))
			{
				logger << " - timer 0 interrupt acknowledged" << std::endl;
			}
			if(!(old_tcsr0 & CONFIG::TCSR0_ENT0_MASK) && (new_tcsr0 & CONFIG::TCSR0_ENT0_MASK))
			{
				logger << " - enabling timer 0 (counter runs)" << std::endl;
			}
			if((old_tcsr0 & CONFIG::TCSR0_ENT0_MASK) && !(new_tcsr0 & CONFIG::TCSR0_ENT0_MASK))
			{
				logger << " - disabling timer 0 (counter halts)" << std::endl;
			}
			if(!(old_tcsr0 & CONFIG::TCSR0_ENIT0_MASK) && (new_tcsr0 & CONFIG::TCSR0_ENIT0_MASK))
			{
				logger << " - enabling timer 0 interrupt signal" << std::endl;
			}
			if((old_tcsr0 & CONFIG::TCSR0_ENIT0_MASK) && !(new_tcsr0 & CONFIG::TCSR0_ENIT0_MASK))
			{
				logger << " - disabling timer 0 interrupt signal" << std::endl;
			}
			if(!(old_tcsr0 & CONFIG::TCSR0_LOAD0_MASK) && (new_tcsr0 & CONFIG::TCSR0_LOAD0_MASK))
			{
				logger << " - loading timer 0 with value in TLR0" << std::endl;
			}
			if((old_tcsr0 & CONFIG::TCSR0_LOAD0_MASK) && !(new_tcsr0 & CONFIG::TCSR0_LOAD0_MASK))
			{
				logger << " - not loading timer 0" << std::endl;
			}
			if(!(old_tcsr0 & CONFIG::TCSR0_ARHT0_MASK) && (new_tcsr0 & CONFIG::TCSR0_ARHT0_MASK))
			{
				if(new_tcsr0 & CONFIG::TCSR0_MDT0_MASK)
				{
					logger << " - timer 0 will overwrite capture value" << std::endl;
				}
				else
				{
					logger << " - timer 0 will reload the generate value" << std::endl;
				}
			}
			if((old_tcsr0 & CONFIG::TCSR0_ARHT0_MASK) && !(new_tcsr0 & CONFIG::TCSR0_ARHT0_MASK))
			{
				if(new_tcsr0 & CONFIG::TCSR0_MDT0_MASK)
				{
					logger << " - timer 0 will hold capture value" << std::endl;
				}
				else
				{
					logger << " - timer 0 will hold counter" << std::endl;
				}
			}
			if(!(old_tcsr0 & CONFIG::TCSR0_CAPT0_MASK) && (new_tcsr0 & CONFIG::TCSR0_CAPT0_MASK))
			{
				logger << " - enabling external capture trigger Timer0" << std::endl;
			}
			if((old_tcsr0 & CONFIG::TCSR0_CAPT0_MASK) && !(new_tcsr0 & CONFIG::TCSR0_CAPT0_MASK))
			{
				logger << " - disabling external capture trigger Timer0" << std::endl;
			}
			if(!(old_tcsr0 & CONFIG::TCSR0_GENT0_MASK) && (new_tcsr0 & CONFIG::TCSR0_GENT0_MASK))
			{
				logger << " - enabling external generate signal Timer0" << std::endl;
			}
			if((old_tcsr0 & CONFIG::TCSR0_GENT0_MASK) && !(new_tcsr0 & CONFIG::TCSR0_GENT0_MASK))
			{
				logger << " - disabling external generate signal Timer0" << std::endl;
			}
			if(!(old_tcsr0 & CONFIG::TCSR0_UDT0_MASK) && (new_tcsr0 & CONFIG::TCSR0_UDT0_MASK))
			{
				logger << " - timer 0 functions as down counter" << std::endl;
			}
			if((old_tcsr0 & CONFIG::TCSR0_UDT0_MASK) && !(new_tcsr0 & CONFIG::TCSR0_UDT0_MASK))
			{
				logger << " - timer 0 functions as up counter" << std::endl;
			}
			if(!(old_tcsr0 & CONFIG::TCSR0_MDT0_MASK) && (new_tcsr0 & CONFIG::TCSR0_MDT0_MASK))
			{
				logger << " - timer 0 enters capture mode" << std::endl;
			}
			if((old_tcsr0 & CONFIG::TCSR0_MDT0_MASK) && !(new_tcsr0 & CONFIG::TCSR0_MDT0_MASK))
			{
				logger << " - timer 0 enters generate mode" << std::endl;
			}
		}

		if((old_tcsr1 & CONFIG::TCSR1_MASK) != (new_tcsr1 & CONFIG::TCSR1_MASK))
		{
			if(!(old_tcsr1 & CONFIG::TCSR1_T1INT_MASK) && (new_tcsr1 & CONFIG::TCSR1_T1INT_MASK))
			{
				logger << " - timer 1 interrupt has occured" << std::endl;
			}
			if((old_tcsr1 & CONFIG::TCSR1_T1INT_MASK) && !(new_tcsr1 & CONFIG::TCSR1_T1INT_MASK))
			{
				logger << " - timer 1 interrupt acknowledged" << std::endl;
			}
			if(!(old_tcsr1 & CONFIG::TCSR1_ENT1_MASK) && (new_tcsr1 & CONFIG::TCSR1_ENT1_MASK))
			{
				logger << " - enabling timer 1 (counter runs)" << std::endl;
			}
			if((old_tcsr1 & CONFIG::TCSR1_ENT1_MASK) && !(new_tcsr1 & CONFIG::TCSR1_ENT1_MASK))
			{
				logger << " - disabling timer 1 (counter halts)" << std::endl;
			}
			if(!(old_tcsr1 & CONFIG::TCSR1_ENIT1_MASK) && (new_tcsr1 & CONFIG::TCSR1_ENIT1_MASK))
			{
				logger << " - enabling timer 1 interrupt signal" << std::endl;
			}
			if((old_tcsr1 & CONFIG::TCSR1_ENIT1_MASK) && !(new_tcsr1 & CONFIG::TCSR1_ENIT1_MASK))
			{
				logger << " - disabling timer 1 interrupt signal" << std::endl;
			}
			if(!(old_tcsr1 & CONFIG::TCSR1_LOAD1_MASK) && (new_tcsr1 & CONFIG::TCSR1_LOAD1_MASK))
			{
				logger << " - loading timer 1 with value in TLR1" << std::endl;
			}
			if((old_tcsr1 & CONFIG::TCSR1_LOAD1_MASK) && !(new_tcsr1 & CONFIG::TCSR1_LOAD1_MASK))
			{
				logger << " - not loading timer 1" << std::endl;
			}
			if(!(old_tcsr1 & CONFIG::TCSR1_ARHT1_MASK) && (new_tcsr1 & CONFIG::TCSR1_ARHT1_MASK))
			{
				if(new_tcsr1 & CONFIG::TCSR1_MDT1_MASK)
				{
					logger << " - timer 1 will overwrite capture value" << std::endl;
				}
				else
				{
					logger << " - timer 1 will reload the generate value" << std::endl;
				}
			}
			if((old_tcsr1 & CONFIG::TCSR1_ARHT1_MASK) && !(new_tcsr1 & CONFIG::TCSR1_ARHT1_MASK))
			{
				if(new_tcsr1 & CONFIG::TCSR1_MDT1_MASK)
				{
					logger << " - timer 1 will hold capture value" << std::endl;
				}
				else
				{
					logger << " - timer 1 will hold counter" << std::endl;
				}
			}
			if(!(old_tcsr1 & CONFIG::TCSR1_CAPT1_MASK) && (new_tcsr1 & CONFIG::TCSR1_CAPT1_MASK))
			{
				logger << " - enabling external capture trigger Timer1" << std::endl;
			}
			if((old_tcsr1 & CONFIG::TCSR1_CAPT1_MASK) && !(new_tcsr1 & CONFIG::TCSR1_CAPT1_MASK))
			{
				logger << " - disabling external capture trigger Timer1" << std::endl;
			}
			if(!(old_tcsr1 & CONFIG::TCSR1_GENT1_MASK) && (new_tcsr1 & CONFIG::TCSR1_GENT1_MASK))
			{
				logger << " - enabling external generate signal Timer1" << std::endl;
			}
			if((old_tcsr1 & CONFIG::TCSR1_GENT1_MASK) && !(new_tcsr1 & CONFIG::TCSR1_GENT1_MASK))
			{
				logger << " - disabling external generate signal Timer1" << std::endl;
			}
			if(!(old_tcsr1 & CONFIG::TCSR1_UDT1_MASK) && (new_tcsr1 & CONFIG::TCSR1_UDT1_MASK))
			{
				logger << " - timer 1 functions as down counter" << std::endl;
			}
			if((old_tcsr1 & CONFIG::TCSR1_UDT1_MASK) && !(new_tcsr1 & CONFIG::TCSR1_UDT1_MASK))
			{
				logger << " - timer 1 functions as up counter" << std::endl;
			}
			if(!(old_tcsr1 & CONFIG::TCSR1_MDT1_MASK) && (new_tcsr1 & CONFIG::TCSR1_MDT1_MASK))
			{
				logger << " - timer 1 enters capture mode" << std::endl;
			}
			if((old_tcsr1 & CONFIG::TCSR1_MDT1_MASK) && !(new_tcsr1 & CONFIG::TCSR1_MDT1_MASK))
			{
				logger << " - timer 1 enters generate mode" << std::endl;
			}
		}
		logger << EndDebugInfo;
	}
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const
{
	return (addr >= c_baseaddr) && ((addr + size - 1) <= c_highaddr);
}

template <class CONFIG>
void XPS_Timer<CONFIG>::Update()
{
	read_idx ^= toggle;
	write_idx ^= toggle;
	toggle = 0;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0() const
{
	return tcsr0[(read_idx >> TCSR0_IDX_SHIFT) & 1] & CONFIG::TCSR0_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTLR0() const
{
	return tlr0[(read_idx >> TLR0_IDX_SHIFT) & 1] & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCR0() const
{
	return tcr0[(read_idx >> TCR0_IDX_SHIFT) & 1] & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1() const
{
	return CONFIG::C_ONE_TIMER_ONLY ? 0 : tcsr1[(read_idx >> TCSR1_IDX_SHIFT) & 1] & CONFIG::TCSR1_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTLR1() const
{
	return tlr1[(read_idx >> TLR1_IDX_SHIFT) & 1] & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCR1() const
{
	return CONFIG::C_ONE_TIMER_ONLY ? 0 : tcr1[(read_idx >> TCR1_IDX_SHIFT) & 1] & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCSR0(uint32_t value)
{
	uint32_t old_tcsr0 = tcsr0[(read_idx >> TCSR0_IDX_SHIFT) & 1];
	uint32_t new_tcsr0 = (old_tcsr0 & (CONFIG::TCSR0_ENALL_MASK | CONFIG::TCSR0_T0INT_MASK)) | (value & ~(CONFIG::TCSR0_ENALL_MASK | CONFIG::TCSR0_T0INT_MASK));
	uint32_t old_tcsr1 = tcsr1[(read_idx >> TCSR1_IDX_SHIFT) & 1];
	uint32_t new_tcsr1 = old_tcsr1;
	
	if(value & CONFIG::TCSR0_ENALL_MASK)
	{
		// writing a '1' in ENALL, sets ENALL, ENT0 and ENT1
		new_tcsr0 = new_tcsr0 | CONFIG::TCSR0_ENALL_MASK | CONFIG::TCSR0_ENT0_MASK;
		new_tcsr1 = new_tcsr1 | CONFIG::TCSR1_ENALL_MASK | CONFIG::TCSR1_ENT1_MASK;
	}
	else
	{
		// writing a '0' in ENALL, clear ENALL
		new_tcsr0 = new_tcsr0 & ~CONFIG::TCSR0_ENALL_MASK;
		new_tcsr1 = new_tcsr1 & ~CONFIG::TCSR1_ENALL_MASK;
	}
	
	if(value & CONFIG::TCSR0_T0INT_MASK)
	{
		// writing a '1' in T0INT, clears T0INT
		new_tcsr0 = new_tcsr0 & ~CONFIG::TCSR0_T0INT_MASK;
	}
	
	toggle |= (1 << TCSR0_IDX_SHIFT);
	tcsr0[(write_idx >> TCSR0_IDX_SHIFT) & 1] = new_tcsr0;
	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		toggle |= (1 << TCSR1_IDX_SHIFT);
		tcsr1[(write_idx >> TCSR1_IDX_SHIFT) & 1] = new_tcsr1;
	}

	if(IsVerbose())
	{
		LogTCSR();
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCSR0_T0INT()
{
	toggle |= (1 << TCSR0_IDX_SHIFT);
	tcsr0[(write_idx >> TCSR0_IDX_SHIFT) & 1] = tcsr0[(read_idx >> TCSR0_IDX_SHIFT) & 1] | CONFIG::TCSR0_T0INT_MASK;
	if(IsVerbose())
	{
		LogTCSR();
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTLR0(uint32_t value)
{
	toggle |= (1 << TLR0_IDX_SHIFT);
	tlr0[(write_idx >> TLR0_IDX_SHIFT) & 1] = value & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCR0(uint32_t value)
{
	toggle |= (1 << TCR0_IDX_SHIFT);
	tcr0[(write_idx >> TCR0_IDX_SHIFT) & 1] = value & CONFIG::COUNTER_MASK;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCSR1(uint32_t value)
{
	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		uint32_t old_tcsr0 = tcsr0[(read_idx >> TCSR0_IDX_SHIFT) & 1];
		uint32_t new_tcsr0 = old_tcsr0;
		uint32_t old_tcsr1 = tcsr1[(read_idx >> TCSR1_IDX_SHIFT) & 1];
		uint32_t new_tcsr1 = (old_tcsr1 & (CONFIG::TCSR1_ENALL_MASK | CONFIG::TCSR1_T1INT_MASK)) | (value & ~(CONFIG::TCSR1_ENALL_MASK | CONFIG::TCSR1_T1INT_MASK));
		
		if(value & CONFIG::TCSR1_ENALL_MASK)
		{
			// writing a '1' in ENALL, sets ENALL, ENT0 and ENT1
			new_tcsr0 = new_tcsr0 | CONFIG::TCSR0_ENALL_MASK | CONFIG::TCSR0_ENT0_MASK;
			new_tcsr1 = new_tcsr1 | CONFIG::TCSR1_ENALL_MASK | CONFIG::TCSR1_ENT1_MASK;
		}
		else
		{
			// writing a '0' in ENALL, clear ENALL
			new_tcsr0 = new_tcsr0 & ~CONFIG::TCSR0_ENALL_MASK;
			new_tcsr1 = new_tcsr1 & ~CONFIG::TCSR1_ENALL_MASK;
		}
		
		if(value & CONFIG::TCSR1_T1INT_MASK)
		{
			// writing a '1' in T1INT, clears T1INT
			new_tcsr1 = new_tcsr1 & ~CONFIG::TCSR1_T1INT_MASK;
		}

		toggle |= (1 << TCSR0_IDX_SHIFT);
		toggle |= (1 << TCSR1_IDX_SHIFT);
		tcsr0[(write_idx >> TCSR0_IDX_SHIFT) & 1] = new_tcsr0;
		tcsr1[(write_idx >> TCSR1_IDX_SHIFT) & 1] = new_tcsr1;

		if(IsVerbose())
		{
			LogTCSR();
		}
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCSR1_T1INT()
{
	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		toggle |= (1 << TCSR1_IDX_SHIFT);
		tcsr1[(write_idx >> TCSR1_IDX_SHIFT) & 1] = tcsr1[(read_idx >> TCSR1_IDX_SHIFT) & 1] | CONFIG::TCSR1_T1INT_MASK;
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTLR1(uint32_t value)
{
	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		toggle |= (1 << TLR1_IDX_SHIFT);
		tlr1[(write_idx >> TLR1_IDX_SHIFT) & 1] = value & CONFIG::COUNTER_MASK;
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::SetTCR1(uint32_t value)
{
	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		toggle |= (1 << TCR1_IDX_SHIFT);
		tcr1[(write_idx >> TCR1_IDX_SHIFT) & 1] = value & CONFIG::COUNTER_MASK;
	}
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_ENALL() const
{
	return (GetTCSR0() & CONFIG::TCSR0_ENALL_MASK) >> CONFIG::TCSR0_ENALL_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_PWMA0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_PWMA0_MASK) >> CONFIG::TCSR0_PWMA0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_T0INT() const
{
	return (GetTCSR0() & CONFIG::TCSR0_T0INT_MASK) >> CONFIG::TCSR0_T0INT_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_ENT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_ENT0_MASK) >> CONFIG::TCSR0_ENT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_ENIT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_ENIT0_MASK) >> CONFIG::TCSR0_ENIT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_LOAD0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_LOAD0_MASK) >> CONFIG::TCSR0_LOAD0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_ARHT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_ARHT0_MASK) >> CONFIG::TCSR0_ARHT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_CAPT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_CAPT0_MASK) >> CONFIG::TCSR0_CAPT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_GENT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_GENT0_MASK) >> CONFIG::TCSR0_GENT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_UDT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_UDT0_MASK) >> CONFIG::TCSR0_UDT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR0_MDT0() const
{
	return (GetTCSR0() & CONFIG::TCSR0_MDT0_MASK) >> CONFIG::TCSR0_MDT0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_ENALL() const
{
	return (GetTCSR1() & CONFIG::TCSR1_ENALL_MASK) >> CONFIG::TCSR1_ENALL_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_PWMB0() const
{
	return (GetTCSR1() & CONFIG::TCSR1_PWMB0_MASK) >> CONFIG::TCSR1_PWMB0_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_T1INT() const
{
	return (GetTCSR1() & CONFIG::TCSR1_T1INT_MASK) >> CONFIG::TCSR1_T1INT_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_ENT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_ENT1_MASK) >> CONFIG::TCSR1_ENT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_ENIT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_ENIT1_MASK) >> CONFIG::TCSR1_ENIT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_LOAD1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_LOAD1_MASK) >> CONFIG::TCSR1_LOAD1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_ARHT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_ARHT1_MASK) >> CONFIG::TCSR1_ARHT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_CAPT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_CAPT1_MASK) >> CONFIG::TCSR1_CAPT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_UDT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_UDT1_MASK) >> CONFIG::TCSR1_UDT1_OFFSET;
}

template <class CONFIG>
uint32_t XPS_Timer<CONFIG>::GetTCSR1_MDT1() const
{
	return (GetTCSR1() & CONFIG::TCSR1_MDT1_MASK) >> CONFIG::TCSR1_MDT1_OFFSET;
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::IsTCSR0_ENT0_Rising() const
{
	return (toggle & (1 << TCSR0_IDX_SHIFT)) &&
	       !(tcsr0[(read_idx >> TCSR0_IDX_SHIFT) & 1] & CONFIG::TCSR0_ENT0_MASK) &&
	       (tcsr0[(write_idx >> TCSR0_IDX_SHIFT) & 1] & CONFIG::TCSR0_ENT0_MASK);
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::IsTCSR1_ENT1_Rising() const
{
	return (toggle & (1 << TCSR1_IDX_SHIFT)) &&
	       !(tcsr1[(read_idx >> TCSR1_IDX_SHIFT) & 1] & CONFIG::TCSR1_ENT1_MASK) &&
	       (tcsr1[(write_idx >> TCSR1_IDX_SHIFT) & 1] & CONFIG::TCSR1_ENT1_MASK);
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::IsTCSR0_UDT0_Toggling() const
{
	return (toggle & (1 << TCSR0_IDX_SHIFT)) &&
	       ((tcsr0[(read_idx >> TCSR0_IDX_SHIFT) & 1] & CONFIG::TCSR0_UDT0_MASK) != (tcsr0[(write_idx >> TCSR0_IDX_SHIFT) & 1] & CONFIG::TCSR0_UDT0_MASK));
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::IsTCSR1_UDT1_Toggling() const
{
	return (toggle & (1 << TCSR1_IDX_SHIFT)) &&
	       ((tcsr1[(read_idx >> TCSR1_IDX_SHIFT) & 1] & CONFIG::TCSR1_UDT1_MASK) != (tcsr1[(write_idx >> TCSR1_IDX_SHIFT) & 1] & CONFIG::TCSR1_UDT1_MASK));
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::RunCounter0(uint32_t delta_count)
{
	if(!GetTCSR0_ENT0()) return false; // counter is halted

	// counter is running
	uint32_t old_tcr0 = GetTCR0();
	
	uint32_t new_tcr0;
	if(GetTCSR0_UDT0())
	{
		// Down counter
		new_tcr0 = (old_tcr0 != CONFIG::MAX_COUNT) ? (old_tcr0 - delta_count) & CONFIG::MAX_COUNT : CONFIG::MAX_COUNT;
		tcr0_roll_over = new_tcr0 > old_tcr0;
	}
	else
	{
		// Up counter
		new_tcr0 = (old_tcr0 != 0) ? (old_tcr0 + delta_count) & CONFIG::MAX_COUNT : 0;
		tcr0_roll_over = new_tcr0 < old_tcr0;
	}

	if(tcr0_roll_over && !GetTCSR0_MDT0() && GetTCSR0_ENIT0())
	{
		// If the mode is generate, T0INT indicates the counter has rolled over
		num_timer0_generate_interrupts++;
		if(GetTCSR0_T0INT())
		{
			num_timer0_generate_interrupt_losses++;
			logger << DebugWarning << "A timer 0 interrupt has been lost while in generate mode: either CPU is too slow to handle interrupts, or temporal decoupling of CPU model is too agressive" << EndDebugWarning; 
		}
		SetTCSR0_T0INT();
	}

	// immediate update of TCR0
	tcr0[(read_idx >> TCR0_IDX_SHIFT) & 1] = new_tcr0;
	
	if(tcr0_roll_over) num_tcr0_roll_over++;

	return tcr0_roll_over;
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::RunCounter1(uint32_t delta_count)
{
	if(CONFIG::C_ONE_TIMER_ONLY || !GetTCSR1_ENT1()) return false; // counter is missing or halted

	// counter is running
	uint32_t old_tcr1 = GetTCR1();
	uint32_t new_tcr1;
	if(GetTCSR1_UDT1())
	{
		// Down counter
		new_tcr1 = (old_tcr1 != CONFIG::MAX_COUNT) ? (old_tcr1 - delta_count) & CONFIG::MAX_COUNT : CONFIG::MAX_COUNT;
		tcr1_roll_over = new_tcr1 > old_tcr1;
	}
	else
	{
		// Up counter
		new_tcr1 = (old_tcr1 != 0) ? (old_tcr1 + delta_count) & CONFIG::MAX_COUNT : 0;
		tcr1_roll_over = new_tcr1 < old_tcr1;
	}

	if(tcr1_roll_over && !GetTCSR1_MDT1() && GetTCSR1_ENIT1())
	{
		// If the mode is generate, T1INT indicates the counter has rolled over
		num_timer1_generate_interrupts++;
		if(GetTCSR1_T1INT())
		{
			num_timer1_generate_interrupt_losses++;
			logger << DebugWarning << "A timer 1 interrupt has been lost while in generate mode: either CPU is too slow to handle interrupts, or temporal decoupling of CPU model is too agressive" << EndDebugWarning; 
		}
		SetTCSR1_T1INT();
	}

	// immediate update of TCR1
	tcr1[(read_idx >> TCR1_IDX_SHIFT) & 1] = new_tcr1;

	if(tcr1_roll_over) num_tcr1_roll_over++;

	return tcr1_roll_over;
}

template <class CONFIG>
void XPS_Timer<CONFIG>::CaptureTrigger0()
{
	// Capture mode (channel 0)
	if(GetTCSR0_CAPT0())
	{
		// Timer 0 is in capture mode
		if(GetTCSR0_MDT0())
		{
			// External capture trigger Timer 0 enabled
			num_timer0_captures++;
			if(GetTCSR0_T0INT() && GetTCSR0_ENIT0())
			{
				logger << DebugWarning << "Timer 0 is loosing ";
				if(GetTCSR0_ARHT0())
				{
					num_timer0_old_capture_losses++;
					logger << "an old capture because a new capture has arrived while the old capture";
				}
				else
				{
					num_timer0_new_capture_losses++;
					logger << "a new capture because it is holding an old capture that";
				}
				logger << DebugWarning << " has not been yet acknowledged: either CPU is too slow to handle interrupts, or temporal decoupling of CPU model is too agressive" << EndDebugWarning;
			}
			if(!GetTCSR0_T0INT() || GetTCSR0_ARHT0())
			{
				// Overwrite capture value
				// Snap counter/timer 0
				SetTLR0(GetTCR0());
				SetTCSR0_T0INT();
			}
		}
	}
}

template <class CONFIG>
void XPS_Timer<CONFIG>::CaptureTrigger1()
{
	// Capture mode (channel 1)
	if(!CONFIG::C_ONE_TIMER_ONLY)
	{
		// Timer 1 is available
		if(GetTCSR1_CAPT1())
		{
			// Timer 1 is in capture mode
			if(GetTCSR1_MDT1())
			{
				// External capture trigger Timer 1 enabled
				num_timer1_captures++;
				if(GetTCSR1_T1INT() && GetTCSR1_ENIT1())
				{
					logger << DebugWarning << "Timer 1 is loosing ";
					if(GetTCSR1_ARHT1())
					{
						num_timer1_old_capture_losses++;
						logger << "an old capture because a new capture has arrived while the old capture";
					}
					else
					{
						num_timer1_new_capture_losses++;
						logger << "a new capture because it is holding an old capture that";
					}
					logger << DebugWarning << " has not been yet acknowledged: either CPU is too slow to handle interrupts, or temporal decoupling of CPU model is too agressive" << EndDebugWarning;
				}
				if(!GetTCSR1_T1INT() || GetTCSR1_ARHT1())
				{
					// Overwrite capture value
					// Snap counter/timer 1
					SetTLR1(GetTCR1());
					SetTCSR1_T1INT();
				}
			}
		}
	}
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::NeedsLoadingTCR0() const
{
	// Load of TCR0 occurs if one of the following condition is met:
	//  - TCSR0[LOAD0]=1
	//  - TCR0 rolls over while in generate mode and autoreload is enabled
	//  - TCR0 rolls over while timer/counter 0 is in PWM mode
	return GetTCSR0_LOAD0() ||
	       (!GetTCSR0_PWMA0() && tcr0_roll_over && GetTCSR0_ENT0() && !GetTCSR0_MDT0() && GetTCSR0_ARHT0()) ||
	       (GetTCSR0_PWMA0() && tcr0_roll_over);
}

template <class CONFIG>
bool XPS_Timer<CONFIG>::NeedsLoadingTCR1() const
{
	// Load of TCR1 occurs if one of the following condition is met:
	//  - TCSR1[LOAD1]=1
	//  - TCR1 rolls over while in generate mode and autoreload is enabled
	//  - TCR0 rolls over while timer/counter 1 is in PWM mode (PWM0 output goes high)
	return GetTCSR1_LOAD1() ||
	       (!GetTCSR1_PWMB0() && tcr1_roll_over && GetTCSR1_ENT1() && !GetTCSR1_MDT1() && GetTCSR1_ARHT1()) ||
	       (GetTCSR1_PWMB0() && tcr0_roll_over);
}

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
