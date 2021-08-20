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

#ifndef __LIBIEEE1666_CHANNELS_CLOCK_H__
#define __LIBIEEE1666_CHANNELS_CLOCK_H__

#include "core/fwd.h"
#include "channels/fwd.h"
#include "core/time.h"
#include "channels/signal.h"
#include "channels/in.h"

namespace sc_core {

class sc_clock : public sc_signal<bool>
{
public:
	sc_clock();
	explicit sc_clock(const char *name_);
	
	sc_clock(const char *name_,
	         const sc_time& period_,
	         double duty_cycle_ = 0.5,
	         const sc_time& start_time_ = SC_ZERO_TIME,
	         bool posedge_first_ = true);
	
	sc_clock(const char *name_,
	         double period_v_,
	         sc_time_unit period_tu_,
	         double duty_cycle_ = 0.5);
	
	sc_clock(const char *name_,
	         double period_v_,
	         sc_time_unit period_tu_,
	         double duty_cycle_,
	         double start_time_v_,
	         sc_time_unit start_time_tu_,
	         bool posedge_first_ = true);
	
	virtual ~sc_clock();
	virtual void write( const bool& );
	const sc_time& period() const;
	double duty_cycle() const;
	const sc_time& start_time() const;
	bool posedge_first() const;
	virtual const char *kind() const;
protected:
	virtual void before_end_of_elaboration();
private:
	// Disabled
	sc_clock(const sc_clock&);
	sc_clock& operator = (const sc_clock&);
	
	void initialize();
	void posedge_process();
	void negedge_process();
	
	sc_time clock_period;
	double clock_duty_cycle;
	sc_time clock_start_time;
	bool clock_posedge_first;
	sc_time clock_posedge_time;
	sc_time clock_negedge_time;
	sc_event gen_posedge_event;
	sc_event gen_negedge_event;
};

typedef sc_in<bool> sc_in_clk ;

} // end of namespace sc_core

#endif
