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

#ifndef __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_GPIO_XPS_GPIO_TCC__
#define __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_GPIO_XPS_GPIO_TCC__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <string.h>

namespace unisim {
namespace component {
namespace cxx {
namespace com {
namespace xilinx {
namespace xps_gpio {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
XPS_GPIO<CONFIG>::XPS_GPIO(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Memory<typename CONFIG::MEMORY_ADDR> >(name, parent)
	, memory_export("memory-export", this)
	, logger(*this)
	, verbose(false)
	, toggle(0)
	, read_idx(0)
	, write_idx(~0)
	, c_baseaddr(CONFIG::C_BASEADDR)
	, c_highaddr(CONFIG::C_HIGHADDR)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_c_baseaddr("c-baseaddr", this, c_baseaddr, "Base address (C_BASEADDR design parameter)")
	, param_c_highaddr("c-highaddr", this, c_highaddr, "High address (C_HIGHADDR design parameter)")
{
	gpio_data_in[0] = 0;
	gpio_data_in[1] = 0;
	gpio_data[0] = CONFIG::C_DOUT_DEFAULT;
	gpio_data[1] = CONFIG::C_DOUT_DEFAULT;
	gpio_tri[0] = CONFIG::C_TRI_DEFAULT;
	gpio_tri[1] = CONFIG::C_TRI_DEFAULT;
	gpio2_data_in[0] = 0;
	gpio2_data_in[1] = 0;
	gpio2_data[0] = CONFIG::C_DOUT_DEFAULT_2;
	gpio2_data[1] = CONFIG::C_DOUT_DEFAULT_2;
	gpio2_tri[0] = CONFIG::C_TRI_DEFAULT_2;
	gpio2_tri[1] = CONFIG::C_TRI_DEFAULT_2;
	gier[0] = 0;
	gier[1] = 0;
	ip_ier[0] = 0;
	ip_ier[1] = 0;
	ip_isr[0] = 0;
	ip_isr[1] = 0;
	
	param_c_baseaddr.SetMutable(false);
	param_c_highaddr.SetMutable(false);

	std::stringstream sstr_description;
	sstr_description << "This module implements a Xilinx XPS GPIO (v2.00a). It has the following characteristics:" << std::endl;
	sstr_description << "PLB data width: " << CONFIG::C_SPLB_DWITH << " bits" << std::endl;
	sstr_description << "Use dual channel: " << (CONFIG::C_IS_DUAL ? "yes" : "no") << std::endl;
	sstr_description << "GPIO Channel 1 width: " << CONFIG::C_GPIO_WIDTH << " bits" << std::endl;
	if(CONFIG::C_IS_DUAL) sstr_description << "GPIO Channel 2 width: " << CONFIG::C_GPIO2_WIDTH << " bits" << std::endl;
	sstr_description << "GPIO_DATA reset value: 0x" << std::hex << CONFIG::C_DOUT_DEFAULT << std::dec << std::endl;
	sstr_description << "GPIO_TRI reset value: 0x" << std::hex << CONFIG::C_TRI_DEFAULT << std::dec << std::endl;
	if(CONFIG::C_IS_DUAL) sstr_description << "GPIO2_DATA reset value: 0x" << std::hex << CONFIG::C_DOUT_DEFAULT_2 << std::dec << std::endl;
	if(CONFIG::C_IS_DUAL) sstr_description << "GPIO2_TRI reset value: 0x" << std::hex << CONFIG::C_TRI_DEFAULT_2 << std::dec << std::endl;
	
	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG>
XPS_GPIO<CONFIG>::~XPS_GPIO()
{
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::BeginSetup()
{
	if(CONFIG::C_GPIO_WIDTH > 32)
	{
		logger << DebugError << "C_GPIO_WIDTH (" << CONFIG::C_GPIO_WIDTH << " must be < 32" << EndDebugError;
		return false;
	}
	if(CONFIG::C_GPIO2_WIDTH > 32)
	{
		logger << DebugError << "C_GPIO2_WIDTH (" << CONFIG::C_GPIO_WIDTH << " must be < 32" << EndDebugError;
		return false;
	}
	Reset();
	return true;
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::Reset()
{
	read_idx = 0;
	write_idx = ~0;
	toggle = 0;
	gpio_data_in[0] = 0;
	gpio_data_in[1] = 0;
	gpio_data[0] = CONFIG::C_DOUT_DEFAULT;
	gpio_data[1] = CONFIG::C_DOUT_DEFAULT;
	gpio_tri[0] = CONFIG::C_TRI_DEFAULT;
	gpio_tri[1] = CONFIG::C_TRI_DEFAULT;
	gpio2_data_in[0] = 0;
	gpio2_data_in[1] = 0;
	gpio2_data[0] = CONFIG::C_DOUT_DEFAULT_2;
	gpio2_data[1] = CONFIG::C_DOUT_DEFAULT_2;
	gpio2_tri[0] = CONFIG::C_TRI_DEFAULT_2;
	gpio2_tri[1] = CONFIG::C_TRI_DEFAULT_2;
	gier[0] = 0;
	gier[1] = 0;
	ip_ier[0] = 0;
	ip_ier[1] = 0;
	ip_isr[0] = 0;
	ip_isr[1] = 0;
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::ResetMemory()
{
	Reset();
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size)
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
bool XPS_GPIO<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size)
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
bool XPS_GPIO<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
template <typename T>
void XPS_GPIO<CONFIG>::Read(typename CONFIG::MEMORY_ADDR addr, T& value)
{
	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;
	uint32_t reg_value;
	switch(sizeof(T))
	{
		case 1:
			switch(offset)
			{
				case CONFIG::GPIO_DATA: reg_value = GetGPIO_DATA() >> 24; break;
				case CONFIG::GPIO_TRI: reg_value = GetGPIO_TRI() >> 24; break;
				case CONFIG::GPIO2_DATA: reg_value = GetGPIO2_DATA() >> 24; break;
				case CONFIG::GPIO2_TRI: reg_value = GetGPIO2_TRI() >> 24; break;
				case CONFIG::GIER: reg_value = GetGIER() >> 24; break;
				case CONFIG::IP_IER: reg_value = GetIP_IER() >> 24; break;
				case CONFIG::IP_ISR: reg_value = GetIP_ISR() >> 24; break;
				default: reg_value = 0; break;
			}
			break;
		case 2:
			switch(offset)
			{
				case CONFIG::GPIO_DATA: reg_value = GetGPIO_DATA() >> 16; break;
				case CONFIG::GPIO_TRI: reg_value = GetGPIO_TRI() >> 16; break;
				case CONFIG::GPIO2_DATA: reg_value = GetGPIO2_DATA() >> 16; break;
				case CONFIG::GPIO2_TRI: reg_value = GetGPIO2_TRI() >> 16; break;
				case CONFIG::GIER: reg_value = GetGIER() >> 16; break;
				case CONFIG::IP_IER: reg_value = GetIP_IER() >> 16; break;
				case CONFIG::IP_ISR: reg_value = GetIP_ISR() >> 16; break;
				default: reg_value = 0; break;
			}
			break;
		case 4:
			switch(offset)
			{
				case CONFIG::GPIO_DATA: reg_value = GetGPIO_DATA(); break;
				case CONFIG::GPIO_TRI: reg_value = GetGPIO_TRI(); break;
				case CONFIG::GPIO2_DATA: reg_value = GetGPIO2_DATA(); break;
				case CONFIG::GPIO2_TRI: reg_value = GetGPIO2_TRI(); break;
				case CONFIG::GIER: reg_value = GetGIER(); break;
				case CONFIG::IP_IER: reg_value = GetIP_IER(); break;
				case CONFIG::IP_ISR: reg_value = GetIP_ISR(); break;
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
			case CONFIG::GPIO_DATA: logger << "GPIO_DATA"; break;
			case CONFIG::GPIO_TRI: logger << "GPIO_TRI"; break;
			case CONFIG::GPIO2_DATA: logger << "GPIO2_DATA"; break;
			case CONFIG::GPIO2_TRI: logger << "GPIO2_TRI"; break;
			case CONFIG::GIER: logger << "GIER"; break;
			case CONFIG::IP_IER: logger << "IP_IER"; break;
			case CONFIG::IP_ISR: logger << "IP_ISR"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}
	value = unisim::util::endian::Host2BigEndian(reg_value);
}

template <class CONFIG>
template <typename T>
void XPS_GPIO<CONFIG>::Write(typename CONFIG::MEMORY_ADDR addr, T value)
{
	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;
	uint32_t reg_value = unisim::util::endian::BigEndian2Host(value);
	if(verbose)
	{
		logger << DebugInfo << "Writing 0x" << std::hex << reg_value << " in register 0x" << offset << std::dec << " (";
		switch(offset)
		{
			case CONFIG::GPIO_DATA: logger << "GPIO_DATA"; break;
			case CONFIG::GPIO_TRI: logger << "GPIO_TRI"; break;
			case CONFIG::GPIO2_DATA: logger << "GPIO2_DATA"; break;
			case CONFIG::GPIO2_TRI: logger << "GPIO2_TRI"; break;
			case CONFIG::GIER: logger << "GIER"; break;
			case CONFIG::IP_IER: logger << "IP_IER"; break;
			case CONFIG::IP_ISR: logger << "IP_ISR"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}

	switch(sizeof(T))
	{
		case 1:
			switch(offset)
			{
				case CONFIG::GPIO_DATA: SetGPIO_DATA((GetGPIO_DATA() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::GPIO_TRI: SetGPIO_TRI((GetGPIO_TRI() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::GPIO2_DATA: SetGPIO2_DATA((GetGPIO2_DATA() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::GPIO2_TRI: SetGPIO2_TRI((GetGPIO2_TRI() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::GIER: SetGIER((GetGIER() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::IP_IER: SetIP_IER((GetIP_IER() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				case CONFIG::IP_ISR: SetIP_ISR((GetIP_ISR() & 0x00ffffffUL) | ((uint32_t) reg_value << 24)); break;
				default: break;
			}
			break;
		case 2:
			switch(offset)
			{
				case CONFIG::GPIO_DATA: SetGPIO_DATA((GetGPIO_DATA() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::GPIO_TRI: SetGPIO_TRI((GetGPIO_TRI() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::GPIO2_DATA: SetGPIO2_DATA((GetGPIO2_DATA() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::GPIO2_TRI: SetGPIO2_TRI((GetGPIO2_TRI() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::GIER: SetGIER((GetGIER() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::IP_IER: SetIP_IER((GetIP_IER() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				case CONFIG::IP_ISR: SetIP_ISR((GetIP_ISR() & 0x0000ffffUL) | ((uint32_t) reg_value << 16)); break;
				default: break;
			}
			break;
		case 4:
			switch(offset)
			{
				case CONFIG::GPIO_DATA: SetGPIO_DATA(reg_value); break;
				case CONFIG::GPIO_TRI: SetGPIO_TRI(reg_value); break;
				case CONFIG::GPIO2_DATA: SetGPIO2_DATA(reg_value); break;
				case CONFIG::GPIO2_TRI: SetGPIO2_TRI(reg_value); break;
				case CONFIG::GIER: SetGIER(reg_value); break;
				case CONFIG::IP_IER: SetIP_IER(reg_value); break;
				case CONFIG::IP_ISR: SetIP_ISR(reg_value); break;
				default: break;
			}
			break;
	}
}

template <class CONFIG>
bool XPS_GPIO<CONFIG>::IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const
{
	return (addr >= c_baseaddr) && ((addr + size - 1) <= c_highaddr);
}

template <class CONFIG>
uint32_t XPS_GPIO<CONFIG>::GetGPIO_DATA() const
{
	return ((gpio_data_in[(read_idx >> GPIO_DATA_IDX_SHIFT) & 1] & gpio_tri[(read_idx >> GPIO_TRI_IDX_SHIFT) & 1]) |
	       (gpio_data[(read_idx >> GPIO_DATA_IDX_SHIFT) & 1] & ~gpio_tri[(read_idx >> GPIO_TRI_IDX_SHIFT) & 1])) & GPIO_MASK;
}

template <class CONFIG>
uint32_t XPS_GPIO<CONFIG>::GetGPIO_TRI() const
{
	return gpio_tri[(read_idx >> GPIO_TRI_IDX_SHIFT) & 1];
}

template <class CONFIG>
uint32_t XPS_GPIO<CONFIG>::GetGPIO2_DATA() const
{
	return ((gpio2_data_in[(read_idx >> GPIO2_DATA_IN_IDX_SHIFT) & 1] & gpio2_tri[(read_idx >> GPIO2_TRI_IDX_SHIFT) & 1]) |
	       (gpio2_data[(read_idx >> GPIO2_DATA_IN_IDX_SHIFT) & 1] & ~gpio2_tri[(read_idx >> GPIO2_TRI_IDX_SHIFT) & 1])) & GPIO_MASK;
}

template <class CONFIG>
uint32_t XPS_GPIO<CONFIG>::GetGPIO2_TRI() const
{
	return gpio2_tri[(read_idx >> GPIO2_TRI_IDX_SHIFT) & 1] & GPIO_MASK;
}

template <class CONFIG>
uint32_t XPS_GPIO<CONFIG>::GetGIER() const
{
	return gier[(read_idx >> GIER_IDX_SHIFT) & 1] & CONFIG::GIER_MASK;
}

template <class CONFIG>
uint32_t XPS_GPIO<CONFIG>::GetGIER_GLOBAL_INTERRUPT_ENABLE() const
{
	return (GetGIER() & CONFIG::GIER_GLOBAL_INTERRUPT_ENABLE_MASK) >> CONFIG::GIER_GLOBAL_INTERRUPT_ENABLE_OFFSET;
}

template <class CONFIG>
uint32_t XPS_GPIO<CONFIG>::GetIP_IER() const
{
	return ip_ier[(read_idx >> IP_IER_IDX_SHIFT) & 1] & CONFIG::IP_IER_MASK;
}

template <class CONFIG>
uint32_t XPS_GPIO<CONFIG>::GetIP_ISR() const
{
	return ip_isr[(read_idx >> IP_ISR_IDX_SHIFT) & 1] & CONFIG::IP_ISR_MASK;
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetGPIO_DATA_IN(uint32_t value)
{
	gpio_data_in[(write_idx >> GPIO_DATA_IN_IDX_SHIFT) & 1] =
		(gpio_data_in[(read_idx >> GPIO_DATA_IN_IDX_SHIFT) & 1] & ~GPIO_MASK) |
		(value & GPIO_MASK);
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetGPIO_DATA(uint32_t value)
{
	gpio_data[(write_idx >> GPIO_DATA_IDX_SHIFT) & 1] = (gpio_data[(read_idx >> GPIO_DATA_IDX_SHIFT) & 1] & ~GPIO_MASK) | (value & GPIO_MASK);
	toggle |= (1 << GPIO_DATA_IDX_SHIFT);
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetGPIO_DATA(unsigned int bitnum, bool value)
{
	if(bitnum >= CONFIG::C_GPIO_WIDTH) return;
	uint32_t write_mask = 0x80000000 >> bitnum;
	SetGPIO_DATA((GetGPIO_DATA() & ~write_mask) | (value ? write_mask : 0));
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetGPIO_TRI(uint32_t value)
{
	gpio_tri[(write_idx >> GPIO_TRI_IDX_SHIFT) & 1] = (gpio_tri[(read_idx >> GPIO_TRI_IDX_SHIFT) & 1] & ~GPIO_MASK) | (value & GPIO_MASK);
	toggle |= (1 << GPIO_TRI_IDX_SHIFT);
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetGPIO2_DATA(uint32_t value)
{
	gpio2_data[(write_idx >> GPIO2_DATA_IDX_SHIFT) & 1] = (gpio2_data[(read_idx >> GPIO2_DATA_IDX_SHIFT) & 1] & ~GPIO2_MASK) | (value & GPIO2_MASK);
	toggle |= (1 << GPIO2_DATA_IDX_SHIFT);
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetGPIO2_DATA(unsigned int bitnum, bool value)
{
	if(bitnum >= CONFIG::C_GPIO2_WIDTH) return;
	uint32_t write_mask = 0x80000000 >> bitnum;
	SetGPIO2_DATA((GetGPIO2_DATA() & ~write_mask) | (value ? write_mask : 0));
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetGPIO2_TRI(uint32_t value)
{
	gpio2_tri[(write_idx >> GPIO2_TRI_IDX_SHIFT) & 1] = (gpio2_tri[(read_idx >> GPIO_TRI_IDX_SHIFT) & 1] & ~GPIO2_MASK) | (value & GPIO2_MASK);
	toggle |= (1 << GPIO2_TRI_IDX_SHIFT);
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetGIER(uint32_t value)
{
	gier[(write_idx >> GIER_IDX_SHIFT) & 1] = (gier[(read_idx >> GIER_IDX_SHIFT) & 1] & ~CONFIG::GIER_MASK) | (value & CONFIG::GIER_MASK);
	toggle |= (1 << GIER_IDX_SHIFT);
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetIP_IER(uint32_t value)
{
	ip_ier[(write_idx >> IP_IER_IDX_SHIFT) & 1] = (ip_ier[(read_idx >> IP_IER_IDX_SHIFT) & 1] & ~CONFIG::IP_IER_MASK) | (value & CONFIG::IP_IER_MASK);
	toggle |= (1 << IP_IER_IDX_SHIFT);
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::SetIP_ISR(uint32_t value)
{
	ip_isr[(write_idx >> IP_ISR_IDX_SHIFT) & 1] = ip_ier[(read_idx >> IP_ISR_IDX_SHIFT) & 1] ^ (value & CONFIG::IP_ISR_TOGGLE_ON_WRITE_MASK);
	toggle |= (1 << IP_ISR_IDX_SHIFT);
}

template <class CONFIG>
void XPS_GPIO<CONFIG>::Update()
{
	if(gpio_data_in[(read_idx >> GPIO_DATA_IN_IDX_SHIFT) & 1] ^ gpio_data_in[(write_idx >> GPIO_DATA_IN_IDX_SHIFT) & 1])
	{
		SetIP_ISR(GetIP_ISR() | CONFIG::IP_ISR_CHANNEL_1_INTERRUPT_STATUS_MASK);
	}
	if(gpio2_data_in[(read_idx >> GPIO2_DATA_IN_IDX_SHIFT) & 1] ^ gpio2_data_in[(write_idx >> GPIO2_DATA_IN_IDX_SHIFT) & 1])
	{
		SetIP_ISR(GetIP_ISR() | CONFIG::IP_ISR_CHANNEL_2_INTERRUPT_STATUS_MASK);
	}
	
	read_idx ^= toggle;
	write_idx ^= toggle;
	toggle = 0;
}

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
