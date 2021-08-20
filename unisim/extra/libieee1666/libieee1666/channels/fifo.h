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

#ifndef __LIBIEEE1666_CHANNELS_FIFO_H__
#define __LIBIEEE1666_CHANNELS_FIFO_H__

#include "core/prim_channel.h"
#include "core/port.h"
#include "core/event.h"
#include "channels/fwd.h"
#include "channels/fifo_if.h"
#include <iostream>

namespace sc_core {

template <class T>
class sc_fifo : public sc_fifo_in_if<T>, public sc_fifo_out_if<T>, public sc_prim_channel
{
public:
	explicit sc_fifo(int size = 16);
	explicit sc_fifo(const char *, int size = 16);
	virtual ~sc_fifo();
	virtual void register_port( sc_port_base&, const char *);
	virtual void read(T&);
	virtual T read();
	virtual bool nb_read(T&);
	operator T ();
	virtual void write(const T&);
	virtual bool nb_write(const T&);
	sc_fifo<T>& operator = (const T&);
	virtual const sc_event& data_written_event() const;
	virtual const sc_event& data_read_event() const;
	virtual int num_available() const;
	virtual int num_free() const;
	virtual void print(std::ostream& = std::cout) const;
	virtual void dump(std::ostream& = std::cout) const;
	virtual const char *kind() const;
protected:
	virtual void update();
private:
	// Disabled
	sc_fifo(const sc_fifo<T>&);
	sc_fifo<T>& operator = (const sc_fifo<T>&);

	int capacity;
	int size;
	int free_count;
	int available_count;
	int read_count;
	int write_count;
	int write_idx;
	int read_idx;

	T *buffer;

	sc_event fifo_data_written_event;
	sc_event fifo_data_read_event;
	sc_port_base *registered_in_port;
	sc_port_base *registered_out_port;
};

template <class T>
inline std::ostream& operator<< ( std::ostream&, const sc_fifo<T>& );

//////////////////////////////////// sc_fifo<> /////////////////////////////////////////////


template <class T>
sc_fifo<T>::sc_fifo(int _size)
	: sc_prim_channel(sc_gen_unique_name("sc_fifo"))
	, capacity(_size)
	, size(0)
	, free_count(_size)
	, available_count(0)
	, read_count(0)
	, write_count(0)
	, write_idx(0)
	, read_idx(0)
	, buffer(0)
	, fifo_data_written_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + "_semaphore_value_incremented_event").c_str())
	, fifo_data_read_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + "_semaphore_value_incremented_event").c_str())
	, registered_in_port(0)
	, registered_out_port(0)
{
	buffer = new T[capacity];
}

template <class T>
sc_fifo<T>::sc_fifo(const char *_name, int _size)
	: sc_prim_channel(_name) 
	, capacity(_size)
	, size(0)
	, free_count(_size)
	, available_count(0)
	, read_count(0)
	, write_count(0)
	, write_idx(0)
	, read_idx(0)
	, buffer(0)
	, fifo_data_written_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + _name + "_semaphore_value_incremented_event").c_str())
	, fifo_data_read_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + _name + "_semaphore_value_incremented_event").c_str())
	, registered_in_port(0)
	, registered_out_port(0)
{
	buffer = new T[capacity];
}

template <class T>
sc_fifo<T>::~sc_fifo()
{
	delete[] buffer;
}

template <class T>
void sc_fifo<T>::register_port(sc_port_base& port, const char *if_typename)
{
	if((std::string(typeid(sc_fifo_in_if<T>).name()).compare(if_typename) == 0) ||
	   (std::string(typeid(sc_fifo_nonblocking_in_if<T>).name()).compare(if_typename) == 0) ||
	   (std::string(typeid(sc_fifo_blocking_in_if<T>).name()).compare(if_typename) == 0))
	{
		if(registered_in_port)
		{
			throw std::runtime_error("sc_fifo can't be bound to more than one input port");
		}
		registered_in_port = &port;
	}
	if((std::string(typeid(sc_fifo_out_if<T>).name()).compare(if_typename) == 0) || 
	   (std::string(typeid(sc_fifo_nonblocking_out_if<T>).name()).compare(if_typename) == 0) ||
	   (std::string(typeid(sc_fifo_blocking_out_if<T>).name()).compare(if_typename) == 0))
	{
		if(registered_out_port)
		{
			throw std::runtime_error("sc_fifo can't be bound to more than one output port");
		}
		registered_out_port = &port;
	}
}

template <class T>
void sc_fifo<T>::read(T& v)
{
	if(available_count == 0)
	{
		do
		{
			kernel->wait(fifo_data_written_event);
		}
		while(available_count == 0);
	}
	v = buffer[read_idx++];
	if(read_idx >= capacity) read_idx = 0;
	read_count++;
	available_count--;

	request_update();
}

template <class T>
T sc_fifo<T>::read()
{
	T v;
	read(v);
	return v;
}

template <class T>
bool sc_fifo<T>::nb_read(T& v)
{
	if(available_count == 0) return false;
		
	v = buffer[read_idx++];
	if(read_idx >= capacity) read_idx = 0;
	read_count++;
	available_count--;

	request_update();
}

template <class T>
sc_fifo<T>::operator T ()
{
	return read();
}

template <class T>
void sc_fifo<T>::write(const T& v)
{
	if(free_count == 0)
	{
		do
		{
			kernel->wait(fifo_data_read_event);
		}
		while(free_count == 0);
	}
	buffer[write_idx++] = v;
	if(write_idx >= capacity) write_idx = 0;
	write_count++;
	free_count--;
	
	request_update();
}

template <class T>
bool sc_fifo<T>::nb_write(const T& v)
{
	if(free_count == 0) return false;

	buffer[write_idx++] = v;
	if(write_idx >= capacity) write_idx = 0;
	write_count++;
	free_count--;
	
	request_update();
}

template <class T>
sc_fifo<T>& sc_fifo<T>::operator = (const T& v)
{
	write(v);
	return *this;
}

template <class T>
const sc_event& sc_fifo<T>::data_written_event() const
{
	return fifo_data_written_event;
}

template <class T>
const sc_event& sc_fifo<T>::data_read_event() const
{
	return fifo_data_read_event;
}

template <class T>
int sc_fifo<T>::num_available() const
{
	return available_count;
}

template <class T>
int sc_fifo<T>::num_free() const
{
	return free_count;
}

template <class T>
void sc_fifo<T>::print(std::ostream& os) const
{
	if(size)
	{
		int count = size;
		int i = read_idx;
		do
		{
			os << buffer[i++];
			if(i >= size) i = 0;
			if(--count) os << " ";
		}
		while(count);
	}
}

template <class T>
void sc_fifo<T>::dump(std::ostream& os) const
{
	os << name() << " = (";
	print(os);
	os << ")";
}

template <class T>
const char* sc_fifo<T>::kind() const
{
	return "sc_fifo";
}

template <class T>
void sc_fifo<T>::update()
{
	if(write_count)
	{
		fifo_data_written_event.notify(SC_ZERO_TIME);
	}
	if(read_count)
	{
		fifo_data_read_event.notify(SC_ZERO_TIME);
	}
	size += write_count;
	size -= read_count;
	available_count = size;
	write_count = 0;
	read_count = 0;
}

// disabled
template <class T>
sc_fifo<T>::sc_fifo( const sc_fifo<T>& )
{
}

// disabled
template <class T>
sc_fifo<T>& sc_fifo<T>::operator= ( const sc_fifo<T>& )
{
	return *this;
}

template <class T>
inline std::ostream& operator << (std::ostream& os, const sc_fifo<T>& fifo)
{
	fifo.print(os);
	return os;
}

} // end of namespace sc_core

#endif
