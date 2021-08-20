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

#include "channels/event_queue.h"

namespace sc_core {

//////////////////////////////////// sc_event_queue /////////////////////////////////////////////

sc_event_queue::sc_event_queue(sc_module_name _name)
	: sc_module(_name)
	, notification_time_queue()
	, event("event")
{
	SC_HAS_PROCESS(sc_event_queue);
	
	SC_METHOD(scheduling_process);
	sensitive << event;
}

sc_event_queue::~sc_event_queue()
{
}

const char *sc_event_queue::kind() const
{
	return "sc_event_queue";
}

void sc_event_queue::notify(double d, sc_time_unit u)
{
	sc_time t(d, u);
	notify(t);
}

void sc_event_queue::notify(const sc_time& t)
{
	if(notification_time_queue.empty())
	{
		event.notify(t);
	}
	else
	{
		std::set<sc_time>::iterator it = notification_time_queue.begin();
			
		sc_time notification_delay(*it);
		notification_time_queue.erase(it);
		notification_delay -= sc_time_stamp();
		event.notify(notification_delay);
	}
	
	sc_time notification_time(sc_time_stamp());
	notification_time += t;
	notification_time_queue.insert(notification_time);
}

void sc_event_queue::cancel_all()
{
	notification_time_queue.clear();
	event.cancel();
}

const sc_event& sc_event_queue::default_event() const
{
	return event;
}

void sc_event_queue::scheduling_process()
{
	if(!notification_time_queue.empty()) return;
	
	std::set<sc_time>::iterator it = notification_time_queue.begin();

	const sc_time& time_stamp = sc_time_stamp();

	do
	{
		sc_time notification_time(*it);
		notification_time_queue.erase(it);
		if(notification_time > time_stamp)
		{
			notification_time -= sc_time_stamp();
			event.notify(notification_time);
			return;
		}
		
		if(notification_time_queue.empty()) return;
		
		it = notification_time_queue.begin();
	}
	while(true);
}


} // end of namespace sc_core
