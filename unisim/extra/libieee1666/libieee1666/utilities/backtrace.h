/*
 *  Copyright (c) 2008,
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
 
#ifndef __LIBIEEE1666_UTILITIES_BACKTRACE_H__
#define __LIBIEEE1666_UTILITIES_BACKTRACE_H__

#include <iosfwd>

#if __LIBIEEE1666_BACKTRACE__

#include "utilities/sysdep/backtrace.h"

#else

namespace sc_core {

class sc_backtrace;

inline std::ostream& operator << (std::ostream& os, const sc_backtrace& backtrace);

class sc_backtrace
{
public:
	inline sc_backtrace(unsigned int max_depth = 32)
	inline ~sc_backtrace()

	friend std::ostream& operator << (std::ostream& os, const sc_backtrace& backtrace);
private:
};

inline sc_backtrace::sc_backtrace(unsigned int max_depth)
{
}

inline sc_backtrace::~sc_backtrace()
{
}

inline std::ostream& operator << (std::ostream& os, const sc_backtrace& backtrace)
{
	return os << "no backtrace available";
}

} // end of namespace sc_core

#endif

#endif
