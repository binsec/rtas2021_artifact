/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_TCC__

#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_DataObjectInfo<MEMORY_ADDR>::DWARF_DataObjectInfo()
	: dw_data_object_loc(0)
	, dw_data_object_type(0)
{
}

template <class MEMORY_ADDR>
DWARF_DataObjectInfo<MEMORY_ADDR>::DWARF_DataObjectInfo(const DWARF_Location<MEMORY_ADDR> *_dw_data_object_loc, const unisim::util::debug::Type *_dw_data_object_type)
	: dw_data_object_loc(_dw_data_object_loc)
	, dw_data_object_type(_dw_data_object_type)
{
}

template <class MEMORY_ADDR>
DWARF_DataObjectInfo<MEMORY_ADDR>::~DWARF_DataObjectInfo()
{
	if(dw_data_object_loc) delete dw_data_object_loc;
	if(dw_data_object_type) delete dw_data_object_type;
}

template <class MEMORY_ADDR>
const DWARF_Location<MEMORY_ADDR> *DWARF_DataObjectInfo<MEMORY_ADDR>::GetLocation() const
{
	return dw_data_object_loc;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Type *DWARF_DataObjectInfo<MEMORY_ADDR>::GetType() const
{
	return dw_data_object_type;
}

template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR>::DWARF_DataObject(const DWARF_Handler<MEMORY_ADDR> *_dw_handler, unsigned int _prc_num, const char *_data_object_name, const CLocOperationStream& _c_loc_operation_stream, bool _debug)
	: dw_handler(_dw_handler)
	, prc_num(_prc_num)
	, data_object_name(_data_object_name)
	, c_loc_operation_stream(_c_loc_operation_stream)
	, infos()
	, cache()
	, exists(false)
	, pc(0)
	, dw_data_object_loc(0)
	, dw_data_object_type(0)
	, arch_endianness(dw_handler->GetArchEndianness())
	, arch_address_size(dw_handler->GetArchAddressSize())
	, dw_reg_num_mapping(dw_handler->GetRegisterNumberMapping())
	, mem_if(dw_handler->GetMemoryInterface())
	, bv(arch_endianness)
	, debug(_debug)
	, debug_info_stream(dw_handler->GetDebugInfoStream())
	, debug_warning_stream(dw_handler->GetDebugWarningStream())
	, debug_error_stream(dw_handler->GetDebugErrorStream())
{
}

template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR>::DWARF_DataObject(const DWARF_Handler<MEMORY_ADDR> *_dw_handler, unsigned int _prc_num, const char *_data_object_name, const CLocOperationStream& _c_loc_operation_stream, MEMORY_ADDR _pc, const DWARF_Location<MEMORY_ADDR> *_dw_data_object_loc, const unisim::util::debug::Type *_dw_data_object_type, bool _debug)
	: dw_handler(_dw_handler)
	, prc_num(_prc_num)
	, data_object_name(_data_object_name)
	, c_loc_operation_stream(_c_loc_operation_stream)
	, infos()
	, cache()
	, exists(true)
	, pc(_pc)
	, dw_data_object_loc(_dw_data_object_loc)
	, dw_data_object_type(_dw_data_object_type)
	, arch_endianness(dw_handler->GetArchEndianness())
	, arch_address_size(dw_handler->GetArchAddressSize())
	, dw_reg_num_mapping(dw_handler->GetRegisterNumberMapping(prc_num))
	, mem_if(dw_handler->GetMemoryInterface(prc_num))
	, bv(arch_endianness)
	, debug(_debug)
	, debug_info_stream(dw_handler->GetDebugInfoStream())
	, debug_warning_stream(dw_handler->GetDebugWarningStream())
	, debug_error_stream(dw_handler->GetDebugErrorStream())
{
	UpdateCache(dw_data_object_loc, dw_data_object_type);
}
	
template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR>::~DWARF_DataObject()
{
	InvalidateCache();
}

template <class MEMORY_ADDR>
const char *DWARF_DataObject<MEMORY_ADDR>::GetName() const
{
	return data_object_name.c_str();
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_DataObject<MEMORY_ADDR>::GetBitSize() const
{
	return dw_data_object_loc ? dw_data_object_loc->GetBitSize() : 0;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Type *DWARF_DataObject<MEMORY_ADDR>::GetType() const
{
	return dw_data_object_type;
}

template <class MEMORY_ADDR>
unisim::util::endian::endian_type DWARF_DataObject<MEMORY_ADDR>::GetEndian() const
{
	return arch_endianness;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::GetPC() const
{
	return pc;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Exists() const
{
	return exists;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::IsOptimizedOut() const
{
	return dw_data_object_loc ? dw_data_object_loc->GetType() == DW_LOC_NULL : true;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::GetAddress(MEMORY_ADDR& addr) const
{
	if(!dw_data_object_loc) return false;
	if(dw_data_object_loc->GetType() != DW_LOC_SIMPLE_MEMORY) return false;
	
	addr = dw_data_object_loc->GetAddress();
	
	return true;
}

template <class MEMORY_ADDR>
void DWARF_DataObject<MEMORY_ADDR>::UpdateCache(const DWARF_Location<MEMORY_ADDR> *_dw_data_object_loc, const unisim::util::debug::Type *_dw_data_object_type)
{
	const DWARF_DataObjectInfo<MEMORY_ADDR> *dw_data_object_info = new DWARF_DataObjectInfo<MEMORY_ADDR>(_dw_data_object_loc, _dw_data_object_type);
	
	infos.push_back(dw_data_object_info);
	
	const std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges = _dw_data_object_loc->GetRanges();
	
	typename std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >::const_iterator it;
	
	for(it = ranges.begin(); it != ranges.end(); it++)
	{
		MEMORY_ADDR _pc;
		
		for(_pc = it->first; _pc < it->second; _pc++)
		{
			cache.insert(std::pair<MEMORY_ADDR, const DWARF_DataObjectInfo<MEMORY_ADDR> *>(_pc, dw_data_object_info));
		}
	}
}

template <class MEMORY_ADDR>
void DWARF_DataObject<MEMORY_ADDR>::InvalidateCache()
{
	typename std::vector<const DWARF_DataObjectInfo<MEMORY_ADDR> *>::iterator it;
	
	for(it = infos.begin(); it != infos.end(); it++)
	{
		delete (*it);
	}
	
	cache.clear();
}

template <class MEMORY_ADDR>
const DWARF_DataObjectInfo<MEMORY_ADDR> *DWARF_DataObject<MEMORY_ADDR>::LookupCache(MEMORY_ADDR _pc) const
{
	typename std::map<MEMORY_ADDR, const DWARF_DataObjectInfo<MEMORY_ADDR> *>::const_iterator iter = cache.find(_pc);
	
	if(iter != cache.end())
	{
		const DWARF_DataObjectInfo<MEMORY_ADDR> *dw_data_object_info = (*iter).second;
		
		return dw_data_object_info;
	}
	
	return 0;
}

template <class MEMORY_ADDR>
void DWARF_DataObject<MEMORY_ADDR>::Seek(MEMORY_ADDR _pc)
{
	bv.Clear();
	pc = _pc;
	
	const DWARF_DataObjectInfo<MEMORY_ADDR> *dw_data_object_info = LookupCache(pc);
	
	if(dw_data_object_info)
	{
		dw_data_object_loc = dw_data_object_info->GetLocation();
		dw_data_object_type = dw_data_object_info->GetType();
		exists = true;
	}
	else
	{
		std::string matched_data_object_name;
		if(dw_handler->FindDataObject(c_loc_operation_stream, prc_num, pc, matched_data_object_name, dw_data_object_loc, dw_data_object_type))
		{
			UpdateCache(dw_data_object_loc, dw_data_object_type);
			exists = true;
		}
		else
		{
			exists = false;
		}
	}
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Fetch()
{
	if(!dw_data_object_loc) return false;
	
	if(debug)
	{
		const std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges = dw_data_object_loc->GetRanges();
		
		debug_info_stream << "PC ranges for which data object location is valid are ";
		typename std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >::const_iterator it;
		for(it = ranges.begin(); it != ranges.end(); it++)
		{
			debug_info_stream << "[0x" << std::hex << it->first << std::dec << "-0x" << std::hex << it->second << std::dec << "[ ";
		}
		debug_info_stream << std::endl;
	}
	
	bv.Clear();
	
	switch(dw_data_object_loc->GetType())
	{
		case DW_LOC_SIMPLE_MEMORY:
			{
				MEMORY_ADDR dw_addr = dw_data_object_loc->GetAddress();
				uint64_t dw_data_object_byte_size = dw_data_object_loc->GetByteSize();
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();
				MEMORY_ADDR dw_byte_size = dw_data_object_byte_size;
				if(debug)
				{
					debug_info_stream << "DW_LOC_SIMPLE_MEMORY: addr=0x" << std::hex << dw_addr << std::dec << ", bit_offset=" << dw_data_object_bit_offset << ", bit_size=" << dw_data_object_bit_size << ", byte_size=" << dw_data_object_byte_size << std::endl;
				}
				uint8_t buffer[dw_byte_size];
				memset(buffer, 0, dw_byte_size);
				if(dw_data_object_bit_offset >= 0)
				{
					MEMORY_ADDR addr = dw_addr + (dw_data_object_bit_offset / 8);
					if(!mem_if->ReadMemory(addr, buffer, dw_byte_size))
					{
						debug_error_stream << "Can't read memory at 0x" << std::hex << addr << std::dec << " (" << dw_byte_size << " bytes)" << std::endl;
						return false;
					}
					
					bv.Append(buffer, dw_data_object_bit_offset % 8, dw_data_object_bit_size);
				}
				else
				{
					if(!mem_if->ReadMemory(dw_addr + (dw_data_object_bit_offset / 8) - 1, buffer, dw_byte_size)) return false;

					bv.Append(buffer, 8 + (dw_data_object_bit_offset % 8), dw_data_object_bit_size);
				}
			}
			return true;
		case DW_LOC_SIMPLE_REGISTER:
			{
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();

				unsigned int dw_reg_num = dw_data_object_loc->GetRegisterNumber();
				const unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetArchReg(dw_reg_num);
				if(debug)
				{
					debug_info_stream << "DW_LOC_SIMPLE_REGISTER: dw_reg_num=" << dw_reg_num << std::endl;
				}
				if(!arch_reg) return false;
				
				unsigned int reg_size = arch_reg->GetSize(); // FIXME: Get true bit size
				
				switch(reg_size)
				{
					case 1:
						{
							uint8_t value = 0;
							arch_reg->GetValue(&value);
							bv.Append(value, dw_data_object_bit_offset, dw_data_object_bit_size);
						}
						break;
					case 2:
						{
							uint16_t value = 0;
							arch_reg->GetValue(&value);
							bv.Append(value, dw_data_object_bit_offset, dw_data_object_bit_size);
						}
						break;
					case 4:
						{
							uint32_t value = 0;
							arch_reg->GetValue(&value);
							bv.Append(value, dw_data_object_bit_offset, dw_data_object_bit_size);
						}
						break;
					case 8:
						{
							uint64_t value = 0;
							arch_reg->GetValue(&value);
							bv.Append(value, dw_data_object_bit_offset, dw_data_object_bit_size);
						}
						break;
					default:
						return false;
				}
			}
			return true;
		case DW_LOC_COMPOSITE:
			{
				const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& dw_loc_pieces = dw_data_object_loc->GetLocationPieces();

				typename std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>::const_iterator dw_loc_piece_iter;
				for(dw_loc_piece_iter = dw_loc_pieces.begin(); dw_loc_piece_iter != dw_loc_pieces.end(); dw_loc_piece_iter++)
				{
					DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece = *dw_loc_piece_iter;
					
					switch(dw_loc_piece->GetType())
					{
						case DW_LOC_PIECE_REGISTER:
							{
								DWARF_RegisterLocationPiece<MEMORY_ADDR> *dw_reg_loc_piece = (DWARF_RegisterLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								unsigned int dw_reg_num = dw_reg_loc_piece->GetRegisterNumber();
								unsigned int dw_bit_offset = dw_reg_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_reg_loc_piece->GetBitSize();
								if(debug)
								{
									debug_info_stream << "DW_LOC_PIECE_REGISTER: dw_reg_num=" << dw_reg_num << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								}
								
								const unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetArchReg(dw_reg_num);
								if(!arch_reg) return false;
								
								unsigned int reg_size = arch_reg->GetSize(); // FIXME: Get true bit size
								
								switch(reg_size)
								{
									case 1:
										{
											uint8_t value = 0;
											arch_reg->GetValue(&value);
											bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 8 - dw_bit_size - dw_bit_offset: dw_bit_offset, dw_bit_size);
										}
										break;
									case 2:
										{
											uint16_t value = 0;
											arch_reg->GetValue(&value);
											bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 16 - dw_bit_size - dw_bit_offset: dw_bit_offset, dw_bit_size);
										}
										break;
									case 4:
										{
											uint32_t value = 0;
											arch_reg->GetValue(&value);
											bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 32 - dw_bit_size - dw_bit_offset: dw_bit_offset, dw_bit_size);
										}
										break;
									case 8:
										{
											uint64_t value = 0;
											arch_reg->GetValue(&value);
											bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 64 - dw_bit_size - dw_bit_offset: dw_bit_offset, dw_bit_size);
										}
										break;
									default:
										return false;
								}
							}
							break;
						case DW_LOC_PIECE_MEMORY:
							{
								DWARF_MemoryLocationPiece<MEMORY_ADDR> *dw_mem_loc_piece = (DWARF_MemoryLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								MEMORY_ADDR dw_addr = dw_mem_loc_piece->GetAddress();
								unsigned int dw_bit_offset = dw_mem_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_mem_loc_piece->GetBitSize();
								if(debug)
								{
									debug_info_stream << "DW_LOC_PIECE_MEMORY: dw_addr=0x" << std::hex << dw_addr << std::dec << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								}
								
								MEMORY_ADDR dw_byte_size = dw_bit_size ? (dw_bit_size + dw_bit_offset + 7) / 8 : arch_address_size;
								uint8_t buffer[dw_byte_size];
								memset(buffer, 0, dw_byte_size);
								if(!mem_if->ReadMemory(dw_addr, buffer, dw_byte_size)) return false;
								
								bv.Append(buffer, dw_bit_offset, dw_bit_size);
							}
							break;
					}
					
				}
				return true;
			}
			break;
			
		case DW_LOC_IMPLICIT_SIMPLE_VALUE:
			{
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();

				MEMORY_ADDR dw_implicit_simple_value = dw_data_object_loc->GetImplicitSimpleValue();
				bv.Append(dw_implicit_simple_value, dw_data_object_bit_offset, dw_data_object_bit_size);
				
				return true;
			}
			break;
		case DW_LOC_IMPLICIT_BLOCK_VALUE:
			{
				const DWARF_Block<MEMORY_ADDR> *dw_implicit_block_value = dw_data_object_loc->GetImplicitBlockValue();
				
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				
				// compute min(die bit size, block bit length)
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();
				uint64_t dw_block_bit_size = 8 * dw_implicit_block_value->GetLength();
				if(dw_data_object_bit_size > dw_block_bit_size) dw_data_object_bit_size = dw_block_bit_size;
				
				bv.Append(dw_implicit_block_value->GetValue(), dw_data_object_bit_offset, dw_data_object_bit_size);
				
				return true;
			}
			break;
		default:
			return false;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Commit()
{
	if(!dw_data_object_loc) return false;
	
	unsigned int source_bit_offset = 0;
	
	switch(dw_data_object_loc->GetType())
	{
		case DW_LOC_SIMPLE_MEMORY:
			{
				uint64_t dw_data_object_byte_size = dw_data_object_loc->GetByteSize();
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();

				MEMORY_ADDR dw_addr = dw_data_object_loc->GetAddress();
				MEMORY_ADDR dw_byte_size = dw_data_object_byte_size;
				if(debug)
				{
					debug_info_stream << "DW_LOC_SIMPLE_MEMORY: addr=0x" << std::hex << dw_addr << std::dec << ", bit_offset=" << dw_data_object_bit_offset << ", bit_size=" << dw_data_object_bit_size << ", byte_size=" << dw_data_object_byte_size << std::endl;
				}
				uint8_t buffer[dw_byte_size];
				memset(buffer, 0, dw_byte_size);
				if(dw_data_object_bit_offset >= 0)
				{
					if(!bv.Read(source_bit_offset, buffer, dw_data_object_bit_offset % 8, dw_data_object_bit_size)) return false;
					source_bit_offset += dw_data_object_bit_size;
					if(!mem_if->WriteMemory(dw_addr + (dw_data_object_bit_offset / 8), buffer, dw_byte_size)) return false;
				}
				else
				{
					if(!bv.Read(source_bit_offset, buffer, 8 + (dw_data_object_bit_offset % 8), dw_data_object_bit_size)) return false;
					source_bit_offset += dw_data_object_bit_size;
					if(!mem_if->WriteMemory(dw_addr + (dw_data_object_bit_offset / 8) - 1, buffer, dw_byte_size)) return false;
				}
			}
			return true;
		case DW_LOC_SIMPLE_REGISTER:
			{
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();

				unsigned int dw_reg_num = dw_data_object_loc->GetRegisterNumber();
				unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetArchReg(dw_reg_num);
				if(debug)
				{
					debug_info_stream << "DW_LOC_SIMPLE_REGISTER: dw_reg_num=" << dw_reg_num << std::endl;
				}
				if(!arch_reg) return false;
				
				unsigned int reg_size = arch_reg->GetSize(); // FIXME: Get true bit size
				
				switch(reg_size)
				{
					case 1:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, dw_data_object_bit_offset, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							uint8_t value8 = value;
							arch_reg->SetValue(&value8);
						}
						break;
					case 2:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, dw_data_object_bit_offset, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							uint16_t value16 = value;
							arch_reg->SetValue(&value16);
						}
						break;
					case 4:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, dw_data_object_bit_offset, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							uint32_t value32 = value;
							arch_reg->SetValue(&value32);
						}
						break;
					case 8:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, dw_data_object_bit_offset, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							arch_reg->SetValue(&value);
						}
						break;
					default:
						return false;
				}
			}
			return true;
		case DW_LOC_COMPOSITE:
			{
				const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& dw_loc_pieces = dw_data_object_loc->GetLocationPieces();

				typename std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>::const_iterator dw_loc_piece_iter;
				for(dw_loc_piece_iter = dw_loc_pieces.begin(); dw_loc_piece_iter != dw_loc_pieces.end(); dw_loc_piece_iter++)
				{
					DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece = *dw_loc_piece_iter;
					
					switch(dw_loc_piece->GetType())
					{
						case DW_LOC_PIECE_REGISTER:
							{
								DWARF_RegisterLocationPiece<MEMORY_ADDR> *dw_reg_loc_piece = (DWARF_RegisterLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								unsigned int dw_reg_num = dw_reg_loc_piece->GetRegisterNumber();
								unsigned int dw_bit_offset = dw_reg_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_reg_loc_piece->GetBitSize();
								if(debug)
								{
									debug_info_stream << "DW_LOC_PIECE_REGISTER: dw_reg_num=" << dw_reg_num << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								}
								
								unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetArchReg(dw_reg_num);
								if(!arch_reg) return false;
								
								unsigned int reg_size = arch_reg->GetSize(); // FIXME: Get true bit size
								
								switch(reg_size)
								{
									case 1:
										{
											uint8_t value8 = 0;
											arch_reg->GetValue(&value8);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 8 - dw_bit_size - dw_bit_offset : dw_bit_offset, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint8_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value8 = (value & ~mask) | (value & mask);
											arch_reg->SetValue(&value8);
										}
										break;
									case 2:
										{
											uint16_t value16 = 0;
											arch_reg->GetValue(&value16);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 16 - dw_bit_size - dw_bit_offset : dw_bit_offset, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint16_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value16 = (value & ~mask) | (value & mask);
											arch_reg->SetValue(&value16);
										}
										break;
									case 4:
										{
											uint8_t value32 = 0;
											arch_reg->GetValue(&value32);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 32 - dw_bit_size - dw_bit_offset : dw_bit_offset, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint16_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value32 = (value & ~mask) | (value & mask);
											arch_reg->SetValue(&value32);
										}
										break;
									case 8:
										{
											uint8_t value64 = 0;
											arch_reg->GetValue(&value64);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 64 - dw_bit_size - dw_bit_offset : dw_bit_offset, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint16_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value64 = (value & ~mask) | (value & mask);
											arch_reg->SetValue(&value64);
										}
										break;
									default:
										return false;
								}
							}
							break;
						case DW_LOC_PIECE_MEMORY:
							{
								DWARF_MemoryLocationPiece<MEMORY_ADDR> *dw_mem_loc_piece = (DWARF_MemoryLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								MEMORY_ADDR dw_addr = dw_mem_loc_piece->GetAddress();
								unsigned int dw_bit_offset = dw_mem_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_mem_loc_piece->GetBitSize();
								if(debug)
								{
									debug_info_stream << "DW_LOC_PIECE_MEMORY: dw_addr=0x" << std::hex << dw_addr << std::dec << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								}
								
								MEMORY_ADDR dw_byte_size = dw_bit_size ? (dw_bit_size + dw_bit_offset + 7) / 8 : arch_address_size;
								uint8_t buffer[dw_byte_size];
								memset(buffer, 0, dw_byte_size);
								if(!mem_if->ReadMemory(dw_addr, buffer, dw_byte_size)) return false;
								if(!bv.Read(source_bit_offset, buffer, dw_bit_offset, dw_bit_size)) return false;
								source_bit_offset += dw_bit_size;
								if(!mem_if->WriteMemory(dw_addr, buffer, dw_byte_size)) return false;
							}
							break;
					}
					
				}
				return true;
			}
			break;
		case DW_LOC_IMPLICIT_SIMPLE_VALUE:
			{
				// implicit value cannot be modified: silently skip source bits
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();
				source_bit_offset += dw_data_object_bit_size;
			}
			break;
		case DW_LOC_IMPLICIT_BLOCK_VALUE:
			{
				// implicit value cannot be modified: silently skip source bits
				const DWARF_Block<MEMORY_ADDR> *dw_implicit_block_value = dw_data_object_loc->GetImplicitBlockValue();
				
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();
				uint64_t dw_block_bit_size = 8 * dw_implicit_block_value->GetLength();
				if(dw_data_object_bit_size > dw_block_bit_size) dw_data_object_bit_size = dw_block_bit_size;
					
				source_bit_offset += dw_data_object_bit_size;
			}
			break;
		default:
			return false;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Read(MEMORY_ADDR obj_bit_offset, uint64_t& value, MEMORY_ADDR bit_size) const
{
	return bv.Read(obj_bit_offset, value, 0, bit_size);
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Write(MEMORY_ADDR obj_bit_offset, uint64_t value, MEMORY_ADDR bit_size)
{
	return bv.Write(obj_bit_offset, value, 0, bit_size);
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Read(MEMORY_ADDR obj_bit_offset, void *buffer, MEMORY_ADDR buf_bit_offset, MEMORY_ADDR bit_size) const
{
	return bv.Read(obj_bit_offset, (uint8_t *) buffer, buf_bit_offset, bit_size);
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Write(MEMORY_ADDR obj_bit_offset, const void *buffer, MEMORY_ADDR buf_bit_offset, MEMORY_ADDR bit_size)
{
	return bv.Write(obj_bit_offset, (uint8_t *) buffer, buf_bit_offset, bit_size);
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_TCC__
