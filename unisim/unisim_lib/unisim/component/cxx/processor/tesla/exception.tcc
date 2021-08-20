/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXCEPTION_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXCEPTION_TCC__

#include <unisim/component/cxx/processor/tesla/exception.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template <class CONFIG>
ProgramException<CONFIG>::ProgramException(const char *name)
{
	stringstream sstr;
	sstr << "Program " << name << " exception";
	what_str = sstr.str();
}

template <class CONFIG>
ProgramException<CONFIG>::~ProgramException() throw()
{
}

template <class CONFIG>
const char * ProgramException<CONFIG>::what () const throw ()
{
	return what_str.c_str();
}

template <class CONFIG>
IllegalInstructionException<CONFIG>::IllegalInstructionException() : ProgramException<CONFIG>("illegal instruction")
{
}


template <class CONFIG>
TrapException<CONFIG>::TrapException() : ProgramException<CONFIG>("trap")
{
}

template <class CONFIG>
TraceException<CONFIG>::TraceException()
{
}

template <class CONFIG>
const char * TraceException<CONFIG>::what () const throw ()
{
	return "Trace exception";
}

template <class CONFIG>
InstructionAddressBreakpointException<CONFIG>::InstructionAddressBreakpointException()
{
}

template <class CONFIG>
const char * InstructionAddressBreakpointException<CONFIG>::what () const throw ()
{
	return "Instruction address breakpoint exception";
}

#if 0

template <class CONFIG>
void CPU<CONFIG>::HandleException(const IllegalInstructionException<CONFIG>& exc)
{
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const TrapException<CONFIG>& exc)
{
}



/* Trace exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const TraceException<CONFIG>& exc)
{
}

/* Instruction Address Breakpoint exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const InstructionAddressBreakpointException<CONFIG>& exc)
{
}

#endif

template <class CONFIG>
MemoryAccessException<CONFIG>::MemoryAccessException()
{
}

template <class CONFIG>
const char * MemoryAccessException<CONFIG>::what () const throw ()
{
	return "Memory access exception";
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
