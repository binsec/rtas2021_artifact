/*
 *  Copyright (c) 2010,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CIE_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_CIE_TCC__

#include <string.h>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_CIE<MEMORY_ADDR>::DWARF_CIE(DWARF_Handler<MEMORY_ADDR> *_dw_handler, DWARF_FrameSectionType _dw_fst)
	: dw_handler(_dw_handler)
	, dw_fst(_dw_fst)
	, dw_fmt(FMT_DWARF32)
	, dw_ver(DW_VER_UNKNOWN)
	, offset(0)
	, id(0)
	, length(0)
	, cie_id(0)
	, version(0)
	, augmentation(0)
	, eh_data(0)
	, code_alignment_factor()
	, data_alignment_factor()
	, dw2_return_address_register(0)
	, dw3_return_address_register()
	, augmentation_length()
	, augmentation_data(0)
	, dw_initial_call_frame_prog(0)
{
}

template <class MEMORY_ADDR>
DWARF_CIE<MEMORY_ADDR>::~DWARF_CIE()
{
	if(augmentation_data)
	{
		delete augmentation_data;
	}
	
	if(dw_initial_call_frame_prog)
	{
		delete dw_initial_call_frame_prog;
	}
}

template <class MEMORY_ADDR>
bool DWARF_CIE<MEMORY_ADDR>::IsTerminator() const
{
	return length == 0;
}

template <class MEMORY_ADDR>
int64_t DWARF_CIE<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	endian_type file_endianness = dw_handler->GetFileEndianness();
	uint32_t length32;
	
	uint64_t size = 0;
	int64_t sz;
	if(max_size < sizeof(length32)) return -1;
	memcpy(&length32, rawdata, sizeof(length32));
	length32 = Target2Host(file_endianness, length32);
	rawdata += sizeof(length32);
	max_size -= sizeof(length32);
	size += sizeof(length32);
	
	if((dw_fst == FST_EH_FRAME) && (length32 == 0))
	{
		length = 0;
		return size;
	}

	dw_fmt = (length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t length64;
		if(max_size < sizeof(length64)) return -1;
		memcpy(&length64, rawdata, sizeof(length64));
		length64 = Target2Host(file_endianness, length64);
		rawdata += sizeof(length64);
		max_size -= sizeof(length64);
		size += sizeof(length64);
		length = length64;
	}
	else
	{
		length = length32;
	}
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t cie_id64;
		if(max_size < sizeof(cie_id64)) return -1;
		memcpy(&cie_id64, rawdata, sizeof(cie_id64));
		cie_id64 = Target2Host(file_endianness, cie_id64);
		rawdata += sizeof(cie_id64);
		max_size -= sizeof(cie_id64);
		size += sizeof(cie_id64);
		switch(dw_fst)
		{
			case FST_DEBUG_FRAME:
				if(cie_id64 != 0xffffffffffffffffULL) return -1;
				break;
			case FST_EH_FRAME:
				if(cie_id64 != 0x0000000000000000ULL) return -1;
				break;
			default:
				return -1;
		}
		cie_id = cie_id64;
	}
	else
	{
		uint32_t cie_id32;
		if(max_size < sizeof(cie_id32)) return -1;
		memcpy(&cie_id32, rawdata, sizeof(cie_id32));
		cie_id32 = Target2Host(file_endianness, cie_id32);
		rawdata += sizeof(cie_id32);
		max_size -= sizeof(cie_id32);
		size += sizeof(cie_id32);
		switch(dw_fst)
		{
			case FST_DEBUG_FRAME:
				if(cie_id32 != 0xffffffffULL) return -1;
				break;
			case FST_EH_FRAME:
				if(cie_id32 != 0x00000000ULL) return -1;
				break;
			default:
				return -1;
		}
		cie_id = cie_id32;
	}

	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(file_endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	switch(version)
	{
		case DW_DEBUG_FRAME_VER2: dw_ver = DW_VER2; break;
		case DW_DEBUG_FRAME_VER3: dw_ver = DW_VER3; break;
		case DW_DEBUG_FRAME_VER4: dw_ver = DW_VER4; break;
		default: return -1;
	}

	augmentation = (const char *) rawdata;
	unsigned int augmentation_string_length = strlen(augmentation);
	if(max_size < (augmentation_string_length + 1)) return -1;
	size += augmentation_string_length + 1;
	rawdata += augmentation_string_length + 1;
	max_size -= augmentation_string_length + 1;
	
	if((dw_fst == FST_EH_FRAME) && (strcmp(augmentation, "eh") == 0))
	{
		if(dw_fmt == FMT_DWARF64)
		{
			uint64_t eh_data64;
			if(max_size < sizeof(eh_data64)) return -1;
			memcpy(&eh_data64, rawdata, sizeof(eh_data64));
			eh_data64 = Target2Host(file_endianness, eh_data64);
			rawdata += sizeof(eh_data64);
			max_size -= sizeof(eh_data64);
			size += sizeof(eh_data64);
			eh_data = eh_data64;
		}
		else
		{
			uint32_t eh_data32;
			if(max_size < sizeof(eh_data32)) return -1;
			memcpy(&eh_data32, rawdata, sizeof(eh_data32));
			eh_data32 = Target2Host(file_endianness, eh_data32);
			rawdata += sizeof(eh_data32);
			max_size -= sizeof(eh_data32);
			size += sizeof(eh_data32);
			eh_data = eh_data32;
		}
	}
	
	if((sz = code_alignment_factor.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if((sz = data_alignment_factor.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	switch(dw_ver)
	{
		case DW_VER2:
			// DWARF v2
			if(max_size < sizeof(dw2_return_address_register)) return -1;
			memcpy(&dw2_return_address_register, rawdata, sizeof(dw2_return_address_register));
			dw2_return_address_register = Target2Host(file_endianness, dw2_return_address_register);
			rawdata += sizeof(dw2_return_address_register);
			max_size -= sizeof(dw2_return_address_register);
			size += sizeof(dw2_return_address_register);
			break;
		case DW_VER3:
		case DW_VER4:
			// DWARF v3/v4
			if((sz = dw3_return_address_register.Load(rawdata, max_size)) < 0) return -1;
			rawdata += sz;
			max_size -= sz;
			size += sz;
			break;
		default:
			break;
	}
	
	if((dw_fst == FST_EH_FRAME) && (*augmentation == 'z'))
	{
		if((sz = augmentation_length.Load(rawdata, max_size)) < 0) return -1;
		rawdata += sz;
		max_size -= sz;
		size += sz;
		
		uint64_t block_length = (uint64_t) augmentation_length;
		if(max_size < block_length) return -1;
		
		augmentation_data = new DWARF_Block<MEMORY_ADDR>(block_length, rawdata);
		
		rawdata += block_length;
		size += block_length;
		max_size -= block_length;
	}
	
	uint64_t initial_instructions_length;
	if(dw_fmt == FMT_DWARF64)
	{
		if(size > length + sizeof(uint32_t) + sizeof(uint64_t)) return -1;
		initial_instructions_length = length + sizeof(uint32_t) + sizeof(uint64_t) - size; 
	}
	else
	{
		if(size > length + sizeof(uint32_t)) return -1;
		initial_instructions_length = length + sizeof(uint32_t) - size; 
	}
	
	if(initial_instructions_length > max_size) return -1;
	dw_initial_call_frame_prog = new DWARF_CallFrameProgram<MEMORY_ADDR>(dw_handler, initial_instructions_length, rawdata, DW_CFP_INITIAL_INSTRUCTIONS, dw_fmt);
	dw_initial_call_frame_prog->SetCIE(this);
	size += initial_instructions_length;

	return size;
}

template <class MEMORY_ADDR>
void DWARF_CIE<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_CIE<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	switch(dw_fst)
	{
		case FST_DEBUG_FRAME:
			sstr << "debug_frame";
			break;
		case FST_EH_FRAME:
			sstr << "eh_frame";
			break;
	}
	sstr << "/cies/" << (id / cies_per_file) << ".html#cie-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_CIE<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
DWARF_Version DWARF_CIE<MEMORY_ADDR>::GetDWARFVersion() const
{
	return dw_ver;
}

template <class MEMORY_ADDR>
const char *DWARF_CIE<MEMORY_ADDR>::GetAugmentation() const
{
	return augmentation;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_CIE<MEMORY_ADDR>::GetCodeAlignmentFactor() const
{
	return code_alignment_factor;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_CIE<MEMORY_ADDR>::GetDataAlignmentFactor() const
{
	return data_alignment_factor;
}

template <class MEMORY_ADDR>
unsigned int DWARF_CIE<MEMORY_ADDR>::GetReturnAddressRegister() const
{
	return (dw_ver == DW_VER2) ? dw2_return_address_register : dw3_return_address_register;
}

template <class MEMORY_ADDR>
const DWARF_CallFrameProgram<MEMORY_ADDR> *DWARF_CIE<MEMORY_ADDR>::GetInitialInstructions() const
{
	return dw_initial_call_frame_prog;
}

template <class MEMORY_ADDR>
bool DWARF_CIE<MEMORY_ADDR>::GetAugmentationData(char c, uint64_t& value) const
{
	if((*augmentation) != 'z') return false;
	
	const uint8_t *augmentation_data_value = augmentation_data->GetValue();
	int64_t augmentation_data_length = augmentation_data->GetLength();
	
	const char *p = augmentation + 1;
	
	while(*p)
	{
		bool match = (*p == c);
		switch(*p)
		{
			case 'L':
				if(!augmentation_data_length)
				{
					std::cerr << "WARNING! bad CIE augmentation data" << std::endl;
					return false;
				}
				if(match)
				{
					value = *augmentation_data_value;
					return true;
				}
				augmentation_data_value++;
				augmentation_data_length--;
				break;
			case 'P':
				{
					if(!augmentation_data_length)
					{
						std::cerr << "WARNING! bad CIE augmentation data" << std::endl;
						return false;
					}
					uint8_t ptr_encoding = *augmentation_data_value;
					augmentation_data_value++;
					augmentation_data_length--;
					
					int64_t sz = 0;
					
					if(ptr_encoding == DW_EH_PE_omit)
					{
						// no value is present
					}
					else if(ptr_encoding == DW_EH_PE_absptr)
					{
						sz = (dw_fmt == FMT_DWARF64) ? 8 : 4;
						if(augmentation_data_length < sz) return false;

						if(match)
						{
							endian_type file_endianness = dw_handler->GetFileEndianness();
							
							switch(sz)
							{
								case 4:
									{
										uint32_t personality_routine_handler_addr;
										memcpy(&personality_routine_handler_addr, augmentation_data_value, sz);
										value = unisim::util::endian::Target2Host(file_endianness, personality_routine_handler_addr);
										return true;
									}
									break;
								case 8:
									{
										uint64_t personality_routine_handler_addr;
										memcpy(&personality_routine_handler_addr, augmentation_data_value, sz);
										value = unisim::util::endian::Target2Host(file_endianness, personality_routine_handler_addr);
										return true;
									}
									break;
								default:
									return false;
							}
						}
					}
					else
					{
						switch(ptr_encoding & 0xf)
						{
							case DW_EH_PE_uleb128:
								{
									DWARF_LEB128 personality_routine_handler_addr;
									if((sz = personality_routine_handler_addr.Load(augmentation_data_value, augmentation_data_length)) < 0) return false;
									if(match)
									{
										value = (uint64_t) personality_routine_handler_addr;
										return true;
									}
								}
								break;
							case DW_EH_PE_udata2:
								{
									sz = 2;
									if(augmentation_data_length < sz) return false;
									if(match)
									{
										endian_type file_endianness = dw_handler->GetFileEndianness();
										uint16_t personality_routine_handler_addr;
										memcpy(&personality_routine_handler_addr, augmentation_data_value, sz);
										value = unisim::util::endian::Target2Host(file_endianness, personality_routine_handler_addr);
										return true;
									}
								}
								break;
							case DW_EH_PE_udata4:
								{
									sz = 4;
									if(augmentation_data_length < sz) return false;
									if(match)
									{
										endian_type file_endianness = dw_handler->GetFileEndianness();
										uint32_t personality_routine_handler_addr;
										memcpy(&personality_routine_handler_addr, augmentation_data_value, sz);
										value = unisim::util::endian::Target2Host(file_endianness, personality_routine_handler_addr);
										return true;
									}
								}
								break;
							case DW_EH_PE_udata8:
								{
									sz = 8;
									if(augmentation_data_length < sz) return false;
									if(match)
									{
										endian_type file_endianness = dw_handler->GetFileEndianness();
										uint64_t personality_routine_handler_addr;
										memcpy(&personality_routine_handler_addr, augmentation_data_value, sz);
										value = unisim::util::endian::Target2Host(file_endianness, personality_routine_handler_addr);
										return true;
									}
								}
								break;
							case DW_EH_PE_sleb128:
								{
									DWARF_LEB128 personality_routine_handler_addr;
									if((sz = personality_routine_handler_addr.Load(augmentation_data_value, augmentation_data_length)) < 0) return false;
									if(match)
									{
										value = (int64_t) personality_routine_handler_addr;
										return true;
									}
								}
								break;
							case DW_EH_PE_sdata2:
								{
									sz = 2;
									if(augmentation_data_length < sz) return false;
									if(match)
									{
										endian_type file_endianness = dw_handler->GetFileEndianness();
										uint16_t personality_routine_handler_addr;
										memcpy(&personality_routine_handler_addr, augmentation_data_value, sz);
										value = unisim::util::arithmetic::SignExtend(unisim::util::endian::Target2Host(file_endianness, personality_routine_handler_addr), 8 * sz);
										return true;
									}
								}
								break;
							case DW_EH_PE_sdata4:
								{
									sz = 4;
									if(augmentation_data_length < sz) return false;
									if(match)
									{
										endian_type file_endianness = dw_handler->GetFileEndianness();
										uint32_t personality_routine_handler_addr;
										memcpy(&personality_routine_handler_addr, augmentation_data_value, sz);
										value = unisim::util::arithmetic::SignExtend(unisim::util::endian::Target2Host(file_endianness, personality_routine_handler_addr), 8 * sz);
										return true;
									}
								}
								break;
							case DW_EH_PE_sdata8:
								{
									sz = 8;
									if(augmentation_data_length < sz) return false;
									if(match)
									{
										endian_type file_endianness = dw_handler->GetFileEndianness();
										uint64_t personality_routine_handler_addr;
										memcpy(&personality_routine_handler_addr, augmentation_data_value, sz);
										value = unisim::util::arithmetic::SignExtend(unisim::util::endian::Target2Host(file_endianness, personality_routine_handler_addr), 8 * sz);
										return true;
									}
								}
								break;
							default:
								return false;
						}
					}
					
					augmentation_data_value += sz;
					augmentation_data_length -= sz;
				}
				break;
			case 'R':
				if(!augmentation_data_length)
				{
					std::cerr << "WARNING! bad CIE augmentation data" << std::endl;
					return false;
				}
				if(match)
				{
					value = *augmentation_data_value;
					return true;
				}
				
				augmentation_data_value++;
				augmentation_data_length--;
				break;
			default:
				return false;
		}
		
		p++;
	}
	
	return false;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_CIE<MEMORY_ADDR>::DecodePointer(MEMORY_ADDR pc, MEMORY_ADDR encoded_ptr) const
{
	uint64_t ptr_encoding = 0;
	if(!GetAugmentationData('R', ptr_encoding)) return 0;
	
	if(ptr_encoding == DW_EH_PE_omit) return 0;
	
	if(ptr_encoding == DW_EH_PE_absptr) return encoded_ptr;
	
	MEMORY_ADDR decoded_ptr = 0;

	switch(ptr_encoding & 0x0f)
	{
		case DW_EH_PE_udata2: // 2 bytes unsigned value
		case DW_EH_PE_udata4: // 4 bytes unsigned value
		case DW_EH_PE_udata8: // 8 bytes unsigned value
			decoded_ptr = encoded_ptr;
			break;
		case DW_EH_PE_sdata2: // 2 bytes signed value
			decoded_ptr = unisim::util::arithmetic::SignExtend(encoded_ptr, 16);
			break;
		case DW_EH_PE_sdata4: // 4 bytes signed value
			decoded_ptr = unisim::util::arithmetic::SignExtend(encoded_ptr, 32);
			break;
		case DW_EH_PE_sdata8: // 8 bytes signed value
			decoded_ptr = unisim::util::arithmetic::SignExtend(encoded_ptr, 64);
			break;
		default:
			std::cerr << "Bad pointer encoding" << std::endl;
			return 0;
	}
	
	if((ptr_encoding & 0xf0) == DW_EH_PE_pcrel)
	{
		decoded_ptr = decoded_ptr + pc;
	}
	
	return decoded_ptr;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CIE<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_CIE id=\"cie-" << id << "\" version=\"" << (uint32_t) version << "\" augmentation=\"";
	c_string_to_XML(os, augmentation);
	os << "\"";
	if((dw_fst == FST_EH_FRAME) && (strcmp(augmentation, "eh") == 0))
	{
		os << " eh_data=\"0x" << std::hex << eh_data << std::dec << "\"";
	}
	os << " code_alignment_factor=\"" << code_alignment_factor.to_string(false) << "\" data_alignment_factor=\"" << data_alignment_factor.to_string(true) << "\" return_address_register=\"";
	if(dw_ver == DW_VER2)
	{
		os << (uint32_t) dw2_return_address_register;
	}
	else
	{
		os << dw3_return_address_register.to_string(false);
	}
	os << "\"";
	if((dw_fst == FST_EH_FRAME) && (*augmentation == 'z') && augmentation_data)
	{
		os << "augmentation_data=\"";
		c_string_to_HTML(os, augmentation_data->to_string().c_str()) << "\"";
	}
	os << " initial_call_frame_program=\"";
	std::stringstream sstr;
	sstr << *dw_initial_call_frame_prog;
	c_string_to_XML(os, sstr.str().c_str());
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CIE<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"cie-" << id << "\">" << std::endl;
	os << "<td>" << id << "</td><td>" << offset << "</td><td>" << (uint32_t) version << "</td><td>";
	c_string_to_HTML(os, augmentation);
	os << "</td>";
	if(dw_fst == FST_EH_FRAME)
	{
		os << "<td>";
		if(strcmp(augmentation, "eh") == 0)
		{
			os << std::hex << eh_data << std::dec;
		}
		os << "</td>";
	}
	os << "<td>" << code_alignment_factor.to_string(false) << "</td><td>" << data_alignment_factor.to_string(true) << "</td><td>";
	if(dw_ver == DW_VER2)
	{
		os << (uint32_t) dw2_return_address_register;
	}
	else
	{
		os << dw3_return_address_register.to_string(false);
	}
	os << "</td>";
	if(dw_fst == FST_EH_FRAME)
	{
		os << "<td>";
		if((*augmentation == 'z') && augmentation_data)
		{
			c_string_to_HTML(os, augmentation_data->to_string().c_str());
		}
		os << "</td>";
	}
	os << "<td>";
	std::stringstream sstr;
	sstr << *dw_initial_call_frame_prog;
	c_string_to_HTML(os, sstr.str().c_str());
	os << "</td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CIE<MEMORY_ADDR>& dw_cie)
{
	os << "Common Information Entry:" << std::endl;
	os << " - Offset: " << dw_cie.offset << std::endl;
	os << " - Version: " << (uint32_t) dw_cie.version << std::endl;          // Independent of DWARF version number. DWARF v2/version=1; DWARF v3/version=3
	os << " - Augmentation: " << dw_cie.augmentation << std::endl;
	os << " - Code alignment factor: " << dw_cie.code_alignment_factor.to_string(false) << std::endl;
	os << " - Data alignment factor: " << dw_cie.data_alignment_factor.to_string(true) << std::endl;
	os << " - Return address register: ";
	if(dw_cie.dw_ver == DW_VER2)
	{
		os << (uint32_t) dw_cie.dw2_return_address_register;
	}
	else
	{
		os << dw_cie.dw3_return_address_register.to_string(false);
	}
	os << std::endl;
	os << " - Initial instructions (" << dw_cie.dw_initial_call_frame_prog->GetLength() << " bytes): " << std::endl << *dw_cie.dw_initial_call_frame_prog << std::endl;
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
