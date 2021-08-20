/*
 *  Copyright (c) 2007-2011,
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

#ifndef __UNISIM_UTIL_DEBUG_ELF_SYMTAB_ELF_SYMTAB_HH__
#define __UNISIM_UTIL_DEBUG_ELF_SYMTAB_ELF_SYMTAB_HH__

#include <unisim/util/blob/blob.hh>
#include <unisim/util/debug/symbol.hh>
#include <unisim/util/debug/symbol_table.hh>
#include <iostream>
#include <list>

namespace unisim {
namespace util {
namespace debug {
namespace elf_symtab {

template <class MEMORY_ADDR, class Elf_Sym>
class ELF_SymtabHandler
{
public:
	ELF_SymtabHandler(std::ostream& debug_info_stream, std::ostream& debug_warning_stream, std::ostream& debug_error_stream, const unisim::util::blob::Blob<MEMORY_ADDR> *blob);
	~ELF_SymtabHandler();

	void Parse();

	void GetSymbols(typename std::list<const unisim::util::debug::Symbol<MEMORY_ADDR> *>& lst, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
private:
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	const unisim::util::blob::Blob<MEMORY_ADDR> *blob;
	unisim::util::debug::SymbolTable<MEMORY_ADDR> *symbol_table;
};

} // end of namespace elf_symtab
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
