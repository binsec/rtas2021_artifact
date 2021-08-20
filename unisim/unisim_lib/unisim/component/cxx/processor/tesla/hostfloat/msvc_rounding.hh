/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_MSVC_ROUNDING_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_MSVC_ROUNDING_HH

#include <float.h>
#include <cassert>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {
namespace hostfloat {

struct BaseRoundingControl
{
	typedef unisim::component::cxx::processor::tesla::RoundingMode RM;
private:
	static int TeslaToX87(RM rm) {
		switch(rm) {
		case RM_RN: return _RC_NEAR;
		case RM_RD: return _RC_DOWN;
		case RM_RU: return _RC_UP;
		case RM_RZ: return _RC_CHOP;
		default: assert(false);
		}
	}
	static RM X87ToTesla(int rm) {
		switch(rm) {
		case _RC_NEAR: return RM_RN;
		case _RC_DOWN: return RM_RD;
		case _RC_UP: return RM_RU;
		case _RC_CHOP: return RM_RZ;
		default: assert(false);
		}
	}
	
public:
	static void SetRoundingMode(RM rm) {
		_controlfp(TeslaToX87(rm), _MCW_RC);
	}

	static RM GetRoundingMode() {
		return X87ToTesla(_controlfp(0, _MCW_RC));
	}
	
	template<class T>
	static T ForceRounding(T src) {
		volatile T r = src;
		return r;
	}

	template<class T>
	static T ToInt(T src) {
		return nearbyint(src);
	}
};


} // end of namespace hostfloat
} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
