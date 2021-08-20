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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_ENUMS_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_ENUMS_HH

#include <cassert>
#include <stdint.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

enum DataType
{
	DT_U8 = 0,
	DT_S8 = 1,
	DT_U16 = 2,
	DT_S16 = 3,
	DT_U64 = 4,
	DT_U128 = 5,
	DT_U32 = 6,
	DT_S32 = 7,
	DT_F32,		// Not in Tesla ISA
	DT_F64,     //
	DT_NONE,	//
	DT_UNKNOWN	//
};

enum ConvType
{
	CT_U16 = 0,
	CT_U32 = 1,
	CT_U8 = 2,
	CT_U32U8 = 3,	// 8 lower bits of a 32-bit reg
	
	CT_S16 = 4,
	CT_S32 = 5,
	CT_S8 = 6,
	CT_NONE = 7	// Not in Decuda
};

enum RoundingMode
{
	RM_RN = 0,
	RM_RD = 1,
	RM_RU = 2,
	RM_RZ = 3
};

enum SMType
{
	SM_U8 = 0,
	SM_U16 = 1,
	SM_S16 = 2,
	SM_U32 = 3
};

// Does not match any ISA field
// ... except b32.
enum RegType
{
	RT_U16 = 0,
	RT_U32 = 1,
	RT_U64,		// 2 regs, Not in ISA
};

enum AbsSat
{
	AS_NONE = 0,
	AS_SAT = 1,
	AS_ABS = 2,
	AS_SSAT = 3
};

inline SMType MvSizeToSMType(uint32_t mv_size);
inline RegType CvtTypeToRT(ConvType ct);
inline DataType RegTypeToDataType(RegType rt);
inline DataType MvSizeToDataType(uint32_t mv_size);
inline DataType CvtTypeToDataType(ConvType ct);
inline size_t DataTypeSize(DataType dt);
inline unsigned int DataTypeLogSize(DataType dt);
inline DataType SMTypeToDataType(SMType st);
inline ConvType SMTypeToConvType(SMType st);


inline SMType MvSizeToSMType(uint32_t mv_size)
{
	switch(mv_size) {
	case 0:
		return SM_U16;
	case 1:
		return SM_U32;
	case 2:
		return SM_U8;
	default:
		assert(false);
	}
}

inline RegType CvtTypeToRT(ConvType ct)
{
	switch(ct) {
	case CT_U16:
	case CT_S16:
	case CT_U8:
	case CT_S8:
		return RT_U16;
	case CT_U32:
	case CT_S32:
	case CT_U32U8:
	case CT_NONE:
		return RT_U32;
	default:
		assert(false);
	}
}

inline DataType RegTypeToDataType(RegType rt)
{
	switch(rt) {
	case RT_U16:
		return DT_U16;
	case RT_U32:
		return DT_U32;
	default:
		assert(false);
	}
}

inline DataType MvSizeToDataType(uint32_t mv_size)
{
	switch(mv_size) {
	case 0:
		return DT_U16;
	case 1:
		return DT_U32;
	case 2:
		return DT_U8;
	default:
		assert(false);
	}
}

inline DataType CvtTypeToDataType(ConvType ct)
{
	switch(ct) {
	case CT_U16:
		return DT_U16;
	case CT_S16:
		return DT_S16;
	case CT_U8:
		return DT_U8;
	case CT_U32U8:
		return DT_U32;	// As far as registers are concerned...
	case CT_S8:
		return DT_S8;
	case CT_U32:
	case CT_NONE:
		return DT_U32;
	case CT_S32:
		return DT_S32;
	default:
		assert(false);
	}
}

inline size_t DataTypeSize(DataType dt)
{
	switch(dt) {
	case DT_U8:
	case DT_S8:
		return 1;
	case DT_U16:
	case DT_S16:
		return 2;
	case DT_U32:
	case DT_S32:
	case DT_F32:
		return 4;
	case DT_U64:
	case DT_F64:
		return 8;
	case DT_U128:
		return 16;
	default:
		assert(false);
	}
}

inline unsigned int DataTypeLogSize(DataType dt)
{
	switch(dt) {
	case DT_U8:
	case DT_S8:
		return 0;
	case DT_U16:
	case DT_S16:
		return 1;
	case DT_U32:
	case DT_S32:
	case DT_F32:
		return 2;
	case DT_U64:
	case DT_F64:
		return 3;
	case DT_U128:
		return 4;
	default:
		assert(false);
	}
}

inline DataType SMTypeToDataType(SMType st)
{
	switch(st) {
	case SM_U8:
		return DT_U8;
	case SM_U16:
		return DT_U16;
	case SM_S16:
		return DT_S16;
	case SM_U32:
		return DT_U32;
	default:
		assert(false);
	}
}

inline ConvType SMTypeToConvType(SMType st)
{
	switch(st) {
	case SM_U8:
		return CT_U8;
	case SM_U16:
		return CT_U16;
	case SM_S16:
		return CT_S16;
	case SM_U32:
		return CT_U32;
	default:
		assert(false);
	}
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
