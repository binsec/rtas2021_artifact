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

#ifndef __LIBIEEE1666_CORE_KERNEL_EVENT_H__
#define __LIBIEEE1666_CORE_KERNEL_EVENT_H__

#include "core/fwd.h"
#include "core/time.h"

namespace sc_core {

class sc_kernel_event
{
public:
	inline sc_kernel_event() ALWAYS_INLINE;
	inline sc_kernel_event(sc_event *event) ALWAYS_INLINE;
	inline void initialize(sc_event *event) ALWAYS_INLINE;
	inline void cancel() ALWAYS_INLINE;
	inline bool canceled() const ALWAYS_INLINE;
	
	inline sc_event *get_event() const ALWAYS_INLINE;
	
protected:
	sc_event *event;
};

class sc_timed_kernel_event : public sc_kernel_event
{
public:
	inline sc_timed_kernel_event() ALWAYS_INLINE;
	inline sc_timed_kernel_event(sc_event *event, const sc_time& time) ALWAYS_INLINE;
	inline void initialize(sc_event *event, const sc_time& time) ALWAYS_INLINE;

	inline const sc_time& get_time() const ALWAYS_INLINE;
	
private:
	sc_time time;
};

inline sc_kernel_event::sc_kernel_event()
	: event(0)
{
}

inline sc_kernel_event::sc_kernel_event(sc_event *_event)
	: event(_event)
{
}

inline void sc_kernel_event::initialize(sc_event *_event)
{
	event = _event;
}

inline void sc_kernel_event::cancel()
{
	event = 0;
}

inline bool sc_kernel_event::canceled() const
{
	return event != 0;
}

inline sc_event *sc_kernel_event::get_event() const
{
	return event;
}

inline sc_timed_kernel_event::sc_timed_kernel_event()
	: sc_kernel_event()
	, time()
{
}

inline sc_timed_kernel_event::sc_timed_kernel_event(sc_event *_event, const sc_time& _time)
	: sc_kernel_event(_event), time(_time)
{
}

inline void sc_timed_kernel_event::initialize(sc_event *_event, const sc_time& _time)
{
	sc_kernel_event::initialize(_event);
	time = _time;
}

inline const sc_time& sc_timed_kernel_event::get_time() const
{
	return time;
}

} // end of namespace sc_core

#endif
