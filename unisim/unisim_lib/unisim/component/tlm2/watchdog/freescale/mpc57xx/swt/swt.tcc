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

#ifndef __UNISIM_COMPONENT_TLM2_WATCHDOG_FREESCALE_MPC57XX_SWT_SWT_TCC__
#define __UNISIM_COMPONENT_TLM2_WATCHDOG_FREESCALE_MPC57XX_SWT_SWT_TCC__

#include <unisim/component/tlm2/watchdog/freescale/mpc57xx/swt/swt.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/kernel/tlm2/trans_attr.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace watchdog {
namespace freescale {
namespace mpc57xx {
namespace swt {

template <typename CONFIG>
const unsigned int SWT<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int SWT<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int SWT<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int SWT<CONFIG>::NUM_MASTERS;

template <typename CONFIG>
const unsigned int SWT<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const uint16_t SWT<CONFIG>::UNLOCK_SEQUENCE_PRIMARY_KEY;

template <typename CONFIG>
const uint16_t SWT<CONFIG>::UNLOCK_SEQUENCE_SECONDARY_KEY;

template <typename CONFIG>
const uint16_t SWT<CONFIG>::FIXED_SERVICE_SEQUENCE_MODE_PRIMARY_KEY;

template <typename CONFIG>
const uint16_t SWT<CONFIG>::FIXED_SERVICE_SEQUENCE_MODE_SECONDARY_KEY;

template <typename CONFIG>
const uint32_t SWT<CONFIG>::MIN_DOWN_COUNTER_LOAD_VALUE;

template <typename CONFIG>
SWT<CONFIG>::SWT(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, swt_reset_b("swt_reset_b")
	, stop("stop")
	, debug("debug")
	, irq("irq")
	, reset_b("reset_b")
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, swt_cr(this)
	, swt_ir(this)
	, swt_to(this)
	, swt_wn(this)
	, swt_sr(this)
	, swt_co(this)
	, swt_sk(this)
	, reg_addr_map()
	, registers_registry()
	, schedule()
	, got_initial_timeout(false)
	, unlock_sequence_index(0)
	, service_sequence_index(0)
	, down_counter(0)
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, swt_cr_reset_value(0x0)
	, param_swt_cr_reset_value("swt-cr-reset-value", this, swt_cr_reset_value, "SWT_CR register value at reset")
	, swt_to_reset_value(0x0)
	, param_swt_to_reset_value("swt-to-reset-value", this, swt_to_reset_value, "SWT_TO register value at reset")
	, irq_level(false)
	, gen_irq_event("gen_irq_event")
	, reset_level(false)
	, gen_reset_event("gen_reset_event")
	, max_time_to_next_down_counter_run(1.0, sc_core::SC_MS)
	, last_down_counter_run_time(sc_core::SC_ZERO_TIME)
	, down_counter_run_event("down_counter_run_event")
	, freeze(false)
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
	, watchdog_clock_period(62500, sc_core::SC_PS) // 16 MHz internal clock
	, param_watchdog_clock_period("watchdog-down-counter-cycle-time", this, watchdog_clock_period, "Watchdog down counter cycle time")
{
	peripheral_slave_if(*this);
	
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Software Watchdog Timer (SWT):" << std::endl;
	description_sstr << "  - " << NUM_MASTERS << " master(s)" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 43" << std::endl;
	this->SetDescription(description_sstr.str().c_str());
	
	SC_HAS_PROCESS(SWT);
	
	// Map SWT registers regarding there address offsets
	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegister(swt_cr.ADDRESS_OFFSET, &swt_cr);
	reg_addr_map.MapRegister(swt_ir.ADDRESS_OFFSET, &swt_ir);
	reg_addr_map.MapRegister(swt_to.ADDRESS_OFFSET, &swt_to);
	reg_addr_map.MapRegister(swt_wn.ADDRESS_OFFSET, &swt_wn);
	reg_addr_map.MapRegister(swt_sr.ADDRESS_OFFSET, &swt_sr);
	reg_addr_map.MapRegister(swt_co.ADDRESS_OFFSET, &swt_co);
	reg_addr_map.MapRegister(swt_sk.ADDRESS_OFFSET, &swt_sk);

	registers_registry.AddRegisterInterface(swt_cr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(swt_ir.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(swt_to.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(swt_wn.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(swt_sr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(swt_co.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(swt_sk.CreateRegisterInterface());
	
	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	SC_METHOD(SWT_RESET_B_Process);
	sensitive << swt_reset_b.pos();
	
	SC_METHOD(RESET_B_Process);
	sensitive << gen_reset_event;
	
	SC_METHOD(RunDownCounterProcess);
	sensitive << down_counter_run_event;
}

template <typename CONFIG>
SWT<CONFIG>::~SWT()
{
}

template <typename CONFIG>
void SWT<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;
	
	// Spawn ClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options clock_properties_changed_process_spawn_options;
	
	clock_properties_changed_process_spawn_options.spawn_method();
	clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&SWT<CONFIG>::ClockPropertiesChangedProcess, this), "ClockPropertiesChangedProcess", &clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void SWT<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	unisim::kernel::tlm2::AlignToClock(notify_time_stamp, master_clock_period, master_clock_start_time, master_clock_posedge_first, master_clock_duty_cycle);
	Event *event = schedule.AllocEvent();
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	unisim::kernel::tlm2::tlm_trans_attr *trans_attr = payload.template get_extension<unisim::kernel::tlm2::tlm_trans_attr>();
	event->SetMasterID(trans_attr ? trans_attr->master_id() : 0);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
bool SWT<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int SWT<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
	
	CustomReadWriteArg custom_rw_arg;
	custom_rw_arg.master_id = 0;
	custom_rw_arg.time_stamp = time_stamp;
	
	switch(cmd)
	{
		case tlm::TLM_WRITE_COMMAND:
			return reg_addr_map.DebugWrite(custom_rw_arg, start_addr, data_ptr, data_length);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map.DebugRead(custom_rw_arg, start_addr, data_ptr, data_length);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum SWT<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
				unisim::kernel::tlm2::tlm_trans_attr *trans_attr = payload.template get_extension<unisim::kernel::tlm2::tlm_trans_attr>();
				event->SetMasterID(trans_attr ? trans_attr->master_id() : 0);
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
unisim::service::interfaces::Register *SWT<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void SWT<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void SWT<CONFIG>::ProcessEvent(Event *event)
{
	tlm::tlm_generic_payload *payload = event->GetPayload();
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
		
			CustomReadWriteArg custom_rw_arg;
			custom_rw_arg.master_id = event->GetMasterID();
			custom_rw_arg.time_stamp = event->GetTimeStamp();
			
			switch(cmd)
			{
				case tlm::TLM_WRITE_COMMAND:
					rws = reg_addr_map.Write(custom_rw_arg, start_addr, data_ptr, data_length);
					break;
				case tlm::TLM_READ_COMMAND:
					rws = reg_addr_map.Read(custom_rw_arg, start_addr, data_ptr, data_length);
					break;
				default:
					break;
			}
			
			if(IsReadWriteError(rws))
			{
				logger << DebugError << "while mapped read/write access, " << std::hex << rws << std::dec << EndDebugError;
				if(swt_cr.template Get<typename SWT_CR::WEN>() && swt_cr.template Get<typename SWT_CR::RIA>())
				{
					// Invalid access to the SWT causes a system reset if WEN=1
					TriggerReset();
					
					if(cmd == tlm::TLM_READ_COMMAND)
					{
						memset(data_ptr, 0, data_length);
					}
					payload->set_response_status(tlm::TLM_OK_RESPONSE);
				}
				else
				{
					//  Invalid access to the SWT generates a bus error
					payload->set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
				}
				
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
}

template <typename CONFIG>
void SWT<CONFIG>::ProcessEvents()
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
			
			ProcessEvent(event);
			schedule.FreeEvent(event);
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

template <typename CONFIG>
void SWT<CONFIG>::Process()
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
void SWT<CONFIG>::Reset()
{
	CheckCompatibility();
	UpdateMasterClock();

	last_down_counter_run_time = sc_core::sc_time_stamp();
	unisim::kernel::tlm2::AlignToClock(last_down_counter_run_time, watchdog_clock_period);

	schedule.Clear();
	
	swt_cr.Initialize(swt_cr_reset_value);
	swt_ir.Initialize(0x0);
	swt_to.Initialize(swt_to_reset_value);
	swt_wn.Initialize(0x0);
	swt_sr.Initialize(0x0);
	swt_co.Initialize(0x0);
	swt_sk.Initialize(0x0);
	unlock_sequence_index = 0;
	service_sequence_index = 0;
	got_initial_timeout = false;
	SetDownCounter(swt_to);
	
	RefreshFreeze();

	ScheduleDownCounterRun();
}

template <typename CONFIG>
void SWT<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void SWT<CONFIG>::ClockPropertiesChangedProcess()
{
	// Clock properties have changed
	UpdateMasterClock();
}

template <typename CONFIG>
void SWT<CONFIG>::SWT_RESET_B_Process()
{
	if(swt_reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void SWT<CONFIG>::IRQ_Process()
{
	if(verbose)
	{
		logger << DebugInfo << irq.name() << " <- " << irq_level << EndDebugInfo;
	}
	irq = irq_level;
}

template <typename CONFIG>
void SWT<CONFIG>::RESET_B_Process()
{
	if(verbose)
	{
		logger << DebugInfo << reset_b.name() << " <- " << !reset_level << EndDebugInfo;
	}
	if(!reset_b && !reset_level)
	{
		Reset();
	}
	
	reset_b = !reset_level;
	
	if(reset_level)
	{
		reset_level = false;
		gen_reset_event.notify(sc_core::sc_time(10.0, sc_core::SC_NS));
	}
}

template <typename CONFIG>
bool SWT<CONFIG>::CheckMasterAccess(unsigned master_id) const
{
	switch(master_id)
	{
		case 0: return swt_cr.template Get<typename SWT_CR::MAP0>() != 0;
		case 1: return swt_cr.template Get<typename SWT_CR::MAP1>() != 0;
		case 2: return swt_cr.template Get<typename SWT_CR::MAP2>() != 0;
		case 3: return swt_cr.template Get<typename SWT_CR::MAP3>() != 0;
		case 4: return swt_cr.template Get<typename SWT_CR::MAP4>() != 0;
		case 5: return swt_cr.template Get<typename SWT_CR::MAP5>() != 0;
		case 6: return swt_cr.template Get<typename SWT_CR::MAP6>() != 0;
		case 7: return swt_cr.template Get<typename SWT_CR::MAP7>() != 0;
	}
	return false;
}

template <typename CONFIG>
void SWT<CONFIG>::SetIRQLevel(bool level)
{
	swt_ir.template Set<typename SWT_IR::TIF>(level);
	
	// Schedule IRQ output assertion
	irq_level = level;
	gen_irq_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void SWT<CONFIG>::TriggerReset()
{
	unlock_sequence_index = 0;
	service_sequence_index = 0;
	// Schedule reset_b output assertion
	reset_level = true;
	gen_reset_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void SWT<CONFIG>::DecrementDownCounter(sc_dt::uint64 delta)
{
	if(delta)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": running down counter for " << delta << " watchdog clock cycles" << EndDebugInfo;
		}

		// Check model consistency
		if(swt_cr.template Get<typename SWT_CR::WEN>())
		{
			// watchdog enabled
			if(down_counter < delta)
			{
				logger << DebugWarning << "Model has a problem because too much time has elapsed since last down counter run, resulting in a down counter underflow" << EndDebugWarning;
			}
		}
		
		SetDownCounter((down_counter >= delta) ? (down_counter - delta) : 0); // decrement down counter

		// Check time out
		if(swt_cr.template Get<typename SWT_CR::WEN>())
		{
			bool timeout = false;
			
			if(down_counter == 0)
			{
				timeout = true;
			}
			
			if(timeout)
			{
				if(swt_cr.template Get<typename SWT_CR::ITR>())
				{
					if(got_initial_timeout)
					{
						TriggerReset();
					}
					else
					{
						SetIRQLevel(true);
						got_initial_timeout = true;
					}
				}
				else
				{
					TriggerReset();
				}
			}
		}
	}
}

template <typename CONFIG>
sc_dt::int64 SWT<CONFIG>::TicksToNextDownCounterRun()
{
	if(swt_cr.template Get<typename SWT_CR::WEN>())
	{
		// watchdog enabled
		return down_counter;
	}
	
	return 0;
}

template <typename CONFIG>
sc_core::sc_time SWT<CONFIG>::TimeToNextDownCounterRun()
{
	sc_dt::int64 ticks_to_next_down_counter_run = TicksToNextDownCounterRun();
	
	if(ticks_to_next_down_counter_run)
	{
		sc_core::sc_time time_to_next_down_counter_run(watchdog_clock_period);
		time_to_next_down_counter_run *= ticks_to_next_down_counter_run;
		
		if(time_to_next_down_counter_run < max_time_to_next_down_counter_run) return time_to_next_down_counter_run;
	}
	
	return max_time_to_next_down_counter_run;
}

template <typename CONFIG>
void SWT<CONFIG>::RunDownCounterToTime(const sc_core::sc_time& time_stamp)
{
	if(!freeze)
	{
		// in the past, until time stamp, down counter was not frozen: decrement down counter
		// Compute the elapsed time since last count run
		sc_core::sc_time delay_since_last_down_counter_run(time_stamp);
		delay_since_last_down_counter_run -= last_down_counter_run_time;
		
		// Compute number of down counter ticks since last count run
		sc_dt::uint64 delta = delay_since_last_down_counter_run / watchdog_clock_period;
		
		if(delta)
		{
			sc_core::sc_time run_time(watchdog_clock_period);
			run_time *= delta;

			DecrementDownCounter(delta);
			last_down_counter_run_time += run_time;
		}
	}
}

template <typename CONFIG>
void SWT<CONFIG>::ScheduleDownCounterRun()
{
	sc_core::sc_time time_to_next_down_counter_run = TimeToNextDownCounterRun();
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": time to next down counter run is " << time_to_next_down_counter_run << EndDebugInfo;
	}
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	time_to_next_down_counter_run += last_down_counter_run_time;
	time_to_next_down_counter_run -= time_stamp;
	
	down_counter_run_event.notify(time_to_next_down_counter_run); // schedule next down counter run
}

template <typename CONFIG>
void SWT<CONFIG>::RunDownCounterProcess()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	if(verbose)
	{
		logger << DebugInfo << time_stamp << ":RunDownCounterProcess()" << EndDebugInfo;
	}
	
	RunDownCounterToTime(time_stamp);
	RefreshFreeze();
	ScheduleDownCounterRun();
}

template <typename CONFIG>
void SWT<CONFIG>::RefreshFreeze()
{
	// Latch value for internal "freeze"
	bool old_freeze = freeze;
	freeze = (debug && swt_cr.template Get<typename SWT_CR::FRZ>()) || (stop && swt_cr.template Get<typename SWT_CR::STP>());
	
	if(old_freeze && !freeze)
	{
		last_down_counter_run_time = sc_core::sc_time_stamp();
		unisim::kernel::tlm2::AlignToClock(last_down_counter_run_time, watchdog_clock_period);
	}
}

template <typename CONFIG>
bool SWT<CONFIG>::IsLocked() const
{
	return swt_cr.template Get<typename SWT_CR::HLK>() || swt_cr.template Get<typename SWT_CR::SLK>();
}

template <typename CONFIG>
void SWT<CONFIG>::UnlockSequence()
{
	switch(unlock_sequence_index)
	{
		case 0:
			if(swt_sr.template Get<typename SWT_SR::WSC>() == UNLOCK_SEQUENCE_PRIMARY_KEY)
			{
				unlock_sequence_index++;
			}
			break;
		case 1:
			if(swt_sr.template Get<typename SWT_SR::WSC>() == UNLOCK_SEQUENCE_SECONDARY_KEY)
			{
				// end of sequence
				unlock_sequence_index = 0;
				
				if(verbose)
				{
					logger << DebugInfo << "End of unlock sequence" << EndDebugInfo;
				}

				// Unlock
				if(verbose)
				{
					logger << DebugInfo << "Unlocking" << EndDebugInfo;
				}
				swt_cr.template Set<typename SWT_CR::SLK>(0);
			}
			break;
	}
}

template <typename CONFIG>
bool SWT<CONFIG>::CheckWindow() const
{
	if(swt_cr.template Get<typename SWT_CR::WND>())
	{
		// window mode is enabled
		if(down_counter < swt_wn.template Get<typename SWT_WN::WST>())
		{
			// When window mode is enabled, the service sequence can only be written when the internal counter is less than SWT_WN[WST]
			return true;
		}
	}
	else
	{
		return true;
	}
	
	return false;
}

template <typename CONFIG>
bool SWT<CONFIG>::ServiceSequence()
{
	switch(swt_cr.template Get<typename SWT_CR::SMD>())
	{
		case SMD_KEYED_SERVICE_SEQUENCE:
			// Keyed service sequence mode is selected
			swt_sk.template Set<typename SWT_SK::SK>((17 * swt_sk.template Get<typename SWT_SK::SK>()) + 3);
			
			if(swt_sr.template Get<typename SWT_SR::WSC>() == swt_sk.template Get<typename SWT_SK::SK>())
			{
				// key match
				
				if(++service_sequence_index == 2)
				{
					// end of service sequence
					service_sequence_index = 0;
					
					if(!CheckWindow()) return false;
					
					if(verbose)
					{
						logger << DebugInfo << "End of keyed service sequence" << EndDebugInfo;
					}
					
					// clear SWT_CR[SLK]
					swt_cr.template Set<typename SWT_CR::SLK>(0);
					
					// Rearm down counter
					RearmDownCounter();
				}
			}
			break;
			
		case SMD_FIXED_SERVICE_SEQUENCE:
			// Fixed service sequence mode is selected
			switch(service_sequence_index)
			{
				case 0:
					if(swt_sr.template Get<typename SWT_SR::WSC>() == FIXED_SERVICE_SEQUENCE_MODE_PRIMARY_KEY)
					{
						// Getting primary key
						
						// start of service sequence
						
						if(!CheckWindow()) return false;

						service_sequence_index++;
					}
					break;
				
				case 1:
					if(swt_sr.template Get<typename SWT_SR::WSC>() == FIXED_SERVICE_SEQUENCE_MODE_SECONDARY_KEY)
					{
						// Getting secondary key
						
						// end of service sequence
						
						if(!CheckWindow()) return false;

						service_sequence_index = 0;

						if(verbose)
						{
							logger << DebugInfo << "End of fixed service sequence" << EndDebugInfo;
						}
						
						// clear SWT_CR[SLK]
						swt_cr.template Set<typename SWT_CR::SLK>(0);

						// Rearm down counter
						RearmDownCounter();
					}
			}
			break;
	}
	
	return true;
}

template <typename CONFIG>
void SWT<CONFIG>::SetDownCounter(uint32_t value)
{
	if(verbose)
	{
		logger << DebugInfo << "down counter <- " << value << EndDebugInfo;
	}
	
	down_counter = value;
	
	// When the watchdog is disabled (SWT_CR[WEN]=0), SWT_CO[CNT] shows the value of the internal down counter
	// When the watchdog is enabled (SWT_CR[WEN]=1), SW_CO[CNT] is cleared
	swt_co.template Set<typename SWT_CO::CNT>(swt_cr.template Get<typename SWT_CR::WEN>() ? down_counter : 0);
}

template <typename CONFIG>
void SWT<CONFIG>::RearmDownCounter()
{
	uint32_t value = swt_to.template Get<typename SWT_TO::WTO>();
	if(value < MIN_DOWN_COUNTER_LOAD_VALUE)
	{
		logger << DebugWarning << "Attempt to load down counter with a value (" << value << ") below the minimum value that is " << MIN_DOWN_COUNTER_LOAD_VALUE << EndDebugWarning;
		value = MIN_DOWN_COUNTER_LOAD_VALUE;
	}

	SetDownCounter(value);
	
	if(verbose)
	{
		logger << DebugInfo << "Rearming down counter (" << swt_to.template Get<typename SWT_TO::WTO>() << ")" << EndDebugInfo;
	}
	
	ScheduleDownCounterRun();
}

template <typename CONFIG>
void SWT<CONFIG>::CheckCompatibility()
{
	switch(swt_cr.template Get<typename SWT_CR::SMD>())
	{
		case SMD_FIXED_ADDRESS_EXECUTION:
		case SMD_INCREMENTAL_ADDRESS_EXECUTION:
			logger << DebugError << "Fixed address execution mode and increment address execution mode are unsupported for now" << EndDebugError;
			unisim::kernel::Object::Stop(-1);
			break;
		default:
			break;
	}

	if(!m_clk_prop_proxy.IsClockCompatible())
	{
		logger << DebugError << "clock port is not bound to a unisim::kernel::tlm2::Clock" << EndDebugError;
		unisim::kernel::Object::Stop(-1);
		return;
	}
}

} // end of namespace swt
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace watchdog
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_SWT_SWT_TCC__
