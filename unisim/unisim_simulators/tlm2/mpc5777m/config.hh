/*
 *  Copyright (c) 2018,
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

#ifndef __MPC5777M_CONFIG_HH__
#define __MPC5777M_CONFIG_HH__

#include <unisim/component/tlm2/interconnect/generic_router/config.hh>
#include <unisim/component/tlm2/interconnect/programmable_router/config.hh>
#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar/config.hh>
#include <unisim/component/tlm2/interconnect/freescale/mpc57xx/ebi/config.hh>
#include <unisim/component/tlm2/com/freescale/mpc57xx/siul2/defs.hh>
#include <stdint.h>

struct Config
{
	//=========================================================================
	//===                       Constants definitions                       ===
	//=========================================================================
	
	static const bool DEBUG_ENABLE = true;


	static const unsigned int NUM_PROCESSORS = 3;
	static const unsigned int MAX_FRONT_ENDS = NUM_PROCESSORS // for gdb-server
	                                         + NUM_PROCESSORS // for inline-debugger
	                                         + NUM_PROCESSORS // for profiler
	                                         ;
	static const unsigned int NUM_DMA_CHANNELS  = 128;
	static const unsigned int NUM_DMA_TRIGGERS  = 8;
	static const unsigned int NUM_DMA_ALWAYS_ON = 64;
	
	// Front Side Bus template parameters
	//static const unsigned int FSB_WIDTH = 8;
	static const unsigned int FSB_BURST_SIZE = 32;
	typedef uint32_t CPU_ADDRESS_TYPE;
	typedef uint32_t FSB_ADDRESS_TYPE;
	typedef uint32_t CPU_REG_TYPE;
	
	struct DEBUGGER_CONFIG
	{
		typedef CPU_ADDRESS_TYPE ADDRESS;
		static const unsigned int NUM_PROCESSORS = Config::NUM_PROCESSORS;
		static const unsigned int MAX_FRONT_ENDS = Config::MAX_FRONT_ENDS;
	};

	struct XBAR_0_CONFIG : unisim::component::tlm2::interconnect::freescale::mpc57xx::xbar::Config
	{
		typedef uint32_t ADDRESS;
		static const unsigned int INPUT_SOCKETS = 6;
		static const unsigned int OUTPUT_SOCKETS = 8;
		static const unsigned int NUM_MAPPINGS = 12;
		static const unsigned int INPUT_BUSWIDTH = 64;
		static const unsigned int OUTPUT_BUSWIDTH = 64;
		static const unsigned int PERIPHERAL_BUSWIDTH = 32;
		static const bool VERBOSE = DEBUG_ENABLE;
		static const unsigned int BURST_LENGTH = FSB_BURST_SIZE / (INPUT_BUSWIDTH / 8);
		static const uint32_t XBAR_PRS_RESET_VALUE = 0x00301425;
		static const uint32_t XBAR_CRS_RESET_VALUE = 0x003f0100;
		static const unsigned int NUM_REGION_DESCRIPTORS = 12;
		static const unsigned int NUM_BUS_MASTERS = 16;
	};
	
	struct XBAR_1_CONFIG : unisim::component::tlm2::interconnect::freescale::mpc57xx::xbar::Config
	{
		typedef uint32_t ADDRESS;
		static const unsigned int INPUT_SOCKETS = 4;
		static const unsigned int OUTPUT_SOCKETS = 4;
		static const unsigned int NUM_MAPPINGS = 13;
		static const unsigned int INPUT_BUSWIDTH = 32;
		static const unsigned int OUTPUT_BUSWIDTH = 32;
		static const unsigned int PERIPHERAL_BUSWIDTH = 32;
		static const bool VERBOSE = DEBUG_ENABLE;
		static const unsigned int BURST_LENGTH = FSB_BURST_SIZE / (INPUT_BUSWIDTH / 8);
		static const uint32_t XBAR_PRS_RESET_VALUE = 0x00003102;
		static const uint32_t XBAR_CRS_RESET_VALUE = 0x000f0100;
		static const unsigned int NUM_REGION_DESCRIPTORS = 8;
		static const unsigned int NUM_BUS_MASTERS = 16;
	};
	
	struct PBRIDGE_A_CONFIG : unisim::component::tlm2::interconnect::programmable_router::Config
	{
		typedef uint32_t ADDRESS;
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 50;
		static const unsigned int NUM_MAPPINGS = 51;
		static const unsigned int INPUT_BUSWIDTH = 32;
		static const unsigned int OUTPUT_BUSWIDTH = 32;
		static const unsigned int PERIPHERAL_BUSWIDTH = 32;
		static const bool VERBOSE = DEBUG_ENABLE;
		static const unsigned int BURST_LENGTH = FSB_BURST_SIZE / (INPUT_BUSWIDTH / 8);
	};
	
	struct PBRIDGE_B_CONFIG : unisim::component::tlm2::interconnect::programmable_router::Config
	{
		typedef uint32_t ADDRESS;
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 6;
		static const unsigned int NUM_MAPPINGS = 6;
		static const unsigned int INPUT_BUSWIDTH = 32;
		static const unsigned int OUTPUT_BUSWIDTH = 32;
		static const unsigned int PERIPHERAL_BUSWIDTH = 32;
		static const bool VERBOSE = DEBUG_ENABLE;
		static const unsigned int BURST_LENGTH = FSB_BURST_SIZE / (INPUT_BUSWIDTH / 8);
	};
	
	struct EBI_CONFIG : unisim::component::tlm2::interconnect::freescale::mpc57xx::ebi::Config
	{
		typedef uint32_t ADDRESS;
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 3;
		static const unsigned int NUM_MAPPINGS = 0;
		static const unsigned int INPUT_BUSWIDTH = 64;
		static const unsigned int OUTPUT_BUSWIDTH = 32;
		static const unsigned int PERIPHERAL_BUSWIDTH = 32;
		static const bool VERBOSE = DEBUG_ENABLE;
		static const unsigned int BURST_LENGTH = FSB_BURST_SIZE / (INPUT_BUSWIDTH / 8);
	};
	
	struct XBAR_1_M1_CONCENTRATOR_CONFIG : unisim::component::tlm2::interconnect::generic_router::Config
	{
		typedef uint32_t ADDRESS;
		static const unsigned int INPUT_SOCKETS = 2;
		static const unsigned int OUTPUT_SOCKETS = 1;
		static const unsigned int NUM_MAPPINGS = 1;
		static const unsigned int INPUT_BUSWIDTH = 32;
		static const unsigned int OUTPUT_BUSWIDTH = 32;
		static const bool VERBOSE = DEBUG_ENABLE;
		static const unsigned int BURST_LENGTH = FSB_BURST_SIZE / (INPUT_BUSWIDTH / 8);
	};
	
	struct IAHBG_0_CONFIG : unisim::component::tlm2::interconnect::generic_router::Config
	{
		typedef uint32_t ADDRESS;
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 1;
		static const unsigned int NUM_MAPPINGS = 1;
		static const unsigned int INPUT_BUSWIDTH = 64;
		static const unsigned int OUTPUT_BUSWIDTH = 32;
		static const bool VERBOSE = DEBUG_ENABLE;
		static const unsigned int BURST_LENGTH = FSB_BURST_SIZE / (INPUT_BUSWIDTH / 8);
	};

	struct IAHBG_1_CONFIG : unisim::component::tlm2::interconnect::generic_router::Config
	{
		typedef uint32_t ADDRESS;
		static const unsigned int INPUT_SOCKETS = 1;
		static const unsigned int OUTPUT_SOCKETS = 1;
		static const unsigned int NUM_MAPPINGS = 1;
		static const unsigned int INPUT_BUSWIDTH = 32;
		static const unsigned int OUTPUT_BUSWIDTH = 64;
		static const bool VERBOSE = DEBUG_ENABLE;
		static const unsigned int BURST_LENGTH = FSB_BURST_SIZE / (INPUT_BUSWIDTH / 8);
	};
	
	struct INTC_0_CONFIG
	{
		static const unsigned int NUM_PROCESSORS = Config::NUM_PROCESSORS;
		static const unsigned int NUM_HW_IRQS = 965 - 32;
		static const unsigned int BUSWIDTH = 32;
		static const unsigned int VOFFSET_WIDTH = 14;
	};
	
	struct STM_0_CONFIG
	{
		static const unsigned int NUM_CHANNELS = 4;
		static const unsigned int BUSWIDTH = 32;
	};

	struct STM_1_CONFIG
	{
		static const unsigned int NUM_CHANNELS = 4;
		static const unsigned int BUSWIDTH = 32;
	};

	struct STM_2_CONFIG
	{
		static const unsigned int NUM_CHANNELS = 4;
		static const unsigned int BUSWIDTH = 32;
	};

	struct SWT_0_CONFIG
	{
		static const unsigned int NUM_MASTERS = 8; // FIXME: probably 4
		static const unsigned int BUSWIDTH = 32;
	};

	struct SWT_1_CONFIG
	{
		static const unsigned int NUM_MASTERS = 8; // FIXME: probably 4
		static const unsigned int BUSWIDTH = 32;
	};

	struct SWT_2_CONFIG
	{
		static const unsigned int NUM_MASTERS = 8; // FIXME: probably 4
		static const unsigned int BUSWIDTH = 32;
	};
	
	struct SWT_3_CONFIG
	{
		static const unsigned int NUM_MASTERS = 8; // FIXME: probably 4
		static const unsigned int BUSWIDTH = 32;
	};
	
	struct PIT_0_CONFIG
	{
		static const unsigned int MAX_CHANNELS = 8;
		static const unsigned int NUM_CHANNELS = 8;
		static const bool HAS_RTI_SUPPORT = true;
		static const bool HAS_DMA_SUPPORT = true;
		static const bool HAS_64_BIT_TIMER_SUPPORT = false;
		static const unsigned int BUSWIDTH = 32;
	};

	struct PIT_1_CONFIG
	{
		static const unsigned int MAX_CHANNELS = 8;
		static const unsigned int NUM_CHANNELS = 2;
		static const bool HAS_RTI_SUPPORT = false;
		static const bool HAS_DMA_SUPPORT = false;
		static const bool HAS_64_BIT_TIMER_SUPPORT = true;
		static const unsigned int BUSWIDTH = 32;
	};
	
	struct LINFlexD_0_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 16;
		static const bool GENERIC_SLAVE = true;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = true;
		static const unsigned int BUSWIDTH = 32;
	};

	struct LINFlexD_1_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 32;
	};

	struct LINFlexD_2_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 32;
	};

	struct LINFlexD_14_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 32;
	};

	struct LINFlexD_15_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 32;
	};

	struct LINFlexD_16_CONFIG
	{
		static const unsigned int NUM_IRQS = 3;
		static const unsigned int TX_CH_NUM = 0;
		static const unsigned int RX_CH_NUM = 0;
		static const unsigned int NUM_FILTERS = 0;
		static const bool GENERIC_SLAVE = false;
		static const bool GENERIC_PSI5 = false;
		static const bool HAS_AUTO_SYNCHRONIZATION_SUPPORT = false;
		static const unsigned int BUSWIDTH = 32;
	};

	struct DMAMUX_0_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 8;  // DMA Channels 0 - 7
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 20
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 32;
	};
	
	struct DMAMUX_1_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 8;  // DMA Channels 8 - 15
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 57
		static const unsigned int NUM_DMA_ALWAYS_ON = 5;  // Always 59 - 63
		static const unsigned int NUM_DMA_TRIGGERS  = 5;  // PIT_0 Triggers 0 - 4
		static const unsigned int BUSWIDTH          = 32;
	};

	struct DMAMUX_2_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 8;  // DMA Channels 16 - 23
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 48
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 1;  // PIT_0 Trigger 5
		static const unsigned int BUSWIDTH          = 32;
	};

	struct DMAMUX_3_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 8;  // DMA Channels 24 - 31
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 49
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 32;
	};
	
	struct DMAMUX_4_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 32 - 47
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 41
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 1;  // PIT_0 Trigger 6
		static const unsigned int BUSWIDTH          = 32;
	};

	struct DMAMUX_5_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 48 - 63
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 41
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 1;  // PIT_0 Trigger 7
		static const unsigned int BUSWIDTH          = 32;
	};

	struct DMAMUX_6_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 64 - 79
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 47
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 32;
	};

	struct DMAMUX_7_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 80 - 95
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 50
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 32;
	};
	
	struct DMAMUX_8_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 96 - 111
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 45
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 32;
	};
	
	struct DMAMUX_9_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS  = 16; // DMA Channels 112 - 127
		static const unsigned int NUM_DMA_SOURCES   = 64; // Sources 1 - 43
		static const unsigned int NUM_DMA_ALWAYS_ON = 1;  // Always 63
		static const unsigned int NUM_DMA_TRIGGERS  = 0;  // No trigger
		static const unsigned int BUSWIDTH          = 32;
	};
	
	struct EDMA_0_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS = 64;
		static const unsigned int BUSWIDTH         = 32;
	};

	struct EDMA_1_CONFIG
	{
		static const unsigned int NUM_DMA_CHANNELS = 64;
		static const unsigned int BUSWIDTH         = 32;
	};
	
	struct DSPI_0_CONFIG
	{
		static const unsigned int BUSWIDTH               = 32;
		static const unsigned int NUM_CTARS              = 8;
		static const unsigned int TX_FIFO_DEPTH          = 4;
		static const unsigned int RX_FIFO_DEPTH          = 4;
		static const unsigned int CMD_FIFO_DEPTH         = 4;
		static const bool HAS_DATA_SERIALIZATION_SUPPORT = false;
		static const unsigned int NUM_DSI_INPUTS         = 0;
		static const unsigned int NUM_DSI_OUTPUTS        = 0;
	};

	struct DSPI_1_CONFIG
	{
		static const unsigned int BUSWIDTH               = 32;
		static const unsigned int NUM_CTARS              = 8;
		static const unsigned int TX_FIFO_DEPTH          = 4;
		static const unsigned int RX_FIFO_DEPTH          = 4;
		static const unsigned int CMD_FIFO_DEPTH         = 4;
		static const bool HAS_DATA_SERIALIZATION_SUPPORT = false;
		static const unsigned int NUM_DSI_INPUTS         = 0;
		static const unsigned int NUM_DSI_OUTPUTS        = 0;
	};

	struct DSPI_2_CONFIG
	{
		static const unsigned int BUSWIDTH               = 32;
		static const unsigned int NUM_CTARS              = 8;
		static const unsigned int TX_FIFO_DEPTH          = 4;
		static const unsigned int RX_FIFO_DEPTH          = 4;
		static const unsigned int CMD_FIFO_DEPTH         = 4;
		static const bool HAS_DATA_SERIALIZATION_SUPPORT = false;
		static const unsigned int NUM_DSI_INPUTS         = 0;
		static const unsigned int NUM_DSI_OUTPUTS        = 0;
	};

	struct DSPI_3_CONFIG
	{
		static const unsigned int BUSWIDTH               = 32;
		static const unsigned int NUM_CTARS              = 8;
		static const unsigned int TX_FIFO_DEPTH          = 4;
		static const unsigned int RX_FIFO_DEPTH          = 4;
		static const unsigned int CMD_FIFO_DEPTH         = 4;
		static const bool HAS_DATA_SERIALIZATION_SUPPORT = false;
		static const unsigned int NUM_DSI_INPUTS         = 0;
		static const unsigned int NUM_DSI_OUTPUTS        = 0;
	};

	struct DSPI_4_CONFIG
	{
		static const unsigned int BUSWIDTH               = 32;
		static const unsigned int NUM_CTARS              = 8;
		static const unsigned int TX_FIFO_DEPTH          = 4;
		static const unsigned int RX_FIFO_DEPTH          = 4;
		static const unsigned int CMD_FIFO_DEPTH         = 4;
		static const bool HAS_DATA_SERIALIZATION_SUPPORT = true;
		static const unsigned int NUM_DSI_INPUTS         = 32;
		static const unsigned int NUM_DSI_OUTPUTS        = 32;
	};

	struct DSPI_5_CONFIG
	{
		static const unsigned int BUSWIDTH               = 32;
		static const unsigned int NUM_CTARS              = 8;
		static const unsigned int TX_FIFO_DEPTH          = 4;
		static const unsigned int RX_FIFO_DEPTH          = 4;
		static const unsigned int CMD_FIFO_DEPTH         = 4;
		static const bool HAS_DATA_SERIALIZATION_SUPPORT = true;
		static const unsigned int NUM_DSI_INPUTS         = 32;
		static const unsigned int NUM_DSI_OUTPUTS        = 32;
	};

	struct DSPI_6_CONFIG
	{
		static const unsigned int BUSWIDTH               = 32;
		static const unsigned int NUM_CTARS              = 8;
		static const unsigned int TX_FIFO_DEPTH          = 4;
		static const unsigned int RX_FIFO_DEPTH          = 4;
		static const unsigned int CMD_FIFO_DEPTH         = 4;
		static const bool HAS_DATA_SERIALIZATION_SUPPORT = true;
		static const unsigned int NUM_DSI_INPUTS         = 32;
		static const unsigned int NUM_DSI_OUTPUTS        = 32;
	};

	struct DSPI_12_CONFIG
	{
		static const unsigned int BUSWIDTH               = 32;
		static const unsigned int NUM_CTARS              = 8;
		static const unsigned int TX_FIFO_DEPTH          = 4;
		static const unsigned int RX_FIFO_DEPTH          = 4;
		static const unsigned int CMD_FIFO_DEPTH         = 4;
		static const bool HAS_DATA_SERIALIZATION_SUPPORT = false;
		static const unsigned int NUM_DSI_INPUTS         = 0;
		static const unsigned int NUM_DSI_OUTPUTS        = 0;
	};
	
	struct SIUL2_CONFIG
	{
		static const unsigned int BUSWIDTH                               = 32;
		static const unsigned int NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS = 512;
		static const unsigned int NUM_PADS                               = 341;
		static const unsigned int NUM_INTERRUPT_FILTERS                  = 32;
		static const unsigned int PART_NUMBER                            = 0x5777;
		static const bool ENGINEERING_DEVICE                             = true;
		static const unisim::component::tlm2::com::freescale::mpc57xx::siul2::PACKAGE_SETTING_TYPE PACKAGE_SETTING = unisim::component::tlm2::com::freescale::mpc57xx::siul2::BGA512;
		static const unsigned int MAJOR_MASK                             = 1;
		static const unsigned int MINOR_MASK                             = 1;
		static const unisim::component::tlm2::com::freescale::mpc57xx::siul2::MANUFACTURER_TYPE MANUFACTURER = unisim::component::tlm2::com::freescale::mpc57xx::siul2::FREESCALE;
		static const unsigned int FLASH_SIZE                             = 8 * 1024 * 1024;
		static const char FAMILY_NUMBER                                  = 'M';
	};
	
	struct M_CAN_1_CONFIG
	{
		// Core Release 3.0.1, May 6th, 2013
		static const unsigned int CORE_RELEASE         = 3;
		static const unsigned int CORE_RELEASE_STEP    = 0;
		static const unsigned int CORE_RELEASE_SUBSTEP = 1;
		static const unsigned int CORE_RELEASE_YEAR    = 3;
		static const unsigned int CORE_RELEASE_MONTH   = 5;
		static const unsigned int CORE_RELEASE_DAY     = 6;
		static const unsigned int BUSWIDTH             = 32;
	};
	
	struct M_CAN_2_CONFIG
	{
		// Core Release 3.0.1, May 6th, 2013
		static const unsigned int CORE_RELEASE         = 3;
		static const unsigned int CORE_RELEASE_STEP    = 0;
		static const unsigned int CORE_RELEASE_SUBSTEP = 1;
		static const unsigned int CORE_RELEASE_YEAR    = 3;
		static const unsigned int CORE_RELEASE_MONTH   = 5;
		static const unsigned int CORE_RELEASE_DAY     = 6;
		static const unsigned int BUSWIDTH             = 32;
	};
	
	struct M_CAN_3_CONFIG
	{
		// Core Release 3.0.1, May 6th, 2013
		static const unsigned int CORE_RELEASE         = 3;
		static const unsigned int CORE_RELEASE_STEP    = 0;
		static const unsigned int CORE_RELEASE_SUBSTEP = 1;
		static const unsigned int CORE_RELEASE_YEAR    = 3;
		static const unsigned int CORE_RELEASE_MONTH   = 5;
		static const unsigned int CORE_RELEASE_DAY     = 6;
		static const unsigned int BUSWIDTH             = 32;
	};
	
	struct M_CAN_4_CONFIG
	{
		// Core Release 3.0.1, May 6th, 2013
		static const unsigned int CORE_RELEASE         = 3;
		static const unsigned int CORE_RELEASE_STEP    = 0;
		static const unsigned int CORE_RELEASE_SUBSTEP = 1;
		static const unsigned int CORE_RELEASE_YEAR    = 3;
		static const unsigned int CORE_RELEASE_MONTH   = 5;
		static const unsigned int CORE_RELEASE_DAY     = 6;
		static const unsigned int BUSWIDTH             = 32;
	};
	
	struct SHARED_CAN_MESSAGE_RAM_ROUTER_CONFIG : unisim::component::tlm2::interconnect::generic_router::Config
	{
		typedef uint32_t ADDRESS;
		static const unsigned int INPUT_SOCKETS = 5;
		static const unsigned int OUTPUT_SOCKETS = 1;
		static const unsigned int NUM_MAPPINGS = 1;
		static const unsigned int INPUT_BUSWIDTH = 32;
		static const unsigned int OUTPUT_BUSWIDTH = 32;
		static const bool VERBOSE = DEBUG_ENABLE;
		static const unsigned int BURST_LENGTH = FSB_BURST_SIZE / (INPUT_BUSWIDTH / 8); // FIXME
	};
	
	struct SEMA4_CONFIG
	{
		static const unsigned int NUM_GATES = 16;
		static const unsigned int BUSWIDTH  = 32;
	};
};

#endif // __MPC5777M_CONFIG_HH__
