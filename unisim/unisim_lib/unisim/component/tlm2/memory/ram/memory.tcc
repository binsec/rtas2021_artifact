/*
 *  Copyright (c) 2007,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_RAM_MEMORY_TCC__
#define __UNISIM_COMPONENT_TLM2_MEMORY_RAM_MEMORY_TCC__

#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace ram {

//using unisim::service::interfaces::operator<<;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

/* Constructor */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
Memory(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent, "this module implements a memory")
	, sc_core::sc_module(name)
	, unisim::component::cxx::memory::ram::Memory<ADDRESS, PAGE_SIZE>(name, parent)
	, slave_sock("slave-sock")
	, read_counter(0)
	, write_counter(0)
	, cycle_time()
	, read_latency(cycle_time)
	, write_latency(sc_core::SC_ZERO_TIME)
	, read_only(false)
	, enable_dmi(true)
	, param_cycle_time("cycle-time", this, cycle_time, "memory cycle time")
	, param_read_latency("read-latency", this, read_latency, "memory read latency")
	, param_write_latency("write-latency", this, write_latency, "memory write latency")
	, param_read_only("read-only", this, read_only, "enable/disable read-only protection")
	, param_enable_dmi("enable-dmi", this, enable_dmi, "Enable/Disable TLM 2.0 DMI (Direct Memory Access) to speed-up simulation")
	, stat_read_counter("read-counter", this, read_counter, "read access counter (not accurate when using SystemC TLM 2.0 DMI)")
	, stat_write_counter("write-counter", this, write_counter, "write access counter (not accurate when using SystemC TLM 2.0 DMI)")
	, burst_latency_lut()
{
	slave_sock(*this);
	
	stat_read_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	stat_write_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
}

/* Destructor */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
~Memory() {
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
ResetMemory() {
	inherited::ResetMemory();

	read_counter = 0;
	write_counter = 0;
}
/* ClientIndependentSetup */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
BeginSetup() {
	if(IsVerbose())
		this->logger << DebugInfo << LOCATION
			<< " cycle time of " << cycle_time 
			<< std::endl << EndDebugInfo;
	if(cycle_time == sc_core::SC_ZERO_TIME) {
		this->logger << DebugError << LOCATION
				<< "cycle time must be different than 0" << std::endl
				<< EndDebugError;
		return false;
	}
	
	burst_latency_lut.SetBaseLatency(cycle_time);
	
	return unisim::component::cxx::memory::ram::Memory<ADDRESS, PAGE_SIZE>::BeginSetup();
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void 
Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
UpdateTime(unsigned int data_length, const sc_core::sc_time& latency, sc_core::sc_time& t)
{
	if(data_length)
	{
		unsigned int data_bus_word_length = ((data_length * 8) + BUSWIDTH - 1) / BUSWIDTH;
		do
		{
			if(data_bus_word_length <= BURST_LENGTH)
			{
				t += latency + burst_latency_lut.Lookup(data_bus_word_length);
				data_bus_word_length = 0;
			}
			else
			{
				t += latency + burst_latency_lut.Lookup(BURST_LENGTH);
				data_bus_word_length -= BURST_LENGTH;
			}
		}
		while(data_bus_word_length);
	}
}

/* TLM2 Slave methods */
template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
bool Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	if(enable_dmi)
	{
		// tlm::tlm_command cmd = payload.get_command();
		ADDRESS addr = payload.get_address();
		ADDRESS dmi_start_addr = addr;
		ADDRESS dmi_end_addr = addr;

		unsigned char *dmi_ptr = (unsigned char *) inherited::GetDirectAccess(addr, dmi_start_addr, dmi_end_addr);

		dmi_data.set_start_address(dmi_start_addr);
		dmi_data.set_end_address(dmi_end_addr);
		dmi_data.set_granted_access(read_only ? tlm::tlm_dmi::DMI_ACCESS_READ : tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);

		if(dmi_ptr)
		{
			//std::cerr << sc_module::name() << ": grant 0x" << std::hex << dmi_start_addr << "-0x" << dmi_end_addr << std::dec << std::endl;
			dmi_data.set_dmi_ptr(dmi_ptr);
			// set latency per byte (with a typical burst of BURST_LENGTH * BUSWIDTH bits)
			sc_core::sc_time read_burst_time = (BURST_LENGTH * cycle_time) + read_latency;
			sc_core::sc_time write_burst_time = (BURST_LENGTH * cycle_time) + write_latency;
			dmi_data.set_read_latency(read_burst_time / ((BURST_LENGTH * BUSWIDTH) / 8));
			dmi_data.set_write_latency(write_burst_time / ((BURST_LENGTH * BUSWIDTH) / 8));
			return true;
		}
	}
	else
	{
		// Deny direct memory interface access
		dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
		dmi_data.set_start_address(0);
		dmi_data.set_end_address(sc_dt::uint64(-1));
	}

	return false;
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
unsigned int Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::transport_dbg(tlm::tlm_generic_payload& payload)
{
	payload.set_dmi_allowed(true);

	tlm::tlm_command cmd = payload.get_command();
	uint64_t addr = payload.get_address();
	unsigned char *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();
	unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload.get_byte_enable_length() : 0;
	unsigned int streaming_width = payload.get_streaming_width();
	bool status = false;

	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			if(IsVerbose())
			{
				this->logger << DebugInfo << LOCATION
					<< ":" << sc_core::sc_time_stamp().to_string()
					<< ": received a TLM_READ_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugInfo;
			}

			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::ReadMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::ReadMemory(addr, data_ptr, data_length);

			break;
		case tlm::TLM_WRITE_COMMAND:
			if(IsVerbose())
			{
				this->logger << DebugInfo << LOCATION
					<< ":" << sc_core::sc_time_stamp().to_string()
					<< ": received a TLM_WRITE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugInfo;
			}
			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::WriteMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::WriteMemory(addr, data_ptr, data_length);

			break;
		case tlm::TLM_IGNORE_COMMAND:
			// transport_dbg should not receive such a command
			this->logger << DebugWarning << LOCATION
					<< ":" << sc_core::sc_time_stamp().to_string() 
					<< " : received an unexpected TLM_IGNORE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugWarning;
			Object::Stop(-1);
			break;
	}

	if(status)
		payload.set_response_status(tlm::TLM_OK_RESPONSE);
	else
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);

	return status ? data_length : 0;
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
tlm::tlm_sync_enum Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase != tlm::BEGIN_REQ)
	{
		this->logger << DebugWarning << LOCATION
				<< ":" << (sc_core::sc_time_stamp() + t).to_string() 
				<< " : received an unexpected phase " << phase << std::endl
				<< EndDebugWarning;
		Object::Stop(-1);
	}

	payload.set_dmi_allowed(true);

	tlm::tlm_command cmd = payload.get_command();
	uint64_t addr = payload.get_address();
	unsigned char *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();
	unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload.get_byte_enable_length() : 0;
	unsigned int streaming_width = payload.get_streaming_width();
	bool status = false;

	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			if(IsVerbose())
			{
				this->logger << DebugInfo << LOCATION
					<< ":" << (sc_core::sc_time_stamp() + t).to_string()
					<< ": received a TLM_READ_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugInfo;
			}

			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)))
				status = inherited::ReadMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::ReadMemory(addr, data_ptr, data_length);
			
			if (status) {
				read_counter++;
			}

			UpdateTime(data_length, read_latency, t);
			break;
		case tlm::TLM_WRITE_COMMAND:
			if(IsVerbose())
			{
				this->logger << DebugInfo << LOCATION
					<< ":" << (sc_core::sc_time_stamp() + t).to_string()
					<< ": received a TLM_WRITE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugInfo;
			}
			
			if(read_only)
			{
				status = true;
				this->logger << DebugWarning << (sc_core::sc_time_stamp() + t).to_string()
					<< ": Attempt to write into a read-only memory at @0x" << std::hex << addr << std::dec
					<< EndDebugWarning;
			}
			else
			{
				if(byte_enable_length ||(streaming_width && (streaming_width != data_length)))
					status = inherited::WriteMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
				else
					status = inherited::WriteMemory(addr, data_ptr, data_length);
				
				if (status) {
					write_counter++;
				}
			}

			UpdateTime(data_length, write_latency, t);
			break;
		case tlm::TLM_IGNORE_COMMAND:
			if(IsVerbose())
			{
				this->logger << DebugInfo << LOCATION
					<< ":" << (sc_core::sc_time_stamp() + t).to_string()
					<< ": received a TLM_IGNORE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugInfo;
			}
			status = true;
			break;
	}

	payload.set_response_status(tlm::TLM_OK_RESPONSE);
	phase = tlm::BEGIN_RESP;

	if(status)
		payload.set_response_status(tlm::TLM_OK_RESPONSE);
	else
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);

	return tlm::TLM_COMPLETED;
}

template <unsigned int BUSWIDTH, class ADDRESS, unsigned int BURST_LENGTH, uint32_t PAGE_SIZE, bool DEBUG>
void 
Memory<BUSWIDTH, ADDRESS, BURST_LENGTH, PAGE_SIZE, DEBUG>::
b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	payload.set_dmi_allowed(true);

	tlm::tlm_command cmd = payload.get_command();
	uint64_t addr = payload.get_address();
	unsigned char *data_ptr = payload.get_data_ptr();
	unsigned int data_length = payload.get_data_length();
	unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
	unsigned int byte_enable_length = byte_enable_ptr ? payload.get_byte_enable_length() : 0;
	unsigned int streaming_width = payload.get_streaming_width();
	bool status = false;

	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			if(IsVerbose())
			{
				this->logger << DebugInfo << LOCATION
					<< ":" << (sc_core::sc_time_stamp() + t).to_string()
					<< ": received a TLM_READ_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugInfo;
			}

			if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
				status = inherited::ReadMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
			else
				status = inherited::ReadMemory(addr, data_ptr, data_length);

			if (status) {
				read_counter++;
			}

			if (status && IsVerbose())
			{
				this->logger << DebugInfo << LOCATION
					<< ": raw data read (0x" << std::hex << addr << std::dec
					<< ", " << data_length << ") = 0x";
				for (unsigned int i = 0; i < data_length; i++)
					this->logger << "0x" << std::hex << (unsigned int)(uint8_t)data_ptr[i] << " " << std::dec;
				this->logger << EndDebugInfo;
			}
			UpdateTime(data_length, read_latency, t);
			break;
		case tlm::TLM_WRITE_COMMAND:
			if(IsVerbose())
			{
				this->logger << DebugInfo << LOCATION
					<< ":" << (sc_core::sc_time_stamp() + t).to_string()
					<< ": received a TLM_WRITE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugInfo;
			}

			if(read_only)
			{
				status = true;
				this->logger << DebugWarning << (sc_core::sc_time_stamp() + t).to_string()
					<< ": Attempt to write into a read-only memory at @0x" << std::hex << addr << std::dec
					<< EndDebugWarning;
			}
			else
			{
				if (IsVerbose())
				{
					this->logger << DebugInfo << LOCATION
						<< ": raw data written (0x" << std::hex << addr << std::dec
						<< ", " << data_length << ") = 0x";
					for (unsigned int i = 0; i < data_length; i++)
						this->logger << "0x" << std::hex << (unsigned int)(uint8_t)data_ptr[i] << " " << std::dec;
					this->logger << EndDebugInfo;
				}
				if(byte_enable_length ||(streaming_width && (streaming_width != data_length)) )
					status = inherited::WriteMemory(addr, data_ptr, data_length, byte_enable_ptr, byte_enable_length, streaming_width);
				else
					status = inherited::WriteMemory(addr, data_ptr, data_length);
	
				if (status) {
					write_counter++;
				}
			}

			UpdateTime(data_length, write_latency, t);
			break;
		case tlm::TLM_IGNORE_COMMAND:
			if(IsVerbose())
			{
				this->logger << DebugInfo << LOCATION
					<< ":" << (sc_core::sc_time_stamp() + t).to_string()
					<< ": received a TLM_IGNORE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length"
					<< EndDebugInfo;
			}
			status = true;
			break;
	}

	if(status)
		payload.set_response_status(tlm::TLM_OK_RESPONSE);
	else
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
}

} // end of namespace ram
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_RAM_MEMORY_TCC__
