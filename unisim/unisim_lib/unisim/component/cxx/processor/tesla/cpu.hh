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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_CPU_HH

#include <stdlib.h>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/resetable.hh>
#include <unisim/service/interfaces/runnable.hh>
#include "unisim/kernel/logger/logger.hh"
#include <unisim/service/interfaces/typed_registers.hh>
#include <unisim/service/interfaces/instruction_stats.hh>
#include <unisim/util/queue/queue.hh>
#include <map>
#include <iostream>
#include <stdio.h>
#include <boost/integer.hpp>
#include <boost/shared_ptr.hpp>
#include <stack>
#include <bitset>
#include <unisim/component/cxx/processor/tesla/interfaces.hh>
#include <unisim/component/cxx/processor/tesla/register.hh>
#include <unisim/component/cxx/processor/tesla/warp.hh>
#include <unisim/component/cxx/processor/tesla/flags.hh>
#include <unisim/component/cxx/processor/tesla/maskstack.hh>
#include <unisim/component/cxx/processor/tesla/implicit_flow.hh>
#include <unisim/component/cxx/processor/tesla/tesla_flow.hh>
#include <unisim/component/cxx/processor/tesla/hostfloat/hostfloat.hh>
#include <unisim/component/cxx/processor/tesla/sampler.hh>
#include <unisim/component/cxx/processor/tesla/register_interface.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using unisim::service::interfaces::DebugYielding;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::TypedRegisters;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::InstructionStats;
using unisim::service::interfaces::Resetable;
using unisim::service::interfaces::Runnable;
using namespace unisim::util::endian;
using unisim::kernel::Client;
using unisim::kernel::Service;
using unisim::kernel::ServiceImport;
using unisim::kernel::ServiceExport;
using unisim::kernel::Object;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Register;
using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::ParameterArray;
using namespace std;
using unisim::util::queue::Queue;
using namespace boost;

using unisim::kernel::logger::Logger;

template <class CONFIG>
class CPU :
	public Service<Disassembly<typename CONFIG::address_t> >,
	public Service<Memory<typename CONFIG::address_t> >,
	public Service<MemoryInjection<typename CONFIG::address_t> >,
	public Service<Registers>,
	public Service<TypedRegisters<uint32_t, GPRID> >,
	public Service<TypedRegisters<uint32_t, ConfigurationRegisterID> >,
	public Service<TypedRegisters<SamplerBase<typename CONFIG::address_t>, SamplerIndex> >,
	public Service<Memory<SMAddress> >,
	public Service<InstructionStats<typename CONFIG::stats_t> >,
	public Service<Resetable>,
	public Service<Runnable>,
	public Client<Loader<typename CONFIG::physical_address_t> >,
	public Client<DebugYielding>,
	public Client<SymbolTableLookup<typename CONFIG::address_t> >,
	public Client<Memory<typename CONFIG::address_t> >
//	public Service<Synchronizable>
{
public:
	typedef typename CONFIG::address_t address_t;
	typedef typename CONFIG::virtual_address_t virtual_address_t;
	typedef typename CONFIG::physical_address_t physical_address_t;
	typedef typename CONFIG::insn_t insn_t;
	typedef typename CONFIG::float_t float_t;
	typedef VectorRegister<CONFIG> VecReg;
	typedef VectorFlags<CONFIG> VecFlags;
	typedef VectorAddress<CONFIG> VecAddr;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	
	static uint32_t const WARP_SIZE = CONFIG::WARP_SIZE;
	static uint32_t const MAX_WARPS_PER_BLOCK = CONFIG::MAX_WARPS_PER_BLOCK;
	static uint32_t const MAX_WARPS = CONFIG::MAX_WARPS;
	static uint32_t const MAX_VGPR = CONFIG::MAX_VGPR;
	static uint32_t const MAX_CTAS = CONFIG::MAX_CTAS;
	static uint32_t const MAX_THREADS = MAX_WARPS * WARP_SIZE;
	static uint32_t const SHARED_MEM_SIZE = CONFIG::SHARED_SIZE;
	static uint32_t const MAX_ADDR_REGS = CONFIG::MAX_ADDR_REGS;
	static uint32_t const MAX_PRED_REGS = CONFIG::MAX_PRED_REGS;


	//=====================================================================
	//=                  public service imports/exports                   =
	//=====================================================================
	
	ServiceExport<Disassembly<address_t> > disasm_export;

	ServiceExport<Memory<address_t> > memory_export;
	ServiceExport<MemoryInjection<address_t> > memory_injection_export;
	ServiceExport<Registers> debug_registers_export;
//	ServiceExport<Synchronizable> synchronizable_export;
	ServiceExport<TypedRegisters<uint32_t, GPRID> > registers_export;
	ServiceExport<TypedRegisters<uint32_t, ConfigurationRegisterID> > configuration_export;
	ServiceExport<TypedRegisters<SamplerBase<typename CONFIG::address_t>, SamplerIndex> > samplers_export;
	ServiceExport<Memory<SMAddress> > shared_memory_export;
	ServiceExport<InstructionStats<typename CONFIG::stats_t> > instruction_stats_export;
	ServiceExport<Resetable> reset_export;
	ServiceExport<Runnable> run_export;
	ServiceImport<Loader<physical_address_t> > kernel_loader_import;
	ServiceImport<DebugYielding> debug_yielding_import;
	ServiceImport<SymbolTableLookup<address_t> > symbol_table_lookup_import;
	ServiceImport<Memory<physical_address_t> > memory_import;

	//=====================================================================
	//=                    Constructor/Destructor                         =
	//=====================================================================

	// TODO: coreid and core_count as Parameters
	CPU(const char *name, Object *parent = 0, int coreid = 0, int core_count = 1);
	virtual ~CPU();
	
	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================
	
	virtual bool Setup();
	virtual void OnDisconnect();
	
	// Service implementation
	
	bool Step();	// -> true when finished
	virtual void Run();
	virtual void Stop(int ret);
	virtual void Synchronize();
	virtual void Reset();
	void Reset(unsigned int threadsperblock, unsigned int numblocks, unsigned int regnum, unsigned int smsize);

	virtual bool ReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(address_t addr, const void *buffer, uint32_t size);
	virtual bool InjectReadMemory(address_t addr, void *buffer, uint32_t size);
	virtual bool InjectWriteMemory(address_t addr, const void *buffer, uint32_t size);

	virtual Register *GetRegister(const char *name);

	virtual uint32_t ReadTypedRegister(GPRID addr);
	virtual void WriteTypedRegister(GPRID addr, uint32_t const & r);
	virtual uint32_t ReadTypedRegister(ConfigurationRegisterID addr);
	virtual void WriteTypedRegister(ConfigurationRegisterID addr, uint32_t const & r);
	virtual SamplerBase<address_t> ReadTypedRegister(SamplerIndex addr);
	virtual void WriteTypedRegister(SamplerIndex addr, SamplerBase<address_t> const & r);

	
	virtual bool ReadMemory(SMAddress addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(SMAddress addr, const void *buffer, uint32_t size);

	virtual typename CONFIG::stats_t * GetStats();
	virtual void SetStats(typename CONFIG::stats_t * stats);
	virtual void InitStats(unsigned int code_size);

	//Register * ParseRegisterName(istringstream & name, unsigned int warp);
	
	void ReadShared(address_t addr, void *buffer, uint32_t size);
	void WriteShared(address_t addr, const void *buffer, uint32_t size);
	void SetSampler(Sampler<CONFIG> const & sampler, unsigned int n);
	

	//=====================================================================
	//=                         utility methods                           =
	//=====================================================================
	
	static uint32_t Mask(uint32_t mb, uint32_t me)
	{
		return (mb > me) ? ~(((uint32_t) -1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1))) : (((uint32_t)-1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1)));
	}
	
	static uint32_t CRFieldMask(uint32_t crfD)
	{
		return (crfD != 7 ? 0xFFFFFFFFul >> ((crfD + 1) * 4) : 0) | (crfD != 0 ? (0xFFFFFFFFul << (8 - crfD) * 4) : 0);
	}

	//=====================================================================
	//=                    user level registers set/get methods           =
	//=====================================================================
	
	
	//=====================================================================
	//=                  instruction address set/get methods              =
	//=====================================================================
	
	inline address_t GetPC() const { return CurrentWarp().pc; }
	inline void SetPC(address_t value) { CurrentWarp().pc = value; }
	inline address_t GetNPC() const { return CurrentWarp().npc; }
	inline address_t & GetNPC() { return CurrentWarp().npc; }
	inline void SetNPC(address_t value) { CurrentWarp().npc = value; }
	
	
	//=====================================================================
	//=                        Debugging stuffs                           =
	//=====================================================================

// 	address_t GetEA() const { return effective_address; }
	
//	Parameter<uint64_t> param_trap_on_instruction_counter;
	virtual string Disasm(address_t addr, address_t& next_addr);
	virtual const char *GetArchitectureName() const;
	inline uint64_t GetInstructionCounter() const { return instruction_counter; }
	void ProcessDebugCommands();
	//inline void MonitorLoad(address_t ea, uint32_t size);
	//inline void MonitorStore(address_t ea, uint32_t size);
	
	void DumpRegisters(int warpid, ostream & os) const;
	void DumpGPR(int warpid, int reg, ostream & os) const;
	void DumpGPR(int reg, ostream & os) const;

	void DumpFlags(int warpid, int reg, ostream & os) const;
	void DumpFlags(int reg, ostream & os) const;

	void DumpAddr(int reg, ostream & os) const;

	void OnBusCycle();

	
	// Control flow
	void Meet(address_t addr);
	void PreBreak(address_t addr);
	bool Join();
	void End();
	void Fence();
	void Kill(std::bitset<CONFIG::WARP_SIZE> mask);
	void Return(std::bitset<CONFIG::WARP_SIZE> mask);
	void Break(std::bitset<CONFIG::WARP_SIZE> mask);
	void Branch(address_t target, std::bitset<CONFIG::WARP_SIZE> mask);
	void Call(address_t target);
	void StepWarp(uint32_t warpid);
	void CheckJoin();
	void CheckFenceCompleted();

	// Register access
	VecReg & GetGPR(unsigned int reg);	// for current warp
	VecReg GetGPR(unsigned int reg) const;	// for current warp
	
	VecReg & GetGPR(unsigned int wid, unsigned int reg);
	VecReg GetGPR(unsigned int wid, unsigned int reg) const;
	
	VecFlags & GetFlags(unsigned int reg);	// for current warp
	VecFlags GetFlags(unsigned int reg) const;
	
	VecAddr & GetAddr(unsigned int reg);
	VecAddr GetAddr(unsigned int reg) const;
	
//	std::bitset<CONFIG::WARP_SIZE> & GetCurrentMask();
	std::bitset<CONFIG::WARP_SIZE> GetCurrentMask() const;

	// Debug register access
	uint32_t & GetScalar(unsigned int reg);
	uint32_t GetScalar(unsigned int reg) const;
	
	
	// Memory access helpers
	
	// Only for <= 32-bit accesses
	VecAddr LocalAddress(VecAddr const & addr, unsigned int segment);
	address_t LocalAddress(address_t addr, unsigned int segment);
	
	void ScatterGlobal(VecReg const output[], uint32_t dest, uint32_t addr_lo,
		uint32_t addr_hi, bool addr_imm, uint32_t segment,
		std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void GatherGlobal(VecReg output[], uint32_t src, uint32_t addr_lo,
		uint32_t addr_hi, bool addr_imm, uint32_t segment,
		std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void ScatterLocal(VecReg const output[], uint32_t dest, uint32_t addr_lo,
		uint32_t addr_hi, bool addr_imm, uint32_t segment,
		std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void GatherLocal(VecReg output[], uint32_t src, uint32_t addr_lo,
		uint32_t addr_hi, bool addr_imm, uint32_t segment,
		std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void ScatterShared(VecReg const & output, uint32_t dest, uint32_t addr_lo,
		uint32_t addr_hi, bool addr_imm,
		std::bitset<CONFIG::WARP_SIZE> mask, SMType type);

	void GatherShared(VecAddr const & addr, VecReg & data,
		std::bitset<CONFIG::WARP_SIZE> mask, SMType t = SM_U32);	// addr in bytes
	void GatherShared(VecReg & output, uint32_t src, uint32_t addr_lo,
		uint32_t addr_hi, bool addr_imm, std::bitset<CONFIG::WARP_SIZE> mask, SMType type);
	void GatherConstant(VecReg & output, uint32_t src, uint32_t addr_lo,
		uint32_t addr_hi, bool addr_imm, uint32_t segment,
		std::bitset<CONFIG::WARP_SIZE> mask, SMType type);

	void Gather(VecAddr const & addr, VecReg data[],
		std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);
	void Scatter(VecAddr const & addr, VecReg const data[],
		std::bitset<CONFIG::WARP_SIZE> mask, DataType dt);

	// High-level memory access
	void ReadShared(address_t addr, VecReg & data, SMType t = SM_U32);		// addr in WORDS!!
	VecReg ReadConstant(VecReg const & addr, uint32_t seg = 0);	// addr in bytes
	VecReg ReadConstant(unsigned int addr, uint32_t seg = 0);

	void LoadLocal32(VecReg & output, address_t addr);
	void StoreLocal32(VecReg const & output, address_t addr);


	// Low-level memory access
	void Gather32(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Scatter32(VecAddr const & addr, VecReg const & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Gather16(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Gather8(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Scatter16(VecAddr const & addr, VecReg const & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Scatter8(VecAddr const & addr, VecReg const & data, std::bitset<CONFIG::WARP_SIZE> mask, uint32_t factor = 1, address_t offset = 0);
	void Broadcast32(address_t addr, VecReg & data, uint32_t factor = 1, address_t offset = 0);
	void Broadcast16(address_t addr, VecReg & data, uint32_t factor = 1, address_t offset = 0);
	void Broadcast8(address_t addr, VecReg & data, uint32_t factor = 1, address_t offset = 0);
	
	void Read32(address_t addr, uint32_t & data, uint32_t factor = 1, address_t offset = 0);
	void Write32(address_t addr, uint32_t data, uint32_t factor = 1, address_t offset = 0);
	void Read16(address_t addr, uint32_t & data, uint32_t factor = 1, address_t offset = 0);
	void Write16(address_t addr, uint32_t data, uint32_t factor = 1, address_t offset = 0);
	void Read8(address_t addr, uint32_t & data, uint32_t factor = 1, address_t offset = 0);
	void Write8(address_t addr, uint32_t data, uint32_t factor = 1, address_t offset = 0);

	VecAddr EffectiveAddress(uint32_t reg, uint32_t addr_lo, uint32_t addr_hi,
		bool addr_imm, uint32_t shift);
	
	void Sample1DS32(unsigned int sampler,
		VectorRegister<CONFIG> dest[],
		VectorRegister<CONFIG> const src[],
		uint32_t destBitfield);
	
	Warp<CONFIG> & CurrentWarp();
	Warp<CONFIG> const & CurrentWarp() const;
	
	Warp<CONFIG> & GetWarp(unsigned int wid);
	Warp<CONFIG> const & GetWarp(unsigned int wid) const;

	Sampler<CONFIG> & GetSampler(unsigned int s);
	
	typename CONFIG::operationstats_t & GetOpStats();

	void PopulateRegisterRegistry();

private:
	//=====================================================================
	//=                           G80 registers                           =
	//=====================================================================

	// Configuration registers
	// Only used for Reset?
	uint32_t threadsperblock;
	uint32_t numblocks;
	uint32_t gprs_per_warp;
	uint32_t sm_size;	// Used for bound checks
	
	// Runtime registers
	unsigned int coreid;
	unsigned int core_count;

	Warp<CONFIG> warps[MAX_WARPS];
	CTA<CONFIG> ctas[MAX_CTAS];
	uint32_t current_warpid;
	
	VecReg gpr[MAX_VGPR];
	Sampler<CONFIG> samplers[CONFIG::MAX_SAMPLERS];
	
	VecReg zero_reg;
	
	uint32_t num_warps;
	uint32_t num_ctas;

	// GT200
//	bool mutex[SHARED_MEM_SIZE];
	
	//=====================================================================
	//=                      Debugging stuffs                             =
	//=====================================================================
	uint32_t current_laneid;
	
	uint64_t instruction_counter;                              //!< Number of executed instructions
	uint64_t max_inst;                                         //!< Maximum number of instructions to execute

	//=====================================================================
	//=                    CPU run-time parameters                        =
	//=====================================================================
	
	Parameter<uint64_t> param_max_inst;                   //!< linked to member max_inst

	Parameter<bool> param_trace_insn;
	Parameter<bool> param_trace_mask;
	Parameter<bool> param_trace_reg;
	Parameter<bool> param_trace_reg_float;
	Parameter<bool> param_trace_loadstore;
	Parameter<bool> param_trace_branch;
	Parameter<bool> param_trace_sync;
	Parameter<bool> param_trace_reset;
	Parameter<bool> param_export_stats;
	
	Parameter<bool> param_filter_trace;
	Parameter<uint32_t> param_filter_warp;
	Parameter<uint32_t> param_filter_cta;

	//=====================================================================
	//=                    CPU run-time statistics                        =
	//=====================================================================

	Statistic<uint64_t> stat_instruction_counter;
	Statistic<uint64_t> stat_cpu_cycle;                   //!< Number of cpu cycles
	Statistic<uint64_t> stat_bus_cycle;                   //!< Number of front side bus cycles
protected:
	//=====================================================================
	//=              CPU Cycle Time/Voltage/Bus Cycle Time                =
	//=====================================================================
	
	uint64_t bus_cycle;      //!< Number of front side bus cycles
	uint64_t cpu_cycle;      //!< Number of cpu cycles

	void Fetch(typename CONFIG::insn_t & insn, typename CONFIG::address_t addr);

	typedef map<string, shared_ptr<Register> > registers_registry_t;
	registers_registry_t registers_registry;

public:
	bool trace_insn;
	bool trace_mask;
	bool trace_reg;
	bool trace_reg_float;
	bool trace_loadstore;
	bool trace_branch;
	bool trace_sync;
	bool trace_reset;
	
	bool filter_trace;
	uint32_t filter_warp;
	uint32_t filter_cta;
	
	bool TraceEnabled() {
		return !filter_trace
			|| (current_warpid == filter_warp && CurrentWarp().CTAID() == filter_cta);
	}
	bool TraceInsn() { return trace_insn && TraceEnabled(); }
	bool TraceMask() { return trace_mask && TraceEnabled(); }
	bool TraceReg() { return trace_reg && TraceEnabled(); }
	bool TraceLoadstore() { return trace_loadstore && TraceEnabled(); }
	bool TraceBranch() { return trace_branch && TraceEnabled(); }
	bool TraceSync() { return trace_sync && TraceEnabled(); }
	bool TraceReset() { return trace_reset && TraceEnabled(); }

	Logger trace_logger;

	typename CONFIG::stats_t* stats;
	bool export_stats;
};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
