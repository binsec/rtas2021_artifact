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
 
#ifndef __VIRTEX5FXT_CONFIG_HH__
#define __VIRTEX5FXT_CONFIG_HH__

#include <unisim/component/cxx/interrupt/xilinx/xps_intc/config.hh>
#include <unisim/component/cxx/timer/xilinx/xps_timer/config.hh>
#include <unisim/component/tlm2/interconnect/generic_router/config.hh>
#include <unisim/component/cxx/memory/flash/am29/s29gl256p_config.hh>
#include <unisim/component/cxx/interconnect/xilinx/dcr_controller/config.hh>
#include <unisim/component/cxx/interconnect/xilinx/crossbar/config.hh>
#include <unisim/component/cxx/interconnect/xilinx/mci/config.hh>
#include <unisim/component/cxx/com/xilinx/xps_uart_lite/config.hh>
#include <unisim/component/cxx/com/xilinx/xps_gpio/config.hh>

//=========================================================================
//===                 Simulator config                                  ===
//=========================================================================

class Config
{
public:
	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================
	static const bool DEBUG_ENABLE = true;
	typedef uint64_t MEMORY_ADDR;
	static const bool DEBUG_INFORMATION = false;
	
	// Front Side Bus template parameters
	static const unsigned int FSB_BURST_SIZE = 32;
	typedef uint32_t CPU_ADDRESS_TYPE;
	typedef uint64_t FSB_ADDRESS_TYPE;

	struct DEBUGGER_CONFIG
	{
		typedef CPU_ADDRESS_TYPE ADDRESS;
		static const unsigned int NUM_PROCESSORS = 1;
		/* gdb_server, inline_debugger, profiler, user_interface */
		static const unsigned int MAX_FRONT_ENDS = 4;
	};
	
	//=========================================================================
	//===                 MPLB compile time configuration                   ===
	//=========================================================================

	class MPLB_CONFIG : public unisim::component::tlm2::interconnect::generic_router::Config
	{
	public:
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 9;
		static const unsigned int NUM_MAPPINGS = 9;
		static const unsigned int OUTPUT_BUSWIDTH = 128;
		static const unsigned int INPUT_BUSWIDTH = 128;
		static const bool VERBOSE = DEBUG_ENABLE;
	};

	//=========================================================================
	//===                 MPLB compile time configuration                   ===
	//=========================================================================

	class MEMORY_ROUTER_CONFIG : public unisim::component::tlm2::interconnect::generic_router::Config
	{
	public:
		static const unsigned int INPUT_SOCKETS = 3;
		static const unsigned int OUTPUT_SOCKETS = 1;
		static const unsigned int NUM_MAPPINGS = 1;
		static const unsigned int OUTPUT_BUSWIDTH = 128;
		static const unsigned int INPUT_BUSWIDTH = 128;
		static const bool VERBOSE = DEBUG_ENABLE;
	};

	//=========================================================================
	//===                intc compile time configuration                    ===
	//=========================================================================

	class INTC_CONFIG : public unisim::component::cxx::interrupt::xilinx::xps_intc::Config
	{
	public:
		static const MEMORY_ADDR C_BASEADDR = 0x81800000ULL; // XPS INTC Base Address default value (as ML507 board)
		static const MEMORY_ADDR C_HIGHADDR = 0x8180ffffULL; // XPS INTC Base Address default value (as ML507 board)
		static const unsigned int MPLB_PORT = 0;
	};

	//=========================================================================
	//===                timer compile time configuration                   ===
	//=========================================================================

	class TIMER_CONFIG : public unisim::component::cxx::timer::xilinx::xps_timer::Config
	{
	public:
		static const unsigned int C_COUNT_WIDTH = 32;        // The width in bits of the counters in the XPS Timer/Counter
		static const MEMORY_ADDR C_BASEADDR = 0x83c00000ULL; // XPS Timer/Counter Base Address default value (as ML507 board)
		static const MEMORY_ADDR C_HIGHADDR = 0x83c0ffffULL; // XPS Timer/Counter High Address default value (as ML507 board)
		
		// Optional features
		static const bool C_ONE_TIMER_ONLY = false;  // Number of Timer modules
		static const bool C_TRIG0_ASSERT = true;     // Assertion level for CaptureTrig0
		static const bool C_TRIG1_ASSERT = true;     // Assertion level for CaptureTrig1
		static const bool C_GEN0_ASSERT = true;      // Assertion level for GenerateOut0
		static const bool C_GEN1_ASSERT = true;      // Assertion level for GenerateOut1

		static const unsigned int MPLB_PORT = 1;
	};

	//=========================================================================
	//===              UART Lite compile time configuration                 ===
	//=========================================================================

	class UART_LITE_CONFIG : public unisim::component::cxx::com::xilinx::xps_uart_lite::Config
	{
	public:
		static const MEMORY_ADDR C_BASEADDR = 0x84000000ULL; // XPS UART Lite Base Address default value
		static const MEMORY_ADDR C_HIGHADDR = 0x8400ffffULL; // XPS UART Lite High Address default value
		
		static const unsigned int C_BAUDRATE = 9600;
		static const unsigned int C_DATA_BITS = 8;
		static const bool C_USE_PARITY = true;
		static const bool C_ODD_PARITY = true;
		
		static const unsigned int RX_FIFO_SIZE = 16;
		static const unsigned int TX_FIFO_SIZE = 16;

		static const unsigned int MPLB_PORT = 4;
	};

	//=========================================================================
	//===                 GPIOs compile time configuration                  ===
	//=========================================================================

	class GPIO_DIP_SWITCHES_8BIT_CONFIG : public unisim::component::cxx::com::xilinx::xps_gpio::Config
	{
	public:
		static const unsigned int C_GPIO_WIDTH = 8;          // The width in bits of GPIO Channel 1
		static const MEMORY_ADDR C_BASEADDR = 0x81460000ULL; // XPS GPIO Base Address default value
		static const MEMORY_ADDR C_HIGHADDR = 0x8146ffffULL; // XPS GPIO High Address default value
		
		// Optional features
		static const bool C_INTERRUPT_IS_PRESENT = true; // Whether interrupt is present or not

		static const unsigned int MPLB_PORT = 5;
	};

	class GPIO_LEDS_8BIT_CONFIG : public unisim::component::cxx::com::xilinx::xps_gpio::Config
	{
	public:
		static const unsigned int C_GPIO_WIDTH = 8;          // The width in bits of GPIO Channel 1
		static const MEMORY_ADDR C_BASEADDR = 0x81400000ULL; // XPS GPIO Base Address default value
		static const MEMORY_ADDR C_HIGHADDR = 0x8140ffffULL; // XPS GPIO High Address default value

		static const unsigned int MPLB_PORT = 6;
	};

	class GPIO_5_LEDS_POSITIONS_CONFIG : public unisim::component::cxx::com::xilinx::xps_gpio::Config
	{
	public:
		static const unsigned int C_GPIO_WIDTH = 5;          // The width in bits of GPIO Channel 1
		static const MEMORY_ADDR C_BASEADDR = 0x81420000ULL; // XPS GPIO Base Address default value
		static const MEMORY_ADDR C_HIGHADDR = 0x8142ffffULL; // XPS GPIO High Address default value

		static const unsigned int MPLB_PORT = 7;
	};

	class GPIO_PUSH_BUTTONS_5BIT_CONFIG : public unisim::component::cxx::com::xilinx::xps_gpio::Config
	{
	public:
		static const unsigned int C_GPIO_WIDTH = 5;          // The width in bits of GPIO Channel 1
		static const MEMORY_ADDR C_BASEADDR = 0x81440000ULL; // XPS GPIO Base Address default value
		static const MEMORY_ADDR C_HIGHADDR = 0x8144ffffULL; // XPS GPIO High Address default value
		
		// Optional features
		static const bool C_INTERRUPT_IS_PRESENT = true; // Whether interrupt is present or not

		static const unsigned int MPLB_PORT = 8;
	};

	//=========================================================================
	//===                 AM29 compile time configuration                   ===
	//=========================================================================

	typedef unisim::component::cxx::memory::flash::am29::S29GL256PConfig AM29_CONFIG;
	static const MEMORY_ADDR FLASH_BASE_ADDR = 0xfc000000ULL;
	static const MEMORY_ADDR FLASH_BYTE_SIZE = 32 * 1024 * 1024; // 32 MB
	static const unsigned int FLASH_MPLB_PORT = 2;

	//=========================================================================
	//===                 BRAM compile time configuration                   ===
	//=========================================================================

	static const MEMORY_ADDR BRAM_BASE_ADDR = 0xfffc0000ULL;
	static const MEMORY_ADDR BRAM_BYTE_SIZE = 256 * 1024; // 256 KB
	static const unsigned int BRAM_MPLB_PORT = 3;

	//=========================================================================
	//===                 RAM compile time configuration                    ===
	//=========================================================================

	static const MEMORY_ADDR RAM_BASE_ADDR = 0x00000000ULL;
	static const MEMORY_ADDR RAM_BYTE_SIZE = 256 * 1024 * 1024; // 256 MB

	//=========================================================================
	//===           DCR controller compile time configuration               ===
	//=========================================================================

	class DCR_CONTROLLER_CONFIG : public unisim::component::cxx::interconnect::xilinx::dcr_controller::Config
	{
	public:
		static const uint8_t TIEDCRBASEADDR = 0x0;      // 2-bit tie-off-pins
		
		static const bool DCR_AUTOLOCK_ENABLE = true;   // enable autolock
	};

	//=========================================================================
	//===              crossbar compile time configuration                  ===
	//=========================================================================

	class CROSSBAR_CONFIG : public unisim::component::cxx::interconnect::xilinx::crossbar::Config
	{
	public:
		// interrupt masking at reset
		static const uint32_t IMASK_RESET_VALUE = 0xffffffffUL; // all 1's
		
		// Crossbar mapping at reset
		static const uint32_t TMPL0_XBAR_MAP_RESET_VALUE = 0xffff0000UL; // MCI: below 2 GB, MPLB: above 2 GB
		static const uint32_t TMPL1_XBAR_MAP_RESET_VALUE = 0x0UL;
		static const uint32_t TMPL2_XBAR_MAP_RESET_VALUE = 0x0UL;
		static const uint32_t TMPL3_XBAR_MAP_RESET_VALUE = 0x0UL;

		static const uint32_t TMPL0_PLBS0_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
		static const uint32_t TMPL1_PLBS0_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
		static const uint32_t TMPL2_PLBS0_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
		static const uint32_t TMPL3_PLBS0_MAP_RESET_VALUE = 0xffffffffUL; // all 1's

		static const uint32_t TMPL0_PLBS1_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
		static const uint32_t TMPL1_PLBS1_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
		static const uint32_t TMPL2_PLBS1_MAP_RESET_VALUE = 0xffffffffUL; // all 1's
		static const uint32_t TMPL3_PLBS1_MAP_RESET_VALUE = 0xffffffffUL; // all 1's

		static const uint32_t TMPL_SEL_REG_RESET_VALUE = 0x3fffffffUL;
		
		// status registers at reset
		static const uint32_t SM_ST_XBC_RESET_VALUE = 0x0UL;
		static const uint32_t SM_ST_PLBS0_RESET_VALUE = 0x0UL;
		static const uint32_t SM_ST_PLBS1_RESET_VALUE = 0x0UL;
		static const uint32_t SM_ST_PLBM_RESET_VALUE = 0x0UL;
		
		// sniff registers at reset
		static const uint32_t CMD_SNIFFA_PLBS0_RESET_VALUE = 0x0UL;
		static const uint32_t CMD_SNIFFA_PLBS1_RESET_VALUE = 0x0UL;
		static const uint32_t CMD_SNIFFA_PLBM_RESET_VALUE = 0x0UL;
	};

	//=========================================================================
	//===                 MCI compile time configuration                    ===
	//=========================================================================

	typedef unisim::component::cxx::interconnect::xilinx::mci::Config MCI_CONFIG;

	//=========================================================================
	//===                            IRQ mapping                            ===
	//=========================================================================

	static const unsigned int TIMER_IRQ = 3;
	static const unsigned int UART_LITE_IRQ = 2;
	static const unsigned int GPIO_DIP_SWITCHES_8BIT_IRQ = 7;
	static const unsigned int GPIO_PUSH_BUTTONS_5BIT_IRQ = 8;
};

#endif
