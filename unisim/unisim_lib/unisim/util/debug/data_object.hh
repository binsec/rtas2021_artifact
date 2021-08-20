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

#ifndef __UNISIM_UTIL_DEBUG_DATA_OBJECT_HH__
#define __UNISIM_UTIL_DEBUG_DATA_OBJECT_HH__

#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/type.hh>

namespace unisim {
namespace util {
namespace debug {

template <class ADDRESS> class DataObject;

template <class ADDRESS>
class DataObject
{
public:
	virtual ~DataObject() {}
	virtual const char *GetName() const = 0;
	virtual ADDRESS GetBitSize() const = 0;
	virtual const Type *GetType() const = 0;
	virtual unisim::util::endian::endian_type GetEndian() const = 0;
	virtual void Seek(ADDRESS pc) = 0;
	virtual bool GetPC() const = 0;
	virtual bool Exists() const = 0;
	virtual bool IsOptimizedOut() const = 0;
	virtual bool GetAddress(ADDRESS& addr) const = 0;
	virtual bool Fetch() = 0;
	virtual bool Commit() = 0;
	virtual bool Read(ADDRESS obj_bit_offset, uint64_t& value, ADDRESS bit_size) const = 0;
	virtual bool Write(ADDRESS obj_bit_offset, uint64_t value, ADDRESS bit_size) = 0;
	virtual bool Read(ADDRESS obj_bit_offset, void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) const = 0;
	virtual bool Write(ADDRESS obj_bit_offset, const void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) = 0;
};

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DATA_OBJECT_HH__
