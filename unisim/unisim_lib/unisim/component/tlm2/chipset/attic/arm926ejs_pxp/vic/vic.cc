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
 
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic.hh"
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
namespace vic {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::LittleEndian2Host;

const uint32_t VIC::REGS_ADDR_ARRAY[VIC::NUMREGS] =
{
	VICIRQSTATUSAddr, // 1
	VICFIQSTATUSAddr, // 2
	VICRAWINTRAddr, // 3
	VICINTSELECTAddr, // 4
	VICINTENABLEAddr, // 5
	VICINTENCLEARAddr, // 6
	VICSOFTINTAddr, // 7
	VICPROTECTIONAddr, // 8
	VICVECTADDRAddr, // 9
	VICDEFVECTADDRAddr, // 10
	VICVECTADDRBaseAddr, // 11 0
	VICVECTADDRBaseAddr + 4, // 12 1
	VICVECTADDRBaseAddr + 8, // 13 2
	VICVECTADDRBaseAddr + 12, // 14 3
	VICVECTADDRBaseAddr + 16, // 15 4
	VICVECTADDRBaseAddr + 20, // 16 5
	VICVECTADDRBaseAddr + 24, // 17 6
	VICVECTADDRBaseAddr + 28, // 18 7
	VICVECTADDRBaseAddr + 32, // 19 8
	VICVECTADDRBaseAddr + 36, // 20 9
	VICVECTADDRBaseAddr + 40, // 21 10
	VICVECTADDRBaseAddr + 44, // 22 11
	VICVECTADDRBaseAddr + 48, // 23 12
	VICVECTADDRBaseAddr + 52, // 24 13
	VICVECTADDRBaseAddr + 56, // 25 14
	VICVECTADDRBaseAddr + 60, // 26 15
	VICVECTCNTLBaseAddr, // 27 0
	VICVECTCNTLBaseAddr + 4, // 28 1
	VICVECTCNTLBaseAddr + 8, // 29 2
	VICVECTCNTLBaseAddr + 12, // 30 3
	VICVECTCNTLBaseAddr + 16, // 31 4
	VICVECTCNTLBaseAddr + 20, // 32 5
	VICVECTCNTLBaseAddr + 24, // 33 6
	VICVECTCNTLBaseAddr + 28, // 34 7
	VICVECTCNTLBaseAddr + 32, // 35 8
	VICVECTCNTLBaseAddr + 36, // 36 9
	VICVECTCNTLBaseAddr + 40, // 37 10
	VICVECTCNTLBaseAddr + 44, // 38 11
	VICVECTCNTLBaseAddr + 48, // 39 12
	VICVECTCNTLBaseAddr + 52, // 40 13
	VICVECTCNTLBaseAddr + 56, // 41 14
	VICVECTCNTLBaseAddr + 60, // 42 15
	VICPERIPHIDBaseAddr, // 43 0
	VICPERIPHIDBaseAddr + 4, // 44 1
	VICPERIPHIDBaseAddr + 8, // 45 2
	VICPERIPHIDBaseAddr + 12, // 46 3
	VICPCELLIDBaseAddr, // 47 0
	VICPCELLIDBaseAddr + 4, // 48 1
	VICPCELLIDBaseAddr + 8, // 49 2
	VICPCELLIDBaseAddr + 12, // 50 3
	VICITCRAddr, // 51
	VICITIP1Addr, // 52
	VICITIP2Addr, // 53
	VICITOP1Addr, // 54
	VICITOP2Addr // 55
};

const char *VIC::REGS_NAME_ARRAY[VIC::NUMREGS] =
{
	"VICIRQSTATUS", // 1
	"VICFIQSTATUS", // 2
	"VICRAWINTR", // 3
	"VICINTSELECT", // 4
	"VICINTENABLE", // 5
	"VICINTENCLEAR", // 6
	"VICSOFTINT", // 7
	"VICPROTECTION", // 8
	"VICVECTADDR", // 9
	"VICDEFVECTADDR", // 10
	"VICVECTADDR[0]", // 11 0
	"VICVECTADDR[1]", // 12 1
	"VICVECTADDR[2]", // 13 2
	"VICVECTADDR[3]", // 14 3
	"VICVECTADDR[4]", // 15 4
	"VICVECTADDR[5]", // 16 5
	"VICVECTADDR[6]", // 17 6
	"VICVECTADDR[7]", // 18 7
	"VICVECTADDR[8]", // 19 8
	"VICVECTADDR[9]", // 20 9
	"VICVECTADDR[10]", // 21 10
	"VICVECTADDR[11]", // 22 11
	"VICVECTADDR[12]", // 23 12
	"VICVECTADDR[13]", // 24 13
	"VICVECTADDR[14]", // 25 14
	"VICVECTADDR[15]", // 26 15
	"VICVECTCNTL[0]", // 27 0
	"VICVECTCNTL[1]", // 28 1
	"VICVECTCNTL[2]", // 29 2
	"VICVECTCNTL[3]", // 30 3
	"VICVECTCNTL[4]", // 31 4
	"VICVECTCNTL[5]", // 32 5
	"VICVECTCNTL[6]", // 33 6
	"VICVECTCNTL[7]", // 34 7
	"VICVECTCNTL[8]", // 35 8
	"VICVECTCNTL[9]", // 36 9
	"VICVECTCNTL[10]", // 37 10
	"VICVECTCNTL[11]", // 38 11
	"VICVECTCNTL[12]", // 39 12
	"VICVECTCNTL[13]", // 40 13
	"VICVECTCNTL[14]", // 41 14
	"VICVECTCNTL[15]", // 42 15
	"VICPERIPHID[0]", // 43 0
	"VICPERIPHID[1]", // 44 1
	"VICPERIPHID[2]", // 45 2
	"VICPERIPHID[3]", // 46 3
	"VICPCELLID[0]", // 47 0
	"VICPCELLID[1]", // 48 1
	"VICPCELLID[2]", // 49 2
	"VICPCELLID[3]", // 50 3
	"VICITCR", // 51
	"VICITIP1", // 52
	"VICITIP2", // 53
	"VICITOP1", // 54
	"VICITOP2" // 55
};

VIC ::
VIC(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, VICIntSourceIdentifierInterface()
	, unisim::util::generic_peripheral_register::GenericPeripheralRegisterInterface<uint32_t>()
	, nvicfiqin("nvicfiqin")
	, nvicirqin("nvicirqin")
	, vicvectaddrin("vicvectaddrin")
	, nvicfiq("nvicfiq")
	, nvicirq("nvicirq")
	, vicvectaddrout("vicvectaddrout")
	, bus_target_socket("bus_target_socket")
	, state_semaphore("state_semaphore", 1)
	, update_status_event()
	, forwarding_nvicirqin(true)
	, nvicirqin_value(false)
	, vect_int_serviced(0)
	, vect_int_for_service(false)
	, max_vect_int_for_service(0)
	, int_source(0)
	, nvicfiq_value(true)
	, nvicirq_value(true)
	, vicvectaddrout_value(0)
	, base_addr(0)
	, param_base_addr("base-addr", this, base_addr,
			"Base address of the system controller.")
	, verbose(0)
	, param_verbose("verbose", this, verbose,
			"Verbose level (0 = no verbose).")
	, logger(*this)
{
	SC_THREAD(UpdateStatusHandler);
	nvicirq.initialize(true);
	nvicfiq.initialize(true);
	vicvectaddrout.initialize(0);

	for ( unsigned int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		std::stringstream vicintsource_name;
		vicintsource_name << "vicintsource[" << i << "]";
		vicintsource[i] = 
			new sc_core::sc_in<bool>(vicintsource_name.str().c_str());
	}

	bus_target_socket.register_nb_transport_fw(this,
			&VIC::bus_target_nb_transport_fw);
	bus_target_socket.register_b_transport(this,
			&VIC::bus_target_b_transport);
	bus_target_socket.register_get_direct_mem_ptr(this,
			&VIC::bus_target_get_direct_mem_ptr);
	bus_target_socket.register_transport_dbg(this,
			&VIC::bus_target_transport_dbg);

	// init the registers values
	memset(regs, 0, 0x01000UL);
	regs[0x0fe0UL] = 0x090;
	regs[0x0fe4UL] = 0x011;
	regs[0x0fe8UL] = 0x04;
	regs[0x0fecUL] = 0;
	regs[0x0ff0UL] = 0x0d;
	regs[0x0ff4UL] = 0x0f0;
	regs[0x0ff8UL] = 0x05;
	regs[0x0ffcUL] = 0x0b1;

	for ( unsigned int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		std::stringstream id_name;

		id_name << "VICIntSourceIdentifier[" << i << "]";
		source_identifier_method[i] =
			new VICIntSourceIdentifier(id_name.str().c_str(),
					i, this);
		source_identifier_method[i]->vicinttarget(*vicintsource[i]);
	}
	
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

VIC ::
~VIC()
{
	for ( unsigned int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		delete source_identifier_method[i];
		delete vicintsource[i];
	}
	
	for ( unsigned int i = 0; i < NUMREGS; i++ )
	{
		delete regs_service[i];
		regs_service[i] = 0;
		delete regs_accessor[i];
		regs_accessor[i] = 0;
	}
}

bool 
VIC ::
BeginSetup()
{
	return true;
}

/** Status update thread handler
 * Basically it just waits for update status events and updates the status of
 *   the VIC depending on all the possible entries by calling the UpdateStatus
 *   method.
 */
void
VIC ::
UpdateStatusHandler()
{
	while ( 1 )
	{
		wait(update_status_event);
		if ( VERBOSE(V0, V_STATUS) )
		{
			logger << DebugInfo
				<< "Starting status update."
				<< EndDebugInfo;
		}
		UpdateStatus();
		if ( VERBOSE(V0, V_STATUS) )
		{
			logger << DebugInfo
				<< "Status update finished."
				<< EndDebugInfo;
		}
	}
}

/** Update the status of the VIC depending on all the possible entries
 */
void
VIC ::
UpdateStatus()
{
	// first we must lock the state to be sure we are the only ones
	//   modifying it
	state_semaphore.wait();

	// apply software interrupt register and the values of 
	//   interrupt sources to the raw interrupt register
	uint32_t softint = GetRegister(VICSOFTINTAddr);
	uint32_t old_rawintr = GetRegister(VICRAWINTRAddr);
	uint32_t rawintr = 0;
	for ( unsigned int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		rawintr |= (softint | int_source) & (0x01UL << i);
	}
	SetRegister(VICRAWINTRAddr, rawintr);
	if ( VERBOSE(V0, V_STATUS) && (old_rawintr != rawintr) )
		logger << DebugInfo
			<< "Updated raw interrupt status register:" << std::endl
			<< " - old value = 0x" << std::hex << old_rawintr << std::endl
			<< " - new value = 0x" << rawintr << std::endl
			<< " - intsource = 0x" << int_source << std::endl
			<< " - softint   = 0x" << softint << std::dec
			<< EndDebugInfo;

	// compute the new fiq and irq status
	uint32_t intselect = GetRegister(VICINTSELECTAddr);
	uint32_t intenable = GetRegister(VICINTENABLEAddr);
	uint32_t old_fiqstatus = GetRegister(VICFIQSTATUSAddr);
	uint32_t old_irqstatus = GetRegister(VICIRQSTATUSAddr);
	uint32_t fiqstatus = 0;
	uint32_t irqstatus = 0;
	for ( unsigned int i = 0; i < NUM_SOURCE_INT; i++ )
	{
		uint32_t current = (rawintr & intenable) & (0x01UL << i);
		if ( intselect & (0x01UL << i) )
			fiqstatus |= current;
		else
			irqstatus |= current;
	}
	SetRegister(VICFIQSTATUSAddr, fiqstatus);
	SetRegister(VICIRQSTATUSAddr, irqstatus);
	if ( VERBOSE(V0, V_STATUS) )
	{
		bool msg_fiq = (fiqstatus != old_fiqstatus);
		bool msg_irq = (irqstatus != old_irqstatus);
		if ( msg_fiq || msg_irq )
			logger << DebugInfo
				<< "Updated interrupt status:" << std::endl
				<< " - FIQSTATUS = 0x" << std::hex << old_fiqstatus
				<< " -> 0x" << fiqstatus << std::endl
				<< " - IRQSTATUS = 0x" << old_irqstatus
				<< " -> 0x" << irqstatus << std::endl
				<< "Other related registers:" << std::endl
				<< " - INTSELECT = 0x" << intselect << std::endl
				<< " - INTENABLE = 0x" << intenable
				<< std::dec
				<< EndDebugInfo;
	}

	// compute the new nVICFIQ
	bool old_nvicfiq_value = nvicfiq_value;
	if ( (!nvicfiqin) || fiqstatus )
		nvicfiq_value = false;
	else
		nvicfiq_value = true;
	if ( VERBOSE(V0, V_STATUS) )
	{
		if ( old_nvicfiq_value != nvicfiq_value )
			logger << DebugInfo
				<< "Updated nvicfiq:" << std::endl
				<< " - nvicfiq = " << old_nvicfiq_value
				<< " -> " << nvicfiq_value << std::endl
				<< "Related signals and registers:" << std::endl
				<< " - nvicfiqin = " << nvicfiqin << std::endl
				<< " - FIQSTATUS = 0x" << std::hex << fiqstatus
				<< std::dec
				<< EndDebugInfo;
	}

	// compute the VECTIRQn signals
	//   and set the VICVECTADDR if required
	bool vectirq[15];
	bool priority = false;
	uint32_t nonvect_irqs = 0;
	bool old_vect_int_for_service = vect_int_for_service;
	vect_int_for_service = false;
	uint32_t old_vicvectaddr = GetRegister(VICVECTADDRAddr);
	uint32_t new_vicvectaddr = 0;
	unsigned int old_max_vect_int_for_service = max_vect_int_for_service;

	for ( unsigned int i = 0; i < NUM_VECT_INT; i++ )
	{
		uint32_t vectcntl = GetRegister(VICVECTCNTLBaseAddr, i);
		uint32_t src = VECTCNTLSource(vectcntl);

		vectirq[i] = false;

		if ( (vect_int_serviced == 0) ||
				(vect_int_serviced && 
				 (vect_int_serviced_level[vect_int_serviced - 1] > i))
			)
		{
			if ( VECTCNTLEnable(vectcntl) )
			{
				// prepare the nonvectored irqs
				nonvect_irqs |= (0x01UL << src);
				if ( priority )
					vectirq[i] = false;
				else
				{
					if ( irqstatus & (0x01UL << src) )
					{
						vect_int_for_service = true;
						max_vect_int_for_service = i;
						vectirq[i] = true;
						priority = true;
						new_vicvectaddr = GetRegister(VICVECTADDRBaseAddr, i);
						SetRegister(VICVECTADDRAddr, new_vicvectaddr);
					}
				}
			}
		}
	}

	if ( VERBOSE(V0, V_STATUS) )
	{
		if ( vect_int_for_service != old_vect_int_for_service )
		{
			if ( vect_int_for_service )
				logger << DebugInfo
					<< "Set vector interrupt for service:" << std::endl
					<< " - vector interrupt for service = " 
					<< max_vect_int_for_service << std::endl
					<< " - VICVECTADDR = 0x" << std::hex << old_vicvectaddr
					<< " -> 0x" << new_vicvectaddr << std::dec
					<< EndDebugInfo;
			else // !vect_int_serviced
				logger << DebugInfo
					<< "Removed vector interrupt for service:" << std::endl
					<< " - old vector interrupt for service = "
					<< max_vect_int_for_service
					<< EndDebugInfo;
		}
		else
		{
			// Note: even if we don't know if a vector interrupt for service
			//   is ready or not the below checked variables only change if one
			//   vector interrupt is ready for service
			// maybe the max vector interrupt for serviced changed
			if ( old_max_vect_int_for_service != max_vect_int_for_service )
			{
				logger << DebugInfo
					<< "Changed the vector interrupt for service:" << std::endl
					<< " - vector interrupt for service = "
					<< old_max_vect_int_for_service
					<< " -> " << max_vect_int_for_service << std::endl
					<< " - VICVECTADDR = 0x" << std::hex << old_vicvectaddr
					<< " -> 0x" << new_vicvectaddr << std::dec
					<< EndDebugInfo;
			}
			else
			{
				// or the VECTORADDR did change
				if ( old_vicvectaddr != new_vicvectaddr )
				{
					logger << DebugInfo
						<< "Changed current vector address register, but not"
						<< " the vector interrupt for service:" << std::endl
						<< " - vector interrupt for service = "
						<< max_vect_int_for_service << std::endl
						<< " - VICVECTADDR = 0x" << std::hex << old_vicvectaddr
						<< " -> 0x" << new_vicvectaddr << std::dec
						<< EndDebugInfo;
				}
			}
		}
	}

	// if there is no interrupt being serviced and no 
	//   vectored irq generated then
	//   a nonvectored irq can be raised
	// also set the default vector addr if none was detected
	bool nonvectored_irq = false;
	if ( (vect_int_serviced == 0) && !priority )
	{
		new_vicvectaddr = GetRegister(VICDEFVECTADDRAddr);
		SetRegister(VICVECTADDRAddr, new_vicvectaddr);
		if ( irqstatus & ~nonvect_irqs )
		{
			vect_int_for_service = true;
			max_vect_int_for_service = NUM_VECT_INT;
			nonvectored_irq = true;
			priority = true;
		}
	}
	
	if ( VERBOSE(V0, V_STATUS) )
	{
		if ( (old_max_vect_int_for_service != max_vect_int_for_service) &&
				nonvectored_irq )
		{
			logger << DebugInfo 
				<< "A non vectored interrupt is being generated:"
				<< " - non vectored interrupt mask = 0x" << std::hex
				<< nonvect_irqs << std::endl
				<< " - VICVECTADDR = 0x" << old_vicvectaddr
				<< " -> 0x" << new_vicvectaddr << std::dec
				<< EndDebugInfo;
		}
		else
		{
			// maybe we just set the default vector address
			logger << DebugInfo
				<< "Setting vector address register:" << std::endl
				<< " - VICVECTADDR = 0x" << std::hex << old_vicvectaddr
				<< " -> 0x" << new_vicvectaddr << std::dec
				<< EndDebugInfo;
		}
	}

	// if a vectored or nonvectored irq was generated
	//   then set the nVICIRQ signal, otherwise set it 
	//   if an external IRQ was generated
	// NOTE: the nvicirq_value is inverted
	bool old_nvicirq_value = nvicirq_value;
	bool old_forwarding_nvicirqin = forwarding_nvicirqin;
	bool old_nvicirqin_value = nvicirqin_value;
	forwarding_nvicirqin = false;
	nvicirq_value = true;
	old_vicvectaddr = GetRegister(VICVECTADDRAddr);
	new_vicvectaddr = old_vicvectaddr;
	if ( priority )
		nvicirq_value = false;
	else
	{
		// no interrupts in the VIC, just forward the nvicirqin
		forwarding_nvicirqin = true;
		if ( nvicirqin )
		{
			nvicirqin_value = true;
			vect_int_for_service = false;
		}
		else
		{
			nvicirqin_value = false;
			vect_int_for_service = true;
			max_vect_int_for_service = NUM_VECT_INT;
			nvicirq_value = false;
			new_vicvectaddr = vicvectaddrin;
			SetRegister(VICVECTADDRAddr, new_vicvectaddr);
		}
	}

	if ( VERBOSE(V0, V_STATUS) )
	{
		if ( priority )
		{
			logger << DebugInfo
				<< "Found irq interrupt, setting nvicirq (" << nvicirq_value << ")"
				<< EndDebugInfo;
			if ( nvicirq_value != old_nvicirq_value )
			{
				logger << DebugInfo
					<< "Setting nvic_irq to " << nvicirq_value
					<< EndDebugInfo;
			}
		}
		else if ( forwarding_nvicirqin != old_forwarding_nvicirqin )
		{
			logger << DebugInfo
				<< "Forwarding nvicirq signal:" << std::endl
				<< " - nvicirqin = " << old_nvicirqin_value
				<< " -> " << nvicirqin_value << std::endl
				<< " - VICVECTADDR = 0x" << std::hex << old_vicvectaddr
				<< " -> 0x" << new_vicvectaddr << std::dec
				<< EndDebugInfo;
		}
	}

	// correctly set the output values
	nvicfiq = nvicfiq_value;
	nvicirq = nvicirq_value;
	vicvectaddrout = new_vicvectaddr;
	if ( VERBOSE(V0, V_OUTIRQ) )
	{
		if ( nvicirq_value != old_nvicirq_value )
		{
			logger << DebugInfo
				<< "Changing nvicirq value to " << nvicirq_value << std::endl
				<< " - vicvectaddrout = 0x" << std::hex
				<< old_vicvectaddr << " => 0x" << new_vicvectaddr << std::dec
				<< EndDebugInfo;
		}
		else if ( new_vicvectaddr != old_vicvectaddr )
		{
			logger << DebugInfo
				<< "Changing the address on vicvectaddrout:" << std::endl
				<< " - vicvectaddrout = 0x" << std::hex
				<< old_vicvectaddr << " => 0x" << new_vicvectaddr << std::dec
				<< EndDebugInfo;
		}
	}

	if ( VERBOSE(V0, V_OUTFIQ) )
	{
		if ( nvicfiq_value != old_nvicfiq_value )
		{
			logger << DebugInfo
				<< "Changing nvicfiq value to " << nvicfiq_value
				<< EndDebugInfo;
		}
	}

	// unlock the state
	state_semaphore.post();
}

/** Source interrupt handling */
void 
VIC ::
VICIntSourceReceived(int id, bool value)
{
	// lock the VIC state
	state_semaphore.wait();

	uint32_t orig = int_source;
	if ( value )
		int_source |= (0x01UL << id);
	else
		int_source &= ~(0x01UL << id);
	if ( VERBOSE(V0, V_INIRQ) )
	{
		logger << DebugInfo
			<< "Received VICIntSource[" << id << "]"
			<< " signal change:" << std::endl
			<< " - old VICIntSource = 0x"
			<< std::hex << orig << std::endl
			<< " - new VICIntSource = 0x"
			<< int_source << std::dec
			<< EndDebugInfo;
	}

	// unlock the state
	state_semaphore.post();

	// UpdateStatus();
	
	if ( VERBOSE(V0, V_STATUS) )
	{
		logger << DebugInfo
			<< "Notifying that the status must be updated."
			<< EndDebugInfo;
	}

	update_status_event.notify(SC_ZERO_TIME);
}

/**************************************************************************/
/* Virtual methods for the target socket for the bus connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
VIC ::
bus_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
VIC ::
bus_target_b_transport(transaction_type &trans, sc_core::sc_time &delay)
{
	wait(delay);
	delay = SC_ZERO_TIME;

	// first we need to lock the statue of the VIC
	state_semaphore.wait();

	uint32_t addr = trans.get_address() - base_addr;
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();
	bool handled = false;

	if ( is_read )
	{
		if ( (0x01000L - size) >= addr )
		{
			memcpy(data, &(regs[addr]), size);
			for ( unsigned int i = 0; i < size; i += 4 )
			{
				uint32_t align_addr = (addr + i) & ~0x03UL;
				if ( align_addr == VICIRQSTATUSAddr )
					handled = true;
				
				else if ( align_addr == VICVECTADDRAddr )
				{
					if ( vect_int_for_service )
					{
						vect_int_serviced_level[vect_int_serviced] =
							max_vect_int_for_service;
						vect_int_serviced++;
						vect_int_for_service = false;
					}
					handled = true;
				}

				else if ( (align_addr == VICPERIPHIDBaseAddr) ||
						(align_addr == (VICPERIPHIDBaseAddr + 4)) ||
						(align_addr == (VICPERIPHIDBaseAddr + 8)) ||
						(align_addr == (VICPERIPHIDBaseAddr + 12)) )
				{
					handled = true;
				}
			}
		}
	}
	else // writing
	{
		if ( (0x01000L - size) >= addr )
		{
			uint32_t vicirqstatus = GetRegister(VICIRQSTATUSAddr);
			uint32_t vicfiqstatus = GetRegister(VICFIQSTATUSAddr);
			bool should_update_status = false;
			memcpy(&(regs[addr]), data, size);
			for ( unsigned int i = 0; i < size; i += 4 )
			{
				uint32_t align_addr = (addr + i) & ~0x03UL;
				if ( align_addr == VICIRQSTATUSAddr )
				{
					SetRegister(VICIRQSTATUSAddr, vicirqstatus);
					logger << DebugWarning
						<< "Trying to modify read only register"
						<< " VICIRQSTATUS."
						<< EndDebugWarning;
					handled = true;
				}

				else if ( align_addr == VICFIQSTATUSAddr )
				{
					SetRegister(VICFIQSTATUSAddr, vicfiqstatus);
					logger << DebugWarning
						<< "Trying to modify read only register"
						<< " VICFIQSTATUS."
						<< EndDebugWarning;
					handled = true;
				}

				else if ( align_addr == VICINTSELECTAddr )
				{
					should_update_status = true;
					handled = true;
				}

				else if ( align_addr == VICINTENABLEAddr )
				{
					should_update_status = true;
					handled = true;
				}
				
				else if ( align_addr == VICINTENCLEARAddr )
				{
					uint32_t vicintenclear = GetRegister(VICINTENCLEARAddr);
					uint32_t vicintenable = GetRegister(VICINTENABLEAddr);
					for ( unsigned int index = 0;
							index < NUM_SOURCE_INT;
							index++ )
					{
						vicintenable = 
							(vicintenable & ~(0x01UL << index)) ||
							((vicintenable & ~vicintenclear) &
							 (0x01UL << index));
					}
					// reset the value of VICINTENCLEAR
					SetRegister(VICINTENCLEARAddr, 0);
					SetRegister(VICINTENABLEAddr, vicintenable);
					should_update_status = true;
					handled = true;
				}

				else if ( align_addr == VICSOFTINTCLEARAddr )
				{
					uint32_t vicsoftintclear = GetRegister(VICSOFTINTCLEARAddr);
					uint32_t vicsoftint = GetRegister(VICSOFTINTAddr);
					for ( unsigned int index = 0;
							index < NUM_SOURCE_INT;
							index++ )
					{
						vicsoftint =
							(vicsoftint & ~(0x01UL << index)) ||
							((vicsoftint & ~vicsoftintclear) &
							 (0x01UL << index));
					}
					// reset the value of VICSOFTINTCLEAR
					SetRegister(VICSOFTINTCLEARAddr, 0);
					SetRegister(VICSOFTINTAddr, vicsoftint);
					should_update_status = true;
					handled = true;
				}

				else if ( align_addr == VICVECTADDRAddr )
				{
					if ( vect_int_serviced != 0 )
					{
						vect_int_serviced_level[vect_int_serviced - 1] = 0;
						vect_int_serviced--;
						should_update_status = true;
					}
					else
					{
						// should_update_status = false;
					}
					handled = true;
				}

				else if ( align_addr == VICDEFVECTADDRAddr )
				{
					handled = true;
				}

				else if ( (align_addr >= VICVECTCNTLBaseAddr) &&
							(align_addr < 
							 (VICVECTCNTLBaseAddr + (4 * NUM_VECT_INT))) )
				{
					should_update_status = true;
					handled = true;
				}

				else if ( align_addr == VICITCRAddr )
				{
					uint32_t vicitcr = GetRegister(VICITCRAddr);
					if ( vicitcr & 0x01UL )
					{
						logger << DebugError
							<< "TODO: handle activation of test mode"
							<< EndDebugError;
						unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
					}
					SetRegister(VICITCRAddr, vicitcr & 0x01UL);
					handled = true;
				}
			}
			if ( should_update_status )
				// UpdateStatus();
				update_status_event.notify(SC_ZERO_TIME);
		}
	}

	if ( !handled )
	{
		logger << DebugError
			<< "Access to VIC:" << std::endl
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
			<< "Access to VIC:" << std::endl
			<< " - read = " << is_read << std::endl
			<< " - addr = 0x" << std::hex << addr << std::dec << std::endl
			<< " - size = " << size << std::endl
			<< " - data =" << std::hex;
		for ( unsigned int i = 0; i < size; i++ )
			logger << " " << (unsigned int)data[i];
		logger << std::dec
			<< EndDebugInfo;
	}

	// we can unlock the state of the VIC
	state_semaphore.post();
}

bool 
VIC ::
bus_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return false;
}

unsigned int 
VIC ::
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

/** Returns the enable field of the VICVECTCNTL register.
 *
 * @param value the register value
 * @return true if enable is set, false otherwise
 */
bool 
VIC ::
VECTCNTLEnable(uint32_t value)
{
	if ( value & (0x01UL << 5) )
		return true;
	return false;
}

/** Returns the source field of the VICVECTCNTL register
 *
 * @param value the register value
 * @return the value of the source field
 */
uint32_t 
VIC ::
VECTCNTLSource(uint32_t value)
{
	return value & 0x01fUL;
}

/** Get interface for the generic peripheral register interface
 *
 * @param addr the address to consider
 * @return the value of the register pointed by the address
 */
uint32_t
VIC ::
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
VIC ::
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
VIC ::
GetRegister(uint32_t addr) const
{
	const uint8_t *data = &(regs[addr]);
	uint32_t value = 0;

	memcpy(&value, data, 4);
	value = LittleEndian2Host(value);
	return value;
}

/** Returns the register pointed by the given address plus index * 4
 *
 * @param addr the base address to consider
 * @param index the register index from the base address
 * @return the value of the register pointed by address + (index * 4)
 */
uint32_t 
VIC ::
GetRegister(uint32_t addr, uint32_t index) const
{
	return GetRegister(addr + (index * 4));
}

/** Sets the register pointed by the given address
 *
 * @param addr the address to consider
 * @param value the value to set the register
 */
void
VIC ::
SetRegister(uint32_t addr, uint32_t value)
{
	uint8_t *data = &(regs[addr]);

	value = Host2LittleEndian(value);
	memcpy(data, &value, 4);
}

/**************************************************************************/
/* Methods to get and set the registers                               END */
/**************************************************************************/

} // end of namespace vic
} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


