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

#ifndef __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_STM_STM_TCC__
#define __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_STM_STM_TCC__

#include <unisim/component/tlm2/timer/freescale/mpc57xx/stm/stm.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace freescale {
namespace mpc57xx {
namespace stm {

template <typename CONFIG>
const unsigned int STM<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int STM<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int STM<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int STM<CONFIG>::NUM_CHANNELS;

template <typename CONFIG>
const unsigned int STM<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const bool STM<CONFIG>::threaded_model;

template <typename CONFIG>
STM<CONFIG>::STM(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, reset_b("reset_b")
	, debug("debug")
	, irq()
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, stm_cr(this)
	, stm_cnt(this)
	, stm_ccr(this)
	, stm_cir(this)
	, stm_cmp(this)
	, reg_addr_map()
	, registers_registry()
	, schedule()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, irq_level()
	, gen_irq_event()
	, max_time_to_next_counter_run(1.0, sc_core::SC_MS)
	, last_counter_run_time(sc_core::SC_ZERO_TIME)
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
	, prescaled_clock_period(sc_core::SC_ZERO_TIME)
	, freeze(false)
{
	std::stringstream description_sstr;
	description_sstr << "MPC57XX System Timer Module (STM):" << std::endl;
	description_sstr << "  - " << NUM_CHANNELS << " channel(s)" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 42" << std::endl;
	this->SetDescription(description_sstr.str().c_str());
	
	peripheral_slave_if(*this); // bind interface
	
	unsigned int channel_num;
	
	SC_HAS_PROCESS(STM);

	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		std::stringstream irq_name_sstr;
		irq_name_sstr << "irq_" << channel_num;
		irq[channel_num] = new sc_core::sc_out<bool>(irq_name_sstr.str().c_str()); 
	}

	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		std::stringstream gen_irq_event_name_sstr;
		gen_irq_event_name_sstr << "gen_irq_event_" << channel_num;
		gen_irq_event[channel_num] = new sc_core::sc_event(gen_irq_event_name_sstr.str().c_str());
	}

	// Map STM registers regarding there address offsets
	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegister(stm_cr.ADDRESS_OFFSET, &stm_cr);
	reg_addr_map.MapRegister(stm_cnt.ADDRESS_OFFSET, &stm_cnt);
	reg_addr_map.MapRegisterFile(STM_CCR::ADDRESS_OFFSET, &stm_ccr, /* size */ 4, /* stride */ 16);
	reg_addr_map.MapRegisterFile(STM_CIR::ADDRESS_OFFSET, &stm_cir, /* size */ 4, /* stride */ 16);
	reg_addr_map.MapRegisterFile(STM_CMP::ADDRESS_OFFSET, &stm_cmp, /* size */ 4, /* stride */ 16);

	registers_registry.AddRegisterInterface(stm_cr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(stm_cnt.CreateRegisterInterface());
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		registers_registry.AddRegisterInterface(stm_ccr[channel_num].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(stm_cir[channel_num].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(stm_cmp[channel_num].CreateRegisterInterface());
	}
	
	SC_METHOD(RESET_B_Process);
	sensitive << reset_b.pos();

	// Spawn an IRQ_Process for each channel
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		sc_core::sc_spawn_options irq_process_spawn_options;
		irq_process_spawn_options.spawn_method();
		irq_process_spawn_options.dont_initialize();
		irq_process_spawn_options.set_sensitivity(gen_irq_event[channel_num]);
		
		std::stringstream irq_process_name_sstr;
		irq_process_name_sstr << "IRQ_Process_" << channel_num;
		sc_core::sc_spawn(sc_bind(&STM<CONFIG>::IRQ_Process, this, channel_num), irq_process_name_sstr.str().c_str(), &irq_process_spawn_options);
	}
}

template <typename CONFIG>
STM<CONFIG>::~STM()
{
	unsigned int channel_num;
	
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		delete irq[channel_num];
	}
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		delete gen_irq_event[channel_num];
	}
}

template <typename CONFIG>
void STM<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;

	// Spawn Process
	sc_core::sc_spawn_options process_spawn_options;
	process_spawn_options.spawn_method();
	process_spawn_options.set_sensitivity(&schedule.GetKernelEvent());
	process_spawn_options.set_sensitivity(&debug.value_changed_event());
	process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());
	
	sc_core::sc_spawn(sc_bind(&STM<CONFIG>::Process, this), "Process", &process_spawn_options);

	Reset();
}

template <typename CONFIG>
void STM<CONFIG>::Reset()
{
	unsigned int channel_num;

	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		SetIRQLevel(channel_num, false);
	}
	
	stm_cr.Initialize(0x0);
	stm_cnt.Initialize(0x0);
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		stm_ccr[channel_num].Initialize(0x0);
		stm_cir[channel_num].Initialize(0x0);
		stm_cmp[channel_num].Initialize(0x0);
	}

	sc_core::sc_time start_time;
	
	if(m_clk_prop_proxy.GetClockPosEdgeFirst())
	{
		// clock rising edge comes first
		start_time = m_clk_prop_proxy.GetClockStartTime();
	}
	else
	{
		// clock falling edge comes first: skip time until first rising edge
		start_time = m_clk_prop_proxy.GetClockStartTime();
		start_time += (1.0 - m_clk_prop_proxy.GetClockDutyCycle()) * m_clk_prop_proxy.GetClockPeriod();
	}
	
	if(verbose)
	{
		logger << DebugInfo << "First MAIN_CLK rising edge is at " << start_time << EndDebugInfo;
	}

	if(start_time > last_counter_run_time)
	{
		last_counter_run_time = start_time;
	}

	UpdatePrescaledClockPeriod();
	RefreshFreeze();
	ScheduleCounterRun();
}

template <typename CONFIG>
void STM<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	unisim::kernel::tlm2::AlignToClock(notify_time_stamp, master_clock_period, master_clock_start_time, master_clock_posedge_first, master_clock_duty_cycle);
	Event *event = schedule.AllocEvent();
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
bool STM<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int STM<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
{
	tlm::tlm_command cmd = payload.get_command();
	unsigned int data_length = payload.get_data_length();
	unsigned char *data_ptr = payload.get_data_ptr();
	sc_dt::uint64 start_addr = payload.get_address();
	sc_core::sc_time time_stamp(sc_core::sc_time_stamp());
	
	if(!data_ptr)
	{
		logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
		unisim::kernel::Object::Stop(-1);
		return 0;
	}
	else if(!data_length)
	{
		return 0;
	}
	
	switch(cmd)
	{
		case tlm::TLM_WRITE_COMMAND:
			return reg_addr_map.DebugWrite(time_stamp, start_addr, data_ptr, data_length);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map.DebugRead(time_stamp, start_addr, data_ptr, data_length);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum STM<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				unisim::kernel::tlm2::AlignToClock(notify_time_stamp, master_clock_period, master_clock_start_time, master_clock_posedge_first, master_clock_duty_cycle);
				Event *event = schedule.AllocEvent();
				event->SetPayload(&payload);
				event->SetTimeStamp(notify_time_stamp);
				schedule.Notify(event);
				phase = tlm::END_REQ;
				return tlm::TLM_UPDATED;
			}
			break;
		case tlm::END_RESP:
			return tlm::TLM_COMPLETED;
		default:
			logger << DebugError << "protocol error" << EndDebugError;
			unisim::kernel::Object::Stop(-1);
			break;
	}
	
	return tlm::TLM_COMPLETED;
}

//////////////// unisim::service::interface::Registers ////////////////////

template <typename CONFIG>
unisim::service::interfaces::Register *STM<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void STM<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void STM<CONFIG>::ProcessEvent(Event *event)
{
	switch(event->GetType())
	{
		case EV_NONE:
			break;
		
		case EV_CPU_PAYLOAD:
		{
			tlm::tlm_generic_payload *payload = event->GetPayload();
			sc_core::sc_time time_stamp(event->GetTimeStamp());
			tlm::tlm_command cmd = payload->get_command();

			if(cmd != tlm::TLM_IGNORE_COMMAND)
			{
				unsigned int streaming_width = payload->get_streaming_width();
				unsigned int data_length = payload->get_data_length();
				unsigned char *data_ptr = payload->get_data_ptr();
				unsigned char *byte_enable_ptr = payload->get_byte_enable_ptr();
				sc_dt::uint64 start_addr = payload->get_address();
		//		sc_dt::uint64 end_addr = start_addr + ((streaming_width > data_length) ? data_length : streaming_width) - 1;
				
				if(!data_ptr)
				{
					logger << DebugError << "data pointer for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
					unisim::kernel::Object::Stop(-1);
					return;
				}
				else if(!data_length)
				{
					logger << DebugError << "data length range for TLM-2.0 GP READ/WRITE command is invalid" << EndDebugError;
					unisim::kernel::Object::Stop(-1);
					return;
				}
				else if(byte_enable_ptr)
				{
					// byte enable is unsupported
					logger << DebugWarning << "byte enable for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
					payload->set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
				}
				else if(streaming_width < data_length)
				{
					// streaming is unsupported
					logger << DebugWarning << "streaming for TLM-2.0 GP READ/WRITE command is unsupported" << EndDebugWarning;
					payload->set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
				}
				else if((start_addr & 3) != 0)
				{
					logger << DebugWarning << "only 32-bit aligned access to register are allowed" << EndDebugWarning;
					payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
				}
				else if(streaming_width != 4)
				{
					logger << DebugWarning << "only 32-bit access to register are allowed" << EndDebugWarning;
					payload->set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
				}
				else
				{
					ReadWriteStatus rws = RWS_OK;
					
					switch(cmd)
					{
						case tlm::TLM_WRITE_COMMAND:
							rws = reg_addr_map.Write(time_stamp, start_addr, data_ptr, data_length);
							break;
						case tlm::TLM_READ_COMMAND:
							rws = reg_addr_map.Read(time_stamp, start_addr, data_ptr, data_length);
							break;
						default:
							break;
					}
					
					if(IsReadWriteError(rws))
					{
						logger << DebugError << "while mapped read/write access, " << std::hex << rws << std::dec << EndDebugError;
						payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
					}
					else
					{
						payload->set_response_status(tlm::TLM_OK_RESPONSE);
					}
				}
			}

			payload->set_dmi_allowed(false);
			
			sc_core::sc_time completion_time(sc_core::SC_ZERO_TIME);
			sc_core::sc_event *completion_event = event->GetCompletionEvent();
			
			if(completion_event)
			{
				completion_event->notify(completion_time);
			}
			else
			{
				tlm::tlm_phase phase = tlm::BEGIN_RESP;
				
				tlm::tlm_sync_enum sync = peripheral_slave_if->nb_transport_bw(*payload, phase, completion_time);
				
				switch(sync)
				{
					case tlm::TLM_ACCEPTED:
						break;
					case tlm::TLM_UPDATED:
						break;
					case tlm::TLM_COMPLETED:
						break;
				}
			}
			
			break;
		}
		
		case EV_WAKE_UP:
			break;
	}
}

template <typename CONFIG>
void STM<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	RunCounterToTime(time_stamp);
	
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
			
			ProcessEvent(event);
			schedule.FreeEvent(event);
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
	
	ScheduleCounterRun();
	RefreshFreeze();
}

template <typename CONFIG>
void STM<CONFIG>::Process()
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
void STM<CONFIG>::UpdatePrescaledClockPeriod()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
	prescaled_clock_period = master_clock_period;
	prescaled_clock_period *= stm_cr.template Get<typename STM_CR::CPS>() + 1;
}

template <typename CONFIG>
void STM<CONFIG>::SetIRQLevel(unsigned int channel_num, bool level)
{
	// Schedule IRQ output assertion
	irq_level[channel_num] = level;
	gen_irq_event[channel_num]->notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void STM<CONFIG>::CompareCounter()
{
	// Get counter (STM_CNT[CNT])
	uint32_t cnt = stm_cnt.template Get<typename STM_CNT::CNT>();
	
	unsigned int channel_num;
	
	// For each channel
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		// Check if that channel is enabled
		if(stm_ccr[channel_num].template Get<typename STM_CCR::CEN>())
		{
			// Channel is enabled
			uint32_t cmp = stm_cmp[channel_num].template Get<typename STM_CMP::CMP>();
			
			if(cnt == cmp)
			{
				// Comparison matches: trigger an interrupt
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ": channel #" << channel_num << " compare value matches counter (" << cnt << ")" << EndDebugInfo;
				}
				
				// Set STM_CIRn[CIF]
				stm_cir[channel_num].template Set<typename STM_CIR::CIF>(1);
				
				// Set IRQ output level 
				SetIRQLevel(channel_num, true);
			}
		}
	}
}

template <typename CONFIG>
void STM<CONFIG>::IncrementCounter(sc_dt::uint64 delta)
{
	// Check that counter is enabled
	if(stm_cr.template Get<typename STM_CR::TEN>())
	{
		// Counter is enabled
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": running counter for " << delta << " Master clock cycles" << EndDebugInfo;
		}
		
		uint32_t cnt = stm_cnt.template Get<typename STM_CNT::CNT>();
		
		// Increment counter
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": STM_CNT increases from " << cnt;
		}
		cnt += delta;
		if(unlikely(verbose))
		{
			logger << DebugInfo << " to " << cnt << EndDebugInfo;
		}
		stm_cnt.template Set<typename STM_CNT::CNT>(cnt);
	}
}

template <typename CONFIG>
sc_dt::uint64 STM<CONFIG>::TicksToNextCounterRun()
{
	static const sc_dt::uint64 roll_over_ticks = sc_dt::uint64(1) << STM_CNT::CNT::GetBitWidth();
	
	// Compute the minimum number of counter ticks to the next potential interrupt
	sc_dt::int64 ticks_to_next_counter_run = 0;
	
	if(stm_cr.template Get<typename STM_CR::TEN>())
	{
		// Counter is enabled
		uint32_t cnt = stm_cnt.template Get<typename STM_CNT::CNT>();

		unsigned channel_num;
		
		for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
		{
			if(stm_ccr[channel_num].template Get<typename STM_CCR::CEN>())
			{
				// Channel is enabled
				uint32_t cmp = stm_cmp[channel_num].template Get<typename STM_CMP::CMP>();
				
				sc_dt::int64 ticks_to_interrupt = (cnt < cmp) ? sc_dt::uint64(cmp - cnt)
				                                              : roll_over_ticks - cnt + cmp;   // roll over
				
				if(ticks_to_interrupt && (!ticks_to_next_counter_run || (ticks_to_interrupt < ticks_to_next_counter_run))) ticks_to_next_counter_run = ticks_to_interrupt;
			}
		}
	}
	
	return ticks_to_next_counter_run;
}

template <typename CONFIG>
sc_core::sc_time STM<CONFIG>::TimeToNextCounterRun()
{
	sc_dt::uint64 ticks_to_next_counter_run = TicksToNextCounterRun();
	
	if(ticks_to_next_counter_run)
	{
		sc_core::sc_time time_to_next_counter_run(prescaled_clock_period);
		time_to_next_counter_run *= ticks_to_next_counter_run;
		return time_to_next_counter_run;
	}
	
	return max_time_to_next_counter_run;
}

template <typename CONFIG>
void STM<CONFIG>::RunCounterToTime(const sc_core::sc_time& time_stamp)
{
	if(!freeze)
	{
		// in the past, until time stamp, counter was not frozen: incrementer counter
		// Compute the elapsed time since last count run
		sc_core::sc_time delay_since_last_counter_run(time_stamp);
		delay_since_last_counter_run -= last_counter_run_time;
		
		// Compute number of counter ticks since last count run
		sc_dt::uint64 delta = delay_since_last_counter_run / prescaled_clock_period;
		
		if(delta)
		{
			sc_core::sc_time run_time(prescaled_clock_period);
			run_time *= delta;

			IncrementCounter(delta);
			last_counter_run_time += run_time;
			
			// Compare counter
			CompareCounter();
		}
	}
}

template <typename CONFIG>
void STM<CONFIG>::ScheduleCounterRun()
{
	// Clock properties may have changed that may affect time to next counter run
	UpdatePrescaledClockPeriod();
	
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	if(last_counter_run_time > time_stamp)
	{
		// Counter not yet started (because of clock)
		Event *event = schedule.AllocEvent();
		event->WakeUp();
		event->SetTimeStamp(last_counter_run_time);
		schedule.Notify(event);
		return;
	}

	sc_core::sc_time time_to_next_counter_run = TimeToNextCounterRun();
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": time to next counter run is " << time_to_next_counter_run << EndDebugInfo;
	}

	sc_core::sc_time next_counter_run_time_stamp(sc_core::sc_time_stamp());
	next_counter_run_time_stamp += time_to_next_counter_run;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": counter will run at " << next_counter_run_time_stamp << EndDebugInfo;
	}

	Event *event = schedule.AllocEvent();
	event->WakeUp();
	event->SetTimeStamp(next_counter_run_time_stamp);
	schedule.Notify(event);
}

template <typename CONFIG>
void STM<CONFIG>::IRQ_Process(unsigned int channel_num)
{
	// Set IRQ output
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << irq[channel_num]->name() << " <- " << irq_level[channel_num] << EndDebugInfo;
	}
	*irq[channel_num] = irq_level[channel_num];
}

template <typename CONFIG>
void STM<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void STM<CONFIG>::RefreshFreeze()
{
	// Latch value for internal "freeze"
	bool old_freeze = freeze;
	freeze = debug && stm_cr.template Get<typename STM_CR::FRZ>();
	
	if(old_freeze && !freeze)
	{
		last_counter_run_time = sc_core::sc_time_stamp();
		unisim::kernel::tlm2::AlignToClock(last_counter_run_time, master_clock_period, master_clock_start_time, master_clock_posedge_first, master_clock_duty_cycle);
	}
}

} // end of namespace stm
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_STM_STM_TCC__
