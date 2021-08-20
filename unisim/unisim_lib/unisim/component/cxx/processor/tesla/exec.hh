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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXEC_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXEC_HH

#include <unisim/component/cxx/processor/tesla/register.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG>
VectorRegister<CONFIG> IAdd(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorFlags<CONFIG> & flags,
	bool sat = false, bool ra = false, bool rb = false,
	bool m32 = true);	// No carry in

template<class CONFIG>
VectorRegister<CONFIG> IAddC(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorFlags<CONFIG> & flags,
	VectorFlags<CONFIG> const & inputflags,
	bool sat = false, bool ra = false, bool rb = false,
	bool m32 = true, bool use_cin = true);


template<class CONFIG>
VectorRegister<CONFIG> IAdd16(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorFlags<CONFIG> & flags,
	VectorFlags<CONFIG> const & inputflags,
	bool sat = false, bool ra = false, bool rb = false, bool use_cin = false);

template<class CONFIG>
VectorRegister<CONFIG> IAdd32(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorFlags<CONFIG> & flags,
	VectorFlags<CONFIG> const & inputflags,
	bool sat = false, bool ra = false, bool rb = false, bool use_cin = false);


template<class CONFIG>
VectorRegister<CONFIG> Mul24(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	bool sat, bool ra, bool rb, bool m24,
	bool issigned, bool hi);

template<class CONFIG>
VectorRegister<CONFIG> Mad24(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorRegister<CONFIG> const & c,
	VectorFlags<CONFIG> & flags,
	bool sat,
	bool src1_neg,
	bool src3_neg,
	bool m24,
	bool issigned,
	bool hi);


template<class CONFIG>
VectorRegister<CONFIG> Mad24C(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorRegister<CONFIG> const & c,
	VectorFlags<CONFIG> & flags,
	VectorFlags<CONFIG> const & inputflags,
	bool sat,
	bool src1_neg,
	bool src3_neg,
	bool m24,
	bool issigned,
	bool hi,
	bool use_cin = true);


template<class CONFIG>
VectorRegister<CONFIG> ConvertIntInt(VectorRegister<CONFIG> const & a, uint32_t cvt_round, uint32_t cvt_type, bool b32, AbsSat abssat, bool neg = false);

template<class CONFIG>
VectorRegister<CONFIG> ShiftLeft(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	bool u32, bool issigned);

template<class CONFIG>
VectorRegister<CONFIG> ShiftRight(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	bool u32, bool issigned);

template<class CONFIG>
VectorRegister<CONFIG> ShiftLeft(VectorRegister<CONFIG> const & a, uint32_t sb, bool u32);

template<class CONFIG>
VectorRegister<CONFIG> ShiftRight(VectorRegister<CONFIG> const & a, uint32_t sb, bool u32, bool issigned);


template<class CONFIG>
VectorRegister<CONFIG> BinNeg(VectorRegister<CONFIG> const & a);

template<class CONFIG>
VectorRegister<CONFIG> BinAnd(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b);

template<class CONFIG>
VectorRegister<CONFIG> BinOr(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b);

template<class CONFIG>
VectorRegister<CONFIG> BinXor(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b);

template<class CONFIG>
VectorRegister<CONFIG> Min(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	unsigned int m32, unsigned int issigned);

template<class CONFIG>
VectorRegister<CONFIG> Max(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	unsigned int m32, unsigned int issigned);

template<class CONFIG>
void FlagsToReg(VectorRegister<CONFIG> & dest, VectorFlags<CONFIG> const & src);

template<class CONFIG>
void RegToFlags(VectorFlags<CONFIG> & dest, VectorRegister<CONFIG> const & src);

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
