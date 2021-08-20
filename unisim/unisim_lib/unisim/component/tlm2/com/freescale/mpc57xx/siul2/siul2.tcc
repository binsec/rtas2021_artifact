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

#ifndef __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_SIUL2_SIUL2_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_SIUL2_SIUL2_TCC__

#include <unisim/component/tlm2/com/freescale/mpc57xx/siul2/siul2.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace freescale {
namespace mpc57xx {
namespace siul2 {

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::NUM_PADS;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::NUM_INTERRUPT_FILTERS;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::PART_NUMBER;

template <typename CONFIG>
const bool SIUL2<CONFIG>::ENGINEERING_DEVICE;

template <typename CONFIG>
const PACKAGE_SETTING_TYPE SIUL2<CONFIG>::PACKAGE_SETTING;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::MAJOR_MASK;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::MINOR_MASK;

template <typename CONFIG>
const MANUFACTURER_TYPE SIUL2<CONFIG>::MANUFACTURER;

template <typename CONFIG>
const unsigned int SIUL2<CONFIG>::FLASH_SIZE;

template <typename CONFIG>
const char SIUL2<CONFIG>::FAMILY_NUMBER;

template <typename CONFIG>
const bool SIUL2<CONFIG>::threaded_model;

template <typename CONFIG>
SIUL2<CONFIG>::SIUL2(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, reset_b("reset_b")
	, pad_in("pad_in", NUM_PADS)
	, pad_out("pad_out", NUM_PADS)
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, siul2_midr1(this)
	, siul2_midr2(this)
	, siul2_disr0(this)
	, siul2_direr0(this)
	, siul2_dirsr0(this)
	, siul2_ireer0(this)
	, siul2_ifeer0(this)
	, siul2_ifer0(this)
	, siul2_ifmcr(this)
	, siul2_scr0(this)
	, siul2_mscr_io(this)
	, siul2_mscr_mux(this)
	, siul2_gpdo(this)
	, siul2_gpdi(this)
	, siul2_pgpdo(this)
	, siul2_pgpdi(this)
	, siul2_mpgpdo(this)
	, reg_addr_map()
	, registers_registry()
	, pad_out_event("pad_out_event", NUM_PADS)
	, schedule()
	, input_buffers()
	, output_buffers()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
{
	unsigned int i;
	
	peripheral_slave_if(*this);
	
	std::stringstream description_sstr;
	description_sstr << "MPC57XX SIUL2:" << std::endl;
	description_sstr << "  - " << NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS << " multiplexed signals configurations" << std::endl;
	description_sstr << "  - " << NUM_PADS << " pads" << std::endl;
	description_sstr << "  - Manufacturer: " << CONFIG::MANUFACTURER << std::endl;
	description_sstr << "  - Family: " << CONFIG::FAMILY_NUMBER << std::endl;
	description_sstr << "  - Part number: 0x" << std::hex << CONFIG::PART_NUMBER << std::dec << std::endl;
	description_sstr << "  - " << (CONFIG::ENGINEERING_DEVICE ? "engineering" : "product") << " device" << std::endl;
	description_sstr << "  - Package: " << CONFIG::PACKAGE_SETTING << std::endl;
	description_sstr << "  - Major mask: " << CONFIG::MAJOR_MASK << std::endl;
	description_sstr << "  - Minor mask: " << CONFIG::MINOR_MASK << std::endl;
	description_sstr << "  - Flash size: " << (CONFIG::FLASH_SIZE / 1024) << "K" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 16" << std::endl;
	this->SetDescription(description_sstr.str().c_str());

	SC_HAS_PROCESS(SIUL2);

	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	
	reg_addr_map.MapRegister(siul2_midr1.ADDRESS_OFFSET, &siul2_midr1);
	reg_addr_map.MapRegister(siul2_midr2.ADDRESS_OFFSET, &siul2_midr2);
	reg_addr_map.MapRegister(siul2_disr0.ADDRESS_OFFSET, &siul2_disr0);
	reg_addr_map.MapRegister(siul2_direr0.ADDRESS_OFFSET, &siul2_direr0);
	reg_addr_map.MapRegister(siul2_dirsr0.ADDRESS_OFFSET, &siul2_dirsr0);
	reg_addr_map.MapRegister(siul2_ireer0.ADDRESS_OFFSET, &siul2_ireer0);
	reg_addr_map.MapRegister(siul2_ifeer0.ADDRESS_OFFSET, &siul2_ifeer0);
	reg_addr_map.MapRegister(siul2_ifer0.ADDRESS_OFFSET, &siul2_ifer0);
	
	reg_addr_map.MapRegisterFile(SIUL2_IFMCR::ADDRESS_OFFSET, &siul2_ifmcr);
	reg_addr_map.MapRegister(siul2_scr0.ADDRESS_OFFSET, &siul2_scr0);
	reg_addr_map.MapRegisterFile(SIUL2_MSCR_IO::ADDRESS_OFFSET, &siul2_mscr_io);
	reg_addr_map.MapRegisterFile(SIUL2_MSCR_MUX::ADDRESS_OFFSET, &siul2_mscr_mux);
	reg_addr_map.MapRegisterFile(SIUL2_GPDO::ADDRESS_OFFSET, &siul2_gpdo);
	reg_addr_map.MapRegisterFile(SIUL2_GPDI::ADDRESS_OFFSET, &siul2_gpdi);
	reg_addr_map.MapRegisterFile(SIUL2_PGPDO::ADDRESS_OFFSET, &siul2_pgpdo);
	reg_addr_map.MapRegisterFile(SIUL2_PGPDI::ADDRESS_OFFSET, &siul2_pgpdi);
	reg_addr_map.MapRegisterFile(SIUL2_MPGPDO::ADDRESS_OFFSET, &siul2_mpgpdo);

	registers_registry.AddRegisterInterface(siul2_midr1.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(siul2_midr2.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(siul2_disr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(siul2_direr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(siul2_dirsr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(siul2_ireer0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(siul2_ifeer0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(siul2_ifer0.CreateRegisterInterface());
	for(i = 0; i < NUM_INTERRUPT_FILTERS; i++)
	{
		registers_registry.AddRegisterInterface(siul2_ifmcr[i].CreateRegisterInterface());
	}
	registers_registry.AddRegisterInterface(siul2_scr0.CreateRegisterInterface());
	for(i = 0; i < NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS; i++)
	{
		registers_registry.AddRegisterInterface(siul2_mscr_io[i].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(siul2_mscr_mux[i].CreateRegisterInterface());
	}
	for(i = 0; i < NUM_PADS; i++)
	{
		registers_registry.AddRegisterInterface(siul2_gpdo[i].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(siul2_gpdi[i].CreateRegisterInterface());
	}
	for(i = 0; i < (MAX_PADS / 16); i++)
	{
		registers_registry.AddRegisterInterface(siul2_pgpdo[i].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(siul2_pgpdi[i].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(siul2_mpgpdo[i].CreateRegisterInterface());
	}
	
	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	// Spawn a PAD_In_Process for each pad
	for(i = 0; i < NUM_PADS; i++)
	{
		sc_core::sc_spawn_options pad_in_process_spawn_options;
		pad_in_process_spawn_options.spawn_method();
		pad_in_process_spawn_options.set_sensitivity(&pad_in[i].value_changed());
		
		std::stringstream pad_process_name_sstr;
		pad_process_name_sstr << "PAD_In_Process_" << i;
		sc_core::sc_spawn(sc_bind(&SIUL2<CONFIG>::PAD_In_Process, this, i), pad_process_name_sstr.str().c_str(), &pad_in_process_spawn_options);
	}
	
	// Spawn a PAD_Out_Process for each pad
	for(i = 0; i < NUM_PADS; i++)
	{
		sc_core::sc_spawn_options pad_out_process_spawn_options;
		pad_out_process_spawn_options.spawn_method();
		pad_out_process_spawn_options.set_sensitivity(&pad_out_event[i]);
		
		std::stringstream pad_process_name_sstr;
		pad_process_name_sstr << "PAD_Out_Process_" << i;
		sc_core::sc_spawn(sc_bind(&SIUL2<CONFIG>::PAD_Out_Process, this, i), pad_process_name_sstr.str().c_str(), &pad_out_process_spawn_options);
	}
}

template <typename CONFIG>
SIUL2<CONFIG>::~SIUL2()
{
}

template <typename CONFIG>
void SIUL2<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;

	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&SIUL2<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void SIUL2<CONFIG>::Reset()
{
	unsigned int i;
	
	siul2_midr1.Reset();
	siul2_midr2.Reset();
	siul2_disr0.Reset();
	siul2_direr0.Reset();
	siul2_dirsr0.Reset();
	siul2_ireer0.Reset();
	siul2_ifeer0.Reset();
	siul2_ifer0.Reset();
	for(i = 0; i < NUM_INTERRUPT_FILTERS; i++)
	{
		siul2_ifmcr[i].Reset();
	}
	siul2_scr0.Reset();
	for(i = 0; i < NUM_MULTIPLEXED_SIGNALS_CONFIGURATIONS; i++)
	{
		siul2_mscr_io[i].Reset();
		siul2_mscr_mux[i].Reset();
	}
	for(i = 0; i < NUM_PADS; i++)
	{
		siul2_gpdo[i].Reset();
		siul2_gpdi[i].Reset();
	}
	for(i = 0; i < (NUM_PADS / 16); i++)
	{
		siul2_pgpdo[i].Reset();
		siul2_pgpdi[i].Reset();
		siul2_mpgpdo[i].Reset();
	}
}

template <typename CONFIG>
void SIUL2<CONFIG>::UpdateOutputBuffer(unsigned int pad_num)
{
	bool output_buffer_value = siul2_gpdo[pad_num].template Get<typename SIUL2_GPDO::PDO>();
	output_buffers[pad_num] = output_buffer_value;
	
	UpdatePadOut(pad_num);
}

template <typename CONFIG>
void SIUL2<CONFIG>::UpdateInputBuffer(unsigned int pad_num)
{
	input_buffers[pad_num] = pad_in[pad_num]->read();
	UpdateGPI(pad_num);
}

template <typename CONFIG>
void SIUL2<CONFIG>::UpdateGPI(unsigned int pad_num)
{
	bool input_buffer_value = input_buffers[pad_num];
	bool inv = siul2_mscr_mux[pad_num].template Get<typename SIUL2_MSCR_MUX::INV>();
	bool gpi_value = inv ? !input_buffer_value : input_buffer_value;
	siul2_gpdi[pad_num].template Set<typename SIUL2_GPDI::PDI>(gpi_value);
	unsigned int pgpdi_reg_num = pad_num / 16;
	unsigned int pgpdi_bit_ofs = 15 - (pad_num % 16);
	siul2_pgpdi[pgpdi_reg_num].Set(pgpdi_bit_ofs, gpi_value);
	if(unlikely(verbose))
	{
		logger << DebugInfo << siul2_gpdi[pad_num].GetName() << " <- 0x" << std::hex << siul2_gpdi[pad_num] << std::dec << EndDebugInfo;
		logger << DebugInfo << siul2_pgpdi[pgpdi_reg_num].GetName() << " <- 0x" << std::hex << siul2_pgpdi[pgpdi_reg_num] << std::dec << EndDebugInfo;
	}
}

template <typename CONFIG>
void SIUL2<CONFIG>::UpdatePadOut(unsigned int pad_num)
{
	pad_out_event[pad_num].notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void SIUL2<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void SIUL2<CONFIG>::MasterClockPropertiesChangedProcess()
{
	// Master Clock properties have changed
	UpdateMasterClock();
}

template <typename CONFIG>
void SIUL2<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	Event *event = schedule.AllocEvent();
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
bool SIUL2<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int SIUL2<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
{
	tlm::tlm_command cmd = payload.get_command();
	unsigned int data_length = payload.get_data_length();
	unsigned char *data_ptr = payload.get_data_ptr();
	sc_dt::uint64 start_addr = payload.get_address();
	
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
tlm::tlm_sync_enum SIUL2<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
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
unisim::service::interfaces::Register *SIUL2<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void SIUL2<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void SIUL2<CONFIG>::ProcessEvent(Event *event)
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
		sc_dt::uint64 end_addr = start_addr + ((streaming_width > data_length) ? data_length : streaming_width) - 1;
		
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
		else if((start_addr & -4) != (end_addr & -4))
		{
			logger << DebugWarning << "access crosses 32-bit boundary" << EndDebugWarning;
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
void SIUL2<CONFIG>::ProcessEvents()
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
void SIUL2<CONFIG>::Process()
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
void SIUL2<CONFIG>::PAD_In_Process(unsigned int pad_num)
{
	bool ibe = siul2_mscr_io[pad_num].template Get<typename SIUL2_MSCR_IO::IBE>();
	
	if(ibe)
	{
		UpdateInputBuffer(pad_num);
	}
}

template <typename CONFIG>
void SIUL2<CONFIG>::PAD_Out_Process(unsigned int pad_num)
{
	OUTPUT_DRIVE_CONTROL odc = OUTPUT_DRIVE_CONTROL(siul2_mscr_io[pad_num].template Get<typename SIUL2_MSCR_IO::ODC>());
	if(odc != ODC_OUTPUT_BUFFER_DISABLED)
	{
		bool output_buffer_value = output_buffers[pad_num];
		bool inv = siul2_mscr_io[pad_num].template Get<typename SIUL2_MSCR_IO::INV>();
		bool pad_value = inv ? !output_buffer_value : output_buffer_value;
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << pad_out[pad_num].name() << " <- '" << pad_value << "'" << EndDebugInfo;
		}
		pad_out[pad_num]->write(pad_value);
	}
}

} // end of namespace siul2
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_SIUL2_SIUL2_TCC__
