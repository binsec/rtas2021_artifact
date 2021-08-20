/*
 *  Copyright (c) 2017,
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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_HH__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <stack>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace freescale {
namespace mpc57xx {
namespace intc {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

using unisim::util::reg::core::Register;
using unisim::util::reg::core::AddressableRegister;
using unisim::util::reg::core::AddressableRegisterHandle;
using unisim::util::reg::core::RegisterAddressMap;
using unisim::util::reg::core::FieldSet;
using unisim::util::reg::core::AddressableRegisterFile;
using unisim::util::reg::core::ReadWriteStatus;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_R;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::RWS_OK;
using unisim::util::reg::core::RWS_ANA;
using unisim::util::reg::core::Access;
using unisim::util::reg::core::IsReadWriteError;

template <typename REGISTER, typename FIELD, int OFFSET1, int OFFSET2 = -1, Access _ACCESS = SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? ((REGISTER::SIZE - 1) - OFFSET2) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : 0
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 0) : 1) : 0
                                             , _ACCESS>
{
};

#if 0
struct CONFIG
{
	static const unsigned int NUM_PROCESSORS = 4;
	static const unsigned int NUM_HW_IRQS = 992;
	static const unsigned int BUSWIDTH = 32;
	static const unsigned int VOFFSET_WIDTH = 14;
};
#endif

template <typename CONFIG>
class INTC
	: public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR                         = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR                         = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH                         = 0;
	static const unsigned int NUM_PROCESSORS                                = CONFIG::NUM_PROCESSORS;
	static const unsigned int NUM_SW_IRQS                                   = 32;
	static const unsigned int NUM_HW_IRQS                                   = CONFIG::NUM_HW_IRQS;
	static const unsigned int NUM_IRQS                                      = NUM_SW_IRQS + NUM_HW_IRQS;
	static const unsigned int NUM_PRIORITY_LEVELS                           = 64;
	static const unsigned int MAX_PRIORITY_LEVEL                            = NUM_PRIORITY_LEVELS - 1;
	static const unsigned int BUSWIDTH                                      = CONFIG::BUSWIDTH;
	static const unsigned int VOFFSET_WIDTH                                 = CONFIG::VOFFSET_WIDTH;
	static const unsigned int HW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES = 3; // in cycles
	static const unsigned int SW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES = 4; // in cycles
	static const bool threaded_model                                        = false;
	
	enum IRQ_Type
	{
		ANY_IRQ = 0,
		SW_IRQ  = 1,
		HW_IRQ  = 2
	};
	
	typedef tlm::tlm_target_socket<BUSWIDTH>        peripheral_slave_if_type;

	peripheral_slave_if_type                        peripheral_slave_if;        // Peripheral slave interface
	sc_core::sc_in<bool>                             m_clk;                     // clock port
	sc_core::sc_in<bool>                             reset_b;                   // reset
	sc_core::sc_in<bool>                            *hw_irq[NUM_HW_IRQS];       // Hardware IRQ sources
	sc_core::sc_in<bool>                            *p_iack[NUM_PROCESSORS];    // Interrupt Acknowledge
	sc_core::sc_out<bool>                           *p_irq_b[NUM_PROCESSORS];   // Processor Interrupt Request
	sc_core::sc_out<bool>                           *p_avec_b[NUM_PROCESSORS];  // Autovector
	sc_core::sc_out<sc_dt::sc_uint<VOFFSET_WIDTH> > *p_voffset[NUM_PROCESSORS]; // vector offset
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	INTC(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~INTC();
	
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);

	//////////////// unisim::service::interface::Registers ////////////////////
	
	virtual unisim::service::interfaces::Register *GetRegister(const char *name);
	virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner);

private:
	virtual void end_of_elaboration();
	
	unisim::kernel::logger::Logger logger;
	
	class Event
	{
	public:
		class Key
		{
		public:
			Key()
				: time_stamp(sc_core::SC_ZERO_TIME)
			{
			}
			
			Key(const sc_core::sc_time& _time_stamp)
				: time_stamp(_time_stamp)
			{
			}

			void SetTimeStamp(const sc_core::sc_time& _time_stamp)
			{
				time_stamp = _time_stamp;
			}

			const sc_core::sc_time& GetTimeStamp() const
			{
				return time_stamp;
			}

			void Clear()
			{
				time_stamp = sc_core::SC_ZERO_TIME;
			}
			
			int operator < (const Key& sk) const
			{
				return time_stamp < sk.time_stamp;
			}
			
		private:
			sc_core::sc_time time_stamp;
		};
		
		Event()
			: key()
			, prc_num(0)
			, payload(0)
			, release_payload(false)
			, completion_event(0)
		{
		}
		
		~Event()
		{
			Clear();
		}
		
		void Clear()
		{
			key.Clear();
			prc_num = 0;
			if(release_payload)
			{
				if(payload && payload->has_mm()) payload->release();
			}
			payload = 0;
			release_payload = false;
			completion_event  = 0;
		}
		
		void SetTimeStamp(const sc_core::sc_time& _time_stamp)
		{
			key.SetTimeStamp(_time_stamp);
		}
		
		void SetProcessorNumber(unsigned int _prc_num)
		{
			prc_num = _prc_num;
		}
		
		void SetPayload(tlm::tlm_generic_payload *_payload, bool _release_payload = false)
		{
			if(release_payload)
			{
				if(payload && payload->has_mm()) payload->release();
			}
			payload = _payload;
			release_payload = _release_payload;
		}
		
		void SetCompletionEvent(sc_core::sc_event *_completion_event)
		{
			completion_event = _completion_event;
		}

		const sc_core::sc_time& GetTimeStamp() const
		{
			return key.GetTimeStamp();
		}
		
		unsigned int GetProcessorNumber() const
		{
			return prc_num;
		}

		tlm::tlm_generic_payload *GetPayload() const
		{
			return payload;
		}

		sc_core::sc_event *GetCompletionEvent() const
		{
			return completion_event;
		}

		const Key& GetKey() const
		{
			return key;
		}
		
	private:
		Key key;                             // schedule key (i.e. time stamp)
		unsigned int prc_num;                // processor number
		tlm::tlm_generic_payload *payload;   // payload
		bool release_payload;                // whether payload must be released using payload memory management
		sc_core::sc_event *completion_event; // completion event (for blocking transport interface)
	};

	typedef unsigned int ProcessorNumber;
	
	// INTC Register
	template <typename REGISTER, unsigned int _SIZE, Access _ACCESS>
	struct INTC_Register : AddressableRegister<REGISTER, _SIZE, _ACCESS, ProcessorNumber>
	{
		typedef AddressableRegister<REGISTER, _SIZE, _ACCESS, ProcessorNumber> Super;
		
		INTC_Register(INTC<CONFIG> *_intc) : Super(), intc(_intc) {}
		INTC_Register(INTC<CONFIG> *_intc, uint32_t value) : Super(value), intc(_intc) {}
		
		inline bool IsVerboseRead() const ALWAYS_INLINE { return intc->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return intc->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return intc->logger.DebugInfoStream(); }
		
		using Super::operator =;
	protected:
		INTC<CONFIG> *intc;
	};
	
	// INTC Block Configuration Register (INTC_BCR)
	struct INTC_BCR : INTC_Register<INTC_BCR, 32, SW_RW>
	{
		typedef INTC_Register<INTC_BCR, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct HVEN3 : Field<INTC_BCR, HVEN3, 19>    {}; // Hardware vector enable for processor 3
		struct HVEN2 : Field<INTC_BCR, HVEN2, 23>    {}; // Hardware vector enable for processor 2
		struct HVEN1 : Field<INTC_BCR, HVEN1, 27>    {}; // Hardware vector enable for processor 1
		struct HVEN0 : Field<INTC_BCR, HVEN0, 31>    {}; // Hardware vector enable for processor 0
		
		SWITCH_ENUM_TRAIT(unsigned int, _);
		CASE_ENUM_TRAIT(1, _) { typedef FieldSet<HVEN0> ALL; };
		CASE_ENUM_TRAIT(2, _) { typedef FieldSet<HVEN1, HVEN0> ALL; };
		CASE_ENUM_TRAIT(3, _) { typedef FieldSet<HVEN2, HVEN1, HVEN0> ALL; };
		CASE_ENUM_TRAIT(4, _) { typedef FieldSet<HVEN3, HVEN2, HVEN1, HVEN0> ALL; };
		typedef typename ENUM_TRAIT(NUM_PROCESSORS, _)::ALL ALL;
		
		INTC_BCR(INTC<CONFIG> *_intc) : Super(_intc) { Init(); }
		INTC_BCR(INTC<CONFIG> *_intc, uint32_t value) : Super(_intc, value) { Init(); }
		
		void Init()
		{
			this->SetName("INTC_BCR"); this->SetDescription("INTC Block Configuration Register");
			HVEN3::SetName("HVEN3"); HVEN3::SetDescription("Hardware vector enable for processor 3");
			HVEN2::SetName("HVEN2"); HVEN2::SetDescription("Hardware vector enable for processor 2");
			HVEN1::SetName("HVEN1"); HVEN1::SetDescription("Hardware vector enable for processor 1");
			HVEN0::SetName("HVEN0"); HVEN0::SetDescription("Hardware vector enable for processor 0");
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!this->intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_BCR
				this->intc->logger << DebugWarning << "CPU #" << writter_prc_num << " is not allowed to write " << this->GetName() << EndDebugWarning;
				return RWS_ANA;
			}
			
			return Super::Write(writter_prc_num, value, bit_enable);
		}
		
		using Super::operator =;
	};
	
	// INTC Master Protection Register (INTC_MPROT)
	struct INTC_MPROT : INTC_Register<INTC_MPROT, 32, SW_RW>
	{
		typedef INTC_Register<INTC_MPROT, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x4;

		struct ID    : Field<INTC_MPROT, ID, 26, 27> {}; // ID
		struct MPROT : Field<INTC_MPROT, MPROT, 31>  {}; // Master Protection
		
		INTC_MPROT(INTC<CONFIG> *_intc) : Super(_intc) { Init(); }
		INTC_MPROT(INTC<CONFIG> *_intc, uint32_t value) : Super(_intc, value) { Init(); }
		
		typedef FieldSet<ID, MPROT> ALL;
		
		void Init()
		{
			this->SetName("INTC_MPROT"); this->SetDescription("INTC Master Protection Register");
			ID   ::SetName("ID");    ID   ::SetDescription("ID");
			MPROT::SetName("MPROT"); MPROT::SetDescription("Master Protection");
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!this->intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_MPROT
				this->intc->logger << DebugWarning << "CPU #" << writter_prc_num << " is not allowed to write " << this->GetName() << EndDebugWarning;
				return RWS_ANA;
			}
			
			return Super::Write(writter_prc_num, value, bit_enable);
		}

		using Super::operator =;
	};
	
	// INTC Current Priority Register for Processor 0-3 (INTC_CPR0-3)
	struct INTC_CPR : INTC_Register<INTC_CPR, 32, SW_RW>
	{
		typedef INTC_Register<INTC_CPR, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x10;

		struct PRI : Field<INTC_CPR, PRI, 26, 31> {}; // Priority of the currently executing ISR
		
		typedef FieldSet<PRI> ALL;
		
		INTC_CPR() : Super(0), reg_num(0) {}
		INTC_CPR(INTC<CONFIG> *_intc) : Super(_intc), reg_num(0) {}
		INTC_CPR(INTC<CONFIG> *_intc, uint32_t value) : Super(_intc, value), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			this->intc = _intc;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_CPR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC Current Priority Register for Processor " << reg_num;
			this->SetDescription(description_sstr.str());
			
			PRI::SetName("PRI");
			PRI::SetDescription("Priority of the currently executing ISR");
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!this->intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_CPR
				this->intc->logger << DebugWarning << "CPU #" << writter_prc_num << " is not allowed to write " << this->GetName() << EndDebugWarning;
				return RWS_ANA;
			}
			
			ReadWriteStatus rws = Super::Write(writter_prc_num, value, bit_enable);
			
			// CPRn change may affect IRQ input selection for processor n
			unsigned int prc_num = reg_num;
			this->intc->UpdateIRQSelect(prc_num);
			
			return rws;
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// INTC Interrupt Acknowledge Register for Processor 0-3 (INTC_IACKR0-3)
	struct INTC_IACK : INTC_Register<INTC_IACK, 32, SW_RW>
	{
		typedef INTC_Register<INTC_IACK, 32, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x20;

		struct VTBA   : Field<INTC_IACK, VTBA, 0, 19>    {};
		struct INTVEC : Field<INTC_IACK, INTVEC, 20, 29, SW_R> {};
		
		typedef FieldSet<VTBA, INTVEC> ALL;
		
		INTC_IACK() : Super(0), reg_num(0) {}
		INTC_IACK(INTC<CONFIG> *_intc) : Super(_intc), reg_num(0) {}
		INTC_IACK(INTC<CONFIG> *_intc, uint32_t value) : Super(_intc, value), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			this->intc = _intc;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_IACK" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC Interrupt Acknowledge Register for Processor " << reg_num;
			this->SetDescription(description_sstr.str());
			
			VTBA  ::SetName("VTBA");   VTBA  ::SetDescription("Vector table base address");
			INTVEC::SetName("INTVEC"); INTVEC::SetDescription("Interrupt vector");
		}

		virtual ReadWriteStatus Read(ProcessorNumber& reader_prc_num, uint32_t& value, const uint32_t& bit_enable)
		{
			if(!this->intc->CheckMasterProtection(reader_prc_num))
			{
				// CPU is not allowed to read INTC_IACK
				this->intc->logger << DebugWarning << "CPU #" << reader_prc_num << " is not allowed to read " << this->GetName() << EndDebugWarning;
				return RWS_ANA;
			}
			
			ReadWriteStatus rws = Super::Read(reader_prc_num, value, bit_enable);
			
			if(IsReadWriteError(rws)) return rws;
			
			unsigned prc_num = reg_num;
			// Check vector mode
			if(!this->intc->IsHardwareVectorEnabled(prc_num))
			{
				// We're in software vector mode (aka. autovectored mode)
				
				// Acknowledge IRQ internally
				this->intc->IRQAcknowledge(prc_num);
				
				// Reading the INTC_IACKRn negates the interrupt request to the associated processor.
				//this->intc->SetIRQOutput(prc_num, false); // To be removed: already done by IRQAcknowledge
			}
			
			return rws;
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!this->intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_IACK
				this->intc->logger << DebugWarning << "CPU #" << writter_prc_num << " is not allowed to write " << this->GetName() << EndDebugWarning;
				return RWS_ANA;
			}
			
			return Super::Write(writter_prc_num, value, bit_enable);
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// INTC End Of Interrupt Register for Processor 0-3 (INTC_EOIR0-3)
	struct INTC_EOIR : INTC_Register<INTC_EOIR, 32, SW_W>
	{
		typedef INTC_Register<INTC_EOIR, 32, SW_W> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x30;

		struct EOI : Field<INTC_EOIR, EOI, 0, 31> {}; // End of Interrupt
		
		typedef FieldSet<EOI> ALL;
		
		INTC_EOIR() : Super(0), reg_num(0) {}
		INTC_EOIR(INTC<CONFIG> *_intc) : Super(_intc), reg_num(0) {}
		INTC_EOIR(INTC<CONFIG> *_intc, uint32_t value) : Super(_intc, value), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			this->intc = _intc;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_EOIR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC End Of Interrupt Register for Processor " << reg_num;
			this->SetDescription(description_sstr.str());
			
			EOI::SetName("EOI");
			EOI::SetDescription("End of Interrupt");
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint32_t& value, const uint32_t& bit_enable)
		{
			if(!this->intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_EOIR
				this->intc->logger << DebugWarning << "CPU #" << writter_prc_num << " is not allowed to write " << this->GetName() << EndDebugWarning;
				return RWS_ANA;
			}
			
			// Writing four zeroes ?
// 			if((value == 0) && (bit_enable == ~uint32_t(0)))
// 			{
				unsigned int prc_num = reg_num;
				this->intc->EndOfInterrupt(prc_num);
//			}
			
			// INTC_EOIR is never updated
			return RWS_OK;
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// INTC Software Set/Clear Interrupt Register 0-31 (INTC_SSCIR0-31)
	struct INTC_SSCIR : INTC_Register<INTC_SSCIR, 8, SW_RW>
	{
		typedef INTC_Register<INTC_SSCIR, 8, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x40;

		struct SET : Field<INTC_SSCIR, SET, 6> {}; // Set flag bits
		struct CLR : Field<INTC_SSCIR, CLR, 7> {}; // Clear flag bits; CLR is the flag bit
		
		typedef FieldSet<SET, CLR> ALL;
		
		INTC_SSCIR() : Super(0), reg_num(0) {}
		INTC_SSCIR(INTC<CONFIG> *_intc) : Super(_intc), reg_num(0) {}
		INTC_SSCIR(INTC<CONFIG> *_intc, uint8_t value) : Super(_intc, value), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			this->intc = _intc;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_SSCIR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC Software Set/Clear Interrupt Register " << reg_num;
			this->SetDescription(description_sstr.str());
			
			SET::SetName("SET"); SET::SetDescription("Set flag bits");
			CLR::SetName("CLR"); CLR::SetDescription("Clear flag bits; CLR is the flag bit");
		}
		
		void Effect()
		{
			if(this->template Get<typename INTC_SSCIR::SET>())
			{
				// Writing a 1 to SET will leave SET unchanged at 0 but will set CLR
				// If a 1 is written to a pair of SET and CLR bits at the same time,
				// CLR is asserted, regardless of whether CLR was asserted before the write
				this->template Set<typename INTC_SSCIR::CLR>(1);
				this->template Set<typename INTC_SSCIR::SET>(0);
			}
			else if(this->template Get<typename INTC_SSCIR::CLR>())
			{
				// Writing a 1 to CLR will clear it
				this->template Set<typename INTC_SSCIR::CLR>(0);
			}
			
			// CLR is the flag bit
			unsigned int sw_irq_num = reg_num;
			// Update latched IRQ input status
			this->intc->SetIRQInputStatus(this->intc->SW_IRQ2IRQ(sw_irq_num), this->template Get<typename INTC_SSCIR::CLR>() != 0);
		}
		
		INTC_SSCIR& operator = (const uint8_t& value)
		{
			Super::operator = (value);
			Effect();
			return *this;
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint8_t& value, const uint8_t& bit_enable)
		{
			unsigned int sw_irq_num = reg_num;
			
			if(INTC_SSCIR::CLR::Get(value) && !this->intc->CheckWriteProtection_SSCIR_CLR(writter_prc_num, sw_irq_num))
			{
				// CPU is not allowed to write 1 into CLR field
				this->intc->logger << DebugWarning << "CPU #" << writter_prc_num << " is not allowed to write 1 into " << this->GetName() << "[" << INTC_SSCIR::CLR::GetName() << "]" << EndDebugWarning;
				return RWS_ANA;
			}
			
			ReadWriteStatus rws = Super::Write(writter_prc_num, value, bit_enable);

			Effect();
			
			return rws;
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// INTC Priority Select Register 0-1023 (INTC_PSR0-1023)
	template <IRQ_Type IRQ_TYPE>
	struct INTC_PSR : INTC_Register<INTC_PSR<IRQ_TYPE>, 16, SW_RW>
	{
		typedef INTC_Register<INTC_PSR<IRQ_TYPE>, 16, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = (IRQ_TYPE == HW_IRQ) ? (0x60 + (NUM_SW_IRQS * 2)) : 0x60;

		struct PRC_SELN0 : Field<INTC_PSR, PRC_SELN0, 0>   {};  // Processor select 0
		struct PRC_SELN1 : Field<INTC_PSR, PRC_SELN1, 1>   {};  // Processor select 1
		struct PRC_SELN2 : Field<INTC_PSR, PRC_SELN2, 2>   {};  // Processor select 2
		struct PRC_SELN3 : Field<INTC_PSR, PRC_SELN3, 3>   {};  // Processor select 3
		struct PRC_SELN  : Field<INTC_PSR, PRC_SELN, 0, 3> {};  // Processor Select mask
		struct SWTN      : Field<INTC_PSR, SWTN, 7>        {};  // Software trigger
		struct PRIN      : Field<INTC_PSR, PRIN, 10, 15>   {};  // Priority Select
		
		// Note: Actual PRC_SELN bits depends on the effective number of processors in the system
		SWITCH_ENUM_TRAIT(unsigned int, _N);
		CASE_ENUM_TRAIT(1, _N) { typedef FieldSet<PRC_SELN0, PRIN> ALL; };
		CASE_ENUM_TRAIT(2, _N) { typedef FieldSet<PRC_SELN0, PRC_SELN1, PRIN> ALL; };
		CASE_ENUM_TRAIT(3, _N) { typedef FieldSet<PRC_SELN0, PRC_SELN1, PRC_SELN2, PRIN> ALL; };
		CASE_ENUM_TRAIT(4, _N) { typedef FieldSet<PRC_SELN0, PRC_SELN1, PRC_SELN2, PRC_SELN3, PRIN> ALL; };

		// Note: SWTN is only available in PSRs that do not correspond to the SSCIRs (aka. first PSR registers)
		SWITCH_ENUM_TRAIT(IRQ_Type, _);
		CASE_ENUM_TRAIT(SW_IRQ, _) { typedef typename ENUM_TRAIT(NUM_PROCESSORS, _N)::ALL ALL; };
		CASE_ENUM_TRAIT(HW_IRQ, _) { typedef FieldSet<typename ENUM_TRAIT(NUM_PROCESSORS, _N)::ALL, SWTN> ALL; };

		typedef typename ENUM_TRAIT(IRQ_TYPE, _)::ALL ALL;
		
		INTC_PSR() : Super(0), reg_num(0) {}
		INTC_PSR(INTC<CONFIG> *_intc) : Super(_intc), reg_num(0) {}
		INTC_PSR(INTC<CONFIG> *_intc, uint8_t value) : Super(_intc, value), reg_num(0) {}
		void WithinRegisterFileCtor(unsigned int _reg_num, INTC<CONFIG> *_intc)
		{
			this->intc = _intc;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "INTC_PSR" << ((IRQ_TYPE == HW_IRQ)  ? (NUM_SW_IRQS + reg_num) : reg_num);
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "INTC Priority Select Register " << reg_num;
			this->SetDescription(description_sstr.str());
			
			PRC_SELN0::SetName("PRC_SELN0"); PRC_SELN0::SetDescription("Processor select 0");
			PRC_SELN1::SetName("PRC_SELN1"); PRC_SELN1::SetDescription("Processor select 1");
			PRC_SELN2::SetName("PRC_SELN2"); PRC_SELN2::SetDescription("Processor select 2");
			PRC_SELN3::SetName("PRC_SELN3"); PRC_SELN3::SetDescription("Processor select 3");
			PRC_SELN3::SetName("PRC_SELN");  PRC_SELN ::SetDescription("Processor select mask");
			SWTN     ::SetName("SWTN");      SWTN     ::SetDescription("Software trigger");
			PRIN     ::SetName("PRIN");      PRIN     ::SetDescription("Priority Select");
		}
		
		void InitializePriorityTree()
		{
			unsigned int hw_sw_irq_num = reg_num;
			unsigned int irq_num = (IRQ_TYPE == HW_IRQ) ? this->intc->HW_IRQ2IRQ(hw_sw_irq_num) : this->intc->SW_IRQ2IRQ(hw_sw_irq_num);
			
			unsigned int priority = this->template Get<typename INTC_PSR::PRIN>();
			unsigned int prc_seln = this->template Get<typename INTC_PSR::PRC_SELN>();
			
			unsigned int prc_num;
			for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
			{
				bool sel = (8 >> prc_num) & prc_seln;
				
				if(sel)
				{
					// CPU is selected
					this->intc->EnableIRQInput(prc_num, irq_num, priority);
				}
			}
		}
		
		virtual ReadWriteStatus Write(ProcessorNumber& writter_prc_num, const uint16_t& value, const uint16_t& bit_enable)
		{
			if(!this->intc->CheckMasterProtection(writter_prc_num))
			{
				// CPU is not allowed to write INTC_PSR
				this->intc->logger << DebugWarning << "CPU #" << writter_prc_num << " is not allowed to write " << this->GetName() << EndDebugWarning;
				return RWS_ANA;
			}
			
			// Detect change of processor selection and IRQ priority in INTC_PSRn
			// Mirror changes in priority tree
			unsigned int old_priority = this->template Get<typename INTC_PSR::PRIN>();
			unsigned int old_prc_seln = this->template Get<typename INTC_PSR::PRC_SELN>();
			unsigned int hw_sw_irq_num = reg_num;
			unsigned int irq_num = (IRQ_TYPE == HW_IRQ) ? this->intc->HW_IRQ2IRQ(hw_sw_irq_num) : this->intc->SW_IRQ2IRQ(hw_sw_irq_num);
			
			ReadWriteStatus rws = Super::Write(writter_prc_num, value, bit_enable);
			
			unsigned int new_priority = this->template Get<typename INTC_PSR::PRIN>();
			unsigned int new_prc_seln = this->template Get<typename INTC_PSR::PRC_SELN>();
			
			unsigned int prc_num;
			for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
			{
				bool old_sel = (8 >> prc_num) & old_prc_seln;
				bool new_sel = (8 >> prc_num) & new_prc_seln;
				
				if(new_sel)
				{
					// CPU is selected
					if(old_sel)
					{
						// CPU was already selected
						if(old_priority != new_priority)
						{
							// IRQ priority changed
							this->intc->ChangeIRQInputPriority(prc_num, irq_num, old_priority, new_priority);
						}
					}
					else
					{
						// CPU gets selected
						this->intc->EnableIRQInput(prc_num, irq_num, new_priority);
					}
				}
				else
				{
					if(old_sel)
					{
						// CPU gets deselected
						this->intc->DisableIRQInput(prc_num, irq_num, old_priority);
					}
					else
					{
						// CPU was already deselected
					}
				}
			}
			
			if(IRQ_TYPE == HW_IRQ)
			{
				if(this->template Get<typename INTC_PSR::SWTN>())
				{
					this->intc->SetIRQInputStatus(irq_num, true);
				}
			}
			
			return rws;
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy; // proxy to get clock properties from clock port
	
	// INTC registers
	INTC_BCR                                                                              intc_bcr;    // INTC_BCR
	INTC_MPROT                                                                            intc_mprot;  // INTC_MPROT
	AddressableRegisterFile<INTC_CPR, NUM_PROCESSORS, INTC<CONFIG>, ProcessorNumber>      intc_cpr;    // INTC_CPRn
	AddressableRegisterFile<INTC_IACK, NUM_PROCESSORS, INTC<CONFIG>, ProcessorNumber>     intc_iack;   // INTC_IACKn
	AddressableRegisterFile<INTC_EOIR, NUM_PROCESSORS, INTC<CONFIG>, ProcessorNumber>     intc_eoir;   // INTC_EOIRn
	AddressableRegisterFile<INTC_SSCIR, NUM_SW_IRQS, INTC<CONFIG>, ProcessorNumber>       intc_sscir;  // INTC_SSCIRn
	AddressableRegisterFile<INTC_PSR<SW_IRQ>, NUM_SW_IRQS, INTC<CONFIG>, ProcessorNumber> intc_psr_sw; // INTC_PSRn (for software interrupt sources)
	AddressableRegisterFile<INTC_PSR<HW_IRQ>, NUM_HW_IRQS, INTC<CONFIG>, ProcessorNumber> intc_psr_hw; // INTC_PSRn (for hardware interrupt sources)
	
	// LIFO (stack of priority levels)
	std::stack<unsigned int> lifo[NUM_PROCESSORS];

	// Latched IRQ input status
	uint64_t irqs[NUM_IRQS / 64];                         
	
	// Priority tree (mirror of INTC_PSRn)
	class IRQPriority
	{
	public:
		IRQPriority() : key(0) {}
		IRQPriority(unsigned int irq_num, unsigned int priority) : key(((MAX_PRIORITY_LEVEL - priority) * NUM_IRQS) + irq_num) {}
		
		int operator < (const IRQPriority& ip) const { return key < ip.key; }
		
		unsigned int GetIRQ() const { return key % NUM_IRQS; }
		unsigned int GetPriority() const { return MAX_PRIORITY_LEVEL - (key / NUM_IRQS); }
	private:
		unsigned int key;
	};

	std::set<IRQPriority> priority_tree[NUM_PROCESSORS];
	
	// INTC registers address map
	RegisterAddressMap<sc_dt::uint64, ProcessorNumber> reg_addr_map;
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	unisim::kernel::tlm2::Schedule<Event> schedule;         // Payload (processor requests over AHB interface) schedule
	sc_core::sc_time last_irq_b_time_stamp[NUM_PROCESSORS]; // Time stamp when last IRQ output was negated
	sc_core::sc_event *irq_select_event[NUM_PROCESSORS];    // Event to trigger IRQ input selection (IRQ_Select_Process)
	sc_core::sc_event *gen_irq_b_event[NUM_PROCESSORS];     // Event to trigger IRQ output (IRQ_B_Process)
	unsigned int selected_irq_num[NUM_PROCESSORS];          // Vector of last selected IRQ input
	bool irq_b[NUM_PROCESSORS];                             // IRQ output status to output for each processor
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	
	sc_core::sc_time cycle_time;                               // cycle time
	sc_core::sc_time hw_irq_source_to_processor_irq_latency;   // latency between hardware IRQ source to processor IRQ
	sc_core::sc_time sw_irq_source_to_processor_irq_latency;   // latency between software IRQ source to processor IRQ
	
	void Reset();
	void UpdateIRQSelect(unsigned int prc_num, const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	void SetIRQOutput(unsigned int prc_num, bool value, IRQ_Type irq_type = ANY_IRQ);
	void SetIRQInputStatus(unsigned int irq_num, bool value);
	bool GetIRQInputStatus(unsigned int irq_num) const;
	void EnableIRQInput(unsigned int prc_num, unsigned int irq_num, unsigned int new_priority);
	void DisableIRQInput(unsigned int prc_num, unsigned int irq_num, unsigned int old_priority);
	void ChangeIRQInputPriority(unsigned int prc_num, unsigned int irq_num, unsigned int old_priority, unsigned int new_priority);
	bool SelectIRQInput(unsigned int prc_num, unsigned int& irq_num) const;
	void IRQAcknowledge(unsigned int prc_num);
	void EndOfInterrupt(unsigned int prc_num);

	bool CheckMasterProtection(unsigned int prc_num) const;
	bool CheckWriteProtection_SSCIR_CLR(unsigned int prc_num, unsigned int sw_irq_num) const;
	bool IsHardwareVectorEnabled(unsigned int prc_num) const;
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	unsigned int SW_IRQ2IRQ(unsigned int sw_irq_num) const;
	unsigned int HW_IRQ2IRQ(unsigned int hw_irq_num) const;
	unsigned int IRQ2SW_IRQ(unsigned int irq_num) const;
	unsigned int IRQ2HW_IRQ(unsigned int irq_num) const;
	bool IsSW_IRQ(unsigned int irq_num) const;
	bool IsHW_IRQ(unsigned int irq_num) const;
	void HW_IRQ_Process(unsigned int hw_irq_num);
	void IRQ_B_Process(unsigned int prc_num);
	void IACK_Process(unsigned int prc_num);
	void IRQ_Select_Process(unsigned int prc_num);
	void UpdateSpeed();
	void ClockPropertiesChangedProcess();
	void RESET_B_Process();
	void DumpPriorityTree(unsigned int prc_num);
};

} // end of namespace intc
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_HH__
