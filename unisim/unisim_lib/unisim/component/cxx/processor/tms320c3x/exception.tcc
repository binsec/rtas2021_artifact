/*
 *  Copyright (c) 2009,
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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_EXCEPTION_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_EXCEPTION_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320c3x {

template<class CONFIG, bool DEBUG>
InternalErrorException<CONFIG, DEBUG>::InternalErrorException(const char *msg)
{
	stringstream sstr;
	sstr << "Internal error! " << msg;
	what_str = sstr.str();
}

template<class CONFIG, bool DEBUG>
InternalErrorException<CONFIG, DEBUG>::~InternalErrorException() throw ()
{
}

template<class CONFIG, bool DEBUG>
const char * InternalErrorException<CONFIG, DEBUG>::what () const throw ()
{
	return what_str.c_str();
}

template<class CONFIG, bool DEBUG>
UnknownOpcodeException<CONFIG, DEBUG>::UnknownOpcodeException(isa::tms320c3x::Operation<CONFIG, DEBUG> *_operation)
	: operation(_operation)
{
	stringstream sstr;
	sstr << "Unknown opcode";
	if(operation)
	{
		sstr << " 0x" << hex << operation->GetEncoding() << " at 0x" << (operation->GetAddr() / 4);
	}
	what_str = sstr.str();
}

template<class CONFIG, bool DEBUG>
UnknownOpcodeException<CONFIG, DEBUG>::~UnknownOpcodeException() throw ()
{
}

template<class CONFIG, bool DEBUG>
const char * UnknownOpcodeException<CONFIG, DEBUG>::what () const throw ()
{
	return what_str.c_str();
}

template<class CONFIG, bool DEBUG>
isa::tms320c3x::Operation<CONFIG, DEBUG> *UnknownOpcodeException<CONFIG, DEBUG>::GetOperation() const
{
	return operation;
}

template<class CONFIG, bool DEBUG>
UnimplementedOpcodeException<CONFIG, DEBUG>::UnimplementedOpcodeException(isa::tms320c3x::Operation<CONFIG, DEBUG> *_operation)
	: operation(_operation)
{
	stringstream sstr;
	sstr << "Unimplemented opcode";
	if(operation)
	{
		sstr << " 0x" << hex << operation->GetEncoding() << " (\"" << operation->GetName() << "\") at 0x" << (operation->GetAddr() / 4);
	}
	what_str = sstr.str();
}

template<class CONFIG, bool DEBUG>
UnimplementedOpcodeException<CONFIG, DEBUG>::~UnimplementedOpcodeException() throw ()
{
}

template<class CONFIG, bool DEBUG>
const char * UnimplementedOpcodeException<CONFIG, DEBUG>::what () const throw ()
{
	return what_str.c_str();
}

template<class CONFIG, bool DEBUG>
isa::tms320c3x::Operation<CONFIG, DEBUG> *UnimplementedOpcodeException<CONFIG, DEBUG>::GetOperation() const
{
	return operation;
}

template<class CONFIG, bool DEBUG>
BogusOpcodeException<CONFIG, DEBUG>::BogusOpcodeException(isa::tms320c3x::Operation<CONFIG, DEBUG> *_operation)
	: operation(_operation)
{
	stringstream sstr;
	sstr << "Bogus opcode";
	if(operation)
	{
		sstr << " 0x" << hex << operation->GetEncoding() << " (\"" << operation->GetName() << "\") at 0x" << (operation->GetAddr() / 4);
	}
	what_str = sstr.str();
}

template<class CONFIG, bool DEBUG>
BogusOpcodeException<CONFIG, DEBUG>::~BogusOpcodeException() throw ()
{
}

template<class CONFIG, bool DEBUG>
const char * BogusOpcodeException<CONFIG, DEBUG>::what () const throw ()
{
	return what_str.c_str();
}

template<class CONFIG, bool DEBUG>
isa::tms320c3x::Operation<CONFIG, DEBUG> *BogusOpcodeException<CONFIG, DEBUG>::GetOperation() const
{
	return operation;
}

template<class CONFIG, bool DEBUG>
MisplacedOpcodeException<CONFIG, DEBUG>::MisplacedOpcodeException(isa::tms320c3x::Operation<CONFIG, DEBUG> *_operation)
	: operation(_operation)
{
	stringstream sstr;
	sstr << "Misplaced opcode";
	if(operation)
	{
		sstr << " 0x" << hex << operation->GetEncoding() << " (\"" << operation->GetName() << "\") at 0x" << (operation->GetAddr() / 4);
	}
	what_str = sstr.str();
}

template<class CONFIG, bool DEBUG>
MisplacedOpcodeException<CONFIG, DEBUG>::~MisplacedOpcodeException() throw ()
{
}

template<class CONFIG, bool DEBUG>
const char * MisplacedOpcodeException<CONFIG, DEBUG>::what () const throw ()
{
	return what_str.c_str();
}

template<class CONFIG, bool DEBUG>
isa::tms320c3x::Operation<CONFIG, DEBUG> *MisplacedOpcodeException<CONFIG, DEBUG>::GetOperation() const
{
	return operation;
}

template<class CONFIG, bool DEBUG>
BadMemoryAccessException<CONFIG, DEBUG>::BadMemoryAccessException(typename CONFIG::address_t _addr)
	: addr(_addr)
{
	stringstream sstr;
	sstr << "Bad memory access at 0x" << hex << addr;
}

template<class CONFIG, bool DEBUG>
BadMemoryAccessException<CONFIG, DEBUG>::~BadMemoryAccessException() throw ()
{
}

template<class CONFIG, bool DEBUG>
const char * BadMemoryAccessException<CONFIG, DEBUG>::what () const throw ()
{
	return what_str.c_str();
}

template<class CONFIG, bool DEBUG>
typename CONFIG::address_t BadMemoryAccessException<CONFIG, DEBUG>::GetAddr() const
{
	return addr;
}

} // end of namespace tms320c3x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_EXCEPTION_TCC__
