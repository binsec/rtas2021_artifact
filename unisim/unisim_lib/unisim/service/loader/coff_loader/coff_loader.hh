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
 
#ifndef __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_HH__

#include <unisim/util/loader/coff_loader/coff_loader.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

namespace unisim {
namespace service {
namespace loader {
namespace coff_loader {

// using namespace std;
using unisim::service::interfaces::Memory;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::Object;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::variable::Parameter;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Blob;

template <class MEMORY_ADDR>
class CoffLoader :
	public Client<Memory<MEMORY_ADDR> >,
	public Service<Loader>,
	public Service<SymbolTableLookup<MEMORY_ADDR> >,
	public Service<Blob<MEMORY_ADDR> >
{
public:
	ServiceImport<Memory<MEMORY_ADDR> > memory_import;
	ServiceExport<Loader> loader_export;
	ServiceExport<Blob<MEMORY_ADDR> > blob_export;
	ServiceExport<SymbolTableLookup<MEMORY_ADDR> > symbol_table_lookup_export;

	CoffLoader(const char *name, Object *parent = 0);
	virtual ~CoffLoader();

	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual bool Load();
	virtual const unisim::util::blob::Blob<MEMORY_ADDR> *GetBlob() const;

	virtual void GetSymbols(typename std::list<const unisim::util::debug::Symbol<MEMORY_ADDR> *>& lst, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;

private:
	unisim::util::loader::coff_loader::CoffLoader<MEMORY_ADDR> *coff_loader;
	
	// Run-time parameters
	std::string filename;
	bool dump_headers;
	bool verbose;

	// Run-time parameters (accessors)
	Parameter<std::string> param_filename;
	Parameter<bool> param_dump_headers;
	Parameter<bool> param_verbose;

	unisim::kernel::logger::Logger logger;
};

} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
