/*
 *  Copyright (c) 2007-2019,
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

#ifndef __UNISIM_SERVICE_TEE_CHAR_IO_TEE_HH__
#define __UNISIM_SERVICE_TEE_CHAR_IO_TEE_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/char_io.hh>
#include <sstream>

namespace unisim {
namespace service {
namespace tee {
namespace char_io {

template <unsigned int MAX_IMPORTS = 8>
class Tee
	: public unisim::kernel::Service<unisim::service::interfaces::CharIO>
	, public unisim::kernel::Client<unisim::service::interfaces::CharIO>
{
public:
	unisim::kernel::ServiceExport<unisim::service::interfaces::CharIO> char_io_export;
	unisim::kernel::ServiceImport<unisim::service::interfaces::CharIO> *char_io_import[MAX_IMPORTS];
	
	Tee(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~Tee();
	
	virtual void ResetCharIO();
	virtual bool GetChar(char& c);
	virtual void PutChar(char c);
	virtual void FlushChars();
private:
};

template <unsigned int MAX_IMPORTS>
Tee<MAX_IMPORTS>::Tee(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, unisim::kernel::Service<unisim::service::interfaces::CharIO>(name, parent)
	, unisim::kernel::Client<unisim::service::interfaces::CharIO>(name, parent)
	, char_io_export("char-io-export", this)
	, char_io_import()
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		std::ostringstream char_io_import_sstr;
		char_io_import_sstr << "char_io_import[" << i << "]";
		char_io_import[i] = new unisim::kernel::ServiceImport<unisim::service::interfaces::CharIO>(char_io_import_sstr.str().c_str(), this);
		
		char_io_export.SetupDependsOn(*char_io_import[i]);
	}
}

template <unsigned int MAX_IMPORTS>
Tee<MAX_IMPORTS>::~Tee()
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		delete char_io_import[i];
	}
}

template <unsigned int MAX_IMPORTS>
void Tee<MAX_IMPORTS>::ResetCharIO()
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		unisim::kernel::ServiceImport<unisim::service::interfaces::CharIO>& import = *char_io_import[i];
		if(import)
		{
			import->ResetCharIO();
		}
	}
}

template <unsigned int MAX_IMPORTS>
bool Tee<MAX_IMPORTS>::GetChar(char& c)
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		unisim::kernel::ServiceImport<unisim::service::interfaces::CharIO>& import = *char_io_import[i];
		if(import)
		{
			if(import->GetChar(c)) return true;
		}
	}
	
	return false;
}

template <unsigned int MAX_IMPORTS>
void Tee<MAX_IMPORTS>::PutChar(char c)
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		unisim::kernel::ServiceImport<unisim::service::interfaces::CharIO>& import = *char_io_import[i];
		if(import)
		{
			import->PutChar(c);
		}
	}
}

template <unsigned int MAX_IMPORTS>
void Tee<MAX_IMPORTS>::FlushChars()
{
	for(unsigned int i = 0; i < MAX_IMPORTS; i++)
	{
		unisim::kernel::ServiceImport<unisim::service::interfaces::CharIO>& import = *char_io_import[i];
		if(import)
		{
			import->FlushChars();
		}
	}
}

} // end of namespace char_io
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_CHAR_IO_TEE_HH__
