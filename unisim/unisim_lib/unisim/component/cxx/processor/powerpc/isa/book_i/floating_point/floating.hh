/*
 *  Copyright (c) 2007,
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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_I_FLOATING_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_I_FLOATING_HH__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

template <class FPSCR, class FLOAT>
inline void GenFPSCR_FPRF(typename FPSCR::TYPE& fpscr, const FLOAT& result)
{
	if(unlikely(result.isQNaN()))
	{
		FPSCR::FPRF::Set(fpscr,FPSCR::FPRF::QNAN());
	}
	else if(unlikely(result.isInfty()))
	{
		if(result.isNegative())
			FPSCR::FPRF::Set(fpscr,FPSCR::FPRF::NEGATIVE_INFINITY());
		else
			FPSCR::FPRF::Set(fpscr,FPSCR::FPRF::POSITIVE_INFINITY());
	}
	else if(unlikely(result.isDenormalized()))
	{
		if(result.isNegative())
			FPSCR::FPRF::Set(fpscr,FPSCR::FPRF::NEGATIVE_DENORMAL());
		else
			FPSCR::FPRF::Set(fpscr,FPSCR::FPRF::POSITIVE_DENORMAL());
	}
	else if(unlikely(result.isZero()))
	{
		if(result.isNegative())
			FPSCR::FPRF::Set(fpscr,FPSCR::FPRF::NEGATIVE_ZERO());
		else
			FPSCR::FPRF::Set(fpscr,FPSCR::FPRF::POSITIVE_ZERO());
	}
	else
	{
		if(result.isNegative())
			FPSCR::FPRF::Set(fpscr,FPSCR::FPRF::NEGATIVE_NORMAL());
		else
			FPSCR::FPRF::Set(fpscr,FPSCR::FPRF::POSITIVE_NORMAL());
	}
}

template <class FPSCR, class FLOAT>
inline void GenFPSCR_FR(typename FPSCR::TYPE& fpscr, const FLOAT& result, const Flags& flags)
{
	FPSCR::FR::Set(fpscr, typename FPSCR::TYPE(flags.hasIncrementFraction(result.isNegative())));
}

template <class FPSCR>
inline void GenFPSCR_OX(typename FPSCR::TYPE& fpscr, const Flags& flags)
{
	if(unlikely(flags.isOverflow() and flags.isApproximate()))
	{
		FPSCR::OX::Set(fpscr, typename FPSCR::TYPE(1));
	}
}

template <class FPSCR>
inline void GenFPSCR_UX(typename FPSCR::TYPE& fpscr, const Flags& flags)
{
	if(unlikely(flags.isUnderflow() and flags.isApproximate()))
	{
		FPSCR::UX::Set(fpscr, typename FPSCR::TYPE(1));
	}
}

template <class FPSCR>
inline void GenFPSCR_VXSNAN(typename FPSCR::TYPE& fpscr, const Flags& flags)
{
	if(unlikely(flags.hasSNaNOperand()))
	{
		FPSCR::VXSNAN::Set(fpscr, typename FPSCR::TYPE(1));
	}
}

template <class FPSCR>
inline void GenFPSCR_VXISI(typename FPSCR::TYPE& fpscr, const Flags& flags)
{
	if(unlikely(flags.isInftyMinusInfty()))
	{
		FPSCR::VXISI::Set(fpscr, typename FPSCR::TYPE(1));
	}
}

template <class FPSCR>
inline void GenFPSCR_VXCVI(typename FPSCR::TYPE& fpscr, const Flags& flags)
{
	if(unlikely(flags.isOverflow()))
	{
		FPSCR::VXCVI::Set(fpscr, typename FPSCR::TYPE(1));
	}
}

template <class FPSCR>
inline void GenFPSCR_FX(typename FPSCR::TYPE& fpscr, typename FPSCR::TYPE old_fpscr)
{
	typename FPSCR::TYPE posedges = ~old_fpscr & fpscr;

	if (unlikely(FPSCR::OX::Get(posedges) or
	             FPSCR::UX::Get(posedges) or
	             FPSCR::ZX::Get(posedges) or
	             FPSCR::XX::Get(posedges) or
	             FPSCR::VXSNAN::Get(posedges) or
	             FPSCR::VXISI::Get(posedges) or
	             FPSCR::VXIDI::Get(posedges) or
	             FPSCR::VXZDZ::Get(posedges) or
	             FPSCR::VXIMZ::Get(posedges) or
	             FPSCR::VXVC::Get(posedges) or
	             FPSCR::VXSOFT::Get(posedges) or
	             FPSCR::VXSQRT::Get(posedges) or
	             FPSCR::VXCVI::Get(posedges)))
	{
		FPSCR::FX::Set(fpscr,typename FPSCR::TYPE(1));
	}
}

template <class FPSCR>
inline void GenFPSCR_VX(typename FPSCR::TYPE& fpscr)
{
	typename FPSCR::TYPE vx(
	  FPSCR::VXSNAN::Get(fpscr) or
	  FPSCR::VXISI::Get(fpscr) or
	  FPSCR::VXIDI::Get(fpscr) or
	  FPSCR::VXZDZ::Get(fpscr) or
	  FPSCR::VXIMZ::Get(fpscr) or
	  FPSCR::VXVC::Get(fpscr) or
	  FPSCR::VXSOFT::Get(fpscr) or
	  FPSCR::VXSQRT::Get(fpscr) or
	  FPSCR::VXCVI::Get(fpscr)
	);
	
	FPSCR::VX::Set(fpscr, vx);
}

template <class FPSCR>
inline void GenFPSCR_FEX(typename FPSCR::TYPE& fpscr)
{
	typename FPSCR::TYPE fex(
	  (FPSCR::VX::Get(fpscr) and FPSCR::VE::Get(fpscr)) or
	  (FPSCR::OX::Get(fpscr) and FPSCR::OE::Get(fpscr)) or
	  (FPSCR::UX::Get(fpscr) and FPSCR::UE::Get(fpscr)) or
	  (FPSCR::ZX::Get(fpscr) and FPSCR::ZE::Get(fpscr)) or
	  (FPSCR::XX::Get(fpscr) and FPSCR::XE::Get(fpscr))
	);
	
	FPSCR::FEX::Set(fpscr, fex);
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_I_FLOATING_HH__
