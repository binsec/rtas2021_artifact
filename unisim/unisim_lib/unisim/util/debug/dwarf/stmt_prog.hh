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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_STMT_PROG_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_STMT_PROG_HH__

#include <inttypes.h>
#include <iosfwd>
#include <vector>
#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/dwarf/version.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog);

template <class MEMORY_ADDR>
class DWARF_StatementProgram
{
public:
	DWARF_StatementProgram(DWARF_Handler<MEMORY_ADDR> *dw_handler);
	~DWARF_StatementProgram();
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	DWARF_Version GetDWARFVersion() const;
	unsigned int GetId() const;
	std::string GetHREF() const;
	const DWARF_Filename *GetFilename(unsigned int filename_idx) const;
	const char *GetIncludeDirectory(unsigned int include_directory_idx) const;
	std::ostream& to_XML(std::ostream& os) const;
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog);
	uint64_t GetOffset() const;
	DWARF_Handler<MEMORY_ADDR> *GetHandler() const;
private:
	friend class DWARF_StatementVM<MEMORY_ADDR>;
	
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_Version dw_ver;
	
	uint64_t offset;
	unsigned int id;
	
	uint64_t unit_length;                 // The size in bytes of the statement information for this compilation unit
	                                      // (not including the unit_length field itself).

	uint16_t version;                     // Version identifier for the statement information format.
	
	uint64_t header_length;               // The number of bytes following the header_length field to the beginning of the first
	                                      // byte of the statement program itself.

	uint8_t minimum_instruction_length;   // The size in bytes of the smallest target machine instruction. Statement program opcodes
	                                      // that alter the address register first multiply their operands by this value.

	uint8_t maximum_operations_per_instruction;   // The maximum number of individual operations that may be encoded in an instruction. Line
	                                              // number program opcodes that alter the address and op_index registers use this and
	                                              // minimum_instruction_length in their calculations.
	                                              // For non-VLIW architectures, this field is 1, the op_index register is always 0, and the
	                                              // operation pointer is simply the address register.

	uint8_t default_is_stmt;              // The initial value of the is_stmt register.
	                                      // A simple code generator that emits machine instructions in the order implied by the source
	                                      // program would set this to "true", and every entry in the matrix would represent a
	                                      // statement boundary. A pipeline scheduling code generator would set this to "false" and
	                                      // emit a specific statement program opcode for each instruction that represented a statement
	                                      // boundary.

	int8_t line_base;                     // This parameter affects the meaning of the special opcodes. See below.

	uint8_t line_range;                   // This parameter affects the meaning of the special opcodes. See below.
	
	uint8_t opcode_base;                  // The number assigned to the first special opcode.

	std::vector<uint8_t> standard_opcode_lengths;   // This array specifies the number of LEB128 operands for each of the standard opcodes. The
	                                                // first element of the array corresponds to the opcode whose value is 1, and the last element
	                                                // corresponds to the opcode whose value is opcode_base - 1. By increasing
	                                                // opcode_base, and adding elements to this array, new standard opcodes can be added,
	                                                // while allowing consumers who do not know about these new opcodes to be able to skip
	                                                // them.
	
	std::vector<const char *> include_directories; // The sequence contains an entry for each path that was searched for included source files in
	                                               // this compilation. (The paths include those directories specified explicitly by the user for
	                                               // the compiler to search and those the compiler searches without explicit direction). Each
	                                               // path entry is either a full path name or is relative to the current directory of the compilation.
	                                               // The current directory of the compilation is understood to be the first entry and is not
	                                               // explicitly represented. Each entry is a null-terminated string containing a full path name.
	                                               // The last entry is followed by a single null byte.

	std::vector<DWARF_Filename> filenames;        // The sequence contains an entry for each source file that contributed to the statement
	                                              // information for this compilation unit or is used in other contexts, such as in a declaration
	                                              // coordinate or a macro file inclusion. Each entry has a null-terminated string containing the
	                                              // file name, an unsigned LEB128 number representing the directory index of the directory in
	                                              // which the file was found, an unsigned LEB128 number representing the time of last
	                                              // modification for the file and an unsigned LEB128 number representing the length in bytes
	                                              // of the file. A compiler may choose to emit LEB128(0) for the time and length fields to
	                                              // indicate that this information is not available. The last entry is followed by a single null
	                                              // byte.

	uint32_t program_length;
	const uint8_t *program;                       // the program itself
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
