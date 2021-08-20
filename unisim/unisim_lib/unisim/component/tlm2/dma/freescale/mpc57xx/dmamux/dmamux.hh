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

#ifndef __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_DMAMUX_DMAMUX_HH__
#define __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_DMAMUX_DMAMUX_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/debug/simple_register_registry.hh>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace dma {
namespace freescale {
namespace mpc57xx {
namespace dmamux {

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
using unisim::util::reg::core::IsReadWriteError;
using unisim::util::reg::core::Access;
using unisim::util::reg::core::SW_RW;
using unisim::util::reg::core::SW_R;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::SW_R_W1C;
using unisim::util::reg::core::RWS_OK;
using unisim::util::reg::core::RWS_ANA;

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
	static const unsigned int NUM_DMA_CHANNELS = 16;
	static const unsigned int NUM_DMA_SOURCES = 16;
	static const unsigned int NUM_DMA_ALWAYS = 4;
	static const unsigned int NUM_DMA_TRIGGERS = 8;
	static const unsigned int BUSWIDTH = 32;
};
#endif

template <typename CONFIG>
class DMAMUX
	: public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH = 0;
	static const unsigned int NUM_DMA_CHANNELS      = CONFIG::NUM_DMA_CHANNELS;
	static const unsigned int NUM_DMA_SOURCES       = CONFIG::NUM_DMA_SOURCES;
	static const unsigned int NUM_DMA_ALWAYS_ON     = CONFIG::NUM_DMA_ALWAYS_ON;
	static const unsigned int NUM_DMA_TRIGGERS      = CONFIG::NUM_DMA_TRIGGERS;
	static const unsigned int BUSWIDTH              = CONFIG::BUSWIDTH;
	static const bool threaded_model                = false;
	
	typedef tlm::tlm_target_socket<BUSWIDTH> peripheral_slave_if_type;

	peripheral_slave_if_type  peripheral_slave_if;              // peripheral slave interface
	sc_core::sc_in<bool>      m_clk;                            // Clock port
	sc_core::sc_in<bool>      reset_b;                          // reset
	sc_core::sc_in<bool>     *dma_source[NUM_DMA_SOURCES];      // DMA sources
	sc_core::sc_in<bool>     *dma_always_on[NUM_DMA_ALWAYS_ON]; // DMA always on
	sc_core::sc_in<bool>    *dma_channel_ack[NUM_DMA_CHANNELS];// DMA channel acknownledgements
	sc_core::sc_in<bool>     *dma_trigger[NUM_DMA_TRIGGERS];    // DMA triggers
	sc_core::sc_out<bool>    *dma_channel[NUM_DMA_CHANNELS];    // DMA channels
	sc_core::sc_out<bool>    *dma_source_ack[NUM_DMA_SOURCES];  // DMA source acknownledgements
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	DMAMUX(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~DMAMUX();
	
	void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);

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
				return (time_stamp < sk.time_stamp);
			}
			
		private:
			sc_core::sc_time time_stamp;
		};
		
		Event()
			: key()
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
		tlm::tlm_generic_payload *payload;   // payload
		bool release_payload;                // whether payload must be released using payload memory management
		sc_core::sc_event *completion_event; // completion event (for blocking transport interface)
	};

	// Common DMAMUX register representation
	template <typename REGISTER, Access _ACCESS>
	struct DMAMUX_Register : AddressableRegister<REGISTER, 8, _ACCESS>
	{
		typedef AddressableRegister<REGISTER, 8, _ACCESS> Super;
		
		DMAMUX_Register(DMAMUX<CONFIG> *_dmamux) : Super(), dmamux(_dmamux) {}
		DMAMUX_Register(DMAMUX<CONFIG> *_dmamux, uint32_t value) : Super(value), dmamux(_dmamux) {}

		inline bool IsVerboseRead() const ALWAYS_INLINE { return dmamux->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return dmamux->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return dmamux->logger.DebugInfoStream(); }

		using Super::operator =;
		
	protected:
		DMAMUX<CONFIG> *dmamux;
	};
	
	// Channel Configuration register (DMAMUX_CHCFGn)
	struct DMAMUX_CHCFG : DMAMUX_Register<DMAMUX_CHCFG, SW_RW>
	{
		typedef DMAMUX_Register<DMAMUX_CHCFG, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct ENBL   : Field<DMAMUX_CHCFG, ENBL, 0>      {}; // DMA Channel Enable
		struct TRIG   : Field<DMAMUX_CHCFG, TRIG, 1>      {}; // DMA Channel Trigger Enable
		struct SOURCE : Field<DMAMUX_CHCFG, SOURCE, 2, 7> {}; // DMA Channel Source (Slot)
		
		typedef FieldSet<ENBL, TRIG, SOURCE> ALL;
		
		DMAMUX_CHCFG() : Super(0), reg_num(0) {}
		DMAMUX_CHCFG(DMAMUX<CONFIG> *_dmamux) : Super(_dmamux) {}
		DMAMUX_CHCFG(DMAMUX<CONFIG> *_dmamux, uint32_t value) : Super(_dmamux, value) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, DMAMUX<CONFIG> *_dmamux)
		{
			this->dmamux = _dmamux;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "DMAMUX_CHCFG" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "Channel Configuration register #" << reg_num;
			this->SetDescription(description_sstr.str());
			
			ENBL  ::SetName("ENBL");   ENBL  ::SetDescription("DMA Channel Enable");
			TRIG  ::SetName("TRIG");   TRIG  ::SetDescription("DMA Channel Trigger Enable");
			SOURCE::SetName("SOURCE"); SOURCE::SetDescription("DMA Channel Source (Slot)");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
			this->dmamux->UpdateChannel(reg_num);
		}

		virtual ReadWriteStatus Write(const uint8_t& value, const uint8_t& bit_enable)
		{
			ReadWriteStatus rws = (reg_num < NUM_DMA_TRIGGERS) ? Super::Write(value, bit_enable)
			                                                   : Super::template WritePreserve<TRIG>(value, bit_enable);
			
			this->dmamux->UpdateChannel(reg_num);
			
			return rws;
		}
		
		using Super::operator =;
		
	private:
		unsigned int reg_num;
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy; // proxy to get clock properties from master clock port
	
	AddressableRegisterFile<DMAMUX_CHCFG, NUM_DMA_CHANNELS, DMAMUX<CONFIG> > dmamux_chcfg; // DMAMUX_CHCFGn
	
	RegisterAddressMap<sc_dt::uint64> reg_addr_map;
	
	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	unisim::kernel::tlm2::Schedule<Event> schedule; // Payload (processor requests over AHB interface) schedule
	
	bool disable_dma_source[NUM_DMA_SOURCES];
	sc_core::sc_event *dma_source_routing_change_event[NUM_DMA_SOURCES]; // configuration change event per DMA channel
	sc_core::sc_event *dma_chcfg_event[NUM_DMA_CHANNELS];
	sc_core::sc_event *dma_source_event[NUM_DMA_CHANNELS];            // source change event per DMA channel
	sc_core::sc_event *dma_source_ack_event[NUM_DMA_SOURCES];
	
	bool conf_chk;
	bool conf_ok;
	unsigned int routing_table[NUM_DMA_SOURCES];
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::ParameterArray<bool> param_disable_dma_source;
	
	sc_core::sc_time master_clock_period;                 // Master clock period
	sc_core::sc_time master_clock_start_time;             // Master clock start time
	bool master_clock_posedge_first;                      // Master clock posedge first ?
	double master_clock_duty_cycle;                       // Master clock duty cycle
	
	void Reset();
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void RESET_B_Process();
	void DMA_SOURCE_Process(unsigned int dma_source_num);
	void DMA_CHANNEL_Process(unsigned int dma_channel_num);
	void DMA_CHANNEL_ACK_Process(unsigned int dma_channel_num);
	void DMA_SOURCE_ACK_Process(unsigned int dma_source_num);
	void MasterClockPropertiesChangedProcess();
	void UpdateMasterClock();
	void UpdateChannel(unsigned int dma_channel_num);
	bool CheckConfiguration();
};

} // end of namespace dmamux
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace dma
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_DMAMUX_DMAMUX_HH__
