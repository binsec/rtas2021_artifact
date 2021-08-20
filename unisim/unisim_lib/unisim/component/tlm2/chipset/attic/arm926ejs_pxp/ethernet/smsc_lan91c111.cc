/*
 *  Copyright (c) 2010,
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
 
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/ethernet/smsc_lan91c111.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace ethernet {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

SMSC_LAN91C111 ::
SMSC_LAN91C111(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, bus_target_socket("bus_target_socket")
	, base_addr(0)
	, param_base_addr("base-addr", this, base_addr,
			"Base address of the ethernet controller.")
	, logger(*this)
	, cur_bank(0)
{
	bus_target_socket.register_nb_transport_fw(this,
			&SMSC_LAN91C111::bus_target_nb_transport_fw);
	bus_target_socket.register_b_transport(this,
			&SMSC_LAN91C111::bus_target_b_transport);
	bus_target_socket.register_get_direct_mem_ptr(this,
			&SMSC_LAN91C111::bus_target_get_direct_mem_ptr);
	bus_target_socket.register_transport_dbg(this,
			&SMSC_LAN91C111::bus_target_transport_dbg);

	// init the registers values
	memset(regs, 0, 65536);
}

SMSC_LAN91C111 ::
~SMSC_LAN91C111()
{
}

bool 
SMSC_LAN91C111 ::
BeginSetup()
{
	return true;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
SMSC_LAN91C111 ::
bus_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	assert("TODO" == 0);
	return tlm::TLM_COMPLETED;
}

uint8_t
SMSC_LAN91C111 ::
ReadByteBank0(uint8_t addr)
{
	bool handled = false;
	uint8_t data = 0;

	switch ( addr )
	{
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			break;
		case 0x0c:
			break;
		case 0x0d:
			break;
		case 0x0e:
			handled = true;
			/* current bank */
			data = cur_bank;
			logger << DebugInfo
				<< "(Bank 0) Reading current index ("
				<< (unsigned int)cur_bank
				<< ")"
				<< EndDebugInfo;
			break;
		case 0x0f:
			handled = true;
			/* reserved space */
			data = 0x33;
			logger << DebugInfo
				<< "(Bank 0) Reading reserved space"
				<< EndDebugInfo;
			break;
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Reading bank 0 addr 0x"
			<< std::hex << (unsigned int)addr
			<< EndDebugError;
		assert("Reading to bank 0 not handled" == 0);
	}
	return data;
}

uint8_t
SMSC_LAN91C111 ::
ReadByteBank1(uint8_t addr)
{
	bool handled = false;
	uint8_t data = 0;

	switch ( addr )
	{
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			break;
		case 0x0c:
			break;
		case 0x0d:
			break;
		case 0x0e:
			handled = true;
			/* current bank */
			data = cur_bank;
			logger << DebugInfo
				<< "(Bank 1) Reading current index ("
				<< (unsigned int)cur_bank
				<< ")"
				<< EndDebugInfo;
			break;
		case 0x0f:
			handled = true;
			/* reserved space */
			data = 0x33;
			logger << DebugInfo
				<< "(Bank 1) Reading reserved space"
				<< EndDebugInfo;
			break;
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Reading bank 1 addr 0x"
			<< std::hex << (unsigned int)addr
			<< EndDebugError;
		assert("Reading to bank 1 not handled" == 0);
	}
	return data;
}

uint8_t
SMSC_LAN91C111 ::
ReadByteBank2(uint8_t addr)
{
	bool handled = false;
	uint8_t data = 0;

	switch ( addr )
	{
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			break;
		case 0x0c:
			break;
		case 0x0d:
			break;
		case 0x0e:
			handled = true;
			/* current bank */
			data = cur_bank;
			logger << DebugInfo
				<< "(Bank 2) Reading current index ("
				<< (unsigned int)cur_bank
				<< ")"
				<< EndDebugInfo;
			break;
		case 0x0f:
			handled = true;
			/* reserved space */
			data = 0x33;
			logger << DebugInfo
				<< "(Bank 2) Reading reserved space"
				<< EndDebugInfo;
			break;
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Reading bank 2 addr 0x"
			<< std::hex << (unsigned int)addr
			<< EndDebugError;
		assert("Reading to bank 2 not handled" == 0);
	}
	return data;
}

uint8_t
SMSC_LAN91C111 ::
ReadByteBank3(uint8_t addr)
{
	bool handled = false;
	uint8_t data = 0;

	switch ( addr )
	{
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			break;
		case 0x0c:
			break;
		case 0x0d:
			break;
		case 0x0e:
			handled = true;
			/* current bank */
			data = cur_bank;
			logger << DebugInfo
				<< "(Bank 3) Reading current index ("
				<< (unsigned int)cur_bank
				<< ")"
				<< EndDebugInfo;
			break;
		case 0x0f:
			handled = true;
			/* reserved space */
			data = 0x33;
			logger << DebugInfo
				<< "(Bank 3) Reading reserved space"
				<< EndDebugInfo;
			break;
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Reading bank 3 addr 0x"
			<< std::hex << (unsigned int)addr
			<< EndDebugError;
		assert("Reading to bank 3 not handled" == 0);
	}
	return data;
}

void
SMSC_LAN91C111 ::
WriteByteBank0(uint8_t addr, uint8_t data)
{
	bool handled = false;

	switch ( addr )
	{
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			break;
		case 0x0c:
			break;
		case 0x0d:
			break;
		case 0x0e:
			handled = true;
			/* change the bank */
			cur_bank = data;
			logger << DebugInfo
				<< "(Bank 0) Changing to bank "
				<< (unsigned int)cur_bank
				<< EndDebugInfo;
			break;
		case 0x0f:
			handled = true;
			/* reserved space */
			logger << DebugInfo
				<< "(Bank 0) Writing 0x"
				<< std::hex << (unsigned int)data
				<< std::dec
				<< " into reserved space (0xf). Do nothing."
				<< EndDebugInfo;
			break;
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Writing to bank 0 addr 0x"
			<< std::hex << (unsigned int)addr
			<< " with data 0x"
			<< (unsigned int)data << std::dec
			<< EndDebugError;
		assert("Writing to bank 0 not handled" == 0);
	}
}

void
SMSC_LAN91C111 ::
WriteByteBank1(uint8_t addr, uint8_t data)
{
	bool handled = false;

	switch ( addr )
	{
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			break;
		case 0x0c:
			break;
		case 0x0d:
			break;
		case 0x0e:
			handled = true;
			/* change the bank */
			cur_bank = data;
			logger << DebugInfo
				<< "(Bank 1) Changing to bank "
				<< (unsigned int)cur_bank
				<< EndDebugInfo;
			break;
		case 0x0f:
			handled = true;
			/* reserved space */
			logger << DebugInfo
				<< "(Bank 1) Writing 0x"
				<< std::hex << (unsigned int)data
				<< std::dec
				<< " into reserved space (0xf). Do nothing."
				<< EndDebugInfo;
			break;
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Writing to bank 1 addr 0x"
			<< std::hex << (unsigned int)addr
			<< " with data 0x"
			<< (unsigned int)data << std::dec
			<< EndDebugError;
		assert("Writing to bank 0 not handled" == 0);
	}
}

void
SMSC_LAN91C111 ::
WriteByteBank2(uint8_t addr, uint8_t data)
{
	bool handled = false;

	switch ( addr )
	{
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			break;
		case 0x0c:
			break;
		case 0x0d:
			break;
		case 0x0e:
			handled = true;
			/* change the bank */
			cur_bank = data;
			logger << DebugInfo
				<< "(Bank 2) Changing to bank "
				<< (unsigned int)cur_bank
				<< EndDebugInfo;
			break;
		case 0x0f:
			handled = true;
			/* reserved space */
			logger << DebugInfo
				<< "(Bank 2) Writing 0x"
				<< std::hex << (unsigned int)data
				<< std::dec
				<< " into reserved space (0xf). Do nothing."
				<< EndDebugInfo;
			break;
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Writing to bank 2 addr 0x"
			<< std::hex << (unsigned int)addr
			<< " with data 0x"
			<< (unsigned int)data << std::dec
			<< EndDebugError;
		assert("Writing to bank 0 not handled" == 0);
	}
}

void
SMSC_LAN91C111 ::
WriteByteBank3(uint8_t addr, uint8_t data)
{
	bool handled = false;

	switch ( addr )
	{
		case 0x00:
			break;
		case 0x01:
			break;
		case 0x02:
			break;
		case 0x03:
			break;
		case 0x04:
			break;
		case 0x05:
			break;
		case 0x06:
			break;
		case 0x07:
			break;
		case 0x08:
			break;
		case 0x09:
			break;
		case 0x0a:
			break;
		case 0x0b:
			break;
		case 0x0c:
			break;
		case 0x0d:
			break;
		case 0x0e:
			handled = true;
			/* change the bank */
			cur_bank = data;
			logger << DebugInfo
				<< "(Bank 3) Changing to bank "
				<< (unsigned int)cur_bank
				<< EndDebugInfo;
			break;
		case 0x0f:
			handled = true;
			/* reserved space */
			logger << DebugInfo
				<< "(Bank 3) Writing 0x"
				<< std::hex << (unsigned int)data
				<< std::dec
				<< " into reserved space (0xf). Do nothing."
				<< EndDebugInfo;
			break;
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Writing to bank 3 addr 0x"
			<< std::hex << (unsigned int)addr
			<< " with data 0x"
			<< (unsigned int)data << std::dec
			<< EndDebugError;
		assert("Writing to bank 0 not handled" == 0);
	}
}

uint8_t 
SMSC_LAN91C111 ::
ReadByte(uint8_t addr)
{
	switch ( cur_bank )
	{
		case 0:
			return ReadByteBank0(addr);
			break;
		case 1:
			return ReadByteBank1(addr);
			break;
		case 2:
			return ReadByteBank2(addr);
			break;
		case 3:
			return ReadByteBank3(addr);
			break;
		default:
			logger << DebugError
				<< "Incorrect bank selected ("
				<< std::hex << cur_bank << std::dec
				<< ")"
				<< EndDebugError;
			assert("Incorrect bank selected" == 0);
			break;
	}
}

void 
SMSC_LAN91C111 ::
WriteByte(uint8_t addr, uint8_t data)
{
	switch ( cur_bank )
	{
		case 0:
			WriteByteBank0(addr, data);
			break;
		case 1:
			WriteByteBank1(addr, data);
			break;
		case 2:
			WriteByteBank2(addr, data);
			break;
		case 3:
			WriteByteBank3(addr, data);
			break;
		default:
			logger << DebugError
				<< "Incorrect bank selected ("
				<< std::hex << cur_bank << std::dec
				<< ")"
				<< EndDebugError;
			assert("Incorrect bank selected" == 0);
			break;
	}
}

void 
SMSC_LAN91C111 ::
bus_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	uint32_t addr = trans.get_address() - base_addr;
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();
	bool handled = false;

	logger << DebugError
		<< (is_read ? "Reading" : "Writing") << " 0x" 
		<< std::hex << addr
		<< " (0x"
		<< trans.get_address() << std::dec
		<< ") of " << size << " bytes"
		<< EndDebugError;

	if ( is_read )
	{
		if ( size == 1 )
		{
			handled = true;
			data[0] = ReadByte((uint8_t)(addr & 0xff));
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
		}
		else if ( size == 2 )
		{
			handled = true;
			uint16_t data16 = 0;
			data16 = ReadByte((uint8_t)(addr & 0xff));
			data16 = data16 << 8;
			data16 |= ReadByte((uint8_t)((addr + 1) & 0xff));
			data16 = Host2LittleEndian(data16);
			memcpy(data, &data16, sizeof(data16));
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
		}
		else if ( size == 4 )
		{
			handled = true;
			uint32_t data32 = 0;
			data32 = ReadByte((uint8_t)(addr & 0xff));
			data32 = data32 << 8;
			data32 |= ReadByte((uint8_t)((addr + 1) & 0xff));
			data32 = data32 << 8;
			data32 |= ReadByte((uint8_t)((addr + 2) & 0xff));
			data32 = data32 << 8;
			data32 |= ReadByte((uint8_t)((addr + 3) & 0xff));
			data32 = Host2LittleEndian(data32);
			memcpy(data, &data32, sizeof(data32));
			trans.set_response_status(tlm::TLM_OK_RESPONSE);
		}
	}
	else // writing
	{
		if ( size == 1 )
		{
			handled = true;
			WriteByte((uint8_t)(addr & 0xff), data[0]);
		}
		else if ( size == 2 )
		{
			handled = true;
			uint16_t data16;
			memcpy(&data16, data, 2);
			data16 = LittleEndian2Host(data16);
			WriteByte((uint8_t)(addr & 0xff), (uint8_t)(data16 & 0xff));
			WriteByte((uint8_t)((addr + 1) & 0xff),
					(uint8_t)((data16 >> 8) & 0xff));
		}
		else if ( size == 4 )
		{
			handled = true;
			uint32_t data32;
			memcpy(&data32, data, 4);
			data32 = LittleEndian2Host(data32);
			if ( addr != 0xc )
			{			
				WriteByte((uint8_t)(addr & 0xff), (uint8_t)(data32 & 0xff));
				WriteByte((uint8_t)((addr + 1) & 0xff),
						(uint8_t)((data32 >> 8) & 0xff));
			}
			WriteByte((uint8_t)((addr + 2) & 0xff),
					(uint8_t)((data32 >> 16) & 0xff));
			WriteByte((uint8_t)((addr + 3) & 0xff),
					(uint8_t)((data32 >> 24) & 0xff));
		}
	}
	if ( !handled )
		assert("TODO" == 0);
}

bool 
SMSC_LAN91C111 ::
bus_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	assert("TODO" == 0);
	return false;
}

unsigned int 
SMSC_LAN91C111 ::
bus_target_transport_dbg(transaction_type &trans)
{
	assert("TODO" == 0);
	return 0;
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection       END */
/**************************************************************************/

} // end of namespace ethernet 
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


