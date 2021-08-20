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

#ifndef __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_DMAMUX_DMAMUX_TCC__
#define __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_DMAMUX_DMAMUX_TCC__

#include <unisim/component/tlm2/dma/freescale/mpc57xx/dmamux/dmamux.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace dma {
namespace freescale {
namespace mpc57xx {
namespace dmamux {

template <typename CONFIG>
const unsigned int DMAMUX<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int DMAMUX<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int DMAMUX<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int DMAMUX<CONFIG>::NUM_DMA_CHANNELS;

template <typename CONFIG>
const unsigned int DMAMUX<CONFIG>::NUM_DMA_SOURCES;

template <typename CONFIG>
const unsigned int DMAMUX<CONFIG>::NUM_DMA_ALWAYS_ON;

template <typename CONFIG>
const unsigned int DMAMUX<CONFIG>::NUM_DMA_TRIGGERS;

template <typename CONFIG>
const unsigned int DMAMUX<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const bool DMAMUX<CONFIG>::threaded_model;

template <typename CONFIG>
DMAMUX<CONFIG>::DMAMUX(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, reset_b("reset_b")
	, dma_source()
	, dma_always_on()
	, dma_channel_ack()
	, dma_trigger()
	, dma_channel()
	, dma_source_ack()
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, dmamux_chcfg(this)
	, reg_addr_map()
	, registers_registry()
	, schedule()
	, disable_dma_source()
	, dma_source_routing_change_event()
	, dma_chcfg_event()
	, dma_source_event()
	, dma_source_ack_event()
	, conf_chk(false)
	, conf_ok(false)
	, routing_table()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, param_disable_dma_source("disable-dma-source", this, disable_dma_source, NUM_DMA_SOURCES, "disable/enable DMA source")
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
{
	param_disable_dma_source.SetMutable(false);
	
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Direct Memory Access Multiplexer (DMAMUX):" << std::endl;
	description_sstr << "  - " << NUM_DMA_CHANNELS << " DMA channel(s)" << std::endl;
	description_sstr << "  - " << NUM_DMA_SOURCES << " DMA source(s)" << std::endl;
	description_sstr << "  - " << NUM_DMA_ALWAYS_ON << " DMA always on slot(s)" << std::endl;
	description_sstr << "  - " << NUM_DMA_TRIGGERS << " DMA trigger(s)" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 25" << std::endl;
	this->SetDescription(description_sstr.str().c_str());

	unsigned int dma_channel_num;
	unsigned int dma_source_num;
	unsigned int dma_always_on_num;
	unsigned int dma_trigger_num;
	
	peripheral_slave_if(*this); // bind interface
	
	SC_HAS_PROCESS(DMAMUX);

	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		std::stringstream dma_source_name_sstr;
		dma_source_name_sstr << "dma_source_" << dma_source_num;
		dma_source[dma_source_num] = new sc_core::sc_in<bool>(dma_source_name_sstr.str().c_str()); 
	}

	for(dma_always_on_num = 0; dma_always_on_num < NUM_DMA_ALWAYS_ON; dma_always_on_num++)
	{
		std::stringstream dma_always_on_name_sstr;
		dma_always_on_name_sstr << "dma_always_on_" << dma_always_on_num;
		dma_always_on[dma_always_on_num] = new sc_core::sc_in<bool>(dma_always_on_name_sstr.str().c_str()); 
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream dma_channel_ack_name_sstr;
		dma_channel_ack_name_sstr << "dma_channel_ack_" << dma_channel_num;
		dma_channel_ack[dma_channel_num] = new sc_core::sc_in<bool>(dma_channel_ack_name_sstr.str().c_str()); 
	}

	for(dma_trigger_num = 0; dma_trigger_num < NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		std::stringstream dma_trigger_name_sstr;
		dma_trigger_name_sstr << "dma_trigger_" << dma_trigger_num;
		dma_trigger[dma_trigger_num] = new sc_core::sc_in<bool>(dma_trigger_name_sstr.str().c_str()); 
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream dma_channel_name_sstr;
		dma_channel_name_sstr << "dma_channel_" << dma_channel_num;
		dma_channel[dma_channel_num] = new sc_core::sc_out<bool>(dma_channel_name_sstr.str().c_str()); 
	}

	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		std::stringstream dma_source_ack_name_sstr;
		dma_source_ack_name_sstr << "dma_source_ack_" << dma_source_num;
		dma_source_ack[dma_source_num] = new sc_core::sc_out<bool>(dma_source_ack_name_sstr.str().c_str()); 
	}

	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		std::stringstream dma_source_routing_change_event_name_sstr;
		dma_source_routing_change_event_name_sstr << "dma_source_routing_change_event_" << dma_source_num;
		dma_source_routing_change_event[dma_source_num] = new sc_core::sc_event(dma_source_routing_change_event_name_sstr.str().c_str());
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream dma_chcfg_event_name_sstr;
		dma_chcfg_event_name_sstr << "dma_chcfg_event_" << dma_channel_num;
		dma_chcfg_event[dma_channel_num] = new sc_core::sc_event(dma_chcfg_event_name_sstr.str().c_str());
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream dma_source_event_name_sstr;
		dma_source_event_name_sstr << "dma_source_event_" << dma_channel_num;
		dma_source_event[dma_channel_num] = new sc_core::sc_event(dma_source_event_name_sstr.str().c_str());
	}
	
	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		std::stringstream dma_source_ack_event_name_sstr;
		dma_source_ack_event_name_sstr << "dma_source_ack_event_" << dma_source_num;
		dma_source_ack_event[dma_source_num] = new sc_core::sc_event(dma_source_ack_event_name_sstr.str().c_str()); 
	}

	// Map DMAMUX registers regarding there address offsets
	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegisterFile(DMAMUX_CHCFG::ADDRESS_OFFSET, &dmamux_chcfg);

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		registers_registry.AddRegisterInterface(dmamux_chcfg[dma_channel_num].CreateRegisterInterface());
	}
		
	SC_METHOD(RESET_B_Process);
	sensitive << reset_b.pos();

	if(threaded_model)
	{
		SC_THREAD(Process);
		sensitive << schedule.GetKernelEvent();
	}
	else
	{
		SC_METHOD(Process);
		sensitive << schedule.GetKernelEvent();
	}
}

template <typename CONFIG>
DMAMUX<CONFIG>::~DMAMUX<CONFIG>()
{
	unsigned int dma_channel_num;
	unsigned int dma_source_num;
	unsigned int dma_always_on_num;
	unsigned int dma_trigger_num;
	
	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		delete dma_source[dma_source_num];
	}

	for(dma_always_on_num = 0; dma_always_on_num < NUM_DMA_ALWAYS_ON; dma_always_on_num++)
	{
		delete dma_always_on[dma_always_on_num];
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete dma_channel_ack[dma_channel_num];
	}

	for(dma_trigger_num = 0; dma_trigger_num < NUM_DMA_TRIGGERS; dma_trigger_num++)
	{
		delete dma_trigger[dma_trigger_num];
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete dma_channel[dma_channel_num];
	}

	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		delete dma_source_ack[dma_source_num]; 
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_SOURCES; dma_channel_num++)
	{
		delete dma_source_routing_change_event[dma_channel_num];
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete dma_source_event[dma_channel_num];
	}
	
	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		delete dma_source_ack_event[dma_source_num]; 
	}
}

template <typename CONFIG>
void DMAMUX<CONFIG>::end_of_elaboration()
{
	unsigned int dma_channel_num;
	unsigned int dma_source_num;

	logger << DebugInfo << this->GetDescription() << EndDebugInfo;

	// Spawn a DMA_SOURCE_Process for each DMA source
	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		sc_core::sc_spawn_options dma_source_process_spawn_options;
		dma_source_process_spawn_options.spawn_method();
		dma_source_process_spawn_options.set_sensitivity(&dma_source[dma_source_num]->value_changed());
		dma_source_process_spawn_options.set_sensitivity(dma_source_routing_change_event[dma_source_num]);
		
		std::stringstream dma_source_process_name_sstr;
		dma_source_process_name_sstr << "DMA_SOURCE_Process_" << dma_source_num;
		sc_core::sc_spawn(sc_bind(&DMAMUX<CONFIG>::DMA_SOURCE_Process, this, dma_source_num), dma_source_process_name_sstr.str().c_str(), &dma_source_process_spawn_options);
	}

	// Spawn a DMA_CHANNEL_Process for each DMA channel
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		sc_core::sc_spawn_options dma_channel_process_spawn_options;
		dma_channel_process_spawn_options.spawn_method();
		dma_channel_process_spawn_options.set_sensitivity(dma_source_event[dma_channel_num]);
		dma_channel_process_spawn_options.set_sensitivity(dma_chcfg_event[dma_channel_num]);
		if(dma_channel_num < NUM_DMA_TRIGGERS)
		{
			dma_channel_process_spawn_options.set_sensitivity(&dma_trigger[dma_channel_num]->pos());
		}
		
		std::stringstream dma_channel_process_name_sstr;
		dma_channel_process_name_sstr << "DMA_CHANNEL_Process_" << dma_channel_num;
		sc_core::sc_spawn(sc_bind(&DMAMUX<CONFIG>::DMA_CHANNEL_Process, this, dma_channel_num), dma_channel_process_name_sstr.str().c_str(), &dma_channel_process_spawn_options);
	}

	// Spawn a DMA_CHANNEL_ACK_Process for each DMA channel
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		sc_core::sc_spawn_options dma_channel_ack_process_spawn_options;
		dma_channel_ack_process_spawn_options.spawn_method();
		dma_channel_ack_process_spawn_options.set_sensitivity(dma_channel_ack[dma_channel_num]);
		
		std::stringstream dma_channel_ack_process_name_sstr;
		dma_channel_ack_process_name_sstr << "DMA_CHANNEL_ACK_Process_" << dma_channel_num;
		sc_core::sc_spawn(sc_bind(&DMAMUX<CONFIG>::DMA_CHANNEL_ACK_Process, this, dma_channel_num), dma_channel_ack_process_name_sstr.str().c_str(), &dma_channel_ack_process_spawn_options);
	}

	// Spawn a DMA_SOURCE_ACK_Process for each DMA source
	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		sc_core::sc_spawn_options dma_source_ack_process_spawn_options;
		dma_source_ack_process_spawn_options.spawn_method();
		dma_source_ack_process_spawn_options.set_sensitivity(dma_source_ack_event[dma_source_num]);
		
		std::stringstream dma_source_ack_process_name_sstr;
		dma_source_ack_process_name_sstr << "DMA_SOURCE_ACK_Process_" << dma_source_num;
		sc_core::sc_spawn(sc_bind(&DMAMUX<CONFIG>::DMA_SOURCE_ACK_Process, this, dma_source_num), dma_source_ack_process_name_sstr.str().c_str(), &dma_source_ack_process_spawn_options);
	}

	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&DMAMUX<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void DMAMUX<CONFIG>::Reset()
{
	UpdateMasterClock();
	
	unsigned int dma_channel_num;
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		dmamux_chcfg[dma_channel_num].Reset();
	}
	
	conf_chk = false;
	conf_ok = false;
}

template <typename CONFIG>
void DMAMUX<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool DMAMUX<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int DMAMUX<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
			return reg_addr_map.DebugWrite(start_addr, data_ptr, data_length);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map.DebugRead(start_addr, data_ptr, data_length);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum DMAMUX<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
unisim::service::interfaces::Register *DMAMUX<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void DMAMUX<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void DMAMUX<CONFIG>::ProcessEvent(Event *event)
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
		else
		{
			ReadWriteStatus rws = RWS_OK;
			
			switch(cmd)
			{
				case tlm::TLM_WRITE_COMMAND:
					rws = reg_addr_map.Write(start_addr, data_ptr, data_length);
					break;
				case tlm::TLM_READ_COMMAND:
					rws = reg_addr_map.Read(start_addr, data_ptr, data_length);
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
}

template <typename CONFIG>
void DMAMUX<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	Event *event = schedule.GetNextEvent();
	
	if(event)
	{
		do
		{
			if(event->GetTimeStamp() != time_stamp)
			{
				logger << DebugError << "Internal error: unexpected event at time stamp (" << event->GetTimeStamp() << " instead of " << time_stamp << ")" << EndDebugError;
				unisim::kernel::Object::Stop(-1);
			}
			
			ProcessEvent(event);
			schedule.FreeEvent(event);
		}
		while((event = schedule.GetNextEvent()) != 0);
	}
}

template <typename CONFIG>
void DMAMUX<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			wait();
			ProcessEvents();
		}
	}
	else
	{
		ProcessEvents();
		next_trigger();
	}
}

template <typename CONFIG>
void DMAMUX<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void DMAMUX<CONFIG>::DMA_SOURCE_Process(unsigned int dma_source_num)
{
	if(!disable_dma_source[dma_source_num])
	{
		if(!conf_chk && !CheckConfiguration())
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": setting multiple CHCFG registers with the same source value will result in unpredictable behavior; This is true, even if a channel is disabled (ENBL==0)" << EndDebugWarning;
		}
		
		unsigned int dma_channel_num = routing_table[dma_source_num];
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": routing source #" << dma_source_num << " -> channel #" << dma_channel_num << EndDebugInfo;
		}
		
		dma_source_event[dma_channel_num]->notify(/*sc_core::SC_ZERO_TIME*/);
	}
}

template <typename CONFIG>
void DMAMUX<CONFIG>::DMA_CHANNEL_Process(unsigned int dma_channel_num)
{
	const DMAMUX_CHCFG& chcfg = dmamux_chcfg[dma_channel_num];
	
	bool dma_channel_value = false;
	
	if(chcfg.template Get<typename DMAMUX_CHCFG::ENBL>()) // DMA channel enabled ?
	{
		// DMA channel enabled
		unsigned int dma_source_num = chcfg.template Get<typename DMAMUX_CHCFG::SOURCE>();
		
		if(!disable_dma_source[dma_source_num])
		{
			bool dma_source_value = dma_source[dma_source_num]->read();
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": " << dma_source[dma_source_num]->name() << " = " << dma_source_value << EndDebugInfo;
			}
				
			if(dma_source_num >= NUM_DMA_TRIGGERS) // last sources are not triggered
			{
				dma_channel_value = dma_source_value;
			}
			else
			{
				bool dma_trigger_value = dma_trigger[dma_channel_num]->read();
				
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ": " << dma_trigger[dma_channel_num]->name() << " = " << dma_trigger_value << EndDebugInfo;
				}
				
				if(chcfg.template Get<typename DMAMUX_CHCFG::TRIG>()) // channel is triggered ?
				{
					dma_channel_value = dma_trigger[dma_channel_num]->posedge() && dma_source_value;
				}
			}
		}
	}
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << dma_channel[dma_channel_num]->name() << " <- " << dma_channel_value << EndDebugInfo;
	}
	
	dma_channel[dma_channel_num]->write(dma_channel_value);
}

template <typename CONFIG>
void DMAMUX<CONFIG>::DMA_CHANNEL_ACK_Process(unsigned int dma_channel_num)
{
	const DMAMUX_CHCFG& chcfg = dmamux_chcfg[dma_channel_num];
	unsigned int dma_source_num = chcfg.template Get<typename DMAMUX_CHCFG::SOURCE>();
	
	dma_source_ack_event[dma_source_num]->notify(/*sc_core::SC_ZERO_TIME*/);
}

template <typename CONFIG>
void DMAMUX<CONFIG>::DMA_SOURCE_ACK_Process(unsigned int dma_source_num)
{
	unsigned int dma_channel_num = routing_table[dma_source_num];
	
	bool dma_channel_ack_value = dma_channel_ack[dma_channel_num]->read();
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << dma_source_ack[dma_source_num]->name() << " <- " << dma_channel_ack_value << EndDebugInfo;
	}
	
	dma_source_ack[dma_source_num]->write(dma_channel_ack_value);
}

template <typename CONFIG>
void DMAMUX<CONFIG>::MasterClockPropertiesChangedProcess()
{
	UpdateMasterClock();
}

template <typename CONFIG>
void DMAMUX<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void DMAMUX<CONFIG>::UpdateChannel(unsigned int dma_channel_num)
{
	const DMAMUX_CHCFG& chcfg = dmamux_chcfg[dma_channel_num];
	unsigned int dma_source_num = chcfg.template Get<typename DMAMUX_CHCFG::SOURCE>();
	unsigned int old_dma_channel_num = routing_table[dma_source_num];
	if(old_dma_channel_num != dma_channel_num)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":route change: source #" << dma_source_num << " -> channel #" << dma_channel_num << EndDebugInfo;
		}
		
		routing_table[dma_source_num] = dma_channel_num;
		
		dma_chcfg_event[old_dma_channel_num]->notify(sc_core::SC_ZERO_TIME);
		dma_source_routing_change_event[dma_source_num]->notify(sc_core::SC_ZERO_TIME);
	}
	
	dma_chcfg_event[dma_channel_num]->notify(sc_core::SC_ZERO_TIME);
	
	conf_chk = false;
}

template <typename CONFIG>
bool DMAMUX<CONFIG>::CheckConfiguration()
{
	if(conf_chk) return conf_ok;
	
	unsigned int dma_source_num;
	unsigned int dma_channel_num;
	bool coverage_map[NUM_DMA_SOURCES];
	
	for(dma_source_num = 0; dma_source_num < NUM_DMA_SOURCES; dma_source_num++)
	{
		coverage_map[dma_source_num] = false;
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		const DMAMUX_CHCFG& chcfg = dmamux_chcfg[dma_channel_num];
		
		dma_source_num = chcfg.template Get<typename DMAMUX_CHCFG::SOURCE>();
		
		if(!disable_dma_source[dma_source_num])
		{
			if(coverage_map[dma_source_num])
			{
				conf_chk = true;
				conf_ok = false;
				return false;
			}
			
			coverage_map[dma_source_num] = true;
		}
	}
	
	conf_chk = true;
	conf_ok = true;
	return true;
}

} // end of namespace dmamux
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace dmamux
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_DMAMUX_DMAMUX_TCC__
