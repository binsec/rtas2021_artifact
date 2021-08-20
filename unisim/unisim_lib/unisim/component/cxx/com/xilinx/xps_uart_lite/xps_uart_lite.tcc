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

#ifndef __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_UART_LITE_XPS_UART_LITE_TCC__
#define __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_UART_LITE_XPS_UART_LITE_TCC__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <string.h>
#include <unisim/util/queue/queue.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace com {
namespace xilinx {
namespace xps_uart_lite {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG>
XPS_UARTLite<CONFIG>::XPS_UARTLite(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Memory<typename CONFIG::MEMORY_ADDR> >(name, parent)
	, Client<CharIO>(name, parent)
	, memory_export("memory-export", this)
	, char_io_import("char-io-import", this)
	, logger(*this)
	, verbose(false)
	, rx_fifo()
	, tx_fifo()
	, stat_reg(CONFIG::STAT_REG_RESET_VALUE)
	, tx_fifo_becomes_empty(false)
	, c_baseaddr(CONFIG::C_BASEADDR)
	, c_highaddr(CONFIG::C_HIGHADDR)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_c_baseaddr("c-baseaddr", this, c_baseaddr, "Base address (C_BASEADDR design parameter)")
	, param_c_highaddr("c-highaddr", this, c_highaddr, "High address (C_HIGHADDR design parameter)")
{
	param_c_baseaddr.SetMutable(false);
	param_c_highaddr.SetMutable(false);

	std::stringstream sstr_description;
	sstr_description << "This module implements a Xilinx XPS UART Lite (v1.01a). It has the following characteristics:" << std::endl;
	sstr_description << "PLB data width: " << CONFIG::C_SPLB_DWITH << " bits" << std::endl;
	sstr_description << "Baud rate: " << CONFIG::C_BAUDRATE << " bits/s" << std::endl;
	sstr_description << "Data bits: " << CONFIG::C_DATA_BITS << " bits" << std::endl;
	sstr_description << "Parity: " << (CONFIG::C_USE_PARITY ? (CONFIG::C_ODD_PARITY ? "odd" : "even") : "none") << std::endl;

	Object::SetDescription(sstr_description.str().c_str());
}

template <class CONFIG>
XPS_UARTLite<CONFIG>::~XPS_UARTLite()
{
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::IsVerbose() const
{
	return verbose;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::BeginSetup()
{
	Reset();
	return true;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::ResetRegs()
{
	rx_fifo.Clear();
	tx_fifo.Clear();
	stat_reg = CONFIG::STAT_REG_RESET_VALUE;
	tx_fifo_becomes_empty = false;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::Reset()
{
	ResetRegs();
	
	if(char_io_import)
	{
		char_io_import->ResetCharIO();
	}
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::ResetMemory()
{
	ResetRegs();
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size)
{
	if(size > 0)
	{
		if(!IsMapped(addr, size)) return false;
		uint8_t *dst = (uint8_t *) buffer;
		
		do
		{
			uint8_t value;
			Read(addr, value, true);
			memcpy(dst, &value, 1);
			size --;
			dst++;
			addr ++;
		}
		while(size);
	}
	return true;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size)
{
	if(size > 0)
	{
		if(!IsMapped(addr, size)) return false;
		const uint8_t *src = (const uint8_t *) buffer;
		
		do
		{
			uint8_t value;
			memcpy(&value, src, 1);
			Write(addr, value, true);
			size--;
			src++;
			addr++;
		}
		while(size);
	}
	return true;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width)
{
	return false; // TODO
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::Read(typename CONFIG::MEMORY_ADDR addr, uint8_t& value, bool debug)
{
	bool status = true;
	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;
	uint8_t reg_value = 0;
	switch(offset)
	{
		case CONFIG::RX_FIFO + 3:
			if(debug)
			{
				reg_value = GetRX_FIFO();
			}
			else
			{
				if(!ReadRX_FIFO(reg_value)) status = false;
			}
			break;
		case CONFIG::STAT_REG + 3:
			reg_value = debug ? GetSTAT_REG() : ReadSTAT_REG();
			break;
	}
	
	if(IsVerbose())
	{
		logger << DebugInfo << "Reading 0x" << std::hex << (unsigned int) reg_value << " from register 0x" << offset << std::dec << " (";
		switch(offset)
		{
			case CONFIG::RX_FIFO + 0:
			case CONFIG::RX_FIFO + 1:
			case CONFIG::RX_FIFO + 2:
			case CONFIG::RX_FIFO + 3: logger << "Rx FIFO"; break;
			case CONFIG::TX_FIFO + 0:
			case CONFIG::TX_FIFO + 1: 
			case CONFIG::TX_FIFO + 2:
			case CONFIG::TX_FIFO + 3: logger << "Tx FIFO"; break;
			case CONFIG::STAT_REG + 0:
			case CONFIG::STAT_REG + 1:
			case CONFIG::STAT_REG + 2:
			case CONFIG::STAT_REG + 3: logger << "STAT_REG"; break;
			case CONFIG::CTRL_REG + 0:
			case CONFIG::CTRL_REG + 1:
			case CONFIG::CTRL_REG + 2:
			case CONFIG::CTRL_REG + 3: logger << "CTRL_REG"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}
	value = unisim::util::endian::Host2BigEndian(reg_value);
	
	return status;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::Write(typename CONFIG::MEMORY_ADDR addr, uint8_t value, bool debug)
{
	typename CONFIG::MEMORY_ADDR offset = addr - c_baseaddr;
	uint8_t reg_value = unisim::util::endian::BigEndian2Host(value);
	if(IsVerbose())
	{
		logger << DebugInfo << "Writing 0x" << std::hex << (unsigned int) reg_value << " in register 0x" << offset << std::dec << " (";
		switch(offset)
		{
			case CONFIG::RX_FIFO + 0:
			case CONFIG::RX_FIFO + 1:
			case CONFIG::RX_FIFO + 2:
			case CONFIG::RX_FIFO + 3: logger << "Rx FIFO"; break;
			case CONFIG::TX_FIFO + 0:
			case CONFIG::TX_FIFO + 1: 
			case CONFIG::TX_FIFO + 2:
			case CONFIG::TX_FIFO + 3: logger << "Tx FIFO"; break;
			case CONFIG::STAT_REG + 0:
			case CONFIG::STAT_REG + 1:
			case CONFIG::STAT_REG + 2:
			case CONFIG::STAT_REG + 3: logger << "STAT_REG"; break;
			case CONFIG::CTRL_REG + 0:
			case CONFIG::CTRL_REG + 1:
			case CONFIG::CTRL_REG + 2:
			case CONFIG::CTRL_REG + 3: logger << "CTRL_REG"; break;
			default: logger << "?"; break;
		}
		logger << ")" << EndDebugInfo;
	}

	switch(offset)
	{
		case CONFIG::TX_FIFO + 3:
			if(debug)
			{
				SetTX_FIFO(reg_value);
			}
			else
			{
				WriteTX_FIFO(reg_value);
			}
			break;
			
		case CONFIG::CTRL_REG + 3:
			if(!debug) WriteCTRL_REG(reg_value);
			break;
		default: break;
	}
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::Read(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size)
{
	bool status = true;
	if(size > 0)
	{
		uint8_t *dst = (uint8_t *) buffer;
		
		do
		{
			uint8_t value;
			status = Read(addr, value) && status;
			memcpy(dst, &value, 1);
			size--;
			dst++;
			addr++;
		}
		while(size);
	}
	return status;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::Write(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size)
{
	if(size > 0)
	{
		const uint8_t *src = (const uint8_t *) buffer;
		
		do
		{
			uint8_t value;
			memcpy(&value, src, 1);
			Write(addr, value);
			size--;
			src++;
			addr++;
		}
		while(size);
	}
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const
{
	return (addr >= c_baseaddr) && ((addr + size - 1) <= c_highaddr);
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::GetRX_FIFO() const
{
	return rx_fifo.Empty() ? 0 : rx_fifo.ConstFront() & CONFIG::DATA_MASK;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::ReadRX_FIFO(uint8_t& value)
{
	if(rx_fifo.Empty())
	{
		logger << DebugWarning << "Reading an empty Rx FIFO returns an undefined result and generates a bus error" << EndDebugWarning;
		return false;
	}
	
	value = rx_fifo.Front() & CONFIG::DATA_MASK;
	rx_fifo.Pop();
	if(rx_fifo.Empty() && IsVerbose())
	{
		logger << DebugInfo << "Rx FIFO becomes empty" << EndDebugInfo;
	}
	return true;
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::GetSTAT_REG() const
{
	uint8_t v = stat_reg;
	
	if(!rx_fifo.Empty()) v = v | CONFIG::STAT_REG_RX_FIFO_VALID_DATA_MASK;
	if(rx_fifo.Full()) v = v | CONFIG::STAT_REG_RX_FIFO_FULL_MASK;
	if(tx_fifo.Empty()) v = v | CONFIG::STAT_REG_TX_FIFO_EMPTY_MASK;
	if(tx_fifo.Full()) v = v | CONFIG::STAT_REG_TX_FIFO_FULL_MASK;
	
	return v;
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::ReadSTAT_REG()
{
	uint8_t v = GetSTAT_REG();
	
	if(IsVerbose())
	{
		logger << DebugInfo << "STAT_REG: PARITY_ERROR FRAME_ERROR OVERRUN_ERROR INTR_ENABLED TX_FIFO_FULL TX_FIFO_EMPTY RX_FIFO_FULL RX_FIFO_VALID_DATA" << EndDebugInfo;
		logger << DebugInfo;
		logger << "                " << ((v & CONFIG::STAT_REG_PARITY_ERROR_MASK) ? '1' : '0');
		logger << "            " << ((v & CONFIG::STAT_REG_FRAME_ERROR_MASK) ? '1' : '0');
		logger << "           " << ((v & CONFIG::STAT_REG_OVERRUN_ERROR_MASK) ? '1' : '0');
		logger << "              " << ((v & CONFIG::STAT_REG_INTR_ENABLED_MASK) ? '1' : '0');
		logger << "            " << ((v & CONFIG::STAT_REG_TX_FIFO_FULL_MASK) ? '1' : '0');
		logger << "            " << ((v & CONFIG::STAT_REG_TX_FIFO_EMPTY_MASK) ? '1' : '0');
		logger << "            " << ((v & CONFIG::STAT_REG_RX_FIFO_FULL_MASK) ? '1' : '0');
		logger << "               " << ((v & CONFIG::STAT_REG_RX_FIFO_VALID_DATA_MASK) ? '1' : '0');
		logger << EndDebugInfo;
	}
	
	// reading STAT_REG resets Parity error, Frame error, and overrun error bits
	stat_reg = stat_reg & ~(CONFIG::STAT_REG_PARITY_ERROR_MASK | CONFIG::STAT_REG_FRAME_ERROR_MASK | CONFIG::STAT_REG_OVERRUN_ERROR_MASK);
	
	return v;
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::GetSTAT_REG_RX_FIFO_VALID_DATA() const
{
	return rx_fifo.Empty() ? 0 : 1;
}

template <class CONFIG>
uint8_t XPS_UARTLite<CONFIG>::GetSTAT_REG_INTR_ENABLED() const
{
	return (stat_reg & CONFIG::STAT_REG_INTR_ENABLED_MASK) >> CONFIG::STAT_REG_INTR_ENABLED_OFFSET;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::TX_FIFO_BecomesEmpty() const
{
	return tx_fifo_becomes_empty;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::ResetTX_FIFO_BecomesEmpty()
{
	tx_fifo_becomes_empty = false;
}

template <class CONFIG>
bool XPS_UARTLite<CONFIG>::HasInterrupt() const
{
	return GetSTAT_REG_INTR_ENABLED() && (GetSTAT_REG_RX_FIFO_VALID_DATA() || TX_FIFO_BecomesEmpty());
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::SetTX_FIFO(uint8_t value)
{
	if(tx_fifo.Empty()) return;
	
	tx_fifo.Front() = value & CONFIG::DATA_MASK;
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::WriteTX_FIFO(uint8_t value)
{
	if(tx_fifo.Full())
	{
		logger << DebugWarning << "Tx FIFO overflow" << EndDebugWarning;
		return;
	}
	
	value = value & CONFIG::DATA_MASK;
	tx_fifo.Push(value);
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::WriteCTRL_REG(uint8_t value)
{
	if(value & CONFIG::CTRL_REG_ENABLE_INTR_MASK)
	{
		if(IsVerbose())
		{
			logger << DebugInfo << "Enabling interrupt" << EndDebugInfo;
		}
		stat_reg = stat_reg | CONFIG::STAT_REG_INTR_ENABLED_MASK;
	}
	
	if(value & CONFIG::CTRL_REG_RST_RX_FIFO_MASK)
	{
		if(IsVerbose())
		{
			logger << DebugInfo << "Resetting Rx fifo" << EndDebugInfo;
		}
		rx_fifo.Clear();
	}

	if(value & CONFIG::CTRL_REG_RST_TX_FIFO_MASK)
	{
		if(IsVerbose())
		{
			logger << DebugInfo << "Resetting Tx fifo" << EndDebugInfo;
		}
		tx_fifo.Clear();
	}
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::TelnetProcessInput()
{
	if(char_io_import)
	{
		char c;
		uint8_t v;
		
		if(!rx_fifo.Full())
		{
			do
			{
				if(!char_io_import->GetChar(c)) break;
				
				v = (uint8_t) c;
				if(IsVerbose())
				{
					logger << DebugInfo << "Receiving ";
					if(v >= 32)
						logger << "character '" << c << "'";
					else
						logger << "control character 0x" << std::hex << (unsigned int) v << std::dec;
					logger << " from telnet client" << EndDebugInfo;
				}

				rx_fifo.Push(v);
			}
			while(!rx_fifo.Full());
		}
	}
}

template <class CONFIG>
void XPS_UARTLite<CONFIG>::TelnetProcessOutput(bool flush_telnet_output)
{
	if(char_io_import)
	{
		char c;
		uint8_t v;
		
		if(!tx_fifo.Empty())
		{
			do
			{
				v = tx_fifo.Front();
				tx_fifo.Pop();
				c = (char) v;
				if(IsVerbose())
				{
					logger << DebugInfo << "Sending ";
					if(v >= 32)
						logger << "character '" << c << "'";
					else
						logger << "control character 0x" << std::hex << (unsigned int) v << std::dec;
					logger << " to telnet client" << EndDebugInfo;
				}
				char_io_import->PutChar(c);
			}
			while(!tx_fifo.Empty());
			
			tx_fifo_becomes_empty = true;
		}
		
		if(flush_telnet_output)
		{
			char_io_import->FlushChars();
		}
	}
	
}

} // end of namespace xps_uart_lite
} // end of namespace xilinx
} // end of namespace com
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
