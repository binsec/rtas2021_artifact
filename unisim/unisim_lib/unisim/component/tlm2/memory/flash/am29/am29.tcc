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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_AM29_AM29_TCC__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_AM29_AM29_TCC__

#define LOCATION __FUNCTION__ << ":" << __FILE__ << ":" <<  __LINE__ << ": "

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace am29 {

using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, unsigned int BUSWIDTH>
AM29<CONFIG, BYTESIZE, IO_WIDTH, BUSWIDTH>::AM29(const sc_core::sc_module_name& name, Object *parent)
	: Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::component::cxx::memory::flash::am29::AM29<CONFIG, BYTESIZE, IO_WIDTH>(name, parent)
	, cycle_time(sc_core::SC_ZERO_TIME)
	, param_cycle_time("cycle-time", this, cycle_time, "Cycle time")
{
	slave_sock(*this);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, unsigned int BUSWIDTH>
AM29<CONFIG, BYTESIZE, IO_WIDTH, BUSWIDTH>::~AM29()
{
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, unsigned int BUSWIDTH>
bool AM29<CONFIG, BYTESIZE, IO_WIDTH, BUSWIDTH>::BeginSetup()
{
	return inherited::BeginSetup();
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, unsigned int BUSWIDTH>
bool AM29<CONFIG, BYTESIZE, IO_WIDTH, BUSWIDTH>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address((sc_dt::uint64) -1);
	return false;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, unsigned int BUSWIDTH>
unsigned int AM29<CONFIG, BYTESIZE, IO_WIDTH, BUSWIDTH>::transport_dbg(tlm::tlm_generic_payload& payload)
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

	if(cmd != tlm::TLM_IGNORE_COMMAND)
	{
		if(streaming_width && (streaming_width != data_length))
		{
			// streaming is not supported
			inherited::logger << DebugError << LOCATION
				<< sc_core::sc_time_stamp()
				<< ": streaming width of " << streaming_width << " bytes is unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return 0;
		}
		else if(byte_enable_length)
		{
			// byte enable is not supported
			inherited::logger << DebugError << LOCATION
				<< sc_core::sc_time_stamp()
				<< ": byte enable is unsupported"
				<< EndDebugError;
			Object::Stop(-1);
			return 0;
		}
	}
	
	switch(cmd)
	{
		case tlm::TLM_READ_COMMAND:
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << LOCATION
					<< sc_core::sc_time_stamp()
					<< ": received a TLM_READ_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}

			status = inherited::ReadMemory(addr, data_ptr, data_length);
			break;
		case tlm::TLM_WRITE_COMMAND:
			if(inherited::IsVerbose())
			{
				inherited::logger << DebugInfo << LOCATION
					<< sc_core::sc_time_stamp()
					<< ": received a TLM_WRITE_COMMAND payload at 0x"
					<< std::hex << addr << std::dec
					<< " of " << data_length << " bytes in length" << std::endl
					<< EndDebugInfo;
			}

			status = inherited::WriteMemory(addr, data_ptr, data_length);
			break;
		default:
			inherited::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}

	if(status)
		payload.set_response_status(tlm::TLM_OK_RESPONSE);
	else
		payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);

	return status ? data_length : 0;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, unsigned int BUSWIDTH>
tlm::tlm_sync_enum AM29<CONFIG, BYTESIZE, IO_WIDTH, BUSWIDTH>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				
				payload.set_dmi_allowed(true);

				tlm::tlm_command cmd = payload.get_command();
				uint64_t addr = payload.get_address();
				unsigned char *data_ptr = payload.get_data_ptr();
				unsigned int data_length = payload.get_data_length();
				unsigned char *byte_enable_ptr = payload.get_byte_enable_ptr();
				unsigned int byte_enable_length = byte_enable_ptr ? payload.get_byte_enable_length() : 0;
				unsigned int streaming_width = payload.get_streaming_width();
				
				if(cmd != tlm::TLM_IGNORE_COMMAND)
				{
					if(streaming_width && (streaming_width != data_length))
					{
						// streaming is not supported
						inherited::logger << DebugError << LOCATION
							<< (sc_core::sc_time_stamp() + t)
							<< ": streaming width of " << streaming_width << " bytes is unsupported"
							<< EndDebugError;
						Object::Stop(-1);
						return tlm::TLM_COMPLETED;
					}
					else if(byte_enable_length)
					{
						// byte enable is not supported
						inherited::logger << DebugError << LOCATION
							<< (sc_core::sc_time_stamp() + t)
							<< ": byte enable is unsupported"
							<< EndDebugError;
						Object::Stop(-1);
						return tlm::TLM_COMPLETED;
					}
				}
				
				bool status = false;

				switch(cmd)
				{
					case tlm::TLM_READ_COMMAND:
						if(inherited::IsVerbose())
						{
							inherited::logger << DebugInfo << LOCATION
								<< sc_core::sc_time_stamp().to_string()
								<< ": received a TLM_READ_COMMAND payload at 0x"
								<< std::hex << addr << std::dec
								<< " of " << data_length << " bytes in length" << std::endl
								<< EndDebugInfo;
						}

						status = inherited::FSM(unisim::component::cxx::memory::flash::am29::CMD_READ, addr, (uint8_t *) data_ptr, data_length);
						
						if(!status) memset(data_ptr, 0, data_length);
						break;
					case tlm::TLM_WRITE_COMMAND:
						if(inherited::IsVerbose())
						{
							inherited::logger << DebugInfo << LOCATION
								<< sc_core::sc_time_stamp().to_string()
								<< ": received a TLM_WRITE_COMMAND payload at 0x"
								<< std::hex << addr << std::dec
								<< " of " << data_length << " bytes in length" << std::endl
								<< EndDebugInfo;
						}

						status = inherited::FSM(unisim::component::cxx::memory::flash::am29::CMD_WRITE, addr, (uint8_t *) data_ptr, data_length);
						break;
					case tlm::TLM_IGNORE_COMMAND:
						inherited::logger << DebugError << LOCATION
								<< (sc_core::sc_time_stamp() + t).to_string() 
								<< " : received an unexpected TLM_IGNORE_COMMAND payload"
								<< EndDebugError;
						Object::Stop(-1);
						return tlm::TLM_COMPLETED;
				}

				t += cycle_time;

				if(status)
					payload.set_response_status(tlm::TLM_OK_RESPONSE);
				else
					payload.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);

				return tlm::TLM_COMPLETED;
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			inherited::logger << DebugError << "protocol error" << EndDebugError;
			Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH, unsigned int BUSWIDTH>
void AM29<CONFIG, BYTESIZE, IO_WIDTH, BUSWIDTH>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	tlm::tlm_phase phase = tlm::BEGIN_REQ;
	nb_transport_fw(payload, phase, t);
}

} // end of namespace am29
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#undef LOCATION

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_AM29_AM29_TCC__
