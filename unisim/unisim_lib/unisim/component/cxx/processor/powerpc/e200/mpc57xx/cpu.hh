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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/cpu.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/queue/queue.hh>
#include <unisim/util/queue/queue.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

template <typename TYPES, typename CONFIG>
class CPU
	: public unisim::component::cxx::processor::powerpc::CPU<TYPES, CONFIG>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Disassembly<typename TYPES::EFFECTIVE_ADDRESS> >
{
public:
	typedef typename unisim::component::cxx::processor::powerpc::CPU<TYPES, CONFIG> SuperCPU;
	typedef typename SuperCPU::SuperMSS SuperMSS;
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	
	typedef typename SuperCPU::MCSR MCSR;
	typedef typename SuperCPU::ESR ESR;
	typedef typename SuperCPU::DBSR DBSR;
	typedef typename SuperCPU::SPEFSCR SPEFSCR;
	
	/////////////////////////// service exports ///////////////////////////////

	unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<EFFECTIVE_ADDRESS> > disasm_export;
	unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<PHYSICAL_ADDRESS> > local_memory_export;

	////////////////////////////// constructor ////////////////////////////////
	
	CPU(const char *name, unisim::kernel::Object *parent = 0);

	/////////////////////////////// destructor ////////////////////////////////

	virtual ~CPU();
	
	////////////////////////////// setup hooks ////////////////////////////////

	virtual bool EndSetup();
	
	void Reset();

	//////////  unisim::service::interfaces::Disassembly<> ////////////////////
	
	virtual std::string Disasm(EFFECTIVE_ADDRESS addr, EFFECTIVE_ADDRESS& next_addr);
	
	///////////////// Interface with SystemC TLM-2.0 wrapper module ///////////
	
	virtual void InterruptAcknowledge() {}
	virtual void Idle() {}
	virtual void Halt() {}
	
	////////////////////////// Machine State Register /////////////////////////
	
	struct MSR : SuperCPU::MSR
	{
		typedef typename SuperCPU::MSR Super;
		
		MSR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		MSR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		inline void Set(unsigned int bit_offset, typename MSR::TYPE bit_value) ALWAYS_INLINE
		{
			Super::Set(bit_offset, bit_value);
			this->cpu->UpdateExceptionEnable();
		}
		
		template <typename FIELD> inline void Set(typename MSR::TYPE field_value)
		{
			Super::template Set<FIELD>(field_value);
			this->cpu->UpdateExceptionEnable();
		}

		MSR& operator = (const typename MSR::TYPE& value)
		{
			this->Super::operator = (value);
			this->cpu->UpdateExceptionEnable();
			return *this;
		}
	};

	///////////// Interface with .isa behavioral description files ////////////
	
	MSR& GetMSR() { return msr; }
	ESR& GetESR() { return esr; }
	SPEFSCR& GetSPEFSCR() { return spefscr; }
	typename SuperCPU::SRR0& GetSRR0() { return srr0; }
	typename SuperCPU::SRR1& GetSRR1() { return srr1; }
	typename SuperCPU::CSRR0& GetCSRR0() { return csrr0; }
	typename SuperCPU::CSRR1& GetCSRR1() { return csrr1; }
	typename SuperCPU::MCSRR0& GetMCSRR0() { return mcsrr0; }
	typename SuperCPU::MCSRR1& GetMCSRR1() { return mcsrr1; }
	typename SuperCPU::DSRR0& GetDSRR0() { return dsrr0; }
	typename SuperCPU::DSRR1& GetDSRR1() { return dsrr1; }

	bool Lbarx(unsigned int rd, EFFECTIVE_ADDRESS addr);
	bool Lharx(unsigned int rd, EFFECTIVE_ADDRESS addr);
	bool Lwarx(unsigned int rd, EFFECTIVE_ADDRESS addr);
	bool Mbar(EFFECTIVE_ADDRESS addr);
	bool Stbcx(unsigned int rs, EFFECTIVE_ADDRESS addr);
	bool Sthcx(unsigned int rs, EFFECTIVE_ADDRESS addr);
	bool Stwcx(unsigned int rs, EFFECTIVE_ADDRESS addr);
	bool Wait();
	bool Msync();
	bool Isync();
	bool Rfi();
	bool Rfci();
	bool Rfdi();
	bool Rfmci();
	
	bool CheckSPV() { return true; } // This architecture totally ignores SPV
	
	////////////////////////// Special Purpose Registers //////////////////////
	
	// Processor ID Register
	struct PIR : SuperCPU::PIR
	{
		typedef typename SuperCPU::PIR Super;
		
		PIR(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		PIR(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual void Reset()
		{
			this->template Set<typename PIR::ID_0_23>(0);
			this->template Set<typename PIR::ID_24_31>(this->cpu->cpuid);
		}
	};

	// Hardware Implementation Dependent Register 0
	struct HID0 : SuperCPU::HID0
	{
		typedef typename SuperCPU::HID0 Super;
		
		HID0(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		HID0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		void Effect()
		{
			this->cpu->UpdateExceptionEnable();
		}
		
		HID0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	};
	
	//////////////////////////////// Interrupts ///////////////////////////////
	
	struct SystemResetInterrupt : SuperCPU::template Interrupt<SystemResetInterrupt, 0x00 /* p_rstbase[0:29] */>
	{
		typedef typename SuperCPU::template Interrupt<SystemResetInterrupt, 0x00> Super;
		
		struct Reset                             : SuperCPU::template Exception<SystemResetInterrupt> { static const char *GetName() { return "System Reset Interrupt/Reset Exception"; } };                  // reset
		
		typedef typename SuperCPU::template ExceptionSet<Reset> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::CE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::ME
		                , typename MSR::FE0
		                , typename MSR::DE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM
		                , typename MSR::RI > MSR_CLEARED_FIELDS;
		
		SystemResetInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "system-reset-interrupt"; }
		static const char *GetName() { return "System Reset Interrupt"; }
	};

	struct CriticalInputInterrupt : SuperCPU::template Interrupt<CriticalInputInterrupt, 0x00>
	{
		typedef typename SuperCPU::template Interrupt<CriticalInputInterrupt, 0x00> Super;
		
		struct CriticalInput                        : SuperCPU::template Exception<CriticalInputInterrupt> { static const char *GetName() { return "Critical Input Interrupt/Critical Input Exception"; } };             //  p_critint_b is asserted and MSR[CE] = 1
		
		typedef typename SuperCPU::template ExceptionSet<CriticalInput> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::CE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_ALWAYS_CLEARED_FIELDS;
		
		CriticalInputInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "critical-input-interrupt"; }
		static const char *GetName() { return "Critical Input Interrupt"; }
	};

	struct MachineCheckInterrupt : SuperCPU::template InterruptWithAddress<MachineCheckInterrupt, 0x10>
	{
		typedef typename SuperCPU::template InterruptWithAddress<MachineCheckInterrupt, 0x10> Super;
		
		enum MachineCheckEventType
		{
			MCE_MCP                                       = 0x00000001, // p_mcp_b transitions from negated to asserted
			MCE_EXCEPTION_ISI                             = 0x00000002, // ISI on first instruction fetch for an exception handler    
			MCE_EXCEPTION_BUS_ERROR                       = 0x00000004, // Bus Error on first instruction fetch for an exception handler
			MCE_INSTRUCTION_CACHE_DATA_ARRAY_PARITY_ERROR = 0x00000008, // Instruction cache data array parity error
			MCE_DATA_CACHE_DATA_ARRAY_PARITY_ERROR        = 0x00000010, // Data cache data array parity error
			MCE_INSTRUCTION_CACHE_TAG_PARITY_ERROR        = 0x00000020, // Instruction cache tag parity error
			MCE_DATA_CACHE_TAG_PARITY_ERROR               = 0x00000040, // Data cache tag parity error
			MCE_INSTRUCTION_CACHE_LOCK_ERROR              = 0x00000080, // Instruction cache lock error
			MCE_DATA_CACHE_LOCK_ERROR                     = 0x00000100, // Data cache lock error
			MCE_INSTRUCTION_FETCH_ERROR_REPORT            = 0x00000200, // Instruction Fetch Error Report
			MCE_LOAD_TYPE_INSTRUCTION_ERROR_REPORT        = 0x00000400, // Load type instruction Error Report
			MCE_STORE_TYPE_INSTRUCTION_ERROR_REPORT       = 0x00000800, // Store type instruction Error Report
			MCE_GUARDED_INSTRUCTION_ERROR_REPORT          = 0x00001000, // Guarded instruction Error Report
			MCE_STACK_LIMIT_CHECK_FAILURE                 = 0x00002000, // Stack limit check failure
			MCE_INSTRUCTION_MEMORY_PARITY_ERROR           = 0x00004000, // Instruction Memory Parity Error
			MCE_DATA_MEMORY_READ_PARITY_ERROR             = 0x00008000, // Data Memory Read Parity Error
			MCE_DATA_MEMORY_WRITE_PARITY_ERROR            = 0x00010000, // Data Memory Write Parity Error
			MCE_INSTRUCTION_READ_BUS_ERROR                = 0x00020000, // Instruction Read Bus Error
			MCE_DATA_READ_BUS_ERROR                       = 0x00040000, // Data Read Bus Error
			MCE_DATA_WRITE_BUS_ERROR                      = 0x00080000, // Data Write Bus Error
			MCE_DATA_WRITE_BUS_ERROR_DSI                  = 0x00100000  // Data Write Bus Error with DSI
		};

		struct NMI                                  : SuperCPU::template Exception<MachineCheckInterrupt> { static const char *GetName() { return "Machine Check Interrupt/NMI Exception"; } };               // Non-Maskable Interrupt: p_nmi_b transitions from negated to asserted.
		struct ErrorReport                          : SuperCPU::template Exception<MachineCheckInterrupt> { static const char *GetName() { return "Machine Check Interrupt/Error Report Exception"; } };      // Non-Maskable Interrupt: Error report
		struct AsynchronousMachineCheck             : SuperCPU::template Exception<MachineCheckInterrupt> { static const char *GetName() { return "Machine Check Interrupt/Asynchronous Machine Check Exception"; } };   // Maskable with MSR[ME]
		
		typedef typename SuperCPU::template ExceptionSet<NMI, ErrorReport, AsynchronousMachineCheck> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::CE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::ME
		                , typename MSR::FE0
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM
		                , typename MSR::RI > MSR_ALWAYS_CLEARED_FIELDS;
		
		MachineCheckInterrupt(typename CONFIG::CPU *cpu) : Super(cpu), machine_check_events(0) {}
		bool GotEvent(MachineCheckEventType machine_check_event) const { return machine_check_events & machine_check_event; }
		MachineCheckInterrupt& SetEvent(MachineCheckEventType machine_check_event) { machine_check_events |= machine_check_event; return *this; }
		void ClearEvents() { machine_check_events = 0; }
		static const char *GetId() { return "machine-check-interrupt"; }
		static const char *GetName() { return "Machine Check Interrupt"; }
	private:
		unsigned int machine_check_events;
	};
	
	struct DataStorageInterrupt : SuperCPU::template InterruptWithAddress<DataStorageInterrupt, 0x20>
	{
		typedef typename SuperCPU::template InterruptWithAddress<DataStorageInterrupt, 0x20> Super;
		
		struct AccessControl                        : SuperCPU::template Exception<DataStorageInterrupt> { static const char *GetName() { return "Data Storage Interrupt/Access Control Exception"; } };               // Access control
		
		typedef typename SuperCPU::template ExceptionSet<AccessControl> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_CLEARED_FIELDS;

		DataStorageInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "data-storage-interrupt"; }
		static const char *GetName() { return "Data Storage Interrupt"; }
	};

	struct InstructionStorageInterrupt : SuperCPU::template Interrupt<InstructionStorageInterrupt, 0x30>
	{
		typedef typename SuperCPU::template Interrupt<InstructionStorageInterrupt, 0x30> Super;
		
		struct AccessControl                        : SuperCPU::template Exception<InstructionStorageInterrupt> { static const char *GetName() { return "Instruction Storage Interrupt/Access Control Exception"; } };        // Access control
		
		typedef typename SuperCPU::template ExceptionSet<AccessControl> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::ME
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_CLEARED_FIELDS;
		
		InstructionStorageInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "instruction-storage-interrupt"; }
		static const char *GetName() { return "Instruction Storage Interrupt"; }
	};

	struct ExternalInputInterrupt : SuperCPU::template Interrupt<ExternalInputInterrupt, 0x40>
	{
		typedef typename SuperCPU::template Interrupt<ExternalInputInterrupt, 0x40> Super;
		
		struct ExternalInput                        : SuperCPU::template Exception<ExternalInputInterrupt> { static const char *GetName() { return "External Input Interrupt/External Input Exception"; } };             // Interrupt Controller interrupt and MSR[EE] = 1
		
		typedef typename SuperCPU::template ExceptionSet<ExternalInput> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_CLEARED_FIELDS;
		
		ExternalInputInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "external-input-interrupt"; }
		static const char *GetName() { return "External Input Interrupt"; }
	};

	struct AlignmentInterrupt : SuperCPU::template InterruptWithAddress<AlignmentInterrupt, 0x50>
	{
		typedef typename SuperCPU::template InterruptWithAddress<AlignmentInterrupt, 0x50> Super;
		
		struct UnalignedLoadStoreMultiple           : SuperCPU::template Exception<AlignmentInterrupt> { static const char *GetName() { return "Alignment Interrupt/Unaligned Load/Store Multiple Exception"; } };                 // lmw, stmw not word aligned
		struct UnalignedLoadLinkStoreConditional    : SuperCPU::template Exception<AlignmentInterrupt> { static const char *GetName() { return "Alignment Interrupt/Unaligned Load Link/Store Conditional Exception"; } };                 // lwarx or stwcx. not word aligned, lharx or sthcx. not halfword aligned
		struct WriteThroughDCBZ                     : SuperCPU::template Exception<AlignmentInterrupt> { static const char *GetName() { return "Alignment Interrupt/Write Through DCBZ Exception"; } };                 // dcbz
		
		typedef typename SuperCPU::template ExceptionSet<UnalignedLoadStoreMultiple, UnalignedLoadLinkStoreConditional, WriteThroughDCBZ> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_CLEARED_FIELDS;
		
		AlignmentInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "alignment-interrupt"; }
		static const char *GetName() { return "Alignment Interrupt"; }
	};
	
	struct ProgramInterrupt : SuperCPU::template Interrupt<ProgramInterrupt, 0x60>
	{
		typedef typename SuperCPU::template Interrupt<ProgramInterrupt, 0x60> Super;
		
		struct IllegalInstruction                   : SuperCPU::template Exception<ProgramInterrupt>  { static const char *GetName() { return "Program Interrupt/Illegal Instruction Exception"; } };                   // illegal instruction
		struct PrivilegeViolation                   : SuperCPU::template Exception<ProgramInterrupt>  { static const char *GetName() { return "Program Interrupt/Privilege Violation Exception"; } };                   // privilege violation
		struct Trap                                 : SuperCPU::template Exception<ProgramInterrupt> { static const char *GetName() { return "Program Interrupt/Trap Exception"; } };                   // trap instruction
		struct UnimplementedInstruction             : SuperCPU::template Exception<ProgramInterrupt> { static const char *GetName() { return "Program Interrupt/Unimplemented Instruction Exception"; } };                   // unimplemented instruction
		
		typedef typename SuperCPU::template ExceptionSet<IllegalInstruction, PrivilegeViolation, Trap> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_CLEARED_FIELDS;
		
		
		ProgramInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "program-interrupt"; }
		static const char *GetName() { return "Program Interrupt"; }
	};

	struct PerformanceMonitorInterrupt : SuperCPU::template Interrupt<PerformanceMonitorInterrupt, 0x70>
	{
		typedef typename SuperCPU::template Interrupt<PerformanceMonitorInterrupt, 0x70> Super;
		
		struct PerformanceCounterOverflow           : SuperCPU::template Exception<PerformanceMonitorInterrupt> { static const char *GetName() { return "Performance Monitor Interrupt/Performance Counter Overflow Exception"; } };        // PMC register overflow
		struct DebugEvent                           : SuperCPU::template Exception<PerformanceMonitorInterrupt> { static const char *GetName() { return "Performance Monitor Interrupt/Debug Event Exception"; } };        // Event w/PMGC0[UDI]=0
		
		typedef typename SuperCPU::template ExceptionSet<PerformanceCounterOverflow, DebugEvent> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_ALWAYS_CLEARED_FIELDS;
		
		PerformanceMonitorInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "performance-monitor-interrupt"; }
		static const char *GetName() { return "Performance Monitor Interrupt"; }
	};
	
	struct SystemCallInterrupt : SuperCPU::template Interrupt<SystemCallInterrupt, 0x80>
	{
		typedef typename SuperCPU::template Interrupt<SystemCallInterrupt, 0x80> Super;
		
		struct SystemCall                           : SuperCPU::template Exception<SystemCallInterrupt> { static const char *GetName() { return "System Call Interrupt/System Call Exception"; } };                // Execution of the System Call (se_sc) instruction
		
		typedef typename SuperCPU::template ExceptionSet<SystemCall> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_CLEARED_FIELDS;
		
		static const char *GetId() { return "system-call-interrupt"; }
		static const char *GetName() { return "System Call Interrupt"; }
		
		SystemCallInterrupt(typename CONFIG::CPU *cpu) : Super(cpu), elev(0) {}
		void SetELEV(unsigned int _elev) { elev = _elev; }
		unsigned int GetELEV() const { return elev; }
		void ClearELEV() { elev = 0; }
	private:
		unsigned int elev;
	};

	struct DebugInterrupt : SuperCPU::template Interrupt<DebugInterrupt, 0x90>
	{
		typedef typename SuperCPU::template Interrupt<DebugInterrupt, 0x90> Super;
		
		enum DebugEventType
		{
			DBG_UNCONDITIONAL_DEBUG_EVENT            = 0x00000001, // Async
			DBG_INSTRUCTION_COMPLETE_DEBUG_EVENT     = 0x00000002, // Sync
			DBG_BRANCH_TAKEN_DEBUG_EVENT             = 0x00000004, // Sync
			DBG_INTERRUPT_TAKEN_DEBUG_EVENT          = 0x00000008, // Sync
			DBG_CRITICAL_INTERRUPT_TAKEN_DEBUG_EVENT = 0x00000010, // Async
			DBG_TRAP_INSTRUCTION_DEBUG_EVENT         = 0x00000020, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE1         = 0x00000040, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE2         = 0x00000080, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE3         = 0x00000100, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE4         = 0x00000200, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE5         = 0x00000400, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE6         = 0x00000800, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE7         = 0x00001000, // Sync
			DBG_INSTRUCTION_ADDRESS_COMPARE8         = 0x00002000, // Sync
			DBG_DATA_ADDRESS_COMPARE1_READ           = 0x00004000, // Sync
			DBG_DATA_ADDRESS_COMPARE1_WRITE          = 0x00008000, // Sync
			DBG_DATA_ADDRESS_COMPARE2_READ           = 0x00010000, // Sync
			DBG_DATA_ADDRESS_COMPARE2_WRITE          = 0x00020000, // Sync
			DBG_DEBUG_NOTIFY_INTERRUPT               = 0x00040000, // Sync
			DBG_RETURN_DEBUG_EVENT                   = 0x00080000, // Sync
			DBG_CRITICAL_RETURN_DEBUG_EVENT          = 0x00100000, // Async
			DBG_EXTERNAL_DEBUG_EVENT1                = 0x00200000, // Async
			DBG_EXTERNAL_DEBUG_EVENT2                = 0x00400000, // Async
			DBG_PERFORMANCE_MONITOR_DEBUG_EVENT      = 0x00800000, // Async
			DBG_MPU_DEBUG_EVENT                      = 0x01000000, // Sync
			DBG_IMPRECISE_DEBUG_EVENT                = 0x02000000  // Sync
		};
		
		struct AsynchronousDebugEvent                   : SuperCPU::template Exception<DebugInterrupt> { static const char *GetName() { return "Debug Interrupt/Asynchronous Debug Event Exception"; } };
		struct SynchronousDebugEvent                    : SuperCPU::template Exception<DebugInterrupt> { static const char *GetName() { return "Debug Interrupt/Synchronous Debug Event Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet< AsynchronousDebugEvent, SynchronousDebugEvent > ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::DE
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_ALWAYS_CLEARED_FIELDS;
		
		DebugInterrupt(typename CONFIG::CPU *cpu) : Super(cpu), dbg_events(0) {}
		bool GotEvent(DebugEventType dbg_event) const { return dbg_events & dbg_event; }
		DebugInterrupt& SetEvent(DebugEventType dbg_event) { dbg_events |= dbg_event; return *this; }
		void ClearEvents() { dbg_events = 0; }
		
		static const char *GetId() { return "debug-interrupt"; }
		static const char *GetName() { return "Debug Interrupt"; }
	private:
		uint32_t dbg_events;
	};

	struct EmbeddedFloatingPointDataInterrupt : SuperCPU::template Interrupt<EmbeddedFloatingPointDataInterrupt, 0xa0>
	{
		typedef typename SuperCPU::template Interrupt<EmbeddedFloatingPointDataInterrupt, 0xa0> Super;
		
		struct EmbeddedFloatingPointData            : SuperCPU::template Exception<EmbeddedFloatingPointDataInterrupt> { static const char *GetName() { return "Embedded Floating-Point Data Interrupt/Embedded Floating-Point Data Exception"; } }; // Embedded Floating-point Data Exception
		
		typedef typename SuperCPU::template ExceptionSet<EmbeddedFloatingPointData> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_CLEARED_FIELDS;
		
		
		EmbeddedFloatingPointDataInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "embedded-floating-point-data-interrupt"; }
		static const char *GetName() { return "Embedded Floating-Point Data Interrupt"; }
	};
	
	struct EmbeddedFloatingPointRoundInterrupt : SuperCPU::template Interrupt<EmbeddedFloatingPointRoundInterrupt, 0xb0>
	{
		typedef typename SuperCPU::template Interrupt<EmbeddedFloatingPointRoundInterrupt, 0xb0> Super;
		
		struct EmbeddedFloatingPointRound           : SuperCPU::template Exception<EmbeddedFloatingPointRoundInterrupt> { static const char *GetName() { return "Embedded Floating-Point Round Interrupt/Embedded Floating-Point Round Exception"; } };// Embedded Floating-point Round Exception
		
		typedef typename SuperCPU::template ExceptionSet<EmbeddedFloatingPointRound> ALL;
		
		typedef FieldSet< typename MSR::SPV
		                , typename MSR::WE
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::FE1
		                , typename MSR::IS
		                , typename MSR::DS
		                , typename MSR::PMM > MSR_CLEARED_FIELDS;
		
		EmbeddedFloatingPointRoundInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "embedded-floating-point-round-interrupt"; }
		static const char *GetName() { return "Embedded Floating-Point Round Interrupt"; }
	};

	// Priorities:
	//  0   SystemResetInterrupt                 Reset
	//  1   MachineCheckInterrupt                NMI
	//  2   MachineCheckInterrupt                ErrorReport
	//  3   MachineCheckInterrupt                AsynchronousMachineCheck
	//  4   ExternalInputInterrupt               ExternalInput
	//  5   CriticalInputInterrupt               CriticalInput
	//  6   DebugInterrupt                       AsynchronousDebugEvent
	//  7   PerformanceMonitorInterrupt          PerformanceCounterOverflow
	//  8   PerformanceMonitorInterrupt          DebugEvent
	//  9   InstructionStorageInterrupt          AccessControl
	// 10   ProgramInterrupt                     IllegalInstruction
	// 11   ProgramInterrupt                     PrivilegeViolation
	// 12   ProgramInterrupt                     Trap
	// 13   ProgramInterrupt                     UnimplementedInstruction
	// 14   SystemCallInterrupt                  SystemCall
	// 15   DataStorageInterrupt                 AccessControl
	// 16   AlignmentInterrupt                   UnalignedLoadStoreMultiple
	// 17   AlignmentInterrupt                   UnalignedLoadLinkStoreConditional
	// 18   AlignmentInterrupt                   WriteThroughDCBZ
	// 19   EmbeddedFloatingPointDataInterrupt   EmbeddedFloatingPointData
	// 20   EmbeddedFloatingPointRoundInterrupt  EmbeddedFloatingPointRound
	// 21   DebugInterrupt                       SynchronousDebugEvent

	typedef typename SuperCPU::template ExceptionPrioritySet<
		typename SystemResetInterrupt               ::Reset                            ,
		typename MachineCheckInterrupt              ::NMI                              ,
		typename MachineCheckInterrupt              ::ErrorReport                      ,
		typename MachineCheckInterrupt              ::AsynchronousMachineCheck         ,
		typename ExternalInputInterrupt             ::ExternalInput                    ,
		typename CriticalInputInterrupt             ::CriticalInput                    ,
		typename DebugInterrupt                     ::AsynchronousDebugEvent           ,
		typename PerformanceMonitorInterrupt        ::PerformanceCounterOverflow       ,
		typename PerformanceMonitorInterrupt        ::DebugEvent                       ,
		typename InstructionStorageInterrupt        ::AccessControl                    ,
		typename ProgramInterrupt                   ::IllegalInstruction               ,
		typename ProgramInterrupt                   ::PrivilegeViolation               ,
		typename ProgramInterrupt                   ::Trap                             ,
		typename ProgramInterrupt                   ::UnimplementedInstruction         ,
		typename SystemCallInterrupt                ::SystemCall                       ,
		typename DataStorageInterrupt               ::AccessControl                    ,
		typename AlignmentInterrupt                 ::UnalignedLoadStoreMultiple       ,
		typename AlignmentInterrupt                 ::UnalignedLoadLinkStoreConditional,
		typename AlignmentInterrupt                 ::WriteThroughDCBZ                 ,
		typename EmbeddedFloatingPointDataInterrupt ::EmbeddedFloatingPointData        ,
		typename EmbeddedFloatingPointRoundInterrupt::EmbeddedFloatingPointRound       ,
		typename DebugInterrupt                     ::SynchronousDebugEvent> EXCEPTION_PRIORITIES;
	
	void ProcessInterrupt(SystemResetInterrupt *);
	void ProcessInterrupt(MachineCheckInterrupt *);
	void ProcessInterrupt(DataStorageInterrupt *);
	void ProcessInterrupt(InstructionStorageInterrupt *);
	void ProcessInterrupt(AlignmentInterrupt *);
	void ProcessInterrupt(ProgramInterrupt *);
	void ProcessInterrupt(EmbeddedFloatingPointDataInterrupt *);
	void ProcessInterrupt(EmbeddedFloatingPointRoundInterrupt *);
	void ProcessInterrupt(SystemCallInterrupt *);
	void ProcessInterrupt(CriticalInputInterrupt *);
	void ProcessInterrupt(ExternalInputInterrupt *);
	void ProcessInterrupt(PerformanceMonitorInterrupt *);
	void ProcessInterrupt(DebugInterrupt *);
	
	void UpdateExceptionEnable();
	
	void SetAutoVector(bool value);
	void SetVectorOffset(EFFECTIVE_ADDRESS value);
	
	bool IsStorageCacheable(STORAGE_ATTR storage_attr) const { return !(storage_attr & TYPES::SA_I); }

	virtual void InvalidateDirectMemPtr(PHYSICAL_ADDRESS start_addr, PHYSICAL_ADDRESS end_addr) {}
	
public:

	void FlushInstructionBuffer();
	bool InstructionFetch(EFFECTIVE_ADDRESS addr, typename CONFIG::VLE_CODE_TYPE& insn);
	void StepOneInstruction();
	
	struct __EFPProcessInput__
	{
		__EFPProcessInput__( CPU& _cpu ) : cpu(_cpu), finv(false) {};
		template <class FLOAT>
		__EFPProcessInput__& set( FLOAT& input ) { finv |= not check_input( input ) ; return *this; }
		template <class FLOAT>
		static bool check_input( FLOAT& input )
		{
			typename FLOAT::inherited& iimpl = input.GetImpl();
			if (unlikely(iimpl.isDenormalized()))
			{
				iimpl.setZero(iimpl.isNegative());
				return false;
			}
			
			if (unlikely(iimpl.hasInftyExponent()))
				return false;
			
			return true;
		}
		bool proceed()
		{
			cpu.spefscr.template Set<typename SPEFSCR::FINV>(finv);
			if (finv)
			{
				cpu.spefscr.template Set<typename SPEFSCR::FINVS>(true);
				if (cpu.spefscr.template Get<typename SPEFSCR::FINVE>())
				{
					cpu.template ThrowException<typename ProgramInterrupt::UnimplementedInstruction>();
					return false;
				}
			}
			return true;
		}
		
		CPU& cpu;
		bool finv;
	};
	
	__EFPProcessInput__
	EFPProcessInput()
	{
		spefscr.template Set<typename SPEFSCR::FG>(false);
		spefscr.template Set<typename SPEFSCR::FX>(false);
		spefscr.template Set<typename SPEFSCR::FGH>(false);
		spefscr.template Set<typename SPEFSCR::FXH>(false);
		spefscr.template Set<typename SPEFSCR::FDBZ>(false);
		spefscr.template Set<typename SPEFSCR::FDBZH>(false);
		spefscr.SetDivideByZero( false );
		return __EFPProcessInput__( *this );
	}
	
	template <class FLOAT, class FLAGS>
	bool
	EFPProcessOutput( FLOAT& output, FLAGS const& flags )
	{
		typename FLOAT::inherited& oimpl = output.GetImpl();
		if (oimpl.hasInftyExponent())
		{
			bool neg = oimpl.isNegative();
			oimpl.setInfty();
			oimpl.setToPrevious();
			oimpl.setNegative(neg);
		}
		bool inexact = flags.isApproximate() and not spefscr.template Get<typename CPU::SPEFSCR::FINV>();
		bool overflow = inexact and flags.isOverflow();
		if (not spefscr.SetOverflow( overflow ))
			return false;
		bool underflow = inexact and flags.isUnderflow();
		if (oimpl.isDenormalized())
		{
			oimpl.setZero(oimpl.isNegative());
			inexact = true, underflow = true;
		}
		if (not spefscr.SetUnderflow( underflow ))
			return false;

		if (inexact)
		{
			// Compute inexact flags (FG, FX)
			spefscr.template Set<typename SPEFSCR::FINXS>(true);
			if (spefscr.template Get<typename SPEFSCR::FINXE>())
			{
				this->template ThrowException<typename ProgramInterrupt::UnimplementedInstruction>();
				return false;
			}
		}

		return true;
	}
	
protected:
	////////////////////////// Run-time parameters ////////////////////////////
	
	uint8_t cpuid;
	unisim::kernel::variable::Parameter<uint8_t> param_cpuid;

	uint32_t processor_version;
	unisim::kernel::variable::Parameter<uint32_t> param_processor_version;
	
	uint32_t system_version;
	unisim::kernel::variable::Parameter<uint32_t> param_system_version;

	uint32_t system_information;
	unisim::kernel::variable::Parameter<uint32_t> param_system_information;

	PHYSICAL_ADDRESS local_memory_base_addr;
	unisim::kernel::variable::Parameter<PHYSICAL_ADDRESS> param_local_memory_base_addr;

	PHYSICAL_ADDRESS local_memory_size;
	unisim::kernel::variable::Parameter<PHYSICAL_ADDRESS> param_local_memory_size;
	
	///////////////////////////// Interrupts //////////////////////////////////
	
	bool enable_auto_vectored_interrupts;
	EFFECTIVE_ADDRESS vector_offset;

	SystemResetInterrupt                system_reset_interrupt;
	CriticalInputInterrupt              critical_input_interrupt;
	MachineCheckInterrupt               machine_check_interrupt;
	DataStorageInterrupt                data_storage_interrupt;
	InstructionStorageInterrupt         instruction_storage_interrupt;
	ExternalInputInterrupt              external_input_interrupt;
	AlignmentInterrupt                  alignment_interrupt;
	ProgramInterrupt                    program_interrupt;
	PerformanceMonitorInterrupt         performance_monitor_interrupt;
	SystemCallInterrupt                 system_call_interrupt;
	DebugInterrupt                      debug_interrupt;
	EmbeddedFloatingPointDataInterrupt  embedded_floating_point_data_interrupt;
	EmbeddedFloatingPointRoundInterrupt embedded_floating_point_round_interrupt;

	////////////////////////// Instruction Buffer /////////////////////////////

	EFFECTIVE_ADDRESS instruction_buffer_base_addr;
	uint16_t instruction_buffer[CONFIG::INSTRUCTION_BUFFER_SIZE / 2];
	
	/////////////////////// VLE Instruction decoder ///////////////////////////
	
	typename CONFIG::VLE_DECODER vle_decoder;
	typename CONFIG::VLE_OPERATION *operation;
	
	/////////////////////////// Machine State Register ////////////////////////
	
	MSR msr;
	
	////////////////////////// Special Purpose Registers //////////////////////
	
	typename SuperCPU::SRR0 srr0;
	typename SuperCPU::SRR1 srr1;
	typename SuperCPU::PID0 pid0;
	typename SuperCPU::CSRR0 csrr0;
	typename SuperCPU::CSRR1 csrr1;
	typename SuperCPU::DEAR dear;
	typename SuperCPU::ESR esr;
	typename SuperCPU::IVPR ivpr;
	typename SuperCPU::USPRG0 usprg0;
	typename SuperCPU::SPRG0 sprg0;
	typename SuperCPU::SPRG1 sprg1;
	typename SuperCPU::SPRG2 sprg2;
	typename SuperCPU::SPRG3 sprg3;
	PIR pir;
	typename SuperCPU::PVR pvr;
	typename SuperCPU::DBSR dbsr;
	typename SuperCPU::DBCR0 dbcr0;
	typename SuperCPU::DBCR1 dbcr1;
	typename SuperCPU::DBCR2 dbcr2;
	typename SuperCPU::IAC1 iac1;
	typename SuperCPU::IAC2 iac2;
	typename SuperCPU::IAC3 iac3;
	typename SuperCPU::IAC4 iac4;
	typename SuperCPU::DAC1 dac1;
	typename SuperCPU::DAC2 dac2;
	typename SuperCPU::DVC1 dvc1;
	typename SuperCPU::DVC2 dvc2;
	typename SuperCPU::TIR tir;
	typename SuperCPU::SPEFSCR spefscr;
	typename SuperCPU::NPIDR npidr;
	typename SuperCPU::DBCR3 dbcr3;
	typename SuperCPU::DBCNT dbcnt;
	typename SuperCPU::DBCR4 dbcr4;
	typename SuperCPU::DBCR5 dbcr5;
	typename SuperCPU::IAC5 iac5;
	typename SuperCPU::IAC6 iac6;
	typename SuperCPU::IAC7 iac7;
	typename SuperCPU::IAC8 iac8;
	typename SuperCPU::MCSRR0 mcsrr0;
	typename SuperCPU::MCSRR1 mcsrr1;
	typename SuperCPU::MCSR mcsr;
	typename SuperCPU::MCAR mcar;
	typename SuperCPU::DSRR0 dsrr0;
	typename SuperCPU::DSRR1 dsrr1;
	typename SuperCPU::DDAM ddam;
	typename SuperCPU::DAC3 dac3;
	typename SuperCPU::DAC4 dac4;
	typename SuperCPU::DBCR7 dbcr7;
	typename SuperCPU::DBCR8 dbcr8;
	typename SuperCPU::DDEAR ddear;
	typename SuperCPU::DVC1U dvc1u;
	typename SuperCPU::DVC2U dvc2u;
	typename SuperCPU::DBCR6 dbcr6;
	typename SuperCPU::EDBRAC0 edbrac0;
	typename SuperCPU::DEVENT devent;
	typename SuperCPU::SIR sir;
	HID0 hid0;
	typename SuperCPU::HID1 hid1;
	typename SuperCPU::BUCSR bucsr;
	typename SuperCPU::SVR svr;
	
	////////////////////////// Device Control Registers ///////////////////////
	
	typename SuperCPU::E2ECTL0 e2ectl0;
	typename SuperCPU::E2EECSR0 e2eecsr0;
	
	//////////////////////// Performance Monitor Registers /////////////////////
	
	typename SuperCPU::performance_monitoring::PMC0 pmc0;
	typename SuperCPU::performance_monitoring::PMC1 pmc1;
	typename SuperCPU::performance_monitoring::PMC2 pmc2;
	typename SuperCPU::performance_monitoring::PMC3 pmc3;
	typename SuperCPU::performance_monitoring::PMGC0 pmgc0;
	typename SuperCPU::performance_monitoring::PMLCa0 pmlca0;
	typename SuperCPU::performance_monitoring::PMLCa1 pmlca1;
	typename SuperCPU::performance_monitoring::PMLCa2 pmlca2;
	typename SuperCPU::performance_monitoring::PMLCa3 pmlca3;
	typename SuperCPU::performance_monitoring::PMLCb0 pmlcb0;
	typename SuperCPU::performance_monitoring::PMLCb1 pmlcb1;
	typename SuperCPU::performance_monitoring::PMLCb2 pmlcb2;
	typename SuperCPU::performance_monitoring::PMLCb3 pmlcb3;
	
	typename SuperCPU::performance_monitoring::UPMC0 upmc0;
	typename SuperCPU::performance_monitoring::UPMC1 upmc1;
	typename SuperCPU::performance_monitoring::UPMC2 upmc2;
	typename SuperCPU::performance_monitoring::UPMC3 upmc3;
	typename SuperCPU::performance_monitoring::UPMGC0 upmgc0;
	typename SuperCPU::performance_monitoring::UPMLCa0 upmlca0;
	typename SuperCPU::performance_monitoring::UPMLCa1 upmlca1;
	typename SuperCPU::performance_monitoring::UPMLCa2 upmlca2;
	typename SuperCPU::performance_monitoring::UPMLCa3 upmlca3;
	typename SuperCPU::performance_monitoring::UPMLCb0 upmlcb0;
	typename SuperCPU::performance_monitoring::UPMLCb1 upmlcb1;
	typename SuperCPU::performance_monitoring::UPMLCb2 upmlcb2;
	typename SuperCPU::performance_monitoring::UPMLCb3 upmlcb3;
	
	///////////////////////// Thread Management Registers /////////////////////
	
	typename SuperCPU::thread_management::TMCFG0 tmcfg0;
	
private:
	struct LocalMemoryGate : unisim::kernel::Service<typename unisim::service::interfaces::Memory<PHYSICAL_ADDRESS> >
	{
		unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<PHYSICAL_ADDRESS> > local_memory_export;
		
		LocalMemoryGate(const char *name, CPU<TYPES, CONFIG> *_cpu)
			: unisim::kernel::Object(name, _cpu)
			, unisim::kernel::Service<typename unisim::service::interfaces::Memory<PHYSICAL_ADDRESS> >(name, _cpu)
			, local_memory_export("local-memory-export", this)
			, cpu(_cpu)
		{
		}
		
		virtual void ResetMemory() {}
		virtual bool ReadMemory(PHYSICAL_ADDRESS addr, void *buffer, uint32_t size) { return cpu->DebugIncomingLoad(addr + cpu->local_memory_base_addr, buffer, size); }
		virtual bool WriteMemory(PHYSICAL_ADDRESS addr, const void *buffer, uint32_t size) { return cpu->DebugIncomingStore(addr + cpu->local_memory_base_addr, buffer, size); }
	private:
		CPU<TYPES, CONFIG> *cpu;
	};
	
	LocalMemoryGate local_memory_gate;
};

} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_CPU_HH__
