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

#ifndef __UNISIM_SERVICE_LOADER_PPC_UBOOT_PPC_UBOOT_TCC__
#define __UNISIM_SERVICE_LOADER_PPC_UBOOT_PPC_UBOOT_TCC__

namespace unisim {
namespace service {
namespace loader {
namespace ppc_uboot {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

template <class MEMORY_ADDR>
PPCUBoot<MEMORY_ADDR>::PPCUBoot(const char *name, Object *parent)
	: Object(name, parent)
	, Service<Loader>(name, parent)
	, Service<Blob<MEMORY_ADDR> >(name, parent)
	, Client<Registers>(name, parent)
	, Client<Memory<MEMORY_ADDR> >(name, parent)
	, Client<Loader>(name, parent)
	, Client<Blob<MEMORY_ADDR> >(name, parent)
	, loader_export("loader-export", this)
	, blob_export("blob-export", this)
	, registers_import("registers-import", this)
	, memory_import("memory-import", this)
	, kernel_loader_import("kernel-loader-import", this)
	, kernel_blob_import("kernel-blob-import", this)
	, device_tree_loader_import("device-tree-loader-import", this)
	, device_tree_blob_import("device-tree-blob-import", this)
	, initrd_loader_import("initrd-loader-import", this)
	, initrd_blob_import("initrd-blob-import", this)
	, blob(0)
	, kernel_blob(0)
	, device_tree_blob(0)
	, initrd_blob(0)
	, kernel_cmd_line_blob(0)
	, ppc_cia(0)
	, ppc_r3(0)
	, ppc_r4(0)
	, ppc_r5(0)
	, ppc_r6(0)
	, ppc_r7(0)
	, kernel_cmd_line("")
	, kernel_cmd_line_addr(0x0)
	, verbose(false)
	, param_kernel_cmd_line("kernel-cmd-line", this, kernel_cmd_line, "Linux kernel command line (Note: Linux, at least 2.6.31, does not use this value. Instead you should fill in device tree: choosen / bootargs = "";)")
	, param_kernel_cmd_line_addr("kernel-cmd-line-addr", this, kernel_cmd_line_addr, "Address location of Linux kernel command line")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, logger(*this)
{
	loader_export.SetupDependsOn(kernel_loader_import);
	loader_export.SetupDependsOn(device_tree_loader_import);
	loader_export.SetupDependsOn(initrd_loader_import);
	loader_export.SetupDependsOn(memory_import);
	loader_export.SetupDependsOn(registers_import);
	loader_export.SetupDependsOn(kernel_blob_import);
	loader_export.SetupDependsOn(device_tree_blob_import);
	loader_export.SetupDependsOn(initrd_blob_import);
	
	blob_export.SetupDependsOn(kernel_blob_import);
	blob_export.SetupDependsOn(device_tree_blob_import);
	blob_export.SetupDependsOn(initrd_blob_import);
}

template <class MEMORY_ADDR>
PPCUBoot<MEMORY_ADDR>::~PPCUBoot()
{
	if(blob)
	{
		blob->Release();
	}
	
	if(kernel_blob)
	{
		kernel_blob->Release();
	}
	
	if(device_tree_blob)
	{
		device_tree_blob->Release();
	}
	
	if(initrd_blob)
	{
		initrd_blob->Release();
	}
	
	if(kernel_cmd_line_blob)
	{
		kernel_cmd_line_blob->Release();
	}
}

template <class MEMORY_ADDR>
bool PPCUBoot<MEMORY_ADDR>::BeginSetup()
{
	if(blob)
	{
		blob->Release();
		blob = 0;
	}

	if(kernel_blob)
	{
		kernel_blob->Release();
		kernel_blob = 0;
	}
	
	if(device_tree_blob)
	{
		device_tree_blob->Release();
		device_tree_blob = 0;
	}
	
	if(initrd_blob)
	{
		initrd_blob->Release();
		initrd_blob = 0;
	}
	
	if(kernel_cmd_line_blob)
	{
		kernel_cmd_line_blob->Release();
		kernel_cmd_line_blob = 0;
	}
	
	return true;
}

template <class MEMORY_ADDR>
bool PPCUBoot<MEMORY_ADDR>::SetupBlob()
{
	if(blob) return true;
	
	blob = new unisim::util::blob::Blob<MEMORY_ADDR>();
	blob->Catch();
	
	if(kernel_blob_import && !kernel_blob)
	{
		kernel_blob = kernel_blob_import->GetBlob();
		if(!kernel_blob) return false;
		kernel_blob->Catch();
		blob->AddBlob(kernel_blob);
	}

	if(device_tree_blob_import && !device_tree_blob)
	{
		device_tree_blob = device_tree_blob_import->GetBlob();
		if(!device_tree_blob) return false;
		device_tree_blob->Catch();
		blob->AddBlob(device_tree_blob);
	}
	
	if(initrd_blob_import && !initrd_blob)
	{
		initrd_blob = initrd_blob_import->GetBlob();
		if(!initrd_blob) return false;
		initrd_blob->Catch();
		blob->AddBlob(initrd_blob);
	}
	
	if(!kernel_cmd_line_blob)
	{
		kernel_cmd_line_blob = new unisim::util::blob::Blob<MEMORY_ADDR>();
		kernel_cmd_line_blob->Catch();

		char *kernel_cmd_line_data = strdup(kernel_cmd_line.c_str());
		unsigned int kernel_cmd_line_size = strlen(kernel_cmd_line_data);
		
		unisim::util::blob::Section<MEMORY_ADDR> *kernel_cmd_line_section = new unisim::util::blob::Section<MEMORY_ADDR>(
			unisim::util::blob::Section<MEMORY_ADDR>::TY_UNKNOWN,
			unisim::util::blob::Section<MEMORY_ADDR>::SA_A,
			"",
			0,
			0,
			kernel_cmd_line_addr,
			kernel_cmd_line_size,
			kernel_cmd_line_data
		);

		kernel_cmd_line_blob->AddSection(kernel_cmd_line_section);
		
		blob->AddBlob(kernel_cmd_line_blob);
	}

	return true;
}

template <class MEMORY_ADDR>
bool PPCUBoot<MEMORY_ADDR>::SetupLoad()
{
	if(!SetupBlob()) return false;
	if(!registers_import) return false;
	
	ppc_cia = registers_import->GetRegister("cia");
	ppc_r3 = registers_import->GetRegister("r3");
	ppc_r4 = registers_import->GetRegister("r4");
	ppc_r5 = registers_import->GetRegister("r5");
	ppc_r6 = registers_import->GetRegister("r6");
	ppc_r7 = registers_import->GetRegister("r7");
	if(!ppc_cia || !ppc_r3 || !ppc_r4 || !ppc_r5 || !ppc_r6 || !ppc_r7) return false;
	if(ppc_cia->GetSize() != sizeof(MEMORY_ADDR)) return false;
	if(ppc_r3->GetSize() != sizeof(MEMORY_ADDR)) return false;
	if(ppc_r4->GetSize() != sizeof(MEMORY_ADDR)) return false;
	if(ppc_r5->GetSize() != sizeof(MEMORY_ADDR)) return false;
	if(ppc_r6->GetSize() != sizeof(MEMORY_ADDR)) return false;
	if(ppc_r7->GetSize() != sizeof(MEMORY_ADDR)) return false;

	if(!kernel_blob || !(kernel_blob->GetCapability() & unisim::util::blob::Blob<MEMORY_ADDR>::CAP_ENTRY_POINT)) return false;
	return true;
}

template <class MEMORY_ADDR>
bool PPCUBoot<MEMORY_ADDR>::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &loader_export) return SetupLoad();
	if(srv_export == &blob_export) return SetupBlob();
	
	logger << DebugError << "Internal Error" << EndDebugError;
	return false;
}

template <class MEMORY_ADDR>
bool PPCUBoot<MEMORY_ADDR>::EndSetup()
{
	return LoadKernelCmdLine() && LoadRegisters();
}

template <class MEMORY_ADDR>
bool PPCUBoot<MEMORY_ADDR>::LoadKernelCmdLine()
{
	if(memory_import)
	{
		if(!kernel_cmd_line_blob) return false;
		const unisim::util::blob::Section<MEMORY_ADDR> *kernel_cmd_line_section = kernel_cmd_line_blob->GetSection(0);
		
		MEMORY_ADDR kernel_cmd_line_start;
		MEMORY_ADDR kernel_cmd_line_end;

		kernel_cmd_line_blob->GetAddrRange(kernel_cmd_line_start, kernel_cmd_line_end);

		if(verbose)
		{
			logger << DebugInfo << "Writing Linux kernel command line at @0x" << std::hex << kernel_cmd_line_start << " - @0x" << kernel_cmd_line_end << std::dec << EndDebugInfo;
		}
		
		if(!memory_import->WriteMemory(kernel_cmd_line_section->GetAddr(), kernel_cmd_line_section->GetData(), kernel_cmd_line_section->GetSize())) return false;

		ppc_r6->SetValue(&kernel_cmd_line_start);
		ppc_r7->SetValue(&kernel_cmd_line_end);
	}
	
	return true;
}

template <class MEMORY_ADDR>
bool PPCUBoot<MEMORY_ADDR>::LoadRegisters()
{
	if(!kernel_blob) return false;
	//MEMORY_ADDR kernel_entry_point = kernel_blob->GetEntryPoint();
	if(!ppc_cia) return false;
	//ppc_cia->SetValue(&kernel_entry_point);

	MEMORY_ADDR kernel_start;
	MEMORY_ADDR kernel_end;
	kernel_blob->GetAddrRange(kernel_start, kernel_end);
	
	if(!device_tree_blob) return false;
	MEMORY_ADDR device_tree_start;
	MEMORY_ADDR device_tree_end;
		
	device_tree_blob->GetAddrRange(device_tree_start, device_tree_end);
	MEMORY_ADDR device_tree_start_offset = device_tree_start - kernel_start;
	if(!ppc_r3) return false;
	ppc_r3->SetValue(&device_tree_start_offset);

	if(!initrd_blob) return false;
	MEMORY_ADDR initrd_start;
	MEMORY_ADDR initrd_end;

	initrd_blob->GetAddrRange(initrd_start, initrd_end);
	initrd_end++; // For Linux, the end is not the last byte, but the byte after
	MEMORY_ADDR initrd_start_offset = initrd_start - kernel_start;
	MEMORY_ADDR initrd_end_offset = initrd_end - kernel_start;

	if(!ppc_r4 || !ppc_r5) return false;
	ppc_r4->SetValue(&initrd_start_offset); // Note: Linux (at least 2.6.31) do not use this value. Instead you should fill in device tree: choosen -> linux,initrd-start = <@>;
	ppc_r5->SetValue(&initrd_end_offset); // Note: Linux (at least 2.6.31) do not use this value. Instead you should fill in device tree: choosen -> linux,initrd-end = <@>;

	if(!kernel_cmd_line_blob) return false;
	const unisim::util::blob::Section<MEMORY_ADDR> *kernel_cmd_line_section = kernel_cmd_line_blob->GetSection(0);
	if(!kernel_cmd_line_section) return false;
		
	MEMORY_ADDR kernel_cmd_line_start;
	MEMORY_ADDR kernel_cmd_line_end;

	kernel_cmd_line_blob->GetAddrRange(kernel_cmd_line_start, kernel_cmd_line_end);
	kernel_cmd_line_end++; // For Linux, the end is not the last byte, but the byte after
	MEMORY_ADDR kernel_cmd_line_start_offset = kernel_cmd_line_start - kernel_start;
	MEMORY_ADDR kernel_cmd_line_end_offset = kernel_cmd_line_end - kernel_start;
	
	if(!ppc_r6 || !ppc_r7) return false;
	ppc_r6->SetValue(&kernel_cmd_line_start_offset); // Note: Linux (at least 2.6.31) do not use this value. Instead you should fill in device tree: choosen / bootargs = "";
	ppc_r7->SetValue(&kernel_cmd_line_end_offset); // Note: Linux (at least 2.6.31) do not use this value. Instead you should fill in device tree: choosen / bootargs = "";
	
	return true;
}

template <class MEMORY_ADDR>
bool PPCUBoot<MEMORY_ADDR>::Load()
{
	if(kernel_loader_import)
	{
		if(!kernel_loader_import->Load()) return false;
	}

	if(device_tree_loader_import)
	{
		if(!device_tree_loader_import->Load()) return false;
	}

	if(initrd_loader_import)
	{
		if(!initrd_loader_import->Load()) return false;
	}

	if(!LoadKernelCmdLine()) return false;
	if(!LoadRegisters()) return false;
	return true;
}

template <class MEMORY_ADDR>
const typename unisim::util::blob::Blob<MEMORY_ADDR> *PPCUBoot<MEMORY_ADDR>::GetBlob() const
{
	return blob;
}

} // end of namespace ppc_uboot
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
