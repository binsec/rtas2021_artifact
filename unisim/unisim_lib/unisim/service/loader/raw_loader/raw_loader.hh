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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_LOADER_RAW_LOADER_RAW_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_RAW_LOADER_RAW_LOADER_HH__

#include <inttypes.h>
#include <string>
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/kernel.hh"
#include <unisim/kernel/variable/variable.hh>
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/service/interfaces/blob.hh"
#include "unisim/util/blob/blob.hh"

namespace unisim {
namespace service {
namespace loader {
namespace raw_loader {

template <class MEMORY_ADDR>
class RawLoader
	: public unisim::kernel::Service<
		unisim::service::interfaces::Loader>
	, public unisim::kernel::Service<
		unisim::service::interfaces::Blob<MEMORY_ADDR> >
	, public unisim::kernel::Client<
	  	unisim::service::interfaces::Memory<MEMORY_ADDR> >
{
public:
	unisim::kernel::ServiceExport<
		unisim::service::interfaces::Loader> loader_export;
	unisim::kernel::ServiceExport<
		unisim::service::interfaces::Blob<MEMORY_ADDR> > blob_export;
	unisim::kernel::ServiceImport<
		unisim::service::interfaces::Memory<MEMORY_ADDR> > memory_import;

	RawLoader(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~RawLoader();

	virtual bool BeginSetup();
	virtual bool Setup(unisim::kernel::ServiceExportBase *srv_export);
	virtual bool EndSetup();
	
	virtual bool Load();
	virtual const unisim::util::blob::Blob<MEMORY_ADDR> *GetBlob() const;

private:
	bool SetupBlob();
	bool SetupLoad();

	unisim::util::blob::Blob<MEMORY_ADDR> *blob;
	std::string filename;
	MEMORY_ADDR base_addr;
	MEMORY_ADDR size;
	bool verbose;
	
	unisim::kernel::variable::Parameter<std::string> param_filename;
	unisim::kernel::variable::Parameter<MEMORY_ADDR> param_base_addr;
	unisim::kernel::variable::Parameter<MEMORY_ADDR> param_size;
	unisim::kernel::variable::Parameter<bool> param_verbose;

	unisim::kernel::logger::Logger logger;
};

typedef RawLoader<uint32_t> RawLoader32;
typedef RawLoader<uint64_t> RawLoader64;

} // end of namespace raw_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_LOADER_RAW_LOADER_RAW_LOADER_HH__
