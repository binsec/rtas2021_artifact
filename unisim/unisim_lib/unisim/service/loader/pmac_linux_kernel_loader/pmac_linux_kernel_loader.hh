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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_LOADER_PMAC_LINUX_KERNEL_LOADER_PMAC_LINUX_KERNEL_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_PMAC_LINUX_KERNEL_LOADER_PMAC_LINUX_KERNEL_LOADER_HH__

#include <unisim/service/loader/pmac_bootx/pmac_bootx.hh>
#include <unisim/service/loader/elf_loader/elf32_loader.hh>

#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/blob.hh>

#include <unisim/kernel/kernel.hh>

namespace unisim {
namespace service {
namespace loader {
namespace pmac_linux_kernel_loader {
	
using unisim::service::loader::elf_loader::Elf32Loader;
using unisim::service::loader::pmac_bootx::PMACBootX;
using unisim::kernel::Object;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;

using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::StatementLookup;
using unisim::service::interfaces::Blob;

class PMACLinuxKernelLoader : public Object
{
public:
	ServiceExport<Loader> loader_export;
	ServiceExport<Blob<uint32_t> > blob_export;
	ServiceExport<SymbolTableLookup<uint32_t> > symbol_table_lookup_export;
	ServiceExport<StatementLookup<uint32_t> > stmt_lookup_export;

	ServiceImport<Memory<uint32_t> > memory_import;
	ServiceImport<Registers> registers_import;
	
	PMACLinuxKernelLoader(const char *name, Object *parent = 0);
	virtual ~PMACLinuxKernelLoader();
private:
	PMACBootX pmac_bootx;
	Elf32Loader<uint32_t> elf32_loader;
};

} // end of namespace pmac_linux_kernel_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
