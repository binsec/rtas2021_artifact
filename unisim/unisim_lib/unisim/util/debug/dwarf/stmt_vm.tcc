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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_STMT_VM_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_STMT_VM_TCC__

#include <unisim/util/debug/dwarf/stmt_vm.hh>
#include <unisim/util/debug/dwarf/option.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_StatementVM<MEMORY_ADDR>::DWARF_StatementVM(const DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, debug_info_stream(_dw_handler->GetDebugInfoStream())
	, debug_warning_stream(_dw_handler->GetDebugWarningStream())
	, debug_error_stream(_dw_handler->GetDebugErrorStream())
	, debug(false)
	, address(0)
	, op_index(0)
	, file(1)
	, line(1)
	, column(0)
	, is_stmt(false)
	, basic_block(false)
	, end_sequence(false)
	, prologue_end(false)
	, prologue_begin(false)
	, isa(0)
	, discriminator(0)
	, filenames()
{
	dw_handler->GetOption(OPT_DEBUG, debug);
}

template <class MEMORY_ADDR>
DWARF_StatementVM<MEMORY_ADDR>::~DWARF_StatementVM()
{
}

template <class MEMORY_ADDR>
bool DWARF_StatementVM<MEMORY_ADDR>::IsAbsolutePath(const char *filename) const
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	// filename starts with '/' or 'drive letter':\ or 'driver letter':/
	return (((filename[0] >= 'a' && filename[0] <= 'z') || (filename[0] >= 'A' && filename[0] <= 'Z')) && (filename[1] == ':') && ((filename[2] == '\\') || (filename[2] == '/'))) || (*filename == '/');
#else
	// filename starts with '/'
	return (*filename == '/');
#endif
}

template <class MEMORY_ADDR>
void DWARF_StatementVM<MEMORY_ADDR>::AddRow(const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog, std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& stmt_matrix, const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu)
{
	if(dw_cu && !dw_cu->HasOverlap(address, 1)) return; // do not insert row if address is not in current compilation unit
	
	const DWARF_Filename *dw_filename = (file >= 1 && file <= filenames.size()) ? &filenames[file - 1] : 0;
	const char *filename = dw_filename ? dw_filename->GetFilename() : 0;
	const char *dirname = 0;
	if(filename)
	{
		if(!IsAbsolutePath(filename))
		{
			const DWARF_LEB128& leb128_dir = dw_filename->GetDirectoryIndex();
			unsigned int dir = (unsigned int) leb128_dir;
			if(dir != 0) // current directory of the compilation ?
			{
				dirname = (dir <= dw_stmt_prog->include_directories.size()) ? dw_stmt_prog->include_directories[dir - 1] : 0;
				if(!dirname)
				{
					if(debug)
					{
						debug_warning_stream << "Directory index is out of range in line number program" << std::endl;
					}
					
					return;
				}
			}
		}
	}
	// At this point, if dirname is null we know that source filename is an absolute path or it is relative to the current directory of the compilation
	
	// Check that there's no duplicated entry in the statement matrix
	std::pair<typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::iterator, typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::iterator> stmt_matrix_equal_range = stmt_matrix.equal_range(address);
	
	typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::iterator stmt_matrix_iter = stmt_matrix_equal_range.first;
	
	while(stmt_matrix_iter != stmt_matrix_equal_range.second)
	{
		typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::iterator next_stmt_matrix_iter = stmt_matrix_iter;
		next_stmt_matrix_iter++;
		
		const Statement<MEMORY_ADDR> *existing_stmt = (*stmt_matrix_iter).second;
		
		if(existing_stmt->GetDiscriminator() == discriminator)
		{
			// If there's already a row, I suppose that overwritting it is the correct behavior
			delete (*stmt_matrix_iter).second;
			stmt_matrix.erase(stmt_matrix_iter);
		}
			
		stmt_matrix_iter = next_stmt_matrix_iter;
	}
	
	Statement<MEMORY_ADDR> *stmt = new Statement<MEMORY_ADDR>(address, is_stmt, basic_block, dirname, filename, line, column, isa, discriminator);
	stmt_matrix.insert(std::pair<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>(address, stmt));
}

template <class MEMORY_ADDR>
bool DWARF_StatementVM<MEMORY_ADDR>::Run(const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog, std::ostream *os, std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *> *matrix, const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu)
{
	// Initialize machine state
	address = 0;
	op_index = 0;
	file = 1;
	line = 1;
	column = 0;
	is_stmt = dw_stmt_prog->default_is_stmt;
	basic_block = false;
	end_sequence = false;
	prologue_end = false;
	prologue_begin = false;
	isa = 0;
	discriminator = 0;
	filenames = dw_stmt_prog->filenames;

	// Run the program
	endian_type file_endianness = dw_stmt_prog->GetHandler()->GetFileEndianness();
	const uint8_t *prg = dw_stmt_prog->program;
	unsigned int count = dw_stmt_prog->program_length;
	uint8_t opcode;

	if(count > 0)
	{
		do
		{
			opcode = *prg;
			prg++;
			count--;
			if(opcode)
			{
				if(opcode < dw_stmt_prog->opcode_base)
				{
					unsigned int num_args = dw_stmt_prog->standard_opcode_lengths[opcode - 1];
					unsigned int i = 0;
					std::vector<DWARF_LEB128> args(num_args);
					uint16_t uhalf_arg = 0;
					
					if(opcode != DW_LNS_fixed_advance_pc)
					{
						for(i = 0; i < num_args; i++)
						{
							int64_t sz;
							if((sz = args[i].Load(prg, count)) < 0)
							{
								if(debug)
								{
									debug_error_stream << "LEB128 argument #" << i << " of standard Opcode 0x" << std::hex << (unsigned int) opcode << std::dec << " is bad or missing in line number program" << std::endl;
								}
								return false;
							}
							prg += sz;
							count -= sz;
						}
					}
					else
					{
						if(count < sizeof(uhalf_arg))
						{
							if(debug)
							{
								debug_error_stream << "uhalf argument #" << i << " of standard Opcode 0x" << std::hex << (unsigned int) opcode << std::dec << " is bad or missing in line number program" << std::endl;
							}
							return false;
						}
						memcpy(&uhalf_arg, prg, sizeof(uhalf_arg));
						uhalf_arg = Target2Host(file_endianness, uhalf_arg);
						prg += sizeof(uhalf_arg);
						count -= sizeof(uhalf_arg);
					}
					
					// Standard opcode
					if(os) (*os) << "   - Standard opcode " << (unsigned int) opcode << ": ";
					switch(opcode)
					{
						case DW_LNS_copy:
							// Copy
							if(os) (*os) << "Copy" << std::endl;
							if(matrix) AddRow(dw_stmt_prog, *matrix, dw_cu);
							// Reset basic_block
							basic_block = false;
							// Reset prologue_end
							prologue_end = false;
							// Reset prologue_begin
							prologue_begin = false;
							// Reset discriminator
							discriminator = 0;
							break;
						case DW_LNS_advance_pc:
							{
								unsigned int operation_advance = (unsigned int) args[0];
								unsigned int address_increment = ((op_index + operation_advance) / dw_stmt_prog->maximum_operations_per_instruction) * dw_stmt_prog->minimum_instruction_length;
								// Advance PC by 'address_increment'
								address += address_increment;
								op_index = (op_index + operation_advance) % dw_stmt_prog->maximum_operations_per_instruction;
								if(os) (*os) << "Advance PC by " << address_increment << " to 0x" << std::hex << address << std::dec << std::endl;
							}
							break;
						case DW_LNS_advance_line:
							{
								const DWARF_LEB128& line_increment = args[0];

								// Advance Line by 'line_increment'
								line += (int) line_increment;
								if(os) (*os) << "Advance Line by " << line_increment.to_string(true) << " to " << line << std::endl;
							}
							break;
						case DW_LNS_set_file:
							{
								const DWARF_LEB128& file_entry = args[0];
								
								// Set File to entry # 'file_entry' in Filenames Table
								file = (unsigned int) file_entry;
								if(os) (*os) << "Set File to entry #" << file << " in Filenames Table" << std::endl;
							}
							break;
						case DW_LNS_set_column:
							{
								const DWARF_LEB128& col = args[0];
								
								// Set Column to 'column'
								column = (unsigned int) col;
								if(os) (*os) << "Set Column to " << column << std::endl;
							}
							break;
						case DW_LNS_negate_stmt:
							// Negate is_stmt
							is_stmt = !is_stmt;
							if(os) (*os) << "Negate is_stmt" << std::endl;
							break;
						case DW_LNS_set_basic_block:
							// Set basic block
							basic_block = true;
							if(os) (*os) << "Set basic block" << std::endl;
							break;
						case DW_LNS_const_add_pc:
							{
								unsigned int operation_advance = (255 - dw_stmt_prog->opcode_base) / dw_stmt_prog->line_range;
								unsigned int address_increment = ((op_index + operation_advance) / dw_stmt_prog->maximum_operations_per_instruction) * dw_stmt_prog->minimum_instruction_length;
								//int address_increment = ((255 - dw_stmt_prog->opcode_base) / dw_stmt_prog->line_range) * dw_stmt_prog->minimum_instruction_length;
								// Advance PC by constant 'address_increment'
								address += address_increment;
								if(os) (*os) << "Advance PC by constant " << address_increment << " to 0x" << std::hex << address << std::dec << std::endl;
							}
							break;
						case DW_LNS_fixed_advance_pc:
							// Advance PC by fixed 'uhalf_arg'
							address += uhalf_arg;
							// Reset op_index
							op_index = 0;
							if(os) (*os) << "Advance PC by fixed " << uhalf_arg << " to 0x" << std::hex << address << std::dec << std::endl;
							break;
						case DW_LNS_set_prologue_end:
							prologue_end = true;
							if(os) (*os) << "DW_LNS_set_prologue_end" << std::endl;
							break;
						case DW_LNS_set_epilogue_begin:
							prologue_end = false;
							if(os) (*os) << "DW_LNS_set_epilogue_begin" << std::endl;
							break;
						case DW_LNS_set_isa:
							{
								const DWARF_LEB128& isa_val = args[0];
								
								// Set isa to 'isa_val'
								isa = (unsigned int) isa_val;
								if(os) (*os) << "Set isa to " << isa << std::endl;
							}
							break;
						default:
							// Skipped unknown
							if(os) (*os) << "Skipped unknown" << std::endl;
							break;
					}
				}
				else
				{
					// Special opcode
					uint8_t adjusted_opcode = opcode - dw_stmt_prog->opcode_base;
					int line_increment = dw_stmt_prog->line_base + (adjusted_opcode % dw_stmt_prog->line_range);
		
					unsigned int operation_advance = adjusted_opcode / dw_stmt_prog->line_range;
					unsigned int address_increment = ((op_index + operation_advance) / dw_stmt_prog->maximum_operations_per_instruction) * dw_stmt_prog->minimum_instruction_length;
					// unsigned int address_increment = (adjusted_opcode / dw_stmt_prog->line_range) * dw_stmt_prog->minimum_instruction_length;
					// Special opcode 'adjusted_opcode': advance Address by 'address_increment' and Line by 'line_increment'
					address += address_increment;
					op_index = (op_index + operation_advance) % dw_stmt_prog->maximum_operations_per_instruction;
					line += line_increment;
					// Add a row to matrix
					if(matrix) AddRow(dw_stmt_prog, *matrix, dw_cu);
					// Reset basic_block
					basic_block = false;
					// Reset prologue_end
					prologue_end = false;
					// Reset prologue_begin
					prologue_begin = false;
					// Reset discriminator
					discriminator = 0;
					if(os) (*os) << "   - Special opcode " << (unsigned int) adjusted_opcode << ": advance Address by " << address_increment << " to 0x" << std::hex << address << std::dec << " and Line by " << line_increment << " to " << line << std::endl;
				}
			}
			else
 			{
				// Extended opcode
				DWARF_LEB128 leb128_insn_length;
				int64_t sz;
				if((sz = leb128_insn_length.Load(prg, count)) < 0)
				{
					if(debug)
					{
						debug_error_stream << "Length (encoded as a LEB128) of an instruction (with an extended opcode) is bad or missing in line number program" << std::endl;
					}
					return false;
				}
				prg += sz;
				count -= sz;
				uint64_t insn_length = (uint64_t) leb128_insn_length;
				opcode = *prg;
				prg++;
				count--;
				// Extended opcode 'opcode'
				if(os) (*os) << "   - Extended opcode " << (unsigned int) opcode << ": ";
				
				switch(opcode)
				{
					case DW_LNE_end_sequence:
						// End of Sequence
						end_sequence = true;
						// Add a row to matrix
						if(matrix) AddRow(dw_stmt_prog, *matrix, dw_cu);
						// Reset machine state but end_sequence !
						address = 0;
						file = 1;
						line = 1;
						column = 0;
						is_stmt = dw_stmt_prog->default_is_stmt;
						end_sequence = false;
						basic_block = false;
						prologue_end = false;
						prologue_begin = false;
						isa = 0;
						if(os) (*os) << "End of Sequence" << std::endl;
						break;
					case DW_LNE_set_address:
						{
							if(os) (*os) << "Set Address to ";
							uint64_t addr_length = insn_length - 1;
							switch(addr_length)
							{
								case sizeof(uint32_t):
									{
										uint32_t addr;
										memcpy(&addr, prg, sizeof(uint32_t));
										addr = Target2Host(file_endianness, addr);
										// Set Address to 'addr'
										address = addr;
									}
									break;
								case sizeof(uint64_t):
									{
										uint64_t addr;
										memcpy(&addr, prg, sizeof(uint64_t));
										addr = Target2Host(file_endianness, addr);
										// Set Address to 'addr'
										address = addr;
									}
									break;
								default:
									if(debug)
									{
										debug_error_stream << "DW_LNE_set_address: unsupported address size" << std::endl;
									}
									return false;
							}
							// Reset op_index
							op_index = 0;
							if(os) (*os) << std::hex << "0x" << address << std::dec;
						}
						break;
					case DW_LNE_define_file:
						{
							unsigned int filename_idx = filenames.size();
							DWARF_Filename dw_filename(filename_idx);
							int64_t sz;
							if((sz = dw_filename.Load(prg, count)) < 0)
							{
								if(debug)
								{
									debug_error_stream << "DW_LNE_define_file: bad DWARF filename entry" << std::endl;
								}
								return false;
							}
							// Define File 'dw_filename'
							filenames.push_back(dw_filename);
							if(os) (*os) << "Define File " << dw_filename;
						}
						break;
					case DW_LNE_set_discriminator:
						{
							DWARF_LEB128 discriminator_val;
							int64_t sz;
							if((sz = discriminator_val.Load(prg, count)) < 0)
							{
								if(debug)
								{
									debug_error_stream << "DW_LNE_set_discriminator: discriminator is bad or missing in line number program" << std::endl;
								}
								return false;
							}
							discriminator = (unsigned int) discriminator_val;
							if(os) (*os) << "Set Discriminator to " << discriminator;
						}
						break;
					default:
						if(debug)
						{
							debug_error_stream << "unknown extended Opcode 0x" << std::hex << (unsigned int) opcode << std::dec << std::endl;
						}
						return false;
				}
				prg += insn_length - 1;
				count -= insn_length - 1;
				if(os) (*os) << std::endl;
			}
		}
		while(count);
	}
	return true;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_STMT_VM_TCC__
