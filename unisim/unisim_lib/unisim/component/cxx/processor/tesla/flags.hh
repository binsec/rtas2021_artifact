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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_FLAGS_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_FLAGS_HH__

//#include <boost/integer.hpp>
#include <bitset>
#include <unisim/component/cxx/processor/tesla/register.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

//using namespace boost;
using std::bitset;

// From Decuda
enum Cond
{
	CD_FL = 0,
	CD_LT = 1,
	CD_EQ = 2,
	CD_LE = 3,
	CD_GT = 4,
	CD_NE = 5,
	CD_GE = 6,
	CD_LEG = 7,
	CD_NAN = 8,
	CD_LTU = 9,
	CD_EQU = 10,
	CD_LEU = 11,
	CD_GTU = 12,
	CD_NEU = 13,
	CD_GEU = 14,
	CD_TR = 15,
	CD_OF = 16,
	CD_CF = 17,
	CD_AB = 18,
	CD_ST = 19,
	// ... Unknown flags
	CD_NSF = 28,
	CD_BLE = 29,
	CD_NCF = 30,
	CD_NOF = 31
};

enum SetCond
{
	// Actually a LUT
	// addressed by SZ flags
	// Unordered comparisons?
	SC_FL = 0,
	SC_LT = 1,
	SC_EQ = 2,
	SC_LE = 3,
	SC_GT = 4,
	SC_NE = 5,
	SC_GE = 6,
	SC_TR = 7,
};


template <class CONFIG>
struct VectorFlags
{
	
	void Write(VectorFlags<CONFIG> const & f, bitset<CONFIG::WARP_SIZE> mask);
	VectorFlags<CONFIG> Reset();
	
	void SetZero(int z, int lane);
	int GetZero(int lane) const;
	void SetSign(int s, int lane);
	int GetSign(int lane) const;
	void SetCarry(int c, int lane);
	int GetCarry(int lane) const;
	void SetOvf(int o, int lane);
	int GetOvf(int lane) const;
	
	bitset<4> operator[](unsigned int i) const;
	bitset<4> & operator[](unsigned int i);

	bitset<4> v[CONFIG::WARP_SIZE];
	
};

template <class CONFIG>
std::ostream & operator << (std::ostream & os, VectorFlags<CONFIG> const & r);

std::string CondCodeString(Cond c);
std::string SetCondString(SetCond c);

bool IsPredSet(uint32_t cond, bitset<4> flags);

template<class CONFIG>
bitset<CONFIG::WARP_SIZE> IsPredSet(uint32_t cond, VectorFlags<CONFIG> vf);

template<class CONFIG>
VectorFlags<CONFIG> ComputePredI32(VectorRegister<CONFIG> const & output, VectorFlags<CONFIG> flags);

template<class CONFIG>
VectorFlags<CONFIG> ComputePredI16(VectorRegister<CONFIG> const & output, VectorFlags<CONFIG> flags);

template<class CONFIG>
VectorFlags<CONFIG> ComputePredSetI(VectorRegister<CONFIG> & output,
	VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	SetCond sc,
	bool is_signed,
	bool b32);

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
