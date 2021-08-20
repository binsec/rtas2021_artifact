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
 
#include <unisim/component/cxx/processor/tesla/disasm.hh>
#include <ostream>
#include <cassert>
#include <iomanip>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using namespace std;

//=====================================================================
//=                Disassembly helper functions                       =
//=====================================================================

void DisasmReg(uint32_t reg, RegType rt, ostream & os)
{
	switch(rt)
	{
	case RT_U16:
		{
		uint32_t rnum = reg >> 1;
		uint32_t hilo = (reg & 1);
		os << "r" << rnum << (hilo ? ".hi" : ".lo");
		}
		break;
	case RT_U32:
		os << "r" << reg;
		break;
	case RT_U64:
		os << "r" << reg + 1 << ":r" << reg;
		break;
	}
}

void DisasmPred(uint32_t pred_cond, uint32_t pred_reg, ostream & buffer)
{
	if(pred_cond != CD_TR)
	{
		buffer << "@$p" << pred_reg << "." << CondCodeString(Cond(pred_cond)) << " ";
	}
}


void DisasmRounding(uint32_t rounding_mode, ostream & buffer)
{
	buffer << RoundingModeString(RoundingMode(rounding_mode));
}

void DisasmMarker(uint32_t marker, ostream & buffer)
{
	switch(marker)
	{
	case 0:
		break;
	case 1:
		buffer << ".end";
		break;
	case 2:
		buffer << ".join";
		break;
	case 3:
		assert(false);
		break;
	}
}

#if 0
void DisasmSetPred(uint32_t set_pred_reg, uint32_t set_pred, ostream & buffer)
{
	if(set_pred)
		buffer << "$p" << set_pred_reg << "|";
}

void DisasmDest(uint32_t dest, uint32_t ignore_output, ostream & buffer)
{
	if(!ignore_output)
	{
		buffer << "$r" << dest;
	}
	else
	{
		buffer << "_";
	}
}
#endif

void DisasmSrc(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg, ostream & buffer)
{
	// TODO: segment??
	// TODO: handle 16-bit operands (hi, lo)
	assert(!(cm && sh));
	if(cm)
		buffer << "c[" << reg * 4 << "]";
	else if(sh)
		buffer << "s[" << reg * 4 << "]";
	else
		buffer << "$r" << reg;
}

#if 0
void DisasmSrc(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg, uint32_t addr_lo,
	uint32_t addr_hi, uint32_t addr_imm, ostream & buffer)
{
	uint32_t addr_reg = (addr_hi << 2) | addr_lo;

	// addr_imm = ???	
	if(addr_imm == 0)
	{
		DisasmSrc(reg, cm, sh, neg, buffer);
	}
	else
	{
		// Indirect access:
		// [$a#addr_reg + reg * 4]
		buffer << "???";	// TODO
	}
}

#endif

void DisasmImm(uint32_t imm_hi, uint32_t imm_lo, ostream & buffer)
{
	// imm_lo 7 bits???
	assert(!(imm_lo & (~0x3f)));
	uint32_t imm = (imm_hi << 6) || imm_lo;
	
	buffer << hex << imm;
}

void DisasmConvert(uint32_t cvt_round, uint32_t cvt_type, bool sign,
	uint32_t data_32, uint32_t abssat, ostream & buffer)
{
//	if(data_32 == 0)
//		buffer << ".b16";
	DisasmSignWidth(sign, data_32, buffer);
	buffer << ConvTypeString(ConvType(cvt_type))
		<< RoundingModeString(RoundingMode(cvt_round))
		<< AbsSatString(AbsSat(abssat));
}

void DisasmConvertSharedConst(RoundingMode cvt_round, SMType srctype, bool sign,
	bool dest32, AbsSat abssat, ostream & buffer)
{
	DisasmSignWidth(sign, dest32, buffer);
	buffer << SMTypeString(srctype)
		<< RoundingModeString(cvt_round)
		<< AbsSatString(abssat);
}

void DisasmConvertFP32(bool dest_32, ConvType srct, RoundingMode cvt_round,
	bool cvt_int, AbsSat as, ostream & buffer)
{
	if(dest_32) {
		buffer << ".f32";
	}
	else {
		buffer << ".f16";
	}
	if(srct == CT_U32) {
		buffer << ".f32";
	}
	else {
		assert(srct == CT_U16);
		buffer << ".f16";
	}
	buffer << RoundingModeString(RoundingMode(cvt_round));
	if(cvt_int) {
		buffer << "i";
	}
	buffer << AbsSatString(as);
}

void DisasmDataType(uint32_t dt, ostream & buffer)
{
	buffer << DataTypeString(DataType(dt));
}

std::string DataTypeString(DataType d)
{
	static char const * dt_desc[] = {
		".u8",	// 0
		".s8",
		".u16",
		".s16",
		".u64",
		".u128",
		".u32",
		".s32"	// 7
	};
	assert(d >= 0 && d < 8);
	return string(dt_desc[d]);
}

std::string RoundingModeString(RoundingMode r)
{
	static char const * rm_desc[] = {
		".rn",	// 0
		".rd",
		".ru",
		".rz"	// 3
	};
	assert(r >= 0 && r < 4);
	return string(rm_desc[r]);
}

std::string ConvTypeString(ConvType t)
{
	static char const * ct_desc[] = {
		".u16",	// 0
		".u32",
		".u8",
		".u8",	// from 32-bit reg
		".s16",
		".s32",
		".s8",
		""	// 7
	};
	assert(t >= 0 && t < 8);
	return string(ct_desc[t]);
}

std::string SMTypeString(SMType t)
{
	static char const * sm_desc[] = {
		".u8",	// 0
		".u16",
		".s16",
		".u32"	// 3
	};
	assert(t >= 0 && t < 4);
	return string(sm_desc[t]);
}

std::string AbsSatString(AbsSat as)
{
	static char const * as_desc[] = {
		"",	// 0
		".sat",
		".abs",
		".ssat"	// 3
	};
	assert(as >= 0 && as < 4);
	return string(as_desc[as]);
}

void DisasmSign(uint32_t sign, ostream & buffer)
{
	if(sign)
		buffer << "-";
}

void DisasmAddress(uint32_t reg, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t shift, ostream & buffer, bool addr_inc)
{
	uint32_t addr_reg = (addr_hi << 2) | addr_lo;
	if(addr_reg != 0)
	{
		buffer << "a" << addr_reg << "+";
		if(addr_inc) {
			buffer << "=";
		}
	}
	else {
		assert(!addr_inc);
	}

	if(addr_imm)
	{
		// s[dest]
		// Immediate in words
		buffer << "0x" << std::setfill('0') << std::setw(4) << std::hex << (reg << shift) << dec;
	}
	else
	{
		// s[r_dest]
		buffer << "r" << reg;
	}
}


void DisasmGlobal(uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, uint32_t dt, ostream & buffer)
{
	buffer << "g" << segment << "[";

	int shift = 0;
	if(dt == DT_U16 || dt == DT_S16) shift = 1;	// 16-bit
	else if(dt == DT_U32 || dt == DT_S32) shift = 2;	// 32-bit
	else if(dt == DT_U64) shift = 3;	// 32-bit
	else if(dt == DT_U128) shift = 4;
	
	DisasmAddress(dest, addr_lo, addr_hi, addr_imm, shift, buffer);
	
	buffer << "]";
}

void DisasmLocal(uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, uint32_t dt, ostream & buffer, bool addr_inc)
{
	buffer << "l" << segment << "[";

	int shift = 0;
/*	if(dt == DT_U16 || dt == DT_S16) shift = 1;	// 16-bit
	else if(dt == DT_U32 || dt == DT_S32) shift = 2;	// 32-bit
	else if(dt == DT_U64) shift = 3;	// 32-bit
	else if(dt == DT_U128) shift = 4;*/
	// Local memory always byte-indexed
	
	DisasmAddress(dest, addr_lo, addr_hi, addr_imm, shift, buffer, addr_inc);
	
	buffer << "]";
}

void DisasmShared(uint32_t addr, uint32_t type, ostream & buffer)
{
	static int sm_size[] = {
		1, 2, 2, 4
	};
	SMType smt = SMType(type);
	assert(smt < 4);
	addr *= sm_size[smt];
	buffer << "s" << SMTypeString(smt)
		<< "[" << "0x" << std::setfill('0') << std::setw(4) << std::hex << addr << "]";
	buffer << std::dec;
}

void DisasmShared(uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, SMType type, ostream & buffer, bool addr_inc)
{
	buffer << "s" << "[";
	
	int shift = 0;
	if(type == SM_U16 || type == SM_S16) shift = 1;
	else if(type == SM_U32) shift = 2;

	DisasmAddress(dest, addr_lo, addr_hi, addr_imm, shift, buffer, addr_inc);
	
	buffer << "]";
	
}

void DisasmConstant(uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, SMType type, ostream & buffer, bool addr_inc)
{
	buffer << "c" << segment << "[";
	
	int shift = 0;
	if(type == SM_U16 || type == SM_S16) shift = 1;
	else if(type == SM_U32) shift = 2;

	DisasmAddress(dest, addr_lo, addr_hi, addr_imm, shift, buffer, addr_inc);
	
	buffer << "]";
}

void DisasmNot(uint32_t bnot, ostream & buffer)
{
	if(bnot) {
		buffer << "~";
	}
}

void DisasmSignWidth(uint32_t sign, uint32_t width, ostream & buffer)
{
	if(width) {
		buffer << (sign ? ".s32" : ".u32");
	}
	else {
		buffer << (sign ? ".s16" : ".u16");
	}
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

