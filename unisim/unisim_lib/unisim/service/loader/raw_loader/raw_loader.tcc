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
 
#ifndef __UNISIM_SERVICE_LOADER_RAW_LOADER_RAW_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_RAW_LOADER_RAW_LOADER_TCC__

#include <inttypes.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include "unisim/service/loader/raw_loader/raw_loader.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace service {
namespace loader {
namespace raw_loader {

using unisim::kernel::Object;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Blob;
using unisim::service::interfaces::Memory;

template <class MEMORY_ADDR>
RawLoader<MEMORY_ADDR> ::
RawLoader(const char *name, Object *parent)
	: Object(name, parent, "Raw loader")
	, Service<Loader>(name, parent)
	, Service<Blob<MEMORY_ADDR> >(name, parent)
	, Client<Memory<MEMORY_ADDR> >(name, parent)
	, loader_export("loader-export", this)
	, blob_export("blob-export", this)
	, memory_import("memory-import", this)
	, blob(0)
	, filename()
	, base_addr(0)
	, size(0)
	, verbose(false)
	, param_filename("filename", this, filename,
			"Location of the raw file to load.")
	, param_base_addr("base-addr", this, base_addr,
			"Target base address.")
	, param_size("size", this, size,
			"Number of bytes to copy."
			" If 0 then all the file contents will be copied."
			" If smaller than the file, the contents of the file will"
			" be cropped."
			" If bigger than the file size, all the file will be copied.")
	, param_verbose("verbose", this, verbose,
			"Set verbose mode (0 not active, otherwise active).")
	, logger(*this)
{
	loader_export.SetupDependsOn(memory_import);
}

template <class MEMORY_ADDR>
RawLoader<MEMORY_ADDR> ::
~RawLoader()
{
	if(blob)
	{
		blob->Release();
	}
}

template <class MEMORY_ADDR>
bool
RawLoader<MEMORY_ADDR> ::
BeginSetup()
{
	if(blob)
	{
		blob->Release();
		blob = 0;
	}
	
	return true;
}

template <class MEMORY_ADDR>
bool
RawLoader<MEMORY_ADDR> ::
SetupLoad()
{
	return SetupBlob();
}

template <class MEMORY_ADDR>
bool
RawLoader<MEMORY_ADDR> ::
SetupBlob()
{
	if(blob) return true;

	if ( !filename.empty() )
	{
		std::string location =
			Object::GetSimulator()->SearchSharedDataFile(filename.c_str());
			
		if ( verbose )
		{
			logger << DebugInfo << "Opening \"" << location << "\"" << EndDebugInfo;
		}
			
		std::ifstream is(location.c_str(), 
				std::ifstream::in | std::ifstream::binary);
		if ( is.fail() )
		{
			logger << DebugError 
				<< "Can't open input \"" << location << "\"" 
				<< EndDebugError;
			return false;
		}
		
		if ( is.seekg(0, std::ios::end).fail() )
		{
			logger << DebugError
				<< "Can't seek into file \"" << location << "\""
				<< EndDebugError;
			return false;
		}
		
		std::streampos file_size = is.tellg();
		
		if ( file_size < 0 )
		{
			logger << DebugError
				<< "Can't get size of file \"" << location << "\""
				<< EndDebugError;
			return false;
		}
		
		if ( is.seekg(0, std::ios::beg).fail() )
		{
			logger << DebugError
				<< "Can't seek into file \"" << location << "\""
				<< EndDebugError;
			return false;
		}
		
		if ( !size ) size = file_size;

		void *raw_data = calloc(size, 1);
		
		if ( !raw_data )
		{
			logger << DebugError
				<< "Out of memory"
				<< EndDebugError;
			return false;
		}
		
		std::streampos read_size = 
			((MEMORY_ADDR)file_size > size) ? (std::streampos) size : file_size;
		
		if ( verbose )
		{
			logger << DebugInfo << "Loading " << read_size 
				<< " bytes of raw data" << EndDebugInfo;
		}

		if ( is.read((char *) raw_data, read_size).fail() )
		{
			logger << DebugError
				<< "Input/Ouput error while reading file \"" << location << "\""
				<< EndDebugError;
			free(raw_data);
			return false;
		}
		
		blob = new unisim::util::blob::Blob<MEMORY_ADDR>();
		blob->Catch();
		
		blob->SetFilename(location.c_str());

		unisim::util::blob::Section<MEMORY_ADDR> *section = 
			new unisim::util::blob::Section<MEMORY_ADDR>(
					unisim::util::blob::Section<MEMORY_ADDR>::TY_UNKNOWN,
					unisim::util::blob::Section<MEMORY_ADDR>::SA_A,
					"",
					0,
					0,
					base_addr,
					size,
					raw_data
					);

		blob->AddSection(section);
	}
	
	return true;
}

template <class MEMORY_ADDR>
bool
RawLoader<MEMORY_ADDR> ::
Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &loader_export) return SetupLoad();
	if(srv_export == &blob_export) return SetupBlob();
	
	logger << DebugError << "Internal Error" << EndDebugError;
	return false;
}

template <class MEMORY_ADDR>
bool
RawLoader<MEMORY_ADDR> ::
EndSetup()
{
	return Load();
}

template <class MEMORY_ADDR>
bool
RawLoader<MEMORY_ADDR> ::
Load()
{
	if(!blob) return true; // nothing to load
	
	bool success = true;

	if(memory_import)
	{
		const typename std::vector<const unisim::util::blob::Section<MEMORY_ADDR> *>& sections = blob->GetSections();
		typename std::vector<const unisim::util::blob::Section<MEMORY_ADDR> *>::const_iterator section_iter;
		for(section_iter = sections.begin(); section_iter != sections.end(); section_iter++)
		{
			const unisim::util::blob::Section<MEMORY_ADDR> *section = *section_iter;
			
			if(section->GetAttr() & unisim::util::blob::Section<MEMORY_ADDR>::SA_A)
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Writing " << section->GetSize() << " bytes of raw data at @0x" << std::hex << section->GetAddr() << " - @0x" << (section->GetAddr() +  section->GetSize() - 1) << std::dec << EndDebugInfo;
				}
				if(!memory_import->WriteMemory(section->GetAddr(), section->GetData(), section->GetSize()))
				{
					logger << DebugError << "Can't write into memory (@0x" << std::hex << section->GetAddr() << " - @0x" << (section->GetAddr() +  section->GetSize() - 1) << std::dec << ")" << EndDebugError;
					success = false;
				}
			}
		}
	}
	else
	{
		if ( verbose )
		{
			logger << DebugInfo
				<< "Not loading \"" << ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) ? blob->GetFilename() : "")
				<< "\" because the memory is not connected to"
				<< " the loader."
				<< EndDebugInfo;
		}
	}
	return success;
}

template <class MEMORY_ADDR>
const typename unisim::util::blob::Blob<MEMORY_ADDR> *
RawLoader<MEMORY_ADDR> ::
GetBlob()
const
{
	return blob;
}

} // end of namespace raw_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
