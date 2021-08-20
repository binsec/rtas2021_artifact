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
 
#ifndef __UNISIM_UTIL_DEBUG_COFF_SYMTAB_COFF_SYMTAB_HH__
#define __UNISIM_UTIL_DEBUG_COFF_SYMTAB_COFF_SYMTAB_HH__

#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/symbol_table.hh>

#include <iosfwd>
#include <inttypes.h>
#include <vector>
#include <map>

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define __UNISIM_PACKED__ __attribute__ ((packed))
#else
#define __UNISIM_PACKED__
#endif

namespace unisim {
namespace util {
namespace debug {
namespace coff_symtab {

template <class MEMORY_ADDR>
class Coff_SymtabHandler
{
public:
	Coff_SymtabHandler(std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream, const unisim::util::blob::Blob<MEMORY_ADDR> *blob);
	~Coff_SymtabHandler();

	void Parse();
	
	void GetSymbols(typename std::list<const unisim::util::debug::Symbol<MEMORY_ADDR> *>& lst, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
private:
	// Basic types
	typedef enum
	{
		T_NULL   = 0,    // type not assigned
		T_VOID   = 1,    // function argument (only used by compiler)
		T_CHAR   = 2,    // character
		T_SHORT  = 3,    // short integer
		T_INT    = 4,    // integer
		T_LONG   = 5,    // long integer
		T_FLOAT  = 6,    // floating point
		T_DOUBLE = 7,    // double precision floating point
		T_STRUCT = 8,    // structure
		T_UNION  = 9,    // union
		T_ENUM   = 10,   // enumeration
		T_MOE    = 11,   // member of enumeration
		T_UCHAR  = 12,   // unsigned character
		T_USHORT = 13,   // unsigned short
		T_UINT   = 14,   // unsigned integer
		T_ULONG  = 15,   // unsigned long
		NUM_BASIC_TYPES
	} BasicType;

	// symbol storage classes
	static const uint8_t C_NULL    = 0;    // no storage class
	static const uint8_t C_AUTO    = 1;    // automatic variable
	static const uint8_t C_EXT     = 2;    // external symbol
	static const uint8_t C_STAT    = 3;    // static
	static const uint8_t C_REG     = 4;    // register variable
	static const uint8_t C_EXTDEF  = 5;    // external definition
	static const uint8_t C_LABEL   = 6;    // label
	static const uint8_t C_ULABEL  = 7;    // undefined label
	static const uint8_t C_MOS     = 8;    // member of structure
	static const uint8_t C_ARG     = 9;    // function argument
	static const uint8_t C_STRTAG  = 10;   // structure tag
	static const uint8_t C_MOU     = 11;   // member of union
	static const uint8_t C_UNTAG   = 12;   // union tag
	static const uint8_t C_TPDEF   = 13;   // type definition
	static const uint8_t C_USTATIC = 14;   // undefined static
	static const uint8_t C_ENTAG   = 15;   // enumeration tag
	static const uint8_t C_MOE     = 16;   // member of enumeration
	static const uint8_t C_REGPARM = 17;   // register parameter
	static const uint8_t C_FIELD   = 18;   // bit field
	static const uint8_t C_AUTOARG = 19;   // auto argument
	static const uint8_t C_LASTENT = 20;   // dummy entry (end of block)
	static const uint8_t C_BLOCK   = 100;  // beginning or end of a block: ".bb" or ".eb"
	static const uint8_t C_FCN     = 101;  // beginning or end of a function: ".bf" or ".ef"
	static const uint8_t C_EOS     = 102;  // end of structure
	static const uint8_t C_FILE    = 103;  // file name
	static const uint8_t C_LINE    = 104;  // line # reformatted as symbol table entry
	static const uint8_t C_ALIAS   = 105;  // duplicate tag
	static const uint8_t C_HIDDEN  = 106;  // ext symbol in dmert public lib
	static const uint8_t C_EFCN    = 255;  // physical end of function

	// Basic type bitfield size
	static const unsigned int N_BTSZ = 4;  // note: some architectures have this to 5 !

	// Derived types
	static const uint16_t DT_NON = 0;      // no derived type
	static const uint16_t DT_PTR = 1;      // pointer
	static const uint16_t DT_FCN = 2;      // function
	static const uint16_t DT_ARY = 3;      // array

	// Derived type bitfield size
	static const unsigned int N_DTSZ = 2;

	// symbol name length
	static const unsigned int E_SYMNMLEN = 8;

	// filename name length
	static const unsigned int E_FILNMLEN = 14;

	// number of dimensions in arrays
	static const unsigned int E_DIMNUM = 4;

	// Symbol table entry
	typedef struct __UNISIM_PACKED__
	{
		union __UNISIM_PACKED__
		{
			char e_name[E_SYMNMLEN];  // name of the symbol padded with null characters
			struct __UNISIM_PACKED__
			{
				uint32_t e_zeroes; // equals to 0 if in string table
				uint32_t e_offset; // offset in string table
			} e;
		} e;
		uint32_t e_value;  // symbol value (storage class dependent)
		int16_t e_scnum;   // section number of the symbol
		uint16_t e_type;   // basic and derived type specification
		uint8_t e_sclass;  // storage class of the symbol
		uint8_t e_numaux;  // number of auxiliary entries
	} syment;

	typedef struct __UNISIM_PACKED__
	{
		int32_t x_tagndx;
		int32_t x_fsize;
		int32_t x_lnnoptr;
		int32_t x_endndx;
	} fcn_auxent;

	typedef struct __UNISIM_PACKED__
	{
		int32_t x_scnlen;
		uint16_t x_nreloc;
		uint16_t x_nlnno;
	} scn_auxent;

	typedef struct __UNISIM_PACKED__
	{
		int32_t x_tagndx;
		uint16_t x_lnno;
		uint16_t x_arylen;
		uint16_t x_dim[E_DIMNUM];
	} ary_auxent;

	typedef struct __UNISIM_PACKED__
	{
		union __UNISIM_PACKED__
		{
			char x_fname[E_FILNMLEN];  // file name padded with null characters
			struct __UNISIM_PACKED__
			{
				uint32_t x_zeroes; // equals to 0 if in string table
				uint32_t x_offset; // offset in string table
			} x;
		} x;
	} file_auxent;

	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	const unisim::util::blob::Blob<MEMORY_ADDR> *blob;
	unisim::util::debug::SymbolTable<MEMORY_ADDR> *symbol_table;

	const char *GetStorageClassName(uint8_t sclass) const;
	const char *GetStorageClassFriendlyName(uint8_t sclass) const;
	const char *GetBasicTypeName(uint16_t basic_type) const;
	const char *GetBasicTypeFriendlyName(uint16_t basic_type) const;
	const char *GetDerivedTypeName(uint16_t derived_type) const;
	const char *GetDerivedTypeFriendlyName(uint16_t derived_type) const;
	std::string GetTypeName(uint16_t type) const;
};

} // end of namespace coff_symtab
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
