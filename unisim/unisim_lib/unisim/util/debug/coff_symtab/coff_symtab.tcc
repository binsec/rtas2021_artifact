/*
 *  Copyright (c) 2009-2012,
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
 
#ifndef __UNISIM_UTIL_DEBUG_COFF_SYMTAB_COFF_SYMTAB_TCC__
#define __UNISIM_UTIL_DEBUG_COFF_SYMTAB_COFF_SYMTAB_TCC__

#include <sstream>

namespace unisim {
namespace util {
namespace debug {
namespace coff_symtab {

template <class MEMORY_ADDR>
Coff_SymtabHandler<MEMORY_ADDR>::Coff_SymtabHandler(std::ostream& _debug_info_stream, std::ostream& _debug_warning_stream, std::ostream& _debug_error_stream, const unisim::util::blob::Blob<MEMORY_ADDR> *_blob)
	: debug_info_stream(_debug_info_stream)
	, debug_warning_stream(_debug_warning_stream)
	, debug_error_stream(_debug_error_stream)
	, blob(_blob)
	, symbol_table(0)
{
	if(blob) blob->Catch();
}

template <class MEMORY_ADDR>
Coff_SymtabHandler<MEMORY_ADDR>::~Coff_SymtabHandler()
{
	if(blob)
	{
		blob->Release();
		blob = 0;
	}
	
	if(symbol_table)
	{
		delete symbol_table;
	}
}

template <class MEMORY_ADDR>
void Coff_SymtabHandler<MEMORY_ADDR>::Parse()
{
	if(symbol_table) delete symbol_table;
	
	symbol_table = new SymbolTable<MEMORY_ADDR>();

	if(!blob) return;
	
	bool dump_headers = false;
	MEMORY_ADDR basic_type_sizes[NUM_BASIC_TYPES];
	
	// Note: basic_type_sizes is architecture dependent
	// Values below are for TMS320C3X
	basic_type_sizes[T_NULL] = 0;
	basic_type_sizes[T_VOID] = 0;
	basic_type_sizes[T_CHAR] = 1;
	basic_type_sizes[T_SHORT] = 1;
	basic_type_sizes[T_INT] = 1;
	basic_type_sizes[T_LONG] = 1;
	basic_type_sizes[T_FLOAT] = 1;
	basic_type_sizes[T_DOUBLE] = 1;
	basic_type_sizes[T_STRUCT] = 1;
	basic_type_sizes[T_UNION] = 1;
	basic_type_sizes[T_ENUM] = 1;
	basic_type_sizes[T_MOE] = 1;
	basic_type_sizes[T_UCHAR] = 1;
	basic_type_sizes[T_USHORT] = 1;
	basic_type_sizes[T_UINT] = 1;
	basic_type_sizes[T_ULONG] = 1;
	
	unisim::util::endian::endian_type file_endianness = blob->GetFileEndian();
	
	const unisim::util::blob::Section<MEMORY_ADDR> *symtab_section = blob->FindSection(unisim::util::blob::Section<MEMORY_ADDR>::TY_COFF_SYMTAB);
	
	if(!symtab_section) return;
	
	const unisim::util::blob::Section<MEMORY_ADDR> *strtab_section = blob->FindSection(unisim::util::blob::Section<MEMORY_ADDR>::TY_STRTAB);
	
	if(!strtab_section) return;
	
	const syment *symtab = (const syment *) symtab_section->GetData();
	unsigned long nsyms = symtab_section->GetSize() / sizeof(syment);
	const char *strtab = (const char *) strtab_section->GetData();
//	uint32_t strtab_size = strtab_section->GetSize();
	unsigned int memory_atom_size = blob->GetMemoryAtomSize();
	unsigned long i;
	int numaux = 0;
	char name_buf[E_SYMNMLEN + 1];
	const syment *coff_sym = 0;
	const char *sym_name = 0;

	for(i = 0; i < nsyms; i++)
	{
		if(dump_headers)
		{
			debug_info_stream << "[" << i << "] ";
		}

		if(numaux)
		{
			uint8_t sclass = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_sclass);
			uint16_t basic_type = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_type) & ((1 << N_BTSZ) - 1);
			uint16_t derived_type1 = (unisim::util::endian::Target2Host(file_endianness, coff_sym->e_type) >> N_BTSZ) & ((1 << N_DTSZ) - 1);

			if((sclass == C_EXT || sclass == C_STAT) && derived_type1 == DT_FCN && basic_type != T_MOE)
			{
				fcn_auxent *fcn_aux = (fcn_auxent *) &symtab[i];

				if(dump_headers)
				{
					debug_info_stream << "FCNAUX: ";
					debug_info_stream << "size=" << unisim::util::endian::Target2Host(file_endianness, fcn_aux->x_fsize) << ",linenoptr=" << unisim::util::endian::Target2Host(file_endianness, fcn_aux->x_lnnoptr) << ", nextentry_index=" << unisim::util::endian::Target2Host(file_endianness, fcn_aux->x_endndx);
				}

				symbol_table->AddSymbol(sym_name, unisim::util::endian::Target2Host(file_endianness, coff_sym->e_value) * memory_atom_size, unisim::util::endian::Target2Host(file_endianness, fcn_aux->x_fsize) * memory_atom_size, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_FUNC, memory_atom_size);
			}
			else if(sclass == C_STAT && derived_type1 == DT_NON && basic_type == T_NULL)
			{
				scn_auxent *scn_aux = (scn_auxent *) &symtab[i];

				if(dump_headers)
				{
					debug_info_stream << "SCNAUX: ";
					debug_info_stream << "length=" << unisim::util::endian::Target2Host(file_endianness, scn_aux->x_scnlen);
				}

				symbol_table->AddSymbol(sym_name, unisim::util::endian::Target2Host(file_endianness, coff_sym->e_value) * memory_atom_size, unisim::util::endian::Target2Host(file_endianness, scn_aux->x_scnlen) * memory_atom_size, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_SECTION, memory_atom_size);
			}
			else if(sclass == C_FILE && derived_type1 == DT_NON && basic_type == T_NULL)
			{
				file_auxent *file_aux = (file_auxent *) &symtab[i];

				const char *filename;
				char filename_buf[E_FILNMLEN + 1];

				if(file_aux->x.x.x_zeroes == 0)
				{
					// file name is in string table
					uint32_t strtab_ofs = unisim::util::endian::Target2Host(file_endianness, file_aux->x.x.x_offset);
					filename = strtab + strtab_ofs;
				}
				else
				{
					memcpy(filename_buf, file_aux->x.x_fname, E_FILNMLEN);
					filename_buf[E_FILNMLEN] = 0;
					filename = filename_buf;
				}

				if(dump_headers)
				{
					debug_info_stream << "FILAUX: ";
					debug_info_stream << "filename=" << filename;
				}

				symbol_table->AddSymbol(filename, unisim::util::endian::Target2Host(file_endianness, coff_sym->e_value) * memory_atom_size, 0, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_FILE, memory_atom_size);
			}
			else if((sclass == C_EXT || sclass == C_STAT) && derived_type1 == DT_ARY)
			{
				ary_auxent *ary_aux = (ary_auxent *) &symtab[i];

				if(dump_headers)
				{
					debug_info_stream << "ARYAUX: ";
					debug_info_stream << "length=" << unisim::util::endian::Target2Host(file_endianness, ary_aux->x_arylen) << ", dim1=" << unisim::util::endian::Target2Host(file_endianness, ary_aux->x_dim[0]);
				}

				symbol_table->AddSymbol(sym_name, unisim::util::endian::Target2Host(file_endianness, coff_sym->e_value) * memory_atom_size, basic_type_sizes[basic_type] * unisim::util::endian::Target2Host(file_endianness, ary_aux->x_arylen) * memory_atom_size, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_OBJECT, memory_atom_size);
			}
			else
			{
				if(dump_headers)
				{
					debug_info_stream << "AUX: <?>";
				}
			}

			numaux--;
		}
		else
		{
			coff_sym = &symtab[i];

			if(coff_sym->e.e.e_zeroes == 0)
			{
				// name is in string table
				uint32_t strtab_ofs = unisim::util::endian::Target2Host(file_endianness, coff_sym->e.e.e_offset);
				sym_name = strtab + strtab_ofs;
			}
			else
			{
				memcpy(name_buf, coff_sym->e.e_name, E_SYMNMLEN);
				name_buf[E_SYMNMLEN] = 0;
				sym_name = name_buf;
			}

			uint8_t sclass = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_sclass);
			uint16_t type = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_type);
			uint16_t basic_type = type & ((1 << N_BTSZ) - 1);
			uint16_t derived_type1 = (type >> N_BTSZ) & ((1 << N_DTSZ) - 1);
			numaux = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_numaux);

			if(dump_headers)
			{
				debug_info_stream << "SYMBOL: ";
				debug_info_stream << "name=\"" << sym_name << "\", value=0x" << std::hex << unisim::util::endian::Target2Host(file_endianness, coff_sym->e_value) << std::dec;
				debug_info_stream << ", storage class=" << GetStorageClassName(sclass) << " (" << GetStorageClassFriendlyName(sclass) << ")";
				debug_info_stream << ", type=" << GetTypeName(type);

				if(numaux)
				{
					debug_info_stream << ", " << (unsigned int) numaux << " aux entries follow";
				}
			}

			if(sclass == C_EXT && derived_type1 == DT_NON)
			{
				symbol_table->AddSymbol(sym_name, unisim::util::endian::Target2Host(file_endianness, coff_sym->e_value) * memory_atom_size, basic_type_sizes[basic_type] * memory_atom_size, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_OBJECT, memory_atom_size);
			}
			else if(sclass == C_FILE && !numaux)
			{
				symbol_table->AddSymbol(sym_name, 0, 0, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_FILE, memory_atom_size);
			}
		}

		if(dump_headers)
		{
			debug_info_stream << std::endl;
		}
	}
}

template <class MEMORY_ADDR>
void Coff_SymtabHandler<MEMORY_ADDR>::GetSymbols(typename std::list<const unisim::util::debug::Symbol<MEMORY_ADDR> *>& lst, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	if(symbol_table)
	{
		symbol_table->GetSymbols(lst, type);
	}
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *Coff_SymtabHandler<MEMORY_ADDR>::FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table ? symbol_table->FindSymbol(name, addr, type) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *Coff_SymtabHandler<MEMORY_ADDR>::FindSymbolByAddr(MEMORY_ADDR addr) const
{
	return symbol_table ? symbol_table->FindSymbolByAddr(addr) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *Coff_SymtabHandler<MEMORY_ADDR>::FindSymbolByName(const char *name) const
{
	return symbol_table ? symbol_table->FindSymbolByName(name) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *Coff_SymtabHandler<MEMORY_ADDR>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table ? symbol_table->FindSymbolByName(name, type) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *Coff_SymtabHandler<MEMORY_ADDR>::FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table ? symbol_table->FindSymbolByAddr(addr, type) : 0;
}

template <class MEMORY_ADDR>
const char *Coff_SymtabHandler<MEMORY_ADDR>::GetStorageClassName(uint8_t sclass) const
{
	switch(sclass)
	{
		case C_NULL: return "C_NULL";
		case C_AUTO: return "C_AUTO";
		case C_EXT: return "C_EXT";
		case C_STAT: return "C_STAT";
		case C_REG: return "C_REG";
		case C_EXTDEF: return "C_EXTDEF";
		case C_LABEL: return "C_LABEL";
		case C_ULABEL: return "C_ULABEL";
		case C_MOS: return "C_MOS";
		case C_ARG: return "C_ARG";
		case C_STRTAG: return "C_STRTAG";
		case C_MOU: return "C_MOU";
		case C_UNTAG: return "C_UNTAG";
		case C_TPDEF: return "C_TPDEF";
		case C_USTATIC: return "C_USTATIC";
		case C_ENTAG: return "C_ENTAG";
		case C_MOE: return "C_MOE";
		case C_REGPARM: return "C_REGPARM";
		case C_FIELD: return "C_FIELD";
		case C_AUTOARG: return "C_AUTOARG";
		case C_LASTENT: return "C_LASTENT";
		case C_BLOCK: return "C_BLOCK";
		case C_FCN: return "C_FCN";
		case C_EOS: return "C_EOS";
		case C_FILE: return "C_FILE";
		case C_LINE: return "C_LINE";
		case C_ALIAS: return "C_ALIAS";
		case C_HIDDEN: return "C_HIDDEN";
		case C_EFCN: return "C_EFCN";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *Coff_SymtabHandler<MEMORY_ADDR>::GetStorageClassFriendlyName(uint8_t sclass) const
{
	switch(sclass)
	{
		case C_NULL: return "no storage class";
		case C_AUTO: return "automatic variable";
		case C_EXT: return "external symbol";
		case C_STAT: return "static";
		case C_REG: return "register variable";
		case C_EXTDEF: return "external definition";
		case C_LABEL: return "label";
		case C_ULABEL: return "undefined label";
		case C_MOS: return "member of structure";
		case C_ARG: return "function argument";
		case C_STRTAG: return "structure tag";
		case C_MOU: return "member of union";
		case C_UNTAG: return "union tag";
		case C_TPDEF: return "type definition";
		case C_USTATIC: return "undefined static";
		case C_ENTAG: return "enumeration tag";
		case C_MOE: return "member of enumeration";
		case C_REGPARM: return "register parameter";
		case C_FIELD: return "bit field";
		case C_AUTOARG: return "auto argument";
		case C_LASTENT: return "dummy entry (end of block)";
		case C_BLOCK: return "beginning or end of a block: \".bb\" or \".eb\"";
		case C_FCN: return "beginning or end of a function: \".bf\" or \".ef\"";
		case C_EOS: return "end of structure";
		case C_FILE: return "file name";
		case C_LINE: return "line # reformatted as symbol table entry";
		case C_ALIAS: return "duplicate tag";
		case C_HIDDEN: return "ext symbol in dmert public lib";
		case C_EFCN: return "physical end of function";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *Coff_SymtabHandler<MEMORY_ADDR>::GetBasicTypeName(uint16_t basic_type) const
{
	switch(basic_type)
	{
		case T_NULL: return "T_NULL";
		case T_VOID: return "T_VOID";
		case T_CHAR: return "T_CHAR";
		case T_SHORT: return "T_SHORT";
		case T_INT: return "T_INT";
		case T_LONG: return "T_LONG";
		case T_FLOAT: return "T_FLOAT";
		case T_DOUBLE: return "T_DOUBLE";
		case T_STRUCT: return "T_STRUCT";
		case T_UNION: return "T_UNION";
		case T_ENUM: return "T_ENUM";
		case T_MOE: return "T_NOE";
		case T_UCHAR: return "T_UCHAR";
		case T_USHORT: return "T_USHORT";
		case T_UINT: return "T_UINT";
		case T_ULONG: return "T_ULONG";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *Coff_SymtabHandler<MEMORY_ADDR>::GetBasicTypeFriendlyName(uint16_t basic_type) const
{
	switch(basic_type)
	{
		case T_VOID: return "void";
		case T_CHAR: return "char";
		case T_SHORT: return "short";
		case T_INT: return "int";
		case T_LONG: return "long";
		case T_FLOAT: return "float";
		case T_DOUBLE: return "double";
		case T_STRUCT: return "struct";
		case T_UNION: return "union";
		case T_ENUM: return "enum";
		case T_MOE: return "enum member";
		case T_UCHAR: return "unsigned char";
		case T_USHORT: return "unsigned short";
		case T_UINT: return "unsigned int";
		case T_ULONG: return "unsigned long";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *Coff_SymtabHandler<MEMORY_ADDR>::GetDerivedTypeName(uint16_t derived_type) const
{
	switch(derived_type)
	{
		case DT_NON: return "DT_NON";
		case DT_PTR: return "DT_PTR";
		case DT_FCN: return "DT_FCN";
		case DT_ARY: return "DT_ARY";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *Coff_SymtabHandler<MEMORY_ADDR>::GetDerivedTypeFriendlyName(uint16_t derived_type) const
{
	switch(derived_type)
	{
		case DT_PTR: return "*";
		case DT_FCN: return "()";
		case DT_ARY: return "[]";
	}
	return "?";
}

template <class MEMORY_ADDR>
std::string Coff_SymtabHandler<MEMORY_ADDR>::GetTypeName(uint16_t type) const
{
	std::stringstream sstr;

	uint16_t ty = type;

	uint16_t basic_type = ty & ((1 << N_BTSZ) - 1);
	ty >>= N_BTSZ;

	sstr << GetBasicTypeName(basic_type);

	unsigned int i;
	for(i = 0; i < 6; i++, ty >>= N_DTSZ)
	{
		uint16_t derived_type = ty & ((1 << N_DTSZ) - 1);
		//if(derived_type == DT_NON) break;
		sstr << " " << GetDerivedTypeName(derived_type);
	}

	sstr << " => ";
	for(ty = type >> N_BTSZ, i = 0; i < 6; i++, ty >>= N_DTSZ)
	{
		uint16_t derived_type = ty & ((1 << N_DTSZ) - 1);
		//if(derived_type == DT_NON) break;
		sstr << GetDerivedTypeFriendlyName(derived_type);
	}

	sstr << " " << GetBasicTypeFriendlyName(basic_type);

	return sstr.str();
}

} // end of namespace coff_symtab
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
