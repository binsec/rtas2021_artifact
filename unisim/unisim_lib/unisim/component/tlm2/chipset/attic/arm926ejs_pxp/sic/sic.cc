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
 
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/sic/sic.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>
#include <assert.h>
#include <sstream>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {
namespace sic {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;
using unisim::component::tlm2::chipset::arm926ejs_pxp::vic::VICIntSourceIdentifier;

SIC ::
SIC(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, unisim::component::tlm2::chipset::arm926ejs_pxp::vic::VICIntSourceIdentifierInterface()
	, bus_target_socket("bus_target_socket")
	, int_source(0)
	, ptint_source(0)
	, int_target(0)
	, sic_output_value(false)
	, base_addr(0)
	, param_base_addr("base-addr", this, base_addr,
			"Base address of the secondary interrupt controller.")
	, verbose(0)
	, param_verbose("verbose", this, verbose,
			"Verbose level (0 = no verbose).")
	, logger(*this)
{
	for ( unsigned int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		std::stringstream sicintsource_name;
		sicintsource_name << "sicintsource[" << i << "]";
		sicintsource[i] =
			new sc_core::sc_in<bool>(sicintsource_name.str().c_str());
	}
	for ( unsigned int i = 0; i < NUM_SOURCE_PT_INT; i++ )
	{
		std::stringstream sicptintsource_name;
		sicptintsource_name << "sicptintsource[" << i << "]";
		sicptintsource[i] =
			new sc_core::sc_in<bool>(sicptintsource_name.str().c_str());
	}
	for ( unsigned int i = 0; i < NUM_TARGET_INT; i++ )
	{
		std::stringstream sicinttarget_name;
		sicinttarget_name << "sicinttarget[" << i << "]";
		sicinttarget[i] =
			new sc_core::sc_out<bool>(sicinttarget_name.str().c_str());
		sicinttarget[i]->initialize(true);
	}

	bus_target_socket.register_nb_transport_fw(this,
			&SIC::bus_target_nb_transport_fw);
	bus_target_socket.register_b_transport(this,
			&SIC::bus_target_b_transport);
	bus_target_socket.register_get_direct_mem_ptr(this,
			&SIC::bus_target_get_direct_mem_ptr);
	bus_target_socket.register_transport_dbg(this,
			&SIC::bus_target_transport_dbg);

	// init the registers values
	memset(regs, 0, 0x028UL);

	// set identifiers for source interrupts
	for ( unsigned int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		std::stringstream id_name;

		id_name << "SICIntSourceIdentifier[" << (i + 1) << "]";
		source_identifier_method[i] =
			new VICIntSourceIdentifier(id_name.str().c_str(),
					i, this);
		source_identifier_method[i]->vicinttarget(*sicintsource[i]);
	}

	// set identifiers for source passthrough interrupts
	for ( unsigned int i = 0; i < NUM_SOURCE_PT_INT; i++ )
	{
		std::stringstream id_name;

		id_name << "SICPtIntSourceIdentifier[" << i << "]";
		pt_source_identifier_method[i] =
			new VICIntSourceIdentifier(id_name.str().c_str(),
					i + NUM_SOURCE_INT, this);
		pt_source_identifier_method[i]->vicinttarget(*sicptintsource[i]);
	}
}

SIC ::
~SIC()
{
	for ( unsigned int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		delete source_identifier_method[i];
		delete sicintsource[i];
	}

	for ( unsigned int i = 0; i < NUM_SOURCE_PT_INT; i++ )
	{
		delete pt_source_identifier_method[i];
		delete sicptintsource[i];
	}
}

bool 
SIC ::
BeginSetup()
{
	return true;
}

/** Update the status of the SIC depending on all the possible entries
 */
void
SIC ::
UpdateStatus()
{
	uint32_t int_target_old = int_target;
	uint32_t mask = 0;
	uint32_t status = 0;
	uint32_t enable = GetSIC_ENABLE();

	SetSIC_RAWSTAT(int_source);
	for ( unsigned int i = 0; i < (NUM_SOURCE_INT + 1); i++ )
	{
		mask = 0x01UL << i;
		status |= mask & enable & int_source;
	}
	SetSIC_STATUS(status);

	uint32_t picenable = GetSIC_PICENABLE();
	int_target = 0;
	for ( unsigned int i = 0; i < 10; i++ )
	{
		mask = 0x01UL << (i + 21);
		if ( mask & picenable )
		{
			int_target |= (int_source & mask);
		}
		else
		{
			int_target |= (ptint_source & mask);
		}
	}
	mask = 0x01UL << 31;
	if ( mask & picenable )
	{
		if ( status )
			int_target |= mask;
	}
	else
	{
		int_target |= mask & ptint_source;
	}

	for ( unsigned int i = 0; i < 11; i++ )
	{
		mask = 0x01UL << (i + 21);
		if ( (int_target_old & mask) != (int_target & mask) )
			*sicinttarget[i] = (int_target & mask) ? false : true;
	}
}

/** Source interrupt handling
 *
 * @param id the port identifier
 * @param value the value of the port
 */
void 
SIC ::
VICIntSourceReceived(int id, bool value)
{
	uint32_t mask = 0;
	uint32_t int_source_old = int_source;
	uint32_t ptint_source_old = ptint_source;

	// identify if it is comming from passthrough or not
	if ( (unsigned int)id < NUM_SOURCE_INT )
	{
		mask = 0x01UL << id;
		if ( value )
			int_source = int_source | mask;
		else
			int_source = int_source & ~mask;
	}
	else
	{
		mask = 0x01UL << (21 + (id - NUM_SOURCE_INT));
		if ( value )
			ptint_source |= mask;
		else
			ptint_source &= ~mask;
	}

	if ( (int_source != int_source_old) ||
			(ptint_source != ptint_source_old) )
		UpdateStatus();
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
SIC ::
bus_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
SIC ::
bus_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	wait(delay);
	delay = SC_ZERO_TIME;

	uint32_t addr = trans.get_address() - base_addr;
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();
	bool handled = false;
	bool should_update_status = false;

	if ( is_read )
	{
		if ( (0x028L - size) >= addr )
		{
			memcpy(data, &(regs[addr]), size);
			for ( unsigned int i = 0; i < size; i+= 4 )
			{
				uint32_t align_addr = (addr + i) & ~0x03UL;
				
				if ( align_addr == SIC_STATUSAddr )
				{
					handled = true;
				}
				
				else if ( align_addr == SIC_RAWSTATAddr )
				{
					handled = true;
				}

				else if ( align_addr == SIC_ENABLEAddr )
				{
					handled = true;
				}

				else if ( align_addr == SIC_SOFTINTSETAddr )
				{
					handled = true;
				}

				else if ( align_addr == SIC_PICENABLEAddr )
				{
					handled = true;
				}

			}
		}
	}
	
	else
	{
		if ( (0x028L - size) >= addr )
		{
			uint32_t enable = GetSIC_ENABLE();
			uint32_t picenable = GetSIC_PICENABLE();
			
			memcpy(&(regs[addr]), data, size);
			for ( unsigned int i = 0; i < size; i += 4 )
			{
				uint32_t align_addr = (addr + i) & ~0x03UL;
				if ( align_addr == SIC_ENSETAddr )
				{
					uint32_t setmask = GetSIC_ENSET();
					enable |= setmask;
					SetSIC_ENABLE(enable);
					should_update_status = true;
					handled = true;
				}

				else if ( align_addr == SIC_ENCLRAddr )
				{
					uint32_t clrmask = GetSIC_ENCLR();
					enable &= ~clrmask;
					SetSIC_ENABLE(enable);
					// reset the value of ENCLR
					SetSIC_ENCLR(0);
					should_update_status = true;
					handled = true;
				}

				else if ( align_addr == SIC_SOFTINTSETAddr )
				{
					should_update_status = true;
				}

				else if ( align_addr == SIC_SOFTINTCLRAddr )
				{
					should_update_status = true;
				}

				else if ( align_addr == SIC_PICENSETAddr )
				{
					uint32_t setmask = GetSIC_PICENSET();
					picenable |= setmask;
					SetSIC_PICENABLE(picenable);
					should_update_status = true;
					handled = true;
				}

				else if ( align_addr == SIC_PICENCLRAddr )
				{
					uint32_t clrmask = GetSIC_PICENCLR();
					picenable &= ~clrmask;
					SetSIC_PICENABLE(picenable);
					// reset the value of PICENCLR
					SetSIC_PICENCLR(0);
					should_update_status = true;
					handled = true;
				}

			}
		}
	}

	if ( should_update_status )
	{
		UpdateStatus();
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Access to SIC:" << std::endl
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

	if ( VERBOSE(V0, V_READ | V_WRITE) )
	{
		if ( (is_read && VERBOSE(V0, V_READ)) ||
				(!is_read && VERBOSE(V0, V_WRITE)) )
		{
			logger << DebugInfo
				<< "Access to SIC:" << std::endl
				<< " - read = " << is_read << std::endl
				<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
				<< " - size = " << size << std::endl
				<< " - data = " << std::hex;
			for ( unsigned int i = 0; i < size; i++ )
				logger << " " << (unsigned int)data[i];
			logger << std::dec
				<< EndDebugInfo;
		}
	}

}

bool 
SIC ::
bus_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return false;
}

unsigned int 
SIC ::
bus_target_transport_dbg(transaction_type &trans)
{
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return 0;
}
	
/**************************************************************************/
/* Virtual methods for the target socket for the bus connection       END */
/**************************************************************************/

/**************************************************************************/
/* Methods to get and set the registers                             START */
/**************************************************************************/

/** Gets the given registet address
 *
 * @param addr the register address
 * @return the value of the register
 */
uint32_t 
SIC ::
GetRegister(uint32_t addr) const
{
	const uint8_t *data = &(regs[addr]);
	uint32_t value = 0;

	memcpy(&value, data, 4);
	value = LittleEndian2Host(value);
	return value;
}

/** Sets the given register address
 *
 * @param addr the reigster address
 * @param value the value to use
 */
void 
SIC ::
SetRegister(uint32_t addr, uint32_t value)
{
	uint8_t *data = &(regs[addr]);

	value = Host2LittleEndian(value);
	memcpy(data, &value, 4);
}

/** Get the SIC_STATUS register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_STATUS() const
{
	return GetRegister(SIC_STATUSAddr);
}

/** Set the SIC_STATUS register
 *
 * @param value the value to set
 */
void 
SIC ::
SetSIC_STATUS(uint32_t value)
{
	SetRegister(SIC_STATUSAddr, value);
}

/** Get the SIC_RAWSTAT register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_RAWSTAT() const
{
	return GetRegister(SIC_RAWSTATAddr);
}

/** Set the SIC_RAWSTAT register
 *
 * @param value the value to set
 */
void 
SIC ::
SetSIC_RAWSTAT(uint32_t value)
{
	SetRegister(SIC_RAWSTATAddr, value);
}

/** Get the SIC_ENABLE register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_ENABLE() const
{
	return GetRegister(SIC_ENABLEAddr);
}

/** Set the SIC_ENABLE register
 *
 * @param value the value to set
 */
void 
SIC ::
SetSIC_ENABLE(uint32_t value)
{
	SetRegister(SIC_ENABLEAddr, value);
}

/** Get the SIC_ENSET register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_ENSET() const
{
	return GetRegister(SIC_ENSETAddr);
}

/** Set the SIC_ENSET register
 *
 * @param value the value to set
 */
void 
SIC ::
SetSIC_ENSET(uint32_t value)
{
	SetRegister(SIC_ENSETAddr, value);
}

/** Get the SIC_ENCLR register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_ENCLR() const
{
	return GetRegister(SIC_ENCLRAddr);
}

/** Set the SIC_ENCLR register
 *
 * @param value the value to set
 */
void 
SIC ::
SetSIC_ENCLR(uint32_t value)
{
	SetRegister(SIC_ENCLRAddr, value);
}

/** Get the SIC_SOFTINTSET register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_SOFTINTSET() const
{
	return GetRegister(SIC_SOFTINTSETAddr);
}

/** Set the SIC_SOFTINTSET register
 *
 * @param value the value to set
 */
void 
SIC ::
SetSIC_SOFTINTSET(uint32_t value)
{
	SetRegister(SIC_SOFTINTSETAddr, value);
}

/** Get the SIC_SOFTINTCLR register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_SOFTINTCLR() const
{
	return GetRegister(SIC_SOFTINTCLRAddr);
}

/** Set the SIC_SOFTINTCLR register
 *
 * @param value the value to set
 */
void 
SIC ::
SetSIC_SOFTINTCLR(uint32_t value)
{
	SetRegister(SIC_SOFTINTCLRAddr, value);
}

/** Get the SIC_PICENABLE register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_PICENABLE() const
{
	return GetRegister(SIC_PICENABLEAddr);
}

/** Set the SIC_PICENABLE register
 *
 * @param value the value to set
 */
void
SIC :: 
SetSIC_PICENABLE(uint32_t value)
{
	SetRegister(SIC_PICENABLEAddr, value);
}

/** Get the SIC_PICENSET register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_PICENSET() const
{
	return GetRegister(SIC_PICENSETAddr);
}

/** Set the SIC_PICENSET register
 *
 * @param value the value to set
 */
void 
SIC ::
SetSIC_PICENSET(uint32_t value)
{
	SetRegister(SIC_PICENSETAddr, value);
}

/** Get the SIC_PICENCLR register
 *
 * @return the value of the register
 */
uint32_t 
SIC ::
GetSIC_PICENCLR() const
{
	return GetRegister(SIC_PICENCLRAddr);
}

/** Set the SIC_PICENCLR register
 *
 * @param value the value to set
 */
void 
SIC ::
SetSIC_PICENCLR(uint32_t value)
{
	SetRegister(SIC_PICENCLRAddr, value);
}
	
/**************************************************************************/
/* Methods to get and set the registers                               END */
/**************************************************************************/

} // end of namespace sic
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


