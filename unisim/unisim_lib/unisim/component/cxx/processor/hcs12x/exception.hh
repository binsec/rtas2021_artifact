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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 *          Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_EXCEPTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_EXCEPTION_HH__

#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <sstream>
#include <stdexcept>
#include <string>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using std::string;

class Exception : public std::exception {};

// Asynchronous exception group : Resets, I-bit, XIRQ and IRQ interrupts.
// The CPU12 isn't aware of what kind of Hardware interrupt happened.
// Only the XINT module has this ability and when the CPU12 issue a request it return the Hw-interrupt vector.
class AsynchronousException : public Exception
{
public:
	AsynchronousException() {};
	virtual ~AsynchronousException() throw (){}
	virtual const char * what () const throw ();
};

// Unimplemented opcode trap
class TrapException : public Exception
{
public:
	TrapException(uint8_t trapnum);
	virtual ~TrapException() throw (){}
	virtual const char * what () const throw ();
private:
	uint8_t _trapnum;
};

// A software interrupt instruction (SWI) or BDM vector request
class NonMaskableSWIInterrupt : public Exception
{
public:
	NonMaskableSWIInterrupt();
	virtual ~NonMaskableSWIInterrupt() throw (){}
	virtual const char * what () const throw ();
};

// A system call interrupt instruction (SYS) (CPU12XV1 and CPU12XV2 only)
class SysCallInterrupt : public Exception
{
public:
	SysCallInterrupt();
	virtual ~SysCallInterrupt() throw (){}
	virtual const char * what () const throw ();
};

// Non-maskable access error interrupts
class NonMaskableAccessErrorInterrupt : public Exception
{
public:
	enum ERROR_TYPE {INVALIDE_RPAGE, INVALIDE_EPAGE, INVALIDE_PPAGE};

	NonMaskableAccessErrorInterrupt(ERROR_TYPE error);
	virtual ~NonMaskableAccessErrorInterrupt() throw (){}
	virtual const char * what () const throw ();
	ERROR_TYPE getErrorType() { return (errorType); }

private:
	ERROR_TYPE errorType;
};

// Spurious interrupt
class SpuriousInterrupt : public Exception
{
public:
	SpuriousInterrupt();
	virtual ~SpuriousInterrupt() throw (){}
	virtual const char * what () const throw ();
};

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
