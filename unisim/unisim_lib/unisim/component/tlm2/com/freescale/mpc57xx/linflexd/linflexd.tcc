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

#ifndef __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_TCC__

#include <unisim/component/tlm2/com/freescale/mpc57xx/linflexd/linflexd.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace freescale {
namespace mpc57xx {
namespace linflexd {

using unisim::kernel::tlm2::tlm_input_bitstream_sync_status;
using unisim::kernel::tlm2::TLM_INPUT_BITSTREAM_SYNC_OK;
using unisim::kernel::tlm2::TLM_INPUT_BITSTREAM_NEED_SYNC;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::NUM_IRQS;

template <typename CONFIG>
const unsigned int LINFlexD<CONFIG>::NUM_IDENTIFIERS;

template <typename CONFIG>
const bool LINFlexD<CONFIG>::threaded_model;

template <typename CONFIG>
LINFlexD<CONFIG>::LINFlexD(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, LINTX("LINTX")
	, LINRX("LINRX")
	, m_clk("m_clk")
	, lin_clk("lin_clk")
	, reset_b("reset_b")
	, INT_RX("INT_RX")
	, INT_TX("INT_TX")
	, INT_ERR("INT_ERR")
	, DMA_RX()
	, DMA_TX()
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, lin_clk_prop_proxy(lin_clk)
	, linflexd_lincr1(this) 
	, linflexd_linier(this) 
	, linflexd_linsr(this)  
	, linflexd_linesr(this) 
	, linflexd_uartcr(this) 
	, linflexd_uartsr(this) 
	, linflexd_lintcsr(this)
	, linflexd_linocr(this) 
	, linflexd_lintocr(this)
	, linflexd_linfbrr(this)
	, linflexd_linibrr(this)
	, linflexd_lincfr(this) 
	, linflexd_lincr2(this) 
	, linflexd_bidr(this)   
	, linflexd_bdrl(this)   
	, linflexd_bdrm(this)   
	, linflexd_ifer(this)   
	, linflexd_ifmi(this)   
	, linflexd_ifmr(this)   
	, linflexd_ifcr(this)   
	, linflexd_gcr(this)    
	, linflexd_uartpto(this)
	, linflexd_uartcto(this)
	, linflexd_dmatxe(this) 
	, linflexd_dmarxe(this) 
	, linflexd_ptd(this)
	, rx_input()
	, rx_prev_input_status(true)
	, rx_input_status(true)
	, rx_dominant_bits_cnt(1)
	, rx_fifo_cnt(0)
	, rx_parity_error_reg(0)
	, gen_int_rx_event("gen_int_rx_event")
	, gen_int_tx_event("gen_int_tx_event")
	, gen_int_err_event("gen_int_err_event")
	, gen_dma_rx_event()
	, gen_dma_tx_event()
	, tx_event("tx_event")
	, pending_tx_request(false)
	, lins_int_rx_mask(false)
	, last_run_time(sc_core::SC_ZERO_TIME)
	, rx_time(sc_core::SC_ZERO_TIME)
	, data_reception_in_progress(false)
	, data_transmission_in_progress(false)
	, dma_rx()
	, dma_tx()
	, reg_addr_map()
	, registers_registry()
	, schedule()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, baud_tolerance(2.0)
	, param_baud_tolerance("baud-tolerance", this, baud_tolerance, "Baud tolerance (up to 5 %) in reception (fraction in percents of baud period); Tolerate +/- X % drift")
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
	, lin_clock_period(10.0, sc_core::SC_NS)          
	, lin_clock_start_time(sc_core::SC_ZERO_TIME)
	, lin_clock_posedge_first(true)
	, lin_clock_duty_cycle(0.5)
	, baud_period_lower_bound(sc_core::SC_ZERO_TIME)
	, baud_period(sc_core::SC_ZERO_TIME)
	, baud_period_upper_bound(sc_core::SC_ZERO_TIME)
{
	param_baud_tolerance.SetMutable(false);
	
	peripheral_slave_if(*this);
	
	LINTX.register_nb_receive(this, &LINFlexD<CONFIG>::nb_receive, LINTX_IF);
	LINRX.register_nb_receive(this, &LINFlexD<CONFIG>::nb_receive, LINRX_IF);
	
	std::stringstream description_sstr;
	description_sstr << "MPC57XX LINFlexD:" << std::endl;
	description_sstr << "  - " << NUM_IRQS << " IRQ(s)" << std::endl;
	description_sstr << "  - " << NUM_DMA_TX_CHANNELS << " Tx DMA channel(s)" << std::endl;
	description_sstr << "  - " << NUM_DMA_RX_CHANNELS << " Rx DMA channel(s)" << std::endl;
	description_sstr << "  - " << NUM_FILTERS << " filter(s) implemented" << std::endl;
	description_sstr << "  - " << ((GENERIC_SLAVE == 1) ? "master/slave" : "master") << " LIN operation mode" << std::endl;
	description_sstr << "  - generic psi5=" << GENERIC_PSI5 << std::endl;
	if(HAS_AUTO_SYNCHRONIZATION_SUPPORT)
	{
		description_sstr << "  - Autosynchronization support" << std::endl;
	}
	if(NUM_IDENTIFIERS > 0)
	{
		description_sstr << "  - " << NUM_IDENTIFIERS << " identifier(s)" << std::endl;
	}
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 59" << std::endl;
	this->SetDescription(description_sstr.str().c_str());
	
	unsigned int dma_tx_num;
	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		std::stringstream DMA_TX_name_sstr;
		DMA_TX_name_sstr << "DMA_TX_" << dma_tx_num;
		DMA_TX[dma_tx_num] = new sc_core::sc_out<bool>(DMA_TX_name_sstr.str().c_str());
	}
	
	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		std::stringstream DMA_ACK_TX_name_sstr;
		DMA_ACK_TX_name_sstr << "DMA_ACK_TX_" << dma_tx_num;
		DMA_ACK_TX[dma_tx_num] = new sc_core::sc_in<bool>(DMA_ACK_TX_name_sstr.str().c_str());
	}

	unsigned int dma_rx_num;
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		std::stringstream DMA_RX_name_sstr;
		DMA_RX_name_sstr << "DMA_RX_" << dma_rx_num;
		DMA_RX[dma_rx_num] = new sc_core::sc_out<bool>(DMA_RX_name_sstr.str().c_str());
	}

	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		std::stringstream DMA_ACK_RX_name_sstr;
		DMA_ACK_RX_name_sstr << "DMA_ACK_RX_" << dma_rx_num;
		DMA_ACK_RX[dma_rx_num] = new sc_core::sc_in<bool>(DMA_ACK_RX_name_sstr.str().c_str());
	}
	
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		std::stringstream gen_dma_rx_event_name_sstr;
		gen_dma_rx_event_name_sstr << "gen_dma_rx_event_" << dma_rx_num;
		gen_dma_rx_event[dma_rx_num] = new sc_core::sc_event(gen_dma_rx_event_name_sstr.str().c_str());
	}

	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		std::stringstream gen_dma_tx_event_name_sstr;
		gen_dma_tx_event_name_sstr << "gen_dma_tx_event_" << dma_tx_num;
		gen_dma_tx_event[dma_tx_num] = new sc_core::sc_event(gen_dma_tx_event_name_sstr.str().c_str());
	}

	SC_HAS_PROCESS(LINFlexD);

	// Map LINFlexD registers regarding there address offsets
	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegister(linflexd_lincr1.ADDRESS_OFFSET,  &linflexd_lincr1);  
	reg_addr_map.MapRegister(linflexd_linier.ADDRESS_OFFSET,  &linflexd_linier);  
	reg_addr_map.MapRegister(linflexd_linsr.ADDRESS_OFFSET,   &linflexd_linsr);   
	reg_addr_map.MapRegister(linflexd_linesr.ADDRESS_OFFSET,  &linflexd_linesr);  
	reg_addr_map.MapRegister(linflexd_uartcr.ADDRESS_OFFSET,  &linflexd_uartcr);  
	reg_addr_map.MapRegister(linflexd_uartsr.ADDRESS_OFFSET,  &linflexd_uartsr);  
	reg_addr_map.MapRegister(linflexd_lintcsr.ADDRESS_OFFSET, &linflexd_lintcsr); 
	reg_addr_map.MapRegister(linflexd_linocr.ADDRESS_OFFSET,  &linflexd_linocr);  
	reg_addr_map.MapRegister(linflexd_lintocr.ADDRESS_OFFSET, &linflexd_lintocr); 
	reg_addr_map.MapRegister(linflexd_linfbrr.ADDRESS_OFFSET, &linflexd_linfbrr); 
	reg_addr_map.MapRegister(linflexd_linibrr.ADDRESS_OFFSET, &linflexd_linibrr); 
	reg_addr_map.MapRegister(linflexd_lincfr.ADDRESS_OFFSET,  &linflexd_lincfr);  
	reg_addr_map.MapRegister(linflexd_lincr2.ADDRESS_OFFSET,  &linflexd_lincr2);  
	reg_addr_map.MapRegister(linflexd_bidr.ADDRESS_OFFSET,    &linflexd_bidr);    
	reg_addr_map.MapRegister(linflexd_bdrl.ADDRESS_OFFSET,    &linflexd_bdrl);    
	reg_addr_map.MapRegister(linflexd_bdrm.ADDRESS_OFFSET,    &linflexd_bdrm);    
	reg_addr_map.MapRegister(linflexd_ifer.ADDRESS_OFFSET,    &linflexd_ifer);    
	reg_addr_map.MapRegister(linflexd_ifmi.ADDRESS_OFFSET,    &linflexd_ifmi);    
	reg_addr_map.MapRegister(linflexd_ifmr.ADDRESS_OFFSET,    &linflexd_ifmr);
	if(NUM_IDENTIFIERS > 0)
	{
		reg_addr_map.MapRegisterFile(LINFlexD_IFCR::ADDRESS_OFFSET, &linflexd_ifcr);
	}
	reg_addr_map.MapRegister(linflexd_gcr.ADDRESS_OFFSET,     &linflexd_gcr);    
	reg_addr_map.MapRegister(linflexd_uartpto.ADDRESS_OFFSET, &linflexd_uartpto);
	reg_addr_map.MapRegister(linflexd_uartcto.ADDRESS_OFFSET, &linflexd_uartcto);
	reg_addr_map.MapRegister(linflexd_dmatxe.ADDRESS_OFFSET,  &linflexd_dmatxe); 
	reg_addr_map.MapRegister(linflexd_dmarxe.ADDRESS_OFFSET,  &linflexd_dmarxe); 
	reg_addr_map.MapRegister(linflexd_ptd.ADDRESS_OFFSET,     &linflexd_ptd);    

	registers_registry.AddRegisterInterface(linflexd_lincr1.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_linier.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_linsr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_linesr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_uartcr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_uartsr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_lintcsr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_linocr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_lintocr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_linfbrr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_linibrr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_lincfr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_lincr2.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_bidr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_bdrl.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_bdrm.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_ifer.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_ifmi.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_ifmr.CreateRegisterInterface());
	if(NUM_IDENTIFIERS > 0)
	{
		for(unsigned int i = 0; i < NUM_IDENTIFIERS; i++)
		{
			registers_registry.AddRegisterInterface(linflexd_ifcr[i].CreateRegisterInterface());
		}
	}
	
	registers_registry.AddRegisterInterface(linflexd_gcr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_uartpto.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_uartcto.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_dmatxe.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_dmarxe.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(linflexd_ptd.CreateRegisterInterface());

	SC_METHOD(RESET_B_Process);
	sensitive << reset_b.pos();
	
	SC_METHOD(INT_RX_Process);
	sensitive << gen_int_rx_event;
	
	SC_METHOD(INT_TX_Process);
	sensitive << gen_int_tx_event;
	
	SC_METHOD(INT_ERR_Process);
	sensitive << gen_int_err_event;
	
	SC_THREAD(TX_Process);
	sensitive << tx_event;
	
	SC_THREAD(RX_Process);
	sensitive << rx_input.event();
	
	// Spawn an DMA_RX_Process for each DMA RX channel
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		sc_core::sc_spawn_options dma_rx_process_spawn_options;
		dma_rx_process_spawn_options.spawn_method();
		dma_rx_process_spawn_options.set_sensitivity(gen_dma_rx_event[dma_rx_num]);
		
		std::stringstream dma_rx_process_name_sstr;
		dma_rx_process_name_sstr << "DMA_RX_Process_" << dma_rx_num;
		sc_core::sc_spawn(sc_bind(&LINFlexD<CONFIG>::DMA_RX_Process, this, dma_rx_num), dma_rx_process_name_sstr.str().c_str(), &dma_rx_process_spawn_options);
	}

	// Spawn an DMA_TX_Process for each DMA TX channel
	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		sc_core::sc_spawn_options dma_tx_process_spawn_options;
		dma_tx_process_spawn_options.spawn_method();
		dma_tx_process_spawn_options.set_sensitivity(gen_dma_tx_event[dma_tx_num]);
		
		std::stringstream dma_tx_process_name_sstr;
		dma_tx_process_name_sstr << "DMA_TX_Process_" << dma_tx_num;
		sc_core::sc_spawn(sc_bind(&LINFlexD<CONFIG>::DMA_TX_Process, this, dma_tx_num), dma_tx_process_name_sstr.str().c_str(), &dma_tx_process_spawn_options);
	}

	// Spawn an DMA_ACK_RX_Process for each DMA RX channel
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		sc_core::sc_spawn_options dma_ack_rx_process_spawn_options;
		dma_ack_rx_process_spawn_options.spawn_method();
		dma_ack_rx_process_spawn_options.set_sensitivity(&DMA_ACK_RX[dma_rx_num]->pos());
		
		std::stringstream dma_ack_rx_process_name_sstr;
		dma_ack_rx_process_name_sstr << "DMA_ACK_RX_Process_" << dma_rx_num;
		sc_core::sc_spawn(sc_bind(&LINFlexD<CONFIG>::DMA_ACK_RX_Process, this, dma_rx_num), dma_ack_rx_process_name_sstr.str().c_str(), &dma_ack_rx_process_spawn_options);
	}

	// Spawn an DMA_ACK_TX_Process for each DMA TX channel
	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		sc_core::sc_spawn_options dma_ack_tx_process_spawn_options;
		dma_ack_tx_process_spawn_options.spawn_method();
		dma_ack_tx_process_spawn_options.set_sensitivity(&DMA_ACK_TX[dma_tx_num]->pos());
		
		std::stringstream dma_ack_tx_process_name_sstr;
		dma_ack_tx_process_name_sstr << "DMA_ACK_TX_Process_" << dma_tx_num;
		sc_core::sc_spawn(sc_bind(&LINFlexD<CONFIG>::DMA_ACK_TX_Process, this, dma_tx_num), dma_ack_tx_process_name_sstr.str().c_str(), &dma_ack_tx_process_spawn_options);
	}
}

template <typename CONFIG>
LINFlexD<CONFIG>::~LINFlexD()
{
	unsigned int dma_tx_num;
	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		delete DMA_TX[dma_tx_num];
	}
	
	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		delete DMA_ACK_TX[dma_tx_num];
	}

	unsigned int dma_rx_num;
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		delete DMA_RX[dma_rx_num];
	}
	
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		delete DMA_ACK_RX[dma_rx_num];
	}
	
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		delete gen_dma_rx_event[dma_rx_num];
	}

	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		delete gen_dma_tx_event[dma_tx_num];
	}
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::EndSetup()
{
	if(baud_tolerance < 0.0)
	{
		baud_tolerance = 0.0;
		logger << DebugWarning << "Parameter " << param_baud_tolerance.GetName() << " should be > " << baud_tolerance << "; using " << param_baud_tolerance.GetName() << " = " << baud_tolerance << " %" << EndDebugWarning;
	}
	if(baud_tolerance > 5.0)
	{
		baud_tolerance = 5.0;
		logger << DebugWarning << "Parameter " << param_baud_tolerance.GetName() << " should be <= " << baud_tolerance << "; using " << param_baud_tolerance.GetName() << " = " << baud_tolerance << " %" << EndDebugWarning;
	}
	
	return true;
}

template <typename CONFIG>
void LINFlexD<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;
	
	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&LINFlexD<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	// Spawn Process
	sc_core::sc_spawn_options process_spawn_options;
	if(!threaded_model) process_spawn_options.spawn_method();
	process_spawn_options.set_sensitivity(&schedule.GetKernelEvent());
	process_spawn_options.set_sensitivity(&lin_clk_prop_proxy.GetClockPropertiesChangedEvent());
	
	sc_core::sc_spawn(sc_bind(&LINFlexD<CONFIG>::Process, this), "Process", &process_spawn_options);

	Reset();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool LINFlexD<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int LINFlexD<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
tlm::tlm_sync_enum LINFlexD<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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

template <typename CONFIG>
void LINFlexD<CONFIG>::nb_receive(int id, unisim::kernel::tlm2::tlm_serial_payload& payload, const sc_core::sc_time& t)
{
	if(((id == LINRX_IF) && !linflexd_lincr1.template Get<typename LINFlexD_LINCR1::LBKM>()) || // from LINRX (loop back mode disabled)
	   ((id == LINTX_IF) && linflexd_lincr1.template Get<typename LINFlexD_LINCR1::LBKM>())) // from LINTX (loop back mode enabled)
	{
		if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RxEn>()) // receiver enable ?
		{
			const unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();
			const sc_core::sc_time& period = payload.get_period();
			
			if(verbose)
			{
				logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": receiving [" << data << "] over LINRX with a period of " << period << EndDebugInfo;
			}

			if(LIN_ClockEnabled())
			{
				if((period < baud_period_lower_bound) || (period >= baud_period_upper_bound))
				{
					logger << DebugWarning << "Receiving over " << LINRX.name() << " with a period " << period << " instead of " << baud_period << " with a tolerance of " << baud_tolerance << " %" << EndDebugWarning;
				}
			}
			else
			{
				logger << DebugWarning << "Receiving over " << LINRX.name() << " with a period " << period << " while LIN clock is disabled" << EndDebugWarning;
			}
			
			rx_input.fill(payload, t);
		}
	}
}

//////////////// unisim::service::interface::Registers ////////////////////

template <typename CONFIG>
unisim::service::interfaces::Register *LINFlexD<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ProcessEvent(Event *event)
{
	switch(event->GetType())
	{
		case EV_NONE:
			break;
		
		case EV_CPU_PAYLOAD:
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
			
			break;
		}
		
		case EV_WAKE_UP:
			break;
		
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ProcessEvents()
{
	const sc_core::sc_time& time_stamp = sc_core::sc_time_stamp();
	
	RunTimeoutCounterToTime(time_stamp); // Run timeout counter to time
	
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
	
	ScheduleTimeoutCounterRun();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::Process()
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
	}
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UART_Mode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::UART>();
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::InitMode() const
{
	return linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_INIT_MODE;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::SleepMode() const
{
	return linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_SLEEP_MODE;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::NormalMode() const
{
	return (linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() != LINS_SLEEP_MODE) &&
	       (linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() != LINS_INIT_MODE);
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::LINTimeoutCounterMode() const
{
	return linflexd_lintcsr.template Get<typename LINFlexD_LINTCSR::MODE>() == 0 /* LIN mode */;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::OutputCompareTimeoutCounterMode() const
{
	return linflexd_lintcsr.template Get<typename LINFlexD_LINTCSR::MODE>() == 1 /* output compare mode */;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::HardwareChecksumCalculation() const
{
	return linflexd_lincr1.template Get<typename LINFlexD_LINCR1::CCD>() == 0 /* checksum calculation is done by hardware */;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::SoftwareChecksumCalculation() const
{
	return linflexd_lincr1.template Get<typename LINFlexD_LINCR1::CCD>() == 1 /* checksum calculation is done by software */;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::LIN_MasterMode() const
{
	return linflexd_lincr1.template Get<typename LINFlexD_LINCR1::MME>() == 1;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::LIN_SlaveMode() const
{
	return linflexd_lincr1.template Get<typename LINFlexD_LINCR1::MME>() == 0;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UART_RX_BufferMode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFBM>() == 0;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UART_RX_FIFO_Mode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFBM>() == 1;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UART_TX_BufferMode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFBM>() == 0;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::UART_TX_FIFO_Mode() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFBM>() == 1;
}

template <typename CONFIG>
unsigned int LINFlexD<CONFIG>::UARTWordLength() const
{
	return linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WLS>() ? 2 : (linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>() ? 2 : 1);
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::LINS_INT_RX() const
{
	return  ((linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_SYNC_DEL) ||
	         (linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_SYNC_FIELD) ||
	         (linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_IDENTIFIER_FIELD) ||
	         (linflexd_linier.template Get <typename LINFlexD_LINIER::LSIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_CHECKSUM)) && 
	       !((linflexd_linier.template Get<typename LINFlexD_LINIER::WUIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::WUF>()) || 
	        (linflexd_linier.template Get<typename LINFlexD_LINIER::DBFIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::DBFF>()) ||
	        (linflexd_linier.template Get<typename LINFlexD_LINIER::DRIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::DRF>()) ||
	        (linflexd_linier.template Get<typename LINFlexD_LINIER::HRIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::HRF>()));
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::LIN_ClockEnabled() const
{
	return linflexd_linibrr.template Get<typename LINFlexD_LINIBRR::IBR>() != 0;
}

template <typename CONFIG>
void LINFlexD<CONFIG>::Reset()
{
	unsigned int ident_num;
	unsigned int dma_rx_num;
	unsigned int dma_tx_num;
	
	rx_prev_input_status = true;
	rx_input_status = true;
	rx_dominant_bits_cnt = 0;
	rx_fifo_cnt = 0;
	rx_parity_error_reg = 0;
	pending_tx_request = false;
	lins_int_rx_mask = false;
	data_reception_in_progress = false;
	data_transmission_in_progress = false;
	
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		dma_rx[dma_rx_num] = false;
	}
	
	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		dma_tx[dma_tx_num] = false;
	}
	
	schedule.Clear();

	linflexd_lincr1.Reset();
	linflexd_linier.Reset();
	linflexd_linsr.Reset();
	linflexd_linesr.Reset();
	linflexd_uartcr.Reset();
	linflexd_uartsr.Reset();
	linflexd_lintcsr.Reset();
	linflexd_linocr.Reset();
	linflexd_lintocr.Reset();
	linflexd_linfbrr.Reset();
	linflexd_linibrr.Reset();
	linflexd_lincfr.Reset();
	linflexd_lincr2.Reset();
	linflexd_bidr.Reset();
	linflexd_bdrl.Reset();
	linflexd_bdrm.Reset();
	linflexd_ifer.Reset();
	linflexd_ifmi.Reset();
	linflexd_ifmr.Reset();
	for(ident_num = 0; ident_num < NUM_IDENTIFIERS; ident_num++)
	{
		linflexd_ifcr[ident_num].Reset();
	}
	linflexd_gcr.Reset();
	linflexd_uartpto.Reset();
	linflexd_uartcto.Reset();
	linflexd_dmatxe.Reset(); 
	linflexd_dmarxe.Reset();
	linflexd_ptd.Reset();
	
	if(!m_clk_prop_proxy.IsClockCompatible())
	{
		logger << DebugError << "master clock port is not bound to a unisim::kernel::tlm2::Clock" << EndDebugError;
		unisim::kernel::Object::Stop(-1);
		return;
	}
	
	if(!lin_clk_prop_proxy.IsClockCompatible())
	{
		logger << DebugError << "LIN clock port is not bound to a unisim::kernel::tlm2::Clock" << EndDebugError;
		unisim::kernel::Object::Stop(-1);
		return;
	}

	UpdateMasterClock();
	UpdateLINClock();
	
	UpdateDMA_RX();
	RequestDMA_TX();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::SoftReset()
{
	linflexd_linsr.SoftReset();
	linflexd_linesr.SoftReset();
	linflexd_lintcsr.SoftReset();
	linflexd_uartsr.SoftReset();
	linflexd_uartcr.SoftReset();
	linflexd_uartcto.SoftReset();
}

template <typename CONFIG>
LIN_State LINFlexD<CONFIG>::GetState()
{
	return LIN_State(linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>());
}

template <typename CONFIG>
void LINFlexD<CONFIG>::SetState(LIN_State lin_state)
{
	if(unlikely(verbose && GetState() != lin_state))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": leaving " << GetState() << EndDebugInfo;
		logger << DebugInfo << sc_core::sc_time_stamp() << ": entering " << lin_state << EndDebugInfo;
	}
	lins_int_rx_mask = false;
	linflexd_linsr.template Set<typename LINFlexD_LINSR::LINS>(lin_state);
	
	// update interrupt signals
	UpdateINT_RX();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateState()
{
	switch(GetState())
	{
		case LINS_SLEEP_MODE:
			if(!linflexd_lincr1.template Get<typename LINFlexD_LINCR1::SLEEP>())
			{
				// software requests to leave sleep mode
				
				if(linflexd_lincr1.template Get<typename LINFlexD_LINCR1::INIT>())
				{
					// software request to enter init mode
					if(!data_reception_in_progress && !data_transmission_in_progress)
					{
						// enter init mode
						SetState(LINS_INIT_MODE);
					}
				}
				else if(!data_reception_in_progress && !data_transmission_in_progress)
				{
					SetState(LINS_IDLE_MODE);
				}
				else
				{
					SetState(LINS_DATA_RECEPTION_DATA_TRANSMISSION);
				}
			}
			break;
		case LINS_INIT_MODE:
			if(!linflexd_lincr1.template Get<typename LINFlexD_LINCR1::INIT>())
			{
				if(linflexd_lincr1.template Get<typename LINFlexD_LINCR1::SLEEP>())
				{
					SetState(LINS_SLEEP_MODE);
				}
				else
				{
					if(!data_reception_in_progress && !data_transmission_in_progress)
					{
						SetState(LINS_IDLE_MODE);
					}
					else
					{
						SetState(LINS_DATA_RECEPTION_DATA_TRANSMISSION);
					}
				}
			}
			
			break;
		case LINS_IDLE_MODE:
			if(data_reception_in_progress || data_transmission_in_progress)
			{
				SetState(LINS_DATA_RECEPTION_DATA_TRANSMISSION);
			}
			else if(linflexd_lincr1.template Get<typename LINFlexD_LINCR1::SLEEP>())
			{
				SetState(LINS_SLEEP_MODE);
			}
			else if(linflexd_lincr1.template Get<typename LINFlexD_LINCR1::INIT>())
			{
				SetState(LINS_INIT_MODE);
			}
			break;
		case LINS_SYNC_BREAK:
			break;
		case LINS_SYNC_DEL:
			break;
		case LINS_SYNC_FIELD:
			break;
		case LINS_IDENTIFIER_FIELD:
			break;
		case LINS_HEADER_RECEPTION_TRANSMISSION:
			break;
		case LINS_DATA_RECEPTION_DATA_TRANSMISSION:
			if(linflexd_lincr1.template Get<typename LINFlexD_LINCR1::SLEEP>())
			{
				if(!data_reception_in_progress && !data_transmission_in_progress)
				{
					SetState(LINS_SLEEP_MODE);
				}
			}
			else if(linflexd_lincr1.template Get<typename LINFlexD_LINCR1::INIT>())
			{
				if(!data_reception_in_progress && !data_transmission_in_progress)
				{
					SetState(LINS_INIT_MODE);
				}
			}
			else if(!data_reception_in_progress && !data_transmission_in_progress)
			{
				SetState(LINS_IDLE_MODE);
			}
			break;
		case LINS_CHECKSUM:
			break;
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::Timeout()
{
	// reset timeout
	linflexd_uartcto.Reset();
	
	// timeout
	linflexd_uartsr.template Set<typename LINFlexD_UARTSR::TO>(1);
	
	// An interrupt will be generated when LINIER.DBEIE/TOIE bit is set on the Error interrupt line in UART mode
	UpdateINT_ERR();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::IncrementTimeoutCounter(sc_dt::uint64 delta)
{
	if(delta)
	{
		uint32_t uart_cto = linflexd_uartcto.template Get<typename LINFlexD_UARTCTO::CTO>();
		
		uart_cto += delta;
		
		linflexd_uartcto.template Set<typename LINFlexD_UARTCTO::CTO>(uart_cto);
		
		if(linflexd_uartcto.template Get<typename LINFlexD_UARTCTO::CTO>() == linflexd_uartcto.template Get<typename LINFlexD_UARTPTO::PTO>())
		{
			// timeout
			Timeout();
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ResetTimeout()
{
	last_run_time = sc_core::sc_time_stamp();
	linflexd_uartcto.Reset();
}

template <typename CONFIG>
sc_dt::int64 LINFlexD<CONFIG>::TicksToNextTimeoutCounterRun()
{
	uint32_t uart_cto = linflexd_uartcto.template Get<typename LINFlexD_UARTCTO::CTO>();
	uint32_t uart_pto = linflexd_uartcto.template Get<typename LINFlexD_UARTPTO::PTO>();
	
	assert(uart_pto && (uart_pto >= uart_cto));
	return uart_pto - uart_cto;
}

template <typename CONFIG>
sc_core::sc_time LINFlexD<CONFIG>::TimeToNextTimeoutCounterRun()
{
	return TicksToNextTimeoutCounterRun() * baud_period;
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RunTimeoutCounterToTime(const sc_core::sc_time& time_stamp)
{
	if(LIN_ClockEnabled() &&                                              // LIN clock enabled and
	   linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RxEn>() &&  // Rx enabled and
	   linflexd_uartcto.template Get<typename LINFlexD_UARTPTO::PTO>())   // Preset timeout is non-null
	{
		if(time_stamp > last_run_time)
		{
			// Compute the elapsed time since last run
			sc_core::sc_time delay_since_last_run(time_stamp);
			delay_since_last_run -= last_run_time;
			
			// Compute number of timers ticks since last run
			sc_dt::uint64 delta = delay_since_last_run / baud_period;
			
			if(delta)
			{
				sc_core::sc_time run_time(baud_period);
				run_time *= delta;
			
				IncrementTimeoutCounter(delta);
				last_run_time += run_time;
			}
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::ScheduleTimeoutCounterRun()
{
	if(LIN_ClockEnabled())
	{
		// Clocks properties may have changed that may affect time to next run
		UpdateLINClock();
		
		uint32_t uart_pto = linflexd_uartcto.template Get<typename LINFlexD_UARTPTO::PTO>();
		
		if(uart_pto)
		{
			sc_core::sc_time time_to_next_timeout_counter_run = TimeToNextTimeoutCounterRun();
			
			if(time_to_next_timeout_counter_run != sc_core::SC_ZERO_TIME)
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ": time to next timeout counter run is " << time_to_next_timeout_counter_run << EndDebugInfo;
				}
				
				sc_core::sc_time next_timeout_counter_run_time_stamp(sc_core::sc_time_stamp());
				next_timeout_counter_run_time_stamp += time_to_next_timeout_counter_run;
				
				if(unlikely(verbose))
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ": timeout counter will run at " << next_timeout_counter_run_time_stamp << EndDebugInfo;
				}
				
				Event *event = schedule.AllocEvent();
				event->WakeUp();
				event->SetTimeStamp(next_timeout_counter_run_time_stamp);
				schedule.Notify(event);
			}
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateINT_RX()
{
	gen_int_rx_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateINT_TX()
{
	gen_int_tx_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateINT_ERR()
{
	gen_int_err_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateDMA_RX(unsigned int dma_rx_num, const sc_core::sc_time& delay)
{
	gen_dma_rx_event[dma_rx_num]->notify(delay);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateDMA_TX(unsigned int dma_tx_num, const sc_core::sc_time& delay)
{
	gen_dma_tx_event[dma_tx_num]->notify(delay);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateDMA_RX(const sc_core::sc_time& delay)
{
	unsigned int dma_rx_num;
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		UpdateDMA_RX(dma_rx_num, delay);
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateDMA_TX(const sc_core::sc_time& delay)
{
	unsigned int dma_tx_num;
	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		UpdateDMA_TX(dma_tx_num, delay);
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RequestDMA_RX()
{
	unsigned int dma_rx_num;
	for(dma_rx_num = 0; dma_rx_num < NUM_DMA_RX_CHANNELS; dma_rx_num++)
	{
		dma_rx[dma_rx_num] = true;
		UpdateDMA_RX(dma_rx_num);
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RequestDMA_TX()
{
	unsigned int dma_tx_num;
	for(dma_tx_num = 0; dma_tx_num < NUM_DMA_TX_CHANNELS; dma_tx_num++)
	{
		dma_tx[dma_tx_num] = true;
		UpdateDMA_TX(dma_tx_num);
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::UpdateLINClock()
{
	lin_clock_period = lin_clk_prop_proxy.GetClockPeriod();
	lin_clock_start_time = lin_clk_prop_proxy.GetClockStartTime();
	lin_clock_posedge_first = lin_clk_prop_proxy.GetClockPosEdgeFirst();
	lin_clock_duty_cycle = lin_clk_prop_proxy.GetClockDutyCycle();
	
	unsigned int rose = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::ROSE>();
	unsigned int osr = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::OSR>();
	unsigned int ibr = linflexd_linibrr.template Get<typename LINFlexD_LINIBRR::IBR>();
	
	if(ibr)
	{
		unsigned int fbr = linflexd_linfbrr.template Get<typename LINFlexD_LINFBRR::FBR>();
		
		if(rose)
		{
			if(fbr)
			{
				logger << DebugWarning << "when reduced oversampling is enabled, LINFBRR should not be used and programmed to zero" << EndDebugWarning;
			}
			
			if((osr != 4) && (osr != 5) && (osr != 6) && (osr != 8))
			{
				logger << DebugWarning << "over sampling rate allowed values are: 4, 5, 6 and 8; got " << osr << EndDebugWarning;
			}
		}
		
		unsigned int period_multiplicator_x16 = ((16 * ibr) + (rose ? 0 : fbr)) * (rose ? osr : 16);
		baud_period = lin_clock_period;
		baud_period *= (double) period_multiplicator_x16 / 16.0;
		if(verbose)
		{
			logger << DebugInfo << "using period of " << baud_period << EndDebugInfo;
		}
		sc_core::sc_time baud_period_tolerance = baud_period * (baud_tolerance / 100.0);
		baud_period_lower_bound = baud_period;
		baud_period_lower_bound -= baud_period_tolerance;
		baud_period_upper_bound = baud_period;
		baud_period_upper_bound += baud_period_tolerance;
	}
	else
	{
		// LIN clock disabled
		if(verbose)
		{
			logger << DebugInfo << "LIN clock is disabled: using a period of " << sc_core::SC_ZERO_TIME << EndDebugInfo;
		}
		baud_period = sc_core::SC_ZERO_TIME;
		baud_period_lower_bound = sc_core::SC_ZERO_TIME;
		baud_period_upper_bound = sc_core::SC_ZERO_TIME;
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::MasterClockPropertiesChangedProcess()
{
	// Clock properties have changed
	UpdateMasterClock();
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::INT_RX_Process()
{
	//             Mode                                           Mask                                 Enable                                                                    Flag/Condition
	bool int_rx = (                                     !lins_int_rx_mask && linflexd_linier.template Get<typename LINFlexD_LINIER::LSIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_SYNC_DEL        ) ||
	              (                                     !lins_int_rx_mask && linflexd_linier.template Get<typename LINFlexD_LINIER::LSIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_SYNC_FIELD      ) ||
	              (                                     !lins_int_rx_mask && linflexd_linier.template Get<typename LINFlexD_LINIER::LSIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_IDENTIFIER_FIELD) ||
	              (                                     !lins_int_rx_mask && linflexd_linier.template Get<typename LINFlexD_LINIER::LSIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::LINS>() == LINS_CHECKSUM        ) ||
	              ( UART_Mode() &&                                           linflexd_linier.template Get<typename LINFlexD_LINIER::WUIE>()  && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::WUF>()                        ) ||
	              (!UART_Mode() &&                                           linflexd_linier.template Get<typename LINFlexD_LINIER::WUIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::WUF>()                          ) ||
	              (                                                          linflexd_linier.template Get<typename LINFlexD_LINIER::DBFIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::DBFF>()                         ) ||
	              ( UART_Mode() && UART_RX_BufferMode()                   && linflexd_linier.template Get<typename LINFlexD_LINIER::DRIE>()  && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::DRFRFE>()                      ) ||
	              (!UART_Mode() &&                                           linflexd_linier.template Get<typename LINFlexD_LINIER::DRIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::DRF>()                          ) ||
	              (                                                          linflexd_linier.template Get<typename LINFlexD_LINIER::HRIE>()  && linflexd_linsr.template Get<typename LINFlexD_LINSR::HRF>()                          );
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_RX.name() << " <- " << int_rx << EndDebugInfo;
	}
	
	INT_RX = int_rx;
}

template <typename CONFIG>
void LINFlexD<CONFIG>::INT_TX_Process()
{
	//             Mode                            Enable                                                                                                          Flag
	bool int_tx = ( UART_Mode() && UART_TX_BufferMode() && linflexd_linier.template Get<typename LINFlexD_LINIER::DTIE>()      && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::DTFTFF>()) ||
	              (!UART_Mode() &&                         linflexd_linier.template Get<typename LINFlexD_LINIER::DTIE>()      && linflexd_linsr.template Get<typename LINFlexD_LINSR::DTF>()    ) ||
	              (                                        linflexd_linier.template Get<typename LINFlexD_LINIER::DBEIETOIE>() && linflexd_linsr.template Get<typename LINFlexD_LINSR::DBEF>()   ) ||
	              (                                        linflexd_linier.template Get<typename LINFlexD_LINIER::HRIE>()      && linflexd_linsr.template Get<typename LINFlexD_LINSR::HRF>()    );

	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_TX.name() << " <- " << int_tx << EndDebugInfo;
	}
	
	INT_TX = int_tx;
}

template <typename CONFIG>
void LINFlexD<CONFIG>::INT_ERR_Process()
{
	//             Mode                            Enable                                                                    Flag
	bool int_err = ( UART_Mode() && linflexd_linier.template Get<typename LINFlexD_LINIER::BOIE>()      && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::BOF>()  ) ||
	               (!UART_Mode() && linflexd_linier.template Get<typename LINFlexD_LINIER::BOIE>()      && linflexd_linesr.template Get<typename LINFlexD_LINESR::BOF>()  ) ||
	               ( UART_Mode() && linflexd_linier.template Get<typename LINFlexD_LINIER::FEIE>()      && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::FEF>()  ) ||
	               (!UART_Mode() && linflexd_linier.template Get<typename LINFlexD_LINIER::FEIE>()      && linflexd_linesr.template Get<typename LINFlexD_LINESR::FEF>()  ) ||
	               (                linflexd_linier.template Get<typename LINFlexD_LINIER::HEIE>()      && linflexd_linesr.template Get<typename LINFlexD_LINESR::SFEF>() ) ||
	               (                linflexd_linier.template Get<typename LINFlexD_LINIER::HEIE>()      && linflexd_linesr.template Get<typename LINFlexD_LINESR::SDEF>() ) ||
	               (                linflexd_linier.template Get<typename LINFlexD_LINIER::HEIE>()      && linflexd_linesr.template Get<typename LINFlexD_LINESR::IDPEF>()) ||
	               (                linflexd_linier.template Get<typename LINFlexD_LINIER::CEIE>()      && linflexd_linesr.template Get<typename LINFlexD_LINESR::CEF>()  ) ||
	               (                linflexd_linier.template Get<typename LINFlexD_LINIER::BEIE>()      && linflexd_linesr.template Get<typename LINFlexD_LINESR::BEF>()  ) ||
	               ( UART_Mode() && linflexd_linier.template Get<typename LINFlexD_LINIER::OCIE>()      && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::OCF>()  ) ||
	               (!UART_Mode() && linflexd_linier.template Get<typename LINFlexD_LINIER::OCIE>()      && linflexd_linesr.template Get<typename LINFlexD_LINESR::OCF>()  ) ||
	               ( UART_Mode() && linflexd_linier.template Get<typename LINFlexD_LINIER::SZIE>()      && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::SZF>()  ) ||
	               (!UART_Mode() && linflexd_linier.template Get<typename LINFlexD_LINIER::SZIE>()      && linflexd_linesr.template Get<typename LINFlexD_LINESR::SZF>()  ) ||
	               (                linflexd_linier.template Get<typename LINFlexD_LINIER::DBEIETOIE>() && linflexd_uartsr.template Get<typename LINFlexD_UARTSR::TO>()   );

	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_ERR.name() << " <- " << int_err << EndDebugInfo;
	}
	
	INT_ERR = int_err;
}

template <typename CONFIG>
void LINFlexD<CONFIG>::DMA_RX_Process(unsigned int dma_rx_num)
{
	unsigned int dre = linflexd_dmarxe.template Get<typename LINFlexD_DMARXE::DRE>(); // DMA Rx channel Y enable
	bool dma_rx_enable = ((dre >> dma_rx_num) & 1) != 0;
	
	bool dma_rx_value = dma_rx[dma_rx_num] && dma_rx_enable;
		
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_RX[dma_rx_num]->name() << " <- " << dma_rx_value << EndDebugInfo;
	}
	
	DMA_RX[dma_rx_num]->write(dma_rx_value);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::DMA_TX_Process(unsigned int dma_tx_num)
{
	unsigned int dte = linflexd_dmatxe.template Get<typename LINFlexD_DMATXE::DTE>(); // DMA Tx channel Y enable
	bool dma_tx_enable = ((dte >> dma_tx_num) & 1) != 0;
	
	bool dma_tx_value = dma_tx[dma_tx_num] && dma_tx_enable;
		
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_TX[dma_tx_num]->name() << " <- " << dma_tx_value << EndDebugInfo;
	}
	
	DMA_TX[dma_tx_num]->write(dma_tx_value);
}

template <typename CONFIG>
void LINFlexD<CONFIG>::DMA_ACK_RX_Process(unsigned int dma_rx_num)
{
	if(DMA_ACK_RX[dma_rx_num]->posedge())
	{
		dma_rx[dma_rx_num] = false;
		UpdateDMA_RX(dma_rx_num/*, master_clock_period*/);
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::DMA_ACK_TX_Process(unsigned int dma_tx_num)
{
	if(DMA_ACK_TX[dma_tx_num]->posedge())
	{
		dma_tx[dma_tx_num] = false;
		UpdateDMA_TX(dma_tx_num/*, master_clock_period*/);
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RX_FIFO_Pop()
{
	if(UART_Mode())
	{
		// UART mode
		if(UART_RX_FIFO_Mode())
		{
			// FIFO mode
			unsigned int rx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFC>();
			unsigned int wls = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WLS>();
			unsigned int wl1 = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>();
			unsigned int byte_cnt = ((wls || wl1) ? 2 : 1);
			unsigned int bit_cnt = ((wls || wl1) ? 16 : 8);
		
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": Poping " << byte_cnt << " byte(s) from Rx FIFO" << EndDebugInfo;
			}
			
			if(rx_fifo_cnt >= byte_cnt)
			{
				// pop front of Rx FIFO in BDRM
				linflexd_bdrm = linflexd_bdrm >> bit_cnt;
				
				// shift accordling parity error bits
				unsigned int pe = linflexd_uartsr.template Get<typename LINFlexD_UARTSR::PE>();
				pe = pe >> byte_cnt;
				linflexd_uartsr.template Set<typename LINFlexD_UARTSR::PE>(pe);
				
				// decrement Rx FIFO count
				rx_fifo_cnt -= byte_cnt;
				
				if(rx_fifo_cnt == 0)
				{
					if(verbose)
					{
						logger << DebugInfo << rx_input.get_time_stamp() << ": Rx FIFO is empty" << EndDebugInfo;
					}
				}
				
				linflexd_uartcr.template Set<typename LINFlexD_UARTCR::RFC>(rx_fifo_cnt);
				
				linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RFE>(rx_fifo_cnt == 0); // Rx FIFO Empty ?

				linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RFNE>(rx_fifo_cnt != 0); // Receive FIFO Not Empty
			
				// Request DMA RX
				if(rx_fifo_cnt != 0) RequestDMA_RX();
			}
			else
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": Attempt to pop " << byte_cnt << " byte(s) from Rx FIFO while RX FIFO count is " << rx_fifo_cnt << EndDebugWarning;
			}
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::IncrementRxTime()
{
	const sc_core::sc_time& input_period = rx_input.get_period();
	if((input_period >= baud_period_lower_bound) && (input_period <= baud_period_upper_bound))
	{
		rx_time += input_period;
	}
	else
	{
		rx_time += baud_period;
		if(UART_Mode())
		{
			if(!linflexd_uartcr.template Get<typename LINFlexD_UARTCR::ROSE>() || (linflexd_uartcr.template Get<typename LINFlexD_UARTCR::OSR>() == 8))
			{
				// NF is set by hardware when noise is detected in the received character. It should be cleared by
				// software. This bit will reflect the same value as in LINESR when in Initialization mode and the UART bit is
				// set. During reduced oversampling (ROSE bit = 1), it is enabled only when OSR = 8.
				linflexd_uartsr.template Set<typename LINFlexD_UARTSR::NF>(1); // noise flag
			}
		}
		UpdateINT_ERR();
	}
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::RX_InputStatus()
{
	unsigned int miss_count = rx_input.get_miss_count();
	
	if(miss_count)
	{
		logger << DebugWarning << (sc_core::sc_time_stamp() + rx_time) << ": " << miss_count << " bit(s) lost from LINRX" << EndDebugWarning;
	}
	
	rx_prev_input_status = rx_input_status;
	rx_input_status = rx_input.read();
	
	if(verbose)
	{
		logger << DebugInfo << (sc_core::sc_time_stamp() + rx_time) << ": LINRX=" << rx_input_status << EndDebugInfo;
	}
	
	if(UART_Mode())
	{
		linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RDI>(rx_input_status);
	}
	
	rx_dominant_bits_cnt = rx_input_status ? 0 : (rx_dominant_bits_cnt + 1);
	
	if(rx_dominant_bits_cnt >= 100) // detect 100 dominant bits
	{
		linflexd_uartsr.template Set<typename LINFlexD_UARTSR::SZF>(1); // Stuck at zero
		
		// update interrupt signals
		UpdateINT_ERR();
	}
	return rx_input_status;
}

template <typename CONFIG>
bool LINFlexD<CONFIG>::RX_FallingEdge()
{
	RX_InputStatus();
	
	return rx_prev_input_status && !rx_input_status;
}

template <typename CONFIG>
void LINFlexD<CONFIG>::RX_Process()
{
	wait(); // wait for RX input
	rx_input.latch();
	
	rx_time = sc_core::SC_ZERO_TIME;  // Rx time offset relative to current simulation time

	while(1)
	{
		if(verbose)
		{
			logger << DebugInfo << rx_input.get_time_stamp() << ": processing received data" << EndDebugInfo;
		}
		
		if(LIN_ClockEnabled()) // is LIN clock enabled ?
		{
			// LIN clock enabled
			if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RxEn>())
			{
				// RX is enabled
				
				if(SleepMode())
				{
					// Sleep mode
					
					if(RX_FallingEdge()) // falling edge ?
					{
						if(linflexd_lincr1.template Get<typename LINFlexD_LINCR1::AUTOWU>()) // autowakeup ?
						{
							// leave sleep mode
							linflexd_lincr1.template Set<typename LINFlexD_LINCR1::SLEEP>(0);
							UpdateState();
						}
						
						if(UART_Mode())
						{
							linflexd_uartsr.template Set<typename LINFlexD_UARTSR::WUF>(1); // wakeup
							
							// update interrupt signals
							UpdateINT_RX(); // generate an INT_RX if LINIER[WUIE] is set
						}
					}
				}
				else if(NormalMode())
				{
					// Normal mode
					unsigned int wls = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WLS>();
					unsigned int wl1 = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>();

					bool config_ok = true;
					if(UART_RX_BufferMode())
					{
						// Buffer mode
						unsigned int rdfl = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RDFL>();
						
						config_ok = (((rdfl + 1) % ((wls || wl1) ? 2 : 1)) == 0); // expected number of bytes shall be a multiple of word length
					}
					
					if(config_ok) 
					{
						bool bit_value = RX_InputStatus();
						IncrementRxTime();
						if(!bit_value) // start bit ?
						{
							// received start bit
							if(verbose)
							{
								logger << DebugInfo << rx_input.get_time_stamp() << ": received start bit" << EndDebugInfo;
							}

							data_reception_in_progress = true;
							UpdateState();
							
							// start of frame
							unsigned int wl = (wl1 << 1) | linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL0>();
							unsigned int rdfbm = linflexd_gcr.template Get<typename LINFlexD_GCR::RDFBM>();
							unsigned int rdlis = linflexd_gcr.template Get<typename LINFlexD_GCR::RDLIS>();
							
							unsigned int rx_word_length = 0;       // Rx word length in bits
							unsigned int rx_frame_word_length = 0; // Rx frame word length in bits
							bool receive_parity = false;           // whether to expect a parity bit 
							uint32_t rx_shift_reg = 0;             // 32-bit receiver shift register
							unsigned int rec_byte_cnt = 0;         // received byte counter
							unsigned int num_of_frames = 0;        // number of frames received
							
							if(wls)
							{
								// 12 bits data + parity
								rx_frame_word_length = 12;
								receive_parity = true;
								rx_word_length = 16;
							}
							else
							{
								switch(wl)
								{
									case 0: // 7 bits data + parity
										rx_frame_word_length = 7;
										receive_parity = true;
										rx_word_length = 8;
										break;
									case 1: // 8 bits data when PCE = 0 or 8 bits data + parity when PCE = 1
										rx_frame_word_length = 8;
										receive_parity = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PCE>();
										rx_word_length = 8;
										break;
									case 2: // 15 bits data + parity
										rx_frame_word_length = 15;
										receive_parity = true;
										rx_word_length = 16;
										break;
									case 3: // 16 bits data when PCE = 0 or 16 bits data + parity when PCE = 1
										rx_frame_word_length = 16;
										receive_parity = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PCE>();
										rx_word_length = 16;
										break;
								}
							}

							unsigned int rx_msg_length = 0;
							
							if(UART_RX_BufferMode())
							{
								// Buffer mode
								unsigned int rdfl = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RDFL>();
								
								rx_msg_length = 8 * (rdfl + 1); // 8, 16, 24 or 32
							}
							else
							{
								// FIFO mode
								rx_msg_length = rx_word_length; // 8 or 16
							}
							
							if(verbose)
							{
								logger << DebugInfo << rx_input.get_time_stamp() << ": expecting a " << rx_msg_length << "-bit message with " << rx_frame_word_length << "-bit data";
								if(receive_parity)
								{
									logger << " + parity bit";
								}
								logger << EndDebugInfo;
							}
							
							// received byte order: DATA4 DATA5 DATA6 DATA7
							
							// bit order: LSB first
							//
							//                32-bit receiver shift register
							//          +---------+---------+---------+---------+
							// LINRX -->| 31 - 24 | 23 - 16 | 15 -  8 |  7 -  0 |
							//          +---------+---------+---------+---------+
							//               |         |        |          |
							//               |         |        |          |
							//               |         |        |          |
							//          +---------+---------+---------+---------+
							//   BDRM   | 31 - 24 | 23 - 16 | 15 -  8 |  7 -  0 |
							//          +---------+---------+---------+---------+
							//             DATA7     DATA6     DATA5     DATA4
							//
							//
							// bit order: MSB first
							//
							//                32-bit receiver shift register
							//          +---------+---------+---------+---------+
							//          | 31 - 24 | 23 - 16 | 15 -  8 |  7 -  0 |<-- LINRX
							//          +---------+---------+---------+---------+
							//               A         B        C          D
							//               |         |        |          |
							//               ------------------------------> 
							//               <------------------------------  
							//               |         |        |          |      
							//               D         C        B          A
							//          +---------+---------+---------+---------+
							//   BDRM   | 31 - 24 | 23 - 16 | 15 -  8 |  7 -  0 |
							//          +---------+---------+---------+---------+
							//             DATA7     DATA6     DATA5     DATA4
							
							// receive payload
							unsigned int i;
							bool parity_bit = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC0>();
							for(i = 0; i < rx_word_length; i++)
							{
								if(rdfbm)
								{
									// MSB first
									rx_shift_reg <<= 1;
								}
								else
								{
									// LSB first
									rx_shift_reg >>= 1;
								}
								
								if(i < rx_frame_word_length)
								{
									if(rx_input.seek(rx_time) == TLM_INPUT_BITSTREAM_NEED_SYNC)
									{
										wait(rx_time);
										rx_input.latch();
										rx_time = sc_core::SC_ZERO_TIME;
									}
									bool bit_value = RX_InputStatus(); // read bitstream
									IncrementRxTime();
									if(rdlis) bit_value = !bit_value; // invert received data if requested
									if(rdfbm)
									{
										// MSB first
										rx_shift_reg |= bit_value;
									}
									else
									{
										// LSB first
										rx_shift_reg |= (uint32_t(bit_value) << 31);
									}
									parity_bit ^= bit_value; // update computed parity bit
								}
							}

							if(UART_RX_BufferMode())
							{
								rec_byte_cnt += rx_word_length / 8; // increment received byte count
							}
								
							if(receive_parity) // looking for a parity bit ?
							{
								if(rx_input.seek(rx_time) == TLM_INPUT_BITSTREAM_NEED_SYNC)
								{
									wait(rx_time);
									rx_input.latch();
									rx_time = sc_core::SC_ZERO_TIME;
								}
								bool bit_value = RX_InputStatus();
								IncrementRxTime();
								
								bool parity_error = false;
								if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC1>())
								{
									// logical 0/1 always transmitted
									parity_error = (bit_value != linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC0>());
								}
								else
								{
									// event/odd parity
									parity_error = (bit_value != parity_bit);
								}
								
								if(parity_error)
								{
									if(verbose)
									{
										logger << DebugInfo << rx_input.get_time_stamp() << ": parity error" << EndDebugInfo;
									}
								}
								
								rx_parity_error_reg = ((rx_parity_error_reg << (rx_word_length / 8)) | parity_error) & 0xf; // fill and shift 4-bit parity error register
							}
							
							unsigned int rx_stop_bits = 0;
							bool end_of_frame = false;
							bool framing_error = false;
							bool discard_message = false;
							
							// sample all stop bits
							while(!end_of_frame && !framing_error)
							{
								if(rx_input.seek(rx_time) == TLM_INPUT_BITSTREAM_NEED_SYNC)
								{
									wait(rx_time);
									rx_input.latch();
									rx_time = sc_core::SC_ZERO_TIME;
								}
								bool bit_value = RX_InputStatus();
								IncrementRxTime();
								if(bit_value) // stop bit ?
								{
									// received a stop bit
									rx_stop_bits++;
									
									if(verbose)
									{
										logger << DebugInfo << rx_input.get_time_stamp() << ": received stop bit #" << rx_stop_bits << EndDebugInfo;
									}
									
									unsigned int sbur = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::SBUR>();
									if(sbur == 3) sbur = 0; // sbur=11b is reserved, treat it as 00b
									
									if(rx_stop_bits >= (sbur + 1))
									{
										// received all stop bits
										if(verbose)
										{
											logger << DebugInfo << rx_input.get_time_stamp() << ": received all stop bits" << EndDebugInfo;
										}
										
										end_of_frame = true;
										
										bool end_of_message = false;
										if(UART_RX_BufferMode())
										{
											// Buffer mode
											unsigned int rdfl = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RDFL>();
								
											end_of_message = (rec_byte_cnt >= (rdfl + 1)); // received all bytes ?
										}
										else
										{
											// FIFO mode
											end_of_message = true;
										}

										if(end_of_message)  // end of message ?
										{
											// wait reception time before updating state
											if(rx_time != sc_core::SC_ZERO_TIME)
											{
												wait(rx_time);
												rx_input.latch();
												rx_time = sc_core::SC_ZERO_TIME;
											}
											
											// check for buffer overrun
											if(UART_RX_BufferMode())
											{
												// Buffer mode
												if(linflexd_uartsr.template Get<typename LINFlexD_UARTSR::RMB>())
												{
													if(verbose)
													{
														logger << DebugInfo << rx_input.get_time_stamp() << ": buffer overrun" << EndDebugInfo;
													}
													
													// software did not clear RMB
													linflexd_uartsr.template Set<typename LINFlexD_UARTSR::BOF>(1); // Buffer overrun
													
													// update interrupt signals
													UpdateINT_ERR();
													
													if(linflexd_lincr1.template Get<typename LINFlexD_LINCR1::RBLM>())
													{
														if(verbose)
														{
															logger << DebugInfo << rx_input.get_time_stamp() << ": discarding message because buffer is not locked" << EndDebugInfo;
														}

														// if RBLM is set then the new message received will be discarded
														discard_message = true;
													}
													else
													{
														// if RBLM is reset then the new message will overwrite the buffer
														if(verbose)
														{
															logger << DebugInfo << rx_input.get_time_stamp() << ": overwriting buffer" << EndDebugInfo;
														}
													}
												}
											}
											else
											{
												// FIFO mode
												unsigned int rx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFC>();
												if((rx_fifo_cnt + (rx_word_length / 8)) > 4) // Rx FIFO Full ?
												{
													if(verbose)
													{
														logger << DebugInfo << rx_input.get_time_stamp() << ": discarding message because Rx FIFO is full" << EndDebugInfo;
													}
													
													discard_message = true;
												}
											}
											
											uint32_t rx_msg = 0; // received message

											if(!discard_message)
											{
												// extract message from receiver shift register
												rx_msg = (rdfbm ? unisim::util::endian::ByteSwap(rx_shift_reg) : rx_shift_reg) >> (32 - rx_msg_length);

												// make parity error flags available to software
												linflexd_uartsr.template Set<typename LINFlexD_UARTSR::PE>(rx_parity_error_reg);
											}
										
											if(UART_RX_BufferMode())
											{
												// Buffer mode
												if(!discard_message)
												{
													// latch received message into BDRM
													uint32_t write_mask = ~uint32_t(0) >> (32 - rx_msg_length);
													linflexd_bdrm = (rx_msg & ~write_mask) | (rx_msg & write_mask);
												}
													
												if(!discard_message)
												{
													if(verbose)
													{
														logger << DebugInfo << rx_input.get_time_stamp() << ": data reception completed" << EndDebugInfo;
													}
													
													// buffer data ready to be read by software
													linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RMB>(1);
													
													// Data Reception Completed
													linflexd_uartsr.template Set<typename LINFlexD_UARTSR::DRF>(1);
													
													// update interrupt signals
													UpdateINT_RX();
												}
											}
											else
											{
												// FIFO mode
												if(!discard_message)
												{
													// latch received message into back of FIFO in BDRM
													unsigned int insert_bit_offset = 8 * rx_fifo_cnt;
													uint32_t write_mask = ((wls || wl1) ? 0xffff : 0xff) << insert_bit_offset;
													uint32_t value = rx_msg << insert_bit_offset;
													
													linflexd_bdrm = (linflexd_bdrm & ~write_mask) | (value & write_mask);
													
													// increment FIFO counter
													unsigned int rx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::RFC>();
													rx_fifo_cnt += ((wls || wl1) ? 2 : 1);
													linflexd_uartcr.template Set<typename LINFlexD_UARTCR::RFC>(rx_fifo_cnt);
													
													if(verbose)
													{
														logger << DebugInfo << rx_input.get_time_stamp() << ": Rx FIFO not empty" << EndDebugInfo;
													}

													linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RFE>(0); // Rx FIFO is not empty
													
													linflexd_uartsr.template Set<typename LINFlexD_UARTSR::RFNE>(1); // Receive FIFO Not Empty

													// Request DMA RX
													RequestDMA_RX();
												}
												
												// shift parity bit register
												rx_parity_error_reg >>= ((wls || wl1) ? 2 : 1);
											}
										
											unsigned int nef = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::NEF>(); // number of expected frame
											if(++num_of_frames == nef)
											{
												// received configured number of frames
												
												if(verbose)
												{
													logger << DebugInfo << rx_input.get_time_stamp() << ": received configured number of frames" << EndDebugInfo;
												}
												
												num_of_frames = 0;

												unsigned int dtu = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::DTU>();
												
												// if DTU bit is set, then the UART timeout counter will be reset after the configured number of frames have been received
												if(dtu)
												{
													linflexd_uartcto.Reset();
												}
											}
										}
									}
								}
								else
								{
									if(verbose)
									{
										logger << DebugInfo << rx_input.get_time_stamp() << ": framing error" << EndDebugInfo;
									}
									
									framing_error = true;
									linflexd_uartsr.template Set<typename LINFlexD_UARTSR::FEF>(1); // framing error
									
									// update interrupt signals
									UpdateINT_ERR(); // it will generate an interrupt if the FEIE bit of LINIER is set
								}
							}
							
							data_reception_in_progress = false;
							UpdateState();
						}
					}
					else
					{
						logger << DebugWarning << rx_input.get_time_stamp() << ": invalid combination of " << LINFlexD_UARTCR::WLS::GetName() << ", " << LINFlexD_UARTCR::WL1::GetName() << " and " << LINFlexD_UARTCR::RDFL_RFC::GetName() << EndDebugWarning;
					}
				}
			}
		}
		
		if(rx_input.next() == TLM_INPUT_BITSTREAM_NEED_SYNC)
		{
			wait();
			rx_input.latch();
			rx_time = sc_core::SC_ZERO_TIME;
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::TX_FIFO_Push()
{
	if(UART_TX_FIFO_Mode())
	{
		// Tx FIFO mode
		if(linflexd_uartsr.template Get<typename LINFlexD_UARTSR::TFF>()) // Tx FIFO Full ?
		{
			// If TFF bit is set and a write is performed to the FIFO, the data transmitted may be erroneous.
			logger << DebugWarning << "The data transmitted may be erroneous if " << LINFlexD_UARTSR::TFF::GetName() << " bit is set and a write is performed to the FIFO" << EndDebugWarning;
			return;
		}
		
		unsigned int wl1 = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>();
		
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": Pushing " << (wl1 ? 2 : 1) << " byte(s) onto Tx FIFO" << EndDebugInfo;
		}
		
		// Push Tx FIFO elements in BDRL, to make space
		linflexd_bdrl = linflexd_bdrl << (wl1 ? 16 : 8);
			
		// increment Tx FIFO counter
		unsigned int tx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFC>();
		tx_fifo_cnt += (wl1 ? 2 : 1);
		linflexd_uartcr.template Set<typename LINFlexD_UARTCR::TFC>(tx_fifo_cnt);

		assert(tx_fifo_cnt <= 4);
		
		if(tx_fifo_cnt >= 4)
		{
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": Tx FIFO is full" << EndDebugInfo;
			}
			
			// Tx FIFO Full
			linflexd_uartsr.template Set<typename LINFlexD_UARTSR::TFF>(1);
			
			// update interrupt signals
			UpdateINT_TX();
		}
		else
		{
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": Tx FIFO is not full" << EndDebugInfo;
			}
			// Request DMA TX
			RequestDMA_TX();
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::Transmit()
{
	if(UART_Mode())
	{
		// UART mode
		if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TxEn>()) // UART Tx enable ?
		{
			// UART Tx is enabled
			if(NormalMode())
			{
				// Normal mode
				
				bool config_err = false;
				
				unsigned int wl1 = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>();
				
				if(UART_TX_BufferMode())
				{
					// Buffer mode
					unsigned int tdfl = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TDFL>();
					config_err = (((tdfl + 1) % (wl1 ? 2 : 1)) != 0);
				}
				else
				{
					// FIFO mode
					unsigned int tx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFC>();
					config_err = ((tx_fifo_cnt % (wl1 ? 2 : 1)) != 0);
				}
				
				if(config_err)
				{
					logger << DebugWarning << "invalid combination of " << LINFlexD_UARTCR::WL1::GetName() << " and " << LINFlexD_UARTCR::TDFL_TFC::GetName() << EndDebugWarning;
				}
				else
				{
					if(UART_TX_BufferMode())
					{
						unsigned int dtf = linflexd_uartsr.template Get<typename LINFlexD_UARTSR::DTF>();
						
						if(dtf)
						{
							// software did not clear DTF
							logger << DebugWarning << "software should clear " << LINFlexD_UARTSR::DTF::GetName() << EndDebugWarning; 
						}
					
						if(pending_tx_request)
						{
							logger << DebugWarning << sc_core::sc_time_stamp() << ": there's already a pending transmission request; the data transmitted may be erroneous" << EndDebugWarning;
						}
					}
					
					pending_tx_request = true; 
					tx_event.notify(sc_core::SC_ZERO_TIME);
				}
			}
		}
	}
}

template <typename CONFIG>
void LINFlexD<CONFIG>::TX_Process()
{
	while(1)
	{
		if(!pending_tx_request)
		{
			wait(); // wait Tx event
		}
		
		pending_tx_request = false;
			
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": processing data to transmit" << EndDebugInfo;
		}

		if(LIN_ClockEnabled()) // is LIN clock enabled ?
		{
			if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TxEn>()) // UART Tx enable ?
			{
				if(NormalMode())
				{
					// Normal mode
					unsigned int rem_byte_cnt = 0;

					if(UART_TX_BufferMode())
					{
						// Buffer mode
						unsigned int tdfl = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TDFL>();
						rem_byte_cnt = tdfl + 1; // 1, 2, 3 or 4
					}
					else
					{
						// FIFO mode
						unsigned int tx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFC>();
						rem_byte_cnt = tx_fifo_cnt; // 1, 2, 3 or 4
					}

					unsigned int tx_msg_length = 8 * rem_byte_cnt; // 8, 16, 24 or 32
					
					if(tx_msg_length > 0) // is there something to transmit ?
					{
						unsigned int wl1 = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL1>();
						
						if((rem_byte_cnt % (wl1 ? 2 : 1)) == 0) // check that combination of WL and TDFL/TFC is valid
						{
							data_transmission_in_progress = true;
							UpdateState();
						
							sc_core::sc_time tx_time = sc_core::SC_ZERO_TIME;
							
							unisim::kernel::tlm2::tlm_serial_payload payload;
							unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();

							payload.set_period(baud_period);
										
							uint32_t tx_shift_reg = 0;             // 32-bit transmitter shift register
						
							unsigned int wl = (wl1 << 1) | linflexd_uartcr.template Get<typename LINFlexD_UARTCR::WL0>();
							unsigned int tdfbm = linflexd_gcr.template Get<typename LINFlexD_GCR::TDFBM>();
							unsigned int tdlis = linflexd_gcr.template Get<typename LINFlexD_GCR::TDLIS>();
							
							unsigned int tx_word_length = 0;       // Tx word length in bits
							unsigned int tx_frame_word_length = 0; // Tx frame word length in bits
							bool transmit_parity = false;          // whether to send a parity bit 

							switch(wl)
							{
								case 0: // 7 bits data + parity
									tx_frame_word_length = 7;
									transmit_parity = true;
									tx_word_length = 8;
									break;
								case 1: // 8 bits data when PCE = 0 or 8 bits data + parity when PCE = 1
									tx_frame_word_length = 8;
									transmit_parity = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PCE>();
									tx_word_length = 8;
									break;
								case 2: // 15 bits data + parity
									tx_frame_word_length = 15;
									transmit_parity = true;
									tx_word_length = 16;
									break;
								case 3: // 16 bits data when PCE = 0 or 16 bits data + parity when PCE = 1
									tx_frame_word_length = 16;
									transmit_parity = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PCE>();
									tx_word_length = 16;
									break;
							}
					
							if(verbose)
							{
								logger << DebugInfo << sc_core::sc_time_stamp() << ": transmitting a " << tx_msg_length << "-bit message with " << tx_frame_word_length << "-bit data";
								if(transmit_parity)
								{
									logger << " + parity bit";
								}
								logger << EndDebugInfo;
							}
							
							// transmitted byte order: DATA0 DATA1 DATA2 DATA3
							
							// bit order: LSB first
							//
							//             DATA3     DATA2     DATA1     DATA0
							//          +---------+---------+---------+---------+
							//   BDRL   | 31 - 24 | 23 - 16 | 15 -  8 |  7 -  0 |
							//          +---------+---------+---------+---------+
							//               |         |        |          |
							//               |         |        |          |
							//               |         |        |          |
							//          +---------+---------+---------+---------+
							//          | 31 - 24 | 23 - 16 | 15 -  8 |  7 -  0 |--> LINTX
							//          +---------+---------+---------+---------+
							//              32-bit transmitter shift register
							//
							//
							// bit order: MSB first
							//
							//             DATA7     DATA6     DATA5     DATA4
							//          +---------+---------+---------+---------+
							//   BDRL   | 31 - 24 | 23 - 16 | 15 -  8 |  7 -  0 |
							//          +---------+---------+---------+---------+
							//               D         C        B          A
							//               |         |        |          |
							//               ------------------------------> 
							//               <------------------------------  
							//               |         |        |          |      
							//               A         B        C          D
							//              32-bit transmitter shift register
							//          +---------+---------+---------+---------+
							// LINTX <--| 31 - 24 | 23 - 16 | 15 -  8 |  7 -  0 |
							//          +---------+---------+---------+---------+

							// extract message from BDRL
							uint32_t tx_msg = 0;

							if(UART_TX_BufferMode())
							{
								// Buffer mode
								
								// extract message from BDRL
								tx_msg = linflexd_bdrl & (~uint32_t(0) >> (32 - tx_msg_length));
							}
							else
							{
								// FIFO mode
								unsigned int tx_fifo_cnt = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::TFC>();
								
								// extract message word by word from BDRL in reverse order, from back to front of Tx FIFO
								unsigned int insert_bit_offset = 0; // bit offset in message where to insert extracted word
								while(tx_fifo_cnt > 0)
								{
									unsigned int extract_bit_offset = 8 * (tx_fifo_cnt - (wl1 ? 2 : 1)); // offset of right most bit of word to extract 
									uint32_t extract_mask = (wl1 ? 0xffff : 0xff); // mask of extracted word
									
									tx_msg |= (((linflexd_bdrl >> extract_bit_offset) & extract_mask) << insert_bit_offset); // extract word from BDRL and insert it into message
									
									tx_fifo_cnt -= (wl1 ? 2 : 1); // decrement Tx FIFO counter
									insert_bit_offset += (wl1 ? 16 : 8);
								}
								
								if(verbose)
								{
									logger << DebugInfo << sc_core::sc_time_stamp() << ": Tx FIFO is empty" << EndDebugInfo;
								}
								
								// Tx FIFO is empty
								linflexd_uartcr.template Set<typename LINFlexD_UARTCR::TFC>(0); // Tx FIFO count = 0

								linflexd_uartsr.template Set<typename LINFlexD_UARTSR::TFF>(0); // Tx FIFO is not full
								
								// update interrupt signals
								UpdateINT_TX();
								
								// request DMA TX
								RequestDMA_TX();
							}
							
							tx_shift_reg = tdfbm ? unisim::util::endian::ByteSwap(tx_msg) : tx_msg; // latch message into transmitter shift register

							do
							{
								data.push_back(0); // start bit
								tx_time += baud_period;

								// transmit payload
								unsigned int i;
								bool parity_bit = linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC0>();
								for(i = 0; i < tx_word_length; i++)
								{
									if(i < tx_frame_word_length)
									{
										bool bit_value = tdfbm ? ((tx_shift_reg >> 31) & 1) : (tx_shift_reg & 1);
										if(tdlis) bit_value = !bit_value; // invert transmitted data if requested
										data.push_back(bit_value); // transmit data bit
										tx_time += baud_period;
										parity_bit ^= bit_value; // update computed parity bit
									}
									
									if(tdfbm)
									{
										// MSB first
										tx_shift_reg <<= 1;
									}
									else
									{
										// LSB first
										tx_shift_reg >>= 1;
									}
								}
								
								rem_byte_cnt = ((tx_word_length / 8) < rem_byte_cnt) ? (rem_byte_cnt - (tx_word_length / 8)) : 0; // decrement remaining byte count to transmit
								
								if(transmit_parity)
								{
									if(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC1>())
									{
										// logical 0/1 always transmitted
										data.push_back(linflexd_uartcr.template Get<typename LINFlexD_UARTCR::PC0>());
									}
									else
									{
										// event/odd parity
										data.push_back(parity_bit);
									}
									
									tx_time += baud_period;
								}
								
								unsigned int num_stop_bits = linflexd_gcr.template Get<typename LINFlexD_GCR::STOP>() ? 2 : 1;
								
								do
								{
									data.push_back(1); // stop bit
									tx_time += baud_period;
								}
								while(--num_stop_bits != 0);
							}
							while(rem_byte_cnt > 0);
							
							if(verbose)
							{
								logger << DebugInfo << sc_core::sc_time_stamp() << ": transmitting [" << data << "] over LINTX with a period of " << baud_period << EndDebugInfo;
							}
							
#if 0
							sc_core::sc_time send_start_time(sc_core::sc_time_stamp());
#endif
							
							LINTX->nb_send(payload);
							
#if 0
							sc_core::sc_time send_time(sc_core::sc_time_stamp());
							send_time -= send_start_time;
							
							if(send_time > tx_time)
							{
								tx_time = send_time;
							}
							else
							{
								sc_core::sc_time rem_time(tx_time);
								rem_time -= send_time; 
								wait(rem_time);
							}
#endif
							wait(tx_time);

							if(verbose)
							{
								logger << DebugInfo << sc_core::sc_time_stamp() << ": transmission lasted " << tx_time << EndDebugInfo;
							}
							
							if(verbose)
							{
								logger << DebugInfo << sc_core::sc_time_stamp() << ": data transmission completed" << EndDebugInfo;
							}
						
							if(UART_TX_BufferMode())
							{
								linflexd_uartsr.template Set<typename LINFlexD_UARTSR::DTF>(1); // Data transmission completed
							
								// update interrupt signals
								UpdateINT_TX();
							}
						
							data_transmission_in_progress = false;
							UpdateState();
							
							// yield so that a new pending transmission request may immediately be serviced
							wait(sc_core::SC_ZERO_TIME);
						}
						else
						{
							logger << DebugError << "invalid combination of WL and TDFL/TFC values" << EndDebugError;
							unisim::kernel::Object::Stop(-1);
						}
					}
				}
			}
		}
	}
}

} // end of namespace linflexd
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_LINFLEXD_LINFLEXD_TCC__
