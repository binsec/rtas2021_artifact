/*
 *  Copyright (c) 2007,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr),
 * 		Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_SERVICE_TEE_SYMBOL_TABLE_LOOKUP_TEE_TCC__
#define __UNISIM_SERVICE_TEE_SYMBOL_TABLE_LOOKUP_TEE_TCC__

#include <string>
#include <sstream>

namespace unisim {
namespace service {
namespace tee {
namespace symbol_table_lookup {

using std::stringstream;
using std::string;

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::Tee(const char *name, Object *parent) :
	Object(name, parent, "This service/client implements a tee ('T'). It unifies the symbol table lookup capability of several services that individually provides their own symbol table lookup capability" ),
	Client<SymbolTableLookup<ADDRESS> >(name, parent),
	Service<SymbolTableLookup<ADDRESS> >(name, parent),
	symbol_table_lookup_export("symbol-table-lookup-export", this)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		stringstream sstr;
		sstr << "symbol-table-lookup-import[" << i << "]";
		string import_name = sstr.str();
		symbol_table_lookup_import[i] = new ServiceImport<SymbolTableLookup<ADDRESS> >(import_name.c_str(), this);
		symbol_table_lookup_export.SetupDependsOn(*symbol_table_lookup_import[i]);
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::~Tee()
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(symbol_table_lookup_import[i]) delete symbol_table_lookup_import[i];
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
void Tee<ADDRESS, MAX_IMPORTS>::GetSymbols(typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(symbol_table_lookup_import[i])
		{
			if(*symbol_table_lookup_import[i])
			{
				(*symbol_table_lookup_import[i])->GetSymbols(lst, type);
			}
		}
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const typename unisim::util::debug::Symbol<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::FindSymbol(const char *name, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(symbol_table_lookup_import[i])
		{
			if(*symbol_table_lookup_import[i])
			{
				const typename unisim::util::debug::Symbol<ADDRESS> *symbol = (*symbol_table_lookup_import[i])->FindSymbol(name, addr, type);
				if(symbol) return symbol;
			}
		}
	}
	return 0;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const typename unisim::util::debug::Symbol<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::FindSymbolByAddr(ADDRESS addr) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(symbol_table_lookup_import[i])
		{
			if(*symbol_table_lookup_import[i])
			{
				const typename unisim::util::debug::Symbol<ADDRESS> *symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr);
				if(symbol) return symbol;
			}
		}
	}
	return 0;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const typename unisim::util::debug::Symbol<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::FindSymbolByName(const char *name) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(symbol_table_lookup_import[i])
		{
			if(*symbol_table_lookup_import[i])
			{
				const typename unisim::util::debug::Symbol<ADDRESS> *symbol = (*symbol_table_lookup_import[i])->FindSymbolByName(name);
				if(symbol) return symbol;
			}
		}
	}
	return 0;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const typename unisim::util::debug::Symbol<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(symbol_table_lookup_import[i])
		{
			if(*symbol_table_lookup_import[i])
			{
				const typename unisim::util::debug::Symbol<ADDRESS> *symbol = (*symbol_table_lookup_import[i])->FindSymbolByName(name, type);
				if(symbol) return symbol;
			}
		}
	}
	return 0;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const typename unisim::util::debug::Symbol<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::FindSymbolByAddr(ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(symbol_table_lookup_import[i])
		{
			if(*symbol_table_lookup_import[i])
			{
				const typename unisim::util::debug::Symbol<ADDRESS> *symbol = (*symbol_table_lookup_import[i])->FindSymbolByAddr(addr, type);
				if(symbol) return symbol;
			}
		}
	}
	return 0;
}


} // end of namespace memory_access_reporting
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_MEMORY_ACCESS_REPORTING_TEE_TCC__
