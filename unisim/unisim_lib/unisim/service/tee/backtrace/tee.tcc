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

#ifndef __UNISIM_SERVICE_TEE_BACKTRACE_TEE_TCC__
#define __UNISIM_SERVICE_TEE_BACKTRACE_TEE_TCC__

#include <string>
#include <sstream>

namespace unisim {
namespace service {
namespace tee {
namespace backtrace {

using std::stringstream;
using std::string;

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::Tee(const char *name, Object *parent) :
	Object(name, parent, "This service/client implements a tee ('T'). It unifies the backtrace capability of several services that individually provides their own backtrace capability" ),
	Client<BackTrace<ADDRESS> >(name, parent),
	Service<BackTrace<ADDRESS> >(name, parent),
	backtrace_export("backtrace-export", this)
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		stringstream sstr;
		sstr << "backtrace-import[" << i << "]";
		string import_name = sstr.str();
		backtrace_import[i] = new ServiceImport<BackTrace<ADDRESS> >(import_name.c_str(), this);
		
		backtrace_export.SetupDependsOn(*backtrace_import[i]);
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
Tee<ADDRESS, MAX_IMPORTS>::~Tee()
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(backtrace_import[i]) delete backtrace_import[i];
	}
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
std::vector<ADDRESS> *Tee<ADDRESS, MAX_IMPORTS>::GetBackTrace(ADDRESS pc) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(backtrace_import[i])
		{
			if(*backtrace_import[i])
			{
				std::vector<ADDRESS> *backtrace = (*backtrace_import[i])->GetBackTrace(pc);
				
				if(backtrace) return backtrace;
			}
		}
	}

	return 0;
}

template <class ADDRESS, unsigned int MAX_IMPORTS>
bool Tee<ADDRESS, MAX_IMPORTS>::GetReturnAddress(ADDRESS pc, ADDRESS& ret_addr) const
{
	unsigned int i;
	for(i = 0; i < MAX_IMPORTS; i++)
	{
		if(backtrace_import[i])
		{
			if(*backtrace_import[i])
			{
				if((*backtrace_import[i])->GetReturnAddress(pc, ret_addr)) return true;
			}
		}
	}

	return false;
}

} // end of namespace backrace
} // end of namespace tee
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_TEE_BACKTRACE_TEE_TCC__
