/*
 *  Copyright (c) 2016,
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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_HH__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/tlm2/tlm.hh>
#include <unisim/kernel/tlm2/tlm_spi.hh>
#include <unisim/util/reg/dev/register.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/component/tlm2/memory/flash/n25q/n25q_defs.hh>
#include <fstream>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

using unisim::util::reg::dev::Register;
using unisim::util::reg::dev::BitField;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::reg::dev::ACC_NONE;
using unisim::util::reg::dev::ACC_RO;
using unisim::util::reg::dev::ACC_WO;
using unisim::util::reg::dev::ACC_RW;

using unisim::kernel::tlm2::TLM_SPI_HALF_DUPLEX;
using unisim::kernel::tlm2::TLM_SPI_OK;
using unisim::kernel::tlm2::TLM_SPI_ERROR;

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
  using namespace sc_core;

enum WrapMode
{
	WRAP_16B,
	WRAP_32B,
	WRAP_64B,
	WRAP_SEQ
};

template <typename CONFIG>
class N25Q
	: public unisim::kernel::Object
	, public sc_core::sc_module
	, public unisim::kernel::tlm2::tlm_spi_fw_if<unisim::kernel::tlm2::tlm_spi_protocol_types>
{
public:
	static const bool threaded_model = false;
	static const unsigned int BUSWIDTH = 4;

	typedef unisim::kernel::tlm2::tlm_spi_protocol_types::tlm_payload_type payload_type;
	typedef unisim::kernel::tlm2::tlm_spi_slave_socket<BUSWIDTH> socket_type;
	
	socket_type qspi_slave_socket;
	sc_core::sc_in<bool> HOLD_RESET_N; // shared port: HOLD# or RESET#
	sc_core::sc_in<bool> W_N_VPP;      // shared port: W# or WPP
	
	N25Q(const sc_core::sc_module_name& name, unisim::kernel::Object *parent = 0);
	virtual ~N25Q();

	virtual unisim::kernel::tlm2::tlm_spi_sync_enum spi_nb_send(payload_type& payload);
	virtual void spi_b_send(payload_type& payload);
	
	void Reset();

protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	
private:
	struct StatusRegister : Register<8, ACC_RW, ACC_RW, E_LITTLE_ENDIAN>
	{
		typedef Register<8, ACC_RW, ACC_RW, E_LITTLE_ENDIAN> BaseType;
		
		BitField<StatusRegister, ACC_RW, ACC_RW, 7>    Status_register_write_enable_disable;  // nonvolatile
		BitField<StatusRegister, ACC_RW, ACC_RW, 5>    Top_bottom;                            // nonvolatile
		BitField<StatusRegister, ACC_RW, ACC_RW, 4, 2> Block_protect_2_0;                     // nonvolatile
		BitField<StatusRegister, ACC_RW, ACC_RO, 1>    Write_enable_latch;                    // volatile
		BitField<StatusRegister, ACC_RW, ACC_RO, 0>    Write_in_progress;                     // volatile

		StatusRegister(N25Q<CONFIG> *_n25q, const std::string& name, const std::string& long_name)
			: BaseType(std::string(_n25q->GetName()) + name, long_name, 0xfc)
			, Status_register_write_enable_disable(this, "Status_register_write_enable_disable", 0x1)
			, Top_bottom(this, "Top_bottom", 0x1)
			, Block_protect_2_0(this, "Block_protect_2_0", 0x7)
			, Write_enable_latch(this, "Write_enable_latch", 0x0)
			, Write_in_progress(this, "Write_in_progress", 0x0)
			, n25q(_n25q)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		
		virtual bool Write(const STORAGE_TYPE& value, const STORAGE_TYPE& byte_enable = STORAGE_TYPE(-1))
		{
			if(!BaseType::Write(value)) return false;
			
			if(Status_register_write_enable_disable && !n25q->W_N_VPP) // When the enable/disable bit is set to 1 and W#/VPP signal is driven LOW
			{
				// nonvolatile bits become read-only for SW
				SetSW_WriteMask(
					Write_enable_latch.SW_WriteMask() |
					Write_in_progress.SW_WriteMask()
				);
			}
			else
			{
				SetSW_WriteMask(
					Status_register_write_enable_disable.SW_WriteMask() |
					Top_bottom.SW_WriteMask() |
					Block_protect_2_0.SW_WriteMask() |
					Write_enable_latch.SW_WriteMask() |
					Write_in_progress.SW_WriteMask()
				);
			}
			
			return true;
		}
		
		StatusRegister& operator = (const STORAGE_TYPE& value)
		{
			bool old_write_in_progress = Write_in_progress;
			
			BaseType::operator = (value);
			
			bool new_write_in_progress = Write_in_progress;
			
			if(!old_write_in_progress && new_write_in_progress)
			{
				if(n25q->verbose)
				{
					n25q->logger << DebugInfo << sc_core::sc_time_stamp() << ": Write in progress" << EndDebugInfo;
				}
			}
			
			if(old_write_in_progress && !new_write_in_progress)
			{
				if(n25q->verbose)
				{
					n25q->logger << DebugInfo << sc_core::sc_time_stamp() << ": No write in progress" << EndDebugInfo;
				}
			}

			return *this;
		}
			
	private:
		N25Q<CONFIG> *n25q;
	};
	
	struct NonVolatileConfigurationRegister : Register<16, ACC_RW, ACC_RW, E_LITTLE_ENDIAN>
	{
		typedef Register<16, ACC_RW, ACC_RW, E_LITTLE_ENDIAN> BaseType;
		
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW, 15, 12> Number_of_dummy_clock_cycles;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW, 11,  9> XIP_mode_at_power_on_reset;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW,  8,  6> Output_driver_strength;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW,  4>     Reset_hold;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW,  3>     Quad_IO_protocol;
		BitField<NonVolatileConfigurationRegister, ACC_RW, ACC_RW,  2>     Dual_IO_protocol;
		
		NonVolatileConfigurationRegister(N25Q<CONFIG> *_n25q, const std::string& name, const std::string& long_name)
			: BaseType(std::string(_n25q->GetName()) + name, long_name, 0xffff)
			, Number_of_dummy_clock_cycles(this, "Number_of_dummy_clock_cycles", 0xf)
			, XIP_mode_at_power_on_reset(this, "XIP_mode_at_power_on_reset", 0x7)
			, Output_driver_strength(this, "Output_driver_strength", 0x7)
			, Reset_hold(this, "Reset_hold", 0x1)
			, Quad_IO_protocol(this, "Quad_IO_protocol", 0x1)
			, Dual_IO_protocol(this, "Dual_IO_protocol", 0x1)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	};
	
	struct VolatileConfigurationRegister : Register<8, ACC_RW, ACC_RW, E_LITTLE_ENDIAN>
	{
		typedef Register<8, ACC_RW, ACC_RW, E_LITTLE_ENDIAN> BaseType;
		
		BitField<VolatileConfigurationRegister, ACC_RW, ACC_RW, 7, 4> Number_of_dummy_clock_cycles;
		BitField<VolatileConfigurationRegister, ACC_RW, ACC_RW, 3>    XIP;
		BitField<VolatileConfigurationRegister, ACC_RW, ACC_RW, 1, 0> Wrap;
		
		VolatileConfigurationRegister(N25Q<CONFIG> *_n25q, const std::string& name, const std::string& long_name)
			: BaseType(std::string(_n25q->GetName()) + name, long_name, 0x0)
			, Number_of_dummy_clock_cycles(this, "Number_of_dummy_clock_cycles", 0xf)
			, XIP(this, "XIP", 0x1)
			, Wrap(this, "Wrap", 0x3)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	private:
		N25Q<CONFIG> *n25q;
	};
	
	struct EnhancedVolatileConfigurationRegister : Register<8, ACC_RW, ACC_RW, E_LITTLE_ENDIAN>
	{
		typedef Register<8, ACC_RW, ACC_RW, E_LITTLE_ENDIAN> BaseType;
		
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 7>    Quad_IO_protocol;
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 6>    Dual_IO_protocol;
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 4>    Reset_hold;
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 3>    VPP_accelerator;
		BitField<EnhancedVolatileConfigurationRegister, ACC_RW, ACC_RW, 2, 0> Output_driver_strength;
		
		EnhancedVolatileConfigurationRegister(N25Q<CONFIG> *_n25q, const std::string& name, const std::string& long_name)
			: BaseType(std::string(_n25q->GetName()) + name, long_name, 0x0)
			, Quad_IO_protocol(this, "Quad_IO_protocol", 0x1)
			, Dual_IO_protocol(this, "Dual_IO_protocol", 0x1)
			, Reset_hold(this, "Reset_hold", 0x1)
			, VPP_accelerator(this, "VPP_accelerator", 0x1)
			, Output_driver_strength(this, "Output_driver_strength", 0x7)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	};
	
	struct FlagStatusRegister : Register<8, ACC_RW, ACC_RW, E_LITTLE_ENDIAN>
	{
		typedef Register<8, ACC_RW, ACC_RW, E_LITTLE_ENDIAN> BaseType;
		
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 7> Program_or_erase_controller;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 6> Erase_suspend;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 5> Erase;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 4> Program;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 3> VPP;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 2> Program_suspend;
		BitField<FlagStatusRegister, ACC_RW, ACC_RW, 1> Protection;
		
		FlagStatusRegister(N25Q<CONFIG> *_n25q, const std::string& name, const std::string& long_name)
			: BaseType(std::string(_n25q->GetName()) + name, long_name, 0xff)
			, Program_or_erase_controller(this, "Program_or_erase_controller", 0x1)
			, Erase_suspend(this, "Erase_suspend", 0x1)
			, Erase(this, "Erase", 0x1)
			, Program(this, "Program", 0x1)
			, VPP(this, "VPP", 0x1)
			, Program_suspend(this, "Program_suspend", 0x1)
			, Protection(this, "Protection", 0x1)
			, n25q(_n25q)
		{
		}
		
		FlagStatusRegister& operator = (const STORAGE_TYPE& value)
		{
			bool old_program_or_erase_controler = Program_or_erase_controller;
			
			BaseType::operator = (value);

			bool new_program_or_erase_controler = Program_or_erase_controller;

			if(!old_program_or_erase_controler && new_program_or_erase_controler)
			{
				if(n25q->verbose)
				{
					n25q->logger << DebugInfo << sc_core::sc_time_stamp() << ": Program or erase controller becomes ready" << EndDebugInfo;
				}
			}
			
			if(old_program_or_erase_controler && !new_program_or_erase_controler)
			{
				if(n25q->verbose)
				{
					n25q->logger << DebugInfo << sc_core::sc_time_stamp() << ": Program or erase controller becomes busy" << EndDebugInfo;
				}
			}
			
			return *this;
		}
		
		using BaseType::operator const STORAGE_TYPE;
	private:
		N25Q<CONFIG> *n25q;
	};
	
	struct InternalConfigurationRegister : Register<16, ACC_RW, ACC_NONE, E_LITTLE_ENDIAN>
	{
		typedef Register<16, ACC_RW, ACC_NONE, E_LITTLE_ENDIAN> BaseType;
		
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE, 15, 12> Number_of_dummy_clock_cycles;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  8,  6> Output_driver_strength;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  5>     VPP_accelerator;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  4>     Reset_hold;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  3>     Quad_IO_protocol;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  2>     Dual_IO_protocol;
		BitField<InternalConfigurationRegister, ACC_RW, ACC_NONE,  1, 0>  Wrap;
		
		InternalConfigurationRegister(N25Q<CONFIG> *_n25q, const std::string& name, const std::string& long_name)
			: BaseType(std::string(_n25q->GetName()) + name, long_name, 0xffff)
			, Number_of_dummy_clock_cycles(this, "Number_of_dummy_clock_cycles", 0xf)
			, Output_driver_strength(this, "Output_driver_strength", 0x7)
			, VPP_accelerator(this, "VPP_accelerator", 0x1)
			, Reset_hold(this, "Reset_hold", 0x1)
			, Quad_IO_protocol(this, "Quad_IO_protocol", 0x1)
			, Dual_IO_protocol(this, "Dual_IO_protocol", 0x1)
			, Wrap(this, "Wrap", 0x3)
		{
		}

		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	};
	
	struct LockRegister : Register<2, ACC_RW, ACC_RW, E_LITTLE_ENDIAN>
	{
		typedef Register<2, ACC_RW, ACC_RW, E_LITTLE_ENDIAN> BaseType;
		
		BitField<LockRegister, ACC_RW, ACC_RW, 1> Sector_lock_down;
		BitField<LockRegister, ACC_RW, ACC_RW, 0> Sector_write_lock;
		
		LockRegister(N25Q<CONFIG> *_n25q, const std::string& name, const std::string& long_name)
			: BaseType(std::string(_n25q->GetName()) + name, long_name, 0x0)
			, Sector_lock_down(this, "Sector_lock_down", 0x0)
			, Sector_write_lock(this, "Sector_write_lock", 0x0)
		{
		}
		
		using BaseType::operator const STORAGE_TYPE;
		using BaseType::operator = ;
	};
	
	enum Stage
	{
		START_OF_OPERATION,
		END_OF_OPERATION
	};
	
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
			, free_data(false)
			, n25q_cmd(N25Q_UNKNOWN_COMMAND)
			, n25q_addr(0)
			, tx_data_bit_offset(0)
			, rx_data_bit_offset(0)
			, read_data_bit_offset(0)
			, stage(START_OF_OPERATION)
			, post_time(sc_core::SC_ZERO_TIME)
			, elapsed_time(sc_core::SC_ZERO_TIME)
			, duration(sc_core::SC_ZERO_TIME)
			, suspended(false)
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
			if(release_payload && payload && payload->has_mm())
			{
				if(free_data && (payload->get_ref_count() == 1)) delete[] payload->get_data_ptr();
				payload->release();
			}
			payload = 0;
			release_payload = false;
			free_data = false;
			n25q_cmd = N25Q_UNKNOWN_COMMAND;
			n25q_addr = 0;
			tx_data_bit_offset = 0;
			rx_data_bit_offset = 0;
			read_data_bit_offset = 0;
			stage = START_OF_OPERATION;
			post_time = sc_core::SC_ZERO_TIME;
			elapsed_time = sc_core::SC_ZERO_TIME;
			duration = sc_core::SC_ZERO_TIME;
			suspended  = false;
			completion_event = 0;
		}
		
		void SetTimeStamp(const sc_core::sc_time& _time_stamp)
		{
			key.SetTimeStamp(_time_stamp);
		}
		
		void SetCommand(n25q_command_type _n25q_cmd)
		{
			n25q_cmd = _n25q_cmd;
		}
		
		void SetAddress(uint32_t _addr)
		{
			n25q_addr = _addr;
		}
		
		void SetTxDataBitOffset(unsigned int _tx_data_bit_offset)
		{
			tx_data_bit_offset = _tx_data_bit_offset;
		}
		
		void SetRxDataBitOffset(unsigned int _rx_data_bit_offset)
		{
			rx_data_bit_offset = _rx_data_bit_offset;
		}

		void SetReadDataBitOffset(unsigned int _read_data_bit_offset)
		{
			read_data_bit_offset = _read_data_bit_offset;
		}

		void SetStage(Stage _stage)
		{
			stage = _stage;
		}
		
		void SetPostTime(const sc_core::sc_time& _post_time)
		{
			post_time = _post_time;
		}
		
		void Suspend()
		{
			suspended = true;
		}
		
		void Resume()
		{
			suspended = false;
		}
		
		void UpdateElapsedTime(const sc_core::sc_time& current_time)
		{
			elapsed_time += current_time;
			elapsed_time -= post_time;
		}
		
		void SetDuration(const sc_core::sc_time& t)
		{
			duration = t;
		}
		
		void SetCompletionEvent(sc_core::sc_event *_completion_event)
		{
			completion_event = _completion_event;
		}
		
		const sc_core::sc_time& GetTimeStamp() const
		{
			return key.GetTimeStamp();
		}

		n25q_command_type GetCommand() const
		{
			return n25q_cmd;
		}
		
		uint32_t GetAddress() const
		{
			return n25q_addr;
		}
		
		unsigned int GetTxDataBitOffset() const
		{
			return tx_data_bit_offset;
		}
		
		unsigned int GetRxDataBitOffset() const
		{
			return rx_data_bit_offset;
		}

		unsigned int GetReadDataBitOffset() const
		{
			return read_data_bit_offset;
		}

		Stage GetStage() const
		{
			return stage;
		}
		
		void SetPayload(payload_type *_payload, bool _release_payload = false, bool _free_data = false)
		{
			if(release_payload && payload && payload->has_mm())
			{
				if(free_data && (payload->get_ref_count() == 1)) delete[] payload->get_data_ptr();
				payload->release();
			}
			payload = _payload;
			release_payload = _release_payload;
			free_data = _free_data;
		}
		
		const sc_core::sc_time& GetDuration() const
		{
			return duration;
		}
		
		const sc_core::sc_time& GetElapsedTime() const
		{
			return elapsed_time;
		}

		bool Suspended() const
		{
			return suspended;
		}
		
		const Key& GetKey() const
		{
			return key;
		}
		
		payload_type *GetPayload() const
		{
			return payload;
		}
		
		sc_core::sc_event *GetCompletionEvent() const
		{
			return completion_event;
		}
		
	private:
		Key key;
		payload_type *payload;                   // the payload
		bool release_payload;                    // whether payload should be released when it becomes useless
		bool free_data;                          // whether data should be freed when payload reference count reaches 0
		n25q_command_type n25q_cmd;              // decoded command
		uint32_t n25q_addr;                      // decoded address
		unsigned int tx_data_bit_offset;         // bit offset in TX data array
		unsigned int rx_data_bit_offset;         // bit offset in RX data array
		unsigned int read_data_bit_offset;       // data bit offset for a read
		Stage stage;                             // stage of operation: start or end
		sc_core::sc_time post_time;              // the time when the event has been posted
		sc_core::sc_time elapsed_time;           // elapsed time of command
		sc_core::sc_time duration;               // duration of command
		bool suspended;                          // whether operation is in suspended state
		sc_core::sc_event *completion_event;     // a SystemC event for blocking interface
	};
	
	unisim::kernel::variable::Parameter<bool> param_verbose;
	std::string input_filename;
	unisim::kernel::variable::Parameter<std::string> param_input_filename;
	std::string output_filename;
	unisim::kernel::variable::Parameter<std::string> param_output_filename;
	
	unisim::kernel::tlm2::PayloadFabric<payload_type> payload_fabric;
	unisim::kernel::tlm2::Schedule<Event> schedule;
	std::vector<Event *> suspend_stack;                   // at most one program and one erase
	Event *program_or_erase_operation_in_progress_event;  // program or erase operation in progress event
	Event *suspend_in_progress_event;                     // suspend operation in progress event
	
	sc_core::sc_time erase_resume_time_stamp;       // sector or bulk erase resume time stamp
	sc_core::sc_time program_resume_time_stamp;     // program resume time stamp
	sc_core::sc_time subsector_erase_resume_time_stamp; // subsector erase resume time stamp
	
	n25q_mode_type n25q_mode;
	n25q_command_type n25q_xip_cmd;               // QSPI FAST READ command that confirmed XIP mode
	
	StatusRegister                        SR;    // status register
	NonVolatileConfigurationRegister      NVCR;  // non volatile configuration register
	VolatileConfigurationRegister         VCR;   // volatile configuration register
	EnhancedVolatileConfigurationRegister VECR;  // enhanced volatile configuration register
	FlagStatusRegister                    FSR;   // flag status register
	InternalConfigurationRegister         ICR;   // internal configuration register (not visible to SW)
	
	LockRegister *LR[CONFIG::SIZE / CONFIG::SECTOR_SIZE]; // sector lock registers
	uint8_t *storage; // data array of CONFIG::SIZE bytes size
	uint8_t *OTP; // OTP array of CONFIG::OTP_ARRAY_SIZE bytes + 1 OTP control byte
	
	sc_core::sc_time MIN_CLOCK_PERIOD;
	sc_core::sc_time FAST_READ_MIN_CLOCK_PERIODS[10][5];
	sc_core::sc_time tWNVCR;
	sc_core::sc_time tW;
	sc_core::sc_time tCFSR;
	sc_core::sc_time tWVCR;
	sc_core::sc_time tWRVECR;
	sc_core::sc_time tSHSL2;
	sc_core::sc_time tPOTP;
	sc_core::sc_time tPP;
	sc_core::sc_time tPP_VPP;
	sc_core::sc_time tSSE;
	sc_core::sc_time tSE;
	sc_core::sc_time tSE_VPP;
	sc_core::sc_time tBE;
	sc_core::sc_time tBE_VPP;
	sc_core::sc_time erase_resume_to_suspend;
	sc_core::sc_time program_resume_to_suspend;
	sc_core::sc_time subsector_erase_resume_to_suspend;
	sc_core::sc_time suspend_latency_program;
	sc_core::sc_time suspend_latency_subsector_erase;
	sc_core::sc_time suspend_latency_erase;
	
	std::ofstream *output_file;
	
	void DumpStorageToFile(unsigned int offset = 0, unsigned int length = CONFIG::SIZE);
	void HWReset();
	void Xfer(uint8_t *dst, unsigned int dst_bit_length, unsigned int dst_bit_offset, n25q_signal_type dst_signals, const uint8_t *src, unsigned int src_bit_length, unsigned int src_bit_offset, n25q_signal_type src_signals, bool program = false);
	void Zero(uint8_t *dst, unsigned int dst_bit_length, unsigned int dst_bit_offset);
	void Program(uint8_t *dst, unsigned int dst_bit_length, unsigned int dst_bit_offset, n25q_signal_type dst_signals, const uint8_t *src, unsigned int src_bit_length, unsigned int src_bit_offset, n25q_signal_type src_signals);
	void ProgramStorage(unsigned int dst_bit_length, unsigned int dst_bit_offset, n25q_signal_type dst_signals, const uint8_t *src, unsigned int src_bit_length, unsigned int src_bit_offset, n25q_signal_type src_signals);
	void ProgramOTP(unsigned int dst_bit_length, unsigned int dst_bit_offset, n25q_signal_type dst_signals, const uint8_t *src, unsigned int src_bit_length, unsigned int src_bit_offset, n25q_signal_type src_signals);
	void EraseStorage(unsigned int offset, unsigned int length);
	
	const n25q_protocol_type GetProtocol() const;
	uint32_t GetWrap() const;
	bool IsSectorProtected(uint32_t addr);
	bool IsDeviceProtected();
	bool IsProgramOrEraseSuspendedFor(uint32_t addr);
	bool IsProgramSuspended();
	bool IsProgramSuspendedFor(uint32_t addr);
	bool IsEraseSuspendedFor(uint32_t addr);
	bool IsEraseSuspended();
	bool IsSubSectorEraseOrProgramSuspended();
	bool CheckClock(const sc_core::sc_time& clock, n25q_command_type n25q_cmd, sc_core::sc_time *& min_clock_period);
	Event *Decode(payload_type& payload, bool blocking_if = false);
	sc_core::sc_time ComputeRequestTime(Event *event);
	sc_core::sc_time ComputeResponseTime(Event *event);
	const sc_core::sc_time ComputeWriteOperationTime(Event *event);
	unsigned int DummyCycles(n25q_command_type n25q_cmd);
	Event *PostEndOfWriteOperation(Event *event, const sc_core::sc_time& operation_duration);

	void ReadID(Event *event);
	void MultipleIOReadID(Event *event);
	void ReadSFDP(Event *event);
	void Read(Event *event);
	void FastRead(Event *event);
	void WriteEnable();
	void WriteDisable();
	void ReadStatusRegister(Event *event);
	void ReadFlagStatusRegister(Event *event);
	void ReadNonVolatileConfigurationRegister(Event *event);
	void ReadVolatileConfigurationRegister(Event *event);
	void ReadEnhancedVolatileConfigurationRegister(Event *event);
	void ReadLockRegister(Event *event);
	void ReadOTPArray(Event *event);
	
	void WriteNonVolatileConfigurationRegister(Event *event);
	void WriteVolatileConfigurationRegister(Event *event);
	void WriteEnhancedVolatileConfigurationRegister(Event *event);
	void WriteStatusRegister(Event *event);
	void WriteLockRegister(Event *event);
	void ClearFlagStatusRegister();
	void ProgramOTPArray(Event *event);
	void Program(Event *event);
	void SubSectorErase(Event *event);
	void SectorErase(Event *event);
	void BulkErase(Event *event);
	void ProgramEraseSuspend(Event *suspend_event);
	void ProgramEraseResume(Event *resume_event);

	void ProcessEvents();
	void ProcessEvent(Event *event);
	void Process();
	void RESET_N_Process();
	void HOLD_N_Process();
};

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_HH__
