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

#ifndef __LIBIEEE1666_CHANNELS_SIGNAL_H__
#define __LIBIEEE1666_CHANNELS_SIGNAL_H__

#include "core/prim_channel.h"
#include "core/port.h"
#include "core/event.h"
#include "core/kernel.h"
#include "channels/fwd.h"
#include "channels/signal_if.h"
#include "channels/signal.h"
#include <iostream>

namespace sc_core {

template <class T, sc_writer_policy WRITER_POLICY>
inline std::ostream& operator << (std::ostream&, const sc_signal<T,WRITER_POLICY>&);

template <class T, sc_writer_policy WRITER_POLICY = SC_ONE_WRITER>
class sc_signal : public sc_signal_inout_if<T>, public sc_prim_channel
{
public:
	sc_signal();
	explicit sc_signal(const char *);
	virtual ~sc_signal();
	virtual void register_port(sc_port_base&, const char *);
	virtual const T& read() const;
	operator const T& () const;
	virtual sc_writer_policy get_writer_policy() const;
	virtual void write(const T&);
	sc_signal<T,WRITER_POLICY>& operator = (const T&);
	sc_signal<T,WRITER_POLICY>& operator = (const sc_signal<T,WRITER_POLICY>&);
	virtual const sc_event& default_event() const;
	virtual const sc_event& value_changed_event() const;
	virtual bool event() const;
	virtual void print(std::ostream& = std::cout) const;
	virtual void dump(std::ostream& = std::cout) const;
	virtual const char *kind() const;
protected:
	virtual void update();
private:
	// Disabled
	sc_signal( const sc_signal<T,WRITER_POLICY>& );
	
	sc_port_base *registered_inout_port;
	sc_object *writer;
	sc_event signal_value_changed_event;
	sc_dt::uint64 signal_value_changed_delta_cycle;
	unsigned int value_index;
	T value[2];

	friend std::ostream& operator << <T, WRITER_POLICY>(std::ostream&, const sc_signal<T,WRITER_POLICY>&);
protected:
	const T& current_value() const;
	T& current_value();
	const T& new_value() const;
	T& new_value();
	void toggle_value();
	bool check_write();
	void do_update();
};

template <sc_writer_policy WRITER_POLICY>
class sc_signal<bool,WRITER_POLICY> : public sc_signal_inout_if<bool>, public sc_prim_channel
{
public:
	sc_signal();
	explicit sc_signal( const char* );
	virtual ~sc_signal();
	virtual void register_port( sc_port_base&, const char* );
	virtual const bool& read() const;
	operator const bool& () const;
	virtual sc_writer_policy get_writer_policy() const;
	virtual void write( const bool& );
	sc_signal<bool,WRITER_POLICY>& operator= ( const bool& );
	sc_signal<bool,WRITER_POLICY>& operator= ( const sc_signal<bool,WRITER_POLICY>& );
	virtual const sc_event& default_event() const;
	virtual const sc_event& value_changed_event() const;
	virtual const sc_event& posedge_event() const;
	virtual const sc_event& negedge_event() const;
	virtual bool event() const;
	virtual bool posedge() const;
	virtual bool negedge() const;
	virtual void print( std::ostream& = std::cout ) const;
	virtual void dump( std::ostream& = std::cout ) const;
	virtual const char* kind() const;
protected:
	virtual void update();
private:
	// Disabled
	sc_signal( const sc_signal<bool,WRITER_POLICY>& );
	
	virtual void is_reset(const sc_process_reset& process_reset);
	
	sc_port_base *registered_inout_port;
	sc_object *writer;
	sc_event signal_value_changed_event;
	sc_event signal_posedge_event;
	sc_event signal_negedge_event;
	sc_dt::uint64 signal_value_changed_delta_cycle;
	sc_dt::uint64 negedge_delta_cycle;
	bool value[2];
	std::vector<sc_process_reset> process_resets;
	
	
	friend std::ostream& operator << <bool, WRITER_POLICY>( std::ostream&, const sc_signal<bool,WRITER_POLICY>& );

protected:
	const bool& current_value() const;
	bool& current_value();
	const bool& new_value() const;
	bool& new_value();
	void toggle_value();
	bool check_write();
	void do_update();
};

// template <sc_writer_policy WRITER_POLICY>
// class sc_signal<sc_dt::sc_logic,WRITER_POLICY>
// : public sc_signal_inout_if<sc_dt::sc_logic,WRITER_POLICY>, public sc_prim_channel
// {
// public:
// 	sc_signal();
// 	explicit sc_signal( const char* );
// 	virtual ~sc_signal();
// 	virtual void register_port( sc_port_base&, const char* );
// 	virtual const sc_dt::sc_logic& read() const;
// 	operator const sc_dt::sc_logic& () const;
// 	virtual void write( const sc_dt::sc_logic& );
// 	sc_signal<sc_dt::sc_logic,WRITER_POLICY>& operator= ( const sc_dt::sc_logic& );
// 	sc_signal<sc_dt::sc_logic,WRITER_POLICY>&
// 	operator= ( const sc_signal<sc_dt::sc_logic,WRITER_POLICY>& );
// 	virtual const sc_event& default_event() const;
// 	virtual const sc_event& value_changed_event() const;
// 	virtual const sc_event& posedge_event() const;
// 	virtual const sc_event& negedge_event() const;
// 	virtual bool event() const;
// 	virtual bool posedge() const;
// 	virtual bool negedge() const;
// 	virtual void print( std::ostream& = std::cout ) const;
// 	virtual void dump( std::ostream& = std::cout ) const;
// 	virtual const char* kind() const;
// protected:
// 	virtual void update();
// private:
// 	// Disabled
// 	sc_signal( const sc_signal<sc_dt::sc_logic,WRITER_POLICY>& );
// };
// 
// class sc_signal_resolved : public sc_signal<sc_dt::sc_logic,SC_MANY_WRITERS>
// {
// public:
// 	sc_signal_resolved();
// 	explicit sc_signal_resolved( const char* );
// 	virtual ~sc_signal_resolved();
// 	virtual void register_port( sc_port_base&, const char* );
// 	virtual void write( const sc_dt::sc_logic& );
// 	sc_signal_resolved& operator= ( const sc_dt::sc_logic& );
// 	sc_signal_resolved& operator= ( const sc_signal_resolved& );
// 	virtual const char* kind() const;
// protected:
// 	virtual void update();
// private:
// 	// Disabled
// 	sc_signal_resolved( const sc_signal_resolved& );
// };
// 
// template <int W>
// class sc_signal_rv : public sc_signal<sc_dt::sc_lv<W>,SC_MANY_WRITERS>
// {
// public:
// 	sc_signal_rv();
// 	explicit sc_signal_rv( const char* );
// 	virtual ~sc_signal_rv();
// 	virtual void register_port( sc_port_base&, const char* );
// 	virtual void write( const sc_dt::sc_lv<W>& );
// 	sc_signal_rv<W>& operator= ( const sc_dt::sc_lv<W>& );
// 	sc_signal_rv<W>& operator= ( const sc_signal_rv<W>& );
// 	virtual const char* kind() const;
// protected:
// 	virtual void update();
// private:
// 	// Disabled
// 	sc_signal_rv( const sc_signal_rv<W>& );
// };

/////////////////////////////////// sc_signal<T, WRITER_POLICY> /////////////////////////////////////////////

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::sc_signal()
	: sc_prim_channel(__LIBIEEE1666_KERNEL_PREFIX__ "_signal")
	, registered_inout_port(0)
	, writer(0)
	, signal_value_changed_event(__LIBIEEE1666_KERNEL_PREFIX__ "_value_changed_event")
	, signal_value_changed_delta_cycle(0x7fffffffffffffffULL)
	, value_index(0)
	, value()
{
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::sc_signal(const char* _name)
	: sc_prim_channel(_name)
	, registered_inout_port(0)
	, writer(0)
	, signal_value_changed_event(__LIBIEEE1666_KERNEL_PREFIX__ "_signal_value_changed_event")
	, value_index(0)
	, value()
{
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::~sc_signal()
{
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::register_port(sc_port_base& port, const char *if_typename)
{
	if(std::string(typeid(sc_signal_inout_if<T>).name()).compare(if_typename) == 0)
	{
		// output port
		if((WRITER_POLICY == SC_ONE_WRITER) && registered_inout_port && (registered_inout_port != &port))
		{
			throw std::runtime_error("sc_signal can't be bound to more than one output port");
		}
		registered_inout_port = &port;
	}
	else if(std::string(typeid(sc_signal_in_if<T>).name()).compare(if_typename) != 0)
	{
		throw std::runtime_error("sc_signal is not bound to sc_signal_in_if<T> nor sc_signal_inout_if<T>");
	}
}

template <class T, sc_writer_policy WRITER_POLICY>
const T& sc_signal<T, WRITER_POLICY>::read() const
{
	return current_value();
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::operator const T& () const
{
	return read();
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_writer_policy sc_signal<T, WRITER_POLICY>::get_writer_policy() const
{
	return WRITER_POLICY;
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::write(const T& v)
{
	bool value_changed = !(v == current_value());
	
	new_value() = v;
	
	if(value_changed)
	{
		if(!check_write())
		{
			throw std::runtime_error("sc_signal<T, SC_ONE_WRITER> with multiple writers");
		}
		
		request_update();
	}
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T,WRITER_POLICY>& sc_signal<T, WRITER_POLICY>::operator = (const T& v)
{
	write(v);
	return *this;
}

template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T,WRITER_POLICY>& sc_signal<T, WRITER_POLICY>::operator = (const sc_signal<T, WRITER_POLICY>& s)
{
	write(s.read());
	return *this;
}

template <class T, sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<T, WRITER_POLICY>::default_event() const
{
	return signal_value_changed_event;
}

template <class T, sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<T, WRITER_POLICY>::value_changed_event() const
{
	return signal_value_changed_event;
}

template <class T, sc_writer_policy WRITER_POLICY>
bool sc_signal<T, WRITER_POLICY>::event() const
{
	return signal_value_changed_delta_cycle == kernel->get_delta_count();
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::print(std::ostream& os) const
{
	os << *this;
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::dump(std::ostream& os) const
{
	os << name() << ": kind=\"" << kind() << "\", current_value=" << current_value() << ", new_value=" << new_value();
}

template <class T, sc_writer_policy WRITER_POLICY>
const char* sc_signal<T, WRITER_POLICY>::kind() const
{
	return "sc_signal";
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::update()
{
	bool value_changed = !(current_value() == new_value());
	
	if(value_changed)
	{
		do_update();
	}
}

template <class T, sc_writer_policy WRITER_POLICY>
bool sc_signal<T, WRITER_POLICY>::check_write()
{
	if(WRITER_POLICY == SC_ONE_WRITER)
	{
		if(!writer) writer = kernel->get_current_writer();
		
		if(writer != kernel->get_current_writer())
		{
			return false;
		}
	}
	
	return true;
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::do_update()
{
	toggle_value();
	signal_value_changed_delta_cycle = kernel->get_delta_count();
	signal_value_changed_event.notify(SC_ZERO_TIME);
}

// disabled
template <class T, sc_writer_policy WRITER_POLICY>
sc_signal<T, WRITER_POLICY>::sc_signal( const sc_signal<T,WRITER_POLICY>& )
{
}

template <class T, sc_writer_policy WRITER_POLICY>
const T& sc_signal<T, WRITER_POLICY>::current_value() const
{
	return value[value_index];
}

template <class T, sc_writer_policy WRITER_POLICY>
T& sc_signal<T, WRITER_POLICY>::current_value()
{
	return value[value_index];
}

template <class T, sc_writer_policy WRITER_POLICY>
const T& sc_signal<T, WRITER_POLICY>::new_value() const
{
	return value[value_index ^ 1];
}

template <class T, sc_writer_policy WRITER_POLICY>
T& sc_signal<T, WRITER_POLICY>::new_value()
{
	return value[value_index ^ 1];
}

template <class T, sc_writer_policy WRITER_POLICY>
void sc_signal<T, WRITER_POLICY>::toggle_value()
{
	value_index ^= 1;
}

template <class T, sc_writer_policy WRITER_POLICY>
inline std::ostream& operator << (std::ostream& os, const sc_signal<T,WRITER_POLICY>& s)
{
	return os << s.current_value();
}

///////////////////////////// sc_signal<bool,WRITER_POLICY> ///////////////////////////////////

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::sc_signal()
	: sc_prim_channel(__LIBIEEE1666_KERNEL_PREFIX__ "_signal")
	, registered_inout_port(0)
	, writer(0)
	, signal_value_changed_event(__LIBIEEE1666_KERNEL_PREFIX__ "_signal_value_changed_event")
	, signal_posedge_event(__LIBIEEE1666_KERNEL_PREFIX__ "_signal_posedge_event")
	, signal_negedge_event(__LIBIEEE1666_KERNEL_PREFIX__ "_signal_negedge_event")
	, signal_value_changed_delta_cycle(0x7fffffffffffffffULL)
	, value()
	, process_resets()
{
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::sc_signal(const char *_name)
	: sc_prim_channel(_name)
	, registered_inout_port(0)
	, writer(0)
	, signal_value_changed_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + "_" + _name + "_signal_value_changed_event").c_str())
	, signal_posedge_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + "_" + _name + "_signal_posedge_event").c_str())
	, signal_negedge_event((std::string(__LIBIEEE1666_KERNEL_PREFIX__) + "_" + _name + "_signal_negedge_event").c_str())
	, signal_value_changed_delta_cycle(0x7fffffffffffffffULL)
	, value()
	, process_resets()
{
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::~sc_signal()
{
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::register_port(sc_port_base& port, const char *if_typename)
{
	if(std::string(typeid(sc_signal_inout_if<bool>).name()).compare(if_typename) == 0)
	{
		// output port
		if((WRITER_POLICY == SC_ONE_WRITER) && registered_inout_port && (registered_inout_port != &port))
		{
			throw std::runtime_error("sc_signal can't be bound to more than one output port");
		}
		registered_inout_port = &port;
	}
	else if(std::string(typeid(sc_signal_in_if<bool>).name()).compare(if_typename) != 0)
	{
		throw std::runtime_error("sc_signal is not bound to sc_signal_in_if<bool> nor sc_signal_inout_if<bool>");
	}
	
}

template <sc_writer_policy WRITER_POLICY>
const bool& sc_signal<bool, WRITER_POLICY>::read() const
{
	return current_value();
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::operator const bool& () const
{
	return read();
}

template <sc_writer_policy WRITER_POLICY>
sc_writer_policy sc_signal<bool, WRITER_POLICY>::get_writer_policy() const
{
	return WRITER_POLICY;
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::write(const bool& v)
{
	bool value_changed = !(v == current_value());

	new_value() = v;

	if(value_changed)
	{
		if(!check_write())
		{
			throw std::runtime_error("sc_signal<T, SC_ONE_WRITER> with multiple writers");
		}
		
		request_update();
	}
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool,WRITER_POLICY>& sc_signal<bool, WRITER_POLICY>::operator = (const bool& v)
{
	write(v);
	return *this;
}

template <sc_writer_policy WRITER_POLICY>
sc_signal<bool,WRITER_POLICY>& sc_signal<bool, WRITER_POLICY>::operator = (const sc_signal<bool, WRITER_POLICY>& s)
{
	write(s.read());
	return *this;
}

template <sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<bool, WRITER_POLICY>::default_event() const
{
	return signal_value_changed_event;
}

template <sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<bool, WRITER_POLICY>::value_changed_event() const
{
	return signal_value_changed_event;
}

template <sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<bool, WRITER_POLICY>::posedge_event() const
{
	return signal_posedge_event;
}

template <sc_writer_policy WRITER_POLICY>
const sc_event& sc_signal<bool, WRITER_POLICY>::negedge_event() const
{
	return signal_negedge_event;
}

template <sc_writer_policy WRITER_POLICY>
bool sc_signal<bool, WRITER_POLICY>::event() const
{
	return signal_value_changed_delta_cycle == kernel->get_delta_count();
}

template <sc_writer_policy WRITER_POLICY>
bool sc_signal<bool, WRITER_POLICY>::posedge() const
{
	return current_value() && event();
}

template <sc_writer_policy WRITER_POLICY>
bool sc_signal<bool, WRITER_POLICY>::negedge() const
{
	return !current_value() && event();
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::print(std::ostream& os) const
{
	os << *this;
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::dump(std::ostream& os) const
{
	os << name() << ": kind=\"" << kind() << "\", current_value=" << current_value() << ", new_value=" << new_value();
}

template <sc_writer_policy WRITER_POLICY>
const char* sc_signal<bool, WRITER_POLICY>::kind() const
{
	return "sc_signal";
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::update()
{
	bool value_changed = !(current_value() == new_value());
	
	if(value_changed)
	{
		do_update();
	}
}

template <sc_writer_policy WRITER_POLICY>
bool sc_signal<bool, WRITER_POLICY>::check_write()
{
	if(WRITER_POLICY == SC_ONE_WRITER)
	{
		if(!writer) writer = kernel->get_current_writer();
		
		if(writer != kernel->get_current_writer())
		{
			return false;
		}
	}
	return true;
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::do_update()
{
	toggle_value();
	signal_value_changed_delta_cycle = kernel->get_delta_count();
	signal_value_changed_event.notify(SC_ZERO_TIME);
	if(current_value())
	{
		signal_posedge_event.notify(SC_ZERO_TIME);
	}
	else
	{
		signal_negedge_event.notify(SC_ZERO_TIME);
	}
	
	unsigned int num_process_resets = process_resets.size();
	if(num_process_resets)
	{
		unsigned int i;
		for(i = 0; i < num_process_resets; i++)
		{
			const sc_process_reset& process_reset = process_resets[i];
			
			process_reset.reset_signal_value_changed(current_value());
		}
	}
}

// disabled
template <sc_writer_policy WRITER_POLICY>
sc_signal<bool, WRITER_POLICY>::sc_signal( const sc_signal<bool,WRITER_POLICY>& )
{
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::is_reset(const sc_process_reset& process_reset)
{
	process_resets.push_back(process_reset);
}

template <sc_writer_policy WRITER_POLICY>
const bool& sc_signal<bool, WRITER_POLICY>::current_value() const
{
	return value[0];
}

template <sc_writer_policy WRITER_POLICY>
bool& sc_signal<bool, WRITER_POLICY>::current_value()
{
	return value[0];
}

template <sc_writer_policy WRITER_POLICY>
const bool& sc_signal<bool, WRITER_POLICY>::new_value() const
{
	return value[1];
}

template <sc_writer_policy WRITER_POLICY>
bool& sc_signal<bool, WRITER_POLICY>::new_value()
{
	return value[1];
}

template <sc_writer_policy WRITER_POLICY>
void sc_signal<bool, WRITER_POLICY>::toggle_value()
{
	current_value() = new_value();
}

} // end of namespace sc_core

#endif
