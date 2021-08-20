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

#ifndef __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_UART_LITE_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_UART_LITE_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace com {
namespace xilinx {
namespace xps_uart_lite {

class AddressMap
{
public:
	static const unsigned int RX_FIFO = 0x0;
	static const unsigned int TX_FIFO = 0x4;
	static const unsigned int STAT_REG = 0x8;
	static const unsigned int CTRL_REG = 0xc;
};

class STAT_REG_Layout
{
public:
	static const unsigned int STAT_REG_PARITY_ERROR_OFFSET = 7;
	static const unsigned int STAT_REG_FRAME_ERROR_OFFSET = 6;
	static const unsigned int STAT_REG_OVERRUN_ERROR_OFFSET = 5;
	static const unsigned int STAT_REG_INTR_ENABLED_OFFSET = 4;
	static const unsigned int STAT_REG_TX_FIFO_FULL_OFFSET = 3;
	static const unsigned int STAT_REG_TX_FIFO_EMPTY_OFFSET = 2;
	static const unsigned int STAT_REG_RX_FIFO_FULL_OFFSET = 1;
	static const unsigned int STAT_REG_RX_FIFO_VALID_DATA_OFFSET = 0;

	static const unsigned int STAT_REG_PARITY_ERROR_BITSIZE = 1;
	static const unsigned int STAT_REG_FRAME_ERROR_BITSIZE = 1;
	static const unsigned int STAT_REG_OVERRUN_ERROR_BITSIZE = 1;
	static const unsigned int STAT_REG_INTR_ENABLED_BITSIZE = 1;
	static const unsigned int STAT_REG_TX_FIFO_FULL_BITSIZE = 1;
	static const unsigned int STAT_REG_TX_FIFO_EMPTY_BITSIZE = 1;
	static const unsigned int STAT_REG_RX_FIFO_FULL_BITSIZE = 1;
	static const unsigned int STAT_REG_RX_FIFO_VALID_DATA_BITSIZE = 1;

	static const uint8_t STAT_REG_PARITY_ERROR_MASK = ((1UL << STAT_REG_PARITY_ERROR_BITSIZE) - 1) << STAT_REG_PARITY_ERROR_OFFSET;
	static const uint8_t STAT_REG_FRAME_ERROR_MASK = ((1UL << STAT_REG_FRAME_ERROR_BITSIZE) - 1) << STAT_REG_FRAME_ERROR_OFFSET;
	static const uint8_t STAT_REG_OVERRUN_ERROR_MASK = ((1UL << STAT_REG_OVERRUN_ERROR_BITSIZE) - 1) << STAT_REG_OVERRUN_ERROR_OFFSET;
	static const uint8_t STAT_REG_INTR_ENABLED_MASK = ((1UL << STAT_REG_INTR_ENABLED_BITSIZE) - 1) << STAT_REG_INTR_ENABLED_OFFSET;
	static const uint8_t STAT_REG_TX_FIFO_FULL_MASK = ((1UL << STAT_REG_TX_FIFO_FULL_BITSIZE) - 1) << STAT_REG_TX_FIFO_FULL_OFFSET;
	static const uint8_t STAT_REG_TX_FIFO_EMPTY_MASK = ((1UL << STAT_REG_TX_FIFO_EMPTY_BITSIZE) - 1) << STAT_REG_TX_FIFO_EMPTY_OFFSET;
	static const uint8_t STAT_REG_RX_FIFO_FULL_MASK = ((1UL << STAT_REG_RX_FIFO_FULL_BITSIZE) - 1) << STAT_REG_RX_FIFO_FULL_OFFSET;
	static const uint8_t STAT_REG_RX_FIFO_VALID_DATA_MASK = ((1UL << STAT_REG_RX_FIFO_VALID_DATA_BITSIZE) - 1) << STAT_REG_RX_FIFO_VALID_DATA_OFFSET;
	
	static const uint8_t STAT_REG_MASK = STAT_REG_PARITY_ERROR_MASK | STAT_REG_FRAME_ERROR_MASK | STAT_REG_OVERRUN_ERROR_MASK |
	                                      STAT_REG_INTR_ENABLED_MASK | STAT_REG_TX_FIFO_FULL_MASK | STAT_REG_TX_FIFO_EMPTY_MASK |
	                                      STAT_REG_RX_FIFO_FULL_MASK | STAT_REG_RX_FIFO_VALID_DATA_MASK;
  
	static const uint8_t STAT_REG_RESET_VALUE = 0x4UL;
};

class CTRL_REG_Layout
{
public:
	static const unsigned int CTRL_REG_ENABLE_INTR_OFFSET = 4;
	static const unsigned int CTRL_REG_RST_RX_FIFO_OFFSET = 1;
	static const unsigned int CTRL_REG_RST_TX_FIFO_OFFSET = 0;

	static const unsigned int CTRL_REG_ENABLE_INTR_BITSIZE = 1;
	static const unsigned int CTRL_REG_RST_RX_FIFO_BITSIZE = 1;
	static const unsigned int CTRL_REG_RST_TX_FIFO_BITSIZE = 1;

	static const uint8_t CTRL_REG_ENABLE_INTR_MASK = ((1UL << CTRL_REG_ENABLE_INTR_BITSIZE) - 1) << CTRL_REG_ENABLE_INTR_OFFSET;
	static const uint8_t CTRL_REG_RST_RX_FIFO_MASK = ((1UL << CTRL_REG_RST_RX_FIFO_BITSIZE) - 1) << CTRL_REG_RST_RX_FIFO_OFFSET;
	static const uint8_t CTRL_REG_RST_TX_FIFO_MASK = ((1UL << CTRL_REG_RST_TX_FIFO_BITSIZE) - 1) << CTRL_REG_RST_TX_FIFO_OFFSET;
	
	static const uint8_t CTRL_REG_MASK = CTRL_REG_ENABLE_INTR_MASK | CTRL_REG_RST_RX_FIFO_MASK | CTRL_REG_RST_TX_FIFO_MASK;
};

class Config
	: public AddressMap
	, public STAT_REG_Layout
	, public CTRL_REG_Layout
{
public:
	typedef uint64_t MEMORY_ADDR;
	
	static const unsigned int C_SPLB_DWITH = 128;        // PLB data with (in bits)
	static const MEMORY_ADDR C_BASEADDR = 0x84000000ULL; // XPS UART Lite Base Address default value
	static const MEMORY_ADDR C_HIGHADDR = 0x8400ffffULL; // XPS UART Lite High Address default value
	
	static const unsigned int C_BAUDRATE = 9600;
	static const unsigned int C_DATA_BITS = 8;
	static const bool C_USE_PARITY = true;
	static const bool C_ODD_PARITY = true;
	
	class RX_FIFO_CONFIG
	{
	public:
		static const bool DEBUG = false;
		typedef uint8_t ELEMENT;
		static const unsigned int SIZE = 16;
	};

	class TX_FIFO_CONFIG
	{
	public:
		static const bool DEBUG = false;
		typedef uint8_t ELEMENT;
		static const unsigned int SIZE = 16;
	};
	
	static const unsigned int TELNET_MAX_BUFFER_SIZE = 256;
	
	static const uint8_t DATA_MASK = (1 << C_DATA_BITS) - 1;
};

} // end of namespace xps_uart_lite
} // end of namespace xilinx
} // end of namespace com
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
