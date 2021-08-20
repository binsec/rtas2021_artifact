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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CU_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_CU_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/fmt.hh>
#include <unisim/util/debug/dwarf/version.hh>
#include <unisim/util/debug/stmt.hh>

#include <unisim/util/debug/dwarf/expr_vm.hh>

#include <list>
#include <set>
#include <string>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CompilationUnit<MEMORY_ADDR>& dw_cu);

template <class MEMORY_ADDR>
class DWARF_CompilationUnit
{
public:
	DWARF_CompilationUnit(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_CompilationUnit();

	const DWARF_Abbrev *FindAbbrev(const DWARF_LEB128& dw_abbrev_code) const;
	const char *GetString(uint64_t debug_str_offset) const;
	DWARF_Handler<MEMORY_ADDR> *GetHandler() const;
	DWARF_Format GetFormat() const;
	uint8_t GetAddressSize() const;
	uint16_t GetVersion() const;
	DWARF_Version GetDWARFVersion() const;
	uint8_t GetOffsetSize() const;
	uint64_t GetOffset() const;
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	void Register(DWARF_DIE<MEMORY_ADDR> *dw_die);
	void UnRegister(DWARF_DIE<MEMORY_ADDR> *dw_die);
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	unsigned int GetId() const;
	std::string GetHREF() const;
	const DWARF_DIE<MEMORY_ADDR> *GetDIE() const;
	std::ostream& to_XML(std::ostream& os);
	std::ostream& to_HTML(std::ostream& os);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_CompilationUnit& dw_cu);
	void BuildStatementMatrix(std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& stmt_matrix);
	bool HasOverlap(MEMORY_ADDR addr, MEMORY_ADDR length) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDIEByAddrRange(unsigned int dw_tag, MEMORY_ADDR addr, MEMORY_ADDR length) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDIEByName(unsigned int dw_tag, const char *name, bool external) const;
	bool GetDefaultBaseAddress(MEMORY_ADDR& base_addr) const;
	bool GetFrameBase(unsigned int prc_num, MEMORY_ADDR pc, MEMORY_ADDR& frame_base) const;
	const char *GetName() const;
	uint16_t GetLanguage() const;
	const char *GetProducer() const;
	uint8_t GetDefaultOrdering() const;
	bool GetDefaultLowerBound(int64_t& lower_bound) const;

	const DWARF_DIE<MEMORY_ADDR> *FindDataObject(const char *name, MEMORY_ADDR pc) const;
	void EnumerateDataObjectNames(std::set<std::string>& name_set, MEMORY_ADDR pc, bool local_only) const;
	
	const DWARF_DIE<MEMORY_ADDR> *FindSubProgram(const char *name) const;
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	bool debug;
	DWARF_Format dw_fmt;
	DWARF_Version dw_ver;
	
	uint64_t offset;
	unsigned int id;
	
	uint64_t unit_length;                 // The length of the .debug_info contribution for that compilation unit
	                                      // (not including the unit_length field itself).

	uint16_t version;                     // Version identifier for the DWARF information.
	
	uint64_t debug_abbrev_offset;         // Offset into the .debug_abbrev section. This offset associates the
	                                      // compilation unit with a particular set of debugging information entry abbreviations.
	
	uint8_t address_size;                 // unsigned integer representing the size in bytes of an address on the target
	                                      // architecture. If the system uses segmented addressing, this value represents the size of the
	                                      // offset portion of an address.
	
	DWARF_DIE<MEMORY_ADDR> *dw_die;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
