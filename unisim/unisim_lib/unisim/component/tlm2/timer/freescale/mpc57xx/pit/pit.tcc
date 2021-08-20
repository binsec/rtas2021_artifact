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

#ifndef __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_PIT_PIT_TCC__
#define __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_PIT_PIT_TCC__

#include <unisim/component/tlm2/timer/freescale/mpc57xx/pit/pit.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace timer {
namespace freescale {
namespace mpc57xx {
namespace pit {

template <typename CONFIG>
const unsigned int PIT<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int PIT<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int PIT<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int PIT<CONFIG>::MAX_CHANNELS;

template <typename CONFIG>
const unsigned int PIT<CONFIG>::NUM_CHANNELS;

template <typename CONFIG>
const bool PIT<CONFIG>::HAS_RTI_SUPPORT;

template <typename CONFIG>
const bool PIT<CONFIG>::HAS_DMA_SUPPORT;

template <typename CONFIG>
const bool PIT<CONFIG>::HAS_64_BIT_TIMER_SUPPORT;

template <typename CONFIG>
const unsigned int PIT<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const bool PIT<CONFIG>::threaded_model;

template <typename CONFIG>
PIT<CONFIG>::PIT(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, per_clk("per_clk")
	, rti_clk("rti_clk")
	, reset_b("reset_b")
	, debug("debug")
	, irq()
	, dma_trigger()
	, rtirq("rtirq")
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, per_clk_prop_proxy(0)
	, rti_clk_prop_proxy(0)
	, pit_mcr(this)
	, pit_ltmr64h(this)
	, pit_ltmr64l(this)
	, pit_rti_ldval(this)
	, pit_rti_cval(this)
	, pit_rti_tctrl(this)
	, pit_rti_tflg(this)
	, pit_ldval(this)
	, pit_cval(this)
	, pit_tctrl(this)
	, pit_tflg(this)
	, reg_addr_map()
	, registers_registry()
	, schedule()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, pit_mcr_reset_value(0x0)
	, param_pit_mcr_reset_value("pit-mcr-reset-value", this, pit_mcr_reset_value, "value of PIT_MCR at reset")
	, irq_level()
	, gen_irq_event()
	, rtirq_level(false)
	, gen_rtirq_event("gen_rtirq_event")
	, dma_trigger_level()
	, gen_dma_pulse_event()
	, max_time_to_next_timers_run(1.0, sc_core::SC_MS)
	, max_time_to_next_rti_timer_run(1.0, sc_core::SC_MS)
	, last_timers_run_time(sc_core::SC_ZERO_TIME)
	, last_rti_timer_run_time(sc_core::SC_ZERO_TIME)
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
	, rti_clock_period(10.0, sc_core::SC_NS)
	, rti_clock_start_time(sc_core::SC_ZERO_TIME)
	, rti_clock_posedge_first(true)
	, rti_clock_duty_cycle(0.5)
	, per_clock_period(10.0, sc_core::SC_NS)
	, per_clock_start_time(sc_core::SC_ZERO_TIME)
	, per_clock_posedge_first(true)
	, per_clock_duty_cycle(0.5)
	, timers_run_event("timers_run_event")
	, freeze_timers(false)
	, freeze_rti_timer(false)
	, load_timer()
	, load_rti_timer(false)
{
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Periodic Interrupt Timer (PIT):" << std::endl;
	if(NUM_CHANNELS > 0)
	{
		description_sstr << "  - " << NUM_CHANNELS << " channel(s)" << std::endl;
	}
	if(HAS_RTI_SUPPORT)
	{
		description_sstr << "  - RTI support" << std::endl;
	}
	if(HAS_DMA_SUPPORT)
	{
		description_sstr << "  - DMA support" << std::endl;
	}
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 44" << std::endl;
	this->SetDescription(description_sstr.str().c_str());

	if(NUM_CHANNELS > 0)
	{
		per_clk_prop_proxy = new unisim::kernel::tlm2::ClockPropertiesProxy(per_clk);
	}
	
	if(HAS_RTI_SUPPORT)
	{
		rti_clk_prop_proxy = new unisim::kernel::tlm2::ClockPropertiesProxy(rti_clk);
	}
	
	unsigned int channel_num;
	
	peripheral_slave_if(*this); // bind interface
	
	SC_HAS_PROCESS(PIT);

	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		load_timer[channel_num] = false;
	}
	
	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		std::stringstream irq_name_sstr;
		irq_name_sstr << "irq_" << channel_num;
		irq[channel_num] = new sc_core::sc_out<bool>(irq_name_sstr.str().c_str()); 
	}
	
	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		std::stringstream dma_trigger_name_sstr;
		dma_trigger_name_sstr << "dma_trigger_" << channel_num;
		dma_trigger[channel_num] = new sc_core::sc_out<bool>(dma_trigger_name_sstr.str().c_str()); 
	}

	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		std::stringstream gen_irq_event_name_sstr;
		gen_irq_event_name_sstr << "gen_irq_event_" << channel_num;
		gen_irq_event[channel_num] = new sc_core::sc_event(gen_irq_event_name_sstr.str().c_str());
	}

	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		std::stringstream gen_dma_pulse_event_name_sstr;
		gen_dma_pulse_event_name_sstr << "gen_dma_pulse_event_" << channel_num;
		gen_dma_pulse_event[channel_num] = new sc_core::sc_event(gen_dma_pulse_event_name_sstr.str().c_str());
	}

	// Map PIT registers regarding there address offsets
	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegister(pit_mcr.ADDRESS_OFFSET, &pit_mcr);
	if(HAS_64_BIT_TIMER_SUPPORT && (NUM_CHANNELS >= 2))
	{
		reg_addr_map.MapRegister(pit_ltmr64h.ADDRESS_OFFSET, &pit_ltmr64h);
		reg_addr_map.MapRegister(pit_ltmr64l.ADDRESS_OFFSET, &pit_ltmr64l);
	}
	if(HAS_RTI_SUPPORT)
	{
		reg_addr_map.MapRegister(pit_rti_ldval.ADDRESS_OFFSET, &pit_rti_ldval);
		reg_addr_map.MapRegister(pit_rti_cval.ADDRESS_OFFSET, &pit_rti_cval);
		reg_addr_map.MapRegister(pit_rti_tctrl.ADDRESS_OFFSET, &pit_rti_tctrl);
		reg_addr_map.MapRegister(pit_rti_tflg.ADDRESS_OFFSET, &pit_rti_tflg);
	}
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		reg_addr_map.MapRegister(PIT_LDVAL::ADDRESS_OFFSET + (4 * 4 * channel_num), &pit_ldval[channel_num]);
		reg_addr_map.MapRegister(PIT_CVAL::ADDRESS_OFFSET + (4 * 4 * channel_num), &pit_cval[channel_num]);
		reg_addr_map.MapRegister(PIT_TCTRL::ADDRESS_OFFSET + (4 * 4 * channel_num), &pit_tctrl[channel_num]);
		reg_addr_map.MapRegister(PIT_TFLG::ADDRESS_OFFSET + (4 * 4 * channel_num), &pit_tflg[channel_num]);
	}

	registers_registry.AddRegisterInterface(pit_mcr.CreateRegisterInterface());
	if(HAS_64_BIT_TIMER_SUPPORT && (NUM_CHANNELS >= 2))
	{
		registers_registry.AddRegisterInterface(pit_ltmr64h.CreateRegisterInterface());
		registers_registry.AddRegisterInterface(pit_ltmr64l.CreateRegisterInterface());
	}
	if(HAS_RTI_SUPPORT)
	{
		registers_registry.AddRegisterInterface(pit_rti_ldval.CreateRegisterInterface());
		registers_registry.AddRegisterInterface(pit_rti_cval.CreateRegisterInterface());
		registers_registry.AddRegisterInterface(pit_rti_tctrl.CreateRegisterInterface());
		registers_registry.AddRegisterInterface(pit_rti_tflg.CreateRegisterInterface());
	}
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		registers_registry.AddRegisterInterface(pit_ldval[channel_num].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(pit_cval[channel_num].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(pit_tctrl[channel_num].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(pit_tflg[channel_num].CreateRegisterInterface());
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
		sc_core::sc_spawn(sc_bind(&PIT<CONFIG>::IRQ_Process, this, channel_num), irq_process_name_sstr.str().c_str(), &irq_process_spawn_options);
	}
	
	if(HAS_DMA_SUPPORT)
	{
		// Spawn an DMA_TRIGGER_Process for each channel
		for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
		{
			sc_core::sc_spawn_options dma_trigger_process_spawn_options;
			dma_trigger_process_spawn_options.spawn_method();
			dma_trigger_process_spawn_options.dont_initialize();
			dma_trigger_process_spawn_options.set_sensitivity(gen_dma_pulse_event[channel_num]);
			
			std::stringstream dma_trigger_process_name_sstr;
			dma_trigger_process_name_sstr << "DMA_TRIGGER_Process_" << channel_num;
			sc_core::sc_spawn(sc_bind(&PIT<CONFIG>::DMA_TRIGGER_Process, this, channel_num), dma_trigger_process_name_sstr.str().c_str(), &dma_trigger_process_spawn_options);
		}
	}

	SC_METHOD(RTIRQ_Process);
	sensitive << gen_rtirq_event;
}

template <typename CONFIG>
PIT<CONFIG>::~PIT<CONFIG>()
{
	if(per_clk_prop_proxy) delete per_clk_prop_proxy;
	if(rti_clk_prop_proxy) delete rti_clk_prop_proxy;
	
	unsigned int channel_num;
	
	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		delete irq[channel_num];
	}
	
	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		delete dma_trigger[channel_num];
	}

	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		delete gen_irq_event[channel_num];
	}

	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		delete gen_dma_pulse_event[channel_num];
	}
}

template <typename CONFIG>
void PIT<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;

	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&PIT<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	// Spawn Process
	sc_core::sc_spawn_options process_spawn_options;
	process_spawn_options.spawn_method();
	process_spawn_options.set_sensitivity(&schedule.GetKernelEvent());
	process_spawn_options.set_sensitivity(&debug.value_changed_event());
	
	if((NUM_CHANNELS > 0) || HAS_RTI_SUPPORT)
	{
		if(NUM_CHANNELS > 0)
		{
			process_spawn_options.set_sensitivity(&per_clk_prop_proxy->GetClockPropertiesChangedEvent());
		}
		
		if(HAS_RTI_SUPPORT)
		{
			process_spawn_options.set_sensitivity(&rti_clk_prop_proxy->GetClockPropertiesChangedEvent());
		}
	}
	
	sc_core::sc_spawn(sc_bind(&PIT<CONFIG>::Process, this), "Process", &process_spawn_options);
	
	Reset();
}

template <typename CONFIG>
void PIT<CONFIG>::Reset()
{
	UpdateMasterClock();
	UpdatePERClock();
	UpdateRTIClock();
	
	unsigned int channel_num;
	
	load_rti_timer = false;
	for(channel_num = 0; channel_num < MAX_CHANNELS; channel_num++)
	{
		load_timer[channel_num] = false;
	}
	
	last_timers_run_time = sc_core::sc_time_stamp();
	unisim::kernel::tlm2::AlignToClock(last_timers_run_time, per_clock_period, per_clock_start_time, per_clock_posedge_first, per_clock_duty_cycle);
	
	last_rti_timer_run_time = sc_core::sc_time_stamp();
	unisim::kernel::tlm2::AlignToClock(last_rti_timer_run_time, rti_clock_period, rti_clock_start_time, rti_clock_posedge_first, rti_clock_duty_cycle);
	
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		SetIRQLevel(channel_num, false);
		SetDMA_TRIGGER_Level(channel_num, false);
	}
	
	SetRTIRQLevel(false);

	pit_mcr.Initialize(pit_mcr_reset_value);
	pit_ltmr64h.Initialize(0x0);
	pit_ltmr64l.Initialize(0x0);
	pit_rti_ldval.Initialize(0x0);
	pit_rti_cval.Initialize(0x0);
	pit_rti_tctrl.Initialize(0x0);
	pit_rti_tflg.Initialize(0x0);
	
	for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
	{
		pit_ldval[channel_num].Initialize(0x0);
		pit_cval[channel_num].Initialize(0x0);
		pit_tctrl[channel_num].Initialize(0x0);
		pit_tflg[channel_num].Initialize(0x0);
	}
	
	sc_core::sc_time per_clk_start_time;
	sc_core::sc_time rti_clk_start_time;
	
	if(NUM_CHANNELS > 0)
	{
		
		if(per_clk_prop_proxy->GetClockPosEdgeFirst())
		{
			// clock rising edge comes first
			per_clk_start_time = per_clk_prop_proxy->GetClockStartTime();
		}
		else
		{
			// clock falling edge comes first: skip time until first rising edge
			per_clk_start_time = per_clk_prop_proxy->GetClockStartTime();
			per_clk_start_time += (1.0 - per_clk_prop_proxy->GetClockDutyCycle()) * per_clk_prop_proxy->GetClockPeriod();
		}
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << "First PER clock rising edge is at " << per_clk_start_time << EndDebugInfo;
		}

		UpdatePERClock();
	}
	
	if(HAS_RTI_SUPPORT)
	{
		if(rti_clk_prop_proxy->GetClockPosEdgeFirst())
		{
			// clock rising edge comes first
			rti_clk_start_time = rti_clk_prop_proxy->GetClockStartTime();
		}
		else
		{
			// clock falling edge comes first: skip time until first rising edge
			rti_clk_start_time = rti_clk_prop_proxy->GetClockStartTime();
			rti_clk_start_time += (1.0 - rti_clk_prop_proxy->GetClockDutyCycle()) * rti_clk_prop_proxy->GetClockPeriod();
		}
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << "First RTI clock rising edge is at " << rti_clk_start_time << EndDebugInfo;
		}

		UpdateRTIClock();
	}

	if(NUM_CHANNELS > 0)
	{
		if(per_clk_start_time > last_timers_run_time)
		{
			last_timers_run_time = per_clk_start_time;
		}
	}

	if(HAS_RTI_SUPPORT)
	{
		if(rti_clk_start_time > last_rti_timer_run_time)
		{
			last_rti_timer_run_time = per_clk_start_time;
		}
	}

	RefreshFreeze();
	
	if((NUM_CHANNELS > 0) || HAS_RTI_SUPPORT)
	{
		ScheduleRun();
	}
}

template <typename CONFIG>
void PIT<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool PIT<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int PIT<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
tlm::tlm_sync_enum PIT<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
unisim::service::interfaces::Register *PIT<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void PIT<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void PIT<CONFIG>::ProcessEvent(Event *event)
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
void PIT<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	RunToTime(time_stamp); // Run to time
	
	Event *event = schedule.GetNextEvent();
	
	if(event)
	{
		do
		{
			if(event->GetTimeStamp() != time_stamp)
			{
				logger << DebugError << "Internal error: unexpected event of type " << event->GetType() << " at time stamp (" << event->GetTimeStamp() << " instead of " << time_stamp << ")" << EndDebugError;
				unisim::kernel::Object::Stop(-1);
			}
			
			ProcessEvent(event);
			schedule.FreeEvent(event);
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
	
	if((NUM_CHANNELS > 0) || HAS_RTI_SUPPORT)
	{
		ScheduleRun();
	}

	RefreshFreeze(); // sample freeze
}

template <typename CONFIG>
void PIT<CONFIG>::Process()
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
void PIT<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void PIT<CONFIG>::MasterClockPropertiesChangedProcess()
{
	UpdateMasterClock();
}

template <typename CONFIG>
void PIT<CONFIG>::UpdateMasterClock()
{
	if(NUM_CHANNELS > 0)
	{
		master_clock_period = m_clk_prop_proxy.GetClockPeriod();
		master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
		master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
		master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
	}
}

template <typename CONFIG>
void PIT<CONFIG>::UpdatePERClock()
{
	if(NUM_CHANNELS > 0)
	{
		per_clock_period = per_clk_prop_proxy->GetClockPeriod();
		per_clock_start_time = per_clk_prop_proxy->GetClockStartTime();
		per_clock_posedge_first = per_clk_prop_proxy->GetClockPosEdgeFirst();
		per_clock_duty_cycle = per_clk_prop_proxy->GetClockDutyCycle();
	}
}

template <typename CONFIG>
void PIT<CONFIG>::UpdateRTIClock()
{
	if(HAS_RTI_SUPPORT)
	{
		rti_clock_period = rti_clk_prop_proxy->GetClockPeriod();
		rti_clock_start_time = rti_clk_prop_proxy->GetClockStartTime();
		rti_clock_posedge_first = rti_clk_prop_proxy->GetClockPosEdgeFirst();
		rti_clock_duty_cycle = rti_clk_prop_proxy->GetClockDutyCycle();
	}
}

template <typename CONFIG>
void PIT<CONFIG>::WakeUp(const sc_core::sc_time& delay)
{
	Event *event = schedule.AllocEvent();
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += delay;
	event->WakeUp();
	event->SetTimeStamp(notify_time_stamp);
	schedule.Notify(event);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": will wake up at " << notify_time_stamp << EndDebugInfo;
	}
}

template <typename CONFIG>
void PIT<CONFIG>::LoadTimer(unsigned int channel_num)
{
	load_timer[channel_num] = true;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Timer #" << channel_num << " will load in " << per_clock_period << EndDebugInfo;
	}
	
	WakeUp(per_clock_period);
}

template <typename CONFIG>
void PIT<CONFIG>::LoadRTITimer()
{
	load_rti_timer = true;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": RTI Timer will load in " << rti_clock_period << EndDebugInfo;
	}
	
	WakeUp(rti_clock_period);
}

template <typename CONFIG>
void PIT<CONFIG>::SetIRQLevel(unsigned int channel_num, bool level)
{
	// Schedule IRQ output assertion
	irq_level[channel_num] = level;
	gen_irq_event[channel_num]->notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void PIT<CONFIG>::SetRTIRQLevel(bool level)
{
	if(HAS_RTI_SUPPORT)
	{
		// Schedule RTIRQ output assertion
		rtirq_level = level;
		gen_rtirq_event.notify(sc_core::SC_ZERO_TIME);
	}
}

template <typename CONFIG>
void PIT<CONFIG>::SetDMA_TRIGGER_Level(unsigned int channel_num, bool level)
{
	// Schedule DMA pulse
	dma_trigger_level[channel_num] = level;
	gen_dma_pulse_event[channel_num]->notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void PIT<CONFIG>::StartDMAPulse(unsigned int channel_num)
{
	// Schedule DMA pulse
	SetDMA_TRIGGER_Level(channel_num, true);
}

template <typename CONFIG>
void PIT<CONFIG>::LoadAndDecrementTimers(sc_dt::uint64 delta)
{
	if(delta)
	{
		if(!pit_mcr.template Get<typename PIT_MCR::MDIS>())
		{
			// Module is enabled
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": running enabled timers for " << delta << " PER clock cycles" << EndDebugInfo;
			}
			
			unsigned int channel_num;
			bool previous_timer_expired = false;
			bool timer_expired = false;
			
			for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++, previous_timer_expired = timer_expired)
			{
				if(pit_tctrl[channel_num].template Get<typename PIT_TCTRL::TEN>())
				{
					// timer is active
					sc_dt::uint64 dec_amount = delta;
				
					if(load_timer[channel_num])
					{
						if(unlikely(verbose))
						{
							logger << DebugInfo << sc_core::sc_time_stamp() << ": Loading Timer #" << channel_num << EndDebugInfo;
						}
						uint32_t ldval = pit_ldval[channel_num].template Get<typename PIT_LDVAL::TSV>();
							
						pit_cval[channel_num].template Set<typename PIT_CVAL::TVL>(ldval);
						
						dec_amount--;
						load_timer[channel_num] = false;
					}
					
					if(dec_amount)
					{
						if((channel_num != 0) && pit_tctrl[channel_num].template Get<typename PIT_TCTRL::CHN>())
						{
							// timer is chained to previous timer
							if(previous_timer_expired)
							{
								// decrement timer one time
								uint32_t cval = pit_cval[channel_num].template Get<typename PIT_CVAL::TVL>();
								
								if(cval == 0)
								{
									timer_expired = false;
									continue;
								}
								
								if(unlikely(verbose))
								{
									logger << DebugInfo << sc_core::sc_time_stamp() << ": PIT_CVAL" << channel_num << " decreases from " << cval;
								}
								cval = (cval > 1) ? (cval - 1) : 0;
								if(unlikely(verbose))
								{
									logger << DebugInfo << " to " << cval << EndDebugInfo;
								}
								
								pit_cval[channel_num].template Set<typename PIT_CVAL::TVL>(cval);
							}
						}
						else
						{
							// timer is not chained
							
							// decrement timer several times
							uint32_t cval = pit_cval[channel_num].template Get<typename PIT_CVAL::TVL>();
							
							if(cval == 0)
							{
								timer_expired = false;
								continue;
							}
							
							if(unlikely(verbose))
							{
								logger << DebugInfo << sc_core::sc_time_stamp() << ": PIT_CVAL" << channel_num << " decreases from " << cval;
							}
							cval = (cval > dec_amount) ? (cval - dec_amount) : 0;
							if(unlikely(verbose))
							{
								logger << DebugInfo << " to " << cval << EndDebugInfo;
							}
							
							pit_cval[channel_num].template Set<typename PIT_CVAL::TVL>(cval);
						}
						
						timer_expired = (pit_cval[channel_num].template Get<typename PIT_CVAL::TVL>() == 0);
						
						if(timer_expired)
						{
							// timer expired
							
							pit_tflg[channel_num].template Set<typename PIT_TFLG::TIF>(1);
							
							if(pit_tctrl[channel_num].template Get<typename PIT_TCTRL::TIE>())
							{
								// Timer interrupt requests are enabled
								SetIRQLevel(channel_num, true);
							}
							
							if(HAS_DMA_SUPPORT)
							{
								// start DMA pulse
								StartDMAPulse(channel_num);
							}
							
							// restart timer
							LoadTimer(channel_num);
						}
					}
					else
					{
						timer_expired = false;
					}
				}
				else
				{
					// timer is disabled
					timer_expired = false;
				}
			}
		}
	}
}

template <typename CONFIG>
void PIT<CONFIG>::LoadAndDecrementRTITimer(sc_dt::uint64 delta)
{
	if(!HAS_RTI_SUPPORT) return;
	
	if(delta)
	{
		if(!pit_mcr.template Get<typename PIT_MCR::MDIS_RTI>())
		{
			// RTI Module is enabled
			if(unlikely(verbose))
			{
				logger << DebugInfo << "running RTI timer for " << delta << " RTI clock cycles" << EndDebugInfo;
			}
			
			sc_dt::uint64 dec_amount = delta;
			
			if(load_rti_timer)
			{
				uint32_t ldval = pit_rti_ldval.template Get<typename PIT_RTI_LDVAL::TSV>();
		
				if(ldval < 32)
				{
					logger << DebugWarning << pit_rti_ldval.GetName() << " value should be greater or equal than 32" << EndDebugWarning; 
				}

				pit_rti_cval.template Set<typename PIT_RTI_CVAL::TVL>(ldval);

				dec_amount--;
				load_rti_timer = false;
			}
			
			if(dec_amount)
			{
				// Decrement RTI timer
				uint32_t cval = pit_rti_cval.template Get<typename PIT_RTI_CVAL::TVL>();
				
				cval = (cval > dec_amount) ? (cval - dec_amount) : 0;
				
				pit_rti_cval.template Set<typename PIT_RTI_CVAL::TVL>(cval);
				
				bool timer_expired = (pit_rti_cval.template Get<typename PIT_RTI_CVAL::TVL>() == 0);
				
				if(timer_expired)
				{
					// RTI timer expired
					
					pit_rti_tflg.template Set<typename PIT_RTI_TFLG::TIF>(1);
					
					if(pit_rti_tctrl.template Get<typename PIT_RTI_TCTRL::TIE>())
					{
						// RTI Timer interrupt requests are enabled
						SetRTIRQLevel(true);
					}
					
					// restart RTI timer
					LoadRTITimer();
				}
			}
		}
	}
}

template <typename CONFIG>
sc_dt::int64 PIT<CONFIG>::TicksToNextTimersRun()
{
	if(freeze_timers) return 0;
	
	sc_dt::int64 ticks_to_next_timers_run = 0;
	
	if(!pit_mcr.template Get<typename PIT_MCR::MDIS>())
	{
		// Module is enabled
		
		unsigned int channel_num;
		sc_dt::int64 previous_ticks_to_expiration = 0;
		sc_dt::int64 ticks_to_expiration = 0;
		
		for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++, previous_ticks_to_expiration = ticks_to_expiration)
		{
			if(pit_tctrl[channel_num].template Get<typename PIT_TCTRL::TEN>())
			{
				// timer is active
				
				if((channel_num != 0) && pit_tctrl[channel_num].template Get<typename PIT_TCTRL::CHN>())
				{
					// timer is chained to previous timer
					
					uint32_t cval = pit_cval[channel_num].template Get<typename PIT_CVAL::TVL>();
					
					if(cval == 1)
					{
						ticks_to_expiration = previous_ticks_to_expiration;

						if(ticks_to_expiration && (!ticks_to_next_timers_run || (ticks_to_expiration < ticks_to_next_timers_run))) ticks_to_next_timers_run = ticks_to_expiration;
					}
					else
					{
						ticks_to_expiration = 0;
					}
				}
				else
				{
					// timer is not chained
					
					uint32_t cval = pit_cval[channel_num].template Get<typename PIT_CVAL::TVL>();
					
					ticks_to_expiration = cval;
					
					if(ticks_to_expiration && (!ticks_to_next_timers_run || (ticks_to_expiration < ticks_to_next_timers_run))) ticks_to_next_timers_run = ticks_to_expiration;
				}
			}
			else
			{
				// timer is disabled
				ticks_to_expiration = 0;
			}
		}
	}
	
	return ticks_to_next_timers_run;
}

template <typename CONFIG>
sc_dt::int64 PIT<CONFIG>::TicksToNextRTITimerRun()
{
	if(freeze_rti_timer) return 0;
	
	if(HAS_RTI_SUPPORT)
	{
		if(!pit_mcr.template Get<typename PIT_MCR::MDIS_RTI>())
		{
			// RTI Module is enabled
			
			if(pit_rti_tctrl.template Get<typename PIT_RTI_TCTRL::TEN>())
			{
				// RTI timer is active
				
				uint32_t cval = pit_rti_cval.template Get<typename PIT_RTI_CVAL::TVL>();
					
				sc_dt::int64 ticks_to_expiration = cval;
					
				return ticks_to_expiration ? ticks_to_expiration : 0;
			}
		}
	}
	
	return 0;
}

template <typename CONFIG>
sc_core::sc_time PIT<CONFIG>::TimeToNextTimersRun()
{
	if(!pit_mcr.template Get<typename PIT_MCR::MDIS>())
	{
		// Module is enabled
		unsigned int channel_num;
		for(channel_num = 0; channel_num < NUM_CHANNELS; channel_num++)
		{
			if(load_timer[channel_num])
			{
				return per_clock_period;
			}
		}
	}

	sc_dt::int64 ticks_to_next_timers_run = TicksToNextTimersRun();
	
	if(ticks_to_next_timers_run)
	{
		sc_core::sc_time time_to_next_timers_run(per_clock_period);
		time_to_next_timers_run *= ticks_to_next_timers_run;
		if(time_to_next_timers_run < max_time_to_next_timers_run) return time_to_next_timers_run;
	}
	
	return max_time_to_next_timers_run;
}

template <typename CONFIG>
sc_core::sc_time PIT<CONFIG>::TimeToNextRTITimerRun()
{
	if(!pit_mcr.template Get<typename PIT_MCR::MDIS>())
	{
		// RTI Module is enabled
		if(load_rti_timer) return rti_clock_period;
	}
	
	sc_dt::int64 ticks_to_next_rti_timer_run = TicksToNextRTITimerRun();
	
	if(ticks_to_next_rti_timer_run)
	{
		sc_core::sc_time time_to_next_rti_timer_run(rti_clock_period);
		time_to_next_rti_timer_run *= ticks_to_next_rti_timer_run;
		if(time_to_next_rti_timer_run < max_time_to_next_rti_timer_run) return time_to_next_rti_timer_run;
	}
	
	return max_time_to_next_rti_timer_run;
}

template <typename CONFIG>
sc_core::sc_time PIT<CONFIG>::TimeToNextRun()
{
	sc_core::sc_time time_to_next_timers_run(TimeToNextTimersRun());
	sc_core::sc_time time_to_next_rti_timer_run(TimeToNextRTITimerRun());
	
	return (time_to_next_timers_run < time_to_next_rti_timer_run) ? time_to_next_timers_run : time_to_next_rti_timer_run;
}

template <typename CONFIG>
void PIT<CONFIG>::RunToTime(const sc_core::sc_time& time_stamp)
{
	if(NUM_CHANNELS > 0)
	{
		if(!freeze_timers)
		{
			if(time_stamp > last_timers_run_time)
			{
				// Compute the elapsed time since last timers run
				sc_core::sc_time delay_since_last_timers_run(time_stamp);
				delay_since_last_timers_run -= last_timers_run_time;
				
				// Compute number of timers ticks since last run
				sc_dt::uint64 delta = delay_since_last_timers_run / per_clock_period;
				
				if(delta)
				{
					sc_core::sc_time run_time(per_clock_period);
					run_time *= delta;
				
					LoadAndDecrementTimers(delta);
					last_timers_run_time += run_time;
				}
			}
		}
	}
	
	if(HAS_RTI_SUPPORT)
	{
		if(!freeze_rti_timer)
		{
			if(time_stamp > last_rti_timer_run_time)
			{
				// Compute the elapsed time since last RTI timer run
				sc_core::sc_time delay_since_last_rti_timer_run(time_stamp);
				delay_since_last_rti_timer_run -= last_rti_timer_run_time;
				
				// Compute number of RTI timer ticks since last run
				sc_dt::uint64 delta = delay_since_last_rti_timer_run / per_clock_period;
				
				if(delta)
				{
					sc_core::sc_time run_time(rti_clock_period);
					run_time *= delta;
					
					LoadAndDecrementRTITimer(delta);
					last_rti_timer_run_time += run_time;
				}
			}
		}
	}
}

template <typename CONFIG>
void PIT<CONFIG>::ScheduleRun()
{
	// Clocks properties may have changed that may affect time to next timers and RTI timer run
	UpdatePERClock();
	
	if(HAS_RTI_SUPPORT)
	{
		UpdateRTIClock();
	}
	
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	if(last_timers_run_time > time_stamp)
	{
		// Timer not yet started (because of clock)
		Event *event = schedule.AllocEvent();
		event->WakeUp();
		event->SetTimeStamp(last_timers_run_time);
		schedule.Notify(event);
		return;
	}
	
	if(last_rti_timer_run_time > time_stamp)
	{
		// RTI Timer not yet started (because of clock)
		Event *event = schedule.AllocEvent();
		event->WakeUp();
		event->SetTimeStamp(last_rti_timer_run_time);
		schedule.Notify(event);
		return;
	}
	
	sc_core::sc_time time_to_next_run = TimeToNextRun();
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": time to next timers/RTI timer run is " << time_to_next_run << EndDebugInfo;
	}
	
	sc_core::sc_time next_run_time_stamp(sc_core::sc_time_stamp());
	next_run_time_stamp += time_to_next_run;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": timers/RTI timer will run at " << next_run_time_stamp << EndDebugInfo;
	}
	
	Event *event = schedule.AllocEvent();
	event->WakeUp();
	event->SetTimeStamp(next_run_time_stamp);
	schedule.Notify(event);
}

template <typename CONFIG>
void PIT<CONFIG>::IRQ_Process(unsigned int channel_num)
{
	// Set IRQ output
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << irq[channel_num]->name() << " <- " << irq_level[channel_num] << EndDebugInfo;
	}
	*irq[channel_num] = irq_level[channel_num];
}

template <typename CONFIG>
void PIT<CONFIG>::RTIRQ_Process()
{
	if(HAS_RTI_SUPPORT)
	{
		// Set IRQ output
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": " << rtirq.name() << " <- " << rtirq_level << EndDebugInfo;
		}
		rtirq = rtirq_level;
	}
}

template <typename CONFIG>
void PIT<CONFIG>::DMA_TRIGGER_Process(unsigned int channel_num)
{
	// Set DMA trigger output
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << dma_trigger[channel_num]->name() << " <- " << dma_trigger_level[channel_num] << EndDebugInfo;
	}
	
	*dma_trigger[channel_num] = dma_trigger_level[channel_num];
	
	if(dma_trigger_level[channel_num])
	{
		dma_trigger_level[channel_num] = false;
		gen_dma_pulse_event[channel_num]->notify(master_clock_period);
	}
}

template <typename CONFIG>
void PIT<CONFIG>::RefreshFreeze()
{
	// Latch value for internal "freeze"
	bool old_freeze_timers = freeze_timers;
	bool old_freeze_rti_timer = freeze_rti_timer;
	
	freeze_timers = pit_mcr.template Get<typename PIT_MCR::MDIS>() || (debug && pit_mcr.template Get<typename PIT_MCR::FRZ>());
	freeze_rti_timer = pit_mcr.template Get<typename PIT_MCR::MDIS_RTI>() || (debug && pit_mcr.template Get<typename PIT_MCR::FRZ>());
	
	if(old_freeze_timers && !freeze_timers)
	{
		last_timers_run_time = sc_core::sc_time_stamp();
		unisim::kernel::tlm2::AlignToClock(last_timers_run_time, per_clock_period, per_clock_start_time, per_clock_posedge_first, per_clock_duty_cycle);
	}
	
	if(old_freeze_rti_timer && !freeze_rti_timer)
	{
		last_rti_timer_run_time = sc_core::sc_time_stamp();
		unisim::kernel::tlm2::AlignToClock(last_rti_timer_run_time, rti_clock_period, rti_clock_start_time, rti_clock_posedge_first, rti_clock_duty_cycle);
	}
}

} // end of namespace pit
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace timer
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_TIMER_FREESCALE_MPC57XX_PIT_PIT_TCC__
