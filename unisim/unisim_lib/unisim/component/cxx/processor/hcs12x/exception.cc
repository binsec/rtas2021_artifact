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
 *          Reda   Nouacer (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_EXCEPTION_CC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_EXCEPTION_CC__

#include <unisim/component/cxx/processor/hcs12x/exception.hh>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {


const char * AsynchronousException::what () const throw ()
{
	return ("Asynchronous exception");
}

TrapException::TrapException(uint8_t trapnum)
{
	_trapnum = trapnum;
}

const char * TrapException::what () const throw ()
{
	return ("Unimplemented opcode trap");
}

NonMaskableSWIInterrupt::NonMaskableSWIInterrupt()
{
}

const char * NonMaskableSWIInterrupt::what () const throw ()
{
	return ("A software interrupt instruction (SWI) or BDM vector request");
}

SysCallInterrupt::SysCallInterrupt()
{
}

const char * SysCallInterrupt::what () const throw ()
{
	return ("A system call interrupt instruction (SYS) (CPU12XV1 and CPU12XV2 only)");
}

NonMaskableAccessErrorInterrupt::NonMaskableAccessErrorInterrupt(ERROR_TYPE error)
{
	errorType = error;
}

const char * NonMaskableAccessErrorInterrupt::what () const throw ()
{
	std::stringstream strStm;

	strStm << "Access Error: Invalid ";

	switch (errorType) {
	case NonMaskableAccessErrorInterrupt::INVALIDE_RPAGE: strStm << "RPAGE value"; break;
	case NonMaskableAccessErrorInterrupt::INVALIDE_EPAGE: strStm << "EPAGE value"; break;
	case NonMaskableAccessErrorInterrupt::INVALIDE_PPAGE: strStm << "PPAGE value"; break;
	}

	return (strStm.str().c_str());
}

SpuriousInterrupt::SpuriousInterrupt()
{
}

const char * SpuriousInterrupt::what () const throw ()
{
	return ("Spurious interrupt");
}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
