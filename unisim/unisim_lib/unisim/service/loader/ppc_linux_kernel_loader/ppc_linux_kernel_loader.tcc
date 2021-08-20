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

#ifndef __UNISIM_SERVICE_LOADER_PPC_LINUX_KERNEL_LOADER_PPC_LINUX_KERNEL_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_PPC_LINUX_KERNEL_LOADER_PPC_LINUX_KERNEL_LOADER_TCC__

namespace unisim {
namespace service {
namespace loader {
namespace ppc_linux_kernel_loader {

template <class MEMORY_ADDR>
PPCLinuxKernelLoader<MEMORY_ADDR>::PPCLinuxKernelLoader(const char *name, Object *parent)
	: Object(name, parent)
	, loader_export("loader-export", this)
	, blob_export("blob-export", this)
	, symbol_table_lookup_export("symbol-table-lookup-export", this)
	, stmt_lookup_export("stmt-lookup-export", this)
	, registers_import("registers-import", this)
	, memory_import("memory-import", this)
	, kernel_loader("kernel-loader", this)
	, device_tree_loader("device-tree-loader", this)
	, initrd_loader("initrd-loader", this)
	, uboot("uboot", this)
{
	loader_export >> uboot.loader_export;
	blob_export >> uboot.blob_export;
	symbol_table_lookup_export >> kernel_loader.symbol_table_lookup_export;
	stmt_lookup_export >> kernel_loader.stmt_lookup_export;
	uboot.kernel_loader_import >> kernel_loader.loader_export;
	uboot.kernel_blob_import >> kernel_loader.blob_export;
	uboot.device_tree_loader_import >> device_tree_loader.loader_export;
	uboot.device_tree_blob_import >> device_tree_loader.blob_export;
	uboot.initrd_loader_import >> initrd_loader.loader_export;
	uboot.initrd_blob_import >> initrd_loader.blob_export;
	kernel_loader.memory_import >> memory_import;
	device_tree_loader.memory_import >> memory_import;
	initrd_loader.memory_import >> memory_import;
	uboot.memory_import >> memory_import;
	uboot.registers_import >> registers_import;
}

template <class MEMORY_ADDR>
PPCLinuxKernelLoader<MEMORY_ADDR>::~PPCLinuxKernelLoader()
{
}

} // end of namespace powerpc_linux_kernel_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
