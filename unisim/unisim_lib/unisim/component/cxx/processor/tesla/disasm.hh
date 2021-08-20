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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_DISASM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_DISASM_HH__

#include <string>
#include <iosfwd>

#include <unisim/component/cxx/processor/tesla/register.hh>
#include <unisim/component/cxx/processor/tesla/flags.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using std::ostream;

//=====================================================================
//=                Disassembly helper functions                       =
//=====================================================================
void DisasmReg(uint32_t reg, RegType rt, ostream & os);
void DisasmPred(uint32_t pred_cond, uint32_t pred_reg, ostream & buffer);
void DisasmRounding(uint32_t rounding_mode, ostream & buffer);
void DisasmMarker(uint32_t marker, ostream & buffer);
//void DisasmSetPred(uint32_t set_pred_reg, uint32_t set_pred, ostream & buffer);
//void DisasmDest(uint32_t dest, uint32_t set_output, ostream & buffer);
void DisasmSrc(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg, ostream & buffer);
//void DisasmSrc(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg, uint32_t addr_lo,
//	uint32_t addr_hi, uint32_t addr_imm, ostream & buffer);
void DisasmImm(uint32_t imm_hi, uint32_t imm_lo, ostream & buffer);
void DisasmConvert(uint32_t cvt_round, uint32_t cvt_type, bool sign, uint32_t data_32, uint32_t abssat, ostream & buffer);
void DisasmConvertSharedConst(RoundingMode cvt_round, SMType srctype, bool sign,
	bool dest32, AbsSat abssat, ostream & buffer);

void DisasmConvertFP32(bool dest_32, ConvType srct, RoundingMode cvt_round,
	bool cvt_int, AbsSat as, ostream & buffer);
void DisasmDataType(uint32_t dt, ostream & buffer);
std::string DataTypeString(DataType d);
std::string RoundingModeString(RoundingMode r);
std::string ConvTypeString(ConvType t);
std::string SMTypeString(SMType t);
std::string AbsSatString(AbsSat as);

void DisasmAddress(uint32_t reg, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t shift, ostream & buffer, bool addr_inc = false);
void DisasmSign(uint32_t sign, ostream & buffer);
void DisasmGlobal(uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, uint32_t dt, ostream & buffer);
void DisasmLocal(uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, uint32_t dt, ostream & buffer, bool addr_inc = false);
void DisasmShared(uint32_t addr, uint32_t type, ostream & buffer);
void DisasmShared(uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, SMType type, ostream & buffer, bool addr_inc = false);
void DisasmConstant(uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, SMType type, ostream & buffer, bool addr_inc = false);
void DisasmNot(uint32_t bnot, ostream & buffer);
void DisasmSignWidth(uint32_t sign, uint32_t width, ostream & buffer);

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
