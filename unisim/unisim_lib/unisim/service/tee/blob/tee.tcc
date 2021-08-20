/*
 *  Copyright (c) 2011,
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
 * 
 */

#ifndef __UNISIM_SERVICE_TEE_BLOB_TEE_TCC__
#define __UNISIM_SERVICE_TEE_BLOB_TEE_TCC__

#include <string>
#include <sstream>

namespace unisim {
namespace service {
namespace tee {
namespace blob {

using std::stringstream;
using std::string;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::Tee(const char *name, Object *parent)
	: Object(name, parent, "This service/client implements a tee ('T'). It unifies the blob interface of several services that individually provides their own blob interface" )
	, Client<Blob<ADDRESS> >(name, parent)
	, Service<Blob<ADDRESS> >(name, parent)
	, blob_export("blob-export", this)
	, logger(*this)
	, blob(0)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		stringstream sstr;
		sstr << "blob-import[" << i << "]";
		string import_name = sstr.str();
		blob_import[i] = new ServiceImport<Blob<ADDRESS> >(import_name.c_str(), this);
		
		blob_export.SetupDependsOn(*blob_import[i]);
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::~Tee()
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(blob_import[i]) delete blob_import[i];
	}
	if(blob)
	{
		blob->Release();
		blob = 0;
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
bool Tee<ADDRESS, MAX_IMPORTS>::Setup(ServiceExportBase *srv_export)
{
	if(srv_export == &blob_export) return SetupBlob();
	
	logger << DebugError << "Internal error" << EndDebugError;
	return false;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
bool Tee<ADDRESS, MAX_IMPORTS>::BeginSetup()
{
	if(blob)
	{
		blob->Release();
		blob = 0;
	}
	return true;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
bool Tee<ADDRESS, MAX_IMPORTS>::SetupBlob()
{
	if(blob) return true;

	blob = new unisim::util::blob::Blob<ADDRESS>();
	blob->Catch();
	
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(blob_import[i])
		{
			if(*blob_import[i])
			{
				const unisim::util::blob::Blob<ADDRESS> *lower_blob = (*blob_import[i])->GetBlob();
				if(lower_blob)
				{
					blob->AddBlob(lower_blob);
				}
			}
		}
	}

	return true;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
const unisim::util::blob::Blob<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::GetBlob() const
{
	return blob;
}

} // end of namespace memory_access_reporting
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_BLOB_TEE_TCC__
