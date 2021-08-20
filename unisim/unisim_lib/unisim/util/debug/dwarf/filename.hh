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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_FILENAME_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_FILENAME_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/leb128.hh>
#include <inttypes.h>
#include <iosfwd>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

std::ostream& operator << (std::ostream& os, const DWARF_Filename& dw_filename);

class DWARF_Filename
{
public:
	DWARF_Filename(unsigned int id);
	DWARF_Filename(const DWARF_Filename& dw_filename);
	~DWARF_Filename();
	const char *GetFilename() const;
	const DWARF_LEB128& GetDirectoryIndex() const;
	const DWARF_LEB128& GetLastModificationTime() const;
	const DWARF_LEB128& GetByteLength() const;
	
	DWARF_Filename& operator = (const DWARF_Filename& dw_filename);
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	void Fix(unsigned int id);
	std::ostream& to_XML(std::ostream& os) const;
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << (std::ostream& os, const DWARF_Filename& dw_filename);
private:
	unsigned int id;
	const char *filename;
	DWARF_LEB128 directory_index;         // unsigned, directory index of the directory in which file was found
	DWARF_LEB128 last_modification_time;  // unsigned, time of last modification for the file
	DWARF_LEB128 byte_length;             // unsigned, length in bytes of the file
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
