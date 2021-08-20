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
 
#include <unisim/service/loader/pmac_linux_kernel_loader/pmac_linux_kernel_loader.hh>

namespace unisim {
namespace service {
namespace loader {
namespace pmac_linux_kernel_loader {

PMACLinuxKernelLoader::PMACLinuxKernelLoader(const char *name, Object *parent) :
	Object(name, parent, "PowerMac Linux kernel loader"),
	loader_export("loader-export", this),
	blob_export("blob-export", this),
	symbol_table_lookup_export("symbol-table-lookup-export", this),
	stmt_lookup_export("stmt-lookup-export", this),
	memory_import("memory-import", this),
	registers_import("registers-import", this),
	pmac_bootx("pmac-bootx", this),
	elf32_loader("elf32-loader", this)
{
	pmac_bootx.loader_import >> elf32_loader.loader_export;
	pmac_bootx.blob_import >> elf32_loader.blob_export;
	pmac_bootx.memory_import >> memory_import;
	pmac_bootx.registers_import >> registers_import;
	elf32_loader.memory_import >> memory_import;
	loader_export >> pmac_bootx.loader_export;
	symbol_table_lookup_export >> elf32_loader.symbol_table_lookup_export;
	stmt_lookup_export >> elf32_loader.stmt_lookup_export;
	blob_export >> pmac_bootx.blob_export;
}

PMACLinuxKernelLoader::~PMACLinuxKernelLoader()
{
}

} // end of namespace pmac_linux_kernel_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim
