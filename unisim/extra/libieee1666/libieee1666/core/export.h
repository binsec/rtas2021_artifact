/*
 *  Copyright (c) 2014,
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

#ifndef __LIBIEEE1666_CORE_EXPORT_H__
#define __LIBIEEE1666_CORE_EXPORT_H__

#include "core/fwd.h"
#include "core/object.h"
#include "core/interface.h"

namespace sc_core {

class sc_export_base : public sc_object
{
protected:
	friend class sc_kernel;
	
	virtual void before_end_of_elaboration();
	virtual void end_of_elaboration();
	virtual void start_of_simulation();
	virtual void end_of_simulation();
	
	////////////////////////////////////////////
	virtual sc_interface* get_interface() = 0;
	virtual const sc_interface* get_interface() const = 0;
protected:
	sc_export_base(const char *name);
	sc_export_base();
	virtual ~sc_export_base();
};

template<class IF>
class sc_export : public sc_export_base
{
public:
	sc_export();
	explicit sc_export( const char* );
	virtual ~sc_export();
	virtual const char* kind() const;
	void operator() ( IF& );
	virtual void bind( IF& );
	operator IF& ();
	operator const IF& () const;
	IF* operator-> ();
	const IF* operator-> () const;
protected:
	IF *interf;
	
	virtual sc_interface* get_interface();
	virtual const sc_interface* get_interface() const;
};

//////////////////////////////////// sc_export<> /////////////////////////////////////////////

template<class IF>
sc_export<IF>::sc_export()
	: sc_export_base()
	, interf(0)
{
}

template<class IF>
sc_export<IF>::sc_export(const char *_name)
	: sc_export_base(_name)
	, interf(0)
{
}

template<class IF>
sc_export<IF>::~sc_export()
{
}

template<class IF>
const char* sc_export<IF>::kind() const
{
	return "sc_export";
}

template<class IF>
void sc_export<IF>::operator () (IF& _if)
{
	bind(_if);
}

template<class IF>
void sc_export<IF>::bind(IF& _if)
{
	interf = &_if;
}

template<class IF>
sc_export<IF>::operator IF& ()
{
	return *interf;
}

template<class IF>
sc_export<IF>::operator const IF& () const
{
	return *(const IF *) interf; 
}

template<class IF>
IF* sc_export<IF>::operator-> ()
{
	return interf;
}

template<class IF>
const IF* sc_export<IF>::operator-> () const
{
	return interf;
}

template<class IF>
sc_interface* sc_export<IF>::get_interface()
{
	return interf;
}

template<class IF>
const sc_interface* sc_export<IF>::get_interface() const
{
	return interf;
}

} // end of namespace sc_core

#endif
