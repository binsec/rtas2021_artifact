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

#ifndef __UNISIM_COMPONENT_TLM2_COM_BOSCH_M_CAN_M_CAN_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_BOSCH_M_CAN_M_CAN_TCC__

#include <unisim/component/tlm2/com/bosch/m_can/m_can.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace bosch {
namespace m_can {

using unisim::kernel::tlm2::TLM_CAN_CORE_SYNCHRONIZING;
using unisim::kernel::tlm2::TLM_CAN_CORE_IDLE;
using unisim::kernel::tlm2::TLM_CAN_CORE_RECEIVER;
using unisim::kernel::tlm2::TLM_CAN_CORE_TRANSMITTER;
using unisim::kernel::tlm2::TLM_CAN_IDLE_PHASE;
using unisim::kernel::tlm2::TLM_CAN_ERROR_ACTIVE_STATE;
using unisim::kernel::tlm2::TLM_CAN_ERROR_ACTIVE_STATE;
using unisim::kernel::tlm2::TLM_CAN_ERROR_PASSIVE_STATE;
using unisim::kernel::tlm2::TLM_CAN_BUS_OFF_STATE;
using unisim::kernel::tlm2::TLM_CAN_NO_ERROR;
using unisim::kernel::tlm2::TLM_CAN_STUFF_ERROR;
using unisim::kernel::tlm2::TLM_CAN_FORM_ERROR;
using unisim::kernel::tlm2::TLM_CAN_ACK_ERROR;
using unisim::kernel::tlm2::TLM_CAN_BIT1_ERROR;
using unisim::kernel::tlm2::TLM_CAN_BIT0_ERROR;
using unisim::kernel::tlm2::TLM_CAN_CRC_ERROR;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const bool M_CAN<CONFIG>::threaded_model;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::ERROR_WARNING_LIMIT;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_DEDICATED_TX_BUFFERS;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_TX_QUEUE_FIFO_SIZE;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_TX_BUFFERS;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_STD_MSG_ID_FILTER_ELEMENTS;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_XTD_MSG_ID_FILTER_ELEMENTS;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_RX_FIFO_WATERMARK;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_RX_FIFO_SIZE;

template <typename CONFIG>
const unsigned int M_CAN<CONFIG>::MAX_TX_EVENT_FIFO_SIZE;

template <typename CONFIG>
M_CAN<CONFIG>::M_CAN(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, Super(name, parent)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, m_clk("m_clk")
	, can_clk("can_clk")
	, reset_b("reset_b")
	, DMA_ACK("DMA_ACK")
	, can_message_ram_if("can_message_ram_if")
	, INT0("INT0")
	, INT1("INT1")
	, DMA_REQ("DMA_REQ")
	, FE("FE", NUM_FILTER_EVENTS)
	, registers_export("registers-export", this)
	, m_clk_prop_proxy(m_clk)
	, can_clk_prop_proxy(can_clk)
	, crel(this)
	, endn(this)
	, fbtp(this)
	, test(this)
	, rwd(this)
	, cccr(this)
	, btp(this)
	, tscc(this)
	, tscv(this)
	, tocc(this)
	, tocv(this)
	, ecr(this)
	, psr(this)
	, ir(this)
	, ie(this)
	, ils(this)
	, ile(this)
	, gfc(this)
	, sidfc(this)
	, xidfc(this)
	, xidam(this)
	, hpms(this)
	, ndat1(this)
	, ndat2(this)
	, rxf0c(this)
	, rxf0s(this)
	, rxf0a(this)
	, rxbc(this)
	, rxf1c(this)
	, rxf1s(this)
	, rxf1a(this)
	, rxesc(this)
	, txbc(this)
	, txfqs(this)
	, txesc(this)
	, txbrp(this)
	, txbar(this)
	, txbcr(this)
	, txbto(this)
	, txbcf(this)
	, txbtie(this)
	, txbcie(this)
	, txefc(this)
	, txefs(this)
	, txefa(this)
	, gen_int0_event("gen_int0_event")
	, gen_int1_event("gen_int1_event")
	, gen_dma_req_event("gen_dma_req_event")
	, gen_filter_event_pulse_event("gen_filter_event_pulse_event", NUM_FILTER_EVENTS)
	, gen_filter_event_pos()
	, gen_filter_event_neg()
	, reg_addr_map()
	, registers_registry()
	, schedule()
	, endian(unisim::util::endian::E_LITTLE_ENDIAN) // the subsystem follows the little-endian format
	, param_endian("endian", this, endian, "endian")
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
	, can_clock_period(10.0, sc_core::SC_NS)          
	, can_clock_start_time(sc_core::SC_ZERO_TIME)
	, can_clock_posedge_first(true)
	, can_clock_duty_cycle(0.5)
	, max_time_to_next_timers_run(1.0, sc_core::SC_MS)
	, last_timers_run_time()
	, start_request(false)
	, init_request(false)
	, core_config()
	, phase(TLM_CAN_IDLE_PHASE)
	, can_error(TLM_CAN_NO_ERROR)
	, can_error_changed(false)
	, tx_scan_event("tx_scan_event")
	, tx_event_store_event("tx_event_store_event")
	, rx_filter_event("rx_filter_event")
	, flip_flop(0)
	, tx_msg_pipe()
	, pending_tx_msg(0)
	, curr_tx_msg(0)
	, rx_msg()
{
	peripheral_slave_if(*this);
	can_message_ram_if(*this);

	std::stringstream description_sstr;
	description_sstr << "Bosch M_CAN Controller Area Network:" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - Endianess: " << endian << std::endl;
	description_sstr << "  - Core release: " << CONFIG::CORE_RELEASE << '.' << CONFIG::CORE_RELEASE_STEP << '.' << CONFIG::CORE_RELEASE_SUBSTEP << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: M_CAN Controller Area Network User's Manual, Revision 3.0.2, 14.02.2013" << std::endl;
	
	this->SetDescription(description_sstr.str().c_str());

	SC_HAS_PROCESS(M_CAN);

	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	
	reg_addr_map.MapRegister(crel  .ADDRESS_OFFSET, &crel  );
	reg_addr_map.MapRegister(endn  .ADDRESS_OFFSET, &endn  );
	reg_addr_map.MapRegister(fbtp  .ADDRESS_OFFSET, &fbtp  );
	reg_addr_map.MapRegister(test  .ADDRESS_OFFSET, &test  );
	reg_addr_map.MapRegister(rwd   .ADDRESS_OFFSET, &rwd   );
	reg_addr_map.MapRegister(cccr  .ADDRESS_OFFSET, &cccr  );
	reg_addr_map.MapRegister(btp   .ADDRESS_OFFSET, &btp   );
	reg_addr_map.MapRegister(tscc  .ADDRESS_OFFSET, &tscc  );
	reg_addr_map.MapRegister(tscv  .ADDRESS_OFFSET, &tscv  );
	reg_addr_map.MapRegister(tocc  .ADDRESS_OFFSET, &tocc  );
	reg_addr_map.MapRegister(tocv  .ADDRESS_OFFSET, &tocv  );
	reg_addr_map.MapRegister(ecr   .ADDRESS_OFFSET, &ecr   );
	reg_addr_map.MapRegister(psr   .ADDRESS_OFFSET, &psr   );
	reg_addr_map.MapRegister(ir    .ADDRESS_OFFSET, &ir    );
	reg_addr_map.MapRegister(ie    .ADDRESS_OFFSET, &ie    );
	reg_addr_map.MapRegister(ils   .ADDRESS_OFFSET, &ils   );
	reg_addr_map.MapRegister(ile   .ADDRESS_OFFSET, &ile   );
	reg_addr_map.MapRegister(gfc   .ADDRESS_OFFSET, &gfc   );
	reg_addr_map.MapRegister(sidfc .ADDRESS_OFFSET, &sidfc );
	reg_addr_map.MapRegister(xidfc .ADDRESS_OFFSET, &xidfc );
	reg_addr_map.MapRegister(xidam .ADDRESS_OFFSET, &xidam );
	reg_addr_map.MapRegister(hpms  .ADDRESS_OFFSET, &hpms  );
	reg_addr_map.MapRegister(ndat1 .ADDRESS_OFFSET, &ndat1 );
	reg_addr_map.MapRegister(ndat2 .ADDRESS_OFFSET, &ndat2 );
	reg_addr_map.MapRegister(rxf0c .ADDRESS_OFFSET, &rxf0c );
	reg_addr_map.MapRegister(rxf0s .ADDRESS_OFFSET, &rxf0s );
	reg_addr_map.MapRegister(rxf0a .ADDRESS_OFFSET, &rxf0a );
	reg_addr_map.MapRegister(rxbc  .ADDRESS_OFFSET, &rxbc  );
	reg_addr_map.MapRegister(rxf1c .ADDRESS_OFFSET, &rxf1c );
	reg_addr_map.MapRegister(rxf1s .ADDRESS_OFFSET, &rxf1s );
	reg_addr_map.MapRegister(rxf1a .ADDRESS_OFFSET, &rxf1a );
	reg_addr_map.MapRegister(rxesc .ADDRESS_OFFSET, &rxesc );
	reg_addr_map.MapRegister(txbc  .ADDRESS_OFFSET, &txbc  );
	reg_addr_map.MapRegister(txfqs .ADDRESS_OFFSET, &txfqs );
	reg_addr_map.MapRegister(txesc .ADDRESS_OFFSET, &txesc );
	reg_addr_map.MapRegister(txbrp .ADDRESS_OFFSET, &txbrp );
	reg_addr_map.MapRegister(txbar .ADDRESS_OFFSET, &txbar );
	reg_addr_map.MapRegister(txbcr .ADDRESS_OFFSET, &txbcr );
	reg_addr_map.MapRegister(txbto .ADDRESS_OFFSET, &txbto );
	reg_addr_map.MapRegister(txbcf .ADDRESS_OFFSET, &txbcf );
	reg_addr_map.MapRegister(txbtie.ADDRESS_OFFSET, &txbtie);
	reg_addr_map.MapRegister(txbcie.ADDRESS_OFFSET, &txbcie);
	reg_addr_map.MapRegister(txefc .ADDRESS_OFFSET, &txefc );
	reg_addr_map.MapRegister(txefs .ADDRESS_OFFSET, &txefs );
	reg_addr_map.MapRegister(txefa .ADDRESS_OFFSET, &txefa );

	registers_registry.AddRegisterInterface(crel.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(endn.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(fbtp.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(test.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(rwd.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(cccr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(btp.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(tscc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(tscv.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(tocc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(tocv.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(ecr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(psr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(ir.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(ie.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(ils.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(ile.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(gfc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(sidfc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(xidfc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(xidam.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(hpms.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(ndat1.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(ndat2.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(rxf0c.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(rxf0s.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(rxf0a.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(rxbc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(rxf1c.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(rxf1s.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(rxf1a.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(rxesc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txbc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txfqs.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txesc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txbrp.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txbar.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txbcr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txbto.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txbcf.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txbtie.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txbcie.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txefc.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txefs.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(txefa.CreateRegisterInterface());

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	SC_METHOD(INT0_Process);
	sensitive << gen_int0_event;
	
	SC_METHOD(INT1_Process);
	sensitive << gen_int1_event;

	SC_METHOD(DMA_REQ_Process);
	sensitive << gen_dma_req_event;
	
	SC_METHOD(DMA_ACK_Process);
	sensitive << DMA_ACK.pos();
	
	unsigned int i;
	for(i = 0; i < NUM_FILTER_EVENTS; i++)
	{
		std::stringstream filter_event_process_name_sstr;
		filter_event_process_name_sstr << "FilterEventProcess" << i;
		
		sc_core::sc_spawn_options filter_event_process_spawn_options;
		filter_event_process_spawn_options.spawn_method();
		filter_event_process_spawn_options.set_sensitivity(&gen_filter_event_pulse_event[i]);
		
		sc_core::sc_spawn(sc_bind(&M_CAN<CONFIG>::FilterEventProcess, this, i), filter_event_process_name_sstr.str().c_str(), &filter_event_process_spawn_options);
	}
}

template <typename CONFIG>
M_CAN<CONFIG>::~M_CAN()
{
}

//////////////// unisim::service::interface::Registers ////////////////////

template <typename CONFIG>
unisim::service::interfaces::Register *M_CAN<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void M_CAN<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void M_CAN<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;

	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&M_CAN<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	// Spawn CANClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options can_clock_properties_changed_process_spawn_options;
	
	can_clock_properties_changed_process_spawn_options.spawn_method();
	can_clock_properties_changed_process_spawn_options.set_sensitivity(&can_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&M_CAN<CONFIG>::CANClockPropertiesChangedProcess, this), "CANClockPropertiesChangedProcess", &can_clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void M_CAN<CONFIG>::Reset()
{
	crel.Reset();
	endn.Reset();
	fbtp.Reset();
	test.Reset();
	rwd.Reset();
	cccr.Reset();
	btp.Reset();
	tscc.Reset();
	tscv.Reset();
	tocc.Reset();
	tocv.Reset();
	ecr.Reset();
	psr.Reset();
	ir.Reset();
	ie.Reset();
	ils.Reset();
	ile.Reset();
	gfc.Reset();
	sidfc.Reset();
	xidfc.Reset();
	xidam.Reset();
	hpms.Reset();
	ndat1.Reset();
	ndat2.Reset();
	rxf0c.Reset();
	rxf0s.Reset();
	rxf0a.Reset();
	rxbc.Reset();
	rxf1c.Reset();
	rxf1s.Reset();
	rxf1a.Reset();
	rxesc.Reset();
	txbc.Reset();
	txfqs.Reset();
	txesc.Reset();
	txbrp.Reset();
	txbar.Reset();
	txbcr.Reset();
	txbto.Reset();
	txbcf.Reset();
	txbtie.Reset();
	txbcie.Reset();
	txefc.Reset();
	txefs.Reset();
	txefa.Reset();
	start_request = false;
	init_request = false;
	phase = TLM_CAN_IDLE_PHASE;
	can_error = TLM_CAN_NO_ERROR;
	can_error_changed = false;
	flip_flop = 0;
	pending_tx_msg = 0;
	curr_tx_msg = 0;
	
	last_timers_run_time = sc_core::sc_time_stamp();
	unisim::kernel::tlm2::AlignToClock(last_timers_run_time, tscc.GetTimerPeriod());
	
	for(unsigned i = 0; i < NUM_FILTER_EVENTS; i++)
	{
		gen_filter_event_pos[i] = false;
		gen_filter_event_neg[i] = false;
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::RequestStart()
{
	if(init_request) init_request = false;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":requesting start (leaving init mode)" << EndDebugInfo;
	}
	
	start_request = true;
}

template <typename CONFIG>
void M_CAN<CONFIG>::RequestInit()
{
	if(start_request) start_request = false;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":requesting init mode" << EndDebugInfo;
	}
	
	init_request = true;
}

template <typename CONFIG>
bool M_CAN<CONFIG>::InitMode() const
{
	return cccr.template Get<typename CCCR::INIT>();
}

template <typename CONFIG>
bool M_CAN<CONFIG>::ConfigurationChangedEnabled() const
{
	return cccr.template Get<typename CCCR::CCE>();
}

template <typename CONFIG>
bool M_CAN<CONFIG>::CheckProtectedWrite() const
{
	if(!InitMode() || !ConfigurationChangedEnabled())
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":Access to protected write (P) register" << EndDebugWarning;
		return false;
	}
	return true;
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateLastErrorCode()
{
	if(can_error_changed)
	{
		unsigned int lec = LEC_NO_CHANGE;
		
		if(can_error == TLM_CAN_NO_ERROR)
		{
			lec = LEC_NO_ERROR;
		}
		else
		{
			switch(can_error)
			{
				case TLM_CAN_STUFF_ERROR:
					lec = LEC_STUFF_ERROR;
					ir.template Set<typename IR::STE>(1); // Stuff Error
					break;
				case TLM_CAN_FORM_ERROR :
					lec = LEC_FORM_ERROR;
					ir.template Set<typename IR::FOE>(1); // Format Error
					break;
				case TLM_CAN_ACK_ERROR  :
					lec = LEC_ACK_ERROR;
					ir.template Set<typename IR::ACKE>(1); // Acknowledge Error
					break;
				case TLM_CAN_BIT1_ERROR :
					lec = LEC_BIT1_ERROR;
					ir.template Set<typename IR::BE>(1); // Bit Error
					break;
				case TLM_CAN_BIT0_ERROR :
					lec = LEC_BIT0_ERROR;
					ir.template Set<typename IR::BE>(1); // Bit Error
					break;
				case TLM_CAN_CRC_ERROR  :
					lec = LEC_CRC_ERROR;
					ir.template Set<typename IR::CRCE>(1); // CRC Error
					break;
				default:
					break;
			}
			
			UpdateInterrupts();
		}
		
		can_error_changed = false;
		psr.template Set<typename PSR::LEC>(lec);
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateWarningStatus()
{
	bool old_ew = psr.template Get<typename PSR::EW>();
	bool new_ew = (ecr.GetTransmitErrorCount() >= ERROR_WARNING_LIMIT) ||
	              (ecr.GetReceiveErrorCount() >= ERROR_WARNING_LIMIT);
	
	psr.template Set<typename PSR::EW>(new_ew);
	
	if(old_ew != new_ew)
	{
		ir.template Set<typename IR::EW>(1);
		UpdateInterrupts();
	}
}

template <typename CONFIG>
bool M_CAN<CONFIG>::is_enabled() const
{
	return (!cccr.template Get<typename CCCR::INIT>() || start_request) && !init_request && !cccr.template Get<typename CCCR::CSR>();
}

template <typename CONFIG>
const tlm_can_core_config& M_CAN<CONFIG>::get_config() const
{
	core_config.reset();
	core_config.set_sample_point(btp.GetSamplePoint());
	core_config.set_phase_seg2(btp.GetPhaseSeg2());
	core_config.set_loopback_mode(cccr.template Get<typename CCCR::TEST>() && test.template Get<typename TEST::LBCK>());
	core_config.set_bus_monitoring_mode(cccr.template Get<typename CCCR::MON>());
	core_config.set_automatic_retransmission(!cccr.template Get<typename CCCR::DAR>());
	core_config.set_restricted_operation_mode(cccr.template Get<typename CCCR::ASM>());
	return core_config;
}

template <typename CONFIG>
tlm_can_error M_CAN<CONFIG>::get_can_error() const
{
	return can_error;
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_can_error(tlm_can_error _can_error)
{
	can_error_changed = (can_error != _can_error);
	can_error = _can_error;
}

template <typename CONFIG>
tlm_can_core_activity M_CAN<CONFIG>::get_core_activity() const
{
	ACTIVITY act = static_cast<ACTIVITY>(psr.template Get<typename PSR::ACT>());
	switch(act)
	{
		case ACT_SYNCHRONIZING: return TLM_CAN_CORE_SYNCHRONIZING;
		case ACT_IDLE         : return TLM_CAN_CORE_IDLE;
		case ACT_RECEIVER     : return TLM_CAN_CORE_RECEIVER;
		case ACT_TRANSMITTER  : return TLM_CAN_CORE_TRANSMITTER;
	}
	return TLM_CAN_CORE_IDLE;
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_core_activity(tlm_can_core_activity core_activity)
{
	if(core_activity != get_core_activity())
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "core is now " << core_activity << EndDebugInfo;
		}
	}
	
	ACTIVITY new_act = ACT_IDLE;
	switch(core_activity)
	{
		case TLM_CAN_CORE_SYNCHRONIZING: new_act = ACT_SYNCHRONIZING; break;
		case TLM_CAN_CORE_IDLE         : new_act = ACT_IDLE; break;
		case TLM_CAN_CORE_RECEIVER     : new_act = ACT_RECEIVER; break;
		case TLM_CAN_CORE_TRANSMITTER  : new_act = ACT_TRANSMITTER; break;
	}
	
	unsigned int old_act = psr.template Get<typename PSR::ACT>();
	if(init_request && (old_act != ACT_IDLE) && (new_act == ACT_IDLE)) // init request (software wrote 1 in CCCR[INIT]) AND node is now idle?
	{
		init_request = false;
		EnterInitMode();
		
		if(cccr.template Get<typename CCCR::CSR>()) // clock stop request?
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "acknowledging clock stop request" << EndDebugInfo;
			}
			
			cccr.template Set<typename CCCR::CSA>(1); // acknowledge clock stop request
		}
	}
	else if(start_request && (old_act == ACT_IDLE) && (new_act != ACT_IDLE)) // start request (software wrote 0 in CCCR[INIT]) AND node is no longer idle?
	{
		start_request = false;
		LeaveInitMode();
	}
	
	psr.template Set<typename PSR::ACT>(new_act);
}

template <typename CONFIG>
tlm_can_phase M_CAN<CONFIG>::get_phase() const
{
	return phase;
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_phase(tlm_can_phase _phase)
{
	if(phase != _phase)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "entering " << _phase << EndDebugInfo;
		}
	}
	phase = _phase;
}

template <typename CONFIG>
uint8_t M_CAN<CONFIG>::get_transmit_error_count() const
{
	return ecr.GetTransmitErrorCount();
}

template <typename CONFIG>
uint8_t M_CAN<CONFIG>::get_receive_error_count() const
{
	return ecr.GetReceiveErrorCount();
}

template <typename CONFIG>
tlm_can_state M_CAN<CONFIG>::get_state() const
{
	return psr.template Get<typename PSR::BO>() ? TLM_CAN_BUS_OFF_STATE
	                                                  : (psr.template Get<typename PSR::EP>() ? TLM_CAN_ERROR_PASSIVE_STATE
	                                                                                                : TLM_CAN_ERROR_ACTIVE_STATE);
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_transmit_error_count(uint8_t v)
{
	ecr.SetTransmitErrorCount(v);
	UpdateWarningStatus();
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_receive_error_count(uint8_t v)
{
	ecr.SetReceiveErrorCount(v);
	UpdateWarningStatus();
}

template <typename CONFIG>
void M_CAN<CONFIG>::set_state(tlm_can_state state)
{
	if(state != get_state())
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "entering " << state << EndDebugInfo;
		}
	}
	
	bool old_ep = psr.template Get<typename PSR::EP>();
	bool old_bo = psr.template Get<typename PSR::BO>();
	bool new_ep = old_ep;
	bool new_bo = old_bo;
	
	switch(state)
	{
		case TLM_CAN_ERROR_ACTIVE_STATE :
			new_ep = 0;
			new_bo = 0;
			break;
		case TLM_CAN_ERROR_PASSIVE_STATE: 
			new_ep = 1;
			new_bo = 0;
			break;
		case TLM_CAN_BUS_OFF_STATE      :
			new_ep = 0;
			new_bo = 1;
			break;
	}
	
	psr.template Set<typename PSR::EP>(new_ep);
	psr.template Set<typename PSR::BO>(new_bo);
	
	if(old_bo != new_bo)
	{
		ir.template Set<typename IR::BO>(1);
		UpdateInterrupts();
		
		if(new_bo)
		{
			RequestInit();
		}
	}
	
	if(old_ep != new_ep)
	{
		ir.template Set<typename IR::EP>(1);
		UpdateInterrupts();
	}
}

template <typename CONFIG>
bool M_CAN<CONFIG>::has_pending_transmission_request() const
{
	return pending_tx_msg != 0;
}

template <typename CONFIG>
const M_CAN_Message& M_CAN<CONFIG>::fetch_pending_transmission_request()
{
	assert(curr_tx_msg == 0);
	
	curr_tx_msg = pending_tx_msg;
	pending_tx_msg = 0;
	TxScan();
	return *curr_tx_msg;
}

template <typename CONFIG>
M_CAN_Message& M_CAN<CONFIG>::get_receive_message()
{
	rx_msg.Reset();
	return rx_msg;
}

template <typename CONFIG>
void M_CAN<CONFIG>::process_message_event(const tlm_can_message_event<M_CAN_TYPES>& msg_event)
{
	const M_CAN_Message& msg = msg_event.get_message();
	
	switch(msg_event.get_type())
	{
		case TLM_CAN_START_OF_FRAME_TRANSMISSION_EVENT:
			assert(curr_tx_msg == &msg);
			RunTimersToTime(sc_core::sc_time_stamp()); // make run timestamp counter until event time stamp
			curr_tx_msg->SetTimeStamp(tscv.template Get<typename TSCV::TSC>()); // timestamp message
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":start of frame transmission of message " << msg << EndDebugInfo;
			}
			break;

		case TLM_CAN_START_OF_FRAME_RECEPTION_EVENT:
			assert(&rx_msg == &msg);
			RunTimersToTime(sc_core::sc_time_stamp()); // make run timestamp counter
			rx_msg.SetTimeStamp(tscv.template Get<typename TSCV::TSC>()); // timestamp message
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":start of frame reception of message " << msg << EndDebugInfo;
			}
			break;
			
		case TLM_CAN_TRANSMISSION_ERROR_EVENT:
			assert(curr_tx_msg == &msg);
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":transmission error of message " << msg << EndDebugInfo;
			}
			UpdateLastErrorCode();
			break;
			
		case TLM_CAN_TRANSMISSION_CANCELLED_EVENT:
		{
			assert(curr_tx_msg == &msg);
			assert(msg.is_transmission_cancelled());
			
			unsigned int tx_buffer_element_index = msg.GetBufferIndex();
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":cancelled transmission of message " << msg << EndDebugInfo;
			}
			
			// Clear corresponding request in Tx Buffer Request Pending Register
			txbrp.ClearRequest(tx_buffer_element_index);

			// Clear corresponding cancellation request in Tx Buffer Cancellation Request Register
			txbcr.ClearRequest(tx_buffer_element_index);
			
			// Set corresponding bit in Tx Buffer Cancellation Finished Register
			txbcf.CancellationFinished(tx_buffer_element_index);
			
			if(txbcie.IsInterruptEnabled(tx_buffer_element_index)) // Tx Buffer Interrupt enabled?
			{
				ir.template Set<typename IR::TCF>(1); // Transmission Cancellation Finished
				UpdateInterrupts();
			}
			break;
		}
			
		case TLM_CAN_TRANSMISSION_OCCURRED_EVENT:
		{
			assert(curr_tx_msg == &msg);

			unsigned int tx_buffer_element_index = msg.GetBufferIndex();
			bool event_fifo_control = msg.GetEFC();
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":transmitted message " << msg << EndDebugInfo;
			}
			curr_tx_msg = 0;
			
			if(event_fifo_control) // Store a TX Event?
			{
				StoreTxEventFIFO(msg);
			}
			
			// Clear corresponding request in Tx Buffer Request Pending Register
			txbrp.ClearRequest(tx_buffer_element_index);
			
			if(msg.is_transmission_cancelled())
			{
				assert(txbcr.IsCancelled(tx_buffer_element_index));
				
				// Clear corresponding cancellation request in Tx Buffer Cancellation Request Register
				txbcr.ClearRequest(tx_buffer_element_index);
				
				// Set corresponding bit in Tx Buffer Cancellation Finished Register
				txbcf.CancellationFinished(tx_buffer_element_index);
			
				if(txbcie.IsInterruptEnabled(tx_buffer_element_index)) // Tx Buffer Interrupt enabled?
				{
					ir.template Set<typename IR::TCF>(1); // Transmission Cancellation Finished
					UpdateInterrupts();
				}
			}
			
			// Set corresponding bit in Tx Buffer Transmission Occurred register 
			txbto.TransmissionOccurred(tx_buffer_element_index);
			
			if(tx_buffer_element_index >= GetNumDedicatedTxBuffers()) // buffer is in Tx FIFO/Queue?
			{
				switch(txbc.template Get<typename TXBC::TFQM>()) // Tx FIFO/Queue Mode
				{
					case TFQM_FIFO:
						// Tx FIFO
						IncrementTxFIFOGetIndex();
						TxScan();
						break;
					
					case TFQM_QUEUE:
						// Tx Queue
						txfqs.template Set<typename TXFQS::TFQF>(0);     // Tx Queue is not full
						break;
				}
			}
			
			if(txbtie.IsInterruptEnabled(tx_buffer_element_index)) // Tx Buffer Interrupt enabled?
			{
				ir.template Set<typename IR::TC>(1); // Transmission Completed
				UpdateInterrupts();
			}
			break;
		}
			
		case TLM_CAN_RECEPTION_ERROR_EVENT:
			assert(&rx_msg == &msg);
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":reception error of message " << msg << EndDebugInfo;
			}
			UpdateLastErrorCode();
			break;
			
		case TLM_CAN_MESSAGE_RECEIVED_EVENT:
		{
			assert(&rx_msg == &msg);
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":received message " << msg << EndDebugInfo;
			}
			Filter(msg);
			break;
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::received_bit(bool value)
{
	if(cccr.template Get<typename CCCR::TEST>()) // Test Mode?
	{
		test.template Set<typename TEST::RX>(value);
	}
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::get_transmit_pause() const
{
	return cccr.template Get<typename CCCR::TXP>();
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateCANClock()
{
	can_clock_period = can_clk_prop_proxy.GetClockPeriod();
	can_clock_start_time = can_clk_prop_proxy.GetClockStartTime();
	can_clock_posedge_first = can_clk_prop_proxy.GetClockPosEdgeFirst();
	can_clock_duty_cycle = can_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void M_CAN<CONFIG>::MasterClockPropertiesChangedProcess()
{
	// Master Clock properties have changed
	UpdateMasterClock();
}

template <typename CONFIG>
void M_CAN<CONFIG>::CANClockPropertiesChangedProcess()
{
	// CAN Clock properties have changed
	UpdateCANClock();
}

template <typename CONFIG>
tlm::tlm_sync_enum M_CAN<CONFIG>::nb_transport_bw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		//trans.release();
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <typename CONFIG>
void M_CAN<CONFIG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

template <typename CONFIG>
void M_CAN<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool M_CAN<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
tlm::tlm_sync_enum M_CAN<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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

template <typename CONFIG>
void M_CAN<CONFIG>::ProcessEvent(Event *event)
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
				//sc_dt::uint64 end_addr = start_addr + ((streaming_width > data_length) ? data_length : streaming_width) - 1;
				
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
						logger << DebugWarning << "while mapped read/write access, " << std::hex << rws << std::dec << EndDebugWarning;
						
						if(cmd == tlm::TLM_READ_COMMAND)
						{
							memset(data_ptr, 0, data_length);
						}
					}
					payload->set_response_status(tlm::TLM_OK_RESPONSE);
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
void M_CAN<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	RunTimersToTime(time_stamp); // Run timers to time
	
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
	
	ScheduleTimersRun();
}

template <typename CONFIG>
void M_CAN<CONFIG>::Process()
{
	if(threaded_model)
	{
		while(1)
		{
			sc_core::sc_module::wait(schedule.GetKernelEvent());
			ProcessEvents();
		}
	}
	else
	{
		ProcessEvents();
		sc_core::sc_module::next_trigger(schedule.GetKernelEvent());
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::IncrementTimestampCounter(sc_dt::uint64 delta)
{
	if(delta)
	{
		if(!cccr.template Get<typename CCCR::INIT>()) // not in init mode?
		{
			uint32_t tss = tscc.template Get<typename TSCC::TSS>(); // Timestamp select
		
			if(tss == TSS_INCREMENTED) // Timestamp count is incremented according to TSCC[TCP] ?
			{
				uint32_t tsc = tscv.template Get<typename TSCV::TSC>();
				tsc += delta;
				tscv.template Set<typename TSCV::TSC>(tsc);
				
				if(tscv.template Get<typename TSCV::TSC>() == 0) // wrap-around?
				{
					ir.template Set<typename IR::TSW>(1); // Timestamp wrap-around
					UpdateInterrupts();
				}
			}
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::DecrementTimeoutCounter(sc_dt::uint64 delta)
{
	if(delta)
	{
		if(!cccr.template Get<typename CCCR::INIT>()) // not in init mode?
		{
			if(tocc.template Get<typename TOCC::ETOC>()) // Timeout Counter enabled?
			{
				uint32_t toc = tocv.template Get<typename TOCV::TOC>();
				
				if(toc > 0)
				{
					toc = (toc > delta) ? (toc - delta) : 0;
					tocv.template Set<typename TOCV::TOC>(toc);
					
					if(tocv.template Get<typename TOCV::TOC>() == 0)
					{
						ir.template Set<typename IR::TOO>(1); // Timeout Occurred
						UpdateInterrupts();
					}
				}
			}
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::RunTimersToTime(const sc_core::sc_time& time_stamp)
{
	if(time_stamp > last_timers_run_time)
	{
		// Compute the elapsed time since last timestamp counter run
		sc_core::sc_time delay_since_last_timers_run(time_stamp);
		delay_since_last_timers_run -= last_timers_run_time;
		
		// Compute number of timers ticks since last timestamp counter run
		sc_dt::uint64 delta = delay_since_last_timers_run / tscc.GetTimerPeriod();
		
		if(delta)
		{
			sc_core::sc_time run_time(tscc.GetTimerPeriod());
			run_time *= delta;
		
			IncrementTimestampCounter(delta);
			DecrementTimeoutCounter(delta);
			last_timers_run_time += run_time;
		}
	}
}

template <typename CONFIG>
sc_dt::uint64 M_CAN<CONFIG>::TicksToNextTimersRun()
{
	bool init = cccr.template Get<typename CCCR::INIT>(); // init mode
	uint32_t tss = tscc.template Get<typename TSCC::TSS>();
	uint32_t tsc = tscv.template Get<typename TSCV::TSC>();
	uint32_t max_tsc = TSCV::TSC::template GetMask<uint32_t>();
	uint32_t toc = tocv.template Get<typename TOCV::TOC>();
	bool etoc = tocc.template Get<typename TOCC::ETOC>(); // Timeout Counter enabled
	
	sc_dt::uint64 ticks_to_timestamp_counter_wrap_around = (!init && (tss == TSS_INCREMENTED)) ? (max_tsc + 1 - tsc) : 0;
	sc_dt::uint64 ticks_to_timeout_counter_zero = (!init && etoc) ? toc : 0;
	
	if(ticks_to_timestamp_counter_wrap_around == 0) return ticks_to_timeout_counter_zero;
	if(ticks_to_timeout_counter_zero == 0) return ticks_to_timestamp_counter_wrap_around;
	
	return std::min(ticks_to_timeout_counter_zero, ticks_to_timestamp_counter_wrap_around);
}

template <typename CONFIG>
sc_core::sc_time M_CAN<CONFIG>::TimeToNextTimersRun()
{
	sc_dt::int64 ticks_to_next_timers_run = TicksToNextTimersRun();
	
	if(ticks_to_next_timers_run)
	{
		sc_core::sc_time time_to_next_timers_run(tscc.GetTimerPeriod());
		time_to_next_timers_run *= ticks_to_next_timers_run;
		if(time_to_next_timers_run < max_time_to_next_timers_run) return time_to_next_timers_run;
	}
	
	return max_time_to_next_timers_run;
}

template <typename CONFIG>
void M_CAN<CONFIG>::ScheduleTimersRun()
{
	sc_core::sc_time time_to_next_timers_run = TimeToNextTimersRun();
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": time to next timers run is " << time_to_next_timers_run << EndDebugInfo;
	}
	
	sc_core::sc_time next_timers_run_time_stamp(sc_core::sc_time_stamp());
	next_timers_run_time_stamp += time_to_next_timers_run;
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": timers will run at " << next_timers_run_time_stamp << EndDebugInfo;
	}
	
	Event *event = schedule.AllocEvent();
	event->WakeUp();
	event->SetTimeStamp(next_timers_run_time_stamp);
	schedule.Notify(event);
}

template <typename CONFIG>
void M_CAN<CONFIG>::ReadWords(sc_dt::uint64 addr, uint32_t *data_ptr, unsigned int n)
{
	tlm::tlm_generic_payload payload;
	payload.set_address(addr * 4);
	payload.set_data_length(n * 4);
	payload.set_data_ptr((unsigned char *) data_ptr);
	payload.set_read();
	
	can_message_ram_if->transport_dbg(payload);
}

template <typename CONFIG>
void M_CAN<CONFIG>::WriteWords(sc_dt::uint64 addr, uint32_t *data_ptr, unsigned int n)
{
	tlm::tlm_generic_payload payload;
	payload.set_address(addr * 4);
	payload.set_data_length(n * 4);
	payload.set_data_ptr((unsigned char *) data_ptr);
	payload.set_write();
	
	can_message_ram_if->transport_dbg(payload);
}

#if 0
static inline bool BitScan(unsigned int& n, uint32_t v)
{
	unsigned int i = 0;
	if(unisim::util::arithmetic::BitScanReverse(i, v))
	{
		n = 31 - i;
		return true;
	}
	
	return false;
}
#endif

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetNumDedicatedTxBuffers() const
{
	return std::min(txbc.template Get<typename TXBC::NDTB>(), MAX_DEDICATED_TX_BUFFERS);
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetTxFIFOQueueSize() const
{
	return std::min(txbc.template Get<typename TXBC::TFQS>(), MAX_TX_QUEUE_FIFO_SIZE);
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetNumTxBuffers() const
{
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	unsigned int tx_fifo_queue_size = GetTxFIFOQueueSize();
	unsigned int num_tx_buffers = std::min(num_dedicated_tx_buffers + tx_fifo_queue_size, MAX_TX_BUFFERS);
	return num_tx_buffers;
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetTxEventFIFOSize() const
{
	return std::min(txefc.template Get<typename TXEFC::EFS>(), MAX_TX_EVENT_FIFO_SIZE);
}

template <typename CONFIG>
unsigned int M_CAN<CONFIG>::GetRxFIFOSize(unsigned int fifo_id) const
{
	assert(fifo_id < 2);
	return std::min(fifo_id ? rxf1c.template Get<typename RXF1C::F1S>() : rxf0c.template Get<typename RXF0C::F0S>(), MAX_RX_FIFO_SIZE);
}

template <typename CONFIG>
uint32_t M_CAN<CONFIG>::GetTxDedicatedMask() const
{
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	
	return num_dedicated_tx_buffers ? (~uint32_t(0) >> (32 - num_dedicated_tx_buffers)) : 0;
}

template <typename CONFIG>
uint32_t M_CAN<CONFIG>::GetTxFIFOQueueMask() const
{
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	if(num_dedicated_tx_buffers >= 32) return 0;
	
	unsigned int num_tx_buffers = GetNumTxBuffers();
	
	return num_tx_buffers ? (~uint32_t(0) << num_dedicated_tx_buffers) & (~uint32_t(0) >> (32 - num_tx_buffers)) : 0;
}

template <typename CONFIG>
uint32_t M_CAN<CONFIG>::GetTxFIFOFillMask() const
{
	if(txfqs.template Get<typename TXFQS::TFQF>()) // Tx FIFO Full?
	{
		return GetTxFIFOQueueMask();
	}

	uint32_t tx_fifo_put_index = txfqs.template Get<typename TXFQS::TFQPI>();  // Tx FIFO Put Index
	uint32_t tx_fifo_get_index = txfqs.template Get<typename TXFQS::TFGI>();   // Tx FIFO Get Index
		
	if(tx_fifo_put_index != tx_fifo_get_index) // Tx FIFO not empty?
	{
		// compute a binary mask between Get Index and Put Index excluded
		return ((~uint32_t(0) << tx_fifo_get_index) & (~uint32_t(0) >> (32 - tx_fifo_put_index))) & GetTxFIFOQueueMask();
	}
	
	return 0;
}

template <typename CONFIG>
uint32_t M_CAN<CONFIG>::GetTxBuffersMask() const
{
	unsigned int num_tx_buffers = GetNumTxBuffers();
	
	return num_tx_buffers ? ~uint32_t(0) >> (32 - num_tx_buffers) : 0;
}

template <typename CONFIG>
void M_CAN<CONFIG>::IncrementTxFIFOQueuePutIndex()
{
	bool tfqf = txfqs.template Get<typename TXFQS::TFQF>(); // Tx FIFO/Queue Full
	assert(!tfqf);
	
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	unsigned int num_tx_buffers = GetNumTxBuffers();
	
	switch(txbc.template Get<typename TXBC::TFQM>()) // Tx FIFO/Queue mode
	{
		case TFQM_FIFO:
		{
			// Tx FIFO
			uint32_t tx_fifo_put_index = txfqs.template Get<typename TXFQS::TFQPI>();  // Read Tx FIFO Put Index
			uint32_t tx_fifo_get_index = txfqs.template Get<typename TXFQS::TFGI>();    // Read Tx FIFO Get Index
			
			// Increment Put Index and wrap around
			tx_fifo_put_index = tx_fifo_put_index + 1;                                                         // Increment by 1 Put Index
			if(tx_fifo_put_index >= num_tx_buffers) tx_fifo_put_index = num_dedicated_tx_buffers;               // Wrap around
			txfqs.template Set<typename TXFQS::TFQPI>(tx_fifo_put_index);              // Commit Tx FIFO Put Index
			
			// Compute Tx FIFO Free Level
			unsigned int tx_fifo_free_level = (tx_fifo_get_index >= tx_fifo_put_index) ? (tx_fifo_get_index - tx_fifo_put_index)
			                                                                           : ((num_tx_buffers - tx_fifo_put_index) + (tx_fifo_get_index - num_dedicated_tx_buffers));
			
			txfqs.template Set<typename TXFQS::TFFL>(tx_fifo_free_level);          // Tx FIFO Free level
			txfqs.template Set<typename TXFQS::TFQF>(tx_fifo_put_index == tx_fifo_get_index); // Tx FIFO Full
				
			break;
		}
		
		case TFQM_QUEUE:
		{
			// Tx Queue
			uint32_t old_tx_queue_put_index = txfqs.template Get<typename TXFQS::TFQPI>();
			uint32_t new_tx_queue_put_index = old_tx_queue_put_index;
			
			// search for next free buffer
			uint32_t transmission_request_pending = txbrp.template Get<typename TXBRP::TRP>();
			bool found = false;
			do
			{
				new_tx_queue_put_index = new_tx_queue_put_index + 1;
				if(new_tx_queue_put_index >= num_tx_buffers) new_tx_queue_put_index = num_dedicated_tx_buffers;
				found = !(transmission_request_pending & (uint32_t(1) << new_tx_queue_put_index));
			}
			while(!found && (new_tx_queue_put_index != old_tx_queue_put_index));

			if(found)
			{
				// Found a free Tx buffer
				txfqs.template Set<typename TXFQS::TFQPI>(new_tx_queue_put_index);
			}
			else
			{
				// No free Tx buffer
				txfqs.template Set<typename TXFQS::TFQF>(1); // Tx Queue is full
			}
			
			break;
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::IncrementTxFIFOGetIndex()
{
	assert(txbc.template Get<typename TXBC::TFQM>() == TFQM_FIFO);

	uint32_t tx_fifo_put_index = txfqs.template Get<typename TXFQS::TFQPI>();  // Read Tx FIFO Put Index
	uint32_t tx_fifo_get_index = txfqs.template Get<typename TXFQS::TFGI>();    // Read Tx FIFO Get Index
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	unsigned int num_tx_buffers = GetNumTxBuffers();
	tx_fifo_get_index = tx_fifo_get_index + 1;
	if(tx_fifo_get_index >= num_tx_buffers) tx_fifo_get_index = num_dedicated_tx_buffers;                 // wrap around
	txfqs.template Set<typename TXFQS::TFGI>(tx_fifo_get_index);
	// Compute Tx FIFO Free Level
	unsigned int tffl = (tx_fifo_get_index > tx_fifo_put_index) ? (tx_fifo_get_index - tx_fifo_put_index) : ((num_tx_buffers - tx_fifo_put_index) + (tx_fifo_get_index - num_dedicated_tx_buffers));
	
	bool tx_fifo_empty = (tx_fifo_get_index == tx_fifo_put_index);
	
	txfqs.template Set<typename TXFQS::TFFL>(tffl);  // TX FIFO Free Level
	txfqs.template Set<typename TXFQS::TFQF>(0);     // Tx FIFO is not full
	
	if(tx_fifo_empty)
	{
		ir.template Set<typename IR::TFE>(1); // TX FIFO Empty
		UpdateInterrupts();
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::AddTxRequests()
{
	// The bits in TXBTO are reset when a new transmission is
	// requested by writing a '1' to the corresponding bit of register TXBAR
	txbto = txbto & ~txbar;
	// The bits in TXBCF are reset when a new transmission is
	// requested by writing a '1' to the corresponding bit of register TXBAR
	txbcf = txbcf & ~txbar;
	
	if(txbc.template Get<typename TXBC::TFQM>() == TFQM_FIFO) // Tx FIFO Mode?
	{
		// For each add request in txbar, increment Tx FIFO Put Index
		do
		{
			uint32_t tx_fifo_put_index = txfqs.template Get<typename TXFQS::TFQPI>();  // Tx FIFO Put Index
			if(!(txbar & (uint32_t(1) << tx_fifo_put_index))) break; // No add request corresponding to Put Index: stop
			IncrementTxFIFOQueuePutIndex();
		}
		while(1);
	}
	
	// Add requests in TXBRP
	if(txbc.template Get<typename TXBC::TFQM>() == TFQM_FIFO) // Tx FIFO Mode?
	{
		// Add Dedicated Tx Buffers and Tx FIFO Fill requests
		txbrp = txbrp | (txbar & (GetTxFIFOFillMask() | GetTxDedicatedMask()));
	}
	else
	{
		// Add all requests
		txbrp = (txbrp | txbar) & GetTxBuffersMask();
	}
	
	// Clear add requests
	txbar = txbar & ~txbrp;
	
	// Tx scan
	TxScan();
}

template <typename CONFIG>
void M_CAN<CONFIG>::CancelTxRequests()
{
	// immediately cancel requests but request corresponding to current message being transmitted (it will be cancelled later)
	uint32_t curr_tx_msg_mask = curr_tx_msg ? (uint32_t(1) << curr_tx_msg->GetBufferIndex()) : 0;
	if(curr_tx_msg)
	{
		if(txbcr.IsCancelled(curr_tx_msg->GetBufferIndex()))
		{
			curr_tx_msg->set_transmission_cancelled();
		}
	}
	txbrp = txbrp & ~txbcr & ~curr_tx_msg_mask;
	txbar = txbar & ~txbar & ~curr_tx_msg_mask;
	txbcf = txbcf | (txbcr & ~curr_tx_msg_mask);
	txbcr = txbcr & txbrp; // the bits remains set until the corresponding bits in TXBRP are reset
	
	TxScan();
}

template <typename CONFIG>
void M_CAN<CONFIG>::EnterInitMode()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":entering init mode" << EndDebugInfo;
	}
	
	cccr.template Set<typename CCCR::INIT>(1);  // enter init mode (that acknowledges software write to CCCR[INIT])
}

template <typename CONFIG>
void M_CAN<CONFIG>::LeaveInitMode()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":leaving init mode" << EndDebugInfo;
	}
	
	cccr.template Set<typename CCCR::INIT>(0); // leave init mode (that acknowledges software write to CCCR[INIT])
	
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	
	// Initialize TX FIFO/Queue Status
	txfqs.template Set<typename TXFQS::TFQF>(0); // TX FIFO/Queue is not full
	txfqs.template Set<typename TXFQS::TFQPI>(num_dedicated_tx_buffers); // Put Index is after last dedicated Tx buffer
	
	switch(txbc.template Get<typename TXBC::TFQM>()) // Tx FIFO/Queue Mode
	{
		case TFQM_FIFO:
		{
			// Tx FIFO
			unsigned int num_tx_buffers = GetNumTxBuffers();
			txfqs.template Set<typename TXFQS::TFGI>(num_dedicated_tx_buffers); // Gut Index is after last dedicated Tx buffer
			txfqs.template Set<typename TXFQS::TFFL>(num_tx_buffers - num_dedicated_tx_buffers); // Tx FIFO Free level is the actual Tx FIFO size
			break;
		}
		
		case TFQM_QUEUE:
		{
			// Tx Queue
			txfqs.template Set<typename TXFQS::TFGI>(0); // read as 0
			txfqs.template Set<typename TXFQS::TFFL>(0); // read as 0
			break;
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::StoreTxEventFIFO(const M_CAN_Message& msg)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":storing Tx Event in Tx Event FIFO" << EndDebugInfo;
	}

	unsigned int tx_event_fifo_size = GetTxEventFIFOSize();
	bool event_fifo_full = txefs.template Get<typename TXEFS::EFF>();
	
	if(tx_event_fifo_size == 0)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":No Tx Event FIFO" << EndDebugInfo;
		}
	}
	
	if((tx_event_fifo_size == 0) || event_fifo_full)
	{
		// Tx Event FIFO has zero size or Tx Event FIFO is full
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":Tx Event FIFO Element lost" << EndDebugInfo;
		}
		
		ir.template Set<typename IR::TEFL>(1); // Tx Event FIFO Element Lost
		txefs.template Set<typename TXEFS::TEFL>(1); // Tx Event FIFO Element Lost
	}
	else // ((tx_event_fifo_size != 0) && !event_fifo_full)
	{
		// Tx Event FIFO has non-zero size and Tx Event FIFO is not full

		Tx_Event_FIFO_Element tx_event;
		tx_event.e[0] = 0;
		Tx_Event_FIFO_Element::E0::ESI::template Set<uint32_t>(tx_event.e[0], msg.get_error_state_indicator());
		Tx_Event_FIFO_Element::E0::XTD::template Set<uint32_t>(tx_event.e[0], msg.is_identifier_extended());
		Tx_Event_FIFO_Element::E0::RTR::template Set<uint32_t>(tx_event.e[0], msg.is_remote_transmission_request());
		Tx_Event_FIFO_Element::E0::ID::template Set<uint32_t>(tx_event.e[0], msg.get_identifier());
		tx_event.e[1] = 0;
		Tx_Event_FIFO_Element::E1::MM::template Set<uint32_t>(tx_event.e[1], msg.GetMessageMarker());
		Tx_Event_FIFO_Element::E1::ET::template Set<uint32_t>(tx_event.e[1], msg.is_transmission_cancelled() ? 0x2 : 0x1);
		Tx_Event_FIFO_Element::E1::EDL::template Set<uint32_t>(tx_event.e[1], msg.is_fd());
		Tx_Event_FIFO_Element::E1::BRS::template Set<uint32_t>(tx_event.e[1], msg.get_bit_rate_switch());
		Tx_Event_FIFO_Element::E1::DLC::template Set<uint32_t>(tx_event.e[1], msg.get_data_length_code());
		Tx_Event_FIFO_Element::E1::TXTS::template Set<uint32_t>(tx_event.e[1], msg.GetTimeStamp());
		
		unsigned int tx_event_fifo_start_addr = txefc.template Get<typename TXEFC::EFSA>(); // word address
		unsigned int tx_event_fifo_watermark = txefc.template Get<typename TXEFC::EFWM>(); // Read Tx Event FIFO Watermark
		unsigned int tx_event_fifo_put_index = txefs.template Get<typename TXEFS::EFPI>(); // Read Tx Event FIFO Put Index
		unsigned int tx_event_fifo_get_index = txefs.template Get<typename TXEFS::EFGI>(); // Read Tx Event FIFO Get Index
		
		sc_dt::uint64 tx_event_addr = tx_event_fifo_start_addr + (tx_event_fifo_put_index * 2); // word address
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":storing Tx Event FIFO Element " << tx_event << " at @0x" << std::hex << tx_event_addr << std::dec << EndDebugInfo;
		}
		
		tx_event.e[0] = unisim::util::endian::Host2Target(endian, tx_event.e[0]);
		tx_event.e[1] = unisim::util::endian::Host2Target(endian, tx_event.e[1]);
		WriteWords(tx_event_addr, &tx_event.e[0], 2); // Write the Tx Event (2 words) in CAN message RAM
		
		// Increment Put Index and wrap around
		tx_event_fifo_put_index = tx_event_fifo_put_index + 1;      // Increment by 1 Put Index
		if(tx_event_fifo_put_index >= tx_event_fifo_size) tx_event_fifo_put_index = 0; // Wrap around
		txefs.template Set<typename TXEFS::EFPI>(tx_event_fifo_put_index);   // Commit Tx Event FIFO Put Index
		
		// Determine Fill level of Tx Event FIFO
		unsigned int tx_event_fifo_fill_level = (tx_event_fifo_get_index >= tx_event_fifo_put_index) ? (tx_event_fifo_size - tx_event_fifo_get_index + tx_event_fifo_put_index)
		                                                                                             : (tx_event_fifo_put_index - tx_event_fifo_get_index);
		// Determine whether Tx Event FIFO is full
		bool tx_event_fifo_full = (tx_event_fifo_put_index == tx_event_fifo_get_index);
		
		if(unlikely(verbose))
		{
			if(tx_event_fifo_full)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":Tx Event FIFO is full" << EndDebugInfo;
			}
		}

		txefs.template Set<typename TXEFS::EFFL>(tx_event_fifo_fill_level);  // Event FIFO Fill Level
		txefs.template Set<typename TXEFS::EFF>(tx_event_fifo_full);         // Event FIFO Full
		
		if(tx_event_fifo_fill_level == tx_event_fifo_watermark)
		{
			ir.template Set<typename IR::TEFW>(1); // Tx Event FIFO Watermark Reached
		}
		
		if(tx_event_fifo_full)
		{
			ir.template Set<typename IR::TEFF>(1); // Tx Event FIFO Full
		}
		
		ir.template Set<typename IR::TEFN>(1); // Tx Event FIFO New Entry
	}
	
	UpdateInterrupts();
}

template <typename CONFIG>
void M_CAN<CONFIG>::AcknowledgeTxEventFIFO()
{
	unsigned int tx_event_fifo_ack_index = txefa.template Get<typename TXEFA::EFAI>();
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":Tx Event FIFO acknowledged up to Element #" << tx_event_fifo_ack_index << EndDebugInfo; 
	}
	
	unsigned int tx_event_fifo_size = GetTxEventFIFOSize();
	
	if(tx_event_fifo_size == 0)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Tx Event while there's no Tx Event FIFO" << EndDebugWarning;
		}
		return;
	}

	if(tx_event_fifo_ack_index >= tx_event_fifo_size)
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Tx Event which is outside of Tx Event FIFO" << EndDebugWarning;
		return;
	}
	
	unsigned int tx_event_fifo_put_index = txefs.template Get<typename TXEFS::EFPI>(); // Read Tx Event FIFO Put Index
	unsigned int tx_event_fifo_get_index = txefs.template Get<typename TXEFS::EFGI>(); // Read Tx Event FIFO Get Index
	
	if((tx_event_fifo_ack_index >= tx_event_fifo_put_index) && (tx_event_fifo_ack_index < tx_event_fifo_get_index))
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Tx Event which is freed in Tx Event FIFO" << EndDebugWarning;
		return;
	}
	
	tx_event_fifo_get_index = tx_event_fifo_ack_index + 1;   // New Tx Event FIFO Get Index is M_CAN_TXEFA[EFAI] + 1
	if(tx_event_fifo_get_index >= tx_event_fifo_size) tx_event_fifo_get_index = 0; // Wrap around
	
	txefs.template Set<typename TXEFS::EFGI>(tx_event_fifo_get_index); // Set Tx Event FIFO Get Index to M_CAN_TXEFA[EFAI] + 1
	
	// Determine Fill level of Tx Event FIFO
	unsigned int event_fifo_fill_level = (tx_event_fifo_get_index > tx_event_fifo_put_index) ? (tx_event_fifo_size - tx_event_fifo_get_index + tx_event_fifo_put_index)
	                                                                                         : (tx_event_fifo_put_index - tx_event_fifo_get_index);
	
	txefs.template Set<typename TXEFS::EFFL>(event_fifo_fill_level);  // Event FIFO Fill Level
	txefs.template Set<typename TXEFS::EFF>(0);                       // Event FIFO is not Full
}

template <typename CONFIG>
void M_CAN<CONFIG>::StoreRx(sc_dt::uint64 addr, unsigned int element_size, const M_CAN_Message& msg, bool match, unsigned int filter_index)
{
	Rx_Buffer_FIFO_Element rx_buffer_fifo_element;
	rx_buffer_fifo_element.element_size = element_size;
	memset(&rx_buffer_fifo_element.r[0], 0, 4 * element_size);
	Rx_Buffer_FIFO_Element::R0::ESI::template Set<uint32_t>(rx_buffer_fifo_element.r[0], msg.get_error_state_indicator());
	Rx_Buffer_FIFO_Element::R0::XTD::template Set<uint32_t>(rx_buffer_fifo_element.r[0], msg.is_identifier_extended());
	Rx_Buffer_FIFO_Element::R0::RTR::template Set<uint32_t>(rx_buffer_fifo_element.r[0], msg.is_remote_transmission_request());
	Rx_Buffer_FIFO_Element::R0::ID::template Set<uint32_t>(rx_buffer_fifo_element.r[0], msg.get_identifier());
	Rx_Buffer_FIFO_Element::R1::ANMF::template Set<uint32_t>(rx_buffer_fifo_element.r[1], !match);
	Rx_Buffer_FIFO_Element::R1::FIDX::template Set<uint32_t>(rx_buffer_fifo_element.r[1], filter_index);
	Rx_Buffer_FIFO_Element::R1::EDL::template Set<uint32_t>(rx_buffer_fifo_element.r[1], msg.is_fd());
	Rx_Buffer_FIFO_Element::R1::BRS::template Set<uint32_t>(rx_buffer_fifo_element.r[1], msg.get_bit_rate_switch());
	Rx_Buffer_FIFO_Element::R1::DLC::template Set<uint32_t>(rx_buffer_fifo_element.r[1], msg.get_data_length_code());
	Rx_Buffer_FIFO_Element::R1::RXTS::template Set<uint32_t>(rx_buffer_fifo_element.r[1], msg.GetTimeStamp());
	
	unsigned int data_field_size = 4 * (element_size - 2);
	unsigned int data_length = msg.get_data_length();
	if(data_length > 0)
	{
		msg.get_data((uint8_t *) &rx_buffer_fifo_element.r[2], (data_length <= data_field_size) ? data_length : data_field_size); // Note: if data in message exceeds data field size crop data from message
	}
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":storing Rx Buffer FIFO Element " << rx_buffer_fifo_element << " at @0x" << std::hex << addr << std::dec << EndDebugInfo;
	}
	
	rx_buffer_fifo_element.r[0] = unisim::util::endian::Host2Target(endian, rx_buffer_fifo_element.r[0]);
	rx_buffer_fifo_element.r[1] = unisim::util::endian::Host2Target(endian, rx_buffer_fifo_element.r[1]);
	if(endian != unisim::util::endian::E_BIG_ENDIAN)
	{
		for(unsigned int i = 2; i < element_size; i++)
		{
			unisim::util::endian::BSwap(rx_buffer_fifo_element.r[i]);
		}
	}
	
	WriteWords(addr, &rx_buffer_fifo_element.r[0], element_size);
}

template <typename CONFIG>
void M_CAN<CONFIG>::StoreRxFIFO(unsigned int fifo_id, const M_CAN_Message& msg, bool match, unsigned int filter_index, bool set_priority)
{
	assert(fifo_id < 2);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":storing message " << msg << " in Rx FIFO #" << fifo_id << EndDebugInfo;
	}
	
	unsigned int rx_fifo_size = GetRxFIFOSize(fifo_id);
	bool rx_fifo_full = fifo_id ? rxf1s.template Get<typename RXF1S::F1F>() : rxf0s.template Get<typename RXF0S::F0F>();
	
	if(rx_fifo_size == 0)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":No Rx FIFO #" << fifo_id << EndDebugInfo;
		}
	}
	
	bool rx_fifo_overwrite_mode = fifo_id ? rxf1c.template Get<typename RXF1C::F1OM>() : rxf0c.template Get<typename RXF0C::F0OM>();
	
	if((rx_fifo_size == 0) || (rx_fifo_full && !rx_fifo_overwrite_mode))
	{
		// Rx FIFO is of size zero or Rx FIFO is full and in blocking mode
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":Rx FIFO #" << fifo_id << " overrun, a message is lost" << EndDebugInfo;
		}

		if(set_priority)
		{
			SetPriority(msg.is_identifier_extended(), filter_index, MSI_FIFO_OVERRUN);
		}
		
		if(fifo_id)
		{
			ir.template Set<typename IR::RF1L>(1); // Rx FIFO #1 Message Lost
		}
		else
		{
			ir.template Set<typename IR::RF0L>(1); // Rx FIFO #0 Message Lost
		}
	}
	else // ((rx_fifo_size != 0) && (!rx_fifo_full || rx_fifo_overwrite_mode))
	{
		// Rx FIFO has non-zero size and Rx FIFO is either not full or in overwrite mode
		
		if(unlikely(verbose))
		{
			if(rx_fifo_full)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":overwriting oldest element in Rx FIFO #" << fifo_id << EndDebugInfo;
			}
		}
		
		unsigned int rx_fifo_put_index = fifo_id ? rxf1s.template Get<typename RXF1S::F1PI>() : rxf0s.template Get<typename RXF0S::F0PI>();
		unsigned int rx_fifo_get_index = fifo_id ? rxf1s.template Get<typename RXF1S::F1GI>() : rxf0s.template Get<typename RXF0S::F0GI>();
		unsigned int rx_fifo_watermark = fifo_id ? rxf1c.template Get<typename RXF1C::F1WM>() : rxf0c.template Get<typename RXF0C::F0WM>();
		
		if(set_priority)
		{
			SetPriority(msg.is_identifier_extended(), filter_index, fifo_id ? MSI_MESSAGE_STORE_IN_FIFO_1 : MSI_MESSAGE_STORE_IN_FIFO_0, rx_fifo_put_index);
		}
		
		unsigned int rx_fifo_start_addr = fifo_id ? rxf1c.template Get<typename RXF1C::F1SA>() : rxf0c.template Get<typename RXF0C::F0SA>();
		unsigned int fds = fifo_id ? rxesc.template Get<typename RXESC::F1DS>() : rxesc.template Get<typename RXESC::F0DS>();
		unsigned int rx_fifo_data_field_size = (fds <= 4) ? (8 + (fds * 4)) : (16 + ((fds & 3) * 16)); // in bytes
		unsigned int rx_fifo_element_size = 2 + (rx_fifo_data_field_size / 4); // in words
		sc_dt::uint64 rx_fifo_element_addr = rx_fifo_start_addr + (rx_fifo_put_index * rx_fifo_element_size);

		StoreRx(rx_fifo_element_addr, rx_fifo_element_size, msg, match, filter_index);
		
		rx_fifo_put_index = rx_fifo_put_index + 1;                   // Increment Put Index
		if(rx_fifo_put_index >= rx_fifo_size) rx_fifo_put_index = 0; // Wrap around
		if(fifo_id)
		{
			rxf1s.template Set<typename RXF1S::F1PI>(rx_fifo_put_index); // Commit Rx FIFO #1 Put Index
		}
		else
		{
			rxf0s.template Set<typename RXF0S::F0PI>(rx_fifo_put_index); // Commit Rx FIFO #0 Put Index
		}
		
		if(rx_fifo_full)
		{
			// Rx FIFO was already full: we've just overwriting oldest element 
			assert(rx_fifo_overwrite_mode);
			
			unsigned int rx_fifo_get_index = fifo_id ? rxf1s.template Get<typename RXF1S::F1GI>() : rxf0s.template Get<typename RXF0S::F0GI>();
			rx_fifo_get_index = rx_fifo_get_index + 1;                   // Increment Get Index
			if(rx_fifo_get_index >= rx_fifo_size) rx_fifo_get_index = 0; // Wrap around
			if(fifo_id)
			{
				rxf1s.template Set<typename RXF1S::F1GI>(rx_fifo_get_index); // Commit Rx FIFO #1 Get Index
			}
			else
			{
				rxf0s.template Set<typename RXF0S::F0GI>(rx_fifo_get_index); // Commit Rx FIFO #0 Get Index
			}
		}
		
		// Compute Rx FIFO Fill Level
		unsigned int rx_fifo_fill_level = (rx_fifo_get_index >= rx_fifo_put_index) ? (rx_fifo_size - rx_fifo_get_index + rx_fifo_put_index)
		                                                                           : (rx_fifo_put_index - rx_fifo_get_index);
		
		// Determine whether Rx FIFO is full
		bool rx_fifo_full = (rx_fifo_put_index == rx_fifo_get_index);
		
		// Determine whether watermark interrupt is enabled
		bool watermark_interrupt_enabled = (rx_fifo_watermark > 0) && (rx_fifo_watermark <= MAX_RX_FIFO_WATERMARK);
		
		// Determine whether watermark is reached
		bool watermark_reached = (rx_fifo_fill_level == rx_fifo_watermark);
		
		if(unlikely(verbose))
		{
			if(rx_fifo_full)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":Rx FIFO #" << fifo_id << " is full" << EndDebugInfo;
			}
		}

		if(fifo_id)
		{
			rxf1s.template Set<typename RXF1S::F1FL>(rx_fifo_fill_level);
			rxf1s.template Set<typename RXF1S::F1F>(rx_fifo_full);
		}
		else
		{
			rxf0s.template Set<typename RXF0S::F0FL>(rx_fifo_fill_level);
			rxf0s.template Set<typename RXF0S::F0F>(rx_fifo_full);
		}

		if(fifo_id)
		{
			if(watermark_interrupt_enabled && watermark_reached)
			{
				ir.template Set<typename IR::RF1W>(1); // Rx FIFO #1 Watermark Reached
			}

			if(rx_fifo_full)
			{
				ir.template Set<typename IR::RF1F>(1); // Rx FIFO #1 is full
			}
			
			ir.template Set<typename IR::RF1N>(1); // Rx FIFO #1 New Message
		}
		else
		{
			if(watermark_interrupt_enabled && watermark_reached)
			{
				ir.template Set<typename IR::RF0W>(1); // Rx FIFO #0 Watermark Reached
			}

			if(rx_fifo_full)
			{
				ir.template Set<typename IR::RF0F>(1); // Rx FIFO #0 is full
			}

			ir.template Set<typename IR::RF0N>(1); // Rx FIFO #0 New Message
		}
	}
	
	UpdateInterrupts();
}

template <typename CONFIG>
void M_CAN<CONFIG>::AcknowledgeRxFIFO(unsigned int fifo_id)
{
	unsigned int rx_fifo_ack_index = fifo_id ? rxf1a.template Get<typename RXF1A::F1AI>() : rxf0a.template Get<typename RXF0A::F0AI>();
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":Rx FIFO #" << fifo_id << " acknowledged up to Element #" << rx_fifo_ack_index << EndDebugInfo; 
	}
	
	unsigned int rx_fifo_size = GetRxFIFOSize(fifo_id);
	
	if(rx_fifo_size == 0)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ":Acknowledging a Rx FIFO element while there's no Rx FIFO #" << fifo_id << EndDebugWarning;
		}
		return;
	}

	if(rx_fifo_ack_index >= rx_fifo_size)
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Rx FIFO element which is outside of Rx FIFO #" << fifo_id << EndDebugWarning;
		return;
	}
	
	unsigned int rx_fifo_put_index = fifo_id ? rxf1s.template Get<typename RXF1S::F1PI>() : rxf0s.template Get<typename RXF0S::F0PI>();
	unsigned int rx_fifo_get_index = fifo_id ? rxf1s.template Get<typename RXF1S::F1GI>() : rxf0s.template Get<typename RXF0S::F0GI>();
	
	if((rx_fifo_ack_index >= rx_fifo_put_index) && (rx_fifo_ack_index < rx_fifo_get_index))
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":acknowledging a Rx FIFO element which is freed in Rx FIFO #" << fifo_id << EndDebugWarning;
		return;
	}
	
	rx_fifo_get_index = rx_fifo_ack_index + 1;                   // New Get Index is M_CAN_RXF1A[F1AI] + 1
	if(rx_fifo_get_index >= rx_fifo_size) rx_fifo_get_index = 0; // Wrap around
	
	if(fifo_id)
	{
		rxf1s.template Set<typename RXF1S::F1GI>(rx_fifo_get_index); // Set Rx FIFO #1 Get Index to M_CAN_RXF1A[F1AI] + 1
	}
	else
	{
		rxf0s.template Set<typename RXF0S::F0GI>(rx_fifo_get_index); // Set Rx FIFO #0 Get Index to M_CAN_RXF0A[F0AI] + 1
	}
	
	// Determine Fill level of Rx FIFO
	unsigned int rx_fifo_fill_level = (rx_fifo_get_index > rx_fifo_put_index) ? (rx_fifo_size - rx_fifo_get_index + rx_fifo_put_index)
	                                                                          : (rx_fifo_put_index - rx_fifo_get_index);
	if(fifo_id)
	{
		rxf1s.template Set<typename RXF1S::F1FL>(rx_fifo_fill_level);  // Rx FIFO #1 fill level
		rxf1s.template Set<typename RXF1S::F1F>(0);                    // Rx FIFO #1 is not full
	}
	else
	{
		rxf0s.template Set<typename RXF0S::F0FL>(rx_fifo_fill_level);  // Rx FIFO #1 fill level
		rxf0s.template Set<typename RXF0S::F0F>(0);                    // Rx FIFO #1 is not full
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::StoreRxBuffer(unsigned int rx_buffer_element_index, const M_CAN_Message& msg, bool match, unsigned int filter_index)
{
	assert(rx_buffer_element_index < 64);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":storing message " << msg << " in Rx Buffer #" << rx_buffer_element_index << EndDebugInfo;
	}

	unsigned int rx_buffers_start_addr = rxbc.template Get<typename RXBC::RBSA>(); // Rx Buffer Start Address
	unsigned int rbds = rxesc.template Get<typename RXESC::RBDS>();
	unsigned int rx_buffer_data_field_size = (rbds <= 4) ? (8 + (rbds * 4)) : (16 + ((rbds & 3) * 16)); // in bytes
	unsigned int rx_buffer_element_size = 2 + (rx_buffer_data_field_size / 4); // in words
	sc_dt::uint64 rx_buffer_addr = rx_buffers_start_addr + (rx_buffer_element_index * rx_buffer_element_size);
	
	StoreRx(rx_buffer_addr, rx_buffer_element_size, msg, match, filter_index);
	
	if(rx_buffer_element_index < 32)
	{
		ndat1.NewData(rx_buffer_element_index);
	}
	else
	{
		ndat2.NewData(rx_buffer_element_index - 32);
	}

	ir.template Set<typename IR::DRX>(1);
	UpdateInterrupts();
}

template <typename CONFIG>
void M_CAN<CONFIG>::SetPriority(bool xtd, unsigned int filter_index, MESSAGE_STORAGE_INDICATOR msi, unsigned int buffer_index)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":setting high priority message" << EndDebugInfo;
	}
	
	hpms.template Set<typename HPMS::FLST>(xtd);          // Filter List: Standard or Extended Filter List
	hpms.template Set<typename HPMS::FIDX>(filter_index); // Filter Index
	hpms.template Set<typename HPMS::MSI>(msi);           // Message Storage Indicator
	hpms.template Set<typename HPMS::BIDX>(buffer_index); // Buffer Index
	
	ir.template Set<typename IR::HPM>(1); // High Priority Message
	UpdateInterrupts();
}

template <typename CONFIG>
void M_CAN<CONFIG>::GenerateFilterEventPulse(uint32_t filter_event)
{
	unsigned int i;
	
	for(i = 0; i < NUM_FILTER_EVENTS; i++)
	{
		if(filter_event & (1 << i))
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":generating a pulse for filter event #" << i << EndDebugInfo;
			}
		
			gen_filter_event_pos[i] = true;
			gen_filter_event_pulse_event[i].notify(sc_core::SC_ZERO_TIME);
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::Filter(const M_CAN_Message& msg)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":filtering message " << msg << EndDebugInfo;
	}
	
	bool xtd = rx_msg.is_identifier_extended();
	bool rtr = rx_msg.is_remote_transmission_request();
	
	if(xtd)
	{
		// extended ID
		if(rtr && gfc.template Get<typename GFC::RRFE>()) // Reject Remote Frames Extended?
		{
			// reject remote frame with extended IDs
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":rejecting message " << EndDebugInfo;
			}
			return;
		}
	}
	else
	{
		// standard ID
		if(rtr && gfc.template Get<typename GFC::RRFS>()) // Reject Remote Frames Standard?
		{
			// reject remote frame with standard IDs
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":rejecting message " << msg << EndDebugInfo;
			}
			return;
		}
	}
	
	uint32_t id_28_0 = rx_msg.get_identifier();
	uint32_t id = xtd ? id_28_0 : (id_28_0 >> (Super::ID_LENGTH - Super::STD_FMT_ID_LENGTH));
	uint32_t fid1 = 0;
	uint32_t fid2 = 0;
	FILTER_TYPE ft = RANGE_FILTER;
	uint32_t fec = 0;
	uint32_t rx_buffer_debug_message_selector = 0; // FID2[10:9]
	uint32_t filter_event = 0;                     // FID2[8:6]
	uint32_t rx_buffer_element_index = 0;          // FID2[5:0]
	uint32_t list_size = 0;
	uint32_t filter_list_start_addr = 0;
	
	union
	{
		Standard_Message_ID_Filter_Element std_msg_id_filter_elements[MAX_STD_MSG_ID_FILTER_ELEMENTS];
		Extended_Message_ID_Filter_Element xtd_msg_id_filter_elements[MAX_XTD_MSG_ID_FILTER_ELEMENTS];
		uint32_t w[MAX_STD_MSG_ID_FILTER_ELEMENTS];
	}
	filter_elements;
	
	if(xtd)
	{
		// extended ID
		unsigned int lse = xidfc.template Get<typename XIDFC::LSE>(); // List Size Extended
		list_size = (lse < MAX_XTD_MSG_ID_FILTER_ELEMENTS) ? lse : MAX_XTD_MSG_ID_FILTER_ELEMENTS;
		filter_list_start_addr = xidfc.template Get<typename XIDFC::FLESA>(); // Filter List Extended Start Address
		
		uint32_t eidm = xidam.template Get<typename XIDAM::EIDM>(); // Extended ID Mask
		id = id & eidm; // Mask ID
		
		ReadWords(filter_list_start_addr, &filter_elements.w[0], 2 * list_size);
	}
	else
	{
		// standard ID
		unsigned lss = sidfc.template Get<typename SIDFC::LSS>(); // List Size Standard
		list_size = (lss < MAX_STD_MSG_ID_FILTER_ELEMENTS) ? lss : MAX_STD_MSG_ID_FILTER_ELEMENTS;
		filter_list_start_addr = sidfc.template Get<typename SIDFC::FLSSA>();
		
		ReadWords(filter_list_start_addr, &filter_elements.w[0], list_size);
	}
	
	// Matching
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":trying to match ID=0x" << std::hex << id << std::dec << EndDebugInfo;
	
		if(list_size)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":filter list has " << list_size << " filter elements" << EndDebugInfo;
		}
		else
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":filter list is empty" << EndDebugInfo;
		}
	}
	
	bool match = false;
	unsigned int filter_index;
	for(filter_index = 0, match = false; (filter_index < list_size) && !match; filter_index = match ? filter_index : (filter_index + 1))
	{
		if(xtd)
		{
			uint32_t f0 = filter_elements.xtd_msg_id_filter_elements[filter_index].f0 = unisim::util::endian::Target2Host(endian, filter_elements.xtd_msg_id_filter_elements[filter_index].f0);
			
			fec = Extended_Message_ID_Filter_Element::F0::EFEC::template Get<uint32_t>(f0);
			
			if(fec != FEC_DISABLE_FILTER_ELEMENT) // filter element enabled?
			{
				fid1 = Extended_Message_ID_Filter_Element::F0::EFID1::template Get<uint32_t>(f0);
				
				uint32_t f1 = filter_elements.xtd_msg_id_filter_elements[filter_index].f1 = unisim::util::endian::Target2Host(endian, filter_elements.xtd_msg_id_filter_elements[filter_index].f1);
			
				if(fec == FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE) // store into Rx buffer or debug message?
				{
					rx_buffer_debug_message_selector = Extended_Message_ID_Filter_Element::F1::EFID2_10_9::template Get<uint32_t>(f1);
					filter_event  = Extended_Message_ID_Filter_Element::F1::EFID2_8_6 ::template Get<uint32_t>(f1);
					rx_buffer_element_index = Extended_Message_ID_Filter_Element::F1::EFID2_5_0 ::template Get<uint32_t>(f1);
				}
				else
				{
					ft = FILTER_TYPE(Extended_Message_ID_Filter_Element::F1::EFT::template Get<uint32_t>(f1));
					fid2 = Extended_Message_ID_Filter_Element::F1::EFID2::template Get<uint32_t>(f1);
				}
			}
		}
		else
		{
			uint32_t s0 = filter_elements.std_msg_id_filter_elements[filter_index].s0 = unisim::util::endian::Target2Host(endian, filter_elements.std_msg_id_filter_elements[filter_index].s0);
		
			fec = Standard_Message_ID_Filter_Element::S0::SFEC::template Get<uint32_t>(s0);
			
			if(fec != FEC_DISABLE_FILTER_ELEMENT) // filter element enabled?
			{
				fid1 = Standard_Message_ID_Filter_Element::S0::SFID1::template Get<uint32_t>(s0);
				
				if(fec == FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE) // store into Rx buffer or debug message?
				{
					rx_buffer_debug_message_selector = Standard_Message_ID_Filter_Element::S0::SFID2_10_9::template Get<uint32_t>(s0);
					filter_event  = Standard_Message_ID_Filter_Element::S0::SFID2_8_6 ::template Get<uint32_t>(s0);
					rx_buffer_element_index  = Standard_Message_ID_Filter_Element::S0::SFID2_5_0 ::template Get<uint32_t>(s0);
				}
				else
				{
					ft = FILTER_TYPE(Standard_Message_ID_Filter_Element::S0::SFT::template Get<uint32_t>(s0));
					fid2 = Standard_Message_ID_Filter_Element::S0::SFID2::template Get<uint32_t>(s0);
				}
			}
		}
		
		if(fec != FEC_DISABLE_FILTER_ELEMENT) // filter element enabled?
		{
			if(fec == FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE) // store into Rx buffer or debug message?
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":filter #" << filter_index << ":executing a exact matching filter with no masking mechanism and FID1=0x" << std::hex << fid1 << std::dec << EndDebugInfo;
				}
				
				match = (id == fid1); // exact match with no masking mechanism
			}
			else
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":filter #" << filter_index << ":Executing a " << ft << " with FID1=0x" << std::hex << fid1 << std::dec << " and FID2=0x" << std::hex << fid2 << std::dec << EndDebugInfo;
				}
				
				switch(ft)
				{
					case RANGE_FILTER: // range filter
						match = (id >= fid1) && (id <= fid2);
						break;
						
					case DUAL_ID_FILTER: // dual ID filter
						match = (id == fid1) || (id == fid2);
						break;
						
					case CLASSIC_FILTER: // classic filter
						match = (id & fid2) == (id & fid2);
						break;
						
					default: // reserved
						break;
				}
			}
		}
		else
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":filter #" << filter_index << ":disabled" << EndDebugInfo;
			}
		}
	}
	
	if(unlikely(verbose))
	{
		if(match)
		{
			if(xtd)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":message " << msg << " matches filter element #" << filter_index << " " << filter_elements.xtd_msg_id_filter_elements[filter_index] << EndDebugInfo;
			}
			else
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":message " << msg << " matches filter element #" << filter_index << " " << filter_elements.std_msg_id_filter_elements[filter_index] << EndDebugInfo;
			}
		}
		else
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":message " << msg << " does not match any filter element" << EndDebugInfo;
		}
	}

	// Acceptance
	bool accept = false;
	
	if(match)
	{
		if(fec == FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE)
		{
			switch(rx_buffer_debug_message_selector)
			{
				case SEL_STORE_MESSAGE_IN_AN_RX_BUFFER: // 00b: store message into an Rx Buffer
					accept = true;
					break;
					
				case SEL_DEBUG_MESSAGE_A: // 01b: Debug Message A
					accept = DebugMessageHandlingFSM(DBG_MSG_A);
					break;
					
				case SEL_DEBUG_MESSAGE_B: // 10b: Debug Message B
					accept = DebugMessageHandlingFSM(DBG_MSG_B);
					break;
					
				case SEL_DEBUG_MESSAGE_C: // 11b: Debug Message C
					accept = DebugMessageHandlingFSM(DBG_MSG_C);
					break;
			}
			GenerateFilterEventPulse(filter_event);
		}
		else
		{
			accept = (fec != FEC_DISABLE_FILTER_ELEMENT);
		}
	}
	else if(xtd)
	{
		accept = ((gfc.template Get<typename GFC::ANFE>() & 2) == 0); // Accept Non-matching Frames Extended
	}
	else
	{
		accept = ((gfc.template Get<typename GFC::ANFS>() & 2) == 0); // Accept Non-matching Frames Standard
	}
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":" << (accept ? "accepting" : "rejecting") << " message " << msg << EndDebugInfo;
	}
	
	// Storing
	if(accept)
	{
		if(match)
		{
			// Accept matching frame
			switch(fec)
			{
				case FEC_DISABLE_FILTER_ELEMENT: // 000b: disable filter element
					// nothing to do
					break;
					
				case FEC_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES: // 001b: store in Rx FIFO 0
					StoreRxFIFO(0, msg, match, filter_index);
					break;
					
				case FEC_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES: // 010b: store in Rx FIFO 1
					StoreRxFIFO(1, msg, match, filter_index);
					break;
					
				case FEC_SET_PRIORITY_IF_FILTER_MATCHES: // 100b: set priority
					SetPriority(xtd, filter_index, MSI_NO_FIFO_SELECTED);
					break;
					
				case FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_0_IF_FILTER_MATCHES: // 101b: set priority and store in FIFO 0
					StoreRxFIFO(0, msg, match, filter_index, true);
					break;
				
				case FEC_SET_PRIORITY_AND_STORE_IN_RX_FIFO_1_IF_FILTER_MATCHES: // 110b: set priority and store in FIFO 1
					StoreRxFIFO(1, msg, match, filter_index, true);
					break;
					
				case FEC_STORE_INTO_RX_BUFFER_OR_AS_DEBUG_MESSAGE: // 111b: store into Rx buffer or debug message
					switch(rx_buffer_debug_message_selector)
					{
						case SEL_STORE_MESSAGE_IN_AN_RX_BUFFER: // 00b: store message into an Rx Buffer
							StoreRxBuffer(rx_buffer_element_index, msg, match, filter_index);
							break;
							
						case SEL_DEBUG_MESSAGE_A: // 01b: Debug Message A
							StoreRxBuffer(0, msg, match, filter_index);
							break;
							
						case SEL_DEBUG_MESSAGE_B: // 10b: Debug Message B
							StoreRxBuffer(1, msg, match, filter_index);
							break;
							
						case SEL_DEBUG_MESSAGE_C: // 11b: Debug Message C
							StoreRxBuffer(2, msg, match, filter_index);
							break;
					}
					break;
			}
		}
		else
		{
			// Accepting Non-matching frame
			if((xtd && ((gfc.template Get<typename GFC::ANFE>() & 1) == 0)) || (!xtd && (gfc.template Get<typename GFC::ANFS>() & 1) == 0))
			{
				StoreRxFIFO(0, msg, match);
			}
			else
			{
				StoreRxFIFO(1, msg, match);
			}
		}
	}
}

template <typename CONFIG>
bool M_CAN<CONFIG>::DebugMessageHandlingFSM(DEBUG_MESSAGE_TYPE dbg_msg_type)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":received " << dbg_msg_type << EndDebugInfo;
	}
	
	bool accept = true;
	bool state_updated = false;
	switch(rxf1s.template Get<typename RXF1S::DMS>()) // Debug Message Status
	{
		case DMS_IDLE: // idle state, wait for reception of debug messages, DMA request is cleared
			if(dbg_msg_type == DBG_MSG_A) // A
			{
				rxf1s.template Set<typename RXF1S::DMS>(DMS_DEBUG_MESSAGE_A_RECEIVED);
				state_updated = true;
			}
			break;
			
		case DMS_DEBUG_MESSAGE_A_RECEIVED: // Debug Message A received
			if(dbg_msg_type == DBG_MSG_B) // B
			{
				rxf1s.template Set<typename RXF1S::DMS>(DMS_DEBUG_MESSAGES_A_B_RECEIVED);
				state_updated = true;
			}
			else if(dbg_msg_type == DBG_MSG_C) // C
			{
				rxf1s.template Set<typename RXF1S::DMS>(DMS_IDLE);
				state_updated = true;
			}
			break;
			
		case DMS_DEBUG_MESSAGES_A_B_RECEIVED: // Debug Messages A, B received
			if(dbg_msg_type == DBG_MSG_C) // C
			{
				rxf1s.template Set<typename RXF1S::DMS>(DMS_DEBUG_MESSAGES_A_B_C_RECEIVED);
				state_updated = true;
				UpdateDMA_REQ();
			}
			else // A, B
			{
				rxf1s.template Set<typename RXF1S::DMS>(DMS_IDLE);
				state_updated = true;
			}
			break;
			
		case DMS_DEBUG_MESSAGES_A_B_C_RECEIVED: // Debug Messages A, B, C received, DMA request is set
			accept = false; // reject
			break;
	}
	
	if(state_updated)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":" << DEBUG_MESSAGE_STATUS(rxf1s.template Get<typename RXF1S::DMS>()) << EndDebugInfo;
	}
	
	return accept;
}

template <typename CONFIG>
void M_CAN<CONFIG>::TxScan()
{
	uint32_t transmission_request_pending = txbrp.template Get<typename TXBRP::TRP>();
	uint32_t curr_tx_msg_mask = curr_tx_msg ? (uint32_t(1) << curr_tx_msg->GetBufferIndex()) : 0;
	transmission_request_pending = transmission_request_pending & ~curr_tx_msg_mask;
	
	if(!transmission_request_pending)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "No pending transmission requests" << EndDebugInfo;
		}
		return;
	}
	
	unsigned int num_dedicated_tx_buffers = GetNumDedicatedTxBuffers();
	unsigned int num_tx_buffers = GetNumTxBuffers();
	unsigned int tx_buffers_start_addr = txbc.template Get<typename TXBC::TBSA>(); // word address
	unsigned int tbds = txesc.template Get<typename TXESC::TBDS>();
	unsigned int tx_buffer_data_field_size = (tbds <= 4) ? (8 + (tbds * 4)) : (16 + ((tbds & 3) * 16)); // in bytes
	unsigned int tx_buffer_element_size = 2 + (tx_buffer_data_field_size / 4); // in words
	unsigned int tx_fifo_queue_mode = txbc.template Get<typename TXBC::TFQM>(); 
	
	bool hit = false;
	unsigned int tx_buffer_element_index = 0;
	uint32_t tx_buffer_element_identifier = ~uint32_t(0);
	Tx_Buffer_Element tx_buffer_element;
	tx_buffer_element.element_size = tx_buffer_element_size;
	
	unsigned int tx_buffer_scan_index;
	if(unisim::util::arithmetic::BitScanForward(tx_buffer_scan_index, transmission_request_pending) && (tx_buffer_scan_index < ((tx_fifo_queue_mode == TFQM_FIFO) ? num_dedicated_tx_buffers : num_tx_buffers)))
	{
		hit = true;
		
		do
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Pending transmission request for Tx Buffer Element #" << tx_buffer_scan_index << EndDebugInfo;
			}
		
			sc_dt::uint64 tx_buffer_addr = tx_buffers_start_addr + (tx_buffer_scan_index * tx_buffer_element_size); // word address
			
			uint32_t t0 = 0;
			ReadWords(tx_buffer_addr, &t0, 1);
			t0 = unisim::util::endian::Target2Host(endian, t0);

			uint32_t identifier = Tx_Buffer_Element::T0::XTD::template Get<uint32_t>(t0) ? Tx_Buffer_Element::T0::ID::template Get<uint32_t>(t0)
			                                                                             : (Tx_Buffer_Element::T0::ID::template Get<uint32_t>(t0) & (~uint32_t(0) << (Super::ID_LENGTH - Super::STD_FMT_ID_LENGTH)));
			
			if(identifier < tx_buffer_element_identifier)
			{
				tx_buffer_element_identifier = identifier;
				tx_buffer_element.t[0] = t0;
				tx_buffer_element_index = tx_buffer_scan_index;
			}
			
			transmission_request_pending &= ~(uint32_t(1) << tx_buffer_scan_index); // clear pending request
		}
		while(unisim::util::arithmetic::BitScanForward(tx_buffer_scan_index, transmission_request_pending) && (tx_buffer_scan_index < ((tx_fifo_queue_mode == TFQM_FIFO) ? num_dedicated_tx_buffers : num_tx_buffers)));
	}
	
	if(tx_fifo_queue_mode == TFQM_FIFO)
	{
		unsigned int tqfs = GetTxFIFOQueueSize();   // Tx FIFO Size
		unsigned int tffl = txfqs.template Get<typename TXFQS::TFFL>(); // Tx FIFO Free Level
		
		if(tffl != tqfs) // TX FIFO not empty?
		{
			unsigned int tx_fifo_get_index = txfqs.template Get<typename TXFQS::TFGI>();
			if(transmission_request_pending & (uint32_t(1) << tx_fifo_get_index))
			{
				hit = true;
				
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Pending transmission request for Tx Buffer Element #" << tx_buffer_scan_index << " from Tx FIFO" << EndDebugInfo;
				}
				
				sc_dt::uint64 tx_buffer_element_addr = tx_buffers_start_addr + (tx_fifo_get_index * tx_buffer_element_size);
				
				uint32_t t0 = 0;
				ReadWords(tx_buffer_element_addr, &t0, 1);
				t0 = unisim::util::endian::Target2Host(endian, t0);
				uint32_t identifier = Tx_Buffer_Element::T0::XTD::template Get<uint32_t>(t0) ? Tx_Buffer_Element::T0::ID::template Get<uint32_t>(t0)
				                                                                             : (Tx_Buffer_Element::T0::ID::template Get<uint32_t>(t0) & (~uint32_t(0) << (Super::ID_LENGTH - Super::STD_FMT_ID_LENGTH)));
				
				if(identifier < tx_buffer_element_identifier)
				{
					tx_buffer_element_identifier = identifier;
					tx_buffer_element.t[0] = t0;
					tx_buffer_element_index = tx_fifo_get_index;
				}
			}
		}
	}
	
	if(hit)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Tx scan hits Tx Buffer Element #" << tx_buffer_element_index << EndDebugInfo;
		}
		
		sc_dt::uint64 tx_buffer_element_addr = tx_buffers_start_addr + (tx_buffer_element_index * tx_buffer_element_size);
		
		ReadWords(tx_buffer_element_addr + 1, &tx_buffer_element.t[1], tx_buffer_element_size - 1);
		tx_buffer_element.t[1] = unisim::util::endian::Target2Host(endian, tx_buffer_element.t[1]);
		
		if(unlikely(verbose))
		{
			for(unsigned int i = 2; i < tx_buffer_element_size; i++)
			{
				tx_buffer_element.t[i] = unisim::util::endian::Target2Host(endian, tx_buffer_element.t[i]); 
			}
			logger << DebugInfo << "Loaded Tx Buffer Element " << tx_buffer_element << " at @0x" << std::hex << tx_buffer_element_addr << std::dec << EndDebugInfo;
			for(unsigned int i = 2; i < tx_buffer_element_size; i++)
			{
				tx_buffer_element.t[i] = unisim::util::endian::Host2Target(endian, tx_buffer_element.t[i]); 
			}
		}
		
		tlm_can_format fmt = Tx_Buffer_Element::T0::XTD::template Get<uint32_t>(tx_buffer_element.t[0]) ? TLM_CAN_XTD_FMT : TLM_CAN_STD_FMT;
		bool remote_transmission_request = Tx_Buffer_Element::T0::RTR::template Get<uint32_t>(tx_buffer_element.t[0]);
		unsigned int message_marker = Tx_Buffer_Element::T1::MM::template Get<uint32_t>(tx_buffer_element.t[1]);
		bool event_fifo_control = Tx_Buffer_Element::T1::EFC::template Get<uint32_t>(tx_buffer_element.t[1]);
		unsigned int data_length_code = Tx_Buffer_Element::T1::DLC::template Get<uint32_t>(tx_buffer_element.t[1]);
		if(endian != unisim::util::endian::E_BIG_ENDIAN)
		{
			for(unsigned int i = 2; i < tx_buffer_element_size; i++)
			{
				unisim::util::endian::BSwap(tx_buffer_element.t[i]);
			}
		}
		
		M_CAN_Message& new_can_msg = tx_msg_pipe[flip_flop ^= 1]; // pick one of the two CAN messages in double buffer
		
		new_can_msg.Reset();
		new_can_msg.set_format(fmt);
		new_can_msg.set_identifier(tx_buffer_element_identifier);
		new_can_msg.set_remote_transmission_request(remote_transmission_request);
		new_can_msg.set_data_length_code(data_length_code);
		
		unsigned int data_length = new_can_msg.get_data_length();
		new_can_msg.set_data((uint8_t *) &tx_buffer_element.t[2], tx_buffer_data_field_size);
		if(data_length > tx_buffer_data_field_size)
		{
			// padding
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Padding message data field" << EndDebugInfo;
			}
			for(unsigned int i = tx_buffer_data_field_size; i < data_length; i++)
			{
				new_can_msg.set_data_byte_at(i, 0xcc);
			}
		}
		
		new_can_msg.SetBufferIndex(tx_buffer_element_index);
		new_can_msg.SetEFC(event_fifo_control);
		new_can_msg.SetMessageMarker(message_marker);
		
		pending_tx_msg = &new_can_msg;
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Tx scan has prepared message " << (*pending_tx_msg) << " for further transmission by CAN core" << EndDebugInfo;
		}
	}
	else
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Tx scan misses" << EndDebugInfo;
		}
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateInterrupts()
{
	gen_int0_event.notify(sc_core::SC_ZERO_TIME);
	gen_int1_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void M_CAN<CONFIG>::UpdateDMA_REQ()
{
	gen_dma_req_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void M_CAN<CONFIG>::INT0_Process()
{
	bool eint0 = ile.template Get<typename ILE::EINT0>();
	bool int0 = eint0 && ((ie & ~ils & ir) != 0);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT0.name() << " <- " << int0 << EndDebugInfo;
	}
	
	INT0 = int0;
}

template <typename CONFIG>
void M_CAN<CONFIG>::INT1_Process()
{
	bool eint1 = ile.template Get<typename ILE::EINT1>();
	bool int1 = eint1 && ((ie & ils & ir) != 0);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT1.name() << " <- " << int1 << EndDebugInfo;
	}
	
	INT1 = int1;
}

template <typename CONFIG>
void M_CAN<CONFIG>::DMA_REQ_Process()
{
	bool dma_req = (rxf1s.template Get<typename RXF1S::DMS>() == DMS_DEBUG_MESSAGES_A_B_C_RECEIVED);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_REQ.name() << " <- " << dma_req << EndDebugInfo;
	}
	
	DMA_REQ = dma_req;
}

template <typename CONFIG>
void M_CAN<CONFIG>::DMA_ACK_Process()
{
	if(DMA_ACK.posedge())
	{
		rxf1s.template Set<typename RXF1S::DMS>(DMS_IDLE); // reset Debug Message Handling FSM
		UpdateDMA_REQ();
	}
}

template <typename CONFIG>
void M_CAN<CONFIG>::FilterEventProcess(unsigned int filter_event_num)
{
	if(gen_filter_event_pos[filter_event_num])
	{
		gen_filter_event_pos[filter_event_num] = false;
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":" << FE[filter_event_num].name() << " <- 1" << EndDebugInfo;
		}

		FE[filter_event_num] = true;
		gen_filter_event_neg[filter_event_num] = true;
		gen_filter_event_pulse_event[filter_event_num].notify(master_clock_period); // schedule falling edge of pulse
	}
	else if(gen_filter_event_neg[filter_event_num])
	{
		gen_filter_event_neg[filter_event_num] = false;
		
		if(unlikely(verbose))
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":" << FE[filter_event_num].name() << " <- 0" << EndDebugInfo;
		}

		FE[filter_event_num] = false;
	}
}

} // end of namespace m_can
} // end of namespace bosch
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_BOSCH_M_CAN_M_CAN_TCC__
