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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_DENORMALS_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_DENORMALS_HH

#include <unisim/component/cxx/processor/tesla/hostfloat/traits.hh>

// TODO: use platform-specific implementations on X86-64, i386-SSE, IA-64, Alpha, etc.

#include <cfloat>
#include <cmath>
#include <cstdlib>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {
namespace hostfloat {



// Fallback when no hardware support for DAZ/FTZ is available:
struct BaseDenormControl
{
	static void SetDAZ(bool) { }
	static bool GetDAZ() { return true; }
	
	static void SetRTZ(bool) { }
	static bool GetRTZ() { return true; }
	
	template<class T>
	static T ForceDAZ(T r) {
		if(fabs(r) < FloatTraits<T>::MinNormal()) {
			r = FloatTraits<T>::Zero();
		}
		return r;
	}
	
	template<class T>
	static T ForceFTZ(T r) {
		if(fabs(r) < FloatTraits<T>::MinNormal()) {
			r = FloatTraits<T>::Zero();
		}
		return r;
	}
};

// RAII wrapper for setting denormal modes
template<class Control = BaseDenormControl>
struct TDenormals
{
private:
	bool saveddaz;
	bool savedftz;
	bool restore;

public:
	TDenormals(bool daz, bool ftz) {
		saveddaz = Control::GetDAZ();
		savedftz = Control::GetRTZ();
		if(saveddaz != daz || savedftz != ftz) {
			restore = true;
			Control::SetDAZ(daz);
			Control::SetRTZ(ftz);
		}
		else {
			restore = false;
		}
	}

	~TDenormals() {
		if(restore) {
			Control::SetDAZ(saveddaz);
			Control::SetRTZ(savedftz);
		}
	}


};

} // end of namespace hostfloat
} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
