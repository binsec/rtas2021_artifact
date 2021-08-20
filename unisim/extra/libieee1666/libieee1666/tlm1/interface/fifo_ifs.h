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

#ifndef __LIBIEEE1666_TLM1_INTERFACE_FIFO_IFS_H__
#define __LIBIEEE1666_TLM1_INTERFACE_FIFO_IFS_H__

#include <tlm1/fwd.h>
#include <systemc>

namespace tlm {

// Fifo debug interface
template <typename T>
class tlm_fifo_debug_if : public virtual sc_core::sc_interface
{
public:
	virtual int used() const = 0;
	virtual int size() const = 0;
	virtual void debug() const = 0;
	virtual bool nb_peek(T&, int n) const = 0;
	virtual bool nb_poke(const T&, int n = 0) = 0;
};

// Fifo interfaces
template <typename T>
class tlm_fifo_put_if
	: public virtual tlm_put_if<T>
	, public virtual tlm_fifo_debug_if<T>
{
};

template < typename T >
class tlm_fifo_get_if
	: public virtual tlm_get_peek_if<T>
	, public virtual tlm_fifo_debug_if<T>
{
};

} // end of namespace tlm

#endif
