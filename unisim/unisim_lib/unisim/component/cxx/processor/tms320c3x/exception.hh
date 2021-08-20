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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_EXCEPTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_EXCEPTION_HH__

#include <stdexcept>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320c3x {

class Exception : public std::exception {};

template <class CONFIG, bool DEBUG>
class InternalErrorException : public Exception
{
public:
	InternalErrorException(const char *msg);
	virtual const char * what () const throw ();
	virtual ~InternalErrorException() throw();
private:
	string what_str;
};

template <class CONFIG, bool DEBUG>
class UnknownOpcodeException : public Exception
{
public:
	UnknownOpcodeException(isa::tms320c3x::Operation<CONFIG, DEBUG> *operation = 0);
	virtual const char * what () const throw ();
	virtual ~UnknownOpcodeException() throw();
	isa::tms320c3x::Operation<CONFIG, DEBUG> *GetOperation() const;
private:
	isa::tms320c3x::Operation<CONFIG, DEBUG> *operation;
	string what_str;
};

template <class CONFIG, bool DEBUG>
class UnimplementedOpcodeException : public Exception
{
public:
	UnimplementedOpcodeException(isa::tms320c3x::Operation<CONFIG, DEBUG> *operation = 0);
	virtual const char * what () const throw ();
	virtual ~UnimplementedOpcodeException() throw();
	isa::tms320c3x::Operation<CONFIG, DEBUG> *GetOperation() const;
private:
	isa::tms320c3x::Operation<CONFIG, DEBUG> *operation;
	string what_str;
};

template <class CONFIG, bool DEBUG>
class BogusOpcodeException : public Exception
{
public:
	BogusOpcodeException(isa::tms320c3x::Operation<CONFIG, DEBUG> *operation = 0);
	virtual const char * what () const throw ();
	virtual ~BogusOpcodeException() throw();
	isa::tms320c3x::Operation<CONFIG, DEBUG> *GetOperation() const;
private:
	isa::tms320c3x::Operation<CONFIG, DEBUG> *operation;
	string what_str;
};

template <class CONFIG, bool DEBUG>
class MisplacedOpcodeException : public Exception
{
public:
	MisplacedOpcodeException(isa::tms320c3x::Operation<CONFIG, DEBUG> *operation = 0);
	virtual const char * what () const throw ();
	virtual ~MisplacedOpcodeException() throw();
	isa::tms320c3x::Operation<CONFIG, DEBUG> *GetOperation() const;
private:
	isa::tms320c3x::Operation<CONFIG, DEBUG> *operation;
	string what_str;
};

template <class CONFIG, bool DEBUG>
class BadMemoryAccessException : public Exception
{
public:
	BadMemoryAccessException(typename CONFIG::address_t addr);
	virtual const char * what () const throw ();
	virtual ~BadMemoryAccessException() throw();
	typename CONFIG::address_t GetAddr() const;
private:
	typename CONFIG::address_t addr;
	string what_str;
};

} // end of namespace tms320c3x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_EXCEPTION_HH__
