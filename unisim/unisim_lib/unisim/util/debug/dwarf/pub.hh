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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_PUB_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_PUB_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/version.hh>
#include <map>
#include <string>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pub<MEMORY_ADDR>& dw_pub);

template <class MEMORY_ADDR>
class DWARF_Pub
{
public:
	DWARF_Pub(const DWARF_Pubs<MEMORY_ADDR> *dw_pubs);
	~DWARF_Pub();
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	const DWARF_DIE<MEMORY_ADDR> *GetDIE() const;
	bool IsNull() const;
	const char *GetName() const;
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	unsigned int GetId() const;
	void EnumerateDataObjectNames(std::set<std::string>& name_set) const;
	std::ostream& to_XML(std::ostream& os) const;
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_Pub<MEMORY_ADDR>& dw_pub);
private:
	const DWARF_Pubs<MEMORY_ADDR> *dw_pubs;
	const DWARF_DIE<MEMORY_ADDR> *dw_die;
	unsigned int id;
	uint64_t debug_info_offset;
	const char *name;
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pubs<MEMORY_ADDR>& dw_pubs);

template <class MEMORY_ADDR>
class DWARF_Pubs
{
public:
	DWARF_Pubs(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_Pubs();
	uint64_t GetDebugInfoOffset() const;
	DWARF_Handler<MEMORY_ADDR> *GetHandler() const;
	DWARF_Format GetFormat() const;
	DWARF_Version GetDWARFVersion() const;
	const DWARF_Pub<MEMORY_ADDR> *FindPub(const char *name) const;
	void EnumerateDataObjectNames(std::set<std::string>& name_set) const;
	int64_t Load(const uint8_t *rawdata, uint64_t max_size);
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	unsigned int GetId() const;
	std::ostream& to_XML(std::ostream& os) const;
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_Pubs<MEMORY_ADDR>& dw_pubs);
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_Format dw_fmt;
	DWARF_Version dw_ver;
	
	uint64_t offset;
	unsigned int id;
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	
	uint64_t unit_length;        // The length of the entries for that set, not including the length field itself (see Section 7.2.2).

	uint16_t version;            // A version number. This number is specific to the name lookup table and is
	                             // independent of the DWARF version number.

	uint64_t debug_info_offset;  // The offset from the beginning of the .debug_info section of the compilation unit header
	                             // referenced by the set.

	uint64_t debug_info_length;  // The size in bytes of the contents of the .debug_info section generated to represent that
	                             // compilation unit.

	std::map<std::string, DWARF_Pub<MEMORY_ADDR> *> dw_pubs;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
