/*
 *  Copyright (c) 2015,
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

#include "core/reset.h"
#include "core/process.h"

namespace sc_core {

sc_process_reset::sc_process_reset(sc_process *_process, bool _async, bool _active_level)
	: process(_process), async(_async), active_level(_active_level)
{
}

void sc_process_reset::reset_signal_value_changed(bool reset_signal_value) const
{
	process->reset_signal_value_changed(reset_signal_value, async, active_level);
}

sc_process_reset_bind_info::sc_process_reset_bind_info(sc_process *_process, bool _async, const sc_in<bool>& _in, bool _active_level)
	:  process(_process), async(_async), in(&_in), inout(0), out(0), signal_in_if(0), active_level(_active_level)
{
}

sc_process_reset_bind_info::sc_process_reset_bind_info(sc_process *_process, bool _async, const sc_inout<bool>& _inout, bool _active_level)
	:  process(_process), async(_async), in(0), inout(&_inout), out(0), signal_in_if(0), active_level(_active_level)
{
}

sc_process_reset_bind_info::sc_process_reset_bind_info(sc_process *_process, bool _async, const sc_out<bool>& _out, bool _active_level)
	:  process(_process), async(_async), in(0), inout(0), out(&_out), signal_in_if(0), active_level(_active_level)
{
}

sc_process_reset_bind_info::sc_process_reset_bind_info(sc_process *_process, bool _async, const sc_signal_in_if<bool>& _signal_in_if, bool _active_level)
	:  process(_process), async(_async), in(0), inout(0), out(0), signal_in_if(&_signal_in_if), active_level(_active_level)
{
}

} // end of namespace sc_core
