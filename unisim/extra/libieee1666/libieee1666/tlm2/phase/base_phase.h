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

#ifndef __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_PHASE_BASE_PHASE_H__
#define __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_PHASE_BASE_PHASE_H__

#include <iostream>
#include <vector>

namespace tlm {

enum tlm_phase_enum
{
	UNINITIALIZED_PHASE = 0,
	BEGIN_REQ = 1,
	END_REQ,
	BEGIN_RESP,
	END_RESP
};

class tlm_phase
{
public:
	tlm_phase();
	tlm_phase(unsigned int v);
	tlm_phase(const tlm_phase_enum& v);
	tlm_phase& operator = (const tlm_phase_enum& v);
	operator unsigned int () const;
private:
	tlm_phase_enum value;
	friend std::ostream& operator << (std::ostream& os, const tlm_phase& phase);
protected:
	static std::vector<const char *> extended_phase_name;
	static unsigned int last_extended_phase_value;
};

inline std::ostream& operator << (std::ostream& os, const tlm_phase& phase)
{
	switch(phase.value)
	{
		case UNINITIALIZED_PHASE:
			os << "UNINITIALIZED_PHASE";
			break;
		case BEGIN_REQ:
			os << "BEGIN_REQ";
			break;
		case END_REQ:
			os << "END_REQ";
			break;
		case BEGIN_RESP:
			os << "BEGIN_RESP";
			break;
		case END_RESP:
			os << "END_RESP";
			break;
		default:
			os << tlm_phase::extended_phase_name[phase.value - END_RESP + 1];
			break;
	}
	return os;
}

#define TLM_DECLARE_EXTENDED_PHASE(name_arg) \
class tlm_phase_##name_arg : public tlm::tlm_phase \
{\
public:\
	static const tlm_phase_##name_arg& get_phase();\
}; \
const tlm_phase_##name_arg& tlm_phase_##name_arg::get_phase()\
{\
	tlm_phase::last_extended_phase_value++;\
	tlm_phase::extended_phase_name.push_back(#name_arg);\
	static tlm_phase extended_phase(tlm_phase::last_extended_phase_value);\
	return extended_phase;\
}\
static const tlm_phase_##name_arg& name_arg=tlm_phase_##name_arg::get_phase()

} // namespace tlm

#endif // __LIBIEEE1666_TLM2_INTEROPERABILITY_LAYER_PHASE_BASE_PHASE_H__
