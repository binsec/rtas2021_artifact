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
 
#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_TCC__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_TCC__

#include <string.h>
#include <math.h>

using namespace sc_core;

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

template <typename CONFIG>
N25Q<CONFIG>::N25Q(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, qspi_slave_socket("qspi_slave_socket")
	, HOLD_RESET_N("HOLD_RESET_N")
	, W_N_VPP("W_N_VPP")
	, logger(*this)
	, verbose(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, input_filename()
	, param_input_filename("input-filename", this, input_filename, "Input filename")
	, output_filename()
	, param_output_filename("output-filename", this, output_filename, "output filename")
	, schedule()
	, suspend_stack()
	, program_or_erase_operation_in_progress_event(0)
	, suspend_in_progress_event(0)
	, erase_resume_time_stamp(sc_core::SC_ZERO_TIME)
	, program_resume_time_stamp(sc_core::SC_ZERO_TIME)
	, subsector_erase_resume_time_stamp(sc_core::SC_ZERO_TIME)
	, n25q_mode(N25Q_STD_MODE)
	, n25q_xip_cmd(N25Q_UNKNOWN_COMMAND)
	, SR(this, "SR", "Status Register")
	, NVCR(this, "NVCR", "Nonvolatile Configuration Register")
	, VCR(this, "VCR", "Volatile Configuration Register")
	, VECR(this, "VECR", "Enhanced Volatile Configuration Register")
	, FSR(this, "FSR", "Flag Status Register")
	, ICR(this, "ICR", "Internal Configuration Register")
	, LR()
	, storage(0)
	, OTP(0)
	, MIN_CLOCK_PERIOD(sc_core::sc_time(1000000.0 / CONFIG::MAX_FREQUENCY, sc_core::SC_PS))
	, FAST_READ_MIN_CLOCK_PERIODS()
	, tWNVCR(CONFIG::tWNVCR, sc_core::SC_SEC)
	, tW(CONFIG::tW, sc_core::SC_MS)
	, tCFSR(CONFIG::tCFSR, sc_core::SC_NS)
	, tWVCR(CONFIG::tWVCR, sc_core::SC_NS)
	, tWRVECR(CONFIG::tWRVECR, sc_core::SC_NS)
	, tSHSL2(CONFIG::tSHSL2, sc_core::SC_NS)
	, tPOTP(CONFIG::tPOTP, sc_core::SC_MS)
	, tPP(CONFIG::tPP, sc_core::SC_MS)
	, tPP_VPP(CONFIG::tPP_VPP, sc_core::SC_MS)
	, tSSE(CONFIG::tSSE, sc_core::SC_SEC)
	, tSE(CONFIG::tSE, sc_core::SC_SEC)
	, tSE_VPP(CONFIG::tSE_VPP, sc_core::SC_SEC)
	, tBE(CONFIG::tBE, sc_core::SC_SEC)
	, tBE_VPP(CONFIG::tBE_VPP, sc_core::SC_SEC)
	, erase_resume_to_suspend(CONFIG::erase_resume_to_suspend, sc_core::SC_US)
	, program_resume_to_suspend(CONFIG::program_resume_to_suspend, sc_core::SC_US)
	, subsector_erase_resume_to_suspend(CONFIG::subsector_erase_resume_to_suspend, sc_core::SC_US)
	, suspend_latency_program(CONFIG::suspend_latency_program, sc_core::SC_US)
	, suspend_latency_subsector_erase(CONFIG::suspend_latency_subsector_erase, sc_core::SC_US)
	, suspend_latency_erase(CONFIG::suspend_latency_erase, sc_core::SC_US)
	, output_file(0)
{
	qspi_slave_socket(*this); // bind interface
	
	unsigned int num_dummy_cycles;
	unsigned int idx;
	
	for(num_dummy_cycles = 1; num_dummy_cycles <= 10; num_dummy_cycles++)
	{
		for(idx = 0; idx < 5; idx++)
		{
			FAST_READ_MIN_CLOCK_PERIODS[num_dummy_cycles - 1][idx] = sc_core::sc_time(1000000.0 / CONFIG::FAST_READ_MAX_FREQUENCIES[num_dummy_cycles - 1][idx], sc_core::SC_PS);
		}
	}
	
	for(idx = 0; idx < CONFIG::SIZE / CONFIG::SECTOR_SIZE; idx++)
	{
		std::stringstream lr_name_sstr;
		lr_name_sstr << "LR_" << idx;
		std::stringstream lr_friendly_name_sstr;
		lr_friendly_name_sstr << "Lock Register #" << idx;
		LR[idx] = new LockRegister(this, lr_name_sstr.str(), lr_friendly_name_sstr.str());
	}
	
	storage = new uint8_t[CONFIG::SIZE];
	OTP = new uint8_t[CONFIG::OTP_ARRAY_SIZE + 1];
	
	Reset();
	
	SC_HAS_PROCESS(N25Q<CONFIG>);
	
	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	if(CONFIG::HOLD_RST_PIN_F == CONFIG::RST_PIN)
	{
		SC_METHOD(RESET_N_Process);
		sensitive << HOLD_RESET_N.neg();
	}
	else if(CONFIG::HOLD_RST_PIN_F == CONFIG::HOLD_PIN)
	{
		SC_METHOD(HOLD_N_Process);
		sensitive << HOLD_RESET_N;
	}
}

template <typename CONFIG>
N25Q<CONFIG>::~N25Q()
{
	if(output_file)
	{
		DumpStorageToFile();
		delete output_file;
	}
	
	unsigned int idx;
	
	for(idx = 0; idx < CONFIG::SIZE / CONFIG::SECTOR_SIZE; idx++)
	{
		delete LR[idx];
	}
	
	delete[] storage;
	delete[] OTP;
}

template <typename CONFIG>
void N25Q<CONFIG>::HWReset()
{
	SR.Reset();
	NVCR.Reset();
	VCR.Reset();
	VECR.Reset();
	FSR.Reset();
	ICR.Reset();
	
	// NVCR   VCR VECR
	//   |       |
	//   +---+---+
	//       |
	//       V
	//      ICR
	ICR.Number_of_dummy_clock_cycles = NVCR.Number_of_dummy_clock_cycles;
	ICR.Output_driver_strength = NVCR.Output_driver_strength;
	ICR.VPP_accelerator = VECR.VPP_accelerator;
	ICR.Reset_hold = NVCR.Reset_hold;
	ICR.Quad_IO_protocol = NVCR.Quad_IO_protocol;
	ICR.Dual_IO_protocol = NVCR.Dual_IO_protocol;
	ICR.Wrap = VCR.Wrap;
	
	switch(NVCR.XIP_mode_at_power_on_reset)
	{
		case 0x0:
			n25q_mode = N25Q_XIP_MODE;
			n25q_xip_cmd = N25Q_FAST_READ_COMMAND;
			break;
		case 0x1:
			n25q_mode = N25Q_XIP_MODE;
			n25q_xip_cmd = N25Q_DUAL_OUTPUT_FAST_READ_COMMAND;
			break;
		case 0x2:
			n25q_mode = N25Q_XIP_MODE;
			n25q_xip_cmd = N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND;
			break;
		case 0x3:
			n25q_mode = N25Q_XIP_MODE;
			n25q_xip_cmd = N25Q_QUAD_OUTPUT_FAST_READ_COMMAND;
			break;
		case 0x4:
			n25q_mode = N25Q_XIP_MODE;
			n25q_xip_cmd = N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND;
			break;
		default:
			n25q_mode = N25Q_STD_MODE;
			n25q_xip_cmd = N25Q_UNKNOWN_COMMAND;
			break;
	}
}

template <typename CONFIG>
unisim::kernel::tlm2::tlm_spi_sync_enum N25Q<CONFIG>::spi_nb_send(payload_type& payload)
{
	Event *event = Decode(payload);
	if(event)
	{
		schedule.Notify(event);
	}
	payload.set_status(TLM_SPI_OK);
	return unisim::kernel::tlm2::TLM_SPI_ACCEPTED;
}

template <typename CONFIG>
void N25Q<CONFIG>::spi_b_send(payload_type& payload)
{
	if(verbose)
	{
		unsigned char *tx_data_ptr = payload.get_tx_data_ptr();
		unsigned int tx_data_length = payload.get_tx_data_length();
		unsigned int tx_width = payload.get_tx_width();
		
		logger << DebugInfo << sc_core::sc_time_stamp() << ": QSPI Master TX/" << tx_width << ": ";
		
		unsigned int i;
		for(i = 0; i < tx_data_length; i++)
		{
			if(i) logger << " ";
			
			logger << "0x" << std::hex << (unsigned int) tx_data_ptr[i] << std::dec;
		}
		logger << EndDebugInfo;
	}
	sc_core::sc_event completion_event;
	Event *event = Decode(payload, true);
	if(event)
	{
		event->SetCompletionEvent(&completion_event);
		schedule.Notify(event);
		sc_core::wait(completion_event);
	}
	
	if(verbose)
	{
		unsigned char *rx_data_ptr = payload.get_rx_data_ptr();
		unsigned int rx_data_length = payload.get_rx_data_length();
		unsigned int rx_width = payload.get_rx_width();
		
		logger << DebugInfo << sc_core::sc_time_stamp() << ": QSPI Master RX/" << rx_width << ": ";

		unsigned int i;
		for(i = 0; i < rx_data_length; i++)
		{
			if(i) logger << " ";
			
			logger << "0x" << std::hex << (unsigned int) rx_data_ptr[i] << std::dec;
		}
		logger << EndDebugInfo;
	}
	payload.set_status(TLM_SPI_OK);
}

template <typename CONFIG>
void N25Q<CONFIG>::DumpStorageToFile(unsigned int offset, unsigned int length)
{
	if(output_file)
	{
		const char *buffer = (const char *)(storage + offset);
		
		output_file->seekp(offset, std::ios_base::beg);
		
		if(output_file->bad())
		{
			logger << DebugWarning << "I/O error while seeking in \"" << output_filename << "\"" << EndDebugWarning;
			return;
		}

		if(verbose)
		{
			logger << DebugInfo << "Writing " << length << " bytes at offset 0x" << std::hex << offset << std::dec << " to \"" << output_filename << "\"" << EndDebugInfo;
		}

		output_file->write(buffer, length);
		
		if(output_file->bad())
		{
			logger << DebugWarning << "I/O error while writing to \"" << output_filename << "\"" << EndDebugWarning;
			return;
		}
		
		output_file->flush();
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Reset()
{
	if(output_file)
	{
		output_file->close();
		delete output_file;
	}
	
	memset(storage, 0xff, CONFIG::SIZE); // bits are all 1's
	memset(OTP, 0x00, CONFIG::OTP_ARRAY_SIZE + 1); // bits are all 0's
	
	if(!input_filename.empty())
	{
		std::ifstream input_file(input_filename.c_str(), std::ios_base::binary);
		
		if(input_file.is_open())
		{
			if(verbose)
			{
				logger << DebugInfo << "Loading up to " << CONFIG::SIZE << " bytes from \"" << input_filename << "\"" << EndDebugInfo;
			}
			std::streamsize n = input_file.readsome((char *) storage, CONFIG::SIZE);
			
			if(input_file.bad())
			{
				logger << DebugWarning << "I/O error while reading from \"" << input_filename << "\"" << EndDebugWarning;
			}
			else if(verbose)
			{
				logger << DebugInfo << n << " bytes loaded from \"" << input_filename << "\"" << EndDebugInfo;
			}
		}
		else
		{
			logger << DebugWarning << "Can't open for input \"" << input_filename << "\"" << EndDebugWarning;
		}
	}
	
	if(!output_filename.empty())
	{
		output_file = new std::ofstream(output_filename.c_str(), std::ios_base::binary);
		
		if(output_file->is_open())
		{
			DumpStorageToFile();
		}
		else
		{
			logger << DebugWarning << "Can't open for output \"" << output_filename << "\"" << EndDebugWarning;
			delete output_file;
			output_file = 0;
			unisim::kernel::Object::Stop(-1);
		}
	}
	
	HWReset();
}

template <typename CONFIG>
void N25Q<CONFIG>::Xfer(uint8_t *dst, unsigned int dst_bit_length, unsigned int dst_bit_offset, n25q_signal_type dst_signals, const uint8_t *src, unsigned int src_bit_length, unsigned int src_bit_offset, n25q_signal_type src_signals, bool program)
{
	unsigned int dst_width = n25q_signal_width(dst_signals);
	unsigned int src_width = n25q_signal_width(src_signals);
	
	bool debug = true;
	
	if(verbose && debug)
	{
		logger << DebugInfo << "Xfer: " << dst_signals << " (/" << dst_width << ") <= " << src_signals << " (/" << src_width << ")" << EndDebugInfo;
	}

	bool warn_toggle_bits_to_one = false;
	unsigned int i, j, k;
	
	if(dst_width > src_width)
	{
		unsigned int last_dst_k = n25q_last_signal_of(dst_signals);
		
		for(i = 0, j = 0; i < dst_bit_length;)
		{
			for(k = 0; (k < dst_width) && (i < dst_bit_length); k++, i++)
			{
				unsigned int dbio = dst_bit_offset + i;
				unsigned int sbio = src_bit_offset + j;
				unsigned int dbyo = dbio / 8;
				unsigned int sbyo = sbio / 8;
				unsigned int dsh = 7 - (dbio % 8);
				unsigned int ssh = 7 - (sbio % 8);
				uint8_t mask = 1 << dsh;
				n25q_signal_type cur_signal = n25q_signal(last_dst_k - k);
				bool has_signal = dst_signals & src_signals & cur_signal;
				uint8_t s = (has_signal && (sbio >= src_bit_offset) && (sbio < src_bit_length)) ? ((src[sbyo] >> ssh) & 1) << dsh : 0;
				uint8_t d = dst[dbyo] & mask; // old value
				if(program && !d && s) warn_toggle_bits_to_one = true;
				dst[dbyo] = program ? (dst[dbyo] & (~mask | s)) /* toggle 1->0 only */: ((dst[dbyo] & ~mask) | (s & mask)); /* copy bit */
				
				if(verbose && debug)
				{
					logger << DebugInfo << cur_signal << ": D" << dbio << " <- ";
					if(has_signal && (sbio >= src_bit_offset) && (sbio < src_bit_length)) logger << DebugInfo << "S" << sbio; else logger << DebugInfo << "X";
					logger << EndDebugInfo;
				}

				if(has_signal) j++;
			}
		}
	}
	else
	{
		unsigned int last_src_k = n25q_last_signal_of(src_signals);
		
		for(i = 0, j = 0; i < dst_bit_length;)
		{
			for(k = 0; (k < src_width) && (i < dst_bit_length); k++, j++)
			{
				unsigned int dbio = dst_bit_offset + i;
				unsigned int sbio = src_bit_offset + j;
				unsigned int dbyo = dbio / 8;
				unsigned int sbyo = sbio / 8;
				unsigned int dsh = 7 - (dbio % 8);
				unsigned int ssh = 7 - (sbio % 8);
				uint8_t mask = 1 << dsh;
				n25q_signal_type cur_signal = n25q_signal(last_src_k - k);
				bool has_signal = dst_signals & src_signals & cur_signal;
				uint8_t s = ((sbio >= src_bit_offset) && (sbio < src_bit_length)) ? ((src[sbyo] >> ssh) & 1) << dsh : 0;
				uint8_t d = dst[dbyo] & mask; // old value
				if(has_signal)
				{
					if(program && !d && s) warn_toggle_bits_to_one = true;
					dst[dbyo] = program ? (dst[dbyo] & (~mask | s)) /* toggle 1->0 only */: ((dst[dbyo] & ~mask) | (s & mask)); /* copy bit */
					
					if(verbose && debug)
					{
						logger << DebugInfo << cur_signal << ": D" << dbio << " <- ";
						if((sbio >= src_bit_offset) && (sbio < src_bit_length)) logger << DebugInfo << "S" << sbio; else logger << DebugInfo << "X";
						logger << EndDebugInfo;
					}
					
					i++;
				}
			}
		}
	}
	
	if(program && warn_toggle_bits_to_one)
	{
		logger << DebugWarning << "Attempt to toggle some bits from 0 to 1 while programming." << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Zero(uint8_t *dst, unsigned int dst_bit_length, unsigned int dst_bit_offset)
{
	bool debug = true;
	
	if(dst_bit_length)
	{
		unsigned int i;
		for(i = 0; i < dst_bit_length; i++)
		{
			unsigned int dbio = dst_bit_offset + i;
			unsigned int dbyo = dbio / 8;
			unsigned int dsh = 7 - (dbio % 8);
			uint8_t mask = 1 << dsh;
			dst[dbyo] = dst[dbyo] & ~mask; // zero bit
			
			if(verbose && debug)
			{
				logger << DebugInfo << "D" << dbio << " <- X" << EndDebugInfo;
			}
		}
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Program(uint8_t *dst, unsigned int dst_bit_length, unsigned int dst_bit_offset, n25q_signal_type dst_signals, const uint8_t *src, unsigned int src_bit_length, unsigned int src_bit_offset, n25q_signal_type src_signals)
{
	Xfer(dst, dst_bit_length, dst_bit_offset, dst_signals, src, src_bit_length, src_bit_offset, src_signals, true);
}

template <typename CONFIG>
void N25Q<CONFIG>::ProgramStorage(unsigned int dst_bit_length, unsigned int dst_bit_offset, n25q_signal_type dst_signals, const uint8_t *src, unsigned int src_bit_length, unsigned int src_bit_offset, n25q_signal_type src_signals)
{
	if(verbose)
	{
		logger << DebugInfo << "Programming " << dst_bit_length << " bits at bit offset 0x" << std::hex << dst_bit_offset << std::dec << EndDebugInfo;
	}
	
	Program(storage, dst_bit_length, dst_bit_offset, dst_signals, src, src_bit_length, src_bit_offset, src_signals);
	
	if(output_file)
	{
		unsigned int first_byte_offset = dst_bit_offset / 8;
		unsigned int last_byte_offset = (dst_bit_offset + dst_bit_length - 1) / 8;
		unsigned int offset = first_byte_offset;
		unsigned int length = last_byte_offset - first_byte_offset + 1;
		DumpStorageToFile(offset, length);
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ProgramOTP(unsigned int dst_bit_length, unsigned int dst_bit_offset, n25q_signal_type dst_signals, const uint8_t *src, unsigned int src_bit_length, unsigned int src_bit_offset, n25q_signal_type src_signals)
{
	Program(OTP, dst_bit_length, dst_bit_offset, dst_signals, src, src_bit_length, src_bit_offset, src_signals);
}

template <typename CONFIG>
void N25Q<CONFIG>::EraseStorage(unsigned int offset, unsigned int length)
{
	if(verbose)
	{
		logger << DebugInfo << "Erasing " << (8 * length) << " bits at bit offset 0x" << std::hex << (8 * offset) << std::dec << EndDebugInfo;
	}
	
	memset(storage + offset, 0xff, length);
	
	if(output_file)
	{
		DumpStorageToFile(offset, length);
	}
}

template <typename CONFIG>
const n25q_protocol_type N25Q<CONFIG>::GetProtocol() const
{
	if(ICR.Quad_IO_protocol == 0) return N25Q_QUAD_IO_PROTOCOL;
	if(ICR.Dual_IO_protocol == 0) return N25Q_DUAL_IO_PROTOCOL;
	return N25Q_EXTENDED_PROTOCOL;
}

template <typename CONFIG>
uint32_t N25Q<CONFIG>::GetWrap() const
{
	if(ICR.Wrap == WRAP_16B) return 16;
	if(ICR.Wrap == WRAP_32B) return 32;
	if(ICR.Wrap == WRAP_64B) return 64;
	return CONFIG::SIZE;
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsSectorProtected(uint32_t addr)
{
	if(LR[addr / CONFIG::SECTOR_SIZE]->Sector_write_lock != 0) return true;
	
	return CONFIG::IsProtected(addr, SR.Top_bottom, SR.Block_protect_2_0);
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsDeviceProtected()
{
	unsigned int i;
	
	for(i = 0; i < (CONFIG::SIZE / CONFIG::SECTOR_SIZE); i++)
	{
		if(LR[i]->Sector_write_lock != 0) return true;
	}
	
	return SR.Block_protect_2_0 != 0; // protected area: *NOT* none
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsProgramOrEraseSuspendedFor(uint32_t addr)
{
	typename std::vector<Event *>::size_type n = suspend_stack.size();
	typename std::vector<Event *>::size_type i;

	for(i = 0; i < n; i++)
	{
		Event *suspended_event = suspend_stack[i];
		
		n25q_command_type n25q_cmd = suspended_event->GetCommand();
		uint32_t program_or_erase_addr = suspended_event->GetAddress();
		
		switch(n25q_cmd)
		{
			case N25Q_PAGE_PROGRAM_COMMAND:
			case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_SUBSECTOR_ERASE_COMMAND:
			case N25Q_SECTOR_ERASE_COMMAND:
				return (program_or_erase_addr / CONFIG::SECTOR_SIZE) == (addr / CONFIG::SECTOR_SIZE);
			case N25Q_BULK_ERASE_COMMAND:
				return (addr < CONFIG::SIZE);
			default:
				break;
		}
	}
	
	return false;
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsProgramSuspended()
{
	typename std::vector<Event *>::size_type n = suspend_stack.size();
	typename std::vector<Event *>::size_type i;

	for(i = 0; i < n; i++)
	{
		Event *suspended_event = suspend_stack[i];
		
		n25q_command_type n25q_cmd = suspended_event->GetCommand();
		
		switch(n25q_cmd)
		{
			case N25Q_PAGE_PROGRAM_COMMAND:
			case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				return true;
			default:
				break;
		}
	}
	
	return false;
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsProgramSuspendedFor(uint32_t addr)
{
	typename std::vector<Event *>::size_type n = suspend_stack.size();
	typename std::vector<Event *>::size_type i;

	for(i = 0; i < n; i++)
	{
		Event *suspended_event = suspend_stack[i];
		
		n25q_command_type n25q_cmd = suspended_event->GetCommand();
		uint32_t program_or_erase_addr = suspended_event->GetAddress();
		
		switch(n25q_cmd)
		{
			case N25Q_PAGE_PROGRAM_COMMAND:
			case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				return (program_or_erase_addr / CONFIG::SECTOR_SIZE) == (addr / CONFIG::SECTOR_SIZE);
			default:
				break;
		}
	}
	
	return false;
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsEraseSuspendedFor(uint32_t addr)
{
	typename std::vector<Event *>::size_type n = suspend_stack.size();
	typename std::vector<Event *>::size_type i;

	for(i = 0; i < n; i++)
	{
		Event *suspended_event = suspend_stack[i];
		
		n25q_command_type n25q_cmd = suspended_event->GetCommand();
		uint32_t erase_addr = suspended_event->GetAddress();
		
		switch(n25q_cmd)
		{
			case N25Q_SUBSECTOR_ERASE_COMMAND:
				return (erase_addr / CONFIG::SUBSECTOR_SIZE) == (addr / CONFIG::SUBSECTOR_SIZE);
			case N25Q_SECTOR_ERASE_COMMAND:
				return (erase_addr / CONFIG::SECTOR_SIZE) == (addr / CONFIG::SECTOR_SIZE);
			case N25Q_BULK_ERASE_COMMAND:
				return (addr < CONFIG::SIZE);
			default:
				break;
		}
	}
	
	return false;
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsEraseSuspended()
{
	typename std::vector<Event *>::size_type n = suspend_stack.size();
	typename std::vector<Event *>::size_type i;

	for(i = 0; i < n; i++)
	{
		Event *suspended_event = suspend_stack[i];
		
		n25q_command_type n25q_cmd = suspended_event->GetCommand();
		
		switch(n25q_cmd)
		{
			case N25Q_SUBSECTOR_ERASE_COMMAND:
			case N25Q_SECTOR_ERASE_COMMAND:
			case N25Q_BULK_ERASE_COMMAND:
				return true;
			default:
				break;
		}
	}
	
	return false;
}

template <typename CONFIG>
bool N25Q<CONFIG>::IsSubSectorEraseOrProgramSuspended()
{
	typename std::vector<Event *>::size_type n = suspend_stack.size();
	typename std::vector<Event *>::size_type i;

	for(i = 0; i < n; i++)
	{
		Event *suspended_event = suspend_stack[i];
		
		n25q_command_type n25q_cmd = suspended_event->GetCommand();
		
		switch(n25q_cmd)
		{
			case N25Q_PAGE_PROGRAM_COMMAND:
			case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
			case N25Q_SUBSECTOR_ERASE_COMMAND:
				return true;
			default:
				break;
		}
	}
	
	return false;
}

template <typename CONFIG>
bool N25Q<CONFIG>::CheckClock(const sc_core::sc_time& clock_period, n25q_command_type n25q_cmd, sc_core::sc_time *& min_clock_period)
{
	switch(n25q_cmd)
	{
		case N25Q_FAST_READ_COMMAND:
		case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
		case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
		case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
		case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
			{
				unsigned int i = DummyCycles(n25q_cmd);
				unsigned int j = n25q_cmd - N25Q_FAST_READ_COMMAND;
				
				assert(i != 0);
				assert(j < 5);
				if(i > 10) i = 10;
				
				min_clock_period = &FAST_READ_MIN_CLOCK_PERIODS[j - 1][i];
			}
			break;
		default:
			min_clock_period = &MIN_CLOCK_PERIOD;
			break;
	}
	
	return clock_period >= (*min_clock_period);
}

template <typename CONFIG>
typename N25Q<CONFIG>::Event *N25Q<CONFIG>::Decode(payload_type& payload, bool blocking_if)
{
	const sc_core::sc_time& clock_period = payload.get_clock();
	
	if(verbose)
	{
		logger << DebugInfo << "Clock frequency is around " << (1.0 / (clock_period.to_seconds() * 1.0e6)) << " MHz (exactly " << clock_period << " clock period)" << EndDebugInfo;
	}
	
	unisim::kernel::tlm2::tlm_spi_com_mode com_mode = payload.get_com_mode();
	
	unsigned int tx_width = payload.get_tx_width();
	
	switch(tx_width)
	{
		case 1:
			break;
		case 2:
		case 4:
			if(com_mode == unisim::kernel::tlm2::TLM_SPI_FULL_DUPLEX)
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master is operating in full duplex communication mode while N25Q needs half duplex communication mode in dual or quad SPI protocol" << EndDebugWarning;
			}
			break;
		default:
			logger << DebugWarning << sc_core::sc_time_stamp() << ": N25Q only supports 1, 2, or 4 input pins" << EndDebugWarning;
			return 0;
	}
	
	n25q_signal_type tx_signals = n25q_input_signal(tx_width);

	unsigned int rx_width = payload.get_rx_width();
	switch(rx_width)
	{
		case 1:
			break;
		case 2:
		case 4:
			if(com_mode == unisim::kernel::tlm2::TLM_SPI_FULL_DUPLEX)
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master is operating in full duplex communication mode while N25Q needs half duplex communication mode in dual or quad SPI protocol" << EndDebugWarning;
			}
			break;
		default:
			logger << DebugWarning << sc_core::sc_time_stamp() << ": N25Q only supports 1, 2, or 4 output pins" << EndDebugWarning;
			return 0;
	}

	n25q_protocol_type n25q_protocol = GetProtocol();

	unsigned int tx_data_bit_length = payload.get_tx_data_bit_length();
	unsigned int rx_data_bit_length = payload.get_rx_data_bit_length();
	
	if(tx_data_bit_length & (tx_width - 1))
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ": N25Q only supports TX data bit length multiple of number of pins" << EndDebugWarning;
		return 0;
	}
	
	if(rx_data_bit_length & (rx_width - 1))
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ": N25Q only supports RX data bit length multiple of number of pins" << EndDebugWarning;
		return 0;
	}

	unsigned char *tx_data_ptr = payload.get_tx_data_ptr();
	unsigned int tx_data_bit_offset = 0;
	unsigned int rx_data_bit_offset = 0;
	unsigned int read_data_bit_offset = 0;

	n25q_command_type n25q_cmd = N25Q_UNKNOWN_COMMAND;
	
	if(n25q_mode == N25Q_STD_MODE)
	{
		unsigned int cmd_width = n25q_width(n25q_protocol);
		n25q_signal_type cmd_signal = n25q_input_signal(cmd_width);
		unsigned int cmd_cycles = 8 / cmd_width;
		
		if((tx_data_bit_offset + (cmd_cycles * tx_width)) > tx_data_bit_length)
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master did not send enough data bits for N25Q command byte" << EndDebugWarning;
			return 0;
		}
		
		uint8_t n25q_cmd_byte = 0;
		
		Xfer(&n25q_cmd_byte, 8, 0, cmd_signal, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
		n25q_cmd = n25q_decode_command_byte(n25q_cmd_byte, n25q_protocol);
		tx_data_bit_offset += cmd_cycles * tx_width;
	}
	else
	{
		n25q_cmd = n25q_xip_cmd;
	}
	
	if(n25q_cmd == N25Q_UNKNOWN_COMMAND)
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ": N25Q command byte is invalid" << EndDebugWarning;
		return 0;
	}
	else if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Receiving " << n25q_cmd << EndDebugInfo;
	}
	
	sc_core::sc_time *min_clock_period;
	
	if(!CheckClock(clock_period, n25q_cmd, min_clock_period))
	{
		logger << DebugWarning << "Clock frequency (around " << (1.0 / (clock_period.to_seconds() * 1.0e6)) << " MHz) is too high for " << n25q_cmd << " (max. supported frequency is around " << (1.0 / (min_clock_period->to_seconds() * 1.0e6)) << " MHz)" << EndDebugWarning;
	}

	uint32_t n25q_addr = 0;
	
	if(n25q_command_has_address(n25q_cmd))
	{
		n25q_signal_type address_signal = n25q_address_signal(n25q_cmd, n25q_protocol, n25q_mode);
		unsigned address_width = n25q_signal_width(address_signal); // either 1, 2 or 4
		
		if(address_width != tx_width)
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master sends address bytes using " << tx_width << " pin(s) (" << n25q_input_signal(tx_width) << ") while N25Q receives address bytes using " << address_width << " pin(s) (" << n25q_address_signal(n25q_cmd, n25q_protocol, n25q_mode) << ")" << EndDebugWarning;
		}
		
		unsigned int address_byte_cycles = 8 / address_width;
		unsigned int address_cycles = 3 * address_byte_cycles;
		
		if((tx_data_bit_offset + (address_cycles * tx_width)) > tx_data_bit_length)
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master did not send enough data bits for N25Q address bytes" << EndDebugWarning;
			return 0;
		}
		
		uint8_t address_bytes[3];
		Xfer(address_bytes, 24, 0, address_signal, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
		tx_data_bit_offset += 3 * address_byte_cycles * tx_width;
		
		n25q_addr = ((uint32_t) address_bytes[0] << 16) | ((uint32_t) address_bytes[1] << 8) | (uint32_t) address_bytes[2];
		
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": 3-byte address is 0x" << std::hex << n25q_addr << std::dec << EndDebugInfo;
		}
	}
	
	if(com_mode == unisim::kernel::tlm2::TLM_SPI_FULL_DUPLEX)
	{
		rx_data_bit_offset = tx_data_bit_offset;
	}

	if(n25q_command_has_input_data(n25q_cmd))
	{
		unsigned int data_width = n25q_data_width(n25q_cmd, n25q_protocol, n25q_mode);
		
		if(data_width != tx_width)
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master sends data bits using " << tx_width << " pin(s) (" << n25q_input_signal(tx_width) << ") while N25Q receives data bits using " << data_width << " pin(s) (" << n25q_data_signal(n25q_cmd, n25q_protocol, n25q_mode) << ") for " << n25q_cmd << EndDebugWarning;
		}
		
		if(tx_data_bit_offset >= tx_data_bit_length)
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master did not send input data bits for " << n25q_cmd << EndDebugWarning;
		}
	}
	else if(n25q_command_has_output_data(n25q_cmd))
	{
		unsigned int data_width = n25q_data_width(n25q_cmd, n25q_protocol, n25q_mode);
		
		if(data_width != tx_width)
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master receives data bits using " << tx_width << " pin(s) (" << n25q_output_signal(tx_width) << ") while N25Q sends data bits using " << data_width << " pin(s) (" << n25q_data_signal(n25q_cmd, n25q_protocol, n25q_mode) << ") for " << n25q_cmd << EndDebugWarning;
		}
		
		unsigned int dummy_cycles = DummyCycles(n25q_cmd);
		
		if(verbose && dummy_cycles)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": " << n25q_cmd << " requires " << dummy_cycles << " dummy cycles (" << (dummy_cycles * tx_width) << " dummy bits from QSPI master)" << EndDebugInfo;
		}
		
		switch(com_mode)
		{
			case unisim::kernel::tlm2::TLM_SPI_FULL_DUPLEX:
				// In full duplex mode: TX and RX are simultaneous
				rx_data_bit_offset += dummy_cycles * tx_width; // RX start after read latency
				break;
			case unisim::kernel::tlm2::TLM_SPI_HALF_DUPLEX:
				// In half duplex mode: TX followed by RX: TX phase has dummy bits to account for read latency
				if(tx_data_bit_offset + (dummy_cycles * tx_width) > tx_data_bit_length)
				{
					logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master did not send enough dummy bits for " << n25q_cmd << " resulting in a truncated response and skewed data read" << EndDebugWarning;
					
					unsigned int lost_bits = tx_data_bit_offset + (dummy_cycles * tx_width) - tx_data_bit_length;
					rx_data_bit_offset = (rx_data_bit_length > (rx_data_bit_offset + lost_bits)) ? (rx_data_bit_offset + lost_bits) : rx_data_bit_length;
					read_data_bit_offset = 0;
				}
				else if(tx_data_bit_offset + (dummy_cycles * tx_width) < tx_data_bit_length)
				{
					logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master did send too many dummy bits for " << n25q_cmd << " resulting in a truncated response and skewed data read" << EndDebugWarning;
					
					unsigned int lost_bits = tx_data_bit_length - (tx_data_bit_offset + (dummy_cycles * data_width));
					rx_data_bit_offset = 0;
					read_data_bit_offset = (rx_data_bit_length > (read_data_bit_offset + lost_bits)) ? (read_data_bit_offset + lost_bits) : rx_data_bit_length;
				}
				break;
		}
			
		if(rx_data_bit_offset >= rx_data_bit_length)
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": QSPI master has deselected N25Q too early to receive any input data bits" << EndDebugWarning;
			return 0;
		}
	}
	
	if(rx_data_bit_offset > 0)
	{
		// beginning of RX data are don't care (zeroes at the moment)
		unsigned char *rx_data_ptr = payload.get_rx_data_ptr();
		Zero(rx_data_ptr, rx_data_bit_offset, 0);
	}

	sc_core::sc_time request_time(clock_period);
	request_time *= payload.get_tx_cycles();
	sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += request_time;

	Event *event = schedule.AllocEvent();
	
	event->SetTimeStamp(notify_time_stamp);
	if(blocking_if)
	{
		event->SetPayload(&payload);
	}
	else
	{
		if(payload.has_mm())
		{
			payload.acquire();
			event->SetPayload(&payload, true);
		}
		else
		{
			payload_type *payload_clone = payload_fabric.allocate(); // payload is already acquired by allocate
			unsigned char *data_ptr_clone = new unsigned char[payload.get_data_length()];
			payload_clone->set_data_ptr(data_ptr_clone);
			payload_clone->copy_from(payload);
			event->SetPayload(payload_clone, true, true);
		}
	}
	event->SetCommand(n25q_cmd);
	event->SetAddress(n25q_addr);
	event->SetTxDataBitOffset(tx_data_bit_offset);
	event->SetRxDataBitOffset(rx_data_bit_offset);
	event->SetReadDataBitOffset(read_data_bit_offset);
	
	return event;
}

template <typename CONFIG>
const sc_core::sc_time N25Q<CONFIG>::ComputeWriteOperationTime(Event *event)
{
	payload_type *payload = event->GetPayload();
	n25q_command_type n25q_cmd = event->GetCommand();
		
	switch(n25q_cmd)
	{
		case N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND: return tWNVCR;
		case N25Q_WRITE_STATUS_REGISTER_COMMAND: return tW;
		case N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND: return tCFSR;
		case N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return tWVCR;
		case N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND: return tWRVECR;
		case N25Q_WRITE_LOCK_REGISTER_COMMAND: return tSHSL2;
		case N25Q_PROGRAM_OTP_ARRAY_COMMAND: return tPOTP;
		case N25Q_PAGE_PROGRAM_COMMAND:
		case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
		case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND: return ceil(payload->get_tx_data_length() / 8) * (W_N_VPP ? tPP_VPP : tPP);
		case N25Q_SUBSECTOR_ERASE_COMMAND: return tSSE;
		case N25Q_SECTOR_ERASE_COMMAND: return W_N_VPP ? tSE_VPP : tSE;
		case N25Q_BULK_ERASE_COMMAND: return W_N_VPP ? tBE_VPP : tBE;
		default: return sc_core::SC_ZERO_TIME;
	}
	
	return sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
unsigned int N25Q<CONFIG>::DummyCycles(n25q_command_type n25q_cmd)
{
	switch(n25q_cmd)
	{
		case N25Q_FAST_READ_COMMAND:
		case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
		case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
		case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
		case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
			{
				unsigned int number_of_dummy_clock_cycles = ICR.Number_of_dummy_clock_cycles;
				if(number_of_dummy_clock_cycles != 0) return number_of_dummy_clock_cycles;
			}
			break;
		default:
			break;
	}
	
	n25q_protocol_type qspi_protocol = GetProtocol();
	
	return n25q_dummy_cycles(n25q_cmd, qspi_protocol, n25q_mode);
}

template <typename CONFIG>
typename N25Q<CONFIG>::Event *N25Q<CONFIG>::PostEndOfWriteOperation(Event *event, const sc_core::sc_time& operation_duration)
{
	payload_type *payload = event->GetPayload();
	n25q_command_type n25q_cmd = event->GetCommand();
	uint32_t n25q_addr = event->GetAddress();
	unsigned int tx_data_bit_offset = event->GetTxDataBitOffset();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
	
	Event *end_of_operation_event = schedule.AllocEvent();
	
	sc_core::sc_time end_of_operation_time_stamp(sc_core::sc_time_stamp());
	end_of_operation_time_stamp += operation_duration;

	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << event->GetCommand() << " will finish at " << end_of_operation_time_stamp << EndDebugInfo;
	}
	
	end_of_operation_event->SetTimeStamp(end_of_operation_time_stamp);
	if(payload->has_mm())
	{
		payload->acquire();
		end_of_operation_event->SetPayload(payload, true);
	}
	else
	{
		payload_type *payload_clone = payload_fabric.allocate(); // payload is already acquired by allocate
		unsigned char *data_ptr_clone = new unsigned char[payload->get_data_length()];
		payload_clone->set_data_ptr(data_ptr_clone);
		payload_clone->copy_from(*payload);
		end_of_operation_event->SetPayload(payload_clone, true, true);
	}
	end_of_operation_event->SetCommand(n25q_cmd);
	end_of_operation_event->SetAddress(n25q_addr);
	end_of_operation_event->SetTxDataBitOffset(tx_data_bit_offset);
	end_of_operation_event->SetRxDataBitOffset(rx_data_bit_offset);
	end_of_operation_event->SetReadDataBitOffset(read_data_bit_offset);
	end_of_operation_event->SetStage(END_OF_OPERATION);
	end_of_operation_event->SetPostTime(sc_core::sc_time_stamp());
	end_of_operation_event->SetDuration(operation_duration);
	
	schedule.Notify(end_of_operation_event);
	
	return end_of_operation_event;
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadID(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
		unsigned int rx_width = payload->get_rx_width();
		n25q_signal_type rx_signals = n25q_output_signal(rx_width);
		n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

		Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, CONFIG::ID, 8 * CONFIG::ID_SIZE, read_data_bit_offset, data_signals);
	}
	else
	{
		Zero(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset);
		logger << DebugWarning << "Attempt to read ID while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::MultipleIOReadID(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
		unsigned int rx_width = payload->get_rx_width();
		n25q_signal_type rx_signals = n25q_output_signal(rx_width);
		n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

		Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, CONFIG::ID, 3 * 8, read_data_bit_offset, data_signals); // Note: Multiple I/O READ ID command can only read first 3 bytes of ID
	}
	else
	{
		Zero(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset);
		logger << DebugWarning << "Attempt to read ID while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadSFDP(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		uint32_t addr = event->GetAddress();
		unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
		unsigned int rx_data_length = payload->get_rx_data_length();
		unsigned int rx_width = payload->get_rx_width();
		n25q_signal_type rx_signals = n25q_output_signal(rx_width);
		n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

		uint8_t tmp[rx_data_length];
		
		unsigned int offset;
		unsigned int boundary = 2048; // When the 2048-byte boundary is reached,
									// the data output wraps to address 0 of the serial
									// Flash discovery parameter table.
		
		if(addr >= boundary) addr = 0;
		
		unsigned int moved = 0;
		
		for(offset = 0; offset < rx_data_length; offset += moved)
		{
			uint32_t data_to_copy = rx_data_length - offset;
			uint32_t data_to_zero = 0;
			if(data_to_copy > (CONFIG::SFDP_SIZE - addr))
			{
				data_to_zero = rx_data_length - data_to_copy;
				if(data_to_zero > (boundary - CONFIG::SFDP_SIZE)) data_to_zero = boundary - CONFIG::SFDP_SIZE;
				data_to_copy = CONFIG::SFDP_SIZE - addr;
			}
			
			memcpy(tmp + offset, CONFIG::SFDP + addr, data_to_copy);
			if(data_to_zero)
			{
				memset(tmp + offset + data_to_copy, 0, data_to_zero);
			}
			
			moved = data_to_copy + data_to_zero;
		}

		Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, tmp, rx_data_bit_length, read_data_bit_offset, data_signals);
	}
	else
	{
		Zero(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset);
		logger << DebugWarning << "Attempt to read serial flash discovery parameters while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Read(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		uint32_t addr = event->GetAddress();
		unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
		unsigned int rx_width = payload->get_rx_width();
		n25q_signal_type rx_signals = n25q_output_signal(rx_width);
		n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);
		unsigned int data_width = n25q_signal_width(data_signals);
		unsigned int data_cycles = (rx_data_bit_length - rx_data_bit_offset) / rx_width;

		unsigned int read_bit_length = data_cycles * data_width;
		unsigned int read_length = (read_bit_length + 7) / 8;
		
		uint8_t tmp[read_length];
		
		unsigned int offset = 0;
		uint32_t wrap = GetWrap();
		
		unsigned int copied = 0;
		
		for(offset = 0; offset < read_length; offset += copied)
		{
			uint32_t wrapped_addr_offset = addr & (wrap - 1);
			uint32_t size_to_copy = wrap - wrapped_addr_offset;
			if(size_to_copy > (read_length - offset)) size_to_copy = read_length - offset;
			if(size_to_copy > (CONFIG::SIZE - addr)) size_to_copy = CONFIG::SIZE - addr;
			
			if(verbose)
			{
				logger << DebugInfo << "Reading " << (8 * size_to_copy) << " bits at bit offset 0x" << std::hex << (8 * addr) << std::dec << EndDebugInfo;
			}
			// During a PROGRAM SUSPEND operation, a READ operation is possible in any page except the one in a suspended state.
			// Reading from a page that is in a suspended state will output indeterminate data.
			// During an ERASE SUSPEND operation, a PROGRAM or READ operation is possible in any sector except the one in a suspended state.
			// Reading from a sector that is in a suspended state will output indeterminate data.
			if(IsProgramOrEraseSuspendedFor(addr))
			{
				logger << DebugWarning << "Attempt to read data at 0x" << addr << " from a page/sector that is in suspend state" << EndDebugWarning;
			}
			memcpy(tmp + offset, storage + addr, size_to_copy);
			
			copied = size_to_copy;
			
			if(wrapped_addr_offset)
			{
				addr += copied;
			}
		}
		
		Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, tmp, read_bit_length, read_data_bit_offset, data_signals);
	}
	else
	{
		Zero(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset);
		logger << DebugWarning << "Attempt to read while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::FastRead(Event *event)
{
	uint8_t xip_confirmation_bit = 0;
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		payload_type *payload = event->GetPayload();
		n25q_command_type n25q_cmd = event->GetCommand();
		unsigned int tx_data_bit_length = payload->get_tx_data_bit_length();
		unsigned char *tx_data_ptr = payload->get_tx_data_ptr();
		unsigned int tx_data_bit_offset = event->GetTxDataBitOffset();
		
		unsigned int tx_width = payload->get_tx_width();
		n25q_signal_type tx_signals = n25q_input_signal(tx_width);

		if((tx_data_bit_offset + (1 * tx_width)) > tx_data_bit_length)
		{
			std::stringstream sstr_cmd;
			sstr_cmd << event->GetCommand();
			logger << DebugWarning << "Not enough dummy bits for XIP confirmation bit of " << sstr_cmd.str() << EndDebugWarning;
			return;
		}
		
		Xfer(&xip_confirmation_bit, 1, 0, DQ0, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);

		if(verbose)
		{
			logger << DebugInfo << "XIP confirmation bit is " << (unsigned int)((xip_confirmation_bit >> 7) & 1) << EndDebugInfo;
		}

		unsigned int dummy_cycles = DummyCycles(n25q_cmd);
		
		tx_data_bit_offset += dummy_cycles * tx_width;
		
		event->SetTxDataBitOffset(tx_data_bit_offset);
	}
	
	Read(event);
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		switch(n25q_mode)
		{
			case N25Q_STD_MODE:
				// Standard mode
				if(((CONFIG::XIP_F == CONFIG::BASIC_XIP) || !VCR.XIP) && !xip_confirmation_bit) // XIP=0 and Xb=0
				{
					n25q_mode = N25Q_XIP_MODE;
					n25q_xip_cmd = event->GetCommand();
				}
				break;
			case N25Q_XIP_MODE:
				// XIP mode
				if(xip_confirmation_bit) // XIP confirmation bit=1
				{
					// XIP is terminated by driving the XIP confirmation bit to 1.
					n25q_mode = N25Q_STD_MODE;
					
					// The device automatically resets volatile configuration register bit 3 to 1.
					VCR.XIP = 1;
				}
				break;
		}
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteEnable()
{
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		SR.Write_enable_latch = 1;
	}
	else
	{
		logger << DebugWarning << "Attempt to run " << N25Q_WRITE_ENABLE_COMMAND << " while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteDisable()
{
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		SR.Write_enable_latch = 0;
	}
	else
	{
		logger << DebugWarning << "Attempt to run " << N25Q_WRITE_DISABLE_COMMAND << " while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadStatusRegister(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	unsigned int rx_data_length = payload->get_rx_data_length();
	unsigned int rx_width = payload->get_rx_width();
	n25q_signal_type rx_signals = n25q_output_signal(rx_width);
	n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

	uint8_t tmp[rx_data_length];
	
	uint8_t sr_value = SR;
	memset(tmp, sr_value, rx_data_length);
	
	Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, tmp, rx_data_bit_length, read_data_bit_offset, data_signals);

}

template <typename CONFIG>
void N25Q<CONFIG>::ReadFlagStatusRegister(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	unsigned int rx_data_length = payload->get_rx_data_length();
	unsigned int rx_width = payload->get_rx_width();
	n25q_signal_type rx_signals = n25q_output_signal(rx_width);
	n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

	uint8_t tmp[rx_data_length];
	
	uint8_t fsr_value = FSR;
	memset(tmp, fsr_value, rx_data_length);
	
	Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, tmp, rx_data_bit_length, read_data_bit_offset, data_signals);
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadNonVolatileConfigurationRegister(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();

	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
		unsigned int rx_data_length = payload->get_rx_data_length();
		unsigned int rx_width = payload->get_rx_width();
		n25q_signal_type rx_signals = n25q_output_signal(rx_width);
		n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

		uint8_t tmp[rx_data_length];
		
		uint16_t nvcr_value_value = FSR;
		unsigned int i;
		// continuously sends NVCR value (MSB followed by LSB)
		for(i = 0; i < rx_data_length; i++)
		{
			tmp[i] = (i & 1) ? nvcr_value_value : (nvcr_value_value >> 8);
		}
		
		Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, tmp, rx_data_bit_length, read_data_bit_offset, data_signals);
	}
	else
	{
		Zero(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset);
		logger << DebugWarning << "Attempt to read Nonvolatile configuration register while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadVolatileConfigurationRegister(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
		unsigned int rx_data_length = payload->get_rx_data_length();
		unsigned int rx_width = payload->get_rx_width();
		n25q_signal_type rx_signals = n25q_output_signal(rx_width);
		n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

		uint8_t tmp[rx_data_length];
		
		uint8_t vcr_value = VCR;
		memset(tmp, vcr_value, rx_data_length);
		
		Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, tmp, rx_data_bit_length, read_data_bit_offset, data_signals);
	}
	else
	{
		Zero(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset);
		logger << DebugWarning << "Attempt to read volatile configuration register while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadEnhancedVolatileConfigurationRegister(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
		unsigned int rx_data_length = payload->get_rx_data_length();
		unsigned int rx_width = payload->get_rx_width();
		n25q_signal_type rx_signals = n25q_output_signal(rx_width);
		n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

		uint8_t tmp[rx_data_length];
		
		uint8_t vecr_value = VECR;
		memset(tmp, vecr_value, rx_data_length);
		
		Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, tmp, rx_data_bit_length, read_data_bit_offset, data_signals);
	}
	else
	{
		Zero(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset);
		logger << DebugWarning << "Attempt to read Enhanced volatile configuration register while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadLockRegister(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		uint32_t addr = event->GetAddress();
		unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
		unsigned int rx_data_length = payload->get_rx_data_length();
		unsigned int rx_width = payload->get_rx_width();
		n25q_signal_type rx_signals = n25q_output_signal(rx_width);
		n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

		uint8_t tmp[rx_data_length];
		
		uint8_t lr_value = (addr < CONFIG::SIZE) ? (const uint8_t) *LR[addr / CONFIG::SECTOR_SIZE] : 0;
		unsigned int i;
		for(i = 0; i < rx_data_length; i++)
		{
			tmp[i] = lr_value;
		}
		
		Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, tmp, rx_data_bit_length, read_data_bit_offset, data_signals);
	}
	else
	{
		Zero(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset);
		logger << DebugWarning << "Attempt to read lock register while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ReadOTPArray(Event *event)
{
	payload_type *payload = event->GetPayload();
	unsigned int rx_data_bit_length = payload->get_rx_data_bit_length();
	unsigned int rx_data_bit_offset = event->GetRxDataBitOffset();
	unsigned char *rx_data_ptr = payload->get_rx_data_ptr();
	
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		uint32_t addr = event->GetAddress();
		unsigned int read_data_bit_offset = event->GetReadDataBitOffset();
		unsigned int rx_data_length = payload->get_rx_data_length();
		unsigned int rx_width = payload->get_rx_width();
		n25q_signal_type rx_signals = n25q_output_signal(rx_width);
		n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);

		uint8_t tmp[rx_data_length];

		if(addr < CONFIG::OTP_ARRAY_SIZE)
		{
			unsigned int data_to_copy = rx_data_length;
			if(data_to_copy > (CONFIG::OTP_ARRAY_SIZE - addr)) data_to_copy = CONFIG::OTP_ARRAY_SIZE - addr;
			
			memcpy(tmp, OTP + addr, data_to_copy);
			
			if(rx_data_length > data_to_copy)
			{
				unsigned int dup_length = rx_data_length - data_to_copy;
				memset(tmp + data_to_copy, OTP[CONFIG::OTP_ARRAY_SIZE - 1], dup_length);
			}
		}
		else
		{
			memset(tmp, 0, rx_data_length);
		}
		
		Xfer(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset, rx_signals, tmp, rx_data_bit_length, read_data_bit_offset, data_signals);
	}
	else
	{
		Zero(rx_data_ptr, rx_data_bit_length - rx_data_bit_offset, rx_data_bit_offset);
		logger << DebugWarning << "Attempt to read OTP array while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteNonVolatileConfigurationRegister(Event *event)
{
	payload_type *payload = event->GetPayload();
	
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			if(SR.Write_enable_latch) // write enable ?
			{
				if(!SR.Write_in_progress) // ready ?
				{
					if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
					{
						if(!IsEraseSuspended()) // Erase suspend state ?
						{
							if(!IsSubSectorEraseOrProgramSuspended()) // Subsector Erase or Suspend Program Suspend State ?
							{
								PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
								// When the operation is in progress, the write in progress bit is set to 1.
								SR.Write_in_progress = 1;
								FSR.Program_or_erase_controller = 0; // busy
							}
							else
							{
								logger << DebugWarning << "Attempt to write Nonvolatile configuration register while in Subsector Erase Suspend or Program Suspend State" << EndDebugWarning;
							}
						}
						else
						{
							logger << DebugWarning << "Attempt to write Nonvolatile configuration register while in Erase Suspend State" << EndDebugWarning;
						}
					}
					else
					{
						logger << DebugWarning << "Attempt to write non volatile configuration register while program or erase controller is busy (PROGRAM, ERASE, WRITE STATUS REGISTER, or WRITE NON VOLATILE CONFIGURATION command cycle is in progress)" << EndDebugWarning;
					}
				}
				else
				{
					logger << DebugWarning << "Attempt to write Nonvolatile configuration register while a write is in progress (WRITE STATUS REGISTER, WRITE NONVOLATILE CONFIGURATION REGISTER, PROGRAM, ERASE command cycle is in progress)" << EndDebugWarning;
				}
				// The write enable latch bit is cleared to 0, whether the operation is successful or not.
				SR.Write_enable_latch = 0;
			}
			else
			{
				logger << DebugWarning << "Attempt to write " << NVCR.GetFriendlyName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
			}
			break;
			
		case END_OF_OPERATION:
			{
				unsigned char *tx_data_ptr = payload->get_tx_data_ptr();
				unsigned int tx_data_bit_length = payload->get_tx_data_bit_length();
				unsigned int tx_data_bit_offset = event->GetTxDataBitOffset();

				unsigned int tx_width = payload->get_tx_width();
				n25q_signal_type tx_signals = n25q_input_signal(tx_width);
				n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);
				unsigned int data_width = n25q_signal_width(data_signals);
				unsigned int data_cycles = 8 * 2 / data_width;

				if((tx_data_bit_offset + (data_cycles * tx_width)) > tx_data_bit_length)
				{
					logger << DebugWarning << "not enough data bits for " << N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND << EndDebugWarning;
				}
				else if((tx_data_bit_offset + (data_cycles * tx_width)) < tx_data_bit_length)
				{
					logger << DebugWarning << "too many data bits for " << N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND << EndDebugWarning;
				}
				else
				{
					// A WRITE NONVOLATILE CONFIGURATION REGISTER operation requires data being sent
					// starting from least significant byte.
					uint8_t data_bytes[2];
					Xfer(data_bytes, 16, 0, data_signals, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
					uint16_t new_nvcr_value = (uint16_t) data_bytes[0] | ((uint16_t) data_bytes[1] << 8);
					NVCR.Write(new_nvcr_value);
				}
				// The write enable latch bit is cleared to 0, whether the operation is successful or not.
				SR.Write_enable_latch = 0;
				// When the operation completes, the write in progress bit is cleared to 0, whether the operation is successful or not.
				SR.Write_in_progress = 0;
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteVolatileConfigurationRegister(Event *event)
{
	payload_type *payload = event->GetPayload();

	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			if(SR.Write_enable_latch) // write enable ?
			{
				if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
				{
					PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
					// Note: it is not specified whether SR.Write_enable_latch is cleared
				}
				else
				{
					logger << DebugWarning << "Attempt to run " << N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND << " while in program or erase state" << EndDebugWarning;
				}
			}
			else
			{
				logger << DebugWarning << "Attempt to write " << VCR.GetFriendlyName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
			}
			break;
		case END_OF_OPERATION:
			{
				unsigned char *tx_data_ptr = payload->get_tx_data_ptr();
				unsigned int tx_data_bit_length = payload->get_tx_data_bit_length();
				unsigned int tx_data_bit_offset = event->GetTxDataBitOffset();

				unsigned int tx_width = payload->get_tx_width();
				n25q_signal_type tx_signals = n25q_input_signal(tx_width);
				n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);
				unsigned int data_width = n25q_signal_width(data_signals);
				unsigned int data_cycles = 8 * 1 / data_width;

				if((tx_data_bit_offset + (data_cycles * tx_width)) > tx_data_bit_length)
				{
					logger << DebugWarning << "not enough data bits for " << N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND << EndDebugWarning;
				}
				else if((tx_data_bit_offset + (data_cycles * tx_width)) < tx_data_bit_length)
				{
					logger << DebugWarning << "too many data bits for " << N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND << EndDebugWarning;
				}
				else
				{
					uint8_t data_byte;
					Xfer(&data_byte, 8, 0, data_signals, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
					uint8_t new_vcr_value = data_byte;
					
					VCR.Write(new_vcr_value);
					ICR.Number_of_dummy_clock_cycles = VCR.Number_of_dummy_clock_cycles;
					ICR.Wrap = VCR.Wrap;
				}
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteEnhancedVolatileConfigurationRegister(Event *event)
{
	payload_type *payload = event->GetPayload();
	
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			if(SR.Write_enable_latch) // write enable ?
			{
				if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
				{
					PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
					// Note: it is not specified whether SR.Write_enable_latch is cleared
				}
				else
				{
					logger << DebugWarning << "Attempt to run " << N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND << " while in program or erase state" << EndDebugWarning;
				}
			}
			else
			{
				logger << DebugWarning << "Attempt to write " << VECR.GetFriendlyName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
			}
			break;
			
		case END_OF_OPERATION:
			{
				unsigned char *tx_data_ptr = payload->get_tx_data_ptr();
				unsigned int tx_data_bit_length = payload->get_tx_data_bit_length();
				unsigned int tx_data_bit_offset = event->GetTxDataBitOffset();

				unsigned int tx_width = payload->get_tx_width();
				n25q_signal_type tx_signals = n25q_input_signal(tx_width);
				n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);
				unsigned int data_width = n25q_signal_width(data_signals);
				unsigned int data_cycles = 8 * 1 / data_width;

				if((tx_data_bit_offset + (data_cycles * tx_width)) > tx_data_bit_length)
				{
					logger << DebugWarning << "not enough data bits for " << N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND << EndDebugWarning;
				}
				else if((tx_data_bit_offset + (data_cycles * tx_width)) < tx_data_bit_length)
				{
					logger << DebugWarning << "too many data bits for " << N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND << EndDebugWarning;
				}
				else
				{
					uint8_t data_byte;
					Xfer(&data_byte, 8, 0, data_signals, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
					uint8_t new_vecr_value = data_byte;
	
					VECR.Write(new_vecr_value);
					ICR.Output_driver_strength = VECR.Output_driver_strength;
					ICR.VPP_accelerator = VECR.VPP_accelerator;
					ICR.Reset_hold = VECR.Reset_hold;
					ICR.Quad_IO_protocol = VECR.Quad_IO_protocol;
					ICR.Dual_IO_protocol = VECR.Dual_IO_protocol;
				}
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteStatusRegister(Event *event)
{
	payload_type *payload = event->GetPayload();

	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			if(SR.Write_enable_latch) // write enable ?
			{
				if(!SR.Write_in_progress) // ready ?
				{
					if(!IsEraseSuspended()) // Erase suspend state ?
					{
						if(!IsSubSectorEraseOrProgramSuspended()) // Subsector Erase or Suspend Program Suspend State ?
						{
							if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
							{
								PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
								// When the operation is in progress, the write in progress bit is set to 1.
								SR.Write_in_progress = 1;
								FSR.Program_or_erase_controller = 0; // busy
							}
							else
							{
								logger << DebugWarning << "Attempt to write status register while program or erase controller is busy (PROGRAM, ERASE, WRITE STATUS REGISTER, or WRITE NON VOLATILE CONFIGURATION command cycle is in progress)" << EndDebugWarning;
								// The write enable latch bit is cleared to 0, whether the operation is successful or not.
								SR.Write_enable_latch = 0;
							}
						}
						else
						{
							logger << DebugWarning << "Attempt to write status register while in Subsector Erase Suspend or Program Suspend State" << EndDebugWarning;
						}
					}
					else
					{
						logger << DebugWarning << "Attempt to write status register while in Erase Suspend State" << EndDebugWarning;
					}
				}
				else
				{
					logger << DebugWarning << "Attempt to write status register while a write is in progress (WRITE STATUS REGISTER, WRITE NONVOLATILE CONFIGURATION REGISTER, PROGRAM, ERASE command cycle is in progress)" << EndDebugWarning;
					// The write enable latch bit is cleared to 0, whether the operation is successful or not.
					SR.Write_enable_latch = 0;
				}
			}
			else
			{
				logger << DebugWarning << "Attempt to write " << SR.GetFriendlyName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				// The write enable latch bit is cleared to 0, whether the operation is successful or not.
				SR.Write_enable_latch = 0;
			}
			break;
		case END_OF_OPERATION:
			{
				// When the operation completes, the write in progress bit is cleared to 0, whether the operation is successful or not.
				SR.Write_in_progress = 0;
				// The write enable latch bit is cleared to 0, whether the operation is successful or not.
				SR.Write_enable_latch = 0;
				
				unsigned char *tx_data_ptr = payload->get_tx_data_ptr();
				unsigned int tx_data_bit_length = payload->get_tx_data_bit_length();
				unsigned int tx_data_bit_offset = event->GetTxDataBitOffset();

				unsigned int tx_width = payload->get_tx_width();
				n25q_signal_type tx_signals = n25q_input_signal(tx_width);
				n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);
				unsigned int data_width = n25q_signal_width(data_signals);
				unsigned int data_cycles = 8 * 1 / data_width;

				if((tx_data_bit_offset + (data_cycles * tx_width)) > tx_data_bit_length)
				{
					logger << DebugWarning << "not enough data bits for " << N25Q_WRITE_STATUS_REGISTER_COMMAND << EndDebugWarning;
				}
				else if((tx_data_bit_offset + (data_cycles * tx_width)) < tx_data_bit_length)
				{
					logger << DebugWarning << "too many data bits for " << N25Q_WRITE_STATUS_REGISTER_COMMAND << EndDebugWarning;
				}
				else
				{
					uint8_t data_byte;
					Xfer(&data_byte, 8, 0, data_signals, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
					uint8_t new_sr_value = data_byte;
					SR.Write(new_sr_value);
				}
				
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::WriteLockRegister(Event *event)
{
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				payload_type *payload = event->GetPayload();
				uint32_t addr = event->GetAddress();
				unsigned int tx_data_bit_length = payload->get_tx_data_bit_length();
				unsigned int tx_data_bit_offset = event->GetTxDataBitOffset();
				unsigned char *tx_data_ptr = payload->get_tx_data_ptr();
				
				unsigned int tx_width = payload->get_tx_width();
				n25q_signal_type tx_signals = n25q_input_signal(tx_width);
				n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);
				unsigned int data_width = n25q_signal_width(data_signals);
				unsigned int data_cycles = 8 * 1 / data_width;

				LockRegister *lr = LR[addr / CONFIG::SECTOR_SIZE];

				if(SR.Write_enable_latch) // write enable ?
				{
					if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
					{
						if(!lr->Sector_lock_down)
						{
							// Because lock register bits are volatile, change to the bits is immediate.
							if((tx_data_bit_offset + (data_cycles * tx_width)) > tx_data_bit_length)
							{
								logger << DebugWarning << "not enough data bits for " << N25Q_WRITE_LOCK_REGISTER_COMMAND << EndDebugWarning;
							}
							else if((tx_data_bit_offset + (data_cycles * tx_width)) < tx_data_bit_length)
							{
								logger << DebugWarning << "too many data bits for " << N25Q_WRITE_LOCK_REGISTER_COMMAND << EndDebugWarning;
							}
							else
							{
								uint8_t data_byte;
								Xfer(&data_byte, 8, 0, data_signals, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
								uint8_t new_lr_value = data_byte;
								lr->Write(new_lr_value);
							}
							
							PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
						}
						else
						{
							logger << DebugWarning << "Attempt to write " << lr->GetFriendlyName() << " while " << lr->Sector_lock_down.GetName() << " = '1'" << EndDebugWarning;
						}
					}
					else
					{
						logger << DebugWarning << "Attempt to run " << N25Q_WRITE_LOCK_REGISTER_COMMAND << " while in program or erase state" << EndDebugWarning;
					}
				}
				else
				{
					logger << DebugWarning << "Attempt to write " << lr->GetFriendlyName() << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
			
		case END_OF_OPERATION:
			// When execution is complete, the write enable latch bit is cleared within tSHSL2 and no error bits are set.
			SR.Write_enable_latch = 0;
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ClearFlagStatusRegister()
{
	if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
	{
		FSR.Erase = 0;
		FSR.Program = 0;
		FSR.Protection = 0;
	}
	else
	{
		logger << DebugWarning << "Attempt to run " << N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND << " while in program or erase state" << EndDebugWarning;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ProgramOTPArray(Event *event)
{
	payload_type *payload = event->GetPayload();

	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch) // write enable ?
				{
					if(!SR.Write_in_progress) // ready (not in Program or Erase State) ?
					{
						if(!IsEraseSuspended()) // Erase suspend state ?
						{
							if(!IsSubSectorEraseOrProgramSuspended()) // Subsector Erase or Suspend Program Suspend State ?
							{
								if(OTP[CONFIG::OTP_ARRAY_SIZE] & 1)
								{
									// unlocked
									PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
									
									// When the operation is in progress, the write in progress bit is set to 1.
									SR.Write_in_progress = 1;
								}
								else
								{
									// permanently locked
									logger << DebugWarning << "Attempt to program permanently locked OTP Array" << EndDebugWarning;
								}
							}
							else
							{
								logger << DebugWarning << "Attempt to program OTP array while in Subsector Erase Suspend or Program Suspend State" << EndDebugWarning;
							}
						}
						else
						{
							logger << DebugWarning << "Attempt to program OTP array while in Erase Suspend State" << EndDebugWarning;
						}
					}
					else
					{
						logger << DebugWarning << "Attempt to program OTP array while a write is in progress (WRITE STATUS REGISTER, WRITE NONVOLATILE CONFIGURATION REGISTER, PROGRAM, ERASE command cycle is in progress)" << EndDebugWarning;
					}
					// The write enable latch bit is cleared to 0, whether the operation is successful or not.
					SR.Write_enable_latch = 0;
				}
				else
				{
					// To initiate the PROGRAM OTP ARRAY command, the WRITE ENABLE command must
					// be issued to set the write enable latch bit to 1; otherwise, the PROGRAM OTP ARRAY
					// command is ignored and flag status register bits are not set.
					logger << DebugWarning << "Attempt to program OTP Array while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		case END_OF_OPERATION:
			{
				unsigned char *tx_data_ptr = payload->get_tx_data_ptr();
				unsigned int tx_data_bit_length = payload->get_tx_data_bit_length();
				unsigned int tx_data_bit_offset = event->GetTxDataBitOffset();
				unsigned int tx_width = payload->get_tx_width();
				n25q_signal_type tx_signals = n25q_input_signal(tx_width);
				n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);
				uint32_t addr = event->GetAddress();

				if(tx_data_bit_offset >= tx_data_bit_length)
				{
					logger << DebugWarning << "not data bits for " << N25Q_PROGRAM_OTP_ARRAY_COMMAND << EndDebugWarning;
				}
				else
				{
					if(addr <= CONFIG::OTP_ARRAY_SIZE)
					{
						unsigned int max_bytes_to_copy = CONFIG::OTP_ARRAY_SIZE + 1 - addr;
						unsigned int max_bits_to_copy = 8 * max_bytes_to_copy;
						unsigned int data_bits_to_copy = tx_data_bit_length - tx_data_bit_offset;
						if(data_bits_to_copy > max_bits_to_copy) data_bits_to_copy = max_bits_to_copy;
						
						ProgramOTP(data_bits_to_copy, 8 * addr, data_signals, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
					}
				}
				
				// When the operation completes, the write in progress bit is cleared to 0.
				SR.Write_in_progress = 0;
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Program(Event *event)
{
	payload_type *payload = event->GetPayload();
	uint32_t addr = event->GetAddress();

	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch) // write enable ?
				{
					if(!SR.Write_in_progress) // ready
					{
						if(FSR.Program_or_erase_controller) // ready
						{
							if(!IsSubSectorEraseOrProgramSuspended())
							{
								if(!IsEraseSuspendedFor(addr))
								{
									if(!IsSectorProtected(addr))
									{
										Event *end_of_operation_event = PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
										program_or_erase_operation_in_progress_event = end_of_operation_event;
										
										// When the operation is in progress, the write in progress bit is set to 1.
										SR.Write_in_progress = 1;
										FSR.Program_or_erase_controller = 0; // busy
									}
									else
									{
										// When a command is applied to a protected sector,
										// the command is not executed, the write enable latch bit remains
										// set to 1, and flag status register bits 1 and 4 are set.
										FSR.Protection = 1; // protection error
										FSR.Program = 1; // protection error
										logger << DebugWarning << "Attempt to program a page within a protected sector" << EndDebugWarning;
									}
								}
								else
								{
									logger << DebugWarning << "Attempt to program a sector at 0x" << addr << " that is in ERASE SUSPEND state" << EndDebugWarning;
									
									// The device ignores a PROGRAM command to a sector that is in an ERASE SUSPEND state;
									// it also sets the flag status register bit 4 to 1: program failure/protection error,
									// and leaves the write enable latch bit unchanged.
									FSR.Program = 1; // program failure
								}
							}
							else
							{
								logger << DebugWarning << "Attempt to program a page while in Subsector Erase Suspend or Program Suspend State" << EndDebugWarning;
							}
						}
						else
						{
							logger << DebugWarning << "Attempt to program a page while program or erase controller is busy (PROGRAM, ERASE, WRITE STATUS REGISTER, or WRITE NON VOLATILE CONFIGURATION command cycle is in progress)" << EndDebugWarning;
						}
					}
					else
					{
						logger << DebugWarning << "Attempt to program while a write is in progress (WRITE STATUS REGISTER, WRITE NONVOLATILE CONFIGURATION REGISTER, PROGRAM, ERASE command cycle is in progress)" << EndDebugWarning;
					}
					
					// The write enable latch bit is cleared to 0, whether the operation is successful or not.
					SR.Write_enable_latch = 0;
				}
				else
				{
					logger << DebugWarning << "Attempt to program while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
			
		case END_OF_OPERATION:
			{
				unsigned char *tx_data_ptr = payload->get_tx_data_ptr();
				unsigned int tx_data_bit_length = payload->get_tx_data_bit_length();
				unsigned int tx_data_bit_offset = event->GetTxDataBitOffset();
				unsigned int tx_width = payload->get_tx_width();
				n25q_signal_type tx_signals = n25q_input_signal(tx_width);
				n25q_signal_type data_signals = n25q_data_signal(event->GetCommand(), GetProtocol(), n25q_mode);
				unsigned int data_width = n25q_signal_width(data_signals);
				
				if(tx_data_bit_offset >= tx_data_bit_length)
				{
					logger << DebugWarning << "not data bits for " << event->GetCommand() << EndDebugWarning;
				}
				else
				{
					unsigned int data_cycles = (tx_data_bit_length - tx_data_bit_offset) / tx_width;
					unsigned int data_bits_to_program = data_cycles * data_width;
					
					sc_dt::uint64 page_offset = addr & (CONFIG::PAGE_SIZE - 1);
			
					//unsigned int data_bits_to_program = tx_data_bit_length - tx_data_bit_offset;
					if(data_bits_to_program > (8 * CONFIG::PAGE_SIZE))
					{
						// If the number of bytes sent to the device exceed the maximum page size,
						// previously latched data is discarded and only the last maximum page size
						// number of data bytes are guaranteed to be programmed correctly within the same page.
						//tx_data_bit_offset = tx_data_bit_length - (8 * CONFIG::PAGE_SIZE); // dicarding first bytes
						data_bits_to_program = 8 * CONFIG::PAGE_SIZE;
						data_cycles = data_bits_to_program / data_width;
						tx_data_bit_offset = tx_data_bit_length - (tx_width * data_cycles); // dicarding first bytes
					}
					
					sc_dt::uint64 bitsize_to_page_boundary = 8 * (CONFIG::PAGE_SIZE - page_offset);
					
					if(page_offset && (data_bits_to_program > bitsize_to_page_boundary))
					{
						// If the bits of the least significant address, which is the starting address, are not all zero,
						// all data transmitted beyond the end of the current page is programmed from the starting address of the same page.
						sc_dt::uint64 page_base_addr = addr & ~(CONFIG::PAGE_SIZE - 1);
						
						unsigned int data_cycles_to_page_boundary = bitsize_to_page_boundary / data_width;
						ProgramStorage(bitsize_to_page_boundary, 8 * addr, data_signals, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
						ProgramStorage(data_bits_to_program - bitsize_to_page_boundary, 8 * page_base_addr, data_signals, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset + (data_cycles_to_page_boundary * tx_width), tx_signals);
					}
					else
					{
						ProgramStorage(data_bits_to_program, 8 * addr, data_signals, tx_data_ptr, tx_data_bit_length, tx_data_bit_offset, tx_signals);
					}
				}
					
				// When the operation completes, the write in progress bit is cleared to 0.
				SR.Write_in_progress = 0;
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::SubSectorErase(Event *event)
{
	uint32_t addr = event->GetAddress();
	
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch) // Write enable ?
				{
					if(!SR.Write_in_progress) // ready ?
					{
						if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
						{
							if(!IsEraseSuspended()) // Erase suspend state ?
							{
								if(!IsSubSectorEraseOrProgramSuspended()) // Subsector Erase or Suspend Program Suspend State ?
								{
									if(!IsSectorProtected(addr)) // Sector is protected ?
									{
										Event *end_of_operation_event = PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
										program_or_erase_operation_in_progress_event = end_of_operation_event;
										// When the operation is in progress, the write in progress bit is set to 1. 
										SR.Write_in_progress = 1;
										FSR.Program_or_erase_controller = 0; // busy
									}
									else
									{
										// When a command is applied to a protected subsector, the command is not executed.
										// Instead, the write enable latch bit remains set to 1, and flag status register bits 1 and 5 are set.
										FSR.Protection = 1; // protection error
										FSR.Erase = 1; // protection error
										logger << DebugWarning << "Attempt to erase a subsector within a protected sector" << EndDebugWarning;
									}
								}
								else
								{
									logger << DebugWarning << "Attempt to erase a subsector at 0x" << addr << " while in Subsector Erase Suspend or Program Suspend State" << EndDebugWarning;
								}
							}
							else
							{
								logger << DebugWarning << "Attempt to erase a subsector at 0x" << addr << " while in Erase Suspend State" << EndDebugWarning;
							}
						}
						else
						{
							logger << DebugWarning << "Attempt to erase a subsector while program or erase controller is busy (PROGRAM, ERASE, WRITE STATUS REGISTER, or WRITE NON VOLATILE CONFIGURATION command cycle is in progress)" << EndDebugWarning;
						}
					}
					else
					{
						logger << DebugWarning << "Attempt to erase subsector while a write is in progress (WRITE STATUS REGISTER, WRITE NONVOLATILE CONFIGURATION REGISTER, PROGRAM, ERASE command cycle is in progress)" << EndDebugWarning;
					}
					
					// The write enable latch bit is cleared to 0, whether the operation is successful or not.
					SR.Write_enable_latch = 0;
				}
				else
				{
					// If the write enable latch bit is not set, the device ignores the SUBSECTOR ERASE command
					// and no error bits are set to indicate operation failure.
					logger << DebugWarning << "Attempt to perform " << N25Q_SUBSECTOR_ERASE_COMMAND << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		
		case END_OF_OPERATION:
			{
				sc_dt::uint64 sector_base_addr = addr & ~(CONFIG::SUBSECTOR_SIZE - 1);
				EraseStorage(sector_base_addr, CONFIG::SUBSECTOR_SIZE);
				// When the operation completes, the write in progress bit is cleared to 0.
				SR.Write_in_progress = 0;
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::SectorErase(Event *event)
{
	uint32_t addr = event->GetAddress();
	
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch) // Write enable ?
				{
					if(!SR.Write_in_progress) // ready ?
					{
						if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
						{
							if(!IsEraseSuspended()) // Erase suspend state ?
							{
								if(!IsSubSectorEraseOrProgramSuspended()) // Subsector Erase or Suspend Program Suspend State ?
								{
									if(!IsSectorProtected(addr)) // Sector is protected ?
									{
										Event *end_of_operation_event = PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
										program_or_erase_operation_in_progress_event = end_of_operation_event;
										// When the operation is in progress, the write in progress bit is set to 1. 
										SR.Write_in_progress = 1;
										FSR.Program_or_erase_controller = 0; // busy
									}
									else
									{
										// When a command is applied to a protected subsector, the command is not executed.
										// Instead, the write enable latch bit remains set to 1, and flag status register bits 1 and 5 are set.
										FSR.Protection = 1; // protection error
										FSR.Erase = 1; // protection error
										logger << DebugWarning << "Attempt to erase a protected sector" << EndDebugWarning;
									}
								}
								else
								{
									logger << DebugWarning << "Attempt to erase a sector at 0x" << addr << " while in Subsector Erase Suspend or Program Suspend State" << EndDebugWarning;
								}
							}
							else
							{
								logger << DebugWarning << "Attempt to erase a sector at 0x" << addr << " while in Erase Suspend State" << EndDebugWarning;
							}
						}
						else
						{
							logger << DebugWarning << "Attempt to erase a sector while program or erase controller is busy (PROGRAM, ERASE, WRITE STATUS REGISTER, or WRITE NON VOLATILE CONFIGURATION command cycle is in progress)" << EndDebugWarning;
						}
					}
					else
					{
						logger << DebugWarning << "Attempt to erase sector while a write is in progress (WRITE STATUS REGISTER, WRITE NONVOLATILE CONFIGURATION REGISTER, PROGRAM, ERASE command cycle is in progress)" << EndDebugWarning;
					}
					
					// The write enable latch bit is cleared to 0, whether the operation is successful or not.
					SR.Write_enable_latch = 0;
				}
				else
				{
					// If the write enable latch bit is not set, the device ignores the SECTOR ERASE command
					// and no error bits are set to indicate operation failure.
					logger << DebugWarning << "Attempt to perform " << N25Q_SECTOR_ERASE_COMMAND << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		
		case END_OF_OPERATION:
			{
				sc_dt::uint64 sector_base_addr = addr & ~(CONFIG::SECTOR_SIZE - 1);
				EraseStorage(sector_base_addr, CONFIG::SECTOR_SIZE);
				// When the operation completes, the write in progress bit is cleared to 0.
				SR.Write_in_progress = 0;
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::BulkErase(Event *event)
{
	switch(event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(SR.Write_enable_latch) // write enable ?
				{
					if(!SR.Write_in_progress) // ready ?
					{
						if(FSR.Program_or_erase_controller) // ready (not in Program or Erase State) ?
						{
							if(!IsEraseSuspended()) // Erase suspend state ?
							{
								if(!IsSubSectorEraseOrProgramSuspended()) // Subsector Erase or Suspend Program Suspend State ?
								{
									if(!IsDeviceProtected()) // Is Device protected (even partially) ?
									{
										Event *end_of_operation_event = PostEndOfWriteOperation(event, ComputeWriteOperationTime(event));;
										program_or_erase_operation_in_progress_event = end_of_operation_event;
										// When the operation is in progress, the write in progress bit is set to 1. 
										SR.Write_in_progress = 1;
										FSR.Program_or_erase_controller = 0; // busy
									}
									else
									{
										// When a command is applied to a protected subsector, the command is not executed.
										// Instead, the write enable latch bit remains set to 1, and flag status register bits 1 and 5 are set.
										FSR.Protection = 1; // protection error
										FSR.Erase = 1; // protection error
										logger << DebugWarning << "Attempt to erase a protected sector during a bulk erase" << EndDebugWarning;
									}
								}
								else
								{
									logger << DebugWarning << "Attempt to bulk erase while in Subsector Erase Suspend or Program Suspend State" << EndDebugWarning;
								}
							}
							else
							{
								logger << DebugWarning << "Attempt to bulk erase while in Erase Suspend State" << EndDebugWarning;
							}
						}
						else
						{
							logger << DebugWarning << "Attempt to bulk erase while program or erase controller is busy (PROGRAM, ERASE, WRITE STATUS REGISTER, or WRITE NON VOLATILE CONFIGURATION command cycle is in progress)" << EndDebugWarning;
						}
					}
					else
					{
						logger << DebugWarning << "Attempt to bulk erase while a write is in progress (WRITE STATUS REGISTER, WRITE NONVOLATILE CONFIGURATION REGISTER, PROGRAM, ERASE command cycle is in progress)" << EndDebugWarning;
					}
					
					// The write enable latch bit is cleared to 0, whether the operation is successful or not.
					SR.Write_enable_latch = 0;
				}
				else
				{
					// If the write enable latch bit is not set, the device ignores the BULK ERASE command
					// and no error bits are set to indicate operation failure.
					logger << DebugWarning << "Attempt to perform " << N25Q_BULK_ERASE_COMMAND << " while " << SR.Write_enable_latch.GetName() << " = '0'" << EndDebugWarning;
				}
			}
			break;
		
		case END_OF_OPERATION:
			{
				EraseStorage(0, CONFIG::SIZE);
				// When the operation completes, the write in progress bit is cleared to 0.
				SR.Write_in_progress = 0;
				FSR.Program_or_erase_controller = 1; // ready
			}
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ProgramEraseSuspend(Event *suspend_event)
{
	switch(suspend_event->GetStage())
	{
		case START_OF_OPERATION:
			{
				if(!FSR.Program_or_erase_controller) // Program or Erase State ?
				{
					if(!program_or_erase_operation_in_progress_event)
					{
						logger << DebugError << "Internal Error! Program or Erase operation in progress is not being tracked" << EndDebugError;
					}
					
					if(suspend_in_progress_event)
					{
						logger << DebugWarning << "There's already a pending " << suspend_event->GetCommand() << EndDebugWarning;
						return;
					}
					
					if(suspend_stack.size() > CONFIG::MAX_PROGRAM_SUSPEND_OPERATION_NESTING_LEVEL)
					{
						switch(CONFIG::MAX_PROGRAM_SUSPEND_OPERATION_NESTING_LEVEL)
						{
							case 0:
								logger << DebugWarning << "It is not possible to nest PROGRAM/ERASE SUSPEND operation inside a PROGRAM/ERASE SUSPEND" << EndDebugWarning;
								break;
							case 1:
								logger << DebugWarning << "It is possible to nest PROGRAM/ERASE SUSPEND operation inside a PROGRAM/ERASE SUSPEND operation just once" << EndDebugWarning;
								break;
							default:
								logger << DebugWarning << "It is possible to nest PROGRAM/ERASE SUSPEND operation inside a PROGRAM/ERASE SUSPEND operation just " << CONFIG::MAX_PROGRAM_SUSPEND_OPERATION_NESTING_LEVEL << " times" << EndDebugWarning;
								break;
						}
						return;
					}
					
					n25q_command_type n25q_cmd = program_or_erase_operation_in_progress_event->GetCommand();
					
					switch(n25q_cmd)
					{
						case N25Q_PAGE_PROGRAM_COMMAND:
						case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
						case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
						case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
						case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
							// program operation
							{
								sc_core::sc_time time_since_last_program_resume(suspend_event->GetTimeStamp());
								time_since_last_program_resume -= program_resume_time_stamp;
								
								if(time_since_last_program_resume < program_resume_to_suspend)
								{
									logger << DebugWarning << "Program resume to program suspend timing constraint is not met" << EndDebugWarning;
								}
							}
							break;
						
						case N25Q_SECTOR_ERASE_COMMAND:
						case N25Q_BULK_ERASE_COMMAND:
							// sector or bulk erase operation
							{
								sc_core::sc_time time_since_last_erase_resume(suspend_event->GetTimeStamp());
								time_since_last_erase_resume -= erase_resume_time_stamp;
								
								if(time_since_last_erase_resume < erase_resume_to_suspend)
								{
									logger << DebugWarning << "Erase resume to erase suspend timing constraint is not met" << EndDebugWarning;
								}
							}
							break;
						case N25Q_SUBSECTOR_ERASE_COMMAND:
							// subsector erase operation
							{
								sc_core::sc_time time_since_last_subsector_erase_resume(suspend_event->GetTimeStamp());
								time_since_last_subsector_erase_resume -= subsector_erase_resume_time_stamp;
								
								if(time_since_last_subsector_erase_resume < subsector_erase_resume_to_suspend)
								{
									logger << DebugWarning << "Subsector erase resume to erase suspend timing constraint is not met" << EndDebugWarning;
								}
							}
							break;
							
						default:
							// not a program/erase operation
							logger << DebugWarning << n25q_cmd << " can't be suspended" << EndDebugWarning;
							return;
					}
					
					program_or_erase_operation_in_progress_event->UpdateElapsedTime(suspend_event->GetTimeStamp());
					
					// compute whether it worths to suspend operation (i.e. suspend latency > time to finish operation)
					bool do_suspend = true;
					
					const sc_core::sc_time& write_operation_duration = program_or_erase_operation_in_progress_event->GetDuration();
					const sc_core::sc_time& write_operation_elapsed_time = program_or_erase_operation_in_progress_event->GetElapsedTime();
					sc_core::sc_time time_to_end_of_write_operation(write_operation_duration);
					time_to_end_of_write_operation -= write_operation_elapsed_time;

					switch(n25q_cmd)
					{
						case N25Q_PAGE_PROGRAM_COMMAND:
						case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
						case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
						case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
						case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
							if(suspend_latency_program > time_to_end_of_write_operation)
							{
								do_suspend = false;
								if(verbose)
								{
									logger << DebugInfo << sc_core::sc_time_stamp() << ": Suspend of " << n25q_cmd << " cancelled because suspend latency (" << suspend_latency_program << ") is greater than time to finish " << n25q_cmd << " (" << time_to_end_of_write_operation << ")" << EndDebugInfo;
								}
							}
							break;
						case N25Q_SUBSECTOR_ERASE_COMMAND:
						case N25Q_SECTOR_ERASE_COMMAND:
						case N25Q_BULK_ERASE_COMMAND:
							if(suspend_latency_erase > time_to_end_of_write_operation)
							{
								do_suspend = false;
								if(verbose)
								{
									logger << DebugInfo << sc_core::sc_time_stamp() << ": Suspend of " << n25q_cmd << " cancelled because suspend latency (" << suspend_latency_erase << ") is greater than time to finish " << n25q_cmd << " (" << time_to_end_of_write_operation << ")" << EndDebugInfo;
								}
							}
							break;
						default:
							logger << DebugError << "Internal error! " << n25q_cmd << " can't be suspended" << EndDebugError;
							unisim::kernel::Object::Stop(-1);
							return;
					}
					
					if(do_suspend)
					{
						if(verbose)
						{
							logger << DebugInfo << sc_core::sc_time_stamp() << ": Suspending " << n25q_cmd << EndDebugInfo;
						}
						
						program_or_erase_operation_in_progress_event->Suspend();
						schedule.Cancel(program_or_erase_operation_in_progress_event);
						suspend_stack.push_back(program_or_erase_operation_in_progress_event);
						program_or_erase_operation_in_progress_event = 0;
						switch(n25q_cmd)
						{
							case N25Q_PAGE_PROGRAM_COMMAND:
							case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
							case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
							case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
							case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
								{
									// If a SUSPEND command is issued during a PROGRAM operation, then the flag status
									// register bit 2 is set to 1.
									FSR.Program_suspend = 1; // In effect
									Event *end_of_operation_event = PostEndOfWriteOperation(suspend_event, suspend_latency_program);
									suspend_in_progress_event = end_of_operation_event;
								}
								break;
							case N25Q_SUBSECTOR_ERASE_COMMAND:
							case N25Q_SECTOR_ERASE_COMMAND:
							case N25Q_BULK_ERASE_COMMAND:
								{
									// If a SUSPEND command is issued during an ERASE operation, then the flag status
									// register bit 6 is set to 1.
									FSR.Erase_suspend = 1; // In effect
									Event *end_of_operation_event = PostEndOfWriteOperation(suspend_event, suspend_latency_erase);
									suspend_in_progress_event = end_of_operation_event;
								}
								break;
							default:
								logger << DebugError << "Internal error! " << n25q_cmd << " can't be suspended" << EndDebugError;
								unisim::kernel::Object::Stop(-1);
								return;
						}
						
						SR.Write_in_progress = 0; // ready
					}
				}
				else
				{
					logger << DebugWarning << "Attempt to PROGRAM SUSPEND or ERASE SUSPEND while not in Program or Erase State" << EndDebugWarning;
				}
			}
			break;

		case END_OF_OPERATION:
			// After erase/program latency time, the flag status register bit 7 is
			// also set to 1, showing the device to be in a suspended state, waiting for any operation
			FSR.Program_or_erase_controller = 1; // ready
			suspend_in_progress_event = 0;
			break;
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ProgramEraseResume(Event *resume_event)
{
	if(suspend_stack.size())
	{
		if(suspend_in_progress_event)
		{
			if(verbose)
			{
				logger << DebugInfo << resume_event->GetTimeStamp() << ": cancelling " << suspend_in_progress_event->GetCommand() << EndDebugInfo;
			}
			schedule.Cancel(suspend_in_progress_event);
		}
		
		Event *suspended_operation_event = suspend_stack.back();
		suspend_stack.pop_back();
	
		if(suspended_operation_event)
		{
			n25q_command_type n25q_cmd = suspended_operation_event->GetCommand();
			
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": Resuming " << n25q_cmd << EndDebugInfo;
			}
			
			switch(n25q_cmd)
			{
				case N25Q_PAGE_PROGRAM_COMMAND:
				case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					program_resume_time_stamp = resume_event->GetTimeStamp();
					break;
				
				case N25Q_SECTOR_ERASE_COMMAND:
				case N25Q_BULK_ERASE_COMMAND:
					erase_resume_time_stamp = resume_event->GetTimeStamp();
					break;
				case N25Q_SUBSECTOR_ERASE_COMMAND:
					subsector_erase_resume_time_stamp = resume_event->GetTimeStamp();
					break;
				default:
					logger << DebugError << "Internal error! Unexpected " << n25q_cmd << " in suspend stack" << EndDebugError;
					unisim::kernel::Object::Stop(-1);
					return;
			}
			
			suspended_operation_event->Resume();
			
			const sc_core::sc_time& operation_duration = suspended_operation_event->GetDuration();
			const sc_core::sc_time& operation_elapsed_time = suspended_operation_event->GetElapsedTime();
			sc_core::sc_time time_to_end_of_operation(operation_duration);
			time_to_end_of_operation -= operation_elapsed_time;
			
			sc_core::sc_time end_of_operation_time_stamp(resume_event->GetTimeStamp());
			end_of_operation_time_stamp += time_to_end_of_operation;
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": " << suspended_operation_event->GetCommand() << " will finish at " << end_of_operation_time_stamp << EndDebugInfo;
			}
			suspended_operation_event->SetTimeStamp(end_of_operation_time_stamp);
			
			schedule.Notify(suspended_operation_event);
			
			switch(n25q_cmd)
			{
				case N25Q_PAGE_PROGRAM_COMMAND:
				case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					program_or_erase_operation_in_progress_event = suspended_operation_event;
					FSR.Program_suspend = 0; // Not in effect
					break;
				
				case N25Q_SECTOR_ERASE_COMMAND:
				case N25Q_BULK_ERASE_COMMAND:
				case N25Q_SUBSECTOR_ERASE_COMMAND:
					program_or_erase_operation_in_progress_event = suspended_operation_event;
					FSR.Erase_suspend = 0; // Not in effect
				default:
					break;
			}
			
			SR.Write_in_progress = 1; // busy
			FSR.Program_or_erase_controller = 0; // busy
		}
	}
	else
	{
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": There's no pending program/erase to resume" << EndDebugInfo;
		}
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	Event *event = schedule.GetNextEvent();
	
	if(event)
	{
		do
		{
			if(event->GetTimeStamp() != time_stamp)
			{
				logger << DebugError << "Internal error: unexpected event time stamp (" << event->GetTimeStamp() << " instead of " << time_stamp << ")" << EndDebugError;
				unisim::kernel::Object::Stop(-1);
			}
			
			if(!event->Suspended())
			{
				ProcessEvent(event);
				schedule.FreeEvent(event);
			}
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::ProcessEvent(Event *event)
{
	if(verbose)
	{
		logger << DebugInfo << event->GetTimeStamp() << ": processing " << ((event->GetStage() == START_OF_OPERATION) ? "start of" : "end of") << " " << event->GetCommand() << EndDebugInfo;
	}
	
	payload_type *payload = event->GetPayload();
	n25q_command_type n25q_cmd = event->GetCommand();
				
	switch(n25q_mode)
	{
		case N25Q_STD_MODE:
			// Standard mode
			
			switch(n25q_cmd)
			{
				case N25Q_UNKNOWN_COMMAND:
					break;
				
				case N25Q_READ_ID_COMMAND:
					ReadID(event);
					break;
					
				case N25Q_MULTIPLE_IO_READ_ID_COMMAND:
					MultipleIOReadID(event);
					break;
					
				case N25Q_READ_SERIAL_FLASH_DISCOVERY_PARAMETER_COMMAND:
					ReadSFDP(event);
					break;
					
				case N25Q_READ_COMMAND:
					Read(event);
					break;
					
				case N25Q_FAST_READ_COMMAND:
				case N25Q_DUAL_OUTPUT_FAST_READ_COMMAND:
				case N25Q_DUAL_INPUT_OUTPUT_FAST_READ_COMMAND:
				case N25Q_QUAD_OUTPUT_FAST_READ_COMMAND:
				case N25Q_QUAD_INPUT_OUTPUT_FAST_READ_COMMAND:
					FastRead(event);
					break;

				// REGISTER Operations
				case N25Q_READ_STATUS_REGISTER_COMMAND:
					ReadStatusRegister(event);
					break;
					
				case N25Q_READ_LOCK_REGISTER_COMMAND:
					ReadLockRegister(event);
					break;
					
				case N25Q_READ_FLAG_STATUS_REGISTER_COMMAND:
					ReadFlagStatusRegister(event);
					break;
					
				case N25Q_READ_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
					ReadNonVolatileConfigurationRegister(event);
					break;
					
				case N25Q_READ_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
					ReadVolatileConfigurationRegister(event);
					break;
					
				case N25Q_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
					ReadEnhancedVolatileConfigurationRegister(event);
					break;
					
				case N25Q_READ_OTP_ARRAY_COMMAND:
					ReadOTPArray(event);
					break;
					
				case N25Q_WRITE_ENABLE_COMMAND:
					WriteEnable();
					break;
					
				case N25Q_WRITE_DISABLE_COMMAND:
					WriteDisable();
					break;
					
				case N25Q_WRITE_STATUS_REGISTER_COMMAND:
					WriteStatusRegister(event);
					break;
					
				case N25Q_WRITE_LOCK_REGISTER_COMMAND:
					WriteLockRegister(event);
					break;
					
				case N25Q_CLEAR_FLAG_STATUS_REGISTER_COMMAND:
					ClearFlagStatusRegister();
					break;
					
				case N25Q_WRITE_NONVOLATILE_CONFIGURATION_REGISTER_COMMAND:
					WriteNonVolatileConfigurationRegister(event);
					break;
					
				case N25Q_WRITE_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
					WriteVolatileConfigurationRegister(event);
					break;
					
				case N25Q_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER_COMMAND:
					WriteEnhancedVolatileConfigurationRegister(event);
					break;
					
				case N25Q_PAGE_PROGRAM_COMMAND:
				case N25Q_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_EXTENDED_DUAL_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_QUAD_INPUT_FAST_PROGRAM_COMMAND:
				case N25Q_EXTENDED_QUAD_INPUT_FAST_PROGRAM_COMMAND:
					Program(event);
					break;
					
				case N25Q_SUBSECTOR_ERASE_COMMAND:
					SubSectorErase(event);
					break;
					
				case N25Q_SECTOR_ERASE_COMMAND:
					SectorErase(event);
					break;
					
				case N25Q_BULK_ERASE_COMMAND:
					BulkErase(event);
					break;
					
				case N25Q_PROGRAM_ERASE_RESUME_COMMAND:
					ProgramEraseResume(event);
					break;
					
				case N25Q_PROGRAM_ERASE_SUSPEND_COMMAND:
					ProgramEraseSuspend(event);
					break;
					
				case N25Q_PROGRAM_OTP_ARRAY_COMMAND:
					ProgramOTPArray(event);
					break;
			}
			break;
			
		case N25Q_XIP_MODE:
			// XIP mode
			FastRead(event);
			break;
	}

	sc_core::sc_event *completion_event = event->GetCompletionEvent();
	
	if(completion_event) // blocking ?
	{
		completion_event->notify(sc_core::SC_ZERO_TIME);
	}
	else // non blocking
	{
		if(payload->get_rx_data_bit_length())
		{
			qspi_slave_socket->spi_nb_receive(*payload);
		}
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			wait(schedule.GetKernelEvent());
			ProcessEvents();
		}
	}
	else
	{
		ProcessEvents();
		next_trigger(schedule.GetKernelEvent());
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::RESET_N_Process()
{
	if((GetProtocol() != N25Q_QUAD_IO_PROTOCOL) && ICR.Reset_hold) // reset enabled
	{
		HWReset();
	}
}

template <typename CONFIG>
void N25Q<CONFIG>::HOLD_N_Process()
{
	if((GetProtocol() != N25Q_QUAD_IO_PROTOCOL) && ICR.Reset_hold) // hold enabled
	{
		if(HOLD_RESET_N)
			schedule.Unpause();
		else
			schedule.Pause();
	}
}

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_N25Q_TCC__
