#include "tlm2/payload/generic_payload.h"
#include <string.h>
#include <stdexcept>

namespace tlm {

static unsigned int next_extension_id = 0;

tlm_generic_payload::tlm_generic_payload()
	: mm(0)
	, ref_count(0)
	, gp_option(TLM_MIN_PAYLOAD)
	, command(TLM_IGNORE_COMMAND)
	, address(0)
	, data_ptr(0)
	, data_length(0)
	, streaming_width(0)
	, byte_enable_ptr(0)
	, byte_enable_length(0)
	, dmi_allowed(false)
	, response_status(TLM_INCOMPLETE_RESPONSE)
	, extension_slots(next_extension_id)
{
}

tlm_generic_payload::tlm_generic_payload(tlm_mm_interface *_mm)
	: mm(_mm)
	, ref_count(0)
	, gp_option(TLM_MIN_PAYLOAD)
	, command(TLM_IGNORE_COMMAND)
	, address(0)
	, data_ptr(0)
	, data_length(0)
	, streaming_width(0)
	, byte_enable_ptr(0)
	, byte_enable_length(0)
	, dmi_allowed(false)
	, response_status(TLM_INCOMPLETE_RESPONSE)
	, extension_slots(next_extension_id)
{
}

tlm_generic_payload::~tlm_generic_payload()
{
	free_all_extensions();
}

// disabled
tlm_generic_payload::tlm_generic_payload(const tlm_generic_payload&)
{
}

// disabled
tlm_generic_payload& tlm_generic_payload::operator = (const tlm_generic_payload&)
{
	return *this;
}

// Memory management
void tlm_generic_payload::set_mm(tlm_mm_interface *_mm)
{
	mm = _mm;
}

bool tlm_generic_payload::has_mm() const
{
	return mm != 0;
}

void tlm_generic_payload::acquire()
{
	ref_count++;
}

void tlm_generic_payload::release()
{
	if(--ref_count == 0)
	{
		mm->free(this);
	}
}

int tlm_generic_payload::get_ref_count() const
{
	return ref_count;
}

void tlm_generic_payload::reset()
{
	free_auto_extensions();
	gp_option = TLM_MIN_PAYLOAD;
}

void tlm_generic_payload::deep_copy_from(const tlm_generic_payload& payload)
{
	gp_option = payload.gp_option;
	command = payload.command;
	address = payload.address;
	data_length = payload.data_length;
	streaming_width = payload.streaming_width;
	byte_enable_length = payload.byte_enable_length;
	dmi_allowed = payload.dmi_allowed;
	response_status = payload.response_status;

	if(data_ptr && payload.data_ptr)
	{
		memcpy(data_ptr, payload.data_ptr, data_length);
	}
	
	if(byte_enable_ptr && payload.byte_enable_ptr)
	{
		memcpy(byte_enable_ptr, payload.byte_enable_ptr, byte_enable_length);
	}
	
	extension_slots.reserve(payload.extension_slots.capacity());
	typename std::vector<tlm_extension_slot>::size_type num_extensions = payload.extension_slots.size();
	
	if(num_extensions > extension_slots.size())
	{
		extension_slots.resize(num_extensions);
	}
	
	typename std::vector<tlm_extension_slot>::size_type extension_id;
	
	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		const tlm_extension_slot& orig_extension_slot = payload.extension_slots[extension_id];
		
		if(orig_extension_slot.extension)
		{
			if(extension_slots[extension_id].extension)
			{
				extension_slots[extension_id].extension->copy_from(*orig_extension_slot.extension);
			}
			else
			{
				tlm_extension_base *cloned_extension = payload.extension_slots[extension_id].extension->clone();
				
				if(cloned_extension)
				{
					if(mm)
					{
						set_auto_extension(extension_id, cloned_extension);
					}
					else
					{
						set_extension(extension_id, cloned_extension);
					}
				}
			}
		}
	}
}

void tlm_generic_payload::update_original_from(const tlm_generic_payload& payload, bool use_byte_enable_on_read)
{
	response_status = payload.response_status;
	dmi_allowed = payload.dmi_allowed;
	if((command == TLM_READ_COMMAND) && data_ptr && payload.data_ptr && (data_ptr != payload.data_ptr) && payload.data_length)
	{
		unsigned int min_data_length = (data_length < payload.data_length) ? data_length : payload.data_length; // avoid buffer copy overflow
		
		if(byte_enable_ptr && byte_enable_length)
		{
			unsigned char *src = payload.data_ptr;
			unsigned char *dst = data_ptr;
			unsigned int copy_size = min_data_length;
			unsigned int byte_enable_offset = 0;

			do
			{
				unsigned char mask = byte_enable_ptr[byte_enable_offset];
				// combine the current data buffer and the payload data buffer according to the byte enable buffer
				*dst = ((*dst) & ~mask) | ((*src) & mask);
				// cycle through the byte enable buffer
				if(++byte_enable_offset >= byte_enable_length) byte_enable_offset = 0;
			} while(++src, ++dst, --copy_size);
		}
		else
		{
			memcpy(data_ptr, payload.data_ptr, min_data_length);
		}
	}
	
	update_extensions_from(payload);
}

void tlm_generic_payload::update_extensions_from(const tlm_generic_payload& payload)
{
	typename std::vector<tlm_extension_slot>::size_type num_extensions = payload.extension_slots.size();
	typename std::vector<tlm_extension_slot>::size_type extension_id;
	
	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		const tlm_extension_slot& orig_extension_slot = payload.extension_slots[extension_id];
		
		if(orig_extension_slot.extension)
		{
			if(extension_slots[extension_id].extension)
			{
				extension_slots[extension_id].extension->copy_from(*orig_extension_slot.extension);
			}
		}
	}
}

void tlm_generic_payload::free_all_extensions()
{
	typename std::vector<tlm_extension_slot>::size_type num_extensions = extension_slots.size();
	typename std::vector<tlm_extension_slot>::size_type extension_id;
	
	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		const tlm_extension_slot& extension_slot = extension_slots[extension_id];
		
		if(extension_slot.extension)
		{
			extension_slot.extension->free();
			
			extension_slots[extension_id].extension = 0;
		}
	}
}

void tlm_generic_payload::free_auto_extensions()
{
	typename std::vector<tlm_extension_slot>::size_type num_extensions = extension_slots.size();
	typename std::vector<tlm_extension_slot>::size_type extension_id;

	for(extension_id = 0; extension_id < num_extensions; extension_id++)
	{
		tlm_extension_slot& extension_slot = extension_slots[extension_id];
		
		if(!extension_slot.sticky && extension_slot.extension)
		{
			extension_slot.extension->free();
			extension_slot.extension = 0;
		}
	}
}

// Access methods
tlm_gp_option tlm_generic_payload::get_gp_option() const
{
	return gp_option;
}

void tlm_generic_payload::set_gp_option(const tlm_gp_option _gp_option)
{
	gp_option = _gp_option;
}

tlm_command tlm_generic_payload::get_command() const
{
	return command;
}

void tlm_generic_payload::set_command(const tlm_command _command)
{
	command = _command;
}

bool tlm_generic_payload::is_read()
{
	return command == TLM_READ_COMMAND;
}

void tlm_generic_payload::set_read()
{
	command = TLM_READ_COMMAND;
}

bool tlm_generic_payload::is_write()
{
	return command == TLM_WRITE_COMMAND;
}

void tlm_generic_payload::set_write()
{
	command = TLM_WRITE_COMMAND;
}

sc_dt::uint64 tlm_generic_payload::get_address() const
{
	return address;
}

void tlm_generic_payload::set_address(const sc_dt::uint64 _address)
{
	address = _address;
}

unsigned char *tlm_generic_payload::get_data_ptr() const
{
	return data_ptr;
}

void tlm_generic_payload::set_data_ptr(unsigned char *_data_ptr)
{
	data_ptr = _data_ptr;
}

unsigned int tlm_generic_payload::get_data_length() const
{
	return data_length;
}

void tlm_generic_payload::set_data_length(const unsigned int _data_length)
{
	data_length = _data_length;
}

unsigned int tlm_generic_payload::get_streaming_width() const
{
	return streaming_width;
}

void tlm_generic_payload::set_streaming_width(const unsigned int _streaming_width)
{
	streaming_width = _streaming_width;
}

unsigned char *tlm_generic_payload::get_byte_enable_ptr() const
{
	return byte_enable_ptr;
}

void tlm_generic_payload::set_byte_enable_ptr(unsigned char *_byte_enable_ptr)
{
	byte_enable_ptr = _byte_enable_ptr;
}

unsigned int tlm_generic_payload::get_byte_enable_length() const
{
	return byte_enable_length;
}

void tlm_generic_payload::set_byte_enable_length(const unsigned int _byte_enable_length)
{
	byte_enable_length = _byte_enable_length;
}

// DMI hint
void tlm_generic_payload::set_dmi_allowed(bool _dmi_allowed)
{
	dmi_allowed = _dmi_allowed;
}

bool tlm_generic_payload::is_dmi_allowed() const
{
	return dmi_allowed;
}

tlm_response_status tlm_generic_payload::get_response_status() const
{
	return response_status;
}

void tlm_generic_payload::set_response_status(const tlm_response_status _response_status)
{
	response_status = _response_status;
}

std::string tlm_generic_payload::get_response_string()
{
	switch(response_status)
	{
		case TLM_OK_RESPONSE: return std::string("TLM_OK_RESPONSE");
		case TLM_INCOMPLETE_RESPONSE: return std::string("TLM_INCOMPLETE_RESPONSE");
		case TLM_GENERIC_ERROR_RESPONSE: return std::string("TLM_GENERIC_ERROR_RESPONSE");
		case TLM_ADDRESS_ERROR_RESPONSE: return std::string("TLM_ADDRESS_ERROR_RESPONSE");
		case TLM_COMMAND_ERROR_RESPONSE: return std::string("TLM_COMMAND_ERROR_RESPONSE");
		case TLM_BURST_ERROR_RESPONSE: return std::string("TLM_BURST_ERROR_RESPONSE");
		case TLM_BYTE_ENABLE_ERROR_RESPONSE: return std::string("TLM_BYTE_ENABLE_ERROR_RESPONSE");
	}
	return std::string("?");
}

bool tlm_generic_payload::is_response_ok()
{
	return response_status == TLM_OK_RESPONSE;
}

bool tlm_generic_payload::is_response_error()
{
	return response_status != TLM_OK_RESPONSE;
}

tlm_extension_base *tlm_generic_payload::set_extension(unsigned int id, tlm_extension_base *extension)
{
	tlm_extension_base *old_extension = (id < extension_slots.size()) ? extension_slots[id].extension : 0;
	extension_slots[id].extension = extension;
	extension_slots[id].sticky = true;
	return old_extension;
}

tlm_extension_base* tlm_generic_payload::set_auto_extension(unsigned int id, tlm_extension_base *extension)
{
	if(!mm) throw std::runtime_error("call to tlm_generic_payload::set_auto_extension while payload has no memory management"); 
	tlm_extension_base *old_extension = (id < extension_slots.size()) ? extension_slots[id].extension : 0;
	extension_slots[id].extension = extension;
	extension_slots[id].sticky = false;
	return old_extension;
}

tlm_extension_base* tlm_generic_payload::get_extension(unsigned int id) const
{
	return (id < extension_slots.size()) ? extension_slots[id].extension : 0;
}

void tlm_generic_payload::resize_extensions()
{
	if(next_extension_id > extension_slots.size())
	{
		extension_slots.resize(next_extension_id);
	}
}

unsigned int tlm_extension_base::allocate_extension_id()
{
	return next_extension_id++;
}

tlm_generic_payload::tlm_extension_slot::tlm_extension_slot()
	: extension(0)
	, sticky(false)
{
}

} // end of namespace tlm
