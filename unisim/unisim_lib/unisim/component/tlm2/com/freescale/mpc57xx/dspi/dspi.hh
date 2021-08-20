/*
 *  Copyright (c) 2018,
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

#ifndef __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_DSPI_DSPI_HH__
#define __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_DSPI_DSPI_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/tlm_serial.hh>
#include <unisim/kernel/tlm2/clock.hh>
#include <unisim/util/reg/core/register.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/debug/simple_register_registry.hh>
#include <math.h>

#define SWITCH_ENUM_TRAIT(ENUM_TYPE, CLASS_NAME) template <ENUM_TYPE, bool __SWITCH_TRAIT_DUMMY__ = true> struct CLASS_NAME {}
#define CASE_ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) template <bool __SWITCH_TRAIT_DUMMY__> struct CLASS_NAME<ENUM_VALUE, __SWITCH_TRAIT_DUMMY__>
#define ENUM_TRAIT(ENUM_VALUE, CLASS_NAME) CLASS_NAME<ENUM_VALUE>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace freescale {
namespace mpc57xx {
namespace dspi {

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
using unisim::util::reg::core::SW_R_HW_RO;
using unisim::util::reg::core::SW_R_W1C;
using unisim::util::reg::core::SW_W;
using unisim::util::reg::core::RWS_OK;
using unisim::util::reg::core::RWS_ANA;
using unisim::util::reg::core::Access;
using unisim::util::reg::core::IsReadWriteError;
using unisim::util::reg::core::ClearReadWriteStatusFlag;
using unisim::util::reg::core::RWSF_WROB;

template <typename REGISTER, typename FIELD, int OFFSET1, int OFFSET2 = -1, Access _ACCESS = SW_RW>
struct Field : unisim::util::reg::core::Field<FIELD
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? ((REGISTER::SIZE - 1) - OFFSET2) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : ((REGISTER::SIZE - 1) - OFFSET1)) : 0
                                             , ((OFFSET1 >= 0) && (OFFSET1 < REGISTER::SIZE)) ? ((OFFSET2 >= 0) ? ((OFFSET2 < REGISTER::SIZE) ? ((OFFSET1 < OFFSET2) ? (OFFSET2 - OFFSET1 + 1) : (OFFSET1 - OFFSET2 + 1)) : 0) : 1) : 0
                                             , _ACCESS>
{
};

enum DSPI_Configuration
{
	DCONF_SPI = 0,
	DCONF_DSI = 1,
	DCONF_CSI = 2,
	DCONF_RES = 3
};

inline std::ostream& operator << (std::ostream& os, const DSPI_Configuration& dconf)
{
	switch(dconf)
	{
		case DCONF_SPI: os << "SPI configuration"; break;
		case DCONF_DSI: os << "DSI configuration"; break;
		case DCONF_CSI: os << "CSI configuration"; break;
		case DCONF_RES: os << "reserved configuration"; break;
		default: os << "unknown configuration"; break;
	}
	
	return os;
}

enum DSPI_State
{
	DSPI_STOPPED = 0,
	DSPI_RUNNING = 1
};

inline std::ostream& operator << (std::ostream& os, const DSPI_State& state)
{
	switch(state)
	{
		case DSPI_STOPPED: os << "stopped state"; break;
		case DSPI_RUNNING: os << "running state"; break;
		default: os << "unknown state"; break;
	}
	
	return os;
}

template <typename CONFIG>
class DSPI
	: public sc_core::sc_module
	, public tlm::tlm_fw_transport_if<>
	, public unisim::kernel::Service<typename unisim::service::interfaces::Registers>
{
public:
	static const unsigned int TLM2_IP_VERSION_MAJOR    = 1;
	static const unsigned int TLM2_IP_VERSION_MINOR    = 0;
	static const unsigned int TLM2_IP_VERSION_PATCH    = 0;
	static const unsigned int BUSWIDTH                 = CONFIG::BUSWIDTH;
	static const unsigned int NUM_CTARS                = CONFIG::NUM_CTARS;
	static const unsigned int TX_FIFO_DEPTH            = CONFIG::TX_FIFO_DEPTH;
	static const unsigned int RX_FIFO_DEPTH            = CONFIG::RX_FIFO_DEPTH;
	static const unsigned int CMD_FIFO_DEPTH           = CONFIG::CMD_FIFO_DEPTH;
	static const bool HAS_DATA_SERIALIZATION_SUPPORT   = CONFIG::HAS_DATA_SERIALIZATION_SUPPORT;
	static const unsigned int NUM_DSI_INPUTS           = CONFIG::NUM_DSI_INPUTS;
	static const unsigned int NUM_DSI_OUTPUTS          = CONFIG::NUM_DSI_OUTPUTS;
	static const unsigned int NUM_SLAVE_CONFIGURATIONS = 2;
	static const unsigned int SPI_CTAR_SLAVE_NUM       = 0;
	static const unsigned int DSI_CTAR_SLAVE_NUM       = 1;
	static const bool threaded_model                   = false;
	
	enum SerialInputInterface
	{
		SIN_IF, // Slave input interface
		SS_IF   // Slave select interface
	};
	
	// TLM socket types
	typedef tlm::tlm_target_socket<BUSWIDTH>                                         peripheral_slave_if_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket_tagged<DSPI<CONFIG> > SOUT_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket_tagged<DSPI<CONFIG> > SIN_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket_tagged<DSPI<CONFIG> > SS_type;
	typedef unisim::kernel::tlm2::tlm_serial_peripheral_socket_tagged<DSPI<CONFIG> > PCS_type;
	
	// inputs
	peripheral_slave_if_type                         peripheral_slave_if;         // Peripheral slave interface
	SOUT_type                                        SOUT;                        // Tx Serial interface
	SIN_type                                         SIN;                         // Rx Serial interface
	SS_type                                          SS;                          // Slave select
	sc_core::sc_in<bool>                             m_clk;                       // clock port
	sc_core::sc_in<bool>                             dspi_clk;                    // DSPI Clock port
	sc_core::sc_in<bool>                             reset_b;                     // reset
	sc_core::sc_in<bool>                             debug;                       // debug
	sc_core::sc_in<bool>                             HT;                          // Hardware trigger
	sc_core::sc_vector<sc_core::sc_in<bool> >        DSI_INPUT;                   // Deserial/Serial Interface parallel input signals
	sc_core::sc_in<bool>                             DMA_ACK_RX;                  // Rx DMA Acknowledgement
	sc_core::sc_in<bool>                             DMA_ACK_TX;                  // Tx DMA Acknowledgement
	sc_core::sc_in<bool>                             DMA_ACK_CMD;                 // CMD DMA Acknowledgement
	sc_core::sc_in<bool>                             DMA_ACK_DD;                  // Deserialized Data Match DMA Acknowledgement
	
	// outputs
	sc_core::sc_vector<PCS_type>                     PCS;                         // Peripheral Chip Select
	sc_core::sc_vector<sc_core::sc_out<bool> >       DSI_OUTPUT;                  // Deserial/Serial Interface parallel output signals
	sc_core::sc_out<bool>                            INT_EOQF;                    // Interrupt request DSPI_SR[EOQF]
	sc_core::sc_out<bool>                            INT_TFFF;                    // Interrupt request DSPI_SR[TFFF]
	sc_core::sc_out<bool>                            INT_CMDFFF;                  // Interrupt request DSPI_SR[CMDFFF]
	sc_core::sc_out<bool>                            INT_TFIWF;                   // Interrupt request DSPI_SR[TFIWF]
	sc_core::sc_out<bool>                            INT_TCF;                     // Interrupt request DSPI_SR[TCF]
	sc_core::sc_out<bool>                            INT_CMDTCF;                  // Interrupt request DSPI_SR[CMDTCF]
	sc_core::sc_out<bool>                            INT_SPITCF;                  // Interrupt request DSPI_SR[SPITCF]
	sc_core::sc_out<bool>                            INT_DSITCF;                  // Interrupt request DSPI_SR[DSITCF]
	sc_core::sc_out<bool>                            INT_TFUF;                    // Interrupt request DSPI_SR[TFUF]  
	sc_core::sc_out<bool>                            INT_RFDF;                    // Interrupt request DSPI_SR[RFDF]
	sc_core::sc_out<bool>                            INT_RFOF;                    // Interrupt request DSPI_SR[RFOF]
	sc_core::sc_out<bool>                            INT_SPEF;                    // Interrupt request DSPI_SR[SPEF]
	sc_core::sc_out<bool>                            INT_DPEF;                    // Interrupt request DSPI_SR[DPEF]
	sc_core::sc_out<bool>                            INT_DDIF;                    // Interrupt request DSPI_SR[DDIF]
	sc_core::sc_out<bool>                            DMA_RX;                      // Rx DMA request
	sc_core::sc_out<bool>                            DMA_TX;                      // Tx DMA request
	sc_core::sc_out<bool>                            DMA_CMD;                     // CMD DMA request
	sc_core::sc_out<bool>                            DMA_DD;                      // Deserialized Data Match DMA request
	
	// services
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;

	DSPI(const sc_core::sc_module_name& name, unisim::kernel::Object *parent);
	virtual ~DSPI();
	
	virtual void b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
	virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data);
	virtual unsigned int transport_dbg(tlm::tlm_generic_payload& payload);
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);

	void nb_receive(int id, unisim::kernel::tlm2::tlm_serial_payload& payload, const sc_core::sc_time& t);
	
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

	// DSPI Register
	template <typename REGISTER, Access _ACCESS>
	struct DSPI_Register : AddressableRegister<REGISTER, 32, _ACCESS>
	{
		typedef AddressableRegister<REGISTER, 32, _ACCESS> Super;
		
		DSPI_Register(DSPI<CONFIG> *_dspi) : Super(), dspi(_dspi) {}
		DSPI_Register(DSPI<CONFIG> *_dspi, uint32_t value) : Super(value), dspi(_dspi) {}
		
		inline bool IsVerboseRead() const ALWAYS_INLINE { return dspi->verbose; }
		inline bool IsVerboseWrite() const ALWAYS_INLINE { return dspi->verbose; }
		inline std::ostream& GetInfoStream() ALWAYS_INLINE { return dspi->logger.DebugInfoStream(); }
		
		using Super::operator =;
	protected:
		DSPI<CONFIG> *dspi;
	};

	// DSPI Module Configuration Register (DSPI_MCR)
	struct DSPI_MCR : DSPI_Register<DSPI_MCR, SW_RW>
	{
		typedef DSPI_Register<DSPI_MCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x0;
		
		struct MSTR      : Field<DSPI_MCR, MSTR     , 0     > {}; // Master/Slave Mode Select
		struct CONT_SCKE : Field<DSPI_MCR, CONT_SCKE, 1     > {}; // Continuous SCK Enable
		struct DCONF     : Field<DSPI_MCR, DCONF    , 2, 3  > {}; // DSPI Configuration
		struct FRZ       : Field<DSPI_MCR, FRZ      , 4     > {}; // Freeze
		struct MTFE      : Field<DSPI_MCR, MTFE     , 5     > {}; // Modified Timing Format Enable
		struct PCSSE     : Field<DSPI_MCR, PCSSE    , 6     > {}; // Peripheral Chip Select Strobe Enable
		struct ROOE      : Field<DSPI_MCR, ROOE     , 7     > {}; // Receive FIFO Overflow Overwrite Enable
		struct PCSIS     : Field<DSPI_MCR, PCSIS    , 8, 15 > {}; // Peripheral Chip Select x Inactive State
		struct MDIS      : Field<DSPI_MCR, MDIS     , 17    > {}; // Module Disable
		struct DIS_TXF   : Field<DSPI_MCR, DIS_TXF  , 18    > {}; // Disable Transmit FIFO
		struct DIS_RXF   : Field<DSPI_MCR, DIS_RXF  , 19    > {}; // Disable Receive FIFO
		struct CLR_TXF   : Field<DSPI_MCR, CLR_TXF  , 20    > {}; // Clear TX FIFO
		struct CLR_RXF   : Field<DSPI_MCR, CLR_RXF  , 21    > {}; // Clear RX FIFO
		struct SMPL_PT   : Field<DSPI_MCR, SMPL_PT  , 22, 23> {}; // Sample Point
		struct XSPI      : Field<DSPI_MCR, XSPI     , 28    > {}; // Extended SPI Mode
		struct FCPCS     : Field<DSPI_MCR, FCPCS    , 29    > {}; // Fast Continuous PCS Mode
		struct PES       : Field<DSPI_MCR, PES      , 30    > {}; // Parity Error Stop
		struct HALT      : Field<DSPI_MCR, HALT     , 31    > {}; // Halt
		
		typedef FieldSet<MSTR, CONT_SCKE, DCONF, FRZ, MTFE, PCSSE, ROOE, PCSIS, MDIS, DIS_TXF, DIS_RXF, CLR_TXF, CLR_RXF, SMPL_PT, XSPI, FCPCS, PES, HALT> ALL;
		
		DSPI_MCR(DSPI<CONFIG> *_dspi) : Super(_dspi), dspi_clock_period(), sample_point() { Init(); }
		DSPI_MCR(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value), dspi_clock_period(), sample_point() { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_MCR"); this->SetDescription("DSPI Module Configuration Register");
			
			MSTR     ::SetName("MSTR");      MSTR     ::SetDescription("Master/Slave Mode Select");
			CONT_SCKE::SetName("CONT_SCKE"); CONT_SCKE::SetDescription("Continuous SCK Enable");
			DCONF    ::SetName("DCONF");     DCONF    ::SetDescription("DSPI Configuration");
			FRZ      ::SetName("FRZ");       FRZ      ::SetDescription("Freeze");
			MTFE     ::SetName("MTFE");      MTFE     ::SetDescription("Modified Timing Format Enable");
			PCSSE    ::SetName("PCSSE");     PCSSE    ::SetDescription("Peripheral Chip Select Strobe Enable");
			ROOE     ::SetName("ROOE");      ROOE     ::SetDescription("Receive FIFO Overflow Overwrite Enable");
			PCSIS    ::SetName("PCSIS");     PCSIS    ::SetDescription("Peripheral Chip Select x Inactive State");
			MDIS     ::SetName("MDIS");      MDIS     ::SetDescription("Module Disable");
			DIS_TXF  ::SetName("DIS_TXF");   DIS_TXF  ::SetDescription("Disable Transmit FIFO");
			DIS_RXF  ::SetName("DIS_RXF");   DIS_RXF  ::SetDescription("Disable Receive FIFO");
			CLR_TXF  ::SetName("CLR_TXF");   CLR_TXF  ::SetDescription("Clear TX FIFO");
			CLR_RXF  ::SetName("CLR_RXF");   CLR_RXF  ::SetDescription("Clear RX FIFO");
			SMPL_PT  ::SetName("SMPL_PT");   SMPL_PT  ::SetDescription("Sample Point");
			XSPI     ::SetName("XSPI");      XSPI     ::SetDescription("Extended SPI Mode");
			FCPCS    ::SetName("FCPCS");     FCPCS    ::SetDescription("Fast Continuous PCS Mode");
			PES      ::SetName("PES");       PES      ::SetDescription("Parity Error Stop");
			HALT     ::SetName("HALT");      HALT     ::SetDescription("Halt");
		}
		
		void Reset()
		{
			this->Initialize(0x00004001);
			this->dspi->UpdateState();
			Update();
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			bool old_mstr = this->template Get<MSTR>();
			bool old_mdis = this->template Get<MDIS>();
			bool old_dis_txf = this->template Get<DIS_TXF>();
			bool old_pes = this->template Get<PES>();
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->dspi->UpdateState();
				
				bool new_mstr = this->template Get<MSTR>();
				bool new_mdis = this->template Get<MDIS>();
				bool new_dis_txf = this->template Get<DIS_TXF>();
				bool new_pes = this->template Get<PES>();
				
				bool mstr_value_changed = new_mstr ^ old_mstr;
				bool dis_txf_value_changed = new_dis_txf ^ old_dis_txf;
				
				if(mstr_value_changed && !this->template Get<HALT>() && (!this->dspi->TX_FIFO_Empty() || !this->dspi->CMD_FIFO_Empty() || !this->dspi->RX_FIFO_Empty()))
				{
					this->dspi->logger << DebugWarning << sc_core::sc_time_stamp() << ": when switching between master and slave modes, to garantee proper operation, " << this->GetName() << " should be set to 1, and both the transmit and the receive FIFOs should be cleared" << EndDebugWarning;
					
					this->dspi->ModeSwitch();
				}
				
				if(mstr_value_changed || dis_txf_value_changed)
				{
					this->dspi->MapRegisters();
				}
				
				if(this->template Get<CLR_TXF>())
				{
					this->dspi->TX_FIFO_Clear();
					this->template Set<CLR_TXF>(0);
				}
				
				if(this->template Get<CLR_RXF>())
				{
					this->dspi->RX_FIFO_Clear();
					this->template Set<CLR_RXF>(0);
				}
				
				if(old_mdis && !new_mdis) // enabling module?
				{
					this->dspi->ScheduleTransfer();
				}
				
				if((!old_mdis && new_mdis && !new_mstr) || (old_mstr && !new_mstr && new_mdis)) // master -> slave mode while MDIS=1, or MDIS=1 -> 0 while in slave mode
				{
					this->dspi->logger << DebugWarning << sc_core::sc_time_stamp() << ": MDIS should be set to '0' in Slave Mode as a slave doesn't control master transactions" << EndDebugWarning;
				}
				
				Update();
				
				// if PES is set to zero, it may allow restarting SPI/DSI operation/transmission if stopped because of parity error
				if(old_pes && !new_pes)
				{
					this->dspi->ScheduleTransfer();
				}
			}
			return rws;
		}
		
		const sc_core::sc_time& GetSamplePoint() const
		{
			Update();
			
			return sample_point;
		}
		
		using Super::operator =;
		
	private:
		mutable sc_core::sc_time dspi_clock_period;
		mutable sc_core::sc_time sample_point;
		
		void Update() const
		{
			if(unlikely(dspi_clock_period != this->dspi->dspi_clock_period))
			{
				// protocol clock: 1 / f_P
				dspi_clock_period = this->dspi->dspi_clock_period;

				// sample point = (1 / f_P) * SMPL_PT
				sample_point = dspi_clock_period;
				sample_point *= (double) this->template Get<SMPL_PT>();
			}
			
		}
	};
	
	// DSPI Transfer Count Register (DSPI_TCR)
	struct DSPI_TCR : DSPI_Register<DSPI_TCR, SW_RW>
	{
		typedef DSPI_Register<DSPI_TCR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x8;
		
		struct SPI_TCNT : Field<DSPI_TCR, SPI_TCNT, 0, 15> {}; // SPI Transfer Counter
		
		typedef FieldSet<SPI_TCNT> ALL;
		
		DSPI_TCR(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_TCR(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_TCR"); this->SetDescription("DSPI Transfer Count Register");
			
			SPI_TCNT::SetName("SPI_TCNT"); SPI_TCNT::SetDescription("SPI Transfer Counter");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(this->dspi->Running())
				{
					this->dspi->logger << DebugWarning << "do not write to " << this->GetName() << " while " << this->dspi->GetName() << " is in " << this->dspi->GetState() << EndDebugWarning;
				}
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// DSPI Clock and Transfer Attributes Register (In Master Mode) (DSPI_CTAR)
	struct DSPI_CTAR : DSPI_Register<DSPI_CTAR, SW_RW>
	{
		typedef DSPI_Register<DSPI_CTAR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc;
		
		struct DBR    : Field<DSPI_CTAR, DBR   , 0     > {}; // Double Baud Rate
		struct FMSZ   : Field<DSPI_CTAR, FMSZ  , 1, 4  > {}; // Frame Size
		struct CPOL   : Field<DSPI_CTAR, CPOL  , 5     > {}; // Clock Polarity
		struct CPHA   : Field<DSPI_CTAR, CPHA  , 6     > {}; // Clock Phase or TSB mode
		struct LSBFE  : Field<DSPI_CTAR, LSBFE , 7     > {}; // LSB First
		struct PCSSCK : Field<DSPI_CTAR, PCSSCK, 8, 9  > {}; // PCS to SCK Delay Prescaler
		struct PASC   : Field<DSPI_CTAR, PASC  , 10, 11> {}; // After SCK Delay Prescaler
		struct PDT    : Field<DSPI_CTAR, PDT   , 12, 13> {}; // Delay after Transfer Prescaler
		struct PBR    : Field<DSPI_CTAR, PBR   , 14, 15> {}; // Baud Rate Prescaler. (Master mode only)
		struct CSSCK  : Field<DSPI_CTAR, CSSCK , 16, 19> {}; // PCS to SCK Delay Scaler. (Master mode only)
		struct ASC    : Field<DSPI_CTAR, ASC   , 20, 23> {}; // After SCK Delay Scaler. (Master mode only)
		struct DT     : Field<DSPI_CTAR, DT    , 24, 27> {}; // Delay After Transfer Scaler (Master mode only)
		struct BR     : Field<DSPI_CTAR, BR    , 28, 31> {}; // Baud Rate Scaler. (Master mode only)
		
		typedef FieldSet<DBR, FMSZ, CPOL, CPHA, LSBFE, PCSSCK, PASC, PDT, PBR, CSSCK, ASC, DT, BR> ALL;
		
		DSPI_CTAR() : Super(0), reg_num(0), master_clock_period(), dspi_clock_period(), dconf(DCONF_SPI), tsbc(false), scke(false), sck_period(), sck_half_period(), t_CSC(), t_ASC(), t_DT(), min_frame_size(0) {}
		DSPI_CTAR(DSPI<CONFIG> *_dspi) : Super(_dspi), reg_num(0), master_clock_period(), dspi_clock_period(), dconf(DCONF_SPI), tsbc(false), scke(false), sck_period(), sck_half_period(), t_CSC(), t_ASC(), t_DT(), min_frame_size(0) {}
		DSPI_CTAR(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value), reg_num(0), master_clock_period(), dspi_clock_period(), dconf(DCONF_SPI), tsbc(false), scke(false), sck_period(), sck_half_period(), t_CSC(), t_ASC(), t_DT(), min_frame_size(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, DSPI<CONFIG> *_dspi)
		{
			this->dspi = _dspi;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "DSPI_CTAR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "DSPI Clock and Transfer Attributes Register " << reg_num << "(In Master Mode)";
			this->SetDescription(description_sstr.str());
			
			DBR   ::SetName("DBR");    DBR   ::SetDescription("Double Baud Rate");
			FMSZ  ::SetName("FMSZ");   FMSZ  ::SetDescription("Frame Size");
			CPOL  ::SetName("CPOL");   CPOL  ::SetDescription("Clock Polarity");
			CPHA  ::SetName("CPHA");   CPHA  ::SetDescription("Clock Phase or TSB mode");
			LSBFE ::SetName("LSBFE");  LSBFE ::SetDescription("LSB First");
			PCSSCK::SetName("PCSSCK"); PCSSCK::SetDescription("PCS to SCK Delay Prescaler");
			PASC  ::SetName("PASC");   PASC  ::SetDescription("After SCK Delay Prescaler");
			PDT   ::SetName("PDT");    PDT   ::SetDescription("Delay after Transfer Prescaler");
			PBR   ::SetName("PBR");    PBR   ::SetDescription("Baud Rate Prescaler. (Master mode only)");
			CSSCK ::SetName("CSSCK");  CSSCK ::SetDescription("PCS to SCK Delay Scaler. (Master mode only)");
			ASC   ::SetName("ASC");    ASC   ::SetDescription("After SCK Delay Scaler. (Master mode only)");
			DT    ::SetName("DT");     DT    ::SetDescription("Delay After Transfer Scaler (Master mode only)");
			BR    ::SetName("BR");     BR    ::SetDescription("Baud Rate Scaler. (Master mode only)");
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(this->dspi->Running())
				{
					this->dspi->logger << DebugWarning << "do not write to " << this->GetName() << " while " << this->dspi->GetName() << " is in " << this->dspi->GetState() << EndDebugWarning;
				}
				
				switch(reg_num)
				{
					case SPI_CTAR_SLAVE_NUM:
					case DSI_CTAR_SLAVE_NUM:
						// mirror FMSZ, CPOL and CPHA in DSPI_CTAR_SLAVE registers
						this->dspi->dspi_ctar_slave[reg_num].template Set<typename DSPI_CTAR_SLAVE::FMSZ>(this->template Get<FMSZ>());
						this->dspi->dspi_ctar_slave[reg_num].template Set<typename DSPI_CTAR_SLAVE::CPOL>(this->template Get<CPOL>());
						this->dspi->dspi_ctar_slave[reg_num].template Set<typename DSPI_CTAR_SLAVE::CPHA>(this->template Get<CPHA>());
						break;
				}
				
				Update();
			}
			return rws;
		}
		
		void Reset()
		{
			this->Initialize(0x78000000);
			Update();
		}
		
		const sc_core::sc_time GetSCKPeriod() const
		{
			Update();
			
			return sck_period;
		}
		
		const sc_core::sc_time GetSCKHalfPeriod() const
		{
			Update();
			
			return sck_half_period;
		}

		const sc_core::sc_time GetPCS2SCKDelay() const
		{
			Update();

			return t_CSC;
		}
		
		const sc_core::sc_time GetAfterSCKDelay() const
		{
			Update();

			return t_ASC;
		}
		
		const sc_core::sc_time GetDelayAfterTransfer() const
		{
			Update();
			
			return t_DT;
		}
		
		unsigned int GetMinFrameSize() const
		{
			Update();
			
			return min_frame_size;
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
		mutable sc_core::sc_time master_clock_period;
		mutable sc_core::sc_time dspi_clock_period;
		mutable DSPI_Configuration dconf;
		mutable bool tsbc;
		mutable bool scke;
		mutable sc_core::sc_time sck_period;
		mutable sc_core::sc_time sck_half_period;
		mutable sc_core::sc_time t_CSC;
		mutable sc_core::sc_time t_ASC;
		mutable sc_core::sc_time t_DT;
		mutable unsigned int min_frame_size;
		
		void Update() const
		{
			if(unlikely((master_clock_period != this->dspi->master_clock_period) ||
			            (dspi_clock_period != this->dspi->dspi_clock_period) ||
			            (dconf != this->dspi->Configuration()) ||
			            (tsbc != this->dspi->dspi_dsicr0.template Get<typename DSPI_DSICR0::TSBC>()) ||
			            (scke != this->dspi->dspi_mcr.template Get<typename DSPI_MCR::CONT_SCKE>())))
			{
				// register read/write clock: 1 / f_r
				master_clock_period = this->dspi->master_clock_period;
				
				// protocol clock: 1 / f_P
				dspi_clock_period = this->dspi->dspi_clock_period;
				
				dconf = this->dspi->Configuration();
				tsbc = this->dspi->dspi_dsicr0.template Get<typename DSPI_DSICR0::TSBC>();
				scke = this->dspi->dspi_mcr.template Get<typename DSPI_MCR::CONT_SCKE>();
				
				// baud rate: 1 / f_SCK = (1 / f_P) * (PBR * BR) / (1 + DBR)
				unsigned int pbr = this->template Get<PBR>() ? ((2 * this->template Get<PBR>()) + 1) : 2;
				unsigned int dbr = this->template Get<DBR>();
				unsigned int br = 1 << (this->template Get<BR>() + 1);
				double n = (double)(pbr * br) / (1 + dbr);
				sck_period = dspi_clock_period;
				sck_period *= n;
				sck_half_period = sck_period;
				sck_half_period *= 0.5;
				
				// PCS to SCK delay: t_CSC = (1 / f_P) * PCSSCK * CSSCK
				unsigned int pcssck = (2 * this->template Get<PCSSCK>()) + 1;
				unsigned int cssck = 1 << (this->template Get<CSSCK>() + 1);
				t_CSC = dspi_clock_period;
				t_CSC *= (double) (pcssck * cssck);
				
				// After SCK delay: t_ASC = (1 / f_P) * PASC * ASC
				unsigned int pasc = (2 * this->template Get<PASC>()) + 1;
				unsigned int asc = 1 << (this->template Get<ASC>() + 1);
				t_ASC = dspi_clock_period;
				t_ASC *= (double) (pasc * asc);
				
				t_DT = dspi_clock_period;
				if((dconf == DCONF_CSI) && tsbc)
				{
					// TSB mode
					// Delay after transfer: t_DT = (1 / f_P) * (concat(PDT, BT) + 1)
					unsigned int pdt = this->template Get<PDT>();
					unsigned int dt = this->template Get<DT>();
					t_DT *= (double)((1 << (((pdt << DT::BITWIDTH) | dt) + 1)) + 1);
				}
				else if(scke)
				{
					// Continuous Serial Communications Clock
					// Delay after transfer: t_DT = (1 / f_P)
				}
				else
				{
					unsigned int pdt = (2 * this->template Get<PDT>()) + 1;
					unsigned int dt = 1 << (this->template Get<DT>() + 1);
					// Delay after transfer: t_DT = (1 / f_P) * PDT * DT
					t_DT *= (double) (pdt * dt);
				}
				
				double _min_frame_size = (((master_clock_period * 4.0) / dspi_clock_period) + 3.0) / n;
				min_frame_size = ceil(_min_frame_size);
				if(min_frame_size < 4) min_frame_size = 4;
			}
		}
	};
	
	// DSPI Clock and Transfer Attributes Register (In Slave Mode) (DSPI_CTAR_SLAVE)
	struct DSPI_CTAR_SLAVE : DSPI_Register<DSPI_CTAR_SLAVE, SW_RW>
	{
		typedef DSPI_Register<DSPI_CTAR_SLAVE, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc;
		
		struct FMSZ   : Field<DSPI_CTAR_SLAVE, FMSZ  , 1, 4  > {}; // Frame Size
		struct CPOL   : Field<DSPI_CTAR_SLAVE, CPOL  , 5     > {}; // Clock Polarity
		struct CPHA   : Field<DSPI_CTAR_SLAVE, CPHA  , 6     > {}; // Clock Phase or TSB mode
		struct PE     : Field<DSPI_CTAR_SLAVE, PE    , 7     > {}; // Parity Enable
		struct PP     : Field<DSPI_CTAR_SLAVE, PP    , 8     > {}; // Parity Polarity
		
		typedef FieldSet<FMSZ, CPOL, CPHA, PE, PP> ALL;
		
		DSPI_CTAR_SLAVE() : Super(0), reg_num(0) {}
		DSPI_CTAR_SLAVE(DSPI<CONFIG> *_dspi) : Super(_dspi), reg_num(0) {}
		DSPI_CTAR_SLAVE(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, DSPI<CONFIG> *_dspi)
		{
			this->dspi = _dspi;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "DSPI_CTAR_SLAVE" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "DSPI Clock and Transfer Attributes Register " << reg_num << "(In Slave Mode)";
			this->SetDescription(description_sstr.str());
			
			FMSZ::SetName("FMSZ");   FMSZ::SetDescription("Frame Size");
			CPOL::SetName("CPOL");   CPOL::SetDescription("Clock Polarity");
			CPHA::SetName("CPHA");   CPHA::SetDescription("Clock Phase or TSB mode");
			PE  ::SetName("PE");     PE  ::SetDescription("Parity Enable");
			PP  ::SetName("PP");     PP  ::SetDescription("Parity Polarity");
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				// mirror FMSZ, CPOL and CPHA in DSPI_CTAR registers
				this->dspi->dspi_ctar[reg_num].template Set<typename DSPI_CTAR::FMSZ>(this->template Get<FMSZ>());
				this->dspi->dspi_ctar[reg_num].template Set<typename DSPI_CTAR::CPOL>(this->template Get<CPOL>());
				this->dspi->dspi_ctar[reg_num].template Set<typename DSPI_CTAR::CPHA>(this->template Get<CPHA>());
			}
			return rws;
		}
		
		void Reset()
		{
			this->Initialize(0x78000000);
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// DSPI Status Register (DSPI_SR)
	struct DSPI_SR : DSPI_Register<DSPI_SR, SW_RW>
	{
		typedef DSPI_Register<DSPI_SR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x2c;
		
		struct TCF       : Field<DSPI_SR, TCF      , 0 , 0 , SW_R_W1C> {}; // Transfer Complete Flag
		struct TXRXS     : Field<DSPI_SR, TXRXS    , 1 , 1 , SW_R_W1C> {}; // TX and RX Status
		struct SPITCF    : Field<DSPI_SR, SPITCF   , 2 , 2 , SW_R_W1C> {}; // SPI Frame Transfer Complete Flag
		struct EOQF      : Field<DSPI_SR, EOQF     , 3 , 3 , SW_R_W1C> {}; // End of Queue Flag
		struct TFUF      : Field<DSPI_SR, TFUF     , 4 , 4 , SW_R_W1C> {}; // Transmit FIFO Underflow Flag
		struct DSITCF    : Field<DSPI_SR, DSITCF   , 5 , 5 , SW_R_W1C> {}; // DSI Frame Transfer Complete Flag
		struct TFFF      : Field<DSPI_SR, TFFF     , 6 , 6 , SW_R_W1C> {}; // Transmit FIFO Fill Flag
		struct BSYF      : Field<DSPI_SR, BSYF     , 7 , 7 , SW_R    > {}; // Busy Flag
		struct CMDTCF    : Field<DSPI_SR, CMDTCF   , 8 , 8 , SW_R_W1C> {}; // Command Transfer Complete Flag
		struct DPEF      : Field<DSPI_SR, DPEF     , 9 , 9 , SW_R_W1C> {}; // DSI Parity Error Flag
		struct SPEF      : Field<DSPI_SR, SPEF     , 10, 10, SW_R_W1C> {}; // SPI Parity Error Flag
		struct DDIF      : Field<DSPI_SR, DDIF     , 11, 11, SW_R_W1C> {}; // DSI Data Received with Active Bits
		struct RFOF      : Field<DSPI_SR, RFOF     , 12, 12, SW_R_W1C> {}; // Receive FIFO Overflow Flag
		struct TFIWF     : Field<DSPI_SR, TFIWF    , 13, 13, SW_R_W1C> {}; // Transmit FIFO Invalid Write Flag
		struct RFDF      : Field<DSPI_SR, RFDF     , 14, 14, SW_R_W1C> {}; // Receive FIFO Drain Flag
		struct CMDFFF    : Field<DSPI_SR, CMDFFF   , 15, 15, SW_R_W1C> {}; // Command FIFO Fill Flag
		struct TXCTR     : Field<DSPI_SR, TXCTR    , 16, 19, SW_R    > {}; // TX FIFO Counter
		struct TXNXTPTR  : Field<DSPI_SR, TXNXTPTR , 20, 23, SW_R    > {}; // Transmit Next Pointer
		struct RXCTR     : Field<DSPI_SR, RXCTR    , 24, 27, SW_R    > {}; // RX FIFO Counter
		struct POPNXTPTR : Field<DSPI_SR, POPNXTPTR, 28, 31, SW_R    > {}; // Pop Next Pointer
		
		SWITCH_ENUM_TRAIT(bool, _);
		CASE_ENUM_TRAIT(false, _) { typedef FieldSet<TCF, TXRXS, SPITCF, EOQF, TFUF, TFFF, BSYF, CMDTCF, SPEF, RFOF, TFIWF, RFDF, CMDFFF, TXCTR, TXNXTPTR, RXCTR, POPNXTPTR> ALL; };
		CASE_ENUM_TRAIT(true, _) { typedef FieldSet<TCF, TXRXS, SPITCF, EOQF, TFUF, DSITCF, TFFF, BSYF, CMDTCF, DPEF, SPEF, DDIF, RFOF, TFIWF, RFDF, CMDFFF, TXCTR, TXNXTPTR, RXCTR, POPNXTPTR> ALL; };
		typedef typename ENUM_TRAIT(HAS_DATA_SERIALIZATION_SUPPORT, _)::ALL ALL;
		
		DSPI_SR(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_SR(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_SR"); this->SetDescription("DSPI Status Register");
			TCF      ::SetName("TCF");       TCF      ::SetDescription("Transfer Complete Flag");
			TXRXS    ::SetName("TXRXS");     TXRXS    ::SetDescription("TX and RX Status");
			SPITCF   ::SetName("SPITCF");    SPITCF   ::SetDescription("SPI Frame Transfer Complete Flag");
			EOQF     ::SetName("EOQF");      EOQF     ::SetDescription("End of Queue Flag");
			TFUF     ::SetName("TFUF");      TFUF     ::SetDescription("Transmit FIFO Underflow Flag");
			if(HAS_DATA_SERIALIZATION_SUPPORT) { DSITCF   ::SetName("DSITCF");    DSITCF   ::SetDescription("DSI Frame Transfer Complete Flag"); }
			TFFF     ::SetName("TFFF");      TFFF     ::SetDescription("Transmit FIFO Fill Flag");
			BSYF     ::SetName("BSYF");      BSYF     ::SetDescription("Busy Flag");
			CMDTCF   ::SetName("CMDTCF");    CMDTCF   ::SetDescription("Command Transfer Complete Flag");
			if(HAS_DATA_SERIALIZATION_SUPPORT) { DPEF     ::SetName("DPEF");      DPEF     ::SetDescription("DSI Parity Error Flag"); }
			SPEF     ::SetName("SPEF");      SPEF     ::SetDescription("SPI Parity Error Flag");
			if(HAS_DATA_SERIALIZATION_SUPPORT) { DDIF     ::SetName("DDIF");      DDIF     ::SetDescription("DSI Data Received with Active Bits"); }
			RFOF     ::SetName("RFOF");      RFOF     ::SetDescription("Receive FIFO Overflow Flag");
			TFIWF    ::SetName("TFIWF");     TFIWF    ::SetDescription("Transmit FIFO Invalid Write Flag");
			RFDF     ::SetName("RFDF");      RFDF     ::SetDescription("Receive FIFO Drain Flag");
			CMDFFF   ::SetName("CMDFFF");    CMDFFF   ::SetDescription("Command FIFO Fill Flag");
			TXCTR    ::SetName("TXCTR");     TXCTR    ::SetDescription("TX FIFO Counter");
			TXNXTPTR ::SetName("TXNXTPTR");  TXNXTPTR ::SetDescription("Transmit Next Pointer");
			RXCTR    ::SetName("RXCTR");     RXCTR    ::SetDescription("RX FIFO Counter");
			POPNXTPTR::SetName("POPNXTPTR"); POPNXTPTR::SetDescription("Pop Next Pointer");
		}
		
		void Reset()
		{
			this->Initialize(0x02010000);
			this->dspi->UpdateState();
			this->dspi->UpdateINT_EOQF();
			this->dspi->UpdateINT_TFFF();  
			this->dspi->UpdateINT_CMDFFF();
			this->dspi->UpdateINT_TFIWF();
			this->dspi->UpdateINT_TCF();   
			this->dspi->UpdateINT_CMDTCF();
			this->dspi->UpdateINT_SPITCF();
			this->dspi->UpdateINT_DSITCF();
			this->dspi->UpdateINT_TFUF();
			this->dspi->UpdateINT_RFDF();
			this->dspi->UpdateINT_RFOF();
			this->dspi->UpdateINT_SPEF();
			if(HAS_DATA_SERIALIZATION_SUPPORT) this->dspi->UpdateINT_DPEF();
			if(HAS_DATA_SERIALIZATION_SUPPORT) this->dspi->UpdateINT_DDIF();
			this->dspi->UpdateDMA_RX();
			this->dspi->UpdateDMA_TX();
			this->dspi->UpdateDMA_CMD();
			if(HAS_DATA_SERIALIZATION_SUPPORT) this->dspi->UpdateDMA_DD();
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			uint32_t old_value = this->Get();
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				ClearReadWriteStatusFlag(rws, RWSF_WROB); // disable writing read-only bits warnings
				
				this->dspi->UpdateState();
				
				uint32_t new_value = this->Get();
				
				uint32_t value_changed = new_value ^ old_value;
				
				if(TCF   ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_TCF();
				
				if(SPITCF::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_SPITCF();
				
				if(EOQF  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_EOQF();
				
				if(TFUF  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_TFUF();
				
				if(HAS_DATA_SERIALIZATION_SUPPORT && DSITCF::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_DSITCF();
				
				if(TFFF  ::template Get<uint32_t>(value_changed))
				{
					this->dspi->UpdateINT_TFFF();
					this->dspi->UpdateDMA_TX();
				}
				
				if(CMDTCF::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_CMDTCF();
				
				if(HAS_DATA_SERIALIZATION_SUPPORT && DPEF  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_DPEF();
				
				if(SPEF  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_SPEF();
				
				if(HAS_DATA_SERIALIZATION_SUPPORT && DDIF  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateDMA_DD();
				
				if(RFOF  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_RFOF();
				
				if(TFIWF ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_TFIWF();
				
				if(RFDF  ::template Get<uint32_t>(value_changed))
				{
					this->dspi->UpdateINT_RFDF();
					this->dspi->UpdateDMA_RX();
				}
				
				if(CMDFFF::template Get<uint32_t>(value_changed))
				{
					this->dspi->UpdateINT_CMDFFF();
					this->dspi->UpdateDMA_CMD();
				}
				
				if(HAS_DATA_SERIALIZATION_SUPPORT && DDIF  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_DDIF();
				
				// if DSPI_MCR[PES] is 1 and a SPEF or DPEF are set, clearing of SPEF or DPEF may allow restarting SPI/DSI operation/transmission
				if(SPEF::template Get<uint32_t>(value_changed) || (HAS_DATA_SERIALIZATION_SUPPORT && DPEF::template Get<uint32_t>(value_changed)))
				{
					this->dspi->ScheduleTransfer();
				}
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// DSPI DMA/Interrupt Request Select and Enable Register (DSPI_RSER)
	struct DSPI_RSER : DSPI_Register<DSPI_RSER, SW_RW>
	{
		typedef DSPI_Register<DSPI_RSER, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x30;
		
		struct TCF_RE      : Field<DSPI_RSER, TCF_RE     , 0 > {}; // Transmission Complete Request Enable
		struct CMDFFF_RE   : Field<DSPI_RSER, CMDFFF_RE  , 1 > {}; // Command FIFO Fill Flag Request Enable
		struct SPITCF_RE   : Field<DSPI_RSER, SPITCF_RE  , 2 > {}; // SPI Frame Transmission Complete Request Enable
		struct EOQF_RE     : Field<DSPI_RSER, EOQF_RE    , 3 > {}; // DSPI Finished Request Enable
		struct TFUF_RE     : Field<DSPI_RSER, TFUF_RE    , 4 > {}; // Transmit FIFO Underflow Request Enable
		struct DSITCF_RE   : Field<DSPI_RSER, DSITCF_RE  , 5 > {}; // DSI Frame Transmission Complete Request Enable
		struct TFFF_RE     : Field<DSPI_RSER, TFFF_RE    , 6 > {}; // Transmit FIFO Fill Request Enable
		struct TFFF_DIRS   : Field<DSPI_RSER, TFFF_DIRS  , 7 > {}; // Transmit FIFO Fill DMA or Interrupt Request Select
		struct CMDTCF_RE   : Field<DSPI_RSER, CMDTCF_RE  , 8 > {}; // Command Transmission Complete Request Enable
		struct DPEF_RE     : Field<DSPI_RSER, DPEF_RE    , 9 > {}; // DSI Parity Error Request Enable
		struct SPEF_RE     : Field<DSPI_RSER, SPEF_RE    , 10> {}; // SPI Parity Error Request Enable
		struct DDIF_RE     : Field<DSPI_RSER, DDIF_RE    , 11> {}; // DSI data received with active bits Request Enable
		struct RFOF_RE     : Field<DSPI_RSER, RFOF_RE    , 12> {}; // Receive FIFO Overflow Request Enable
		struct TFIWF_RE    : Field<DSPI_RSER, TFIWF_RE   , 13> {}; // Transmit FIFO Invalid Write Request Enable
		struct RFDF_RE     : Field<DSPI_RSER, RFDF_RE    , 14> {}; // Receive FIFO Drain Request Enable
		struct RFDF_DIRS   : Field<DSPI_RSER, RFDF_DIRS  , 15> {}; // Receive FIFO Drain DMA or Interrupt Request Select
		struct CMDFFF_DIRS : Field<DSPI_RSER, CMDFFF_DIRS, 16> {}; // Command FIFO Fill DMA or Interrupt Request Select
		struct DDIF_DIRS   : Field<DSPI_RSER, DDIF_DIRS  , 17> {}; // DSI data received with active bits - DMA or Interrupt Request Select
		
		SWITCH_ENUM_TRAIT(bool, _);
		CASE_ENUM_TRAIT(false, _) { typedef FieldSet<TCF_RE, CMDFFF_RE, SPITCF_RE, EOQF_RE, TFUF_RE, TFFF_RE, TFFF_DIRS, CMDTCF_RE, SPEF_RE, RFOF_RE, TFIWF_RE, RFDF_RE, RFDF_DIRS, CMDFFF_DIRS> ALL; };
		CASE_ENUM_TRAIT(true, _) { typedef FieldSet<TCF_RE, CMDFFF_RE, SPITCF_RE, EOQF_RE, TFUF_RE, DSITCF_RE, TFFF_RE, TFFF_DIRS, CMDTCF_RE, DPEF_RE, SPEF_RE, DDIF_RE, RFOF_RE, TFIWF_RE, RFDF_RE, RFDF_DIRS, CMDFFF_DIRS, DDIF_DIRS> ALL; };
		typedef typename ENUM_TRAIT(HAS_DATA_SERIALIZATION_SUPPORT, _)::ALL ALL;
		
		DSPI_RSER(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_RSER(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_RSER"); this->SetDescription("DSPI DMA/Interrupt Request Select and Enable Register");
			TCF_RE     ::SetName("TCF_RE");      TCF_RE     ::SetDescription("Transmission Complete Request Enable");
			CMDFFF_RE  ::SetName("CMDFFF_RE");   CMDFFF_RE  ::SetDescription("Command FIFO Fill Flag Request Enable");
			SPITCF_RE  ::SetName("SPITCF_RE");   SPITCF_RE  ::SetDescription("SPI Frame Transmission Complete Request Enable");
			EOQF_RE    ::SetName("EOQF_RE");     EOQF_RE    ::SetDescription("DSPI Finished Request Enable");
			TFUF_RE    ::SetName("TFUF_RE");     TFUF_RE    ::SetDescription("Transmit FIFO Underflow Request Enable");
			if(HAS_DATA_SERIALIZATION_SUPPORT) { DSITCF_RE  ::SetName("DSITCF_RE");   DSITCF_RE  ::SetDescription("DSI Frame Transmission Complete Request Enable"); }
			TFFF_RE    ::SetName("TFFF_RE");     TFFF_RE    ::SetDescription("Transmit FIFO Fill Request Enable");
			TFFF_DIRS  ::SetName("TFFF_DIRS");   TFFF_DIRS  ::SetDescription("Transmit FIFO Fill DMA or Interrupt Request Select");
			CMDTCF_RE  ::SetName("CMDTCF_RE");   CMDTCF_RE  ::SetDescription("Command Transmission Complete Request Enable");
			if(HAS_DATA_SERIALIZATION_SUPPORT) { DPEF_RE    ::SetName("DPEF_RE");     DPEF_RE    ::SetDescription("DSI Parity Error Request Enable"); }
			SPEF_RE    ::SetName("SPEF_RE");     SPEF_RE    ::SetDescription("SPI Parity Error Request Enable");
			if(HAS_DATA_SERIALIZATION_SUPPORT) { DDIF_RE    ::SetName("DDIF_RE");     DDIF_RE    ::SetDescription("DSI data received with active bits Request Enable"); }
			RFOF_RE    ::SetName("RFOF_RE");     RFOF_RE    ::SetDescription("Receive FIFO Overflow Request Enable");
			TFIWF_RE   ::SetName("TFIWF_RE");    TFIWF_RE   ::SetDescription("Transmit FIFO Invalid Write Request Enable");
			RFDF_RE    ::SetName("RFDF_RE");     RFDF_RE    ::SetDescription("Receive FIFO Drain Request Enable");
			RFDF_DIRS  ::SetName("RFDF_DIRS");   RFDF_DIRS  ::SetDescription("Receive FIFO Drain DMA or Interrupt Request Select");
			CMDFFF_DIRS::SetName("CMDFFF_DIRS"); CMDFFF_DIRS::SetDescription("Command FIFO Fill DMA or Interrupt Request Select");
			if(HAS_DATA_SERIALIZATION_SUPPORT) { DDIF_DIRS  ::SetName("DDIF_DIRS");   DDIF_DIRS  ::SetDescription("DSI data received with active bits - DMA or Interrupt Request Select"); }
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			uint32_t old_value = this->Get();
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(this->dspi->Running())
				{
					this->dspi->logger << DebugWarning << "do not write to " << this->GetName() << " while " << this->dspi->GetName() << " is in " << this->dspi->GetState() << EndDebugWarning;
				}
				
				uint32_t new_value = this->Get();
				
				uint32_t value_changed = new_value ^ old_value;
				
				if(TCF_RE   ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_TCF();
				
				if(CMDFFF_RE  ::template Get<uint32_t>(value_changed) ||
				   CMDFFF_DIRS::template Get<uint32_t>(value_changed))
				{
					this->dspi->UpdateINT_CMDFFF();
					this->dspi->UpdateDMA_CMD();
				}

				if(SPITCF_RE::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_SPITCF();
				if(EOQF_RE  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_EOQF();
				if(TFUF_RE  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_TFUF();   
				if(HAS_DATA_SERIALIZATION_SUPPORT && DSITCF_RE::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_DSITCF();
				
				if(TFFF_RE  ::template Get<uint32_t>(value_changed) ||
				   TFFF_DIRS::template Get<uint32_t>(value_changed))
				{
					this->dspi->UpdateINT_TFFF();
					this->dspi->UpdateDMA_TX();
				}
				
				if(CMDTCF_RE::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_CMDTCF();
				if(HAS_DATA_SERIALIZATION_SUPPORT && DPEF_RE  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_DPEF();
				if(SPEF_RE  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_SPEF();
				
				if(HAS_DATA_SERIALIZATION_SUPPORT &&
				   (DDIF_RE  ::template Get<uint32_t>(value_changed) ||
				    DDIF_DIRS::template Get<uint32_t>(value_changed)))
				{
					this->dspi->UpdateINT_DDIF();
					this->dspi->UpdateDMA_DD();
				}
				
				if(RFOF_RE  ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_RFOF();
				if(TFIWF_RE ::template Get<uint32_t>(value_changed)) this->dspi->UpdateINT_TFIWF();
				
				if(RFDF_RE  ::template Get<uint32_t>(value_changed) ||
				   RFDF_DIRS::template Get<uint32_t>(value_changed))
				{
					this->dspi->UpdateINT_RFDF();
					this->dspi->UpdateDMA_RX();
				}
			}
			
			return rws;
		}

		using Super::operator =;
	};
	
	// DSPI PUSH FIFO Register (In Master Mode) (DSPI_PUSHR)
	struct DSPI_PUSHR : DSPI_Register<DSPI_PUSHR, SW_RW>
	{
		typedef DSPI_Register<DSPI_PUSHR, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x34;
		
		struct CONT    : Field<DSPI_PUSHR, CONT   , 0     > {}; // Continuous Peripheral Chip Select Enable (SPI master mode)
		struct CTAS    : Field<DSPI_PUSHR, CTAS   , 1, 3  > {}; // Clock and Transfer Attributes Select
		struct EOQ     : Field<DSPI_PUSHR, EOQ    , 4     > {}; // End Of Queue
		struct CTCNT   : Field<DSPI_PUSHR, CTCNT  , 5     > {}; // Clear Transfer Counter
		struct PE_MASC : Field<DSPI_PUSHR, PE_MASC, 6     > {}; // Parity Enable or Mask tASC delay in the current frame
		struct PP_MCSC : Field<DSPI_PUSHR, PP_MCSC, 7     > {}; // Parity Polarity or Mask tCSC delay in the next frame
		struct PCS     : Field<DSPI_PUSHR, PCS    , 8, 15 > {}; // Select which PCS signals are to be asserted for the transfer
		struct CMD     : Field<DSPI_PUSHR, CMD    , 0, 15 > {}; // (16-bit) Command
		struct TXDATA  : Field<DSPI_PUSHR, TXDATA , 16, 31> {}; // (16-bit) Transmit Data
		
		struct CMD_MSB    : Field<DSPI_PUSHR, CMD_MSB   , 0, 7  > {}; // MSB of Command
		struct CMD_LSB    : Field<DSPI_PUSHR, CMD_LSB   , 8, 15 > {}; // LSB of Command
		struct TXDATA_MSB : Field<DSPI_PUSHR, TXDATA_MSB, 16, 23> {}; // MSB of Transmit Data
		struct TXDATA_LSB : Field<DSPI_PUSHR, TXDATA_LSB, 24, 31> {}; // LSB of Transmit Data
		
		typedef FieldSet<CONT, CTAS, EOQ, CTCNT, PE_MASC, PP_MCSC, PCS, TXDATA> ALL;
		
		DSPI_PUSHR(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_PUSHR(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_PUSHR"); this->SetDescription("DSPI PUSH FIFO Register (In Master Mode)");
			
			CONT   ::SetName("CONT");    CONT   ::SetDescription("Continuous Peripheral Chip Select Enable (SPI master mode)");
			CTAS   ::SetName("CTAS");    CTAS   ::SetDescription("Clock and Transfer Attributes Select");
			EOQ    ::SetName("EOQ");     EOQ    ::SetDescription("End Of Queue");
			CTCNT  ::SetName("CTCNT");   CTCNT  ::SetDescription("Clear Transfer Counter");
			PE_MASC::SetName("PE_MASC"); PE_MASC::SetDescription("Parity Enable or Mask tASC delay in the current frame");
			PP_MCSC::SetName("PP_MCSC"); PP_MCSC::SetDescription("Parity Polarity or Mask tCSC delay in the next frame");
			PCS    ::SetName("PCS");     PCS    ::SetDescription("Select which PCS signals are to be asserted for the transfer");
			TXDATA ::SetName("TXDATA");  TXDATA ::SetDescription("Transmit Data");
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				// mirror TXDATA in DSPI_PUSHR_SLAVE
				this->dspi->dspi_pushr_slave.template Set<typename DSPI_PUSHR_SLAVE::TXDATA>(this->template Get<TXDATA>());
				
				if(this->dspi->ExtendedMode())
				{
					if((bit_enable & CMD_MSB::template GetMask<uint32_t>()) == CMD_MSB::template GetMask<uint32_t>())
					{
						this->dspi->XSPI_Master_CMD_FIFO_Push(this->template Get<CMD>());
					}
					
					if((bit_enable & TXDATA_MSB::template GetMask<uint32_t>()) == TXDATA_MSB::template GetMask<uint32_t>())
					{
						this->dspi->XSPI_Master_TX_FIFO_Push(this->template Get<TXDATA>());
					}
				}
				else
				{
					if((bit_enable & TXDATA_MSB::template GetMask<uint32_t>()) == TXDATA_MSB::template GetMask<uint32_t>())
					{
						this->dspi->SPI_Master_TX_FIFO_Push(this->template Get<CMD>(), this->template Get<TXDATA>());
					}
				}
			}
			return rws;
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		using Super::operator =;
	};
	
	// DSPI PUSH FIFO Register (In Slave Mode) (DSPI_PUSHR_SLAVE)
	struct DSPI_PUSHR_SLAVE : DSPI_Register<DSPI_PUSHR_SLAVE, SW_RW>
	{
		typedef DSPI_Register<DSPI_PUSHR_SLAVE, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x34;
		
		struct TXDATA : Field<DSPI_PUSHR_SLAVE, TXDATA, 16, 31> {}; // Transmit Data
		
		typedef FieldSet<TXDATA> ALL;
		
		DSPI_PUSHR_SLAVE(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_PUSHR_SLAVE(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_PUSHR_SLAVE"); this->SetDescription("DSPI PUSH FIFO Register (In Slave Mode)");
			TXDATA::SetName("TXDATA"); TXDATA::SetDescription("Transmit Data");
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				// mirror TXDATA in DSPI_PUSHR
				this->dspi->dspi_pushr.template Set<typename DSPI_PUSHR::TXDATA>(this->template Get<TXDATA>());
				
				this->dspi->SPI_Slave_TX_FIFO_Push(this->template Get<TXDATA>());
			}
			return rws;
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}

		using Super::operator =;
	};
	
	// DSPI POP FIFO Register (DSPI_POPR)
	struct DSPI_POPR : DSPI_Register<DSPI_POPR, SW_R>
	{
		typedef DSPI_Register<DSPI_POPR, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x38;
		
		struct RXDATA : Field<DSPI_POPR, RXDATA, 0, 31, SW_R> {}; // Received Data
		
		typedef FieldSet<RXDATA> ALL;
		
		DSPI_POPR(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_POPR(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_POPR"); this->SetDescription("DSPI POP FIFO Register");
			RXDATA::SetName("RXDATA"); RXDATA::SetDescription("Received Data");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Read(uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Read(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(this->dspi->RX_FIFO_Empty())
				{
					this->dspi->logger << DebugWarning << sc_core::sc_time_stamp() << ": Attempts to pop data from an empty RX FIFO are ignored and the RX FIFO Counter remains unchanged; The data read from the empty RX FIFO is undetermined" << EndDebugWarning;
					value = (value & ~bit_enable) | (0xdeadbeef & bit_enable);
				}
				else
				{
					uint32_t rxdata = this->dspi->RX_FIFO_Front();
					if(this->dspi->verbose)
					{
						this->dspi->logger << DebugInfo << sc_core::sc_time_stamp() << ":RX FIFO:Front() = 0x" << std::hex << rxdata << std::dec << EndDebugInfo;
					}
					this->dspi->RX_FIFO_Pop();
					value = (value & ~bit_enable) | (rxdata & bit_enable);
				}
			}
			return rws;
		}

		using Super::operator =;
	};
	
	
	// DSPI Transmit FIFO Register (DSPI_TXFR)
	struct DSPI_TXFR : DSPI_Register<DSPI_TXFR, SW_R>
	{
		typedef DSPI_Register<DSPI_TXFR, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x3c;
		
		struct TXCMD_TXDATA : Field<DSPI_TXFR, TXCMD_TXDATA, 0 , 15, SW_R> {}; // Transmit Command or Transmit Data
		struct TXDATA       : Field<DSPI_TXFR, TXDATA      , 16, 31, SW_R> {}; // Transmit Data
		
		typedef FieldSet<TXCMD_TXDATA, TXDATA> ALL;
		
		DSPI_TXFR() : Super(0), reg_num(0) {}
		DSPI_TXFR(DSPI<CONFIG> *_dspi) : Super(_dspi), reg_num(0) {}
		DSPI_TXFR(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, DSPI<CONFIG> *_dspi)
		{
			this->dspi = _dspi;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "DSPI_TXFR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "DSPI Transmit FIFO Register " << reg_num;
			this->SetDescription(description_sstr.str());
		
			TXCMD_TXDATA::SetName("TXCMD_TXDATA"); TXCMD_TXDATA::SetDescription("Transmit Command or Transmit Data");
			TXDATA      ::SetName("TXDATA");       TXDATA      ::SetDescription("Transmit Data");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// DSPI Receive FIFO Register (DSPI_RXFR)
	struct DSPI_RXFR : DSPI_Register<DSPI_RXFR, SW_R>
	{
		typedef DSPI_Register<DSPI_RXFR, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x7c;
		
		struct RXDATA : Field<DSPI_RXFR, RXDATA, 0, 31, SW_R> {}; // Receive Data
		
		typedef FieldSet<RXDATA> ALL;
		
		DSPI_RXFR() : Super(0), reg_num(0) {}
		DSPI_RXFR(DSPI<CONFIG> *_dspi) : Super(_dspi), reg_num(0) {}
		DSPI_RXFR(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, DSPI<CONFIG> *_dspi)
		{
			this->dspi = _dspi;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "DSPI_RXFR" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "DSPI Receive FIFO Register " << reg_num;
			this->SetDescription(description_sstr.str());
		
			RXDATA::SetName("RXDATA"); RXDATA::SetDescription("Receive Data");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Read(uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Read(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(this->dspi->ModuleDisabled())
				{
					this->dspi->logger << DebugWarning << "module must not be disabled when " << this->GetName() << " is read" << EndDebugWarning;
				}
			}
			
			return rws;
		}

		using Super::operator =;
	private:
		unsigned int reg_num;
	};
	
	// DSPI DSI Configuration Register 0 (DSPI_DSICR0)
	struct DSPI_DSICR0 : DSPI_Register<DSPI_DSICR0, SW_RW>
	{
		typedef DSPI_Register<DSPI_DSICR0, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xbc;
		
		struct Reserved1 : Field<DSPI_DSICR0, Reserved1, 0           > {}; // This field is reserved
		struct FMSZ4     : Field<DSPI_DSICR0, FMSZ4    , 1           > {}; // MSB of the frame size in master mode when DSI is used in 32-bit mode
		struct Reserved2 : Field<DSPI_DSICR0, Reserved2, 2, 7        > {}; // This field is reserved
		struct Reserved3 : Field<DSPI_DSICR0, Reserved3, 8, 8, SW_R  > {}; // This field is reserved
		struct Reserved4 : Field<DSPI_DSICR0, Reserved4, 9           > {}; // This field is reserved
		struct ITSB      : Field<DSPI_DSICR0, ITSB     , 10          > {}; // Interleaved TSB mode
		struct TSBC      : Field<DSPI_DSICR0, TSBC     , 11          > {}; // Timed Serial Bus Configuration
		struct TXSS      : Field<DSPI_DSICR0, TXSS     , 12          > {}; // Transmit Data Source Select
		struct TPOL      : Field<DSPI_DSICR0, TPOL     , 13          > {}; // Trigger Polarity
		struct TRRE      : Field<DSPI_DSICR0, TRRE     , 14, 14, SW_R> {}; // Trigger Reception Enable
		struct CID       : Field<DSPI_DSICR0, CID      , 15          > {}; // Change In Data Transfer Enable
		struct DCONT     : Field<DSPI_DSICR0, DCONT    , 16          > {}; // DSI Continuous Peripheral Chip Select Enable
		struct DSICTAS   : Field<DSPI_DSICR0, DSICTAS  , 17, 19      > {}; // DSI Clock and Transfer Attributes Select (DSI Master mode only)
		struct DMS       : Field<DSPI_DSICR0, DMS      , 20          > {}; // Data Match Stop
		struct PES       : Field<DSPI_DSICR0, PES      , 21          > {}; // Parity Error Stop
		struct PE        : Field<DSPI_DSICR0, PE       , 22          > {}; // Parity Enable
		struct PP        : Field<DSPI_DSICR0, PP       , 23          > {}; // Parity Polarity
		struct DPCSx     : Field<DSPI_DSICR0, DPCSx    , 24, 31      > {}; // DSI Peripheral Chip Select 0-7
		
		typedef FieldSet<Reserved1, FMSZ4, Reserved2, Reserved3, Reserved4, ITSB, TSBC, TXSS, TPOL, TRRE, CID, DCONT, DSICTAS, DMS, PES, PE, PP, DPCSx> ALL;
		
		DSPI_DSICR0(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_DSICR0(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_DSICR0"); this->SetDescription("DSPI DSI Configuration Register 0");
			
			Reserved1::SetName("Reserved1"); Reserved1::SetDescription("This field is reserved");
			FMSZ4    ::SetName("FMSZ4");     FMSZ4    ::SetDescription("MSB of the frame size in master mode when DSI is used in 32-bit mode");
			Reserved2::SetName("Reserved2"); Reserved2::SetDescription("This field is reserved");
			Reserved3::SetName("Reserved3"); Reserved3::SetDescription("This field is reserved");
			Reserved4::SetName("Reserved4"); Reserved4::SetDescription("This field is reserved");
			ITSB     ::SetName("ITSB");      ITSB     ::SetDescription("Interleaved TSB mode");
			TSBC     ::SetName("TSBC");      TSBC     ::SetDescription("Timed Serial Bus Configuration");
			TXSS     ::SetName("TXSS");      TXSS     ::SetDescription("Transmit Data Source Select");
			TPOL     ::SetName("TPOL");      TPOL     ::SetDescription("Trigger Polarity");
			TRRE     ::SetName("TRRE");      TRRE     ::SetDescription("Trigger Reception Enable");
			CID      ::SetName("CID");       CID      ::SetDescription("Change In Data Transfer Enable");
			DCONT    ::SetName("DCONT");     DCONT    ::SetDescription("DSI Continuous Peripheral Chip Select Enable");
			DSICTAS  ::SetName("DSICTAS");   DSICTAS  ::SetDescription("DSI Clock and Transfer Attributes Select (DSI Master mode only)");
			DMS      ::SetName("DMS");       DMS      ::SetDescription("Data Match Stop");
			PES      ::SetName("PES");       PES      ::SetDescription("Parity Error Stop");
			PE       ::SetName("PE");        PE       ::SetDescription("Parity Enable");
			PP       ::SetName("PP");        PP       ::SetDescription("Parity Polarity");
			DPCSx    ::SetName("DPCSx");     DPCSx    ::SetDescription("DSI Peripheral Chip Select 0-7");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				if(this->dspi->Running())
				{
					this->dspi->logger << DebugWarning << "do not write to " << this->GetName() << " while " << this->dspi->GetName() << " is in " << this->dspi->GetState() << EndDebugWarning;
				}
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// DSPI DSI Serialization Data Register 0 (DSPI_SDR0)
	struct DSPI_SDR0 : DSPI_Register<DSPI_SDR0, SW_R>
	{
		typedef DSPI_Register<DSPI_SDR0, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc0;
		
		struct SER_DATA : Field<DSPI_SDR0, SER_DATA, 0, 31, SW_R> {}; // Serialized Data
		
		typedef FieldSet<SER_DATA> ALL;
		
		DSPI_SDR0(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_SDR0(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_SDR0"); this->SetDescription("DSPI DSI Serialization Data Register 0");
			SER_DATA::SetName("SER_DATA"); SER_DATA::SetDescription("Serialized Data");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// DSPI DSI Alternate Serialization Data Register 0 (DSPI_ASDR0)
	struct DSPI_ASDR0 : DSPI_Register<DSPI_ASDR0, SW_RW>
	{
		typedef DSPI_Register<DSPI_ASDR0, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc4;
		
		struct ASER_DATA : Field<DSPI_ASDR0, ASER_DATA, 0, 31> {}; // Alternate Serialized Data
		
		typedef FieldSet<ASER_DATA> ALL;
		
		DSPI_ASDR0(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_ASDR0(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_ASDR0"); this->SetDescription("DSPI DSI Alternate Serialization Data Register 0");
			ASER_DATA::SetName("ASER_DATA"); ASER_DATA::SetDescription("Alternate Serialized Data");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		virtual ReadWriteStatus Write(const uint32_t& value, const uint32_t& bit_enable)
		{
			ReadWriteStatus rws = Super::Write(value, bit_enable);
			
			if(!IsReadWriteError(rws))
			{
				this->dspi->ScheduleLatchSerializedData();
			}
			
			return rws;
		}
		
		using Super::operator =;
	};
	
	// DSPI DSI Transmit Comparison Register 0 (DSPI_COMPR0)
	struct DSPI_COMPR0 : DSPI_Register<DSPI_COMPR0, SW_R>
	{
		typedef DSPI_Register<DSPI_COMPR0, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xc8;
		
		struct COMP_DATA : Field<DSPI_COMPR0, COMP_DATA, 0, 31, SW_R> {}; // Compare Data
		
		typedef FieldSet<COMP_DATA> ALL;
		
		DSPI_COMPR0(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_COMPR0(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_COMPR0"); this->SetDescription("DSPI DSI Transmit Comparison Register 0");
			COMP_DATA::SetName("COMP_DATA"); COMP_DATA::SetDescription("Compare Data");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// DSPI DSI Deserialization Data Register 0 (DSPI_DDR0)
	struct DSPI_DDR0 : DSPI_Register<DSPI_DDR0, SW_R>
	{
		typedef DSPI_Register<DSPI_DDR0, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xcc;
		
		struct DESER_DATA : Field<DSPI_DDR0, DESER_DATA, 0, 31, SW_R> {}; // Deserialized Data
		
		typedef FieldSet<DESER_DATA> ALL;
		
		DSPI_DDR0(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_DDR0(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_DDR0"); this->SetDescription("DSPI DSI Deserialization Data Register 0");
			DESER_DATA::SetName("DESER_DATA"); DESER_DATA::SetDescription("Deserialized Data");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// DSPI DSI Configuration Register 1 (DSPI_DSICR1)
	struct DSPI_DSICR1 : DSPI_Register<DSPI_DSICR1, SW_RW>
	{
		typedef DSPI_Register<DSPI_DSICR1, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xd0;
		
		struct TSBCNT  : Field<DSPI_DSICR1, TSBCNT , 3, 7  > {}; // Timed Serial Bus Operation Count
		struct DSE1    : Field<DSPI_DSICR1, DSE1   , 14    > {}; // Data Select Enable 1
		struct DSE0    : Field<DSPI_DSICR1, DSE0   , 15    > {}; // Data Select Enable 0
		struct TRGPRD  : Field<DSPI_DSICR1, TRGPRD , 16, 23> {}; // Internal Trigger Period for the ITSB mode
		struct DPCS1_x : Field<DSPI_DSICR1, DPCS1_x, 24, 31> {}; // DSI Peripheral Chip Select 0-7
		
		typedef FieldSet<TSBCNT, DSE1, DSE0, TRGPRD, DPCS1_x> ALL;
		
		DSPI_DSICR1(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_DSICR1(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_DSICR1"); this->SetDescription("DSPI DSI Configuration Register 1");
			
			TSBCNT ::SetName("TSBCNT");  TSBCNT ::SetDescription("Timed Serial Bus Operation Count");
			DSE1   ::SetName("DSE1");    DSE1   ::SetDescription("Data Select Enable 1");
			DSE0   ::SetName("DSE0");    DSE0   ::SetDescription("Data Select Enable 0");
			TRGPRD ::SetName("TRGPRD");  TRGPRD ::SetDescription("Internal Trigger Period for the ITSB mode");
			DPCS1_x::SetName("DPCS1_x"); DPCS1_x::SetDescription("DSI Peripheral Chip Select 0-7");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// DSPI DSI Serialization Source Select Register 0 (DSPI_SSR0)
	struct DSPI_SSR0 : DSPI_Register<DSPI_SSR0, SW_RW>
	{
		typedef DSPI_Register<DSPI_SSR0, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xd4;
		
		struct SS : Field<DSPI_SSR0, SS, 0, 31> {}; // Source Select
		
		typedef FieldSet<SS> ALL;
		
		DSPI_SSR0(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_SSR0(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_SSR0"); this->SetDescription("DSPI DSI Serialization Source Select Register 0");
			
			SS::SetName("SS"); SS::SetDescription("Source Select");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// DSPI DSI Deserialized Data Interrupt Mask Register 0 (DSPI_DIMR0)
	struct DSPI_DIMR0 : DSPI_Register<DSPI_DIMR0, SW_RW>
	{
		typedef DSPI_Register<DSPI_DIMR0, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xe8;
		
		struct MASK : Field<DSPI_DIMR0, MASK, 0, 31> {}; // Mask
		
		typedef FieldSet<MASK> ALL;
		
		DSPI_DIMR0(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_DIMR0(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_DIMR0"); this->SetDescription("DSPI DSI Deserialized Data Interrupt Mask Register 0");
			MASK::SetName("MASK"); MASK::SetDescription("Mask");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// DSPI DSI Deserialized Data Polarity Interrupt Register 0 (DSPI_DPIR0)
	struct DSPI_DPIR0 : DSPI_Register<DSPI_DPIR0, SW_RW>
	{
		typedef DSPI_Register<DSPI_DPIR0, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0xec;
		
		struct DP : Field<DSPI_DPIR0, DP, 0, 31> {}; // Data Polarity
		
		typedef FieldSet<DP> ALL;
		
		DSPI_DPIR0(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_DPIR0(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_DPIR0"); this->SetDescription("DSPI DSI Deserialized Data Polarity Interrupt Register 0");
			DP::SetName("DP"); DP::SetDescription("Data Polarity");
		}
		
		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	// DSPI Clock and Transfer Attributes Register Extended (DSPI_CTARE)
	struct DSPI_CTARE : DSPI_Register<DSPI_CTARE, SW_RW>
	{
		typedef DSPI_Register<DSPI_CTARE, SW_RW> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x11c;
		
		struct FMSZE : Field<DSPI_CTARE, FMSZE, 15    > {}; // Frame Size Extended
		struct DTCP  : Field<DSPI_CTARE, DTCP , 21, 31> {}; // Data Transfer Count Preload
		
		typedef FieldSet<FMSZE, DTCP> ALL;
		
		DSPI_CTARE() : Super(0), reg_num(0) {}
		DSPI_CTARE(DSPI<CONFIG> *_dspi) : Super(_dspi), reg_num(0) {}
		DSPI_CTARE(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value), reg_num(0) {}
		
		void WithinRegisterFileCtor(unsigned int _reg_num, DSPI<CONFIG> *_dspi)
		{
			this->dspi = _dspi;
			reg_num = _reg_num;
			
			std::stringstream name_sstr;
			name_sstr << "DSPI_CTARE" << reg_num;
			this->SetName(name_sstr.str());
			std::stringstream description_sstr;
			description_sstr << "DSPI Clock and Transfer Attributes Register Extended " << reg_num;
			this->SetDescription(description_sstr.str());
			
			FMSZE::SetName("FMSZE"); FMSZE::SetDescription("Frame Size Extended");
			DTCP ::SetName("DTCP");  DTCP ::SetDescription("Data Transfer Count Preload");
		}
		
		void Reset()
		{
			this->Initialize(0x00000001);
		}
		
		using Super::operator =;
	private:
		unsigned int reg_num;
	};

	// DSPI Status Register Extended (DSPI_SREX)
	struct DSPI_SREX : DSPI_Register<DSPI_SREX, SW_R>
	{
		typedef DSPI_Register<DSPI_SREX, SW_R> Super;
		
		static const sc_dt::uint64 ADDRESS_OFFSET = 0x13c;
		
		struct CMDCTR    : Field<DSPI_SREX, CMDCTR   , 24, 27, SW_R> {}; // CMD FIFO Counter
		struct CMDNXTPTR : Field<DSPI_SREX, CMDNXTPTR, 28, 31, SW_R> {}; // Command Next Pointer
		
		typedef FieldSet<CMDCTR, CMDNXTPTR> ALL;
		
		DSPI_SREX(DSPI<CONFIG> *_dspi) : Super(_dspi) { Init(); }
		DSPI_SREX(DSPI<CONFIG> *_dspi, uint32_t value) : Super(_dspi, value) { Init(); }
		
		void Init()
		{
			this->SetName("DSPI_SREX"); this->SetDescription("DSPI Status Register Extended");
			CMDCTR   ::SetName("CMDCTR");    CMDCTR   ::SetDescription("CMD FIFO Counter");
			CMDNXTPTR::SetName("CMDNXTPTR"); CMDNXTPTR::SetDescription("Command Next Pointer");
		}
		
		virtual ReadWriteStatus Read(uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->dspi->ExtendedMode())
			{
				return Super::Read(value, bit_enable);
			}
			else
			{
				// When not in extended mode, SREX[CMDCTR]=SR[TXCTR] and SREX[CMDNXTPTR]=SR[TXNXTPTR]
				uint32_t _value = 0;
				CMDCTR::Set(_value, this->dspi->dspi_sr.template Get<typename DSPI_SR::TXCTR>());
				CMDNXTPTR::Set(_value, this->dspi->dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>());
				value = (value & ~bit_enable) || (_value & bit_enable);
				
				return RWS_OK;
			}
		}
		
		virtual void DebugRead(uint32_t& value, const uint32_t& bit_enable)
		{
			if(this->dspi->ExtendedMode())
			{
				Super::DebugRead(value, bit_enable);
			}
			else
			{
				// When not in extended mode, SREX[CMDCTR]=SR[TXCTR] and SREX[CMDNXTPTR]=SR[TXNXTPTR]
				uint32_t _value = 0;
				CMDCTR::Set(_value, this->dspi->dspi_sr.template Get<typename DSPI_SR::TXCTR>());
				CMDNXTPTR::Set(_value, this->dspi->dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>());
				value = (value & ~bit_enable) || (_value & bit_enable);
			}
		}

		void Reset()
		{
			this->Initialize(0x0);
		}
		
		using Super::operator =;
	};
	
	struct TransferControl
	{
		bool cont;               // continuous peripheral chip select    (Master only)
		bool masc;               // mask tASC delay in the current frame (Master only)
		bool mcsc;               // mask tCSC delay in the current frame (Master only)
		unsigned int pcs;        // peripheral chip select               (Master only)
		unsigned int ctas;       // clock and transfer attribute select  (Master/Slave)
		bool lsbfe;              // LSB First                            (Master/Slave)
		bool pe;                 // parity enable                        (Master/Slave)
		bool cpha;               // clock phase                          (Master/Slave)
		unsigned int frame_size; // frame size                           (Master/Slave)
	};
	
	unisim::kernel::tlm2::ClockPropertiesProxy m_clk_prop_proxy;    // proxy to get clock properties from master clock port
	unisim::kernel::tlm2::ClockPropertiesProxy dspi_clk_prop_proxy; // proxy to get clock properties from DSPI clock port

	// DSPI registers
	DSPI_MCR                                                                          dspi_mcr;
	DSPI_TCR                                                                          dspi_tcr;
	AddressableRegisterFile<DSPI_CTAR, NUM_CTARS, DSPI<CONFIG> >                      dspi_ctar;
	AddressableRegisterFile<DSPI_CTAR_SLAVE, NUM_SLAVE_CONFIGURATIONS, DSPI<CONFIG> > dspi_ctar_slave;
	DSPI_SR                                                                           dspi_sr;
	DSPI_RSER                                                                         dspi_rser;
	DSPI_PUSHR                                                                        dspi_pushr;
	DSPI_PUSHR_SLAVE                                                                  dspi_pushr_slave;
	DSPI_POPR                                                                         dspi_popr;
	AddressableRegisterFile<DSPI_TXFR, TX_FIFO_DEPTH, DSPI<CONFIG> >                  dspi_txfr;
	AddressableRegisterFile<DSPI_RXFR, RX_FIFO_DEPTH, DSPI<CONFIG> >                  dspi_rxfr;
	DSPI_DSICR0                                                                       dspi_dsicr0;
	DSPI_SDR0                                                                         dspi_sdr0;
	DSPI_ASDR0                                                                        dspi_asdr0;
	DSPI_COMPR0                                                                       dspi_compr0;
	DSPI_DDR0                                                                         dspi_ddr0;
	DSPI_DSICR1                                                                       dspi_dsicr1;
	DSPI_SSR0                                                                         dspi_ssr0;
	DSPI_DIMR0                                                                        dspi_dimr0;
	DSPI_DPIR0                                                                        dspi_dpir0;
	AddressableRegisterFile<DSPI_CTARE, NUM_CTARS, DSPI<CONFIG> >                     dspi_ctare;
	DSPI_SREX                                                                         dspi_srex;
	
// 	unsigned int txbackptr;   // TX FIFO back pointer
// 	unsigned int rxbackptr;   // RX FIFO back pointer
// 	unsigned int cmdbackptr;  // CMD FIFO back pointer
	unsigned int rxpart;      // part of RX data for slave (0=LSB or 1=MSB)
	DSPI_Configuration prev_dconf;
	DSPI_Configuration curr_dconf;
	uint16_t prev_cmd;
	uint16_t cmd;
	unsigned int cmd_cycling_cnt;
	bool tx_parity_bit;
	bool rx_parity_bit;
	uint32_t serialized_data;
	//bool trigger_spi_transfer;
	bool trigger_dsi_transfer;
	
	unisim::kernel::tlm2::tlm_input_bitstream sin_bitstream; // Rx timed input bit stream
	unisim::kernel::tlm2::tlm_input_bitstream ss_bitstream;
	sc_core::sc_event transfer_event;
	sc_core::sc_time transfer_ready_time;
	
	sc_core::sc_event gen_int_eoqf_event;
	sc_core::sc_event gen_int_tfff_event;
	sc_core::sc_event gen_int_cmdfff_event;
	sc_core::sc_event gen_int_tfiwf_event;
	sc_core::sc_event gen_int_tcf_event;
	sc_core::sc_event gen_int_cmdtcf_event;
	sc_core::sc_event gen_int_spitcf_event;
	sc_core::sc_event gen_int_dsitcf_event;
	sc_core::sc_event gen_int_tfuf_event;
	sc_core::sc_event gen_int_rfdf_event;
	sc_core::sc_event gen_int_rfof_event;
	sc_core::sc_event gen_int_spef_event;
	sc_core::sc_event gen_int_dpef_event;
	sc_core::sc_event gen_int_ddif_event;
	sc_core::sc_event gen_dma_rx_event;
	sc_core::sc_event gen_dma_tx_event;
	sc_core::sc_event gen_dma_cmd_event;
	sc_core::sc_event gen_dma_dd_event;
	sc_core::sc_event gen_tfff_event;
	sc_core::sc_event gen_cmdfff_event;
	sc_core::sc_event gen_rfdf_event;
	
	sc_core::sc_vector<sc_core::sc_event> dsi_input_sample_event;
	sc_core::sc_event latch_serialized_data_event;
	
	// DSPI registers address map
	RegisterAddressMap<sc_dt::uint64> reg_addr_map;

	unisim::util::debug::SimpleRegisterRegistry registers_registry;

	unisim::kernel::tlm2::Schedule<Event> schedule;         // Payload (processor requests over AHB interface) schedule
	
	unisim::util::endian::endian_type endian;
	unisim::kernel::variable::Parameter<unisim::util::endian::endian_type> param_endian;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;

	sc_core::sc_time master_clock_period;                 // Master clock period
	sc_core::sc_time master_clock_start_time;             // Master clock start time
	bool master_clock_posedge_first;                      // Master clock posedge first ?
	double master_clock_duty_cycle;                       // Master clock duty cycle

	sc_core::sc_time dspi_clock_period;                    // DSPI clock period
	sc_core::sc_time dspi_clock_start_time;                // DSPI clock start time
	bool dspi_clock_posedge_first;                         // DSPI clock posedge first ?
	double dspi_clock_duty_cycle;                          // DSPI clock duty cycle
	
	void Reset();
	void ModeSwitch();
	void MapRegisters();
	void ExportRegisters();
	bool Running() const;
	bool Stopped() const;
	DSPI_State GetState() const;
	void UpdateState();
	bool ModuleDisabled() const;
	bool MasterMode() const;
	bool SlaveMode() const;
	bool ExtendedMode() const;
	bool SPI_FrameTransmissionsStopped() const;
	bool DSI_OperationsStopped() const;
	bool DSI_FrameTransmissionsStopped() const;
	DSPI_Configuration Configuration() const;
	unsigned int MasterFrameSize(DSPI_Configuration dconf, unsigned int ctar_num) const;
	unsigned int SlaveFrameSize(DSPI_Configuration dconf, unsigned int ctar_num) const;

	unsigned int TX_FIFO_Depth() const;
	unsigned int CMD_FIFO_Depth() const;
	unsigned int RX_FIFO_Depth() const;
	
	// TX FIFO
	void SPI_Master_TX_FIFO_Push(uint16_t txcmd, uint16_t txdata);
	void XSPI_Master_TX_FIFO_Push(uint16_t txdata);
	void XSPI_Master_CMD_FIFO_Push(uint16_t txcmd);
	void SPI_Slave_TX_FIFO_Push(uint16_t txdata);
	void TX_FIFO_Clear();
	unsigned int TX_FIFO_Size() const;
	bool TX_FIFO_Empty() const;
	bool CMD_FIFO_Empty() const;
	bool TX_FIFO_Full() const;
	bool CMD_FIFO_Full() const;
	uint16_t SPI_Master_TX_FIFO_Front() const;
	uint16_t SPI_Master_CMD_FIFO_Front() const;
	uint32_t SPI_Slave_TX_FIFO_Front() const;
	void SPI_Master_TX_FIFO_Pop();
	void XSPI_Master_CMD_FIFO_Pop();
	void SPI_Slave_TX_FIFO_Pop();
	void TX_FIFO_Dump();
	void CMD_FIFO_Dump();
	
	// RX FIFO
	void RX_FIFO_Clear();
	bool RX_FIFO_Empty() const;
	bool RX_FIFO_Full() const;
	void RX_FIFO_Push(uint32_t rxdata);
	uint32_t RX_FIFO_Front();
	void RX_FIFO_Pop();
	void RX_FIFO_Dump();

	
	void Set_TFFF_If_TX_FIFO_Not_Full();
	void Set_CMDFFF_If_CMD_FIFO_Not_Full();
	void Set_RFDF_If_RX_FIFO_Not_Empty();
	
	void UpdateINT_EOQF();
	void UpdateINT_TFFF();  
	void UpdateINT_CMDFFF();
	void UpdateINT_TFIWF();
	void UpdateINT_TCF();   
	void UpdateINT_CMDTCF();
	void UpdateINT_SPITCF();
	void UpdateINT_DSITCF();
	void UpdateINT_TFUF();
	void UpdateINT_RFDF();
	void UpdateINT_RFOF();
	void UpdateINT_SPEF();
	void UpdateINT_DPEF();
	void UpdateINT_DDIF();
	void UpdateDMA_RX(const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	void UpdateDMA_TX(const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	void UpdateDMA_CMD(const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	void UpdateDMA_DD(const sc_core::sc_time& delay = sc_core::SC_ZERO_TIME);
	
	void UpdateBSYF();
	
	void UpdateMasterClock();
	void UpdateDSPIClock();
	void MasterClockPropertiesChangedProcess();
	void DSPIClockPropertiesChangedProcess();
	
	void ProcessEvent(Event *event);
	void ProcessEvents();
	void Process();
	void INT_EOQF_Process();  
	void INT_TFFF_Process();  
	void INT_CMDFFF_Process();
	void INT_TFIWF_Process(); 
	void INT_TCF_Process();
	void INT_CMDTCF_Process();
	void INT_SPITCF_Process();
	void INT_DSITCF_Process();
	void INT_TFUF_Process();  
	void INT_RFDF_Process();
	void INT_RFOF_Process();
	void INT_SPEF_Process();
	void INT_DPEF_Process();
	void INT_DDIF_Process();
	void DMA_RX_Process();
	void DMA_TX_Process();
	void DMA_CMD_Process();
	void DMA_DD_Process();
	void DMA_ACK_RX_Process();
	void DMA_ACK_TX_Process();
	void DMA_ACK_CMD_Process();
	void DMA_ACK_DD_Process();
	void Set_TFFF_Process();
	void Set_CMDFFF_Process();
	void Set_RFDF_Process();
	void HT_Process();
	void DEBUG_Process();
	void SetPCS(unsigned int slave_num, const sc_core::sc_time& t, const sc_core::sc_time& duration, bool value);
	bool SPI_TransferTriggered() const;
	void DSI_TriggerTransfer();
	void ScheduleTransfer();
	void LatchSerializedData();
	void CheckChangeInData();
	void CheckDeserializedData();
	void LatchDSI_INPUT();
	void MasterTransfer(uint32_t& rxdata, const uint32_t& txdata, bool& parity_error, const TransferControl& ctrl);
	bool SelectConfiguration(DSPI_Configuration& sel_dconf) const;
	void SPI_MasterTransfer();
	void DSI_MasterTransfer();
	void SlaveTransfer(uint32_t& rxdata, const uint32_t& txdata, bool& parity_error, const TransferControl& ctrl);
	void SPI_SlaveTransfer();
	void DSI_SlaveTransfer();
	void MasterTransferProcess();
	void SlaveTransferProcess();
	void DSI_INPUT_Process(unsigned int i);
	void DSI_INPUT_SamplingProcess(unsigned int i);
	void ScheduleLatchSerializedData();
	void LatchSerializedDataProcess();
};

} // end of namespace dspi
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_DSPI_DSPI_HH__
