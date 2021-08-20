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

#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_SEMAPHORE_FREESCALE_MPC57XX_SEMA42_TCC__
#define __UNISIM_COMPONENT_TLM2_MEMORY_SEMAPHORE_FREESCALE_MPC57XX_SEMA42_TCC__

#include <unisim/component/tlm2/memory/semaphore/freescale/mpc57xx/sema42/sema42.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/likely/likely.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace semaphore {
namespace freescale {
namespace mpc57xx {
namespace sema42 {

template <typename CONFIG>
const unsigned int SEMA42<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int SEMA42<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int SEMA42<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int SEMA42<CONFIG>::NUM_GATES;

template <typename CONFIG>
const unsigned int SEMA42<CONFIG>::MAX_GATES;

template <typename CONFIG>
const unsigned int SEMA42<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const uint8_t SEMA42<CONFIG>::RESET_GATE_KEY1;

template <typename CONFIG>
const uint8_t SEMA42<CONFIG>::RESET_GATE_KEY2;

template <typename CONFIG>
const bool SEMA42<CONFIG>::threaded_model;

template <typename CONFIG>
SEMA42<CONFIG>::SEMA42(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, reset_b("reset_b")
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, sema42_gate(this)
	, sema42_rstgt_w(this)
	, sema42_rstgt_r(this)
	, read_reg_addr_map()
	, write_reg_addr_map()
	, registers_registry()
	, schedule()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, default_trans_attr()
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
{
	default_trans_attr.set_privileged(true);
	default_trans_attr.set_instruction(false);
	default_trans_attr.set_master_id(0);

	std::stringstream description_sstr;
	description_sstr << "MPC57XX Semaphores2 (SEMA42):" << std::endl;
	description_sstr << "  - " << NUM_GATES << " Gate(s)" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 22" << std::endl;
	this->SetDescription(description_sstr.str().c_str());

	peripheral_slave_if(*this); // bind interface
	
	SC_HAS_PROCESS(SEMA42);

	// Map SEMA42 registers regarding there address offsets
	read_reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	read_reg_addr_map.SetEndian(endian);
	read_reg_addr_map.MapRegisterFile(SEMA42_GATE::ADDRESS_OFFSET, &sema42_gate);
	read_reg_addr_map.MapRegister(sema42_rstgt_r.ADDRESS_OFFSET, &sema42_rstgt_r);

	write_reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	write_reg_addr_map.SetEndian(endian);
	write_reg_addr_map.MapRegisterFile(SEMA42_GATE::ADDRESS_OFFSET, &sema42_gate);
	write_reg_addr_map.MapRegister(sema42_rstgt_w.ADDRESS_OFFSET, &sema42_rstgt_w);

	for(unsigned int i = 0; i < NUM_GATES; i++)
	{
		registers_registry.AddRegisterInterface(sema42_gate[i].CreateRegisterInterface());
	}
	registers_registry.AddRegisterInterface(sema42_rstgt_r.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(sema42_rstgt_w.CreateRegisterInterface());

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
SEMA42<CONFIG>::~SEMA42<CONFIG>()
{
}

template <typename CONFIG>
void SEMA42<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;

	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&SEMA42<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void SEMA42<CONFIG>::Reset()
{
	UpdateMasterClock();
	
	ResetAllGates();
	sema42_rstgt_w.Reset();
	sema42_rstgt_r.Reset();
}

template <typename CONFIG>
void SEMA42<CONFIG>::ResetGate(unsigned int gate_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":resetting gate #" << gate_num << EndDebugInfo;
	}
	sema42_gate[gate_num].Reset();
}

template <typename CONFIG>
void SEMA42<CONFIG>::ResetAllGates()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":resetting all gates" << EndDebugInfo;
	}

	unsigned int gate_num;
	
	for(gate_num = 0; gate_num < NUM_GATES; gate_num++)
	{
		sema42_gate[gate_num].Reset();
	}
}

template <typename CONFIG>
ReadWriteStatus SEMA42<CONFIG>::ResetGateFSM(tlm_trans_attr& trans_attr, const uint16_t& value, const uint16_t& bit_enable)
{
	ReadWriteStatus rws = RWS_OK;
	
	unsigned int rstgsm = sema42_rstgt_r.template Get<typename SEMA42_RSTGT_R::RSTGSM>(); // Reset Gate Finite State Machine
	
	switch(rstgsm)
	{
		case RSTGSM_IDLE_WAITING_1ST_WRITE    :  // 00b: Idle, waiting for the first data pattern write
		{
			sema42_rstgt_w.SEMA42_RSTGT_W::Super::template WritePreserve<typename SEMA42_RSTGT_W::RSTGTN>(value, bit_enable);
			
			unsigned int rstgdp = sema42_rstgt_w.template Get<typename SEMA42_RSTGT_W::RSTGDP>(); // Reset Gate Data Pattern
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":reset FSM:got data pattern 0x" << std::hex << rstgdp << std::dec << EndDebugInfo;
			}
			
			if(rstgdp == RESET_GATE_KEY1)
			{
				// got first data pattern write
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":reset FSM:got first data pattern write" << EndDebugInfo;
				}
				sema42_rstgt_r.template Set<typename SEMA42_RSTGT_R::RSTGSM>(RSTGSM_WAITING_2ND_WRITE);
			}
			else
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":reset FSM:did not get first data pattern write" << EndDebugInfo;
				}
				sema42_rstgt_r.Reset();
			}
			break;
		}
		case RSTGSM_WAITING_2ND_WRITE         :  // 01b: Waiting for the second data pattern write
		{
			rws = sema42_rstgt_w.SEMA42_RSTGT_W::Super::Write(value, bit_enable);
			
			unsigned int rstgdp = sema42_rstgt_w.template Get<typename SEMA42_RSTGT_W::RSTGDP>(); // Reset Gate Data Pattern
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":reset FSM:got data pattern 0x" << std::hex << rstgdp << std::dec << EndDebugInfo;
			}
			
			if(rstgdp == RESET_GATE_KEY2)
			{
				// got second data pattern write
				unsigned int rstgtn = sema42_rstgt_w.template Get<typename SEMA42_RSTGT_W::RSTGTN>(); // Reset Gate Number
				
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":reset FSM:got second data pattern write" << EndDebugInfo;
				}

				if(rstgtn < MAX_GATES)
				{
					// reset one gate
					ResetGate(rstgtn);
				}
				else
				{
					// reset all gates
					ResetAllGates();
				}
				
				sema42_rstgt_r.template Set<typename SEMA42_RSTGT_R::RSTGMS>(trans_attr.master_id()); // Reset Gate Bus Master
				sema42_rstgt_r.template Set<typename SEMA42_RSTGT_R::RSTGSM>(RSTGSM_IDLE_WAITING_1ST_WRITE);
			}
			else
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":reset FSM:did not get second data pattern write" << EndDebugInfo;
				}
				sema42_rstgt_r.Reset();
			}
			
			break;
		}
	}
	
	return rws;
}

template <typename CONFIG>
ReadWriteStatus SEMA42<CONFIG>::GateFSM(unsigned int gate_num, tlm_trans_attr& trans_attr, const uint16_t& value, const uint16_t& bit_enable)
{
	ReadWriteStatus rws = RWS_OK;
	
	uint8_t write_value = value & bit_enable;
	
	unsigned int gtfsm = sema42_gate[gate_num].template Get<typename SEMA42_GATE::GTFSM>(); // Gate Finite State Machine
	
	if(gtfsm) // gate locked?
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":gate #" << gate_num << " is locked" << EndDebugInfo;
		}
		
		if(gtfsm == (unsigned int)(trans_attr.master_id() + 1)) // locked by writer?
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":gate #" << gate_num << " was locked by Processor #" << trans_attr.master_id() << EndDebugInfo;
			}
			if(SEMA42_GATE::GTFSM::template Get<uint16_t>(write_value) == 0) // unlocking?
			{
				// unlock
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":Processor #" << trans_attr.master_id() << " unlocks gate #" << gate_num << EndDebugInfo;
				}
				rws = sema42_gate[gate_num].SEMA42_GATE::Super::Write(trans_attr, value, bit_enable);
			}
		}
	}
	else
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":gate #" << gate_num << " is not locked" << EndDebugInfo;
		}

		if(SEMA42_GATE::GTFSM::template Get<uint16_t>(write_value) == (unsigned int)(trans_attr.master_id() + 1)) // Logical bus master matches hardware bus master 
		{
			// lock
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":Processor #" << trans_attr.master_id() << " locks gate #" << gate_num << EndDebugInfo;
			}
			rws = sema42_gate[gate_num].SEMA42_GATE::Super::Write(trans_attr, value, bit_enable);
		}
		else
		{
			logger << DebugWarning << "Master #" << trans_attr.master_id() << " is only allowed to write " << (trans_attr.master_id() + 1) << " into " << sema42_gate[gate_num].GetName() << "[" << SEMA42_GATE::GTFSM::GetName() << "]" << EndDebugWarning;
		}
	}
	
	return rws;
}

template <typename CONFIG>
void SEMA42<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool SEMA42<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int SEMA42<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
	
	tlm_trans_attr *trans_attr = payload.template get_extension<tlm_trans_attr>();
	if(!trans_attr) trans_attr = &default_trans_attr;

	switch(cmd)
	{
		case tlm::TLM_WRITE_COMMAND:
			return write_reg_addr_map.DebugWrite(*trans_attr, start_addr, data_ptr, data_length);
		case tlm::TLM_READ_COMMAND:
			return read_reg_addr_map.DebugRead(*trans_attr, start_addr, data_ptr, data_length);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum SEMA42<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
unisim::service::interfaces::Register *SEMA42<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void SEMA42<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void SEMA42<CONFIG>::ProcessEvent(Event *event)
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
			tlm_trans_attr *trans_attr = event->GetAttributes();
			if(!trans_attr) trans_attr = &default_trans_attr;
			
			switch(cmd)
			{
				case tlm::TLM_WRITE_COMMAND:
					rws = write_reg_addr_map.Write(*trans_attr, start_addr, data_ptr, data_length);
					break;
				case tlm::TLM_READ_COMMAND:
					rws = read_reg_addr_map.Read(*trans_attr, start_addr, data_ptr, data_length);
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
void SEMA42<CONFIG>::ProcessEvents()
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
void SEMA42<CONFIG>::Process()
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
void SEMA42<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void SEMA42<CONFIG>::MasterClockPropertiesChangedProcess()
{
	UpdateMasterClock();
}

template <typename CONFIG>
void SEMA42<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

} // end of namespace sema42
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace semaphore
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_SEMA42_SEMA42_TCC__
