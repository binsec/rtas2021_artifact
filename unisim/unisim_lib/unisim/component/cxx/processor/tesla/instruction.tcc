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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INSTRUCTION_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INSTRUCTION_TCC

#include <unisim/component/cxx/processor/tesla/instruction.hh>
#include <unisim/component/cxx/processor/tesla/tesla_opcode.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_dest.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_src1.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_src2.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_src3.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_control.tcc>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


template <class CONFIG>
isa::opcode::Decoder<CONFIG> Instruction<CONFIG>::op_decoder;


template <class CONFIG>
Instruction<CONFIG>::Instruction(CPU<CONFIG> * cpu, typename CONFIG::address_t addr, typename CONFIG::insn_t iw) :
	cpu(cpu), addr(addr), iw(iw), stats(0)
{
	operation = op_decoder.Decode(addr, iw);
	if(operation->OutputsPred()) {
		flags.Reset();
	}
	if(cpu->stats != 0) {
		stats = &(*cpu->stats)[addr - CONFIG::CODE_START];
	}
}

template <class CONFIG>
Instruction<CONFIG>::~Instruction()
{
}

template <class CONFIG>
void Instruction<CONFIG>::Read()
{
	// mask = current warp mask & predicate mask

	mask = cpu->GetCurrentMask();
	mask &= operation->dest->predMask(cpu);

	if(operation->control->is_join)
	{
		if(cpu->Join()) {
			// Mask may be altered by Join
			mask = cpu->GetCurrentMask();
		}
		else {
			// Do not execute if Join returns "not synched"
			mask = 0;
		}
	}
	
	//if(mask != 0)
	{
		operation->read(cpu, this);
	}
}

template <class CONFIG>
void Instruction<CONFIG>::Execute()
{
	//if(mask != 0) {
	// Execute even when mask = 0 ! Esp. branches, sync...
	operation->execute(cpu, this);
	stats->Execute(Mask());
	//}
	if(operation->control->is_end) {
		cpu->End();
	}
}

template <class CONFIG>
void Instruction<CONFIG>::Write()
{
	operation->write(cpu, this);
	if(operation->OutputsPred()) {
		operation->writePred(cpu, this);
	}
}

template <class CONFIG>
void Instruction<CONFIG>::Disasm(std::ostream & os) const
{
	operation->dest->disasmPred(cpu, this, os);

	operation->disasm(cpu, this, os);
}


template <class CONFIG>
VectorRegister<CONFIG> const & Instruction<CONFIG>::Temp(unsigned int i) const
{
	assert(i < TempCount);
	return temp[i];
}

template <class CONFIG>
VectorRegister<CONFIG> & Instruction<CONFIG>::Temp(unsigned int i)
{
	assert(i < TempCount);
	return temp[i];
}

template <class CONFIG>
VectorRegister<CONFIG> & Instruction<CONFIG>::GetSrc1()
{
	return temp[TempSrc1];
}

template <class CONFIG>
VectorRegister<CONFIG> & Instruction<CONFIG>::GetSrc2()
{
	return temp[TempSrc2];
}

template <class CONFIG>
VectorRegister<CONFIG> & Instruction<CONFIG>::GetSrc3()
{
	return temp[TempSrc3];
}

template <class CONFIG>
VectorRegister<CONFIG> & Instruction<CONFIG>::GetDest()
{
	return temp[TempDest];
}

template <class CONFIG>
VectorFlags<CONFIG> & Instruction<CONFIG>::Flags()
{
	return flags;
}

template <class CONFIG>
void Instruction<CONFIG>::SetPredI32()
{
	VectorFlags<CONFIG> myflags = ComputePredI32(temp[TempDest], flags);
	operation->dest->writePred(cpu, myflags, Mask());
}

template <class CONFIG>
void Instruction<CONFIG>::SetPredI16()
{
	VectorFlags<CONFIG> myflags = ComputePredI16(temp[TempDest], flags);
	operation->dest->writePred(cpu, myflags, Mask());
}

template <class CONFIG>
void Instruction<CONFIG>::SetPredF32()
{
	VectorFlags<CONFIG> myflags = CONFIG::vfp32::ComputePredFP32(temp[TempDest], Mask());
	operation->dest->writePred(cpu, myflags, Mask());
}


// For load/store instructions, no subword access
template <class CONFIG>
void Instruction<CONFIG>::ReadBlock(int dest, DataType dt)
{
	switch(dt)
	{
	case DT_U8:
	case DT_S8:
	case DT_U16:
	case DT_S16:
	case DT_U32:
	case DT_S32:
	case DT_F32:
		Temp(0) = cpu->GetGPR(dest);
		break;
	case DT_U64:
		// Needs to be aligned
		assert(dest % 2 == 0);
		Temp(0) = cpu->GetGPR(dest);
		Temp(1) = cpu->GetGPR(dest + 1);
		break;
	case DT_U128:
		assert(dest % 4 == 0);
		Temp(0) = cpu->GetGPR(dest);
		Temp(1) = cpu->GetGPR(dest + 1);
		Temp(2) = cpu->GetGPR(dest + 2);
		Temp(3) = cpu->GetGPR(dest + 3);
		break;
	default:
		assert(false);
	}
	stats->RegRead(&Temp(0), dt);
}

template <class CONFIG>
void Instruction<CONFIG>::WriteBlock(int reg, DataType dt)
{
	bitset<CONFIG::WARP_SIZE> mask = Mask();
	switch(dt)
	{
	case DT_U8:
	case DT_S8:
	case DT_U16:
	case DT_S16:
	case DT_U32:
	case DT_S32:
	case DT_F32:
		// Overwrite whole register
		cpu->GetGPR(reg).Write(Temp(0), mask);
		if(cpu->TraceReg()) {
			cpu->DumpGPR(reg, cerr);
		}
		break;
	case DT_U64:
		// Needs to be aligned
		assert(reg % 2 == 0);
		cpu->GetGPR(reg).Write(Temp(0), mask);
		cpu->GetGPR(reg + 1).Write(Temp(1), mask);
		if(cpu->TraceReg()) {
			cpu->DumpGPR(reg, cerr);
			cpu->DumpGPR(reg + 1, cerr);
		}
		break;
	case DT_U128:
		assert(reg % 4 == 0);
		cpu->GetGPR(reg).Write(Temp(0), mask);
		cpu->GetGPR(reg + 1).Write(Temp(1), mask);
		cpu->GetGPR(reg + 2).Write(Temp(2), mask);
		cpu->GetGPR(reg + 3).Write(Temp(3), mask);
		if(cpu->TraceReg()) {
			cpu->DumpGPR(reg, cerr);
			cpu->DumpGPR(reg + 1, cerr);
			cpu->DumpGPR(reg + 2, cerr);
			cpu->DumpGPR(reg + 3, cerr);
		}
		break;
	default:
		assert(false);
	}
	stats->RegWrite(&Temp(0), dt, mask);
}


// For GP instructions, loads from 16 to 64-bit regs
template <class CONFIG>
void Instruction<CONFIG>::ReadReg(int reg, int tempbase, RegType rt)
{
	switch(rt)
	{
	case RT_U16:
		{
		uint32_t rnum = (reg >> 1);
		uint32_t hilo = (reg & 1);
		Temp(tempbase) = cpu->GetGPR(rnum).Split(hilo);
		}
		break;
	case RT_U32:
		Temp(tempbase) = cpu->GetGPR(reg);
		break;
	case RT_U64:
		// Needs to be aligned
		assert(reg % 2 == 0);
		Temp(tempbase) = cpu->GetGPR(reg);
		Temp(tempbase + 1) = cpu->GetGPR(reg + 1);
		break;
	default:
		assert(false);
	}
	stats->RegRead(&Temp(tempbase), RegTypeToDataType(rt));
}

template <class CONFIG>
void Instruction<CONFIG>::WriteReg(int reg, int tempbase, DataType dt,
	bitset<CONFIG::WARP_SIZE> mask)
{
	switch(dt)
	{
	case DT_U16:
	case DT_S16:
		{
		uint32_t rnum = (reg >> 1);
		uint32_t hilo = (reg & 1);
		cpu->GetGPR(rnum).Write16(Temp(tempbase), mask, hilo);
		if(cpu->TraceReg()) {
			cpu->DumpGPR(rnum, cerr);
		}
		}
		break;
	case DT_U32:
	case DT_S32:
	case DT_F32:
		cpu->GetGPR(reg).Write(Temp(tempbase), mask);
		if(cpu->TraceReg()) {
			cpu->DumpGPR(reg, cerr);
		}
		break;
	case DT_U64:
		assert(reg % 2 == 0);
		cpu->GetGPR(reg).Write(Temp(tempbase), mask);
		cpu->GetGPR(reg + 1).Write(Temp(tempbase), mask);
		if(cpu->TraceReg()) {
			cpu->DumpGPR(reg, cerr);
			cpu->DumpGPR(reg + 1, cerr);
		}
		break;
	default:
		assert(false);
	}
	stats->RegWrite(&Temp(tempbase), dt, mask);
}

template <class CONFIG>
void Instruction<CONFIG>::ReadSrc1()
{
	operation->src1->read(cpu, this);
}

template <class CONFIG>
void Instruction<CONFIG>::ReadSrc2()
{
	operation->src2->read(cpu, this);
}

template <class CONFIG>
void Instruction<CONFIG>::ReadSrc3()
{
	operation->src3->read(cpu, this);
}

template <class CONFIG>
void Instruction<CONFIG>::WriteDest()
{
	operation->dest->write(cpu, this, Mask());
}

template <class CONFIG>
void Instruction<CONFIG>::WriteFlags()
{
	operation->dest->writePred(cpu, flags, Mask());
}

template <class CONFIG>
void Instruction<CONFIG>::SetDest(VectorRegister<CONFIG> const & value)
{
	Temp(TempDest) = value;
}

template <class CONFIG>
void Instruction<CONFIG>::SetPredFP32(VectorRegister<CONFIG> const & value) const
{
	VectorFlags<CONFIG> flags = CONFIG::vfp::ComputePredFP32(value, Mask());
	operation->dest->writePred(cpu, flags, Mask());
}

template <class CONFIG>
void Instruction<CONFIG>::SetPredI32(VectorRegister<CONFIG> const & value, VectorFlags<CONFIG> flags) const
{
	VectorFlags<CONFIG> myflags = ComputePredI32(value, flags);
	operation->dest->writePred(cpu, myflags, Mask());
}

template <class CONFIG>
void Instruction<CONFIG>::SetPredI32(VectorRegister<CONFIG> const & value) const
{
	SetPredI32(value, flags);
}

template <class CONFIG>
void Instruction<CONFIG>::SetPred(VectorFlags<CONFIG> flags) const
{
	operation->dest->writePred(cpu, flags, Mask());
}

template <class CONFIG>
bitset<CONFIG::WARP_SIZE> Instruction<CONFIG>::Mask() const
{
	return mask;
}

template <class CONFIG>
void Instruction<CONFIG>::DisasmSrc1(std::ostream & os) const
{
	operation->src1->disasm(cpu, this, os);
}

template <class CONFIG>
void Instruction<CONFIG>::DisasmSrc2(std::ostream & os) const
{
	operation->src2->disasm(cpu, this, os);
}

template <class CONFIG>
void Instruction<CONFIG>::DisasmSrc3(std::ostream & os) const
{
	operation->src3->disasm(cpu, this, os);
}


template <class CONFIG>
void Instruction<CONFIG>::DisasmDest(std::ostream & os) const
{
	operation->dest->disasm(cpu, this, os);
}

template <class CONFIG>
void Instruction<CONFIG>::DisasmControl(std::ostream & os) const
{
	operation->control->disasm(os);
}

template <class CONFIG>
bool Instruction<CONFIG>::IsLong() const
{
	return operation->control->is_long;
}

template <class CONFIG>
bool Instruction<CONFIG>::IsEnd() const
{
	return operation->control->is_end;
}

template <class CONFIG>
RegType Instruction<CONFIG>::OperandRegType(Operand op) const
{
	DataType dt = OperandDataType(op);
	switch(dt)
	{
	case DT_U8:
	case DT_S8:
	case DT_U16:
	case DT_S16:
		return RT_U16;
	case DT_U32:
	case DT_S32:
	case DT_F32:
		return RT_U32;
	case DT_U64:
	case DT_F64:
		return RT_U64;
	default:
		assert(false);
	}
}

template <class CONFIG>
SMType Instruction<CONFIG>::OperandSMType(Operand op) const
{
	DataType dt = OperandDataType(op);
	switch(dt)
	{
	case DT_U8:
	case DT_S8:
		return SM_U8;
	case DT_U16:
		return SM_U16;
	case DT_S16:
		return SM_S16;
	case DT_U32:
	case DT_S32:
	case DT_F32:
		return SM_U32;
	default:
		assert(false);
	}
}

template <class CONFIG>
size_t Instruction<CONFIG>::OperandSize(Operand op) const
{
	DataType dt = OperandDataType(op);
	return DataTypeSize(dt);
}

template <class CONFIG>
DataType Instruction<CONFIG>::OperandDataType(Operand op) const
{
	// NOT actual type, can be overriden by operand
	// TODO: fix with override
	return operation->op_type[op];
}

template <class CONFIG>
Domain Instruction<CONFIG>::OperandDomain(Operand op) const
{
	assert(false);
	throw "";
}

template <class CONFIG>
size_t Instruction<CONFIG>::NumGPRsIn() const
{
	assert(false);
	throw "";
}

template <class CONFIG>
VectorAddress<CONFIG> Instruction<CONFIG>::EffectiveAddress(Operand op) const
{
	assert(false);
	throw "";
}

template <class CONFIG>
typename CONFIG::operationstats_t * Instruction<CONFIG>::Stats()
{
	return stats;
}

template <class CONFIG>
void Instruction<CONFIG>::InitStats()
{
	ostringstream sstr;
	Disasm(sstr);
	stats->SetName(sstr.str().c_str());
	GetOperation()->initStats(stats);
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
