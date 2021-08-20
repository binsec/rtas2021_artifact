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

#ifndef __UNISIM_SERVICE_LOADER_PPC_UBOOT_PPC_UBOOT_HH__
#define __UNISIM_SERVICE_LOADER_PPC_UBOOT_PPC_UBOOT_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/util/blob/blob.hh>

namespace unisim {
namespace service {
namespace loader {
namespace ppc_uboot {

using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::Object;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::variable::Parameter;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Blob;

template <class MEMORY_ADDR>
class PPCUBoot
	: public Service<Loader>
	, public Service<Blob<MEMORY_ADDR> >
	, public Client<Registers>
	, public Client<Memory<MEMORY_ADDR> >
	, public Client<Loader>
	, public Client<Blob<MEMORY_ADDR> >
{
public:
	PPCUBoot(const char *name, Object *parent = 0);
	virtual ~PPCUBoot();
	
	ServiceExport<Loader> loader_export;
	ServiceExport<Blob<MEMORY_ADDR> > blob_export;
	ServiceImport<Registers> registers_import;
	ServiceImport<Memory<MEMORY_ADDR> > memory_import;
	ServiceImport<Loader> kernel_loader_import;
	ServiceImport<Blob<MEMORY_ADDR> > kernel_blob_import;
	ServiceImport<Loader> device_tree_loader_import;
	ServiceImport<Blob<MEMORY_ADDR> > device_tree_blob_import;
	ServiceImport<Loader> initrd_loader_import;
	ServiceImport<Blob<MEMORY_ADDR> > initrd_blob_import;
	
	virtual const unisim::util::blob::Blob<MEMORY_ADDR> *GetBlob() const;
	virtual bool Load();

	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();
private:
	bool SetupBlob();
	bool SetupLoad();
	bool LoadKernelCmdLine();
	bool LoadRegisters();

	unisim::util::blob::Blob<MEMORY_ADDR> *blob;
	const unisim::util::blob::Blob<MEMORY_ADDR> *kernel_blob;
	const unisim::util::blob::Blob<MEMORY_ADDR> *device_tree_blob;
	const unisim::util::blob::Blob<MEMORY_ADDR> *initrd_blob;
	unisim::util::blob::Blob<MEMORY_ADDR> *kernel_cmd_line_blob;
	unisim::service::interfaces::Register *ppc_cia;
	unisim::service::interfaces::Register *ppc_r3;
	unisim::service::interfaces::Register *ppc_r4;
	unisim::service::interfaces::Register *ppc_r5;
	unisim::service::interfaces::Register *ppc_r6;
	unisim::service::interfaces::Register *ppc_r7;
	std::string kernel_cmd_line;
	MEMORY_ADDR kernel_cmd_line_addr;
	bool verbose;
	
	Parameter<std::string> param_kernel_cmd_line;
	Parameter<MEMORY_ADDR> param_kernel_cmd_line_addr;
	Parameter<bool> param_verbose;

	unisim::kernel::logger::Logger logger;
};

} // end of namespace ppc_uboot
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
