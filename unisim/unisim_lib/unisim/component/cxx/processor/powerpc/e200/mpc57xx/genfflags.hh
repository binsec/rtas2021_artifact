/*
 *  Copyright (c) 2007-2017,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_GENFFLAGS_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_GENFFLAGS_HH__

#include <unisim/component/cxx/processor/powerpc/e200/mpc57xx/floating.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

template <class CPU>
inline bool HasGenSPEFSCR(CPU* cpu)
{
	// if(!CONFIG::HAS_FPU)
	// {
	//	 cpu->SetException(CONFIG::EXC_PROGRAM_UNIMPLEMENTED_INSTRUCTION);
	//	 // cpu->template ThrowException<typename CPU::ProgramInterrupt::UnimplementedInstruction>();
	//	 return false;
	// }
	// Check if FPU is enabled
	/* if(unlikely(!cpu->GetMSR().template Get<typename CPU::MSR::SPV>()))
	{
		cpu->template ThrowException<typename CPU::ProgramInterrupt::UnimplementedInstruction>();
		// SPE/embedded floating-point unavailable exception (causes the SPE/embedded floating-point
		//	 unavailable interrupt)—IVOR32 (SPR 528)
		// cpu->template ThrowException<typename CPU::ProgramInterrupt::UnavailableInterrupt>();
		// see cpu.hh
		// cpu->SetException(CONFIG::EXC_FLOATING_POINT_UNAVAILABLE);
		return false;
	} */
	return true;
}

template <class SPEFSCR, class FLOAT>
inline void GenSPEFSCR_FINV(SPEFSCR& spefscr, const FLOAT& first,
		const FLOAT* second = NULL, const FLOAT* third = NULL, bool isDivideOperation=false)
{
	spefscr.template Set<typename SPEFSCR::FINV>(false);
	if (unlikely(first.hasInftyExponent() || first.isDenormalized()))
		spefscr.template Set<typename SPEFSCR::FINV>(true);
	else if (unlikely(second && (second->hasInftyExponent() || second->isDenormalized())))
		spefscr.template Set<typename SPEFSCR::FINV>(true);
	else if (unlikely(third && (third->hasInftyExponent() || third->isDenormalized())))
		spefscr.template Set<typename SPEFSCR::FINV>(true);
	else if (unlikely(isDivideOperation && second && first.isZero() && second->isZero()))
		spefscr.template Set<typename SPEFSCR::FINV>(true);
}

template <class SPEFSCR, class FLOAT>
inline void GenSPEFSCR_DefaultResults(SPEFSCR& spefscr, FLOAT& result)
{
	spefscr.template Set<typename SPEFSCR::FGH>(false);
	spefscr.template Set<typename SPEFSCR::FXH>(false);
	spefscr.template Set<typename SPEFSCR::FG>(false);
	spefscr.template Set<typename SPEFSCR::FX>(false);
	// default results are provided by the hardware when an infinity, denormalized, or NaN input is received
}

template <class SPEFSCR>
inline void GenSPEFSCR_FOVF(SPEFSCR& spefscr, const Flags& flags)
{
	spefscr.template Set<typename SPEFSCR::FOVF>(false);
	if (flags.isOverflow())
		spefscr.template Set<typename SPEFSCR::FOVF>(true);
}

template <class SPEFSCR>
inline void GenSPEFSCR_FUNF(SPEFSCR& spefscr, const Flags& flags)
{
	spefscr.template Set<typename SPEFSCR::FUNF>(false);
	if (flags.isUnderflow())
		spefscr.template Set<typename SPEFSCR::FUNF>(true);
}

template <class SPEFSCR>
inline void GenSPEFSCR_FINXS(SPEFSCR& spefscr, const Flags& flags)
{
	if (flags.isApproximate())
		spefscr.template Set<typename SPEFSCR::FINXS>(true);
	else if (spefscr.template Get<typename SPEFSCR::FOVFE>()
			&& (spefscr.template Get<typename SPEFSCR::FOVF>() || spefscr.template Get<typename SPEFSCR::FOVFH>()))
		spefscr.template Set<typename SPEFSCR::FINXS>(true);
	else if (spefscr.template Get<typename SPEFSCR::FUNFE>()
			&& (spefscr.template Get<typename SPEFSCR::FUNF>() || spefscr.template Get<typename SPEFSCR::FUNFH>()))
		spefscr.template Set<typename SPEFSCR::FINXS>(true);
}

template <class SPEFSCR>
inline void GenSPEFSCR_FDBZ(SPEFSCR& spefscr, const Flags& flags)
{
	if (flags.isDivisionByZero())
		spefscr.template Set<typename SPEFSCR::FDBZ>(true);
}

template <class SPEFSCR, class CONFIG>
inline void GenSPEFSCR_FG_FX(SPEFSCR& spefscr, const Flags& flags, bool isResultNegative)
{
	if (spefscr.template Get<typename SPEFSCR::FINV>() || flags.isOverflow() || flags.isUnderflow())
		spefscr.template Set<typename SPEFSCR::FG>(false);
	else {
		if (likely(flags.isApproximate())) {
			if (unlikely(flags.hasEffectiveRoundToEven())) {
				spefscr.template Set<typename SPEFSCR::FG>(true);
				spefscr.template Set<typename SPEFSCR::FX>(false);
			}
			else {
				spefscr.template Set<typename SPEFSCR::FG>(!isResultNegative == flags.isUpApproximate());
				spefscr.template Set<typename SPEFSCR::FX>(true);
			};
		};
	};
}

template <class FLOAT>
inline void GenSPEFSCR_ExactResult(FLOAT& result, const Flags& flags, unsigned int rounding_mode)
{
	if (rounding_mode == RN_ZERO) {
		if (result.isPositive() ? flags.isUpApproximate() : flags.isDownApproximate()) {
			if (result.isPositive())
				result.setToPrevious();
			else
				result.setToNext();
			if (unlikely(result.isDenormalized())) {
				bool isNegative = result.isNegative();
				result.clear();
				result.setNegative(isNegative);
			};
		};
	}
	else if (rounding_mode == RN_UP) {
		if (flags.isDownApproximate()) {
			result.setToNext();
			if (result.isDenormalized()) {
				bool isNegative = result.isNegative();
				result.clear();
				result.setNegative(isNegative);
				// first normalized ?
			};
			if (unlikely(result.hasInftyExponent()))
				result.setToPrevious();
		};
	}
	else if (rounding_mode == RN_DOWN) {
		if (flags.isUpApproximate()) {
			result.setToPrevious();
			if (result.isDenormalized()) {
				bool isNegative = result.isNegative();
				result.clear();
				result.setNegative(isNegative);
				// first normalized ?
			};
			if (unlikely(result.hasInftyExponent()))
				result.setToNext();
		};
	}
}

template <class SPEFSCR>
inline void GenSPEFSCR_FX(SPEFSCR& spefscr, const Flags& flags)
{
	if (flags.isApproximate())
		spefscr.template Set<typename SPEFSCR::FX>(true);
	if (spefscr.template Get<typename SPEFSCR::FINV>()  || flags.isOverflow() || flags.isUnderflow())
		spefscr.template Set<typename SPEFSCR::FX>(false);
}

template <class SPEFSCR>
inline bool DoesSPEFSCR_TriggerException(const SPEFSCR& spefscr)
{	// Check for floating point exception condition:
	return (spefscr.template Get<typename SPEFSCR::FINXS>() && spefscr.template Get<typename SPEFSCR::FINXE>())
		  || ((spefscr.template Get<typename SPEFSCR::FINV>() || spefscr.template Get<typename SPEFSCR::FINVH>())
				&& spefscr.template Get<typename SPEFSCR::FINVE>())
		  || ((spefscr.template Get<typename SPEFSCR::FDBZ>() || spefscr.template Get<typename SPEFSCR::FDBZH>())
				&& spefscr.template Get<typename SPEFSCR::FDBZE>())
		  || ((spefscr.template Get<typename SPEFSCR::FUNF>() || spefscr.template Get<typename SPEFSCR::FUNFH>())
				&& spefscr.template Get<typename SPEFSCR::FUNFE>())
		  || ((spefscr.template Get<typename SPEFSCR::FOVF>() || spefscr.template Get<typename SPEFSCR::FOVFH>())
				&& spefscr.template Get<typename SPEFSCR::FOVFE>());
}

template <class CPU>
inline void GenSPEFSCR_TriggerException(CPU* cpu)
{
	// Raise a floating point exception
	cpu->template ThrowException<typename CPU::ProgramInterrupt::UnimplementedInstruction>();
}

template <class SPEFSCR, class FLOAT>
inline void GenSPEFSCR_FG(SPEFSCR& spefscr, const FLOAT& result)
{
	spefscr.template Set<typename SPEFSCR::FG>(false);
}

} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

