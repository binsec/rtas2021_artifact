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

#ifndef __LIBIEEE1666_CORE_RESET_H__
#define __LIBIEEE1666_CORE_RESET_H__

#include "core/fwd.h"
#include "channels/fwd.h"

namespace sc_core {

class sc_process_reset
{
public:
	sc_process_reset(sc_process *process, bool async, bool active_level);
	
	sc_process *process;
	bool async;
	bool active_level;
	
	void reset_signal_value_changed(bool reset_signal_value) const;
};

struct sc_process_reset_bind_info
{
	sc_process_reset_bind_info(sc_process *process, bool async, const sc_in<bool>& in, bool active_level);
	sc_process_reset_bind_info(sc_process *process, bool async, const sc_inout<bool>& in, bool active_level);
	sc_process_reset_bind_info(sc_process *process, bool async, const sc_out<bool>& in, bool active_level);
	sc_process_reset_bind_info(sc_process *process, bool async, const sc_signal_in_if<bool>& in, bool active_level);
	
	sc_process *process;
	bool async;
	const sc_in<bool> *in;
	const sc_inout<bool> *inout;
	const sc_out<bool> *out;
	const sc_signal_in_if<bool> *signal_in_if;
	bool active_level;
};

} // end of namespace sc_core

#endif // __LIBIEEE1666_CORE_RESET_H__
