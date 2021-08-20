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
 */

#ifndef __UNISIM_SERVICE_INTERFACES_CHAR_IO_HH__
#define __UNISIM_SERVICE_INTERFACES_CHAR_IO_HH__

#include <unisim/service/interfaces/interface.hh>
#include <string>

namespace unisim {
namespace service {
namespace interfaces {

class CharIO : public ServiceInterface
{
public:
	virtual void ResetCharIO() = 0;
	virtual bool GetChar(char& c) = 0;
	virtual void PutChar(char c) = 0;
	virtual void FlushChars() = 0;
	inline void PutString(const char *s);
	inline void PutString(const std::string& s);
};

inline void CharIO::PutString(const char *s)
{
	char c = *s;
	if(c)
	{
		do
		{
			PutChar(c);
		}
		while((c = *++s) != 0);
	}
}

inline void CharIO::PutString(const std::string& s)
{
	std::size_t n = s.length();
	for(std::size_t i = 0; i < n; i++)
	{
		PutChar(s[i]);
	}
}

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif
