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

#ifndef __LIBIEEE1666_TLM1_INTERFACE_MESSAGE_PASSING_IFS_H__
#define __LIBIEEE1666_TLM1_INTERFACE_MESSAGE_PASSING_IFS_H__

#include <tlm1/fwd.h>
#include <systemc>

namespace tlm {

template <class T>
class tlm_tag
{
};

// Uni-directional blocking interfaces
template <typename T>
class tlm_blocking_put_if : public virtual sc_core::sc_interface
{
public:
	virtual void put(const T& t) = 0;
};

template <typename T>
class tlm_blocking_get_if : public virtual sc_core::sc_interface
{
public:
	virtual T get(tlm_tag<T> *t = 0) = 0;
	virtual void get(T& t) { t = get(); }
};

template <typename T>
class tlm_blocking_peek_if : public virtual sc_core::sc_interface
{
public:
	virtual T peek(tlm_tag<T> *t = 0) const = 0;
	virtual void peek(T& t) const { t = peek(); }
};

// Uni-directional non blocking interfaces
template <typename T>
class tlm_nonblocking_put_if : public virtual sc_core::sc_interface
{
public:
	virtual bool nb_put(const T& t) = 0;
	virtual bool nb_can_put(tlm_tag<T> *t = 0) const = 0;
	virtual const sc_core::sc_event& ok_to_put(tlm_tag<T> *t = 0) const = 0;
};

template <typename T>
class tlm_nonblocking_get_if : public virtual sc_core::sc_interface
{
public:
	virtual bool nb_get(T& t) = 0;
	virtual bool nb_can_get(tlm_tag<T> *t = 0) const = 0;
	virtual const sc_core::sc_event& ok_to_get(tlm_tag<T> *t = 0) const = 0;
};

template <typename T>
class tlm_nonblocking_peek_if : public virtual sc_core::sc_interface
{
public:
	virtual bool nb_peek(T& t) const = 0;
	virtual bool nb_can_peek(tlm_tag<T> *t = 0) const = 0;
	virtual const sc_core::sc_event& ok_to_peek(tlm_tag<T> *t = 0) const = 0;
};

// Uni-directional combined blocking and non blocking interfaces
template <typename T>
class tlm_put_if
	: public virtual tlm_blocking_put_if<T>
	, public virtual tlm_nonblocking_put_if<T>
{
};

template <typename T>
class tlm_get_if
	: public virtual tlm_blocking_get_if<T>
	, public virtual tlm_nonblocking_get_if<T>
{
};

template <typename T>
class tlm_peek_if
	: public virtual tlm_blocking_peek_if<T>
	, public virtual tlm_nonblocking_peek_if<T>
{
};

// Uni-directional combined get-peek interfaces
template <typename T>
class tlm_blocking_get_peek_if
	: public virtual tlm_blocking_get_if<T>
	, public virtual tlm_blocking_peek_if<T>
{
};

template <typename T>
class tlm_nonblocking_get_peek_if
	: public virtual tlm_nonblocking_get_if<T>
	, public virtual tlm_nonblocking_peek_if<T>
{
};

template <typename T>
class tlm_get_peek_if
	: public virtual tlm_get_if<T>
	, public virtual tlm_peek_if<T>
	, public virtual tlm_blocking_get_peek_if<T>
	, public virtual tlm_nonblocking_get_peek_if<T>
{
};

// Bidirectional blocking transport interface
template <typename REQ, typename RSP>
class tlm_transport_if : public virtual sc_core::sc_interface
{
public:
	virtual RSP transport(const REQ&) = 0;
	virtual void transport(const REQ& req, RSP& rsp) { rsp = transport(req); }
};

} // end of namespace tlm

#endif
