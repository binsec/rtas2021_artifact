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

#ifndef __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_TCC__
#define __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_TCC__

#include <unisim/component/tlm2/interrupt/freescale/mpc57xx/intc/intc.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/kernel/tlm2/trans_attr.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace interrupt {
namespace freescale {
namespace mpc57xx {
namespace intc {

template <typename CONFIG>
const unsigned int INTC<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::NUM_PROCESSORS;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::NUM_SW_IRQS;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::NUM_HW_IRQS;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::NUM_IRQS;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::NUM_PRIORITY_LEVELS;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::MAX_PRIORITY_LEVEL;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::VOFFSET_WIDTH;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::HW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES;

template <typename CONFIG>
const unsigned int INTC<CONFIG>::SW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES;

template <typename CONFIG>
const bool INTC<CONFIG>::threaded_model;

template <typename CONFIG>
INTC<CONFIG>::INTC(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, reset_b("reset_b")
	, hw_irq()
	, p_iack()
	, p_irq_b()
	, p_avec_b()
	, p_voffset()
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, intc_bcr(this)
	, intc_mprot(this)
	, intc_cpr(this)
	, intc_iack(this)
	, intc_eoir(this)
	, intc_sscir(this)
	, intc_psr_sw(this)
	, intc_psr_hw(this)
	, lifo()
	, irqs()
	, priority_tree()
	, reg_addr_map()
	, registers_registry()
	, schedule()
	, last_irq_b_time_stamp()
	, irq_select_event()
	, gen_irq_b_event()
	, selected_irq_num()
	, irq_b()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, cycle_time(10.0, sc_core::SC_NS)
	, hw_irq_source_to_processor_irq_latency(cycle_time * HW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES)
	, sw_irq_source_to_processor_irq_latency(cycle_time * SW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES)
{
	peripheral_slave_if(*this);
	
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Interrupt Controller (INTC):" << std::endl;
	description_sstr << "  - Preemptive prioritized interrupt request to processor" << std::endl;
	description_sstr << "  - " << HW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES << " INTC clock cycles from receipt of interrupt request from peripheral to interrupt request to processor" << std::endl;
	description_sstr << "  - " << SW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES << " INTC clock cycles from receipt of interrupt request from software to interrupt request to processor" << std::endl;
	description_sstr << "  - " << NUM_PRIORITY_LEVELS << " priority level(s)" << std::endl;
	description_sstr << "  - " << NUM_PROCESSORS << " processor(s)" << std::endl;
	description_sstr << "  - " << NUM_SW_IRQS << " software IRQ source(s)" << std::endl;
	description_sstr << "  - " << NUM_HW_IRQS << " hardware IRQ source(s)" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - " << VOFFSET_WIDTH << "-bit vector offset" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 23" << std::endl;
	this->SetDescription(description_sstr.str().c_str());
	
	SC_HAS_PROCESS(INTC);

	unsigned int sw_irq_num;
	unsigned int hw_irq_num;
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		irq_b[prc_num] = true;
	}

	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
	{
		std::stringstream hw_irq_name_sstr;
		hw_irq_name_sstr << "hw_irq_" << hw_irq_num;
		hw_irq[hw_irq_num] = new sc_core::sc_in<bool>(hw_irq_name_sstr.str().c_str()); 
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_iack_name_sstr;
		p_iack_name_sstr << "p_iack_" << prc_num;
		p_iack[prc_num] = new sc_core::sc_in<bool>(p_iack_name_sstr.str().c_str());
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_irq_b_name_sstr;
		p_irq_b_name_sstr << "p_irq_b_" << prc_num;
		p_irq_b[prc_num] = new sc_core::sc_out<bool>(p_irq_b_name_sstr.str().c_str());
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_avec_b_name_sstr;
		p_avec_b_name_sstr << "p_avec_b_" << prc_num;
		p_avec_b[prc_num] = new sc_core::sc_out<bool>(p_avec_b_name_sstr.str().c_str());
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream p_voffset_name_sstr;
		p_voffset_name_sstr << "p_voffset_" << prc_num;
		p_voffset[prc_num] = new sc_core::sc_out<sc_dt::sc_uint<VOFFSET_WIDTH> >(p_voffset_name_sstr.str().c_str());
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream irq_select_event_name_sstr;
		irq_select_event_name_sstr << "irq_select_event_" << prc_num;
		irq_select_event[prc_num] = new sc_core::sc_event(irq_select_event_name_sstr.str().c_str());
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream gen_irq_b_event_name_sstr;
		gen_irq_b_event_name_sstr << "gen_irq_b_event_" << prc_num;
		gen_irq_b_event[prc_num] = new sc_core::sc_event(gen_irq_b_event_name_sstr.str().c_str());
	}

	// Map INTC registers regarding there address offsets
	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegister(intc_bcr.ADDRESS_OFFSET, &intc_bcr);
	reg_addr_map.MapRegister(intc_mprot.ADDRESS_OFFSET, &intc_mprot);
	reg_addr_map.MapRegisterFile(INTC_CPR::ADDRESS_OFFSET, &intc_cpr);
	reg_addr_map.MapRegisterFile(INTC_IACK::ADDRESS_OFFSET, &intc_iack);
	reg_addr_map.MapRegisterFile(INTC_EOIR::ADDRESS_OFFSET, &intc_eoir);
	reg_addr_map.MapRegisterFile(INTC_SSCIR::ADDRESS_OFFSET, &intc_sscir);
	reg_addr_map.MapRegisterFile(INTC_PSR<SW_IRQ>::ADDRESS_OFFSET, &intc_psr_sw);
	reg_addr_map.MapRegisterFile(INTC_PSR<HW_IRQ>::ADDRESS_OFFSET, &intc_psr_hw);

	registers_registry.AddRegisterInterface(intc_bcr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(intc_mprot.CreateRegisterInterface());

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		registers_registry.AddRegisterInterface(intc_cpr[prc_num].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(intc_iack[prc_num].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(intc_eoir[prc_num].CreateRegisterInterface());
	}
	
	for(sw_irq_num = 0; sw_irq_num < NUM_SW_IRQS; sw_irq_num++)
	{
		registers_registry.AddRegisterInterface(intc_sscir[sw_irq_num].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(intc_psr_sw[sw_irq_num].CreateRegisterInterface());
	}
	
	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
	{
		registers_registry.AddRegisterInterface(intc_psr_hw[hw_irq_num].CreateRegisterInterface());
	}

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	SC_METHOD(RESET_B_Process);
	sensitive << reset_b.pos();
	
	// Spawn an HW_IRQ_Process for each HW IRQ
	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
	{
		sc_core::sc_spawn_options hw_irq_process_spawn_options;
		hw_irq_process_spawn_options.spawn_method();
		//hw_irq_process_spawn_options.dont_initialize();
		hw_irq_process_spawn_options.set_sensitivity(hw_irq[hw_irq_num]);
		
		std::stringstream hw_irq_process_name_sstr;
		hw_irq_process_name_sstr << "HW_IRQ_Process_" << hw_irq_num;
		sc_core::sc_spawn(sc_bind(&INTC<CONFIG>::HW_IRQ_Process, this, hw_irq_num), hw_irq_process_name_sstr.str().c_str(), &hw_irq_process_spawn_options);
	}
	
	// Spawn an IACK_Process for each processor
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		sc_core::sc_spawn_options iack_process_spawn_options;
		iack_process_spawn_options.spawn_method();
		//iack_process_spawn_options.dont_initialize();
		iack_process_spawn_options.set_sensitivity(&p_iack[prc_num]->neg());
		
		std::stringstream iack_process_name_sstr;
		iack_process_name_sstr << "IACK_Process_" << prc_num;
		sc_core::sc_spawn(sc_bind(&INTC<CONFIG>::IACK_Process, this, prc_num), iack_process_name_sstr.str().c_str(), &iack_process_spawn_options);
	}

	// Spawn an IRQ_Select_Process for each processor
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		sc_core::sc_spawn_options irq_select_process_spawn_options;
		irq_select_process_spawn_options.spawn_method();
		//irq_select_process_spawn_options.dont_initialize();
		irq_select_process_spawn_options.set_sensitivity(irq_select_event[prc_num]);
		
		std::stringstream irq_select_process_name_sstr;
		irq_select_process_name_sstr << "IRQ_Select_Process_" << prc_num;
		sc_core::sc_spawn(sc_bind(&INTC<CONFIG>::IRQ_Select_Process, this, prc_num), irq_select_process_name_sstr.str().c_str(), &irq_select_process_spawn_options);
	}

	// Spawn an IRQ_B_Process for each processor
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		sc_core::sc_spawn_options irq_b_process_spawn_options;
		irq_b_process_spawn_options.spawn_method();
		//irq_b_process_spawn_options.dont_initialize();
		irq_b_process_spawn_options.set_sensitivity(gen_irq_b_event[prc_num]);
		
		std::stringstream irq_b_process_name_sstr;
		irq_b_process_name_sstr << "IRQ_B_Process_" << prc_num;
		sc_core::sc_spawn(sc_bind(&INTC<CONFIG>::IRQ_B_Process, this, prc_num), irq_b_process_name_sstr.str().c_str(), &irq_b_process_spawn_options);
	}
}

template <typename CONFIG>
INTC<CONFIG>::~INTC()
{
	unsigned int hw_irq_num;
	unsigned int prc_num;
	
	for(hw_irq_num = 0; hw_irq_num < NUM_HW_IRQS; hw_irq_num++)
	{
		delete hw_irq[hw_irq_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_iack[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_irq_b[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_avec_b[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete p_voffset[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete irq_select_event[prc_num];
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete gen_irq_b_event[prc_num];
	}
}

template <typename CONFIG>
void INTC<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;
	
	// Spawn ClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options clock_properties_changed_process_spawn_options;
	
	clock_properties_changed_process_spawn_options.spawn_method();
	clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&INTC<CONFIG>::ClockPropertiesChangedProcess, this), "ClockPropertiesChangedProcess", &clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
bool INTC<CONFIG>::CheckMasterProtection(unsigned int prc_num) const
{
	if(prc_num == 0) return true; // The processor with master ID zero will be allowed write access

	if(!intc_mprot.template Get<typename INTC_MPROT::MPROT>()) return true; // all processors have write access to this register
	
	return intc_mprot.template Get<typename INTC_MPROT::ID>() == prc_num; // the processor with master ID (0-3) = INTC_MPROT[ID] has write access, otherwise, a termination error is assert
}

template <typename CONFIG>
bool INTC<CONFIG>::CheckWriteProtection_SSCIR_CLR(unsigned int prc_num, unsigned int sw_irq_num) const
{
	if(!intc_mprot.template Get<typename INTC_MPROT::MPROT>()) return true; // all processors can write 1 in SSCIR[CLR]
	
	unsigned int seln = intc_psr_sw[sw_irq_num].template Get<typename INTC_PSR<SW_IRQ>::PRC_SELN>();
	
	if((seln << prc_num) & 8)
	{
		// processor is selected for that software IRQ
		return true;
	}
	
	return false;
}

template <typename CONFIG>
bool INTC<CONFIG>::IsHardwareVectorEnabled(unsigned int prc_num) const
{
	// INTC_BCR[HVENn] indicates whether hardware vector mode (aka. vectored mode) is enable for Processor #n
	switch(prc_num)
	{
		case 0: return intc_bcr.template Get<typename INTC_BCR::HVEN0>() != 0;
		case 1: return intc_bcr.template Get<typename INTC_BCR::HVEN1>() != 0;
		case 2: return intc_bcr.template Get<typename INTC_BCR::HVEN2>() != 0;
		case 3: return intc_bcr.template Get<typename INTC_BCR::HVEN3>() != 0;
	}
	return false;
}

template <typename CONFIG>
void INTC<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
	sc_core::sc_event completion_event;
	sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
	notify_time_stamp += t;
	Event *event = schedule.AllocEvent();
	unisim::kernel::tlm2::tlm_trans_attr *trans_attr = payload.template get_extension<unisim::kernel::tlm2::tlm_trans_attr>();
	event->SetProcessorNumber(trans_attr ? trans_attr->master_id() : 0);
	event->SetPayload(&payload);
	event->SetTimeStamp(notify_time_stamp);
	event->SetCompletionEvent(&completion_event);
	schedule.Notify(event);
	sc_core::wait(completion_event);
	t = sc_core::SC_ZERO_TIME;
}

template <typename CONFIG>
bool INTC<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
	
	unisim::kernel::tlm2::tlm_trans_attr *trans_attr = payload.template get_extension<unisim::kernel::tlm2::tlm_trans_attr>();
	ProcessorNumber prc_num = trans_attr ? trans_attr->master_id() : 0;
	
	switch(cmd)
	{
		case tlm::TLM_WRITE_COMMAND:
			return reg_addr_map.DebugWrite(prc_num, start_addr, data_ptr, data_length);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map.DebugRead(prc_num, start_addr, data_ptr, data_length);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum INTC<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	switch(phase)
	{
		case tlm::BEGIN_REQ:
			{
				sc_core::sc_time notify_time_stamp(sc_core::sc_time_stamp());
				notify_time_stamp += t;
				Event *event = schedule.AllocEvent();
				unisim::kernel::tlm2::tlm_trans_attr *trans_attr = payload.template get_extension<unisim::kernel::tlm2::tlm_trans_attr>();
				event->SetProcessorNumber(trans_attr ? trans_attr->master_id() : 0);
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
unisim::service::interfaces::Register *INTC<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void INTC<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void INTC<CONFIG>::ProcessEvent(Event *event)
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
			ProcessorNumber prc_num = event->GetProcessorNumber();
			ReadWriteStatus rws = RWS_OK;
			
			switch(cmd)
			{
				case tlm::TLM_WRITE_COMMAND:
					rws = reg_addr_map.Write(prc_num, start_addr, data_ptr, data_length);
					break;
				case tlm::TLM_READ_COMMAND:
					rws = reg_addr_map.Read(prc_num, start_addr, data_ptr, data_length);
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
void INTC<CONFIG>::ProcessEvents()
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
void INTC<CONFIG>::Process()
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
void INTC<CONFIG>::Reset()
{
	unsigned int prc_num;
	unsigned int sw_irq_num;
	unsigned int hw_irq_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		priority_tree[prc_num].clear();
	}
	schedule.Clear();

	intc_bcr.Initialize(0x0);
	intc_mprot.Initialize(0x0);
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		intc_cpr[prc_num].Initialize(0x0);
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		intc_iack[prc_num].Initialize(0x0);
	}
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		intc_eoir[prc_num].Initialize(0x0);
	}
	
	for(sw_irq_num = 0; sw_irq_num < NUM_SW_IRQS; sw_irq_num++)
	{
		intc_sscir[sw_irq_num].Initialize(0x0);
	}
	for(sw_irq_num = 0; sw_irq_num < NUM_SW_IRQS; sw_irq_num++)
	{
		intc_psr_sw[sw_irq_num].Initialize(0x0);
	}
	for(hw_irq_num = 0; hw_irq_num < NUM_SW_IRQS; hw_irq_num++)
	{
		intc_psr_hw[hw_irq_num].Initialize(0x0);
	}
	for(sw_irq_num = 0; sw_irq_num < NUM_SW_IRQS; sw_irq_num++)
	{
		intc_psr_sw[sw_irq_num].InitializePriorityTree();
	}
	for(hw_irq_num = 0; hw_irq_num < NUM_SW_IRQS; hw_irq_num++)
	{
		intc_psr_hw[hw_irq_num].InitializePriorityTree();
	}
	
	if(!m_clk_prop_proxy.IsClockCompatible())
	{
		logger << DebugError << "clock port is not bound to a unisim::kernel::tlm2::Clock" << EndDebugError;
		unisim::kernel::Object::Stop(-1);
		return;
	}
	
	UpdateSpeed();
}

template <typename CONFIG>
void INTC<CONFIG>::UpdateIRQSelect(unsigned int prc_num, const sc_core::sc_time& delay)
{
	// Schedule an update of IRQ input selection
	irq_select_event[prc_num]->notify(delay);
}

template <typename CONFIG>
void INTC<CONFIG>::SetIRQOutput(unsigned int prc_num, bool value, IRQ_Type irq_type)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": IRQ output for Processor #" << prc_num << " will be " << (value ? "asserted" : "deasserted") << EndDebugInfo;
	}
	// Keep value of IRQ output to send to processor (used by IRQ_B_Process) as a delay is applied
	irq_b[prc_num] = !value;
	
	// When asserting IRQ, account for latency depending on interrupt source type (hardware or software)
	if(value)
	{
		sc_core::sc_time notify_delay(sc_core::sc_time_stamp());
		
		switch(irq_type)
		{
			case ANY_IRQ:
				// should never happen
				logger << DebugError << "Internal error in INTC<>::SetIRQOutput(...)" << EndDebugError;
				unisim::kernel::Object::Stop(-1);
				return;
			case SW_IRQ:
				notify_delay += sw_irq_source_to_processor_irq_latency;
				break;
			case HW_IRQ:
				notify_delay += hw_irq_source_to_processor_irq_latency;
				break;
		}
		
		// Align to clock cycle time
		unisim::kernel::tlm2::AlignToClock(notify_delay, cycle_time);
		
		notify_delay -= sc_core::sc_time_stamp();
		
		// Schedule an update of IRQ output (trigger IRQ_B_Process after a delay)
		gen_irq_b_event[prc_num]->notify(notify_delay);
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": assertion will occur after " << notify_delay << EndDebugInfo;
		}
	}
	else
	{
		// Schedule an immediate update of IRQ output (trigger IRQ_B_Process at current simulation time)
		gen_irq_b_event[prc_num]->notify(sc_core::SC_ZERO_TIME);
	}
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::SW_IRQ2IRQ(unsigned int sw_irq_num) const
{
	// Software IRQ have lowest IRQ vector
	return sw_irq_num;
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::HW_IRQ2IRQ(unsigned int hw_irq_num) const
{
	// Hardware IRQ have highest IRQ vector
	return NUM_SW_IRQS + hw_irq_num;
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::IRQ2SW_IRQ(unsigned int irq_num) const
{
	// Software IRQ have lowest IRQ vector
	return irq_num;
}

template <typename CONFIG>
unsigned int INTC<CONFIG>::IRQ2HW_IRQ(unsigned int irq_num) const
{
	// Hardware IRQ have highest IRQ vector
	return irq_num - NUM_SW_IRQS;
}

template <typename CONFIG>
bool INTC<CONFIG>::IsSW_IRQ(unsigned int irq_num) const
{
	// Software IRQ have lowest IRQ vector
	return irq_num < NUM_SW_IRQS;
}

template <typename CONFIG>
bool INTC<CONFIG>::IsHW_IRQ(unsigned int irq_num) const
{
	// Hardware IRQ have highest IRQ vector
	return irq_num >= NUM_SW_IRQS;
}

template <typename CONFIG>
void INTC<CONFIG>::HW_IRQ_Process(unsigned int hw_irq_num)
{
	// Update latched IRQ input status
	unsigned int irq_num = HW_IRQ2IRQ(hw_irq_num);
	SetIRQInputStatus(irq_num, *hw_irq[hw_irq_num]);
}

template <typename CONFIG>
void INTC<CONFIG>::IACK_Process(unsigned int prc_num)
{
	if(IsHardwareVectorEnabled(prc_num))
	{
		// Hardware vector mode (aka. vectored mode)
		
		// Detect IRQ acknowledge from processor
		if(p_iack[prc_num]->negedge())
		{
			// Acknowledge IRQ internally
			IRQAcknowledge(prc_num);
		}
	}
}

template <typename CONFIG>
void INTC<CONFIG>::IRQ_Select_Process(unsigned int prc_num)
{
	// Select an IRQ input based on priority and processor selection
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Looking for an IRQ to select for Processor #" << prc_num << EndDebugInfo;
		DumpPriorityTree(prc_num);
	}
	
	if(irq_b[prc_num])
	{
		if(SelectIRQInput(prc_num, selected_irq_num[prc_num]))
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": Selecting IRQ#" << selected_irq_num[prc_num] << " for Processor #" << prc_num << EndDebugInfo;
			}
			
			// Set IRQ output and provide vector of IRQ (in Process IRQ_B_Process)
			if(IsSW_IRQ(selected_irq_num[prc_num]))
			{
				SetIRQOutput(prc_num, true, SW_IRQ);
			}
			else
			{
				SetIRQOutput(prc_num, true, HW_IRQ);
			}
		}
		else
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": No IRQ selected for Processor #" << prc_num << EndDebugInfo;
			}
		}
	}
	else
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": Can't select yet an IRQ for Processor #" << prc_num << " because IRQ output is already asserted" << EndDebugInfo;
		}
	}
}

template <typename CONFIG>
void INTC<CONFIG>::IRQ_B_Process(unsigned int prc_num)
{
	if(!irq_b[prc_num])
	{
		// IRQ output must be asserted
		bool cur_irq_b_value = *p_irq_b[prc_num];
	
		if(cur_irq_b_value)
		{
			// IRQ output is currently negated, but IRQ output shall negate for at least one clock cycle
			sc_core::sc_time time_since_last_irq_b(sc_core::sc_time_stamp());
			time_since_last_irq_b -= last_irq_b_time_stamp[prc_num];
			if(time_since_last_irq_b >= cycle_time)
			{
				// Assert IRQ
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":" << p_irq_b[prc_num]->name() << " <- " << !true << EndDebugInfo;
				}
				*p_irq_b[prc_num] = !true;
				
				// When the interrupt request to the processor asserts, the INTVEC is updated,
				// whether the INTC is in software or hardware vector mode.
				intc_iack[prc_num].template Set<typename INTC_IACK::INTVEC>(selected_irq_num[prc_num]);

				if(IsHardwareVectorEnabled(prc_num))
				{
					// Tell CPU that we're operating in hardware vector mode (aka. vectored mode)
					if(unlikely(verbose))
					{
						logger << DebugInfo << sc_core::sc_time_stamp() << ":" << p_avec_b[prc_num]->name() << " <- " << !false << EndDebugInfo;
					}
					*p_avec_b[prc_num] = !false;
					
					// The vector value matches the value of the INTVEC field in the INTC_IACKRn,
					// depending on which processor was assigned to handle a given interrupt source.
					*p_voffset[prc_num] = (selected_irq_num[prc_num] << 2); // Note: each handlers have 16 bytes available.
					                                                        // Guess: INTC shifts unique vector by 2 to provide p_voffset,
					                                                        // and processor shift voffset by 2 before oring with IVPR.
				}
				else
				{
					// Tell CPU that we're operating in software vector mode (aka. autovectored mode)
					if(unlikely(verbose))
					{
						logger << DebugInfo << sc_core::sc_time_stamp() << ":" << p_avec_b[prc_num]->name() << " <- " << !true << EndDebugInfo;
					}
					*p_avec_b[prc_num] = !true;
				}
			}
			else
			{
				sc_core::sc_time notify_delay(last_irq_b_time_stamp[prc_num]);
				notify_delay += cycle_time;
				notify_delay -= sc_core::sc_time_stamp();
				gen_irq_b_event[prc_num]->notify(notify_delay);
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ": IRQ output is currently negated, but IRQ output shall negate for at least one clock cycle; assertion will occur after " << notify_delay << EndDebugInfo;
				}
			}
		}
	}
	else
	{
		// negate IRQ output
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":" << p_irq_b[prc_num]->name() << " <- " << !false << EndDebugInfo;
		}
		*p_irq_b[prc_num] = !false;
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":" << p_avec_b[prc_num]->name() << " <- " << !false << EndDebugInfo;
		}
		*p_avec_b[prc_num] = !false;
		
		// keep time stamp when IRQ output was negated, because IRQ output shall negate for at least one clock cycle
		last_irq_b_time_stamp[prc_num] = sc_core::sc_time_stamp();
		
		// after negating IRQ output one cycle, we can select another IRQ input
		UpdateIRQSelect(prc_num, cycle_time);
	}
}

template <typename CONFIG>
void INTC<CONFIG>::UpdateSpeed()
{
	cycle_time = m_clk_prop_proxy.GetClockPeriod();
	hw_irq_source_to_processor_irq_latency = cycle_time;
	hw_irq_source_to_processor_irq_latency *= HW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES;
	sw_irq_source_to_processor_irq_latency = cycle_time;
	sw_irq_source_to_processor_irq_latency *= SW_IRQ_SOURCE_TO_PROCESSOR_IRQ_LATENCY_CYCLES;
}

template <typename CONFIG>
void INTC<CONFIG>::ClockPropertiesChangedProcess()
{
	// Clock properties have changed
	UpdateSpeed();
}

template <typename CONFIG>
void INTC<CONFIG>::SetIRQInputStatus(unsigned int irq_num, bool value)
{
	// Latch value of IRQ input status
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": IRQ#" << irq_num << " = " << value << EndDebugInfo;
	}
	
	uint64_t& s = irqs[irq_num / 64];
	uint64_t m = uint64_t(1) << (irq_num % 64);
	
	if(value)
	{
		s = s | m;
	}
	else
	{
		s = s & ~m;
	}
	
	// IRQ status change may affect IRQ input selection for each processor
	unsigned int prc_num;
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		UpdateIRQSelect(prc_num);
	}
}

template <typename CONFIG>
bool INTC<CONFIG>::GetIRQInputStatus(unsigned int irq_num) const
{
	// Get latch value of IRQ input status
	uint64_t s = irqs[irq_num / 64];
	uint64_t m = uint64_t(1) << (irq_num % 64);
	
	return (s & m) != 0;
}

template <typename CONFIG>
void INTC<CONFIG>::EnableIRQInput(unsigned int prc_num, unsigned int irq_num, unsigned int new_priority)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Enabling IRQ#" << irq_num << " with priority level " << new_priority << EndDebugInfo;
	}

	// Insert, in priority tree, the IRQ input being enabled regarding its priority 
	IRQPriority new_irq_priority(irq_num, new_priority);

	priority_tree[prc_num].insert(new_irq_priority);
	
	// Priority tree modifications may affect IRQ input selection
	UpdateIRQSelect(prc_num);
}

template <typename CONFIG>
void INTC<CONFIG>::DisableIRQInput(unsigned int prc_num, unsigned int irq_num, unsigned int old_priority)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Disabling IRQ#" << irq_num << " that got priority level " << old_priority << EndDebugInfo;
	}

	// Remove, from priority tree, the IRQ input being disabled 
	IRQPriority old_irq_priority(irq_num, old_priority);
	typename std::set<IRQPriority>::iterator it = priority_tree[prc_num].find(old_irq_priority);
	
	if(it != priority_tree[prc_num].end())
	{
		priority_tree[prc_num].erase(it);
	}
	
	// Priority tree modifications may affect IRQ input selection
	UpdateIRQSelect(prc_num);
}

template <typename CONFIG>
void INTC<CONFIG>::ChangeIRQInputPriority(unsigned int prc_num, unsigned int irq_num, unsigned int old_priority, unsigned int new_priority)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Changing IRQ#" << irq_num << " priority level from " << old_priority << " to " << new_priority << EndDebugInfo;
	}

	// Update priority tree because IRQ input priority has changed
	
	// Remove it first from priority tree
	IRQPriority old_irq_priority(irq_num, old_priority);
	typename std::set<IRQPriority>::iterator it = priority_tree[prc_num].find(old_irq_priority);
	
	if(it != priority_tree[prc_num].end())
	{
		priority_tree[prc_num].erase(it);
	}
	
	// Then, reinsert it with the new priority
	IRQPriority new_irq_priority(irq_num, new_priority);

	priority_tree[prc_num].insert(new_irq_priority);
	
	// Priority tree modifications may affect IRQ input selection
	UpdateIRQSelect(prc_num);
}

template <typename CONFIG>
bool INTC<CONFIG>::SelectIRQInput(unsigned int prc_num, unsigned int& selected_irq_num) const
{
	// Find IRQ input with higher priority and with priority (aka. INTC_PSR[PRI] mirrored in priority tree) greater than current priority (aka. INTC_CPRn[PRI])
	
	// Get current priority from INTC_CPRn[PRI]
	unsigned int current_priority = intc_cpr[prc_num].template Get<typename INTC_CPR::PRI>();
	
	typename std::set<IRQPriority>::const_iterator it;
	
	// Scan the priority tree, where IRQ inputs are ordered from higher priority to lower priority
	for(it = priority_tree[prc_num].begin(); it != priority_tree[prc_num].end(); it++)
	{
		const IRQPriority& irq_priority = *it;
		unsigned int priority = irq_priority.GetPriority();
		
		// Check if IRQ input has greater priority than current priority
		if(priority > current_priority)
		{
			unsigned int irq_num = irq_priority.GetIRQ();
			
			// Check IRQ input status
			if(GetIRQInputStatus(irq_num))
			{
				// IRQ input is selected
				selected_irq_num = irq_num;
				return true;
			}
		}
	}
	
	// No IRQ inputs can be selected
	return false;
}

template <typename CONFIG>
void INTC<CONFIG>::IRQAcknowledge(unsigned int prc_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Processor #" << prc_num << " acknowledged IRQ" << EndDebugInfo;
	}
	
	// Lower IRQ output
	SetIRQOutput(prc_num, false);

	// Push INTC_CPRn[PRI] onto the associated LIFO
	unsigned int current_priority = intc_cpr[prc_num].template Get<typename INTC_CPR::PRI>();

	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Pushing Priority level " << current_priority << " into LIFO #" << prc_num << EndDebugInfo;
	}
	lifo[prc_num].push(current_priority);

	// Update INTC_CPRn[PRI] with the new priority
	unsigned int irq_num = selected_irq_num[prc_num];
	
	unsigned int priority = IsSW_IRQ(irq_num) ? intc_psr_sw[IRQ2SW_IRQ(irq_num)].template Get<typename INTC_PSR<SW_IRQ>::PRIN>()
	                                          : intc_psr_hw[IRQ2HW_IRQ(irq_num)].template Get<typename INTC_PSR<HW_IRQ>::PRIN>();
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Setting Current Priority level to " << priority << " for Processor #" << prc_num << EndDebugInfo;
	}
	intc_cpr[prc_num].template Set<typename INTC_CPR::PRI>(priority);

	// For hardware IRQ, on the interrupt acknowledge cycle, the SWTn bit is cleared
	if(IsHW_IRQ(irq_num))
	{
		intc_psr_hw[IRQ2HW_IRQ(irq_num)].template Set<typename INTC_PSR<HW_IRQ>::SWTN>(0);
	}
	
	// Lower latched IRQ input status
	//SetIRQInputStatus(irq_num, false); // Probably a mistake
	
	// INTC_CPRn changes may affect IRQ selection
	UpdateIRQSelect(prc_num);
}

template <typename CONFIG>
void INTC<CONFIG>::EndOfInterrupt(unsigned int prc_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Processor #" << prc_num << " signaled End-of-Interrupt" << EndDebugInfo;
	}

	// Pop last pushed priority from LIFO, and put it in INTC_CPRn[PRI]
	unsigned int priority = lifo[prc_num].empty() ? 0 : lifo[prc_num].top();
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": Poping Priority level " << priority << " from LIFO #" << prc_num << " into current priority for Processor #" << prc_num << EndDebugInfo;
	}

	intc_cpr[prc_num].template Set<typename INTC_CPR::PRI>(priority);

	lifo[prc_num].pop();

	// INTC_CPRn changes may affect IRQ selection
	UpdateIRQSelect(prc_num);
}

template <typename CONFIG>
void INTC<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void INTC<CONFIG>::DumpPriorityTree(unsigned int prc_num)
{
	typename std::set<IRQPriority>::const_iterator it;
	
	// Scan the priority tree, where IRQ inputs are ordered from higher priority to lower priority
	for(it = priority_tree[prc_num].begin(); it != priority_tree[prc_num].end(); it++)
	{
		const IRQPriority& irq_priority = *it;
		unsigned int priority = irq_priority.GetPriority();
		unsigned int irq_num = irq_priority.GetIRQ();
		
		logger << DebugInfo << "PRI #" << priority << ": IRQ #" << irq_num << " = " << GetIRQInputStatus(irq_num) << EndDebugInfo;
	}
}

} // end of namespace intc
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace interrupt
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_INTERRUPT_FREESCALE_MPC57XX_INTC_INTC_TCC__
