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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_HH__

#include "unisim/service/interfaces/memory.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/service/interfaces/blob.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"

#include <string>
#include <vector>

namespace unisim {
namespace service {
namespace loader {
namespace linux_loader {

using std::string;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Blob;
using namespace unisim::util::endian;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::Object;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::ParameterArray;
using unisim::kernel::logger::Logger;

template<class T>
class LinuxLoader
  : public Client<Loader>
  , public Client<Blob<T> >
  , public Client<Memory<T> >
  , public Service<Loader>
  , public Service<Blob<T> >
{
public:
	/* Import of the different services */
	ServiceImport<Loader> loader_import;
	ServiceImport<Blob<T> > blob_import;
	ServiceImport<Memory<T> > memory_import;
	/* Exported services */
	ServiceExport<Loader> loader_export;
	ServiceExport<Blob<T> > blob_export;

	/* Constructor/Destructor */
	LinuxLoader(const char *name, Object *parent = 0);
	virtual ~LinuxLoader();

	/* Service methods */
	virtual void OnDisconnect();
	virtual bool BeginSetup();
	virtual bool Setup(ServiceExportBase *srv_export);
	virtual bool EndSetup();

	/* Service interface methods */
	virtual bool Load();
	virtual const unisim::util::blob::Blob<T> *GetBlob();

protected:
	endian_type endianness;
	T stack_base;
	T stack_size;
	T max_environ;
	T memory_page_size;
	unsigned int argc;
	std::vector<std::string *> argv;
	bool apply_host_environ;
	unsigned int envc;
	std::vector<std::string *> envp;
	std::vector<std::string *> target_envp;
	unisim::util::blob::Blob<T> *blob;
	unisim::util::blob::Blob<T> *stack_blob;
	
	//T stack_address;
	//T arg_address;

	bool verbose;

private:
	bool LoadStack();
	bool SetupLoad();
	bool SetupBlob();
	void DumpBlob(unisim::util::blob::Blob<T> const &, int);
	void DumpSection(unisim::util::blob::Section<T> const &, int);
	void DumpSegment(unisim::util::blob::Segment<T> const &, int);
	
	static const int arch_size = sizeof(uint32_t); // this works for 32 bits which is the case of arm and powerpc

	string endianness_string;
	Parameter<string> param_endian;
	Parameter<T> param_stack_base;
	Parameter<T> param_stack_size;
	Parameter<T> param_max_environ;
	struct ArgsAndEnvsParam : public Parameter<unsigned int>
	{
		ArgsAndEnvsParam(char const* name, LinuxLoader* _linux_loader, unsigned int& var, const char *description)
		  : Parameter<unsigned int>(name, _linux_loader, var, description), linux_loader(*_linux_loader)
		{}
		void Set( unsigned int const& value ) {
			Parameter<unsigned int>::Set( value );
			linux_loader.SetupArgsAndEnvs();
		}
		LinuxLoader& linux_loader;
	};
	
	ArgsAndEnvsParam param_argc;
	std::vector<Parameter<string> *> param_argv;
	Parameter<bool> param_apply_host_environ;
	ArgsAndEnvsParam param_envc;
	std::vector<Parameter<string> *> param_envp;
	Parameter<T> param_memory_page_size;

	Parameter<bool> param_verbose;
	Logger logger;

	void Log(T addr, const uint8_t *value, uint32_t size);

	void SetupArgsAndEnvs();

	// auxiliary table symbols
	static const T AT_NULL = 0;
	static const T AT_IGNORE = 1;     /* entry should be ignored */
	static const T AT_EXECFD = 2;     /* file descriptor of program */
	static const T AT_PHDR = 3;     /* program headers for program */
	static const T AT_PHENT = 4;     /* size of program header entry */
	static const T AT_PHNUM = 5;     /* number of program headers */
	static const T AT_PAGESZ = 6;     /* system page size */
	static const T AT_BASE = 7;     /* base address of interpreter */
	static const T AT_FLAGS = 8;     /* flags */
	static const T AT_ENTRY = 9;     /* entry point of program */
	static const T AT_NOTELF = 10;    /* program is not ELF */
	static const T AT_UID = 11;    /* real uid */
	static const T AT_EUID = 12;    /* effective uid */
	static const T AT_GID = 13;    /* real gid */
	static const T AT_EGID = 14;    /* effective gid */
	static const T AT_PLATFORM = 15;  /* string identifying CPU for optimizations */
	static const T AT_HWCAP = 16;    /* arch dependent hints at CPU capabilities */
	static const T AT_CLKTCK = 17;    /* frequency at which times() increments */
	
	T SetAuxTableEntry(uint8_t * _stack_data, T _sp, T const & _entry, const T &_value)
	{
		_sp = _sp - sizeof(T);
		uint8_t *addr = _stack_data + _sp;
		memcpy(addr, &_value, sizeof(T));
		_sp = _sp - sizeof(T);
		addr = _stack_data + _sp;
		memcpy(addr, &_entry, sizeof(T));
		return _sp;
	};

	static const T ARM_HWCAP_ARM_SWP       = 1 << 0;
	static const T ARM_HWCAP_ARM_HALF      = 1 << 1;
	static const T ARM_HWCAP_ARM_THUMB     = 1 << 2;
	static const T ARM_HWCAP_ARM_26BIT     = 1 << 3;
	static const T ARM_HWCAP_ARM_FAST_MULT = 1 << 4;
	static const T ARM_HWCAP_ARM_FPA       = 1 << 5;
	static const T ARM_HWCAP_ARM_VFP       = 1 << 6;
	static const T ARM_HWCAP_ARM_EDSP      = 1 << 7;
	static const T ARM_HWCAP_ARM_JAVA      = 1 << 8;
	static const T ARM_HWCAP_ARM_IWMMXT    = 1 << 9;
	static const T ARM_HWCAP_ARM_THUMBEE   = 1 << 10;
	static const T ARM_HWCAP_ARM_NEON      = 1 << 11;
	static const T ARM_HWCAP_ARM_VFPv3     = 1 << 12;
	static const T ARM_HWCAP_ARM_VFPv3D16  = 1 << 13;

	static const T ARM_ELF_HWCAP = ARM_HWCAP_ARM_SWP 
								   | ARM_HWCAP_ARM_HALF
		   						   | ARM_HWCAP_ARM_THUMB
		   						   | ARM_HWCAP_ARM_FAST_MULT
								   | ARM_HWCAP_ARM_FPA
								   | ARM_HWCAP_ARM_VFP
								   | ARM_HWCAP_ARM_NEON
								   | ARM_HWCAP_ARM_VFPv3;

};

} // end of linux_loader
} // end of loader namespace
} // end of service namespace
} // end of unisim namespace

#endif // __UNISIM_SERVICE_LOADER_LINUX_LOADER_LINUX_LOADER_HH__
