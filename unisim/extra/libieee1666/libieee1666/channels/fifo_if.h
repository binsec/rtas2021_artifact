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

#ifndef __LIBIEEE1666_CHANNELS_FIFO_IF_H__
#define __LIBIEEE1666_CHANNELS_FIFO_IF_H__

#include "core/event.h"
#include "core/interface.h"

namespace sc_core {

template <class T>
class sc_fifo_nonblocking_in_if : virtual public sc_interface
{
public:
	virtual bool nb_read( T& ) = 0;
	virtual const sc_event& data_written_event() const = 0;
};

template <class T>
class sc_fifo_blocking_in_if : virtual public sc_interface
{
public:
	virtual void read( T& ) = 0;
	virtual T read() = 0;
};

template <class T>
class sc_fifo_in_if : public sc_fifo_nonblocking_in_if<T>, public sc_fifo_blocking_in_if<T>
{
public:
	virtual int num_available() const = 0;
protected:
	sc_fifo_in_if();
private:
	// Disabled
	sc_fifo_in_if( const sc_fifo_in_if<T>& );
	sc_fifo_in_if<T>& operator= ( const sc_fifo_in_if<T>& );
};

template <class T>
class sc_fifo_nonblocking_out_if : virtual public sc_interface
{
public:
	virtual bool nb_write( const T& ) = 0;
	virtual const sc_event& data_read_event() const = 0;
};

template <class T>
class sc_fifo_blocking_out_if : virtual public sc_interface
{
public:
	virtual void write( const T& ) = 0;
};

template <class T>
class sc_fifo_out_if : public sc_fifo_nonblocking_out_if<T>, public sc_fifo_blocking_out_if<T>
{
public:
	virtual int num_free() const = 0;
	protected:
	sc_fifo_out_if();
private:
	// Disabled
	sc_fifo_out_if( const sc_fifo_out_if<T>& );
	sc_fifo_out_if<T>& operator= ( const sc_fifo_out_if<T>& );
};

//////////////////////////////////// sc_fifo_in_if<> /////////////////////////////////////////////

template <class T>
sc_fifo_in_if<T>::sc_fifo_in_if( const sc_fifo_in_if<T>& )
{
}

template <class T>
sc_fifo_in_if<T>& sc_fifo_in_if<T>::operator= ( const sc_fifo_in_if<T>& )
{
}

//////////////////////////////////// sc_fifo_out_if<> /////////////////////////////////////////////

template <class T>
sc_fifo_out_if<T>::sc_fifo_out_if()
{
}

template <class T>
sc_fifo_out_if<T>::sc_fifo_out_if( const sc_fifo_out_if<T>& )
{
}

template <class T>
sc_fifo_out_if<T>& sc_fifo_out_if<T>::operator= ( const sc_fifo_out_if<T>& )
{
}

} // end of namespace sc_core

#endif
