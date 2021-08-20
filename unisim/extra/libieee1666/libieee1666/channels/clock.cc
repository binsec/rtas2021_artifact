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

#include "channels/clock.h"
#include "core/spawn.h"
#include <stdexcept>

namespace sc_core {

//////////////////////////////////// sc_clock /////////////////////////////////////////////
	
sc_clock::sc_clock()
	: sc_signal<bool>(sc_gen_unique_name("clock"))
	, clock_period(sc_time(1.0, SC_NS))
	, clock_duty_cycle(0.5)
	, clock_start_time(SC_ZERO_TIME)
	, clock_posedge_first(true)
	, gen_posedge_event(sc_gen_unique_name((std::string(name()) + "_gen_posedge_event").c_str()))
	, gen_negedge_event(sc_gen_unique_name((std::string(name()) + "_gen_negedge_event").c_str()))
{
	initialize();
}

sc_clock::sc_clock(const char *name_)
	: sc_signal<bool>(name_)
	, clock_period(sc_time(1.0, SC_NS))
	, clock_duty_cycle(0.5)
	, clock_start_time(SC_ZERO_TIME)
	, clock_posedge_first(true)
	, gen_posedge_event(sc_gen_unique_name((std::string(name()) + "_gen_posedge_event").c_str()))
	, gen_negedge_event(sc_gen_unique_name((std::string(name()) + "_gen_negedge_event").c_str()))
{
	initialize();
}

sc_clock::sc_clock(const char *name_,
                   const sc_time& period_,
                   double duty_cycle_,
                   const sc_time& start_time_,
                   bool posedge_first_ )
	: sc_signal<bool>(name_)
	, clock_period(period_)
	, clock_duty_cycle(duty_cycle_)
	, clock_start_time(start_time_)
	, clock_posedge_first(posedge_first_)
	, gen_posedge_event(sc_gen_unique_name((std::string(name()) + "_gen_posedge_event").c_str()))
	, gen_negedge_event(sc_gen_unique_name((std::string(name()) + "_gen_negedge_event").c_str()))
{
	initialize();
}

sc_clock::sc_clock( const char* name_,
                    double period_v_,
                    sc_time_unit period_tu_,
                    double duty_cycle_ )
	: sc_signal<bool>(name_)
	, clock_period(sc_time(period_v_, period_tu_))
	, clock_duty_cycle(duty_cycle_)
	, clock_start_time(SC_ZERO_TIME)
	, clock_posedge_first(true)
	, gen_posedge_event(sc_gen_unique_name((std::string(name()) + "_gen_posedge_event").c_str()))
	, gen_negedge_event(sc_gen_unique_name((std::string(name()) + "_gen_negedge_event").c_str()))
{
	initialize();
}

sc_clock::sc_clock( const char* name_,
                    double period_v_,
                    sc_time_unit period_tu_,
                    double duty_cycle_,
                    double start_time_v_,
                    sc_time_unit start_time_tu_,
                    bool posedge_first_ )
	: sc_signal<bool>(name_)
	, clock_period(sc_time(period_v_, period_tu_))
	, clock_duty_cycle(duty_cycle_)
	, clock_start_time(sc_time(start_time_v_, start_time_tu_))
	, clock_posedge_first(posedge_first_)
	, gen_posedge_event(sc_gen_unique_name((std::string(name()) + "_gen_posedge_event").c_str()))
	, gen_negedge_event(sc_gen_unique_name((std::string(name()) + "_gen_negedge_event").c_str()))
{
	initialize();
}

sc_clock::~sc_clock()
{
}

void sc_clock::write(const bool& value)
{
	throw std::runtime_error("attempt to write clock");
}

const sc_time& sc_clock::period() const
{
	return clock_period;
}

double sc_clock::duty_cycle() const
{
	return clock_duty_cycle;
}

const sc_time& sc_clock::start_time() const
{
	return clock_start_time;
}

bool sc_clock::posedge_first() const
{
	return clock_posedge_first;
}

const char* sc_clock::kind() const
{
	return "sc_clock";
}

void sc_clock::before_end_of_elaboration()
{
	sc_signal<bool>::write(!clock_posedge_first);
	
	if(clock_posedge_first)
		gen_posedge_event.notify(clock_start_time);
	else
		gen_negedge_event.notify(clock_start_time);
	
	sc_spawn_options posedge_process_spawn_options;
	posedge_process_spawn_options.spawn_method();
	posedge_process_spawn_options.set_sensitivity(&gen_posedge_event);
	posedge_process_spawn_options.dont_initialize();
	sc_spawn(sc_bind(&sc_clock::posedge_process, this), (std::string(name()) + "_posedge_process").c_str(), &posedge_process_spawn_options);
	
	sc_spawn_options negedge_process_spawn_options;
	negedge_process_spawn_options.spawn_method();
	negedge_process_spawn_options.set_sensitivity(&gen_negedge_event);
	negedge_process_spawn_options.dont_initialize();
	sc_spawn(sc_bind(&sc_clock::negedge_process, this), (std::string(name()) + "_negedge_process").c_str(), &negedge_process_spawn_options);
}

// disabled
sc_clock::sc_clock(const sc_clock&)
{
}

// disabled
sc_clock& sc_clock::operator = (const sc_clock&)
{
	return *this;
}

void sc_clock::initialize()
{
	if(clock_period <= SC_ZERO_TIME)
	{
		throw std::runtime_error("clock period shall be greater than zero");
	}
	
	if((clock_duty_cycle <= 0.0) || (clock_duty_cycle >= 1.0))
	{
		throw std::runtime_error("clock duty cycle shall lie between the limits 0.0 and 1.0, exclusive");
	}
	
	clock_negedge_time = clock_period * clock_duty_cycle;
	clock_posedge_time = clock_period - clock_negedge_time;	
}

void sc_clock::posedge_process()
{
	new_value() = true;
	request_update();
	gen_negedge_event.notify(clock_negedge_time);
}

void sc_clock::negedge_process()
{
	new_value() = false;
	request_update();
	gen_posedge_event.notify(clock_posedge_time);
}

} // end of namespace sc_core
