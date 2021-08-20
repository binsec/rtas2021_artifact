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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_OPERATION_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_OPERATION_HH

//#include <unisim/component/cxx/processor/tesla/tesla_opcode.hh>
#include <unisim/component/cxx/processor/tesla/tesla_src1.hh>
#include <unisim/component/cxx/processor/tesla/tesla_src2.hh>
#include <unisim/component/cxx/processor/tesla/tesla_src3.hh>
#include <unisim/component/cxx/processor/tesla/tesla_dest.hh>
#include <unisim/component/cxx/processor/tesla/tesla_control.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

// Location of operands
enum Domain
{
	DomainNone,	// no operand
	DomainGPR,
	DomainAR,	// address register
	DomainPred,	// pred/flag register
	DomainConst,
	DomainShared,
	DomainGlobal,	// or local
};

enum Operand
{
	OpDest,
	OpSrc1,
	OpSrc2,
	OpSrc3
};

// Operation: base of all opcodes
// Only one instance of Operation per machine instruction in memory
// Unmutable
// Controls subfields decoding
template<class CONFIG>
struct Operation
{
	Operation(typename CONFIG::address_t addr, typename CONFIG::insn_t iw);
	~Operation();

	virtual void disasm(CPU<CONFIG> * cpu, Instruction<CONFIG> const * insn,
		ostream& buffer) = 0;
	virtual void classify(typename CONFIG::operationstats_t * stats) = 0;
	void initStats(typename CONFIG::operationstats_t * stats);
	
	bool OutputsPred();
	
	//typedef typename isa::opcode::Operation<CONFIG> OpCode;
	typedef isa::dest::Operation<CONFIG> OperDest;
	typedef isa::src1::Operation<CONFIG> OperSrc1;
	typedef isa::src2::Operation<CONFIG> OperSrc2;
	typedef isa::src3::Operation<CONFIG> OperSrc3;
	typedef isa::control::Operation<CONFIG> OperControl;

	OperSrc1 * src1;
	OperSrc2 * src2;
	OperSrc3 * src3;
	OperDest * dest;
	OperControl * control;
	
	DataType op_type[4];

private:
	typename CONFIG::address_t addr;
	typename CONFIG::insn_t iw;
	
	static isa::src1::Decoder<CONFIG> src1_decoder;
	static isa::src2::Decoder<CONFIG> src2_decoder;
	static isa::src3::Decoder<CONFIG> src3_decoder;
	static isa::dest::Decoder<CONFIG> dest_decoder;
	static isa::control::Decoder<CONFIG> control_decoder;
};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
