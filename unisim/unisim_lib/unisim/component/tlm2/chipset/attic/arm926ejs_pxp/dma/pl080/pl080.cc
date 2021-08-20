/*
 *  Copyright (c) 2011,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/dma/pl080/pl080.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>
#include <sstream>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace dma {
namespace pl080 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

const uint32_t PL080::REGS_ADDR_ARRAY[PL080::NUMREGS] =
{
	DMACIntStatus,
	DMACIntTCStatus,
	DMACIntTCClear,
	DMACIntErrorStatus,
	DMACIntErrClr,
	DMACRawIntTCStatus,
	DMACRawIntErrorStatus,
	DMACEnbldChns,
	DMACSoftBReq,
	DMACSoftSReq,
	DMACSoftLBReq,
	DMACSoftLSReq,
	DMACConfiguration,
	DMACSync,
	DMACC0_Base + DMACCxSrcAddr,
	DMACC0_Base + DMACCxDestAddr,
	DMACC0_Base + DMACCxLLI,
	DMACC0_Base + DMACCxControl,
	DMACC0_Base + DMACCxConfiguration,
	DMACC1_Base + DMACCxSrcAddr,
	DMACC1_Base + DMACCxDestAddr,
	DMACC1_Base + DMACCxLLI,
	DMACC1_Base + DMACCxControl,
	DMACC1_Base + DMACCxConfiguration,
	DMACC2_Base + DMACCxSrcAddr,
	DMACC2_Base + DMACCxDestAddr,
	DMACC2_Base + DMACCxLLI,
	DMACC2_Base + DMACCxControl,
	DMACC2_Base + DMACCxConfiguration,
	DMACC3_Base + DMACCxSrcAddr,
	DMACC3_Base + DMACCxDestAddr,
	DMACC3_Base + DMACCxLLI,
	DMACC3_Base + DMACCxControl,
	DMACC3_Base + DMACCxConfiguration,
	DMACC4_Base + DMACCxSrcAddr,
	DMACC4_Base + DMACCxDestAddr,
	DMACC4_Base + DMACCxLLI,
	DMACC4_Base + DMACCxControl,
	DMACC4_Base + DMACCxConfiguration,
	DMACC5_Base + DMACCxSrcAddr,
	DMACC5_Base + DMACCxDestAddr,
	DMACC5_Base + DMACCxLLI,
	DMACC5_Base + DMACCxControl,
	DMACC5_Base + DMACCxConfiguration,
	DMACC6_Base + DMACCxSrcAddr,
	DMACC6_Base + DMACCxDestAddr,
	DMACC6_Base + DMACCxLLI,
	DMACC6_Base + DMACCxControl,
	DMACC6_Base + DMACCxConfiguration,
	DMACC7_Base + DMACCxSrcAddr,
	DMACC7_Base + DMACCxDestAddr,
	DMACC7_Base + DMACCxLLI,
	DMACC7_Base + DMACCxControl,
	DMACC7_Base + DMACCxConfiguration,
	DMACPeriphID0,
	DMACPeriphID1,
	DMACPeriphID2,
	DMACPeriphID3,
	DMACPCellID0,
	DMACPCellID1,
	DMACPCellID2,
	DMACPCellID3
};

const char *PL080::REGS_NAME_ARRAY[PL080::NUMREGS] =
{
	"DMACIntStatus",
	"DMACIntTCStatus",
	"DMACIntTCClear",
	"DMACIntErrorStatus",
	"DMACIntErrClr",
	"DMACRawIntTCStatus",
	"DMACRawIntErrorStatus",
	"DMACEnbldChns",
	"DMACSoftBReq",
	"DMACSoftSReq",
	"DMACSoftLBReq",
	"DMACSoftLSReq",
	"DMACConfiguration",
	"DMACSync",
	"DMACC0SrcAddr",
	"DMACC0DestAddr",
	"DMACC0LLI",
	"DMACC0Control",
	"DMACC0Configuration",
	"DMACC1SrcAddr",
	"DMACC1DestAddr",
	"DMACC1LLI",
	"DMACC1Control",
	"DMACC1Configuration",
	"DMACC2SrcAddr",
	"DMACC2DestAddr",
	"DMACC2LLI",
	"DMACC2Control",
	"DMACC2Configuration",
	"DMACC3SrcAddr",
	"DMACC3DestAddr",
	"DMACC3LLI",
	"DMACC3Control",
	"DMACC3Configuration",
	"DMACC4SrcAddr",
	"DMACC4DestAddr",
	"DMACC4LLI",
	"DMACC4Control",
	"DMACC4Configuration",
	"DMACC5SrcAddr",
	"DMACC5DestAddr",
	"DMACC5LLI",
	"DMACC5Control",
	"DMACC5Configuration",
	"DMACC6SrcAddr",
	"DMACC6DestAddr",
	"DMACC6LLI",
	"DMACC6Control",
	"DMACC6Configuration",
	"DMACC7SrcAddr",
	"DMACC7DestAddr",
	"DMACC7LLI",
	"DMACC7Control",
	"DMACC7Configuration",
	"DMACPeriphID0",
	"DMACPeriphID1",
	"DMACPeriphID2",
	"DMACPeriphID3",
	"DMACPCellID0",
	"DMACPCellID1",
	"DMACPCellID2",
	"DMACPCellID3"
};

PL080 ::
PL080(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, unisim::util::generic_peripheral_register::GenericPeripheralRegisterInterface<uint32_t>()
	, dmacinterr("dmacinterr")
	, dmacinttc("dmacinttc")
	, dmacintr("dmacintr")
	, bus_target_socket("bus_target_socket")
	, bus_master_1_socket("bus_master_1_socket")
	, bus_master_2_socket("bus_master_2_socket")
	, base_addr(0)
	, param_base_addr("base-addr", this, base_addr,
			"Base address of the dma controller.")
	, verbose(0)
	, param_verbose("verbose", this, verbose,
			"Verbose level (0 = no verbose).")
	, logger(*this)
{
	// put here the required SC_THREADS
	dmacinterr.initialize(false);
	dmacinttc.initialize(false);
	dmacintr.initialize(false);

	bus_target_socket.register_nb_transport_fw(this,
			&PL080::bus_target_nb_transport_fw);
	bus_target_socket.register_b_transport(this,
			&PL080::bus_target_b_transport);
	bus_target_socket.register_get_direct_mem_ptr(this,
			&PL080::bus_target_get_direct_mem_ptr);
	bus_target_socket.register_transport_dbg(this,
			&PL080::bus_target_transport_dbg);
	
	bus_master_1_socket.register_nb_transport_bw(this,
			&PL080::bus_master_1_nb_transport_bw);
	bus_master_1_socket.register_invalidate_direct_mem_ptr(this,
			&PL080::bus_master_1_invalidate_direct_mem_ptr);

	bus_master_2_socket.register_nb_transport_bw(this,
			&PL080::bus_master_2_nb_transport_bw);
	bus_master_2_socket.register_invalidate_direct_mem_ptr(this,
			&PL080::bus_master_2_invalidate_direct_mem_ptr);

	// init the registers values
	memset(regs, 0, sizeof(regs));
	regs[DMACPeriphID0] = 0x080;
	regs[DMACPeriphID1] = 0x010;
	regs[DMACPeriphID2] = 0x004;
	regs[DMACPeriphID3] = 0x00a;
	regs[DMACPCellID0]  = 0x00d;
	regs[DMACPCellID1]  = 0x0f0;
	regs[DMACPCellID2]  = 0x005;
	regs[DMACPCellID3]  = 0x0b1;

	for ( unsigned int i = 0; i < NUMREGS; i++ )
	{
		regs_accessor[i] = new
			unisim::util::generic_peripheral_register::GenericPeripheralWordRegister(
					this, REGS_ADDR_ARRAY[i]);
		std::stringstream desc;
		desc << REGS_NAME_ARRAY[i] << " register.";
		regs_service[i] = new
			unisim::kernel::variable::Register<unisim::util::generic_peripheral_register::GenericPeripheralWordRegister>(
					REGS_NAME_ARRAY[i], this, *regs_accessor[i], 
					desc.str().c_str());
	}
}

PL080 ::
~PL080()
{
	for ( unsigned int i = 0; i < NUMREGS; i++ )
	{
		delete regs_service[i];
		regs_service[i] = 0;
		delete regs_accessor[i];
		regs_accessor[i] = 0;
	}
}

bool 
PL080 ::
BeginSetup()
{
	return true;
}

bool
PL080 ::
EndSetup()
{
	return true;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection     START */
/**************************************************************************/

PL080::sync_enum_type 
PL080 ::
bus_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PL080 ::
bus_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	uint32_t addr = trans.get_address() - base_addr;
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();
	bool handled = false;

	// address must be:
	// - within bounds
	// - word aligned
	// - be a multiple of a word size (multiple of 4 bytes)
	// - writes must be exactly 4 bytes
	bool address_error = false;
	if ( !((trans.get_address() >= base_addr) &&
			((trans.get_address() + size) <= (base_addr + 0x01000UL))) )
	{
		address_error = true;
	}
	else
	{
		if ( trans.get_address() & 0x03UL )
		{
			address_error = true;
		}
		else
		{
			if ( is_read )
			{
				if ( (size % 4) != 0 )
				{
					address_error = true;
				}
			}
			else
			{
				if ( size != 4 )
				{
					address_error = true;
				}
			}
		}
	}
	if ( address_error )
	{
		logger << DebugError
			<< "Out of bounds access to DMAC:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size;
		if ( !is_read )
		{
			logger << std::endl
				<< " - data =" << std::hex;
			for ( unsigned int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec;
		}
		logger << EndDebugError;
		trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
		return;
	}

	if ( is_read )
	{
		handled = true;
		memcpy(data, &(regs[addr]), size);
		for ( unsigned int i = 0; handled && (i < size); i += 4 )
		{
			uint32_t cur_addr = addr + i;
			switch ( cur_addr )
			{
				case DMACPeriphID0:
				case DMACPeriphID1:
				case DMACPeriphID2:
				case DMACPeriphID3:
				case DMACPCellID0:
				case DMACPCellID1:
				case DMACPCellID2:
				case DMACPCellID3:
					break;
				default:
					handled = false;
					break;
			}

		}
	}
	// it is a write
	else
	{
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Access to DMAC:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size;
		if ( !is_read )
		{
			logger << std::endl
				<< " - data =" << std::hex;
			for ( unsigned int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec;
		}
		logger << EndDebugError;
		unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	}

	// everything went fine, update the status of the tlm response
	trans.set_response_status(tlm::TLM_OK_RESPONSE);

	if ( VERBOSE(V0, V_TRANS) )
	{
		logger << DebugInfo
			<< "Access to DMAC:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size << std::endl
			<< " - data =" << std::hex;
		for ( unsigned int i = 0; i < size; i++ )
			logger << " " << (unsigned int)data[i];
		logger << std::dec
			<< EndDebugInfo;
	}

}

bool 
PL080 ::
bus_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	return false;
}

unsigned int 
PL080 ::
bus_target_transport_dbg(transaction_type &trans)
{
	return 0;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection       END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the master sockets for the bus connection    START */
/**************************************************************************/

PL080::sync_enum_type
PL080 ::
bus_master_1_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	return tlm::TLM_ACCEPTED;
}

PL080::sync_enum_type
PL080 ::
bus_master_2_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	return tlm::TLM_ACCEPTED;
}

void 
PL080 ::
bus_master_1_invalidate_direct_mem_ptr(sc_dt::uint64 addr,
		sc_dt::uint64 size)
{
}

void 
PL080 ::
bus_master_2_invalidate_direct_mem_ptr(sc_dt::uint64 addr,
		sc_dt::uint64 size)
{
}

/**************************************************************************/
/* Virtual methods for the master sockets for the bus connection      END */
/**************************************************************************/

/**************************************************************************/
/* Methods to get and set the registers                             START */
/**************************************************************************/

/** Get interface for the generic peripheral register interface
 *
 * @param addr the address to consider
 * @return the value of the register pointed by the address
 */
uint32_t
PL080 ::
GetPeripheralRegister(uint64_t addr)
{
	return GetRegister(addr);
}

/** Set interface for the generic peripheral register interface
 *
 * @param addr the address to consider
 * @param value the value to set the register to
 */
void
PL080 ::
SetPeripheralRegister(uint64_t addr, uint32_t value)
{
	SetRegister(addr, value);
}

/** Returns the register pointed by the given address
 *
 * @param addr the address to consider
 * @return the value of the register pointed by the address
 */
uint32_t
PL080 ::
GetRegister(uint32_t addr) const
{
	const uint8_t *data = &(regs[addr]);
	uint32_t value = 0;

	memcpy(&value, data, 4);
	value = LittleEndian2Host(value);
	return value;
}

/** Sets the register pointed by the given address
 *
 * @param addr the address to consider
 * @param value the value to set the register
 */
void
PL080 ::
SetRegister(uint32_t addr, uint32_t value)
{
	uint8_t *data = &(regs[addr]);

	value = Host2LittleEndian(value);
	memcpy(data, &value, 4);
}

/**************************************************************************/
/* Methods to get and set the registers                               END */
/**************************************************************************/

} // end of namespace pl080
} // end of namespace dma
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

