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

#ifndef __UNISIM_SERVICE_LOADER_PPC_LINUX_KERNEL_LOADER_PPC_LINUX_KERNEL_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_PPC_LINUX_KERNEL_LOADER_PPC_LINUX_KERNEL_LOADER_HH__

#include <unisim/service/loader/elf_loader/elf32_loader.hh>
#include <unisim/service/loader/raw_loader/raw_loader.hh>
#include <unisim/service/loader/ppc_uboot/ppc_uboot.hh>

namespace unisim {
namespace service {
namespace loader {
namespace ppc_linux_kernel_loader {

using unisim::kernel::Object;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Blob;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::StatementLookup;
using unisim::service::loader::elf_loader::Elf32Loader;
using unisim::service::loader::raw_loader::RawLoader;
using unisim::service::loader::ppc_uboot::PPCUBoot;

template <class MEMORY_ADDR>
class PPCLinuxKernelLoader
	: public Object
{
public:
	PPCLinuxKernelLoader(const char *name, Object *parent = 0);
	virtual ~PPCLinuxKernelLoader();
	
	ServiceExport<Loader> loader_export;
	ServiceExport<Blob<MEMORY_ADDR> > blob_export;
	ServiceExport<SymbolTableLookup<MEMORY_ADDR> > symbol_table_lookup_export;
	ServiceExport<StatementLookup<MEMORY_ADDR> > stmt_lookup_export;
	ServiceImport<Registers> registers_import;
	ServiceImport<Memory<MEMORY_ADDR> > memory_import;
private:
	Elf32Loader<MEMORY_ADDR> kernel_loader;
	RawLoader<MEMORY_ADDR> device_tree_loader;
	RawLoader<MEMORY_ADDR> initrd_loader;
	PPCUBoot<MEMORY_ADDR> uboot;
};

} // end of namespace powerpc_linux_kernel_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
