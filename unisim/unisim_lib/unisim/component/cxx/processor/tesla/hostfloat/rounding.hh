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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_ROUNDING_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_HOSTFLOAT_ROUNDING_HH

#include <unisim/component/cxx/processor/tesla/register.hh>

// Select appropriate platform-specific rounding control wrapper
#if defined(__LINUX__) || defined(__APPLE_CC__) || defined(MINGW) || defined(__USE_ISOC99) || defined(__MSL__) || defined(__MWERKS__) || defined(__ICC)
#include <unisim/component/cxx/processor/tesla/hostfloat/c99_rounding.hh>
#elif defined(_MSC_VER) || defined(__BORLANDC__)
#include <unisim/component/cxx/processor/tesla/hostfloat/msvc_rounding.hh>
#else
#error "Unsupported platform. Please update hostfloat/rounding.hh."
#endif


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {
namespace hostfloat {


// RAII wrapper for setting rounding modes
template<class Control = BaseRoundingControl>
struct TRounding
{
	typedef unisim::component::cxx::processor::tesla::RoundingMode RoundingMode;

	TRounding(RoundingMode newrm) {
		//savedrm = Control::GetRoundingMode();
		//if(savedrm == newrm) {
		if(newrm == RM_RN) {
			restore = false;
		}
		else {
			Control::SetRoundingMode(newrm);
			restore = true;
		}
	}

	~TRounding() {
		if(restore) {
			//Control::SetRoundingMode(savedrm);
			Control::SetRoundingMode(RM_RN);
		}
	}

private:
	//RoundingMode savedrm;
	bool restore;

};



} // end of namespace hostfloat
} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
