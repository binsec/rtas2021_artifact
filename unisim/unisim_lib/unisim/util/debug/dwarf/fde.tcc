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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_FDE_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_FDE_TCC__

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_FDE<MEMORY_ADDR>::DWARF_FDE(DWARF_Handler<MEMORY_ADDR> *_dw_handler, DWARF_FrameSectionType _dw_fst)
	: dw_handler(_dw_handler)
	, dw_fst(_dw_fst)
	, dw_fmt(FMT_DWARF32)
	, offset(0)
	, id(0)
	, length(0)
	, cie_pointer(0)
	, initial_location(0)
	, address_range(0)
	, augmentation_length()
	, augmentation_data(0)
	, dw_call_frame_prog(0)
	, dw_cie(0)
{
}

template <class MEMORY_ADDR>
DWARF_FDE<MEMORY_ADDR>::~DWARF_FDE()
{
	if(augmentation_data)
	{
		delete augmentation_data;
	}

	if(dw_call_frame_prog)
	{
		delete dw_call_frame_prog;
	}
}

template <class MEMORY_ADDR>
bool DWARF_FDE<MEMORY_ADDR>::IsTerminator() const
{
	return length == 0;
}

template <class MEMORY_ADDR>
int64_t DWARF_FDE<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, MEMORY_ADDR _section_addr, uint64_t _offset)
{
	offset = _offset;
	endian_type file_endianness = dw_handler->GetFileEndianness();
	uint32_t length32;
	
	uint64_t size = 0;
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
		uint64_t cie_pointer64;
		if(max_size < sizeof(cie_pointer64)) return -1;
		memcpy(&cie_pointer64, rawdata, sizeof(cie_pointer64));
		cie_pointer64 = Target2Host(file_endianness, cie_pointer64);
		rawdata += sizeof(cie_pointer64);
		max_size -= sizeof(cie_pointer64);
		size += sizeof(cie_pointer64);
		switch(dw_fst)
		{
			case FST_DEBUG_FRAME:
				if(cie_pointer64 == 0xffffffffffffffffULL) return -1;
				break;
			case FST_EH_FRAME:
				if(cie_pointer64 == 0x0000000000000000ULL) return -1;
				break;
			default:
				return -1;
		}
		cie_pointer = cie_pointer64;
	}
	else
	{
		uint32_t cie_pointer32;
		if(max_size < sizeof(cie_pointer32)) return -1;
		memcpy(&cie_pointer32, rawdata, sizeof(cie_pointer32));
		cie_pointer32 = Target2Host(file_endianness, cie_pointer32);
		rawdata += sizeof(cie_pointer32);
		max_size -= sizeof(cie_pointer32);
		size += sizeof(cie_pointer32);
		switch(dw_fst)
		{
			case FST_DEBUG_FRAME:
				if(cie_pointer32 == 0xffffffffULL) return -1;
				break;
			case FST_EH_FRAME:
				if(cie_pointer32 == 0x00000000ULL) return -1;
				cie_pointer32 = offset - cie_pointer32 + ((dw_fmt == FMT_DWARF64) ? sizeof(uint32_t) + sizeof(uint64_t) : sizeof(uint32_t));
				break;
			default:
				return -1;
		}
		cie_pointer = cie_pointer32;
	}

	uint8_t address_size = (dw_fmt == FMT_DWARF64) ? 8 : 4;
	uint64_t initial_location_pc = _section_addr + _offset + size;

	switch(address_size)
	{
		case sizeof(uint32_t):
			{
				if(max_size < sizeof(uint32_t)) return -1;
				uint32_t value;
				memcpy(&value, rawdata, sizeof(uint32_t));
				initial_location = Target2Host(file_endianness, value);
				size += sizeof(uint32_t);
				rawdata += sizeof(uint32_t);
				max_size -= sizeof(uint32_t);
			}
			break;
		case sizeof(uint64_t):
			{
				if(max_size < sizeof(uint64_t)) return -1;
				uint64_t value;
				memcpy(&value, rawdata, sizeof(uint64_t));
				initial_location = Target2Host(file_endianness, value);
				size += sizeof(uint64_t);
				rawdata += sizeof(uint64_t);
				max_size -= sizeof(uint64_t);
			}
			break;
		default:
			return -1;
	}

	switch(address_size)
	{
		case sizeof(uint32_t):
			{
				if(max_size < sizeof(uint32_t)) return -1;
				uint32_t value;
				memcpy(&value, rawdata, sizeof(uint32_t));
				address_range = Target2Host(file_endianness, value);
				size += sizeof(uint32_t);
				rawdata += sizeof(uint32_t);
				max_size -= sizeof(uint32_t);
			}
			break;
		case sizeof(uint64_t):
			{
				if(max_size < sizeof(uint64_t)) return -1;
				uint64_t value;
				memcpy(&value, rawdata, sizeof(uint64_t));
				address_range = Target2Host(file_endianness, value);
				size += sizeof(uint64_t);
				rawdata += sizeof(uint64_t);
				max_size -= sizeof(uint64_t);
			}
			break;
		default:
			return -1;
	}
	
	if(dw_fst == FST_EH_FRAME)
	{
		const DWARF_CIE<MEMORY_ADDR> *_dw_cie = dw_handler->FindCIE(cie_pointer, dw_fst);
		if(!_dw_cie) return -1;
		
		if(*_dw_cie->GetAugmentation() == 'z')
		{
			int64_t sz;
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
		
		initial_location = _dw_cie->DecodePointer(initial_location_pc, initial_location);
	}
	
	uint64_t instructions_length;
	if(dw_fmt == FMT_DWARF64)
	{
		if(size > length + sizeof(uint32_t) + sizeof(uint64_t)) return -1;
		instructions_length = length + sizeof(uint32_t) + sizeof(uint64_t) - size; 
	}
	else
	{
		if(size > length + sizeof(uint32_t)) return -1;
		instructions_length = length + sizeof(uint32_t) - size; 
	}
	
	if(instructions_length > max_size) return -1;
	dw_call_frame_prog = new DWARF_CallFrameProgram<MEMORY_ADDR>(dw_handler, instructions_length, rawdata, DW_CFP_INSTRUCTIONS, dw_fmt);
	dw_call_frame_prog->SetFDE(this);
	size += instructions_length;

	return size;
}

template <class MEMORY_ADDR>
void DWARF_FDE<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
	dw_cie = dw_handler->FindCIE(cie_pointer, dw_fst);
	if(!dw_cie)
	{
		std::cerr << "Can't find CIE at offset " << cie_pointer << " in section ";
		switch(dw_fst)
		{
			case FST_DEBUG_FRAME:
				std::cerr << ".debug_frame";
				break;
			case FST_EH_FRAME:
				std::cerr << ".eh_frame";
				break;
		}
		std::cerr << std::endl;
	}
	else
	{
		dw_call_frame_prog->SetCIE(dw_cie);
	}
}

template <class MEMORY_ADDR>
const DWARF_CIE<MEMORY_ADDR> *DWARF_FDE<MEMORY_ADDR>::GetCIE() const
{
	return dw_cie;
}

template <class MEMORY_ADDR>
const DWARF_CallFrameProgram<MEMORY_ADDR> *DWARF_FDE<MEMORY_ADDR>::GetInstructions() const
{
	return dw_call_frame_prog;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_FDE<MEMORY_ADDR>::GetInitialLocation() const
{
	return initial_location;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_FDE<MEMORY_ADDR>::GetAddressRange() const
{
	return address_range;
}

// template <class MEMORY_ADDR>
// std::string DWARF_FDE<MEMORY_ADDR>::GetHREF() const
// {
// 	std::stringstream sstr;
// 	sstr << "debug_frame/fdes/" << (id / cies_per_file) << ".html#fde-" << id;
// 	return sstr.str();
// }
// 
template <class MEMORY_ADDR>
unsigned int DWARF_FDE<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_FDE<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_FDE id=\"fde-" << id << "\" length=\"" << length << "\" cie_pointer=\"" << cie_pointer << "\" idref=\"cie-" << dw_cie->GetId() << "\" initial_location=\"0x" << std::hex << initial_location << std::dec << "\" address_range=\"0x" << std::hex << address_range << std::dec << "\" call_frame_program=\"";
	std::stringstream sstr;
	sstr << *dw_call_frame_prog;
	c_string_to_XML(os, sstr.str().c_str());
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_FDE<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr>" << std::endl;
	os << "<td>" << offset << "</td><td><a href=\"../../" << dw_cie->GetHREF() << "\">cie-" << dw_cie->GetId() << "</a></td>";
	os << "<td>0x" << std::hex << initial_location << std::dec << "</td><td>0x" << std::hex << address_range << std::dec << "</td>";
	if(dw_fst == FST_EH_FRAME)
	{
		os << "<td>";
		if(dw_cie && (*dw_cie->GetAugmentation() == 'z'))
		{
			c_string_to_HTML(os, augmentation_data->to_string().c_str());
		}
		os << "</td>";
	}
	os << "<td>";
	std::stringstream sstr_call_frame_prog;
	sstr_call_frame_prog << *dw_call_frame_prog;
	std::stringstream sstr_cfi;
	DWARF_CallFrameVM<MEMORY_ADDR> dw_call_frame_vm = DWARF_CallFrameVM<MEMORY_ADDR>(dw_handler);
	const DWARF_CFI<MEMORY_ADDR> *cfi = dw_call_frame_vm.ComputeCFI(this);
	if(cfi)
	{
		sstr_cfi << *cfi;
		delete cfi;
	}
	else
	{
		sstr_cfi << "Invalid CFI";
	}
	c_string_to_HTML(os, sstr_call_frame_prog.str().c_str());
	os << "<hr>";
	c_string_to_HTML(os, sstr_cfi.str().c_str());
	os << "</td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_FDE<MEMORY_ADDR>& dw_fde)
{
	os << "Frame Description Entry:" << std::endl;
	os << " - Offset: " << dw_fde.offset << std::endl;
	os << " - Length: " << dw_fde.length << std::endl;
	os << " - Pointer to CIE: " << dw_fde.cie_pointer << std::endl;
	os << " - Initial location: 0x" << std::hex <<  dw_fde.initial_location << std::dec << std::endl;
	os << " - Address range: 0x" << std::hex <<  dw_fde.address_range << std::dec << std::endl;
	os << " - Instructions (" << dw_fde.dw_call_frame_prog->GetLength()  << " bytes): " << std::endl << *dw_fde.dw_call_frame_prog << std::endl;
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_FDE_TCC__
