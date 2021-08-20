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
 
#ifndef __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_TCC__

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace service {
namespace loader {
namespace coff_loader {

using std::stringstream;
using namespace unisim::kernel::logger;

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::CoffLoader(const char *name, Object *parent)
	: Object(name, parent, "COFF loader")
	, Client<Memory<MEMORY_ADDR> >(name, parent)
	, Service<Loader>(name, parent)
	, Service<SymbolTableLookup<MEMORY_ADDR> >(name, parent)
	, Service<Blob<MEMORY_ADDR> >(name, parent)
	, memory_import("memory-import", this)
	, loader_export("loader-export", this)
	, blob_export("blob-export", this)
	, symbol_table_lookup_export("symbol-table-lookup-export", this)
	, coff_loader(0)
	, filename()
	, dump_headers(false)
	, verbose(false)
	, param_filename("filename", this, filename, "the COFF filename to load")
	, param_dump_headers("dump-headers", this, dump_headers, "Enable/Disable dump of COFF file headers while loading")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, logger(*this)
{
	loader_export.SetupDependsOn(memory_import);
}

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::~CoffLoader()
{
	if(coff_loader)
	{
		delete coff_loader;
		coff_loader = 0;
	}
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::OnDisconnect()
{
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::BeginSetup()
{
	if(coff_loader)
	{
		delete coff_loader;
		coff_loader = 0;
	}

	coff_loader = new unisim::util::loader::coff_loader::CoffLoader<MEMORY_ADDR>(logger.DebugInfoStream(), logger.DebugWarningStream(), logger.DebugErrorStream());

	coff_loader->SetOption(unisim::util::loader::coff_loader::OPT_FILENAME, Object::GetSimulator()->SearchSharedDataFile(filename.c_str()).c_str());
	coff_loader->SetOption(unisim::util::loader::coff_loader::OPT_DUMP_HEADERS, dump_headers);
	
	return coff_loader->Load();
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &loader_export) return coff_loader != 0;
	if(srv_export == &blob_export) return coff_loader != 0;
	if(srv_export == &symbol_table_lookup_export) return coff_loader != 0;

	logger << DebugError << "Internal Error" << EndDebugError;
	return false;
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::EndSetup()
{
	if(memory_import)
	{
		return Load();
	}
	return true;
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::Load()
{
	if(!memory_import) return false;

	const unisim::util::blob::Blob<MEMORY_ADDR> *blob = coff_loader->GetBlob();
	if(!blob) return false;
	
	bool success = true;
	const typename std::vector<const unisim::util::blob::Segment<MEMORY_ADDR> *>& segments = blob->GetSegments();
	typename std::vector<const unisim::util::blob::Segment<MEMORY_ADDR> *>::const_iterator segment_iter;
	for(segment_iter = segments.begin(); segment_iter != segments.end(); segment_iter++)
	{
		const unisim::util::blob::Segment<MEMORY_ADDR> *segment = *segment_iter;
		
		if(segment->GetType() == unisim::util::blob::Segment<MEMORY_ADDR>::TY_LOADABLE)
		{
			MEMORY_ADDR write_size = segment->GetSize();
			if(write_size)
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Writing segment (" << write_size << " bytes) at @0x" << std::hex << segment->GetAddr() << " - @0x" << (segment->GetAddr() +  write_size - 1) << std::dec << EndDebugInfo;
				}

				if(!memory_import->WriteMemory(segment->GetAddr(), segment->GetData(), write_size))
				{
					logger << DebugError << "Can't write into memory (@0x" << std::hex << segment->GetAddr() << " - @0x" << (segment->GetAddr() +  write_size - 1) << std::dec << ")" << EndDebugError;
					success = false;
				}
			}
		}
	}
	return success;
}

template <class MEMORY_ADDR>
const unisim::util::blob::Blob<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::GetBlob() const
{
	return coff_loader ? coff_loader->GetBlob() : 0;
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::GetSymbols(typename std::list<const unisim::util::debug::Symbol<MEMORY_ADDR> *>& lst, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	if(coff_loader)
	{
		coff_loader->GetSymbols(lst, type);
	}
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return coff_loader ? coff_loader->FindSymbol(name, addr, type) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByAddr(MEMORY_ADDR addr) const
{
	return coff_loader ? coff_loader->FindSymbolByAddr(addr) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByName(const char *name) const
{
	return coff_loader ? coff_loader->FindSymbolByName(name) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return coff_loader ? coff_loader->FindSymbolByName(name, type) : 0;
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return coff_loader ? coff_loader->FindSymbolByAddr(addr, type) : 0;
}

} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
