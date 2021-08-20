/*
 *  Copyright (c) 2007-2019,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_CPU_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_CPU_HH__

#include <unisim/component/cxx/processor/powerpc/cpu.hh>
#include <unisim/component/cxx/processor/powerpc/floating.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/queue/queue.hh>
#include <unisim/util/queue/queue.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e600 {

template <typename TYPES, typename CONFIG>
class CPU
	: public unisim::component::cxx::processor::powerpc::CPU<TYPES, CONFIG>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Disassembly<typename TYPES::EFFECTIVE_ADDRESS> >
{
public:
	typedef typename unisim::component::cxx::processor::powerpc::CPU<TYPES, CONFIG> SuperCPU;
	typedef typename unisim::util::cache::MemorySubSystem<TYPES, typename CONFIG::CPU> SuperMSS;
	typedef typename TYPES::EFFECTIVE_ADDRESS EFFECTIVE_ADDRESS;
	typedef typename TYPES::ADDRESS ADDRESS;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef typename TYPES::STORAGE_ATTR STORAGE_ATTR;
	
	typedef typename SuperCPU::MCSR MCSR;
	typedef typename SuperCPU::ESR ESR;
	typedef typename SuperCPU::TSR TSR;
	typedef typename SuperCPU::SRR1 SRR1;
	typedef typename SuperCPU::MSSSR0 MSSSR0;
	typedef typename SuperCPU::DSISR DSISR;
	typedef typename SuperCPU::L2CR L2CR;
	
	/////////////////////////// service exports ///////////////////////////////

	unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<EFFECTIVE_ADDRESS> > disasm_export;

	////////////////////////////// constructor ////////////////////////////////
	
	CPU(const char *name, unisim::kernel::Object *parent = 0);

	/////////////////////////////// destructor ////////////////////////////////

	virtual ~CPU();
	
	////////////////////////////// setup hooks ////////////////////////////////

	virtual bool EndSetup();

	//////////////////////////////// Reset ////////////////////////////////////

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
			Effect();
		}
		
		template <typename FIELD> inline void Set(typename MSR::TYPE field_value)
		{
			Super::template Set<FIELD>(field_value);
			Effect();
		}

		MSR& operator = (const typename MSR::TYPE& value)
		{
			this->Super::operator = (value);
			Effect();
			return *this;
		}
		
		void Effect()
		{
			this->cpu->UpdateExceptionEnable();
			this->cpu->UpdatePowerSave();
		}
	};
	
	/////////////////// Hardware Implementation-Dependent Register 0 //////////
	
	struct HID0 : SuperCPU::HID0
	{
		typedef typename SuperCPU::HID0 Super;
		
		HID0(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		HID0(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		inline void Set(unsigned int bit_offset, typename HID0::TYPE bit_value) ALWAYS_INLINE
		{
			Super::Set(bit_offset, bit_value);
			Effect();
		}
		
		template <typename FIELD> inline void Set(typename HID0::TYPE field_value)
		{
			Super::template Set<FIELD>(field_value);
			Effect();
		}

		HID0& operator = (const typename HID0::TYPE& value)
		{
			this->Super::operator = (value);
			Effect();
			return *this;
		}
		
		void Effect()
		{
			this->cpu->UpdatePowerSave();
		}
	};
	
	/////////////////// Hardware Implementation-Dependent Register 1 //////////
	
	struct HID1 : SuperCPU::HID1
	{
		typedef typename SuperCPU::HID1 Super;
		
		HID1(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		HID1(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		inline void Set(unsigned int bit_offset, typename HID1::TYPE bit_value) ALWAYS_INLINE
		{
			Super::Set(bit_offset, bit_value);
			Effect();
		}
		
		template <typename FIELD> inline void Set(typename HID1::TYPE field_value)
		{
			Super::template Set<FIELD>(field_value);
			Effect();
		}

		HID1& operator = (const typename HID1::TYPE& value)
		{
			this->Super::operator = (value);
			Effect();
			return *this;
		}
		
		void Effect()
		{
			this->cpu->UpdateExceptionEnable();
		}
	};
	
	// Decrementer
	struct DEC : SuperCPU::DEC
	{
		typedef typename SuperCPU::DEC Super;
		
		DEC(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		DEC(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		inline void Set(unsigned int bit_offset, typename DEC::TYPE bit_value) ALWAYS_INLINE
		{
			bool old_bit_0 = this->template Get<typename DEC::_0>();
			Super::Set(bit_offset, bit_value);
			Effect(old_bit_0, this->template Get<typename DEC::_0>());
		}
		
		template <typename FIELD> inline void Set(typename DEC::TYPE field_value)
		{
			bool old_bit_0 = this->template Get<typename DEC::_0>();
			Super::template Set<FIELD>(field_value);
			Effect(old_bit_0, this->template Get<typename DEC::_0>());
		}

		DEC& operator = (const typename DEC::TYPE& value)
		{
			bool old_bit_0 = this->template Get<typename DEC::_0>();
			this->Super::operator = (value);
			Effect(old_bit_0, this->template Get<typename DEC::_0>());
			return *this;
		}
		
		void Effect(bool old_bit_0, bool new_bit_0)
		{
			if(!old_bit_0 && new_bit_0) // bit 0 toggling from 0 to 1 ?
			{
				this->cpu->template ThrowException<typename DecrementerInterrupt::Decrementer>();
			}
		}
	};

	//////////////  Instruction Cache and Interrupt Control Register //////////
	
	struct ICTRL : SuperCPU::ICTRL
	{
		typedef typename SuperCPU::ICTRL Super;
		
		ICTRL(typename CONFIG::CPU *_cpu) : Super(_cpu) {}
		ICTRL(typename CONFIG::CPU *_cpu, uint32_t _value) : Super(_cpu, _value) {}
		
		virtual bool MoveTo(uint32_t value)
		{
			bool old_cirq = this->template Get<typename ICTRL::CIRQ>();
			if(!this->Super::MoveTo(value)) return false;
			bool new_cirq = this->template Get<typename ICTRL::CIRQ>();
				
			if(!old_cirq && new_cirq)
			{
				this->cpu->template ThrowException<typename ExternalInterrupt::ExternalInterruptException>().SoftwareInterruptRequest(true);
			}
			return true;
		}
	};

	////////////////////////////// SPR General ////////////////////////////////
	
	struct SPRG_x4
	{
		SPRG_x4(typename CONFIG::CPU *cpu) : sprg0(cpu), sprg1(cpu), sprg2(cpu), sprg3(cpu) {}
		
		typename SuperCPU::SPRG0 sprg0;
		typename SuperCPU::SPRG1 sprg1;
		typename SuperCPU::SPRG2 sprg2;
		typename SuperCPU::SPRG3 sprg3;
	};
	
	struct SPRG_x8
	{
		SPRG_x8(typename CONFIG::CPU *cpu) : sprg0(cpu), sprg1(cpu), sprg2(cpu), sprg3(cpu), sprg4(cpu), sprg5(cpu), sprg6(cpu), sprg7(cpu) {}
		
		typename SuperCPU::SPRG0 sprg0;
		typename SuperCPU::SPRG1 sprg1;
		typename SuperCPU::SPRG2 sprg2;
		typename SuperCPU::SPRG3 sprg3;
		typename SuperCPU::SPRG4 sprg4;
		typename SuperCPU::SPRG5 sprg5;
		typename SuperCPU::SPRG6 sprg6;
		typename SuperCPU::SPRG7 sprg7;
	};
	
	SWITCH_ENUM_TRAIT(Model, _SPRGs_impl);
	CASE_ENUM_TRAIT(MPC7400    , _SPRGs_impl) { typedef SPRG_x4 SPRGs; };
	CASE_ENUM_TRAIT(MPC7410    , _SPRGs_impl) { typedef SPRG_x4 SPRGs; };
	CASE_ENUM_TRAIT(MPC7441    , _SPRGs_impl) { typedef SPRG_x4 SPRGs; };
	CASE_ENUM_TRAIT(MPC7445    , _SPRGs_impl) { typedef SPRG_x8 SPRGs; };
	CASE_ENUM_TRAIT(MPC7447    , _SPRGs_impl) { typedef SPRG_x8 SPRGs; };
	CASE_ENUM_TRAIT(MPC7447A   , _SPRGs_impl) { typedef SPRG_x8 SPRGs; };
	CASE_ENUM_TRAIT(MPC7448    , _SPRGs_impl) { typedef SPRG_x8 SPRGs; };
	CASE_ENUM_TRAIT(MPC7450    , _SPRGs_impl) { typedef SPRG_x4 SPRGs; };
	CASE_ENUM_TRAIT(MPC7451    , _SPRGs_impl) { typedef SPRG_x4 SPRGs; };
	CASE_ENUM_TRAIT(MPC7455    , _SPRGs_impl) { typedef SPRG_x8 SPRGs; };
	CASE_ENUM_TRAIT(MPC7457    , _SPRGs_impl) { typedef SPRG_x8 SPRGs; };
	typedef typename ENUM_TRAIT(CONFIG::MODEL, _SPRGs_impl)::SPRGs SPRGs;

	///////////// Interface with .isa behavioral description files ////////////
	
	MSR& GetMSR() { return msr; }
	typename SuperCPU::SRR0& GetSRR0() { return srr0; }
	typename SuperCPU::SRR1& GetSRR1() { return srr1; }
	HID0& GetHID0() { return hid0; }
	typename SuperCPU::L2CR& GetL2CR() { return l2cr; }
	typename SuperCPU::PTEHI& GetPTEHI() { return ptehi; }
	typename SuperCPU::PTELO& GetPTELO() { return ptelo; }

	bool Lwarx(unsigned int rd, EFFECTIVE_ADDRESS addr);
	bool Stwcx(unsigned int rs, EFFECTIVE_ADDRESS addr);
	bool Sync();
	bool Isync();
	bool TLB_Sync();
	bool Rfi();
	
	//////////////////////////////// Interrupts ///////////////////////////////
	
	struct SystemResetInterrupt : SuperCPU::template Interrupt<SystemResetInterrupt, 0x00100>
	{
		typedef typename SuperCPU::template Interrupt<SystemResetInterrupt, 0x00100> Super;
		
		struct HardReset : SuperCPU::template Exception<SystemResetInterrupt> { static const char *GetName() { return "System Reset Interrupt/Hard Reset Exception"; } };
		struct SoftReset : SuperCPU::template Exception<SystemResetInterrupt> { static const char *GetName() { return "System Reset Interrupt/Soft Reset Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet< HardReset,
		                                                  SoftReset > ALL;
		
		typedef FieldSet< typename MSR::VEC
		                , typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::PMM
		                , typename MSR::RI > MSR_ALWAYS_CLEARED_FIELDS;
		
		SystemResetInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "system-reset-interrupt"; }
		static const char *GetName() { return "System Reset Interrupt"; }
	};
	
	struct MachineCheckInterrupt : SuperCPU::template InterruptWithAddress<MachineCheckInterrupt, 0x00200>
	{
		typedef typename SuperCPU::template InterruptWithAddress<MachineCheckInterrupt, 0x00200> Super;
		
		struct TEA : SuperCPU::template Exception<MachineCheckInterrupt> { static const char *GetName() { return "Machine Check Interrupt/Transfer Error Acknowledge Exception"; } };
		struct MCP : SuperCPU::template Exception<MachineCheckInterrupt> { static const char *GetName() { return "Machine Check Interrupt/Machine Check Exception"; } };

		typedef typename SuperCPU::template ExceptionSet< TEA
		                                                , MCP > ALL;
		typedef FieldSet< typename MSR::VEC
		                , typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::ME
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::PMM
		                , typename MSR::RI > MSR_ALWAYS_CLEARED_FIELDS;
		
		MachineCheckInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "machine-check-interrupt"; }
		static const char *GetName() { return "Machine Check Interrupt"; }
	};

	struct DataStorageInterrupt : SuperCPU::template InterruptWithAddress<DataStorageInterrupt, 0x00300>
	{
		typedef typename SuperCPU::template InterruptWithAddress<DataStorageInterrupt, 0x00300> Super;
		
		struct ProtectionViolation         : SuperCPU::template Exception<DataStorageInterrupt> { static const char *GetName() { return "Data Storage Interrupt/Protection Violation Exception"; } };
		struct DirectStore                 : SuperCPU::template Exception<DataStorageInterrupt> { static const char *GetName() { return "Data Storage Interrupt/Direct Store Exception Exception"; } };
		struct PageFault                   : SuperCPU::template Exception<DataStorageInterrupt> { static const char *GetName() { return "Data Storage Interrupt/Page Fault Exception Exception"; } };
		struct DataAddressBreakpoint       : SuperCPU::template Exception<DataStorageInterrupt> { static const char *GetName() { return "Data Storage Interrupt/Data Address Breakpoint Exception"; } };
		struct ExternalAccessDisabled      : SuperCPU::template Exception<DataStorageInterrupt> { static const char *GetName() { return "Data Storage Interrupt/External Access Disable Exception"; } };
		struct WriteThroughLinkedLoadStore : SuperCPU::template Exception<DataStorageInterrupt> { static const char *GetName() { return "Data Storage Interrupt/Write Through Linked Load/Store Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet< ProtectionViolation        
		                                                , DirectStore                
		                                                , PageFault                  
		                                                , DataAddressBreakpoint      
		                                                , ExternalAccessDisabled     
		                                                , WriteThroughLinkedLoadStore > ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		DataStorageInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "data-storage-interrupt"; }
		static const char *GetName() { return "Data Storage Interrupt"; }
		
		DataStorageInterrupt& SetWrite(bool v) { write = v; return *this; }
		bool IsWrite() const { return write; }
	private:
		bool write;
	};

	struct InstructionStorageInterrupt : SuperCPU::template Interrupt<InstructionStorageInterrupt, 0x00400>
	{
		typedef typename SuperCPU::template Interrupt<InstructionStorageInterrupt, 0x00400> Super;
		
		struct GuardedMemory       : SuperCPU::template Exception<InstructionStorageInterrupt> { static const char *GetName() { return "Instruction Storage Instruction Interrupt/Guarded Memory Exception"; } };
		struct PageFault           : SuperCPU::template Exception<InstructionStorageInterrupt> { static const char *GetName() { return "Instruction Storage Instruction Interrupt/Page Fault Exception"; } };
		struct ProtectionViolation : SuperCPU::template Exception<InstructionStorageInterrupt> { static const char *GetName() { return "Instruction Storage Instruction Interrupt/Protection Violation Exception"; } };
		struct DirectStore         : SuperCPU::template Exception<InstructionStorageInterrupt> { static const char *GetName() { return "Instruction Storage Instruction Interrupt/Direct Store Exception"; } };
		struct NoExecute           : SuperCPU::template Exception<InstructionStorageInterrupt> { static const char *GetName() { return "Instruction Storage Instruction Interrupt/No Execute Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet< GuardedMemory      
		                                                , PageFault          
		                                                , ProtectionViolation
		                                                , DirectStore > ALL;        

		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		InstructionStorageInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "instruction-storage-interrupt"; }
		static const char *GetName() { return "Instruction Storage Interrupt"; }
	};

	struct ExternalInterrupt : SuperCPU::template Interrupt<ExternalInterrupt, 0x00500>
	{
		typedef typename SuperCPU::template Interrupt<ExternalInterrupt, 0x00500> Super;
		
		struct ExternalInterruptException : SuperCPU::template Exception<ExternalInterrupt> { static const char *GetName() { return "External Interrupt/External Interrupt Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<ExternalInterruptException> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		ExternalInterrupt(typename CONFIG::CPU *cpu) : Super(cpu), sw_irq(false) {}
		static const char *GetId() { return "external-interrupt"; }
		static const char *GetName() { return "External Interrupt"; }
		
		ExternalInterrupt& SoftwareInterruptRequest(bool v) { sw_irq = v; return *this; }
		bool SoftwareInterruptRequest() const { return sw_irq; }
		void Clear() { sw_irq = false; }
	private:
		bool sw_irq;
	};

	struct AlignmentInterrupt : SuperCPU::template InterruptWithAddress<AlignmentInterrupt, 0x00600>
	{
		typedef typename SuperCPU::template InterruptWithAddress<AlignmentInterrupt, 0x00600> Super;
		
		struct Alignment : SuperCPU::template Exception<AlignmentInterrupt> { static const char *GetName() { return "Alignment Interrupt/Alignment Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<Alignment> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		AlignmentInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "alignment-interrupt"; }
		static const char *GetName() { return "Alignment Interrupt"; }
	};
	
	struct ProgramInterrupt : SuperCPU::template Interrupt<ProgramInterrupt, 0x00700>
	{
		typedef typename SuperCPU::template Interrupt<ProgramInterrupt, 0x00700> Super;
		
		struct IllegalInstruction                   : SuperCPU::template Exception<ProgramInterrupt>  { static const char *GetName() { return "Program Interrupt/Illegal Instruction Exception"; } };
		struct PrivilegeViolation                   : SuperCPU::template Exception<ProgramInterrupt>  { static const char *GetName() { return "Program Interrupt/Privilege Violation Exception"; } };
		struct Trap                                 : SuperCPU::template Exception<ProgramInterrupt> { static const char *GetName() { return "Program Interrupt/Trap Exception"; } };
		struct FloatingPoint                        : SuperCPU::template Exception<ProgramInterrupt> { static const char *GetName() { return "Program Interrupt/Floating-point"; } };
		struct UnimplementedInstruction             : SuperCPU::template Exception<ProgramInterrupt> { static const char *GetName() { return "Program Interrupt/Unimplemented Instruction Exception"; } };                   // unimplemented instruction
		
		typedef typename SuperCPU::template ExceptionSet<IllegalInstruction, PrivilegeViolation, Trap, FloatingPoint, UnimplementedInstruction> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		ProgramInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "program-interrupt"; }
		static const char *GetName() { return "Program Interrupt"; }
	};

	struct FloatingPointUnavailableInterrupt : SuperCPU::template Interrupt<FloatingPointUnavailableInterrupt, 0x00800>
	{
		typedef typename SuperCPU::template Interrupt<FloatingPointUnavailableInterrupt, 0x00800> Super;
		
		struct FloatingPointUnavailable : SuperCPU::template Exception<FloatingPointUnavailableInterrupt> { static const char *GetName() { return "Floating-point Unavailable Interrupt/Floating-point Unavailable Interrupt"; } };
		
		typedef typename SuperCPU::template ExceptionSet<FloatingPointUnavailable> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		FloatingPointUnavailableInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "floating-point-unvailable-interrupt"; }
		static const char *GetName() { return "Floating-point Unavailable Interrupt"; }
	};
	
	struct DecrementerInterrupt : SuperCPU::template Interrupt<DecrementerInterrupt, 0x00900>
	{
		typedef typename SuperCPU::template Interrupt<DecrementerInterrupt, 0x00900> Super;
		
		struct Decrementer : SuperCPU::template Exception<DecrementerInterrupt> { static const char *GetName() { return "Decrementer Interrupt/Decrementer Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<Decrementer> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		DecrementerInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "decrementer-interrupt"; }
		static const char *GetName() { return "Decrementer Interrupt"; }
	};

	struct SystemCallInterrupt : SuperCPU::template Interrupt<SystemCallInterrupt, 0x00c00>
	{
		typedef typename SuperCPU::template Interrupt<SystemCallInterrupt, 0x00c00> Super;
		
		struct SystemCall                           : SuperCPU::template Exception<SystemCallInterrupt> { static const char *GetName() { return "System Call Interrupt/System Call Exception"; } };                // Execution of the System Call (se_sc) instruction
		
		typedef typename SuperCPU::template ExceptionSet<SystemCall> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		static const char *GetId() { return "system-call-interrupt"; }
		static const char *GetName() { return "System Call Interrupt"; }
		
		SystemCallInterrupt(typename CONFIG::CPU *cpu) : Super(cpu), elev(0) {}
		void SetELEV(unsigned int _elev) { elev = _elev; }
		unsigned int GetELEV() const { return elev; }
		void ClearELEV() { elev = 0; }
	private:
		unsigned int elev;
	};

	struct TraceInterrupt : SuperCPU::template Interrupt<TraceInterrupt, 0x00d00>
	{
		typedef typename SuperCPU::template Interrupt<TraceInterrupt, 0x00d00> Super;
		
		struct Trace : SuperCPU::template Exception<TraceInterrupt> { static const char *GetName() { return "Trace Interrupt/Trace Exception"; } };
		
		typedef typename SuperCPU::template ExceptionSet<Trace> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		TraceInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "trace-interrupt"; }
		static const char *GetName() { return "Trace Interrupt"; }
	};
	
	struct PerformanceMonitorInterrupt : SuperCPU::template Interrupt<PerformanceMonitorInterrupt, 0x00f00>
	{
		typedef typename SuperCPU::template Interrupt<PerformanceMonitorInterrupt, 0x00f00> Super;
		
		struct PerformanceMonitor : SuperCPU::template Exception<PerformanceMonitorInterrupt> { static const char *GetName() { return "Performance Monitor Interrupt/Performance Monitor Exception"; } };

		typedef typename SuperCPU::template ExceptionSet<PerformanceMonitor> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		PerformanceMonitorInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "performance-monitor-interrupt"; }
		static const char *GetName() { return "Performance Monitor Interrupt"; }
	};
	
	struct AltivecUnavailableInterrupt : SuperCPU::template Interrupt<AltivecUnavailableInterrupt, 0x00f20>
	{
		typedef typename SuperCPU::template Interrupt<AltivecUnavailableInterrupt, 0x00f20> Super;
		
		struct AltivecUnavailable : SuperCPU::template Exception<AltivecUnavailableInterrupt> { static const char *GetName() { return "Altivec Unavailable Interrupt/Altivec Unavailable Exception"; } };

		typedef typename SuperCPU::template ExceptionSet<AltivecUnavailable> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		AltivecUnavailableInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "altivec-unavailable-interrupt"; }
		static const char *GetName() { return "Altivec Unavailable Interrupt"; }
	};
	
	struct InstructionTLBMissInterrupt : SuperCPU::template Interrupt<InstructionTLBMissInterrupt, 0x01000>
	{
		typedef typename SuperCPU::template Interrupt<InstructionTLBMissInterrupt, 0x01000> Super;
		
		struct InstructionTLBMiss : SuperCPU::template Exception<InstructionTLBMissInterrupt> { static const char *GetName() { return "Instruction TLB Miss Interrupt/Instruction TLB Miss Exception"; } };

		typedef typename SuperCPU::template ExceptionSet<InstructionTLBMiss> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		InstructionTLBMissInterrupt(typename CONFIG::CPU *cpu) : Super(cpu), tlbmiss(0), ptehi(0), key(0) {}
		static const char *GetId() { return "instruction-tlb-miss-interrupt"; }
		static const char *GetName() { return "Instruction TLB Miss Interrupt"; }
		
		InstructionTLBMissInterrupt& SetTLBMISS(uint32_t v) { tlbmiss = v; return *this; }
		InstructionTLBMissInterrupt& SetPTEHI(uint32_t v) { ptehi = v; return *this; }
		InstructionTLBMissInterrupt& SetKey(unsigned int v) { key = v; return *this; }
		void Clear() { tlbmiss = 0; ptehi = 0; key = 0; }
		uint32_t GetTLBMISS() const { return tlbmiss; }
		uint32_t GetPTEHI() const { return ptehi; }
		unsigned int GetKey() const { return key; }
	private:
		uint32_t tlbmiss;
		uint32_t ptehi;
		unsigned int key;
	};
	
	struct DataTLBMissOnLoadInterrupt : SuperCPU::template Interrupt<DataTLBMissOnLoadInterrupt, 0x01100>
	{
		typedef typename SuperCPU::template Interrupt<DataTLBMissOnLoadInterrupt, 0x01100> Super;
		
		struct DataTLBMissOnLoad : SuperCPU::template Exception<DataTLBMissOnLoadInterrupt> { static const char *GetName() { return "Data TLB Miss-On-Load Interrupt/Data TLB Miss-On-Load Exception"; } };

		typedef typename SuperCPU::template ExceptionSet<DataTLBMissOnLoad> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		DataTLBMissOnLoadInterrupt(typename CONFIG::CPU *cpu) : Super(cpu), tlbmiss(0), ptehi(0), key(0) {}
		static const char *GetId() { return "data-tlb-miss-on-load-interrupt"; }
		static const char *GetName() { return "Data TLB Miss-On-Load Interrupt"; }
		DataTLBMissOnLoadInterrupt& SetTLBMISS(uint32_t v) { tlbmiss = v; return *this; }
		DataTLBMissOnLoadInterrupt& SetPTEHI(uint32_t v) { ptehi = v; return *this; }
		DataTLBMissOnLoadInterrupt& SetKey(unsigned int v) { key = v; return *this; }
		void Clear() { tlbmiss = 0; ptehi = 0; key = 0; }
		uint32_t GetTLBMISS() const { return tlbmiss; }
		uint32_t GetPTEHI() const { return ptehi; }
		unsigned int GetKey() const { return key; }
	private:
		uint32_t tlbmiss;
		uint32_t ptehi;
		unsigned int key;
	};
	
	struct DataTLBMissOnStoreInterrupt : SuperCPU::template Interrupt<DataTLBMissOnStoreInterrupt, 0x01200>
	{
		typedef typename SuperCPU::template Interrupt<DataTLBMissOnStoreInterrupt, 0x01200> Super;
		
		struct DataTLBMissOnStore : SuperCPU::template Exception<DataTLBMissOnStoreInterrupt> { static const char *GetName() { return "Data TLB Miss-On-Store Interrupt/Data TLB Miss-On-Store Exception"; } };

		typedef typename SuperCPU::template ExceptionSet<DataTLBMissOnStore> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		DataTLBMissOnStoreInterrupt(typename CONFIG::CPU *cpu) : Super(cpu), tlbmiss(0), ptehi(0), key(0), c(0) {}
		static const char *GetId() { return "data-tlb-miss-on-store-interrupt"; }
		static const char *GetName() { return "Data TLB Miss-On-Store Interrupt"; }
		DataTLBMissOnStoreInterrupt& SetTLBMISS(uint32_t v) { tlbmiss = v; return *this; }
		DataTLBMissOnStoreInterrupt& SetPTEHI(uint32_t v) { ptehi = v; return *this; }
		DataTLBMissOnStoreInterrupt& SetKey(unsigned int v) { key = v; return *this; }
		DataTLBMissOnStoreInterrupt& SetC(unsigned int v) { c = v; return *this; }
		void Clear() { tlbmiss = 0; ptehi = 0; key = 0; c = 0; }
		uint32_t GetTLBMISS() const { return tlbmiss; }
		uint32_t GetPTEHI() const { return ptehi; }
		unsigned int GetKey() const { return key; }
		unsigned int GetC() const { return c; }
	private:
		uint32_t tlbmiss;
		uint32_t ptehi;
		unsigned int key;
		unsigned int c;
	};
	
	struct InstructionAddressBreakpointInterrupt : SuperCPU::template Interrupt<InstructionAddressBreakpointInterrupt, 0x01300>
	{
		typedef typename SuperCPU::template Interrupt<InstructionAddressBreakpointInterrupt, 0x01300> Super;
		
		struct InstructionAddressBreakpoint : SuperCPU::template Exception<InstructionAddressBreakpointInterrupt> { static const char *GetName() { return "Instruction Address Breakpoint Interrupt/Instruction Address Breakpoint Exception"; } };

		typedef typename SuperCPU::template ExceptionSet<InstructionAddressBreakpoint> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		InstructionAddressBreakpointInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "instruction-address-breakpoint-interrupt"; }
		static const char *GetName() { return "Instruction Address Breakpoint Interrupt"; }
	};
	
	struct SystemManagementInterrupt : SuperCPU::template Interrupt<SystemManagementInterrupt, 0x01400>
	{
		typedef typename SuperCPU::template Interrupt<SystemManagementInterrupt, 0x01400> Super;
		
		struct SMI : SuperCPU::template Exception<SystemManagementInterrupt> { static const char *GetName() { return "System Management Interrupt/SMI Exception"; } };

		typedef typename SuperCPU::template ExceptionSet<SMI> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		SystemManagementInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "system-management-interrupt"; }
		static const char *GetName() { return "System Management Interrupt"; }
	};
	
	struct AltivecAssistInterrupt : SuperCPU::template Interrupt<AltivecAssistInterrupt, 0x01600>
	{
		typedef typename SuperCPU::template Interrupt<AltivecAssistInterrupt, 0x01600> Super;
		
		struct AltivecAssist : SuperCPU::template Exception<AltivecAssistInterrupt> { static const char *GetName() { return "Altivec Assist Interrupt/Altivec Assist Exception"; } };

		typedef typename SuperCPU::template ExceptionSet<AltivecAssist> ALL;
		
		typedef FieldSet< typename MSR::POW
		                , typename MSR::EE
		                , typename MSR::PR
		                , typename MSR::FP
		                , typename MSR::FE0
		                , typename MSR::SE
		                , typename MSR::BE
		                , typename MSR::FE1
		                , typename MSR::IR
		                , typename MSR::DR
		                , typename MSR::IR > MSR_ALWAYS_CLEARED_FIELDS;
		
		AltivecAssistInterrupt(typename CONFIG::CPU *cpu) : Super(cpu) {}
		static const char *GetId() { return "altivec-assist-interrupt"; }
		static const char *GetName() { return "Altivec Assist Interrupt"; }
	};
	
	typedef typename SuperCPU::template ExceptionPrioritySet<
		typename SystemResetInterrupt                 ::HardReset                   ,
		typename SystemResetInterrupt                 ::SoftReset                   ,
		typename MachineCheckInterrupt                ::TEA                         ,
		typename MachineCheckInterrupt                ::MCP                         ,
		typename SystemManagementInterrupt            ::SMI                         ,
		typename ExternalInterrupt                    ::ExternalInterruptException  ,
		typename PerformanceMonitorInterrupt          ::PerformanceMonitor          ,
		typename DecrementerInterrupt                 ::Decrementer                 ,
		typename InstructionStorageInterrupt          ::NoExecute                   ,
		typename InstructionStorageInterrupt          ::DirectStore                 ,
		typename InstructionTLBMissInterrupt          ::InstructionTLBMiss          ,
		typename InstructionStorageInterrupt          ::PageFault                   ,
		typename InstructionStorageInterrupt          ::GuardedMemory               ,
		typename InstructionStorageInterrupt          ::ProtectionViolation         ,
		typename InstructionAddressBreakpointInterrupt::InstructionAddressBreakpoint, 
		typename ProgramInterrupt                     ::IllegalInstruction          ,         
		typename ProgramInterrupt                     ::PrivilegeViolation          ,         
		typename ProgramInterrupt                     ::Trap                        ,         
		typename ProgramInterrupt                     ::UnimplementedInstruction    ,         
		typename SystemCallInterrupt                  ::SystemCall                  ,         
		typename FloatingPointUnavailableInterrupt    ::FloatingPointUnavailable    ,
		typename AltivecUnavailableInterrupt          ::AltivecUnavailable          ,
		typename ProgramInterrupt                     ::FloatingPoint               ,         
		typename AlignmentInterrupt                   ::Alignment                   ,
		typename DataStorageInterrupt                 ::ExternalAccessDisabled      ,
		typename DataStorageInterrupt                 ::DirectStore                 ,
		typename DataTLBMissOnStoreInterrupt          ::DataTLBMissOnStore          ,
		typename DataTLBMissOnLoadInterrupt           ::DataTLBMissOnLoad           ,
		typename DataStorageInterrupt                 ::ProtectionViolation         ,
		typename DataStorageInterrupt                 ::PageFault                   ,
		typename DataStorageInterrupt                 ::DataAddressBreakpoint       ,
		typename DataStorageInterrupt                 ::WriteThroughLinkedLoadStore ,
		typename AltivecAssistInterrupt               ::AltivecAssist               ,
		typename TraceInterrupt                       ::Trace 
	> EXCEPTION_PRIORITIES;

	void ProcessInterrupt(SystemResetInterrupt *); 
	void ProcessInterrupt(MachineCheckInterrupt *);
	void ProcessInterrupt(DataStorageInterrupt *); 
	void ProcessInterrupt(InstructionStorageInterrupt *);
	void ProcessInterrupt(ExternalInterrupt *); 
	void ProcessInterrupt(AlignmentInterrupt *);
	void ProcessInterrupt(ProgramInterrupt *); 
	void ProcessInterrupt(FloatingPointUnavailableInterrupt *);
	void ProcessInterrupt(DecrementerInterrupt *);
	void ProcessInterrupt(SystemCallInterrupt *);
	void ProcessInterrupt(TraceInterrupt *); 
	void ProcessInterrupt(PerformanceMonitorInterrupt *);
	void ProcessInterrupt(AltivecUnavailableInterrupt *);
	void ProcessInterrupt(InstructionTLBMissInterrupt *);
	void ProcessInterrupt(DataTLBMissOnLoadInterrupt *); 
	void ProcessInterrupt(DataTLBMissOnStoreInterrupt *); 
	void ProcessInterrupt(InstructionAddressBreakpointInterrupt *);
	void ProcessInterrupt(SystemManagementInterrupt *);
	void ProcessInterrupt(AltivecAssistInterrupt *); 
	
	void UpdateExceptionEnable();
	void UpdatePowerSave();
	void PowerManagement();
	
	bool IsStorageCacheable(STORAGE_ATTR storage_attr) const { return !(storage_attr & TYPES::SA_I); }
	bool IsDataStoreAccessWriteThrough(STORAGE_ATTR storage_attr) const { return (storage_attr & TYPES::SA_W) != 0; }

	virtual void InvalidateDirectMemPtr(PHYSICAL_ADDRESS start_addr, PHYSICAL_ADDRESS end_addr) {}

	void RunTimers(uint64_t delta);
	uint64_t GetMaxIdleTime() const;
	uint64_t GetTimersDeadline() const;
	
	template <typename T> EFFECTIVE_ADDRESS MungEffectiveAddress(EFFECTIVE_ADDRESS ea) { return (msr.template Get<typename MSR::LE>()) ? (ea ^ (8 - sizeof(T))) : ea; }
	
	bool CheckFp32LoadAlignment(EFFECTIVE_ADDRESS ea) { return (ea & 3) == 0; }
	bool CheckFp64LoadAlignment(EFFECTIVE_ADDRESS ea) { return (ea & 3) == 0; }
	bool CheckFp32StoreAlignment(EFFECTIVE_ADDRESS ea) { return (ea & 3) == 0; }
	bool CheckFp64StoreAlignment(EFFECTIVE_ADDRESS ea) { return (ea & 3) == 0; }
	bool CheckFpStoreLSWAlignment(EFFECTIVE_ADDRESS ea) { return (ea & 3) == 0; }

	int StringLength(EFFECTIVE_ADDRESS addr);
	std::string ReadString(EFFECTIVE_ADDRESS addr, unsigned int count = 0);

public:

	void FlushInstructionBuffer();
	bool InstructionFetch(EFFECTIVE_ADDRESS addr, typename CONFIG::CODE_TYPE& insn);
	void StepOneInstruction();
	
protected:
	////////////////////////// Run-time parameters ////////////////////////////
	
	uint8_t cpuid;
	unisim::kernel::variable::Parameter<uint8_t> param_cpuid;

	uint32_t processor_version;
	unisim::kernel::variable::Parameter<uint32_t> param_processor_version;
	
	////////////////////////// Instruction Buffer /////////////////////////////

	EFFECTIVE_ADDRESS instruction_buffer_base_addr;
	uint32_t instruction_buffer[CONFIG::INSTRUCTION_BUFFER_SIZE / 4];
	
	/////////////////////// VLE Instruction decoder ///////////////////////////
	
	typename CONFIG::DECODER decoder;
	typename CONFIG::OPERATION *operation;
	
	////////////////////////////// Interrupts /////////////////////////////////
	
	SystemResetInterrupt                  system_reset_interrupt;
	MachineCheckInterrupt                 machine_check_interrupt;
	DataStorageInterrupt                  data_storage_interrupt;
	InstructionStorageInterrupt           instruction_storage_interrupt;
	ExternalInterrupt                     external_interrupt;
	AlignmentInterrupt                    alignment_interrupt;
	ProgramInterrupt                      program_interrupt;
	FloatingPointUnavailableInterrupt     floating_point_unavailable_interrupt;
	DecrementerInterrupt                  decrementer_interrupt;
	SystemCallInterrupt                   system_call_interrupt;
	TraceInterrupt                        trace_interrupt;
	PerformanceMonitorInterrupt           performance_monitor_interrupt;
	AltivecUnavailableInterrupt           altivec_unavailable_interrupt;
	InstructionTLBMissInterrupt           instruction_tlb_miss_interrupt;
	DataTLBMissOnLoadInterrupt            data_tlb_miss_on_load_interrupt;
	DataTLBMissOnStoreInterrupt           data_tlb_miss_on_store_interrupt;
	InstructionAddressBreakpointInterrupt instruction_address_breakpoint_interrupt;
	SystemManagementInterrupt             system_management_interrupt;
	AltivecAssistInterrupt                altivec_assist_interrupt;
        
	//////////////////////////// Power Management /////////////////////////////
	
	bool powersave;
	
	/////////////////////////////// Registers /////////////////////////////////

	// VEA
	typename SuperCPU::TB tb;

	// OEA
	typename SuperCPU::BAMR        bamr;
	typename SuperCPU::DABR        dabr;
	typename SuperCPU::DAR         dar;
	DEC                            dec;
	DSISR                          dsisr;
	typename SuperCPU::EAR         ear;
	HID0                           hid0;
	HID1                           hid1;
	typename SuperCPU::IABR        iabr;
	typename SuperCPU::ICTC        ictc;
	ICTRL                          ictrl;
	L2CR                           l2cr;
	typename SuperCPU::LDSTCR      ldstcr;
	typename SuperCPU::LDSTDB      ldstdb;
	typename SuperCPU::MMCR0       mmcr0;
	typename SuperCPU::MMCR1       mmcr1;
	typename SuperCPU::MMCR2       mmcr2;
	MSR                            msr;
	typename SuperCPU::MSSCR0      msscr0;
	MSSSR0                         msssr0;
	typename SuperCPU::PIR         pir;
	typename SuperCPU::PMC1        pmc1;
	typename SuperCPU::PMC2        pmc2;
	typename SuperCPU::PMC3        pmc3;
	typename SuperCPU::PMC4        pmc4;
	typename SuperCPU::PMC5        pmc5;
	typename SuperCPU::PMC6        pmc6;
	typename SuperCPU::PTEHI       ptehi;
	typename SuperCPU::PTELO       ptelo;
	typename SuperCPU::PVR         pvr;
	typename SuperCPU::SIAR        siar;
	SPRGs                          sprgs;
	typename SuperCPU::SRR0        srr0;
	SRR1                           srr1;
	typename SuperCPU::TLBMISS     tlbmiss;
};

} // end of namespace e600
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E600_CPU_HH__
