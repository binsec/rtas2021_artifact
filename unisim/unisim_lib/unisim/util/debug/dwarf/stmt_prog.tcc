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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_STMT_PROG_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_STMT_PROG_TCC__

#include <unisim/util/debug/dwarf/stmt_prog.hh>
#include <unisim/util/debug/dwarf/filename.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_StatementProgram<MEMORY_ADDR>::DWARF_StatementProgram(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, dw_ver(DW_VER_UNKNOWN)
	, unit_length(0)
	, version(0)
	, header_length(0)
	, minimum_instruction_length(0)
	, maximum_operations_per_instruction(1)
	, default_is_stmt(0)
	, line_base(0)
	, line_range(0)
	, opcode_base(0)
	, program(0)
{
}

template <class MEMORY_ADDR>
DWARF_StatementProgram<MEMORY_ADDR>::~DWARF_StatementProgram()
{
}

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR> *DWARF_StatementProgram<MEMORY_ADDR>::GetHandler() const
{
	return dw_handler;
}

template <class MEMORY_ADDR>
DWARF_Version DWARF_StatementProgram<MEMORY_ADDR>::GetDWARFVersion() const
{
	return dw_ver;
}

template <class MEMORY_ADDR>
int64_t DWARF_StatementProgram<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	endian_type file_endianness = dw_handler->GetFileEndianness();
	standard_opcode_lengths.clear();
	include_directories.clear();
	filenames.clear();
	
	uint32_t unit_length32;
	
	uint64_t size = 0;

	if(max_size < sizeof(unit_length32)) return -1;
	memcpy(&unit_length32, rawdata, sizeof(unit_length32));
	unit_length32 = Target2Host(file_endianness, unit_length32);
	rawdata += sizeof(unit_length32);
	max_size -= sizeof(unit_length32);
	size += sizeof(unit_length32);
	
	DWARF_Format dw_fmt = (unit_length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;

	// 64-bit DWARF format ?
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t unit_length64;
		if(max_size < sizeof(unit_length64)) return -1;
		memcpy(&unit_length64, rawdata, sizeof(unit_length64));
		unit_length64 = Target2Host(file_endianness, unit_length64);
		rawdata += sizeof(unit_length64);
		max_size -= sizeof(unit_length64);
		size += sizeof(unit_length64);
		unit_length = unit_length64;
	}
	else
	{
		unit_length = unit_length32;
	}
	
	if(max_size < unit_length) return -1;

	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(file_endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	// 64-bit DWARF format ?
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t header_length64;
		
		if(max_size < sizeof(header_length64)) return -1;
		memcpy(&header_length64, rawdata, sizeof(header_length64));
		header_length64 = Target2Host(file_endianness, header_length64);
		rawdata += sizeof(header_length64);
		max_size -= sizeof(header_length64);
		size += sizeof(header_length64);
		header_length = header_length64;
	}
	else
	{
		uint32_t header_length32;
		
		if(max_size < sizeof(header_length32)) return -1;
		memcpy(&header_length32, rawdata, sizeof(header_length32));
		header_length32 = Target2Host(file_endianness, header_length32);
		rawdata += sizeof(header_length32);
		max_size -= sizeof(header_length32);
		size += sizeof(header_length32);
		header_length = header_length32;
	}

	switch(version)
	{
		case DW_DEBUG_LINE_VER2: dw_ver = DW_VER2; break;
		case DW_DEBUG_LINE_VER3: dw_ver = DW_VER3; break;
		case DW_DEBUG_LINE_VER4: dw_ver = DW_VER4; break;
		default: return -1;
	}

	if(dw_fmt == FMT_DWARF64)
	{
		if((header_length > (unit_length - sizeof(version) - sizeof(uint32_t) - sizeof(uint64_t))) || ((unit_length - sizeof(version) - sizeof(uint32_t) - sizeof(uint64_t)) > max_size)) return -1;
	}
	else
	{
		if((header_length > (unit_length - sizeof(version) - sizeof(uint32_t))) || ((unit_length - sizeof(version) - sizeof(uint32_t)) > max_size)) return -1;
	}

	if(max_size < sizeof(minimum_instruction_length)) return -1;
	memcpy(&minimum_instruction_length, rawdata, sizeof(minimum_instruction_length));
	minimum_instruction_length = Target2Host(file_endianness, minimum_instruction_length);
	rawdata += sizeof(minimum_instruction_length);
	max_size -= sizeof(minimum_instruction_length);
	size += sizeof(minimum_instruction_length);

	if(dw_ver == DW_VER4)
	{
		if(max_size < sizeof(maximum_operations_per_instruction)) return -1;
		memcpy(&maximum_operations_per_instruction, rawdata, sizeof(maximum_operations_per_instruction));
		maximum_operations_per_instruction = Target2Host(file_endianness, maximum_operations_per_instruction);
		rawdata += sizeof(maximum_operations_per_instruction);
		max_size -= sizeof(maximum_operations_per_instruction);
		size += sizeof(maximum_operations_per_instruction);
	}
	
	if(max_size < sizeof(default_is_stmt)) return -1;
	memcpy(&default_is_stmt, rawdata, sizeof(default_is_stmt));
	default_is_stmt = Target2Host(file_endianness, default_is_stmt);
	rawdata += sizeof(default_is_stmt);
	max_size -= sizeof(default_is_stmt);
	size += sizeof(default_is_stmt);

	if(max_size < sizeof(line_base)) return -1;
	memcpy(&line_base, rawdata, sizeof(line_base));
	line_base = Target2Host(file_endianness, line_base);
	rawdata += sizeof(line_base);
	max_size -= sizeof(line_base);
	size += sizeof(line_base);

	if(max_size < sizeof(line_range)) return -1;
	memcpy(&line_range, rawdata, sizeof(line_range));
	line_range = Target2Host(file_endianness, line_range);
	rawdata += sizeof(line_range);
	max_size -= sizeof(line_range);
	size += sizeof(line_range);
	
	if(max_size < sizeof(opcode_base)) return -1;
	memcpy(&opcode_base, rawdata, sizeof(opcode_base));
	opcode_base = Target2Host(file_endianness, opcode_base);
	rawdata += sizeof(opcode_base);
	max_size -= sizeof(opcode_base);
	size += sizeof(opcode_base);

	unsigned int i;
	for(i = 1; i < opcode_base; i++)
	{
		if(!max_size) return -1;
		standard_opcode_lengths.push_back(*rawdata);
		
		rawdata++;
		size++;
		max_size--;
	}

	do
	{
		if(!max_size) return -1;
		const char *directory_name = (const char *) rawdata;
		int64_t sz = strlen(directory_name) + 1;
		size += sz;
		rawdata += sz;
		max_size -= sz;
		
		if(*directory_name == 0) break; // empty directory name
		
		include_directories.push_back(directory_name);
	} while(1);

	unsigned int filename_idx = 0;
	do
	{
		if(!max_size) return -1;
		if(!(*rawdata)) break;
		
		DWARF_Filename filename(filename_idx);
			
		int64_t sz;
		if((sz = filename.Load(rawdata, max_size)) < 0) return -1;
		size += sz;
		rawdata += sz;
		max_size -= sz;
		
		filenames.push_back(filename);
		filename_idx++;
	} while(1);

	if(dw_fmt == FMT_DWARF64)
	{
		if(size > header_length + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(version) + sizeof(uint64_t)) return -1;
		rawdata += header_length + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(version) + sizeof(uint64_t) - size;
		max_size -= header_length + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(version) + sizeof(uint64_t) - size;
		size = header_length + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(version) + sizeof(uint64_t);
		program_length = unit_length + sizeof(uint32_t) + sizeof(uint64_t) - size;
	}
	else
	{
		if(size > header_length + sizeof(uint32_t) + sizeof(version) + sizeof(uint32_t)) return -1;
		rawdata += header_length + sizeof(uint32_t) + sizeof(version) + sizeof(uint32_t) - size;
		max_size -= header_length + sizeof(uint32_t) + sizeof(version) + sizeof(uint32_t) - size;
		size = header_length + sizeof(uint32_t) + sizeof(version) + sizeof(uint32_t);
		program_length = unit_length + sizeof(uint32_t) - size;
	}
	
	if(program_length > max_size) return -1;
	program = rawdata;
	size += program_length;

	return size;
}

template <class MEMORY_ADDR>
void DWARF_StatementProgram<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_StatementProgram<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_line/" << (id / debug_line_per_file) << ".html#stmt-prog-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_StatementProgram<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_StatementProgram<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	unsigned int i;
	
	os << "<DW_STMT_PROG id=\"stmt-prog-" << id << "\" unit_length=\"" << unit_length << "\" version=\"" << version << "\" header_length=\"" << header_length << "\"";
	os << " minimum_instruction_length=\"" << (uint32_t) minimum_instruction_length << "\" default_is_stmt=\"" << (uint32_t) default_is_stmt << "\"";
	os << " line_base=\"" << (int32_t) line_base << "\" line_range=\"" << (uint32_t) line_range << "\" opcode_base=\"" << (uint32_t) opcode_base << "\"";
	os << " standard_opcode_lengths=\"";
	unsigned int num_standard_opcode_lengths = standard_opcode_lengths.size();
	for(i = 0; i < num_standard_opcode_lengths; i++)
	{
		if(i != 0) os << " ";
		os << (uint32_t) standard_opcode_lengths[i];
	}
	os << "\">" << std::endl;
	os << "<DW_INCLUDE_DIRECTORIES>" << std::endl;
	unsigned int num_include_directories = include_directories.size();
	for(i = 0; i < num_include_directories; i++)
	{
		os << "<DW_INLCUDE_DIRECTORY name=\"";
		c_string_to_XML(os, include_directories[i]);
		os << "\"/>" << std::endl;
	}
	os << "</DW_INCLUDE_DIRECTORIES>" << std::endl;
	os << "<DW_FILENAMES>" << std::endl;
	unsigned int num_filenames = filenames.size();
	for(i = 0; i < num_filenames; i++)
	{
		filenames[i].to_XML(os) << std::endl;
	}
	os << "</DW_FILENAMES>" << std::endl;
	os << "<DW_STMT_PROG_OPCODES>" << std::endl;
	std::stringstream sstr;
	DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>(dw_handler);
	dw_stmt_vm.Run(this, &sstr, 0);
	c_string_to_XML(os, sstr.str().c_str());
	os << "</DW_STMT_PROG_OPCODES>" << std::endl;
	os << "</DW_STMT_PROG>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_StatementProgram<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	unsigned int i;

	os << "<tr id=\"stmt-prog-" << id << "\"><td>" << std::endl;
	os << "<ul>" << std::endl;
	os << "<li>Id: " << id << "</li>" << std::endl;
	os << "<li>Offset: " << offset << "</li>" << std::endl;
	os << "<li>Version: " << version << "</li>" << std::endl;
	os << "<li>Minimum Instruction Length: " << (uint32_t) minimum_instruction_length << "</li>" << std::endl;
	os << "<li>Initial value of 'is_stmt': " << (uint32_t) default_is_stmt << "</li>" << std::endl;
	os << "<li>Line Base: " << (int32_t) line_base << "</li>" << std::endl;
	os << "<li>Line Range: " << (uint32_t) line_range << "</li>" << std::endl;
	os << "<li>Opcode Base: " << (uint32_t) opcode_base << "</li>" << std::endl;
	os << "<li>Opcodes:" << std::endl;
	os << "<ul>" << std::endl;
	unsigned int num_standard_opcode_lengths = standard_opcode_lengths.size();
	for(i = 0; i < num_standard_opcode_lengths; i++)
	{
		os << "<li>Opcode " << (i + 1) << " has " << (uint32_t) standard_opcode_lengths[i] << " args" << "</li>" << std::endl;
	}
	os << "</ul>" << std::endl;
	os << "</ul>" << std::endl;
	
	os << "Directory Table:<br><table><tr><th>Id</th><th>Directory name</th></tr>" << std::endl;
	unsigned int num_include_directories = include_directories.size();
	for(i = 0; i < num_include_directories; i++)
	{
		os << "<tr><td>" << i << "</td><td>";
		c_string_to_HTML(os, include_directories[i]);
		os << "</td></tr>" << std::endl;
	}
	os << "</table>" << std::endl;
	os << "<br>" << std::endl;
	os << "Filenames Table:<br><table><tr><th>Id</th><th>Filename</th><th>Directory index</th><th>Last modification time</th><th>Byte size</th></tr>" << std::endl;
	unsigned int num_filenames = filenames.size();
	for(i = 0; i < num_filenames; i++)
	{
		filenames[i].to_HTML(os) << std::endl;
	}
	os << "</table>" << std::endl;
	os << "<br>" << std::endl;
	os << "Program (" << program_length << " bytes):<br>" << std::endl;
	
	std::stringstream sstr;
	DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>(dw_handler);
	dw_stmt_vm.Run(this, &sstr, 0);
	c_string_to_HTML(os, sstr.str().c_str());
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog)
{
	unsigned int i;

	os << "Statement program:" << std::endl
	   << " - Total Length: " << dw_stmt_prog.unit_length << std::endl
	   << " - DWARF Version: " << dw_stmt_prog.version << std::endl
	   << " - Prologue Length: " << dw_stmt_prog.header_length << std::endl
	   << " - Minimum Instruction Length: " << (uint32_t) dw_stmt_prog.minimum_instruction_length << std::endl
	   << " - Initial value of 'is_stmt': " << (uint32_t) dw_stmt_prog.default_is_stmt << std::endl
	   << " - Line Base: " << (int32_t) dw_stmt_prog.line_base << std::endl
	   << " - Line Range: " << (uint32_t) dw_stmt_prog.line_range << std::endl
	   << " - Opcode Base: " << (uint32_t) dw_stmt_prog.opcode_base << std::endl;
	
	os << " - Opcodes:" << std::endl;
	unsigned int num_standard_opcode_lengths = dw_stmt_prog.standard_opcode_lengths.size();
	for(i = 0; i < num_standard_opcode_lengths; i++)
	{
		os << "   - Opcode " << (i + 1) << " has " << (uint32_t) dw_stmt_prog.standard_opcode_lengths[i] << " args" << std::endl;
	}
	
	os << " - Directory Table:" << std::endl;
	unsigned int num_include_directories = dw_stmt_prog.include_directories.size();
	for(i = 0; i < num_include_directories; i++)
	{
		os << "   - Entry #" << (i + 1) << ": \"" << dw_stmt_prog.include_directories[i] << "\"" << std::endl;
	}
	os << " - Filenames Table:" << std::endl;
	unsigned int num_filenames = dw_stmt_prog.filenames.size();
	for(i = 0; i < num_filenames; i++)
	{
		os << "   - Entry #" << (i + 1) << ": " << dw_stmt_prog.filenames[i] << std::endl;
	}
	os << " - Program (" << dw_stmt_prog.program_length << " bytes):" << std::endl;
	
	DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>(dw_stmt_prog.dw_handler);
	
	dw_stmt_vm.Run(&dw_stmt_prog, &os, 0);
	return os;
}

template <class MEMORY_ADDR>
uint64_t DWARF_StatementProgram<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
const DWARF_Filename *DWARF_StatementProgram<MEMORY_ADDR>::GetFilename(unsigned int filename_idx) const
{
	return &filenames[filename_idx];
}

template <class MEMORY_ADDR>
const char *DWARF_StatementProgram<MEMORY_ADDR>::GetIncludeDirectory(unsigned int include_directory_idx) const
{
	return include_directories[include_directory_idx];
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_STMT_PROG_TCC__
