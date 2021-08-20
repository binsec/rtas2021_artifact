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

#ifndef __LIBIEEE1666_CHANNELS_SIGNAL_IF_H__
#define __LIBIEEE1666_CHANNELS_SIGNAL_IF_H__

#include "core/interface.h"
#include "core/event.h"
#include "core/reset.h"
#include "channels/fwd.h"

namespace sc_core {

template <class T>
class sc_signal_in_if : virtual public sc_interface
{
public:
	virtual const T& read() const = 0;
	virtual const sc_event& value_changed_event() const = 0;
	virtual bool event() const = 0;
protected:
	sc_signal_in_if();
private:
	// Disabled
	sc_signal_in_if( const sc_signal_in_if<T>& );
	sc_signal_in_if<T>& operator= ( const sc_signal_in_if<T>& );
};

template <>
class sc_signal_in_if<bool> : virtual public sc_interface
{
public:
	virtual const bool& read() const = 0;
	virtual const sc_event& value_changed_event() const = 0;
	virtual const sc_event& posedge_event() const = 0;
	virtual const sc_event& negedge_event() const = 0;
	virtual bool event() const = 0;
	virtual bool posedge() const = 0;
	virtual bool negedge() const = 0;
protected:
	sc_signal_in_if();
private:
	// Disabled
	sc_signal_in_if( const sc_signal_in_if<bool>& );
	sc_signal_in_if<bool>& operator= ( const sc_signal_in_if<bool>& );
	
	friend class sc_process;
	virtual void is_reset(const sc_process_reset& process_reset) {}
};

// template <>
// class sc_signal_in_if<sc_dt::sc_logic> : virtual public sc_interface
// {
// public:
// 	virtual const T& read() const = 0;
// 	virtual const sc_event& value_changed_event() const = 0;
// 	virtual const sc_event& posedge_event() const = 0;
// 	virtual const sc_event& negedge_event() const = 0;
// 	virtual bool event() const = 0;
// 	virtual bool posedge() const = 0;
// 	virtual bool negedge() const = 0;
// protected:
// 	sc_signal_in_if();
// private:
// 	// Disabled
// 	sc_signal_in_if( const sc_signal_in_if<sc_dt::sc_logic>& );
// 	sc_signal_in_if<sc_dt::sc_logic>& operator= ( const sc_signal_in_if<sc_dt::sc_logic>& );
// };

template <class T>
class sc_signal_write_if : virtual public sc_interface
{
public:
	virtual sc_writer_policy get_writer_policy() const;
	virtual void write( const T& ) = 0;
protected:
	sc_signal_write_if();
private:
	// Disabled
	sc_signal_write_if( const sc_signal_write_if<T>& );
	sc_signal_write_if<T>& operator= ( const sc_signal_write_if<T>& );
};

template <class T>
class sc_signal_inout_if : public sc_signal_in_if<T> , public sc_signal_write_if<T>
{
protected:
	sc_signal_inout_if();
private:
	// Disabled
	sc_signal_inout_if( const sc_signal_inout_if<T>& );
	sc_signal_inout_if<T>& operator= ( const sc_signal_inout_if<T>& );
};

/////////////////////////////// sc_signal_in_if<> ////////////////////////////////////////

template <class T>
sc_signal_in_if<T>::sc_signal_in_if()
{
}

template <class T>
sc_signal_in_if<T>::sc_signal_in_if( const sc_signal_in_if<T>& )
{
}

template <class T>
sc_signal_in_if<T>& sc_signal_in_if<T>::operator= ( const sc_signal_in_if<T>& )
{
}

/////////////////////////////// sc_signal_write_if<> ////////////////////////////////////////

template <class T>
sc_writer_policy sc_signal_write_if<T>::get_writer_policy() const
{
	return SC_ONE_WRITER;
}

template <class T>
sc_signal_write_if<T>::sc_signal_write_if()
{
}

template <class T>
sc_signal_write_if<T>::sc_signal_write_if( const sc_signal_write_if<T>& )
{
}

template <class T>
sc_signal_write_if<T>& sc_signal_write_if<T>::operator= ( const sc_signal_write_if<T>& )
{
}

/////////////////////////////// sc_signal_inout_if<> ////////////////////////////////////////

template <class T>
sc_signal_inout_if<T>::sc_signal_inout_if()
{
}

template <class T>
sc_signal_inout_if<T>::sc_signal_inout_if( const sc_signal_inout_if<T>& )
{
}

template <class T>
sc_signal_inout_if<T>& sc_signal_inout_if<T>::operator= ( const sc_signal_inout_if<T>& )
{
}

} // end of namespace sc_core

#endif
