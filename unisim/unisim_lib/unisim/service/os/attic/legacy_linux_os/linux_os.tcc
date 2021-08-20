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
 */
 
#ifndef __UNISIM_SERVICE_OS_LINUX_OS_LINUX_OS_TCC__
#define __UNISIM_SERVICE_OS_LINUX_OS_LINUX_OS_TCC__

#if !defined(linux) && !defined(__linux) && !defined(__linux__) && !defined(__APPLE_CC__) && !defined(_WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
#error "Unsupported host machine for Linux system call translation !"
#endif


#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <process.h>
#else
#include <sys/times.h>
#endif

#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/linux_os.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/os/linux_os/linux_os_exception.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/service/interfaces/register.hh"
#include "unisim/util/likely/likely.hh"

#define LOCATION 	" - location = " << __FUNCTION__ << ":unisim/service/os/linux_os/linux_os.tcc:" << __LINE__

namespace unisim {
namespace service {
namespace os {
namespace linux_os {

using std::runtime_error;
using std::string;
using std::stringstream;
using std::hex;
using std::dec;
using std::endl;
using std::flush;
using std::cout;
using std::cerr;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::variable::Parameter;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebug;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Registers;
using unisim::util::endian::endian_type;
using unisim::service::interfaces::Register;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::Host2Target;

/** Constructor. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LinuxOS(const char *name, Object *parent)
	: Object(name, parent)
	, Service<unisim::service::interfaces::LinuxOS>(name, parent)
	, Service<Loader>(name, parent)
	, Service<Blob<ADDRESS_TYPE> >(name, parent)
	, Client<Memory<ADDRESS_TYPE> >(name, parent)
	, Client<MemoryInjection<ADDRESS_TYPE> >(name, parent)
	, Client<Registers>(name, parent)
	, Client<Loader>(name, parent)
	, Client<Blob<ADDRESS_TYPE> >(name, parent)
	, linux_os_export("linux-os-export", this)
	, loader_export("loader-export", this)
	, blob_export("blob-export", this)
	, memory_import("memory-import", this)
	, memory_injection_import("memory-injection-import", this)
	, registers_import("registers-import", this)
	, loader_import("loader-import", this)
	, blob_import("blob-import", this)
	, blob(0)
	, syscall_name_map()
	, syscall_name_assoc_map()
	, syscall_impl_assoc_map()
	, current_syscall_id(0)
	, current_syscall_name()
	, system("")
	, param_system("system", this, system, "Emulated system architecture "
			"available values are \"arm\", \"arm-eabi\" and \"powerpc\"")
	, endianess(E_LITTLE_ENDIAN)
	, endianess_string("little-endian")
	, param_endian("endianness", this, endianess_string,
    		"The endianness of the binary loaded. Available values are: little-endian and big-endian.")
	, memory_page_size(4096)
	, param_memory_page_size("memory-page-size", this, memory_page_size)
	, utsname_sysname("Linux")
	, param_utsname_sysname("utsname-sysname", this, utsname_sysname,
			"The value that the uname system call should return. As this"
			" service is providing linux emulation supoort its value should"
			" be 'Linux', so you should not modify it.")
	, utsname_nodename("localhost")
	, param_utsname_nodename("utsname-nodename", this, utsname_nodename,
			"The network node hostname that the uname system call should"
			" return. Default value is localhost, but you could write whatever"
			" name you want.")
	, utsname_release("2.6.27.35")
	, param_utsname_release("utsname-release", this, utsname_release,
			"The kernel realese information that the uname system call should"
			" return. This should usually match the linux-kernel parameter.")
	, utsname_version("#UNISIM SMP Fri Mar 12 05:23:09 UTC 2010")
	, param_utsname_version("utsname-version", this, utsname_version,
			"The kernel version information that the uname system call should"
			" return.")
	, utsname_machine("armv5")
	, param_utsname_machine("utsname-machine", this, utsname_machine,
			"The machine information that the uname system call should"
			" return. This should be one of the supported architectures (the"
			" system parameter, that is, arm or powerpc) or a specific model "
			" derived from it (i.e., arm926ejs).")
	, utsname_domainname("localhost")
	, param_utsname_domainname("utsname-domainname", this, utsname_domainname,
			"The domain name information that the uname system call should"
			" return.")
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose)
	, mmap_base(0xd4000000)
	, mmap_brk_point(0)
	, brk_point(0)
	, ppc_cr(0)
	, osrelease_filename("/proc/sys/kernel/osrelease")
	, fake_osrelease_filename("osrelease")
{
	SetSyscallNameMap();

	blob_export.SetupDependsOn(blob_import);
	
	loader_export.SetupDependsOn(loader_import);
	loader_export.SetupDependsOn(blob_import);
	loader_export.SetupDependsOn(registers_import);
	loader_export.SetupDependsOn(memory_import);
	
	linux_os_export.SetupDependsOn(blob_import);
}

/** Destructor. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
~LinuxOS() 
{
	if(blob)
	{
		blob->Release();
	}
}

/** Method to execute when the LinuxOS is disconnected from its client. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
OnDisconnect() 
{
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
BeginSetup()
{
	if(blob)
	{
		blob->Release();
		blob = 0;
	}

	// check the endianess parameter
	if ( (endianess_string.compare("little-endian") != 0) &&
			(endianess_string.compare("big-endian") != 0) )
	{
		logger << DebugError
			<< "Unknown endian value. Correct values are:"
			<< " little-endian and big-endian"
			<< EndDebugError;
		return false;
	}
	else
	{
		if ( endianess_string.compare("little-endian") == 0 )
			endianess = E_LITTLE_ENDIAN;
		else
			endianess = E_BIG_ENDIAN;
	}
	
	// check that the given system is supported
	if (system != "arm" && system != "arm-eabi" && system != "powerpc") 
	{
		logger << DebugError
			<< "Unsupported system (" << system << "), this service only supports"
			<< " arm and ppc systems" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}

	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetupLoad()
{
	if (!memory_import) 
	{
		logger << DebugError
			<< memory_import.GetName() << " is not connected" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}
	
	if (!registers_import) 
	{
		logger << DebugError
			<< registers_import.GetName() << " is not connected" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}
	
	// Call the system dependent setup
	if ( (system == "arm") || (system == "arm-eabi") )
	{
		if ( !SetupLoadARM() ) 
		{
			logger << DebugError
				<< "Setup of the ARM configuration failed"
				<< EndDebugError;
			return false;
		}
	}
	if ( system == "powerpc" )
	{
		if ( !SetupLoadPPC() )
		{
			logger << DebugError
				<< "Setup of the PowerPC configuration failed"
				<< EndDebugError;
			return false;
		}
	}
	return true;
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetupLinuxOS()
{
	if ( !SetupBlob() ) 
	{
		logger << DebugError
			<< "Failed to setup the blob while setting up linux os."
			<< EndDebugError;
		return false;
	}
	if ( !blob ) 
	{
		logger << DebugError
			<< "blob not found while setting up linux os."
			<< EndDebugError;
		return false;
	}

	if (!memory_injection_import) 
	{
		logger << DebugError
			<< memory_injection_import.GetName() << " is not connected" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}

	if (!registers_import) 
	{
		logger << DebugError
			<< registers_import.GetName() << " is not connected" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}

	syscall_impl_assoc_map.clear();

	// Call the system dependent setup
	if ( (system == "arm") || (system == "arm-eabi") ) 
	{
		if ( !SetupLinuxOSARM() ) 
		{
			logger << DebugError
				<< "Error while trying to setup the linux os arm"
				<< EndDebugError;
			return false;
		}
	}
	if ( system == "powerpc" )
	{
		if ( !SetupLinuxOSPPC() ) 
		{
			logger << DebugError
				<< "Error while trying to setup the linux os powerpc"
				<< EndDebugError;
			return false;
		}
	}

	// Set mmap_brk_point and brk_point
	mmap_brk_point = mmap_base;

	ADDRESS_TYPE top_addr = blob->GetStackBase() + 1;
	logger << DebugInfo
		<< "top_addr = 0x" << std::hex << top_addr << std::dec
		<< EndDebugInfo;
	
	brk_point = top_addr +
    	(memory_page_size - (top_addr % memory_page_size));
		
	if ( verbose )
	{
		logger << DebugInfo 
			<< "Using brk start at @0x" << std::hex << brk_point << std::dec
			<< EndDebugInfo;
	}

	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
DumpBlob()
{
	logger << DebugWarning
		<< "Dumping blobs:" << std::endl;
	DumpBlob(blob, 0);
//	const std::vector<const unisim::util::blob::Blob<ADDRESS_TYPE> *> &blobs =
//		blob->GetBlobs();
//	typename std::vector<
//		const unisim::util::blob::Blob<ADDRESS_TYPE> *
//		>::const_iterator b;
//	b = blobs.begin();
//	for ( b = blobs.begin();
//			b != blobs.end();
//			b++ )
//	{
//		DumpBlob(*b, 0);
//		(*iter)->GetAddrRange(start, end);
//		logger << std::endl
//			<< " + 0x" << std::hex << start << " - "
//			<< "0x" << end << std::dec;
//		const std::vector<const unisim::util::blob::Section<ADDRESS_TYPE> *> &secs =
//			(*iter)->GetSections();
//		typename std::vector<
//			const unisim::util::blob::Section<ADDRESS_TYPE> *
//			>::const_iterator sec;
//		for ( sec = secs.begin();
//				sec != secs.end();
//				sec++ )
//		{
//			logger << std::endl
//				<< "   - Section \"" << (*sec)->GetName() << "\"" << std::endl
//				<< "     Addr = 0x" << std::hex << (*sec)->GetAddr() << std::dec << std::endl
//				<< "     Top  = 0x" << std::hex << ((*sec)->GetAddr() + (*sec)->GetSize()) << std::dec << std::endl
//				<< "     Size = " << (*sec)->GetSize();
//		}
//	}
	logger << EndDebugWarning;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
DumpBlob(const unisim::util::blob::Blob<ADDRESS_TYPE> *b, int level)
{
	ADDRESS_TYPE start_addr, end_addr;
	b->GetAddrRange(start_addr, end_addr);
	const std::vector<const unisim::util::blob::Blob<ADDRESS_TYPE> *> &blobs =
		b->GetBlobs();
	const std::vector<const unisim::util::blob::Section<ADDRESS_TYPE> *> &secs =
		b->GetSections();
	logger << std::endl
		<< "(" << level << ") 0x"
		<< std::hex << start_addr
		<< " - 0x" << end_addr << std::dec
		<< " Cap = " << b->GetCapability();
	typename std::vector<
		const unisim::util::blob::Section<ADDRESS_TYPE> *
		>::const_iterator sec;
	for ( sec = secs.begin();
			sec != secs.end();
			sec++ )
	{
		logger << std::endl
			<< " - Section \"" << (*sec)->GetName() << "\"" << std::endl
			<< "   Addr = 0x" << std::hex << (*sec)->GetAddr() << std::dec << std::endl
			<< "   Top  = 0x" << std::hex << ((*sec)->GetAddr() + (*sec)->GetSize()) << std::dec << std::endl
			<< "   Size = " << (*sec)->GetSize();
	}
	typename std::vector<
		const unisim::util::blob::Blob<ADDRESS_TYPE> *
		>::const_iterator bl;
	for ( bl = blobs.begin();
			bl != blobs.end();
			bl++ )
		DumpBlob(*bl, level + 1);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetupBlob()
{
	if ( blob ) return true;
	if ( !blob_import )
	{
		logger << DebugError
			<< "blob_import not connected."
			<< EndDebugError;
		return false;
	}
	const unisim::util::blob::Blob<ADDRESS_TYPE> *loader_blob = 
		blob_import->GetBlob();
	if ( !loader_blob )
	{
		logger << DebugError
			<< "Failed to setup the blob loader."
			<< EndDebugError;
		return false;
	}

	blob = new typename unisim::util::blob::Blob<ADDRESS_TYPE>();
	blob->Catch();
	blob->AddBlob(loader_blob);

	// Call the system dependent setup
	if ( (system == "arm") || (system == "arm-eabi") )
	{
		if (!SetupBlobARM()) return false;
	}
	if (system == "powerpc")
	{
		if (!SetupBlobPPC()) return false;
	}
	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetupBlobARM()
{
	if(!blob) return false;
	if ((utsname_machine.compare("armv5") == 0) or
      (utsname_machine.compare("armv6") == 0) or
      (utsname_machine.compare("armv7") == 0) or
      )
	{
		// TODO: Check that the program/stack is not in conflict with the
		//   tls and cmpxchg interfaces
		// Set the tls interface, this requires a write into the memory
		//   system
		// The following instructions need to be added to memory:
		// 0xffff0fe0:	e59f0008	ldr r0, [pc, #(16 - 8)] 	@ TLS stored
		// 														@ at 0xffff0ff0
		// 0xffff0fe4:	e1a0f00e	mov pc, lr
		// 0xffff0fe8: 	0
		// 0xffff0fec: 	0
		// 0xffff0ff0: 	0
		// 0xffff0ff4: 	0
		// 0xffff0ff8: 	0
		uint32_t tls_base_addr = 0xffff0fe0UL;
		static const uint32_t tls_buf_length = 7;
		static const uint32_t tls_buf[tls_buf_length] = 
			{0xe59f0008UL, 0xe1a0f00eUL, 0, 0, 0, 0, 0};
		uint32_t *blob_tls_buf = 0;
		blob_tls_buf = (uint32_t *)malloc(sizeof(uint32_t) * tls_buf_length);
		if ( unlikely(verbose) )
		{
			logger << DebugInfo
				<< "Setting TLS handling:";
			for ( unsigned int i = 0; i < tls_buf_length; i++ )
			{
				logger << endl;
				logger << " - 0x" << hex << (tls_base_addr + (i * 4)) << " = "
						<< "0x" << (unsigned int)tls_buf[i] << dec;
			}
			logger << EndDebugInfo;
		}
		
		for ( unsigned int i = 0; i < tls_buf_length; i++ )
		{
			if ( endianess == E_BIG_ENDIAN )
				blob_tls_buf[i] = Host2BigEndian(tls_buf[i]);
			else
				blob_tls_buf[i] = Host2LittleEndian(tls_buf[i]);
		}

		unisim::util::blob::Section<ADDRESS_TYPE> *tls_if_section = 
			new unisim::util::blob::Section<ADDRESS_TYPE>(
					unisim::util::blob::Section<ADDRESS_TYPE>::TY_UNKNOWN,
					unisim::util::blob::Section<ADDRESS_TYPE>::SA_A,
					"tls_if",
					4,
					0,
					tls_base_addr,
					sizeof(tls_buf),
					blob_tls_buf
					);
		if ( unlikely(verbose) )
		{
			logger << DebugInfo
					<< "TLS handler configured." << EndDebugInfo;
		}

		// Set the cmpxchg (atomic compare and exchange) interface, the
		//   following instructions need to be added to memory:
		// 0xffff0fc0:	e5923000	ldr	r3, [r2]
		// 0xffff0fc4:	e0533000	subs	r3, r3, r0
		// 0xffff0fc8:	05821000 	streq	r1, [r2]
		// 0xffff0fcc:	e2730000 	rsbs	r0, r3, #0	; 0x0
		// 0xffff0fd0:	e1a0f00e 	mov	pc, lr
		uint32_t cmpxchg_base_addr = 0xffff0fc0UL;
		static const uint32_t cmpxchg_buf_length = 5;
		static const uint32_t cmpxchg_buf[cmpxchg_buf_length] = {
				0xe5923000UL,
				0xe0533000UL,
				0x05821000UL,
				0xe2730000UL,
				0xe1a0f00eUL
		};
		uint32_t *blob_cmpxchg_buf = 0;
		blob_cmpxchg_buf = (uint32_t *)malloc(sizeof(uint32_t) * 
				cmpxchg_buf_length);
		if ( unlikely(verbose) )
		{
			logger << DebugInfo
					<< "Setting cmpxchg handling:";
			for ( unsigned int i = 0; i < cmpxchg_buf_length; i++ )
			{
				logger << endl;
				logger << " - 0x" << hex << (cmpxchg_base_addr + (i * 4))
						<< " = " << "0x" << (unsigned int)cmpxchg_buf[i] << dec;
			}
			logger << EndDebugInfo;
		}
		
		for ( unsigned int i = 0; i < cmpxchg_buf_length; i++ )
		{
			if ( endianess == E_BIG_ENDIAN )
				blob_cmpxchg_buf[i] = Host2BigEndian(cmpxchg_buf[i]);
			else
				blob_cmpxchg_buf[i] = Host2LittleEndian(cmpxchg_buf[i]);
		}

		typename unisim::util::blob::Section<ADDRESS_TYPE> *cmpxchg_if_section = 
			new unisim::util::blob::Section<ADDRESS_TYPE>(
					unisim::util::blob::Section<ADDRESS_TYPE>::TY_UNKNOWN,
					unisim::util::blob::Section<ADDRESS_TYPE>::SA_A,
					"cmpxchg_if",
					4,
					0,
					cmpxchg_base_addr,
					sizeof(cmpxchg_buf),
					blob_cmpxchg_buf
					);
		if ( unlikely(verbose) )
		{
			logger << DebugInfo
					<< "cmpxchg handler configured." << EndDebugInfo;
		}
		typename unisim::util::blob::Blob<ADDRESS_TYPE> *arm_blob = 
			new typename unisim::util::blob::Blob<ADDRESS_TYPE>();
		arm_blob->SetArchitecture("arm");
		arm_blob->SetEndian(endianess);
		arm_blob->AddSection(tls_if_section);
		arm_blob->AddSection(cmpxchg_if_section);
		blob->AddBlob(arm_blob);
	}

	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetupBlobPPC()
{
	if(!blob) return false;
	// nothing to add to blob
	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetupLoadARM()
{
	if(!SetupBlobARM()) return false;
	for (unsigned int i = 0; i < 13; i++) 
	{
		stringstream buf;
		buf << "r" << i;
		arm_regs[i] = registers_import->GetRegister(buf.str().c_str());
		if (!arm_regs[i]) 
		{
			logger << DebugError
				<< "CPU has no register named " << buf.str() << endl
				<< LOCATION
				<< EndDebugError;
			return false;
		}
	}
	arm_regs[13] = registers_import->GetRegister("sp");
	if (!arm_regs[13]) 
	{
		logger << DebugError
			<< "CPU has no register named sp" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}
	arm_regs[14] = registers_import->GetRegister("lr");
	if (!arm_regs[14]) 
	{
		logger << DebugError
			<< "CPU has no register named lr" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}
	arm_regs[15] = registers_import->GetRegister("pc");
	if (!arm_regs[15]) 
	{
		logger << DebugError
			<< "CPU has no register named pc" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}
	for (unsigned int i = 0; i < 16; i++) 
	{
		if (arm_regs[i]->GetSize() != sizeof(PARAMETER_TYPE)) 
		{
			logger << DebugError
				<< "Unexpected register size for register " << i << endl
				<< LOCATION
				<< EndDebugError;
			return false;
		}
	}
	
	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetupLoadPPC()
{
	if(!SetupBlobPPC()) return false;

    ppc_cr = registers_import->GetRegister("cr");
    if (!ppc_cr) 
	{
		logger << DebugError
			<< "CPU has no register named \"cr\"" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}

    for (unsigned int i = 0; i < 32; i++) 
	{
		stringstream buf;
		buf << "r" << i;
		ppc_regs[i] = registers_import->GetRegister(buf.str().c_str());
		if (!ppc_regs[i]) 
		{
			logger << DebugError
				<< "CPU has no register named \"" << buf.str() << "\"" << endl
				<< LOCATION
				<< EndDebugError;
			return false;
		}
    }

    ppc_cia = registers_import->GetRegister("cia");
    if (!ppc_cia) 
	{
		logger << DebugError
			<< "CPU has no register named \"cia\"" << endl
			<< LOCATION
			<< EndDebugError;
		return false;
	}
	
	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetupLinuxOSARM()
{
	// Set the system calls mappings
	SetSyscallId(string("exit"), 1);
	SetSyscallId(string("read"), 3);
	SetSyscallId(string("write"), 4);
	SetSyscallId(string("open"), 5);
	SetSyscallId(string("close"), 6);
	SetSyscallId(string("unlink"), 10);
	SetSyscallId(string("time"), 13);
	SetSyscallId(string("lseek"), 19);
	SetSyscallId(string("getpid"), 20);
	SetSyscallId(string("getuid"), 24);
	SetSyscallId(string("access"), 33);
	SetSyscallId(string("kill"), 37);
	SetSyscallId(string("rename"), 38);
	SetSyscallId(string("times"), 43);
	SetSyscallId(string("brk"), 45);
	SetSyscallId(string("getgid"), 47);
	SetSyscallId(string("geteuid"), 49);
	SetSyscallId(string("getegid"), 50);
	SetSyscallId(string("ioctl"), 54);
	SetSyscallId(string("setrlimit"), 75);
	SetSyscallId(string("getrusage"), 77);
	SetSyscallId(string("gettimeofday"), 78);
	SetSyscallId(string("munmap"), 91);
    	SetSyscallId(string("ftruncate"), 93);
	SetSyscallId(string("socketcall"), 102);
	SetSyscallId(string("stat"), 106);
	SetSyscallId(string("fstat"), 108);
	SetSyscallId(string("uname"), 122);
	SetSyscallId(string("llseek"), 140);
	SetSyscallId(string("writev"), 146);
	SetSyscallId(string("rt_sigaction"), 174);
	SetSyscallId(string("rt_sigprocmask"), 175);
	SetSyscallId(string("ugetrlimit"), 191);
	SetSyscallId(string("mmap2"), 192);
	SetSyscallId(string("stat64"), 195);
	SetSyscallId(string("fstat64"), 197);
	SetSyscallId(string("getuid32"), 199);
	SetSyscallId(string("getgid32"), 200);
	SetSyscallId(string("geteuid32"), 201);
	SetSyscallId(string("getegid32"), 202);
	SetSyscallId(string("flistxattr"), 234);
	SetSyscallId(string("exit_group"), 248);
	// the following are private to the arm
	SetSyscallId(string("breakpoint"), 983041);
	SetSyscallId(string("cacheflush"), 983042);
	SetSyscallId(string("usr26"), 983043);
	SetSyscallId(string("usr32"), 983044);
	SetSyscallId(string("set_tls"), 983045);

	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetupLinuxOSPPC()
{
    // Set the system calls mappings
    SetSyscallId(string("exit"), 1);
    SetSyscallId(string("read"), 3);
    SetSyscallId(string("write"), 4);
    SetSyscallId(string("open"), 5);
    SetSyscallId(string("close"), 6);
    SetSyscallId(string("unlink"), 10);
    SetSyscallId(string("time"), 13);
    SetSyscallId(string("lseek"), 19);
    SetSyscallId(string("getpid"), 20);
    SetSyscallId(string("getuid"), 24);
    SetSyscallId(string("access"), 33);
    SetSyscallId(string("kill"), 37);
    SetSyscallId(string("rename"), 38);
    SetSyscallId(string("times"), 43);
    SetSyscallId(string("brk"), 45);
    SetSyscallId(string("getgid"), 47);
    SetSyscallId(string("geteuid"), 49);
    SetSyscallId(string("getegid"), 50);
    SetSyscallId(string("ioctl"), 54);
    SetSyscallId(string("setrlimit"), 75);
    SetSyscallId(string("getrlimit"), 76);
    SetSyscallId(string("getrusage"), 77);
    SetSyscallId(string("gettimeofday"), 78);
    SetSyscallId(string("mmap"), 90);
    SetSyscallId(string("munmap"), 91);
    SetSyscallId(string("ftruncate"), 93);
    SetSyscallId(string("socketcall"), 102);
    SetSyscallId(string("stat"), 106);
    SetSyscallId(string("fstat"), 108);
    SetSyscallId(string("uname"), 122);
    SetSyscallId(string("llseek"), 140);
    SetSyscallId(string("writev"), 146);
    SetSyscallId(string("rt_sigaction"), 173);
    SetSyscallId(string("rt_sigprocmask"), 174);
    SetSyscallId(string("ugetrlimit"), 190);
    SetSyscallId(string("mmap2"), 192);
    SetSyscallId(string("stat64"), 195);
    SetSyscallId(string("fstat64"), 197);
    SetSyscallId(string("fcntl64"), 204);
    SetSyscallId(string("flistxattr"), 217);
    SetSyscallId(string("exit_group"), 234);
	
	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Setup(ServiceExportBase *srv_export)
{
	if ( srv_export == &loader_export )
	{
		if ( !SetupLoad() )
		{
			logger << DebugError
				<< "Failed the setup of the load_export."
				<< EndDebugError;
			return false;
		}
		return true;
	}

	if ( srv_export == &blob_export )
	{
		if ( !SetupBlob() )
		{
			logger << DebugError
				<< "Failed the setup of the blob_export."
				<< EndDebugError;
			return false;
		}
		return true;
	}

	if ( srv_export == &linux_os_export )
	{
		if ( !SetupLinuxOS() )
		{
			logger << DebugError
				<< "Failed to setup of the linux_os_export."
				<< EndDebugError;
			return false;
		}
		return true;
	}
	
	logger << DebugError << "Internal error" << EndDebugError;
	
	return false;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
EndSetup()
{
	if ( (system == "arm") || (system == "arm-eabi") )
	{
		if ( !LoadARM() )
		{
			logger << DebugError
				<< "Failed the ARM setup."
				<< EndDebugError;
			return false;
		}
		return true;
	}

	if ( system == "powerpc" )
	{
		if ( !LoadPPC() )
		{
			logger << DebugError
				<< "Failed the PowerPC setup."
				<< EndDebugError;
			return false;
		}
		return true;
	}
	
	logger << DebugError
		<< "Unknown system to load (supported arm and powerpc)."
		<< EndDebugError;
	return false;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LoadARM()
{
	if ( !blob ) 
	{
		logger << DebugError
			<< "blob not found while loading ARM setup."
			<< EndDebugError;
		return false;
	}

	bool status = true;
	const typename unisim::util::blob::Section<ADDRESS_TYPE> 
		*tls_if_section = blob->FindSection("tls_if");
	if ( tls_if_section )
	{
		if ( !memory_import )
		{
			logger << DebugError
				<< "memory_import not found when loading ARM setup (tls)."
				<< EndDebugError;
			status = false;
		}
		else if ( !memory_import->WriteMemory(tls_if_section->GetAddr(), 
						tls_if_section->GetData(), 
						tls_if_section->GetSize()) )
		{
			logger << DebugError
				<< "Could not write into memory tls data while loading"
				<< " ARM setup."
				<< EndDebugError;
			status = false;
		}
	}
	const typename unisim::util::blob::Section<ADDRESS_TYPE> 
		*cmpxchg_if_section = blob->FindSection("cmpxchg_if");
	if ( cmpxchg_if_section )
	{
		if ( !memory_import )
		{
			logger << DebugError
				<< "memory_import not found when loading ARM setup (cmpxchg)."
				<< EndDebugError;
			status = false;
		}
		else if ( !memory_import->WriteMemory(cmpxchg_if_section->GetAddr(), 
					cmpxchg_if_section->GetData(), 
					cmpxchg_if_section->GetSize()) ) 
		{
			logger << DebugError
				<< "Could not write into memory cmpxchg data while loading"
				<< " ARM setup."
				<< EndDebugError;
			status = false;
		}
	}
	
	// Set initial CPU registers
	PARAMETER_TYPE pc = blob->GetEntryPoint();
	if ( unlikely(verbose) )
		logger << DebugInfo
			<< "Setting register \"" << arm_regs[15]->GetName() << "\""
			<< " to value 0x" << hex << pc << dec << endl
			<< LOCATION
			<< EndDebugInfo;
	arm_regs[15]->SetValue(&pc);
	PARAMETER_TYPE st = blob->GetStackBase();
	if ( unlikely(verbose) )
		logger << DebugInfo
			<< "Setting register \"" << arm_regs[13]->GetName() << "\""
			<< " to value 0x" << hex << st << dec << endl
			<< LOCATION
			<< EndDebugInfo;
	arm_regs[13]->SetValue(&st);

	PARAMETER_TYPE envp4;
	PARAMETER_TYPE envp8;
	memory_import->ReadMemory(st + 4, &envp4, sizeof(envp4));
	memory_import->ReadMemory(st + 8, &envp8, sizeof(envp4));	
	arm_regs[1]->SetValue(&envp4);
	arm_regs[2]->SetValue(&envp8);
	return status;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LoadPPC()
{
	if ( !blob ) 
	{
		logger << DebugError
			<< "blob not found while loading PPC setup."
			<< EndDebugError;
		return false;
	}

    for (unsigned int i = 0; i < 32; i++) 
	{
		if (!ppc_regs[i]) return false;
		PARAMETER_TYPE zero = 0;
		ppc_regs[i]->SetValue(&zero);
    }

    // Set initial CPU registers
    PARAMETER_TYPE pc = blob->GetEntryPoint();
    if (!ppc_cia) return false;
    ppc_cia->SetValue(&pc);
    PARAMETER_TYPE st = blob->GetStackBase();
	if(!ppc_regs[1]) return false;
    ppc_regs[1]->SetValue(&st);
	
	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Load() 
{
	if(!loader_import) return false;
	if(!loader_import->Load()) return false;
	// Call the system dependent Load
	if (system == "arm") return LoadARM();
	if (system == "arm-eabi") return LoadARM();
	if (system == "powerpc") return LoadPPC();
	
	return false;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
const typename unisim::util::blob::Blob<ADDRESS_TYPE> *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetBlob()
{
	return blob;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ReadMem(ADDRESS_TYPE addr, void *buffer, uint32_t size) 
{
	bool result;
	result = memory_injection_import->InjectReadMemory(addr, buffer, size);
	if(unlikely(verbose)) 
	{
		logger << DebugInfo
			<< "OS read memory: " << endl
			<< " - addr = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for (unsigned int i = 0; i < size; i++)
			logger << " " << (unsigned int)(((uint8_t *)buffer)[i]);
		logger << dec << endl
			<< LOCATION
			<< EndDebugInfo;
	}
	return result;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
WriteMem(ADDRESS_TYPE addr, const void *buffer, uint32_t size) 
{
	bool result;

	if(unlikely(verbose)) 
	{
		logger << DebugInfo 
			<< "OS write memory: " << endl
			<< " - addr = 0x" << hex << addr << dec << endl
			<< " - size = " << size << endl
			<< " - data =" << hex;
		for (unsigned int i = 0; i < size; i++)
			logger << " " << (unsigned int)(((uint8_t *)buffer)[i]);
		logger << dec << endl
			<< LOCATION
			<< EndDebugInfo;
	}

	result = memory_injection_import->InjectWriteMemory(addr, buffer, size);
	return result;
}

/**
 * Checks that an implementation exists for a syscall name.
 *
 * @param syscall_name the name of the syscall to check.
 * @return True if the syscall exists.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
HasSyscall(const string &syscall_name) 
{
	return syscall_name_map.find(syscall_name) != syscall_name_map.end();
}

/**
 * Checks that an id has been associated to a system call implementation.
 *
 * @param syscall_id id to check. 
 * @return True if syscall_id has been associated to an implementation.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
HasSyscall(int syscall_id) 
{
	return syscall_impl_assoc_map.find(syscall_id) != syscall_impl_assoc_map.end();
}

/**
 * Associates an id (integer) to a the implementation of the syscall specified by syscall_name.
 *
 * @param syscall_name
 * @param syscall_id
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetSyscallId(const string &syscall_name, int syscall_id) 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "Associating syscall_name \"" << syscall_name << "\""
			<< "to syscall_id number " << syscall_id << endl
			<< LOCATION
			<< EndDebugInfo;
    if (HasSyscall(syscall_name)) 
	{
    	if (HasSyscall(syscall_id)) 
		{
    		stringstream s;
    		s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << endl;
    		s << "syscall_id already associated to syscall \"" << syscall_name_assoc_map[syscall_id] << "\"" << endl;
    		s << "  you wanted to associate it to " << syscall_name << endl;
    		throw std::runtime_error(s.str().c_str());
    	}
    	syscall_name_assoc_map[syscall_id] = syscall_name;
    	syscall_impl_assoc_map[syscall_id] = syscall_name_map[syscall_name];
    } 
	else 
	{
    	stringstream s;
    	s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << endl;
    	s << "Unimplemented system call (" << syscall_name << ")";
    	throw std::runtime_error(s.str().c_str());      
    }
}

/**
 * Gets from the cpu calling this method the syscall id to execute and emulates it if it exists.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ExecuteSystemCall(int id) 
{
	int translated_id = GetSyscallNumber(id);

	if (HasSyscall(translated_id)) 
	{
		current_syscall_id = translated_id;
		current_syscall_name = syscall_name_assoc_map[translated_id];
		if(unlikely(verbose))
			logger << DebugInfo
				<< "Executing syscall(name = " << current_syscall_name << ";"
				<< " id = " << translated_id << ";"
				<< " unstranslated id = " << id << ")" << endl
				<< LOCATION
				<< EndDebugInfo;
		syscall_t y = syscall_impl_assoc_map[translated_id];
		(this->*y)();
    } 
	else
    	throw UnimplementedSystemCall(__FUNCTION__,
				    __FILE__,
				    __LINE__,
				    translated_id);
}

/**
 * Checks if the endianess of the host and the target system differs.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Swap() 
{
#if BYTE_ORDER == BIG_ENDIAN
	if (GetEndianess() == E_BIG_ENDIAN) return false;
	else return true;
#else
    if (GetEndianess() == E_BIG_ENDIAN) return true;
    else return false;
#endif
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PerformSwap(void *buf,
		int count) 
{
	int size = count;
    
    if (count > 0) 
	{
    	char *dst_base, *dst;
    	char *src;

    	dst = (char *)malloc(sizeof(char) * count);
    	dst_base = dst;
    	src = (char *)buf + count - 1;
      
    	do 
		{
    		*dst = *src;
        }
 		while (src--, dst++, --count);
    	memcpy(buf, dst_base, size);
    	free(dst_base);
    }
}
  
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
StringLength(ADDRESS_TYPE addr) 
{
    int len = 0;
    char buffer;
    
    while (1) 
	{
    	ReadMem(addr, &buffer, 1);
    	if (buffer == 0) return len;
    	len++;
    	addr += 1;
    }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_unknown() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "Unimplemented system call #" << current_syscall_id << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(-1, true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_exit() 
{
	string name = "exit";
	int ret;
    
	ret = GetSystemCallParam(0);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "LSC_exit with ret = 0X" << hex << ret << dec
			<< LOCATION
			<< EndDebugInfo;
	Object::Stop(ret);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_read() 
{
	string name = "read";
	int fd;
	size_t count;
	ADDRESS_TYPE buf_addr;
	void *buf;
	size_t ret;
   
	fd = GetSystemCallParam(0);
	buf_addr = (ADDRESS_TYPE) GetSystemCallParam(1);
	count = (size_t) GetSystemCallParam(2);
	
	buf = malloc(count);
   
	if (buf) 
	{
		ret = read(fd, buf, count);
		if (ret > 0) WriteMem(buf_addr, buf, ret);
		free(buf);
	}
	else 
		ret = (size_t)-1;
		
	if(unlikely(verbose))
		logger << DebugInfo
			<< "read(fd=" << fd << ", buf=0x" << hex << buf_addr << dec 
			<< ", count=" << count << ") return " << ret << endl 
			<< LOCATION 
			<< EndDebugInfo;
		
	SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_write() 
{
	int fd; 
	size_t count;
	void *buf;
	ADDRESS_TYPE buf_addr;
	size_t ret;
   
	fd = GetSystemCallParam(0);
	buf_addr = GetSystemCallParam(1);
	count = (size_t)GetSystemCallParam(2);
	buf = malloc(count);
   
	ret = (size_t)-1;
   
	if (buf) 
	{
		ReadMem(buf_addr, buf, count);
		if ((fd == 1 || fd == 2)) 
		{
			char *tbuf = new char[count + 1];
			memcpy(tbuf, buf, count);
			tbuf[count] = '\0';
			string str(tbuf);
			cout << (str);
			cout << flush;
			ret = count;
			delete[] tbuf;
		}
		else 
			ret = write(fd, buf, count);
		free(buf);
	} 
	else 
		ret = (size_t)-1;
		
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "write(fd=" << fd << ", buf=0x" << hex << buf_addr << dec 
			<< ", count=" << count << ") return " << ret << endl 
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_open() 
{
	ADDRESS_TYPE addr;
	int pathnamelen;
	char *pathname;
	int flags;
	int64_t ret;
	mode_t mode;
    
	addr = GetSystemCallParam(0);
	pathnamelen = StringLength(addr);
	pathname = (char *) malloc(pathnamelen + 1);
	ReadMem(addr, pathname, pathnamelen + 1);
	flags = GetSystemCallParam(1);
	mode = GetSystemCallParam(2);
#if defined(linux) || defined(__linux) || defined(__linux__)
	ret = open(pathname, flags, mode);
#else
	int host_flags = 0;
	int host_mode = 0;
	// non-Linux open flags encoding may differ from a true Linux host
	if ((flags & LINUX_O_ACCMODE) == LINUX_O_RDONLY) host_flags = (host_flags & ~O_ACCMODE) | O_RDONLY;
	if ((flags & LINUX_O_ACCMODE) == LINUX_O_WRONLY) host_flags = (host_flags & ~O_ACCMODE) | O_WRONLY;
	if ((flags & LINUX_O_ACCMODE) == LINUX_O_RDWR) host_flags = (host_flags & ~O_ACCMODE) | O_RDWR;
	if (flags & LINUX_O_CREAT) host_flags |= O_CREAT;
	if (flags & LINUX_O_EXCL) host_flags |= O_EXCL;
	if (flags & LINUX_O_TRUNC) host_flags |= O_TRUNC;
	if (flags & LINUX_O_APPEND) host_flags |= O_APPEND;
#if defined(_WIN32) || defined(_WIN64)
	host_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
#else
	host_mode = mode; // other UNIX systems should have the same bit encoding for protection
#endif
	if (strcmp(pathname, osrelease_filename) == 0)
	{
		{
			std::ofstream fake_file(fake_osrelease_filename);
			fake_file << utsname_release << std::endl;
		}
		ret = open(fake_osrelease_filename, host_flags, host_mode);
	}
	else
	{
		ret = open(pathname, host_flags, host_mode);
	}
#endif
	
	if(unlikely(verbose))
		logger << DebugInfo
			<< "open(pathname=\"" << pathname << "\", flags=0x" << hex << flags 
			<< ", mode=0x" << mode << dec << ") return " << ret << endl 
			<< LOCATION
			<< EndDebugInfo;
		
	free(pathname);
	SetSystemCallStatus(ret, (ret < 0));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_close() 
{
	int fd;
	int ret;
   
	fd = GetSystemCallParam(0);
	ret = close(fd);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "close(fd=" << fd << ") return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret, ret < 0);
}
	

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_lseek() 
{
	int fildes;
	off_t offset;
	int whence;
	off_t ret;
   
	fildes = GetSystemCallParam(0);
	offset = GetSystemCallParam(1);
	whence = GetSystemCallParam(2);
	ret = lseek(fildes, offset, whence);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "lseek(fildes=" << fildes << ", offset=" << offset 
			<< ", whence=" << whence << ") return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
  
	if (ret == (off_t)-1) 
		SetSystemCallStatus(errno, true);
	else
		SetSystemCallStatus(ret, false);	
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getpid() 
{
	pid_t ret;
    
	ret = (pid_t) 1000;
	if(unlikely(verbose))
		logger << DebugInfo
			<< "getpid() return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getuid() 
{
#if defined(_WIN32) || defined(_WIN64)
	uint32_t ret = 0;
#else
	uid_t ret;
    
	ret = getuid();
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "getuid() return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_access() 
{
	ADDRESS_TYPE addr;
	int pathnamelen;
	char *pathname;
	mode_t mode;
	int ret;
    
	addr = GetSystemCallParam(0);
	pathnamelen = StringLength(addr);
	pathname = (char *) malloc(pathnamelen + 1);
	ReadMem(addr, pathname, pathnamelen + 1);
	mode = GetSystemCallParam(1);
#if defined(_WIN32) || defined(_WIN64)
	int win_mode = 0;
	win_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
	ret = access(pathname, win_mode);
#else
	ret = access(pathname, mode);
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "access(pathname=\"" << pathname 
			<< "\", mode=0x" << hex << mode << dec << ") return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	free(pathname);
	SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_times() 
{
	int ret;
	ADDRESS_TYPE buf_addr;
	buf_addr = GetSystemCallParam(0);

	if ( (system == "arm") || (system == "arm-eabi") )
	{
		struct arm_tms_t target_tms;
		ret = Times(&target_tms);

		if (ret >= 0)
		{
			WriteMem(buf_addr, &target_tms, sizeof(target_tms));
		}
	}
	else if (system == "powerpc")
	{
		struct powerpc_tms_t target_tms;
		ret = Times(&target_tms);

		if (ret >= 0)
		{
			WriteMem(buf_addr, &target_tms, sizeof(target_tms));
		}
	}
	else ret = -1;

	if(unlikely(verbose))
		logger << DebugInfo
			<< "times(buf=0x" << hex << buf_addr << dec << ") return " << ret 
			<< endl 
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret, ret != -1);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_brk() 
{
	ADDRESS_TYPE new_brk_point;
    
	new_brk_point = GetSystemCallParam(0);
   
	if (new_brk_point > GetBrkPoint())
		SetBrkPoint(new_brk_point);      
		
	if(unlikely(verbose))
		logger << DebugInfo
			<< "brk(new_brk_point=0x" << hex << new_brk_point 
			<< ") return 0x" << GetBrkPoint() << dec << endl
			<< LOCATION 
			<< EndDebugInfo;
	SetSystemCallStatus(GetBrkPoint(),false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getgid() 
{
#if defined(_WIN32) || defined(_WIN64)
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getgid();
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "getgid() return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_geteuid() 
{
#if defined(_WIN32) || defined(_WIN64)
	uint32_t ret = 0;
#else
	uid_t ret;
    
	ret = geteuid();
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "geteuid() return " << ret << endl 
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getegid() 
{
#if defined(_WIN32) || defined(_WIN64)
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getegid();
#endif
	if(unlikely(verbose))
		logger << DebugInfo
			<< "getegid() return " << ret << endl
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret,false);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_munmap() 
{
	size_t u = (size_t)(GetSystemCallParam(1));
   
	if (GetMmapBrkPoint() - u > GetMmapBrkPoint()) 
	{
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(unlikely(verbose))
			logger << DebugInfo
				<< "size = " << u 
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
				<< endl
				<< LOCATION << EndDebugInfo;
		return;
	}
    
	if (GetMmapBrkPoint() - u < GetMmapBase())
		u = (size_t)(GetMmapBrkPoint() - GetMmapBase());
   
	if (GetMmapBrkPoint() - u >= GetMmapBrkPoint()) 
	{
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(unlikely(verbose))
			logger << DebugInfo
				<< "size = " << u 
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
				<< endl
				<< LOCATION << EndDebugInfo;
		return;
	}

	SetSystemCallStatus((PARAMETER_TYPE)0,false);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "size = " << u 
			<< ", ret = 0x" << hex << 0 << dec 
			<< endl 
			<< LOCATION << EndDebugInfo;
	SetMmapBrkPoint(GetMmapBrkPoint() - u);
}
	
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_stat() 
{
	logger << DebugWarning
			<< "TODO" << endl
			<< LOCATION
			<< EndDebugWarning;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Stat(int fd, struct powerpc_stat_t *target_stat)
{
	int ret;
	struct stat host_stat;
	ret = fstat(fd, &host_stat);
	if (ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
	// 64-bit host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(_WIN64) // Windows x64
	target_stat->st_blksize = Host2Target((int32_t) 512);
	target_stat->st_blocks = Host2Target((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
	target_stat->st_blksize = Host2Target(endianess, (int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
	target_stat->st_blksize = Host2Target(endianess, (int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(_WIN32) // Windows 32
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atime);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtime);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctime);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
	target_stat->__pad1 = 0;
	target_stat->__pad2 = 0;
	target_stat->__unused4 = 0;
	target_stat->__unused5 = 0;
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Stat64(int fd, struct powerpc_stat64_t *target_stat)
{
	int ret;
#if defined(_WIN32) || defined(_WIN64)
	struct _stati64 host_stat;
	ret = _fstati64(fd, &host_stat);
#elif defined(linux) || defined(__linux) || defined(__linux__)
	struct stat64 host_stat;
	ret = fstat64(fd, &host_stat);
#elif defined(__APPLE_CC__)
	struct stat host_stat;
	ret = fstat(fd, &host_stat);
#endif

	if (ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
	// 64-bit host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(_WIN64) // Windows x64
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC64/x86_64
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(_WIN32) // Windows 32
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atime);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtime);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctime);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
	target_stat->__pad2 = 0;
    target_stat->__unused4 = 0;
    target_stat->__unused5 = 0;
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Stat64(int fd, arm_stat64_t *target_stat)
{
	int ret;
#if defined(_WIN32) || defined(_WIN64)
	struct _stati64 host_stat;
	ret = _fstati64(fd, &host_stat);
#elif defined(linux) || defined(__linux) || defined(__linux__)
	struct stat64 host_stat;
	ret = fstat64(fd, &host_stat);
#elif defined(__APPLE_CC__)
	struct stat host_stat;
	ret = fstat(fd, &host_stat);
#endif
	if(ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
	// 64-bit host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->__st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
	target_stat->st_ino = Host2Target(endianess, (uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(_WIN64) // Windows x64
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->__st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
	target_stat->st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(_WIN32) // Windows 32
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atime);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtime);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctime);
	target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
	target_stat->__pad1 = 0;
	target_stat->__pad2 = 0;
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Times(struct powerpc_tms_t *target_tms)
{
	int ret;
#if defined(_WIN32) || defined(_WIN64)
	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	if (GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime)) return -1;

	target_tms->tms_utime = Host2Target(endianess, (uint32_t) ftUserTime.dwLowDateTime);
	target_tms->tms_stime = Host2Target(endianess, (uint32_t) ftKernelTime.dwLowDateTime);
	target_tms->tms_cutime = 0;   // User CPU time of dead children
	target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
	struct tms host_tms;

	ret = (int) times(&host_tms);
	target_tms->tms_utime = Host2Target(endianess, (int32_t) host_tms.tms_utime);
	target_tms->tms_stime = Host2Target(endianess, (int32_t) host_tms.tms_stime);
	target_tms->tms_cutime = Host2Target(endianess, (int32_t) host_tms.tms_cutime);
	target_tms->tms_cstime = Host2Target(endianess, (int32_t) host_tms.tms_cstime);
#endif
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Times(struct arm_tms_t *target_tms)
{
	int ret;
#if defined(_WIN32) || defined(_WIN64)
	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	if (GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime)) return -1;

	target_tms->tms_utime = Host2Target(endianess, (uint32_t) ftUserTime.dwLowDateTime);
	target_tms->tms_stime = Host2Target(endianess, (uint32_t) ftKernelTime.dwLowDateTime);
	target_tms->tms_cutime = 0;   // User CPU time of dead children
	target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
	struct tms host_tms;

	ret = (int) times(&host_tms);
	target_tms->tms_utime = Host2Target(endianess, (int32_t) host_tms.tms_utime);
	target_tms->tms_stime = Host2Target(endianess, (int32_t) host_tms.tms_stime);
	target_tms->tms_cutime = Host2Target(endianess, (int32_t) host_tms.tms_cutime);
	target_tms->tms_cstime = Host2Target(endianess, (int32_t) host_tms.tms_cstime);
#endif
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetTimeOfDay(struct arm_timeval_t *target_tv)
{
	int ret = -1;

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	struct timeval host_tv;

	ret = (int)gettimeofday(&host_tv, NULL);
	target_tv->tv_sec = Host2Target(endianess, (int32_t) host_tv.tv_sec);
	target_tv->tv_usec = Host2Target(endianess, (int32_t) host_tv.tv_usec);
#endif
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetTimeOfDay(struct powerpc_timeval_t *target_tv)
{
	int ret = -1;

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	struct timeval host_tv;

	ret = (int) gettimeofday(&host_tv, NULL);
	target_tv->tv_sec = Host2Target(endianess, (int32_t) host_tv.tv_sec);
	target_tv->tv_usec = Host2Target(endianess, (int32_t) host_tv.tv_usec);
#endif
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetRUsage(PARAMETER_TYPE who, struct arm_rusage_t *target_ru)
{
	int ret = -1;

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	struct rusage host_ru;

	ret = (int)getrusage((int)who, &host_ru);
	target_ru->ru_utime.tv_sec = Host2Target(endianess, (int32_t) host_ru.ru_utime.tv_sec);
	target_ru->ru_utime.tv_usec = Host2Target(endianess, (int32_t) host_ru.ru_utime.tv_usec);
	target_ru->ru_stime.tv_sec = Host2Target(endianess, (int32_t) host_ru.ru_stime.tv_sec);
	target_ru->ru_stime.tv_usec = Host2Target(endianess, (int32_t) host_ru.ru_stime.tv_usec);
	target_ru->ru_maxrss = Host2Target(endianess, (int32_t) host_ru.ru_maxrss);
	target_ru->ru_ixrss = Host2Target(endianess, (int32_t) host_ru.ru_ixrss);
	target_ru->ru_idrss = Host2Target(endianess, (int32_t) host_ru.ru_idrss);
	target_ru->ru_isrss = Host2Target(endianess, (int32_t) host_ru.ru_isrss);
	target_ru->ru_minflt = Host2Target(endianess, (int32_t) host_ru.ru_minflt);
	target_ru->ru_majflt = Host2Target(endianess, (int32_t) host_ru.ru_majflt);
	target_ru->ru_nswap = Host2Target(endianess, (int32_t) host_ru.ru_nswap);
	target_ru->ru_inblock = Host2Target(endianess, (int32_t) host_ru.ru_inblock);
	target_ru->ru_oublock = Host2Target(endianess, (int32_t) host_ru.ru_oublock);
	target_ru->ru_msgsnd = Host2Target(endianess, (int32_t) host_ru.ru_msgsnd);
	target_ru->ru_msgrcv = Host2Target(endianess, (int32_t) host_ru.ru_msgrcv);
	target_ru->ru_nsignals = Host2Target(endianess, (int32_t) host_ru.ru_nsignals);
	target_ru->ru_nvcsw = Host2Target(endianess, (int32_t) host_ru.ru_nvcsw);
	target_ru->ru_nivcsw = Host2Target(endianess, (int32_t) host_ru.ru_nivcsw);
#endif
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetRUsage(PARAMETER_TYPE who, struct powerpc_rusage_t *target_ru)
{
	int ret = -1;

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	struct rusage host_ru;

	ret = (int)getrusage((int)who, &host_ru);
	target_ru->ru_utime.tv_sec = Host2Target(endianess, (int32_t) host_ru.ru_utime.tv_sec);
	target_ru->ru_utime.tv_usec = Host2Target(endianess, (int32_t) host_ru.ru_utime.tv_usec);
	target_ru->ru_stime.tv_sec = Host2Target(endianess, (int32_t) host_ru.ru_stime.tv_sec);
	target_ru->ru_stime.tv_usec = Host2Target(endianess, (int32_t) host_ru.ru_stime.tv_usec);
	target_ru->ru_maxrss = Host2Target(endianess, (int32_t) host_ru.ru_maxrss);
	target_ru->ru_ixrss = Host2Target(endianess, (int32_t) host_ru.ru_ixrss);
	target_ru->ru_idrss = Host2Target(endianess, (int32_t) host_ru.ru_idrss);
	target_ru->ru_isrss = Host2Target(endianess, (int32_t) host_ru.ru_isrss);
	target_ru->ru_minflt = Host2Target(endianess, (int32_t) host_ru.ru_minflt);
	target_ru->ru_majflt = Host2Target(endianess, (int32_t) host_ru.ru_majflt);
	target_ru->ru_nswap = Host2Target(endianess, (int32_t) host_ru.ru_nswap);
	target_ru->ru_inblock = Host2Target(endianess, (int32_t) host_ru.ru_inblock);
	target_ru->ru_oublock = Host2Target(endianess, (int32_t) host_ru.ru_oublock);
	target_ru->ru_msgsnd = Host2Target(endianess, (int32_t) host_ru.ru_msgsnd);
	target_ru->ru_msgrcv = Host2Target(endianess, (int32_t) host_ru.ru_msgrcv);
	target_ru->ru_nsignals = Host2Target(endianess, (int32_t) host_ru.ru_nsignals);
	target_ru->ru_nvcsw = Host2Target(endianess, (int32_t) host_ru.ru_nvcsw);
	target_ru->ru_nivcsw = Host2Target(endianess, (int32_t) host_ru.ru_nivcsw);
#endif
	return ret;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fstat() 
{
	int ret;
	int fd;
	ADDRESS_TYPE buf_address;

	fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	if ( (system == "arm") || (system == "arm-eabi") )
	{
		ret = -1;
	}
	else if (system == "powerpc")
	{
		struct powerpc_stat_t target_stat;
		ret = Stat(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else ret = -1;

	if(unlikely(verbose))
		logger << DebugInfo
			<< "stat(fd=" << fd 
			<< ", buf_addr=0x" << hex << buf_address << dec 
			<< ") return " << ret << endl
			<< LOCATION 
			<< EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_uname() 
{
	int ret;
 	ADDRESS_TYPE buf_addr = GetSystemCallParam(0);
 	struct utsname_t value;
 	memset(&value, 0, sizeof(value));
 	memcpy(&(value.sysname),
 			utsname_sysname.c_str(), utsname_sysname.length() + 1);
 	memcpy(&(value.nodename),
 			utsname_nodename.c_str(), utsname_nodename.length() + 1);
 	memcpy(&(value.release),
 			utsname_release.c_str(), utsname_release.length() + 1);
 	memcpy(&(value.version),
 			utsname_version.c_str(), utsname_version.length() + 1);
 	memcpy(&(value.machine),
 			utsname_machine.c_str(), utsname_machine.length() + 1);
 	WriteMem(buf_addr, &value, sizeof(value));
 	ret = 0;
	SetSystemCallStatus((PARAMETER_TYPE) ret, ret < 0);
}
	
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_llseek() 
{
	int fd;
	uint32_t offset_high;
	uint32_t offset_low;
	PARAMETER_TYPE result_addr;
	uint64_t lseek_ret64;
	off_t lseek_ret;
	int whence;
		
	fd = GetSystemCallParam(0);
	offset_high = GetSystemCallParam(1);
	offset_low = GetSystemCallParam(2);
	result_addr = GetSystemCallParam(3);
	whence = GetSystemCallParam(4);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "llseek(fd=" << fd 
			<< ", offset=" << (((int64_t) offset_high << 32) | (int64_t) offset_low)
			<< ", result_addr=0x" << hex << result_addr << dec 
			<< ", whence=" << whence << ")" << endl 
			<< LOCATION << EndDebugInfo;
	if (offset_high == 0) 
	{
		lseek_ret = lseek(fd, offset_low, whence);
		if (lseek_ret >= 0) 
		{
			lseek_ret64 = lseek_ret;
			if (Swap())
				PerformSwap(&lseek_ret64, sizeof(lseek_ret64));
			WriteMem(result_addr, &lseek_ret64, sizeof(lseek_ret64));
			SetSystemCallStatus((PARAMETER_TYPE)lseek_ret, false);
		} 
		else 
			SetSystemCallStatus((PARAMETER_TYPE)errno, true);
	} 
	else 
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_writev() 
{
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" 
			<< hex << ((PARAMETER_TYPE)(-EINVAL)) << dec << endl
			<< LOCATION << EndDebugInfo;

	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_mmap() 
{
	SetSystemCallStatus(-1,true); return;
	if (GetSystemCallParam(3) == 0x32) 
	{ /* MAP_PRIVATE | MAP_ANONYMOUS */
		SetSystemCallStatus(GetSystemCallParam(0),false);
		if(unlikely(verbose)) 
			logger << DebugInfo
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)GetSystemCallParam(0)) << dec 
				<< endl << LOCATION << EndDebugInfo;
		return;
	}
		
	if ((GetSystemCallParam(3)&0xFF) != 0x22) 
	{ /* MAP_PRIVATE | MAP_ANONYMOUS */
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(unlikely(verbose)) 
			logger << DebugInfo
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
				<< endl << LOCATION << EndDebugInfo;
		return;
	}
	SetSystemCallStatus(GetMmapBrkPoint(),false);
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
			<< ", size = " << GetSystemCallParam(1)
			<< ", ret = 0x" << hex << GetMmapBrkPoint() << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_mmap2() 
{
	if (GetSystemCallParam(3) != 0x22) 
	{ /* MAP_PRIVATE | MAP_ANONYMOUS */
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(unlikely(verbose))    
			logger << DebugInfo 
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
				<< endl << LOCATION << EndDebugInfo;
		return;
	}
    
	if (GetMmapBrkPoint() + GetSystemCallParam(1) > GetSystemCallParam(1)) 
	{
		SetSystemCallStatus(GetMmapBrkPoint(),false);
		if(unlikely(verbose)) 
			logger << DebugInfo
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << GetMmapBrkPoint() << dec 
				<< endl << LOCATION << EndDebugInfo;
		SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
	} 
	else 
	{
		if(unlikely(verbose)) 
			logger << DebugInfo
				<< "map_type = 0x" << hex << GetSystemCallParam(3) << dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
				<< endl << LOCATION << EndDebugInfo;
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
	}
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_stat64() 
{ 
	int ret;
	ADDRESS_TYPE buf_address;
	int fd;

	fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	if ( (system == "arm") || (system == "arm-eabi") )
	{
		struct arm_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else if (system == "powerpc")
	{
		struct powerpc_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else ret = -1;
	if(unlikely(verbose))     
		logger << DebugInfo
			<< "fd = " << fd << ", buf_address = 0x" << hex << buf_address << dec 
			<< ", ret = 0x" << hex << ret << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fstat64() 
{ 
	int ret;
	ADDRESS_TYPE buf_address;
	int fd;

	fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	if ( (system == "arm") || (system == "arm-eabi") )
	{
		struct arm_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else if (system == "powerpc")
	{
		struct powerpc_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else ret = -1;
	if(unlikely(verbose))     
		logger << DebugInfo
			<< "fd = " << fd << ", buf_address = 0x" << hex << buf_address << dec 
			<< ", ret = 0x" << hex << ret << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getuid32() 
{
#if defined(_WIN32) || defined(_WIN64)
	uint32_t ret = 0;
#else
	uid_t ret;

	ret = getuid();
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getgid32() 
{
#if defined(_WIN32) || defined(_WIN64)
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getgid();
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_geteuid32() 
{
#if defined(_WIN32) || defined(_WIN64)
	uint32_t ret = 0;
#else
	uid_t ret;
    
	ret = geteuid();
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getegid32() 
{
#if defined(_WIN32) || defined(_WIN64)
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getegid();
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_flistxattr() 
{
	logger << DebugWarning
		<< "TODO" 
		<< endl << LOCATION << EndDebugWarning;
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_exit_group() 
{ 
	if(unlikely(verbose))  
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fcntl() 
{ 
	int64_t ret;
    
#if defined(_WIN32) || defined(_WIN64)
	ret = -1;
#else
	ret = fcntl(GetSystemCallParam(0),
			GetSystemCallParam(1),
			GetSystemCallParam(2));
#endif
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = " <<  ((PARAMETER_TYPE)ret)  
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fcntl64() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_dup() 
{
	int ret;
	int oldfd = GetSystemCallParam(0);
    
	ret = dup(oldfd);
	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "oldfd = " << oldfd << ", new fd = " << ((PARAMETER_TYPE)ret) 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret, ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_ioctl() 
{
	if(unlikely(verbose))  
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_ugetrlimit() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getrlimit() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_setrlimit() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_rt_sigaction() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getrusage() 
{
	int ret;
	ADDRESS_TYPE buf_addr;
	PARAMETER_TYPE who;
	who = GetSystemCallParam(0);
	buf_addr = GetSystemCallParam(1);

	if ( (system == "arm") || (system == "arm-eabi") )
	{
		struct arm_rusage_t target_ru;
		ret = GetRUsage(who, &target_ru);
		if ( ret >= 0 )
		{
			WriteMem(buf_addr, &target_ru, sizeof(target_ru));
		}
	}
	else if (system == "powerpc")
	{
		struct powerpc_rusage_t target_ru;
		ret = GetRUsage(who, &target_ru);
		if ( ret >= 0 )
		{
			WriteMem(buf_addr, &target_ru, sizeof(target_ru));
		}
	}
	else ret = -1;

	if(unlikely(verbose))
		logger << DebugInfo
			<< "GetRUsage(who=" << who << ", buf=0x" << hex << buf_addr << dec << ") return " << ret 
			<< endl 
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret, ret != -1);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_unlink() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_rename() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_time() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_gettimeofday()
{
	int ret;
	ADDRESS_TYPE buf_addr;
	buf_addr = GetSystemCallParam(0);

	if ( (system == "arm") || (system == "arm-eabi") )
	{
		struct arm_timeval_t target_tv;
		ret = GetTimeOfDay(&target_tv);
		if ( ret >= 0 )
		{
			WriteMem(buf_addr, &target_tv, sizeof(target_tv));
		}
	}
	else if (system == "powerpc")
	{
		struct powerpc_timeval_t target_tv;
		ret = GetTimeOfDay(&target_tv);
		if ( ret >= 0 )
		{
			WriteMem(buf_addr, &target_tv, sizeof(target_tv));
		}
	}
	else ret = -1;

	if(unlikely(verbose))
		logger << DebugInfo
			<< "GetTimeOfDay(buf=0x" << hex << buf_addr << dec << ") return " << ret 
			<< endl 
			<< LOCATION
			<< EndDebugInfo;
	SetSystemCallStatus(ret, ret != -1);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_socketcall() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_rt_sigprocmask() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_kill() 
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec 
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)0, false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_ftruncate()
{
	int ret;
	
	ret = ftruncate(GetSystemCallParam(0), GetSystemCallParam(1));

	if(unlikely(verbose)) 
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec 
			<< endl << LOCATION << EndDebugInfo;

	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_breakpoint()
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_cacheflush()
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_usr26()
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_usr32()
{
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_arm_set_tls()
{
	uint32_t r0 = GetSystemCallParam(0);
	memory_injection_import->InjectWriteMemory(0xffff0ff0UL,
			(void *)&(r0), 4);
	if(unlikely(verbose))
		logger << DebugInfo
			<< "ret = 0x" << hex << ((PARAMETER_TYPE)0) << dec
			<< endl << LOCATION << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)0, true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetSyscallNameMap()
{
	syscall_name_map[string("unknown")] = &thistype::LSC_unknown;
	syscall_name_map[string("exit")] = &thistype::LSC_exit;
	syscall_name_map[string("read")] = &thistype::LSC_read;
	syscall_name_map[string("write")] = &thistype::LSC_write;
	syscall_name_map[string("open")] = &thistype::LSC_open;
	syscall_name_map[string("close")] = &thistype::LSC_close;
	syscall_name_map[string("lseek")] = &thistype::LSC_lseek;
	syscall_name_map[string("getpid")] = &thistype::LSC_getpid;
	syscall_name_map[string("getuid")] = &thistype::LSC_getuid;
	syscall_name_map[string("access")] = &thistype::LSC_access;
	syscall_name_map[string("times")] = &thistype::LSC_times;
	syscall_name_map[string("brk")] = &thistype::LSC_brk;
	syscall_name_map[string("getgid")] = &thistype::LSC_getgid;
	syscall_name_map[string("geteuid")] = &thistype::LSC_geteuid;
	syscall_name_map[string("getegid")] = &thistype::LSC_getegid;
	syscall_name_map[string("munmap")] = &thistype::LSC_munmap;
	syscall_name_map[string("stat")] = &thistype::LSC_stat;
	syscall_name_map[string("fstat")] = &thistype::LSC_fstat;
	syscall_name_map[string("uname")] = &thistype::LSC_uname;
	syscall_name_map[string("llseek")] = &thistype::LSC_llseek;
	syscall_name_map[string("writev")] = &thistype::LSC_writev;
	syscall_name_map[string("mmap")] = &thistype::LSC_mmap;
	syscall_name_map[string("mmap2")] = &thistype::LSC_mmap2;
	syscall_name_map[string("stat64")] = &thistype::LSC_stat64;
	syscall_name_map[string("fstat64")] = &thistype::LSC_fstat64;
	syscall_name_map[string("getuid32")] = &thistype::LSC_getuid32;
	syscall_name_map[string("getgid32")] = &thistype::LSC_getgid32;
	syscall_name_map[string("geteuid32")] = &thistype::LSC_geteuid32;
	syscall_name_map[string("getegid32")] = &thistype::LSC_getegid32;
	syscall_name_map[string("fcntl64")] = &thistype::LSC_fcntl64;
	syscall_name_map[string("flistxattr")] = &thistype::LSC_flistxattr;
	syscall_name_map[string("exit_group")] = &thistype::LSC_exit_group;
	syscall_name_map[string("fcntl")] = &thistype::LSC_fcntl;
	syscall_name_map[string("dup")] = &thistype::LSC_dup;
	syscall_name_map[string("ioctl")] = &thistype::LSC_ioctl;
	syscall_name_map[string("ugetrlimit")] = &thistype::LSC_ugetrlimit;
	syscall_name_map[string("getrlimit")] = &thistype::LSC_getrlimit;
	syscall_name_map[string("setrlimit")] = &thistype::LSC_setrlimit;
	syscall_name_map[string("rt_sigaction")] = &thistype::LSC_rt_sigaction;
	syscall_name_map[string("getrusage")] = &thistype::LSC_getrusage;
	syscall_name_map[string("unlink")] = &thistype::LSC_unlink;
	syscall_name_map[string("rename")] = &thistype::LSC_rename;
	syscall_name_map[string("time")] = &thistype::LSC_time;
	syscall_name_map[string("gettimeofday")] = &thistype::LSC_gettimeofday;
	syscall_name_map[string("socketcall")] = &thistype::LSC_socketcall;
	syscall_name_map[string("rt_sigprocmask")] = &thistype::LSC_rt_sigprocmask;
	syscall_name_map[string("kill")] = &thistype::LSC_kill;
	syscall_name_map[string("ftruncate")] = &thistype::LSC_ftruncate;
	// the following are arm private system calls
	if ((system.compare("arm") == 0) || (system.compare("arm-eabi") == 0))
	{
		syscall_name_map[string("breakpoint")] = &thistype::LSC_arm_breakpoint;
		syscall_name_map[string("cacheflush")] = &thistype::LSC_arm_cacheflush;
		syscall_name_map[string("usr26")] = &thistype::LSC_arm_usr26;
		syscall_name_map[string("usr32")] = &thistype::LSC_arm_usr32;
		syscall_name_map[string("set_tls")] = &thistype::LSC_arm_set_tls;
	}
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetSyscallNumber(int id) 
{
	if ( system == "arm" )
		return ARMGetSyscallNumber(id);
	else if ( system == "arm-eabi" )
		return ARMEABIGetSyscallNumber(id);
	else
		return PPCGetSyscallNumber(id);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMGetSyscallNumber(int id) 
{
	int translated_id = id - 0x0900000;
	return translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMEABIGetSyscallNumber(int id) 
{
	// the arm eabi ignores the given id and uses register 7
	//   as the id and translated id
	uint32_t translated_id = 0;
	arm_regs[7]->GetValue(&translated_id);
	return (int)translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCGetSyscallNumber(int id) 
{
	return id;
}

// template<class ADDRESS_TYPE, class PARAMETER_TYPE>
// void *
// LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
// PPCConvertStat(struct stat *s) {
//     // this implementation supposes that the host is a x86 with __USE_LARGEFILE64
//     //   and __USE_MISC
//     struct powerpc_stat_t *res;
//       
//     res = (powerpc_stat_t *)malloc(sizeof(powerpc_stat_t));
//     if(GetEndianess() == E_BIG_ENDIAN) {
// #if defined(__x86_64)
// 		res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
//         res->st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
//         res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
//         res->st_nlink = Host2Target(endianess, (uint64_t)s->st_nlink);
//         res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
//         res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
//         res->st_rdev = Host2Target(endianess, (int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2Target(endianess, (int64_t)s->st_size);
//         res->st_blksize = Host2Target(endianess, (int64_t)s->st_blksize);
//         res->st_blocks = Host2Target(endianess, (int64_t)s->st_blocks);
//         res->st_atim.tv_sec = Host2Target(endianess, (int64_t)s->st_atim.tv_sec);
//         res->st_atim.tv_nsec = Host2Target(endianess, (int64_t)s->st_atim.tv_nsec);
//         res->st_mtim.tv_sec = Host2Target(endianess, (int64_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = Host2Target(endianess, (int64_t)s->st_mtim.tv_nsec);
//         res->st_ctim.tv_sec = Host2Target(endianess, (int64_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = Host2Target(endianess, (int64_t)s->st_ctim.tv_nsec);
// #else
//         res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
//         res->st_ino = Host2Target(endianess, (uint32_t)s->st_ino);
//         res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
//         res->st_nlink = Host2Target(endianess, (uint32_t)s->st_nlink);
//         res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
//         res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
//         res->st_rdev = Host2Target(endianess, (int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2Target(endianess, (int64_t)s->st_size);
// #if defined(_WIN32) || defined(_WIN64)
//         res->st_blksize = 512;
//         res->st_blocks = s->st_size / 512;
// #else
//         res->st_blksize = Host2Target(endianess, (int32_t)s->st_blksize);
//         res->st_blocks = Host2Target(endianess, (int64_t)s->st_blocks);
// #endif
//         res->st_atim.tv_sec = Host2Target(endianess, (int32_t)s->st_atim);
//         res->st_atim.tv_nsec = 0;
//         res->st_mtim.tv_sec = Host2Target(endianess, (int32_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = 0;
//         res->st_ctim.tv_sec = Host2Target(endianess, (int32_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = 0;
// #endif
// 	} else {
// #if defined(__x86_64)
//         res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
//         res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
//         res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
//         res->st_nlink = Host2LittleEndian((uint64_t)s->st_nlink);
//         res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
//         res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
//         res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2LittleEndian((int64_t)s->st_size);
//         res->st_blksize = Host2LittleEndian((int64_t)s->st_blksize);
//         res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
//         res->st_atim.tv_sec = Host2LittleEndian((int64_t)s->st_atim.tv_sec);
//         res->st_atim.tv_nsec = Host2LittleEndian((int64_t)s->st_atim.tv_nsec);
//         res->st_mtim.tv_sec = Host2LittleEndian((int64_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = Host2LittleEndian((int64_t)s->st_mtim.tv_nsec);
//         res->st_ctim.tv_sec = Host2LittleEndian((int64_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = Host2LittleEndian((int64_t)s->st_ctim.tv_nsec);
// #else
//         res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
//         res->st_ino = Host2LittleEndian((uint32_t)s->st_ino);
//         res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
//         res->st_nlink = Host2LittleEndian((uint32_t)s->st_nlink);
//         res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
//         res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
//         res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2LittleEndian((int64_t)s->st_size);
//         res->st_blksize = Host2LittleEndian((int32_t)s->st_blksize);
//         res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
//         res->st_atim.tv_sec = Host2LittleEndian((int32_t)s->st_atim.tv_sec);
//         res->st_atim.tv_nsec = Host2LittleEndian((int32_t)s->st_atim.tv_nsec);
//         res->st_mtim.tv_sec = Host2LittleEndian((int32_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = Host2LittleEndian((int32_t)s->st_mtim.tv_nsec);
//         res->st_ctim.tv_sec = Host2LittleEndian((int32_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = Host2LittleEndian((int32_t)s->st_ctim.tv_nsec);
// #endif
//     }
//     res->__unused4 = 0;
//     res->__unused5 = 0;
//     return (void *)res;
// }
/*
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMConvertStat64(struct stat64 *s) {
	// this implementation supposes that the host is a x86 with __USE_LARGEFILE64
	//   and __USE_MISC
  
	struct arm_stat64_t *res;
  
	res = (arm_stat64_t *)malloc(sizeof(arm_stat64_t));
	if(GetEndianess() == E_BIG_ENDIAN) {
#if defined(__x86_64)
		res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
		res->__pad1 = 0;
	    res->__st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
	    res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
	    res->st_nlink = Host2Target(endianess, (uint64_t)s->st_nlink);
	    res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
	    res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
	    res->st_rdev = Host2Target(endianess, (uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2Target(endianess, (uint64_t)s->st_size);
	    res->st_blksize = Host2Target(endianess, (uint32_t)s->st_blksize);
	    res->st_blocks = Host2Target(endianess, (uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2Target(endianess, (uint64_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2Target(endianess, (uint64_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2Target(endianess, (uint64_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2Target(endianess, (uint64_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2Target(endianess, (uint64_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2Target(endianess, (uint64_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
#else
	    res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
	    res->__pad1 = 0;
	    res->__st_ino = Host2Target(endianess, (uint32_t)s->__st_ino);
	    res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
	    res->st_nlink = Host2Target(endianess, (uint32_t)s->st_nlink);
	    res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
	    res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
	    res->st_rdev = Host2Target(endianess, (uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2Target(endianess, (uint64_t)s->st_size);
	    res->st_blksize = Host2Target(endianess, (uint32_t)s->st_blksize);
	    res->st_blocks = Host2Target(endianess, (uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2Target(endianess, (uint32_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2Target(endianess, (uint32_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2Target(endianess, (uint32_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2Target(endianess, (uint32_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2Target(endianess, (uint32_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2Target(endianess, (uint32_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
#endif
	} else {
#if defined(__x86_64)
	    res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
	    res->__pad1 = 0;
	    res->__st_ino = Host2LittleEndian((uint64_t)s->st_ino);
	    res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
	    res->st_nlink = Host2LittleEndian((uint64_t)s->st_nlink);
	    res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
	    res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
	    res->st_rdev = Host2LittleEndian((uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2LittleEndian((uint64_t)s->st_size);
	    res->st_blksize = Host2LittleEndian((uint32_t)s->st_blksize);
	    res->st_blocks = Host2LittleEndian((uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2LittleEndian((uint64_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2LittleEndian((uint64_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2LittleEndian((uint64_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2LittleEndian((uint64_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2LittleEndian((uint64_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2LittleEndian((uint64_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
#else
	    res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
	    res->__pad1 = 0;
	    res->__st_ino = Host2LittleEndian((uint32_t)s->__st_ino);
	    res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
	    res->st_nlink = Host2LittleEndian((uint32_t)s->st_nlink);
	    res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
	    res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
	    res->st_rdev = Host2LittleEndian((uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2LittleEndian((uint64_t)s->st_size);
	    res->st_blksize = Host2LittleEndian((uint32_t)s->st_blksize);
	    res->st_blocks = Host2LittleEndian((uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2LittleEndian((uint32_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2LittleEndian((uint32_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2LittleEndian((uint32_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2LittleEndian((uint32_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2LittleEndian((uint32_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2LittleEndian((uint32_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
#endif
	}
	return (void *)res;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCConvertStat64(struct stat64 *s) {
    // this implementation supposes that the host is a x86 with __USE_LARGEFILE64
    //   and __USE_MISC
    
	struct powerpc_stat64_t *res;
    
    res = (powerpc_stat64_t *)malloc(sizeof(powerpc_stat64_t));
    if(GetEndianess() == E_BIG_ENDIAN) {
#if defined(__x86_64)
    	res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
      	res->st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
      	res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
      	res->st_nlink = Host2Target(endianess, (uint64_t)s->st_nlink);
      	res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
      	res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
      	res->st_rdev = Host2Target(endianess, (int64_t)s->st_rdev);
      	res->__pad2 = 0;
      	res->st_size = Host2Target(endianess, (int64_t)s->st_size);
      	res->st_blksize = Host2Target(endianess, (int32_t)s->st_blksize);
      	res->st_blocks = Host2Target(endianess, (int64_t)s->st_blocks);
      	res->st_atim.tv_sec = Host2Target(endianess, (int64_t)s->st_atim.tv_sec);
      	res->st_atim.tv_nsec = Host2Target(endianess, (int64_t)s->st_atim.tv_nsec);
      	res->st_mtim.tv_sec = Host2Target(endianess, (int64_t)s->st_mtim.tv_sec);
		res->st_mtim.tv_nsec = Host2Target(endianess, (int64_t)s->st_mtim.tv_nsec);
		res->st_ctim.tv_sec = Host2Target(endianess, (int64_t)s->st_ctim.tv_sec);
		res->st_ctim.tv_nsec = Host2Target(endianess, (int64_t)s->st_ctim.tv_nsec);
#else
		res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
      	res->st_ino = Host2Target(endianess, (uint32_t)s->__st_ino);
      	res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
      	res->st_nlink = Host2Target(endianess, (uint32_t)s->st_nlink);
      	res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
      	res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
      	res->st_rdev = Host2Target(endianess, (int64_t)s->st_rdev);
      	res->__pad2 = 0;
      	res->st_size = Host2Target(endianess, (int64_t)s->st_size);
      	res->st_blksize = Host2Target(endianess, (int32_t)s->st_blksize);
      	res->st_blocks = Host2Target(endianess, (int64_t)s->st_blocks);
      	res->st_atim.tv_sec = Host2Target(endianess, (int32_t)s->st_atim.tv_sec);
      	res->st_atim.tv_nsec = Host2Target(endianess, (int32_t)s->st_atim.tv_nsec);
      	res->st_mtim.tv_sec = Host2Target(endianess, (int32_t)s->st_mtim.tv_sec);
      	res->st_mtim.tv_nsec = Host2Target(endianess, (int32_t)s->st_mtim.tv_nsec);
      	res->st_ctim.tv_sec = Host2Target(endianess, (int32_t)s->st_ctim.tv_sec);
      	res->st_ctim.tv_nsec = Host2Target(endianess, (int32_t)s->st_ctim.tv_nsec);
#endif
    } else {
#if defined(__x86_64)
    	res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
      	res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
      	res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
      	res->st_nlink = Host2LittleEndian((uint64_t)s->st_nlink);
      	res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
      	res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
      	res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
      	res->__pad2 = 0;
      	res->st_size = Host2LittleEndian((int64_t)s->st_size);
      	res->st_blksize = Host2LittleEndian((int32_t)s->st_blksize);
      	res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
      	res->st_atim.tv_sec = Host2LittleEndian((int64_t)s->st_atim.tv_sec);
		res->st_atim.tv_nsec = Host2LittleEndian((int64_t)s->st_atim.tv_nsec);
		res->st_mtim.tv_sec = Host2LittleEndian((int64_t)s->st_mtim.tv_sec);
		res->st_mtim.tv_nsec = Host2LittleEndian((int64_t)s->st_mtim.tv_nsec);
		res->st_ctim.tv_sec = Host2LittleEndian((int64_t)s->st_ctim.tv_sec);
		res->st_ctim.tv_nsec = Host2LittleEndian((int64_t)s->st_ctim.tv_nsec);
#else
		res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
		res->st_ino = Host2LittleEndian((uint32_t)s->__st_ino);
		res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
		res->st_nlink = Host2LittleEndian((uint32_t)s->st_nlink);
		res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
		res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
		res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
		res->__pad2 = 0;
		res->st_size = Host2LittleEndian((int64_t)s->st_size);
		res->st_blksize = Host2LittleEndian((int32_t)s->st_blksize);
		res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
		res->st_atim.tv_sec = Host2LittleEndian((int32_t)s->st_atim.tv_sec);
		res->st_atim.tv_nsec = Host2LittleEndian((int32_t)s->st_atim.tv_nsec);
		res->st_mtim.tv_sec = Host2LittleEndian((int32_t)s->st_mtim.tv_sec);
		res->st_mtim.tv_nsec = Host2LittleEndian((int32_t)s->st_mtim.tv_nsec);
		res->st_ctim.tv_sec = Host2LittleEndian((int32_t)s->st_ctim.tv_sec);
		res->st_ctim.tv_nsec = Host2LittleEndian((int32_t)s->st_ctim.tv_nsec);
#endif
    }
    res->__unused4 = 0;
    res->__unused5 = 0;
    return (void *)res;
}
  
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMConvertTms(struct tms *t) {
	struct arm_tms_t *res;
  
	res = (arm_tms_t *)malloc(sizeof(arm_tms_t));
	if(GetEndianess() == E_BIG_ENDIAN) {
		res->tms_utime = Host2Target(endianess, (uint32_t)t->tms_utime);
		res->tms_stime = Host2Target(endianess, (uint32_t)t->tms_stime);
		res->tms_cutime = Host2Target(endianess, (uint32_t)t->tms_cutime);
		res->tms_cstime = Host2Target(endianess, (uint32_t)t->tms_cstime);
	} else {
		res->tms_utime = Host2LittleEndian((uint32_t)t->tms_utime);
		res->tms_stime = Host2LittleEndian((uint32_t)t->tms_stime);
		res->tms_cutime = Host2LittleEndian((uint32_t)t->tms_cutime);
		res->tms_cstime = Host2LittleEndian((uint32_t)t->tms_cstime);
	}
  
	return (void *)res;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCConvertTms(struct tms *t) {
    struct powerpc_tms_t *res;
    
    res = (powerpc_tms_t *)malloc(sizeof(powerpc_tms_t));
    if(GetEndianess() == E_BIG_ENDIAN) {
		res->tms_utime = Host2Target(endianess, (int32_t)t->tms_utime);
		res->tms_stime = Host2Target(endianess, (int32_t)t->tms_stime);
		res->tms_cutime = Host2Target(endianess, (int32_t)t->tms_cutime);
		res->tms_cstime = Host2Target(endianess, (int32_t)t->tms_cstime);
    } else {
		res->tms_utime = Host2LittleEndian((int32_t)t->tms_utime);
		res->tms_stime = Host2LittleEndian((int32_t)t->tms_stime);
		res->tms_cutime = Host2LittleEndian((int32_t)t->tms_cutime);
		res->tms_cstime = Host2LittleEndian((int32_t)t->tms_cstime);
    }
    
    return (void *)res;
}*/
  

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetMmapBase() const 
{
	return mmap_base;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetMmapBase(ADDRESS_TYPE base) 
{
	mmap_base = base;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetMmapBrkPoint() const 
{
	return mmap_brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetMmapBrkPoint(ADDRESS_TYPE brk_point) 
{
	mmap_brk_point = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetBrkPoint() const 
{
	return brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetBrkPoint(ADDRESS_TYPE brk_point) 
{
	this->brk_point = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetSystemCallParam(int id) 
{
	if ( system == "arm" )
		return ARMGetSystemCallParam(id);
	else if ( system == "arm-eabi" )
		return ARMEABIGetSystemCallParam(id);
	else
		return PPCGetSystemCallParam(id);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMGetSystemCallParam(int id) 
{
	PARAMETER_TYPE val;
	arm_regs[id]->GetValue(&val);
	return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMEABIGetSystemCallParam(int id) 
{
	PARAMETER_TYPE val;
	arm_regs[id]->GetValue(&val);
	return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCGetSystemCallParam(int id) 
{
    PARAMETER_TYPE val;
    ppc_regs[id+3]->GetValue(&val);
    return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetSystemCallStatus(int ret, bool error) 
{
	if ( system == "arm" )
		ARMSetSystemCallStatus(ret, error);
	else if ( system == "arm-eabi" )
		ARMEABISetSystemCallStatus(ret, error);
	else
		PPCSetSystemCallStatus(ret, error);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMSetSystemCallStatus(int ret, bool error) 
{
	PARAMETER_TYPE val = (PARAMETER_TYPE)ret;
	arm_regs[0]->SetValue(&val);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMEABISetSystemCallStatus(int ret, bool error) 
{
	PARAMETER_TYPE val = (PARAMETER_TYPE)ret;
	arm_regs[0]->SetValue(&val);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCSetSystemCallStatus(int ret, bool error) 
{
    PARAMETER_TYPE val;
    if (error) 
	{
		ppc_cr->GetValue(&val);
		val |= (1 << 28); // CR0[SO] <- 1
      	ppc_cr->SetValue(&val);
    }
 	else
	{
		ppc_cr->GetValue(&val);
		val &= ~(1 << 28); // CR0[SO] <- 0
		ppc_cr->SetValue(&val);
    }
    val = (PARAMETER_TYPE)ret;
    ppc_regs[3]->SetValue(&val);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
endian_type
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetEndianess()
{
	return endianess;
}
//
//void 
//ARMLinuxOS::Reset() {
//	return cpu_arm_linux_os_import->Reset();
//}
//
//bool 
//ARMLinuxOS::ReadMemory(ADDRESS_TYPE addr, void *buffer, uint32_t size) {
//	return cpu_arm_linux_os_import->ReadMemory(addr, buffer, size);
//}
//
//bool 
//ARMLinuxOS::WriteMemory(ADDRESS_TYPE addr, const void *buffer, uint32_t size) {
//	return cpu_arm_linux_os_import->WriteMemory(addr, buffer, size);
//}
//
} // end of linux_os namespace
} // end of os namespace
} // end of service namespace
} // end of unisim namespace

#undef LOCATION

#endif // __UNISIM_SERVICE_OS_LINUX_OS_LINUX_OS_TCC__
