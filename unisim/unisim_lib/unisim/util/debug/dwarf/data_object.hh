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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/expr_vm.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/data_object.hh>
#include <unisim/util/debug/dwarf/c_loc_expr_parser.hh>
#include <inttypes.h>
#include <vector>
#include <iostream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

class DWARF_BitVector;

std::ostream& operator << (std::ostream& os, const DWARF_BitVector& bv);

class DWARF_BitVector
{
public:
	DWARF_BitVector(unisim::util::endian::endian_type target_endian);
	~DWARF_BitVector();
	
	void Clear();
	void Append(uint64_t source_value, unsigned int source_bit_offset, unsigned int source_bit_size);
	void Append(const uint8_t *source_buffer, unsigned int source_bit_offset, unsigned int source_bit_size);
	bool Write(unsigned int dest_bit_offset, uint64_t source_value, unsigned int source_bit_offset, unsigned int source_bit_size);
	bool Write(unsigned int dest_bit_offset, const uint8_t *source_buffer, unsigned int source_bit_offset, unsigned int source_bit_size);
	bool Read(unsigned int source_bit_offset, uint64_t& dest_value, unsigned int dest_bit_offset, unsigned int dest_bit_size) const;
	bool Read(unsigned int source_bit_offset, uint8_t *dest_buffer, unsigned int dest_bit_offset, unsigned int dest_bit_size) const;
private:
	unisim::util::endian::endian_type target_endian;
	unsigned int bit_size;
	std::vector<uint8_t> storage;
	friend std::ostream& operator << (std::ostream& os, const DWARF_BitVector& bv);
};

template <class MEMORY_ADDR>
class DWARF_DataObjectInfo
{
public:
	DWARF_DataObjectInfo();
	DWARF_DataObjectInfo(const DWARF_Location<MEMORY_ADDR> *dw_data_object_loc, const unisim::util::debug::Type *dw_data_object_type);
	~DWARF_DataObjectInfo();
	
	const DWARF_Location<MEMORY_ADDR> *GetLocation() const;
	const unisim::util::debug::Type *GetType() const;
private:
	const DWARF_Location<MEMORY_ADDR> *dw_data_object_loc;
	const unisim::util::debug::Type *dw_data_object_type;
};

template <class MEMORY_ADDR>
class DWARF_DataObject : public unisim::util::debug::DataObject<MEMORY_ADDR>
{
public:
	DWARF_DataObject(const DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int prc_num, const char *data_object_name, const CLocOperationStream& c_loc_operation_stream, bool debug);
	DWARF_DataObject(const DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int prc_num, const char *data_object_name, const CLocOperationStream& c_loc_operation_stream, MEMORY_ADDR pc, const DWARF_Location<MEMORY_ADDR> *dw_data_object_loc, const unisim::util::debug::Type *type, bool debug);
	virtual ~DWARF_DataObject();
	virtual const char *GetName() const;
	virtual MEMORY_ADDR GetBitSize() const;
	virtual unisim::util::endian::endian_type GetEndian() const;
	virtual const unisim::util::debug::Type *GetType() const;
	virtual void Seek(MEMORY_ADDR pc);
	virtual bool GetPC() const;
	virtual bool Exists() const;
	virtual bool IsOptimizedOut() const;
	virtual bool GetAddress(MEMORY_ADDR& addr) const;
	virtual bool Fetch();
	virtual bool Commit();
	virtual bool Read(MEMORY_ADDR obj_bit_offset, uint64_t& value, MEMORY_ADDR bit_size) const;
	virtual bool Write(MEMORY_ADDR obj_bit_offset, uint64_t value, MEMORY_ADDR bit_size);
	virtual bool Read(MEMORY_ADDR obj_bit_offset, void *buffer, MEMORY_ADDR buf_bit_offset, MEMORY_ADDR bit_size) const;
	virtual bool Write(MEMORY_ADDR obj_bit_offset, const void *buffer, MEMORY_ADDR buf_bit_offset, MEMORY_ADDR bit_size);
private:
	const DWARF_Handler<MEMORY_ADDR> *dw_handler;
	unsigned int prc_num;
	std::string data_object_name;
	const CLocOperationStream c_loc_operation_stream;
	std::vector<const DWARF_DataObjectInfo<MEMORY_ADDR> *> infos;
	std::map<MEMORY_ADDR, const DWARF_DataObjectInfo<MEMORY_ADDR> *> cache;
	bool exists;
	MEMORY_ADDR pc;
	const DWARF_Location<MEMORY_ADDR> *dw_data_object_loc;
	const unisim::util::debug::Type *dw_data_object_type;
	unisim::util::endian::endian_type arch_endianness;
	unsigned int arch_address_size;
	DWARF_RegisterNumberMapping *dw_reg_num_mapping;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
	DWARF_BitVector bv;
	bool debug;
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	
	void UpdateCache(const DWARF_Location<MEMORY_ADDR> *dw_data_object_loc, const unisim::util::debug::Type *dw_data_object_type);
	void InvalidateCache();
	const DWARF_DataObjectInfo<MEMORY_ADDR> *LookupCache(MEMORY_ADDR pc) const;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_HH__
