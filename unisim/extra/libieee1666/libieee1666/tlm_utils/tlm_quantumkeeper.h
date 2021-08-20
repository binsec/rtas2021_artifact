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

#ifndef __TLM_UTILS_TLM_QUANTUM_KEEPER_H__
#define __TLM_UTILS_TLM_QUANTUM_KEEPER_H__

#include <tlm>

namespace tlm_utils {

class tlm_quantumkeeper
{
public:
	static void set_global_quantum( const sc_core::sc_time& );
	static const sc_core::sc_time& get_global_quantum();
	tlm_quantumkeeper();
	virtual ~tlm_quantumkeeper();
	virtual void inc( const sc_core::sc_time& );
	virtual void set( const sc_core::sc_time& );
	virtual sc_core::sc_time get_current_time() const;
	virtual sc_core::sc_time get_local_time();
	virtual bool need_sync() const;
	virtual void sync();
	void set_and_sync(const sc_core::sc_time& t)
	{
		set(t);
		if (need_sync()) sync();
	}
	virtual void reset();
protected:
	virtual sc_core::sc_time compute_local_quantum();
private:
	sc_core::sc_time local_time_offset;
	sc_core::sc_time sync_deadline;
};

} // end of namespace tlm_utils

#endif // __TLM_UTILS_TLM_QUANTUM_KEEPER_H__
