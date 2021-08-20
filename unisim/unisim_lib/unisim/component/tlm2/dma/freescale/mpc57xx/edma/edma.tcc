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

#ifndef __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_EDMA_EDMA_TCC__
#define __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_EDMA_EDMA_TCC__

#include <unisim/component/tlm2/dma/freescale/mpc57xx/edma/edma.hh>
#include <unisim/util/reg/core/register.tcc>
#include <unisim/util/arithmetic/arithmetic.hh>

namespace unisim {
namespace component {
namespace tlm2 {
namespace dma {
namespace freescale {
namespace mpc57xx {
namespace edma {

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::NUM_DMA_CHANNELS;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::NUM_GROUPS;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::NUM_CHANNELS_PER_GROUP;

template <typename CONFIG>
const unsigned int EDMA<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const bool EDMA<CONFIG>::threaded_model;

template <typename CONFIG>
EDMA<CONFIG>::EDMA(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, master_if("master_if")
	, m_clk("m_clk")
	, dma_clk("dma_clk")
	, reset_b("reset_b")
	, dma_channel()
	, dma_channel_ack()
	, irq()
	, err_irq()
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, dma_clk_prop_proxy(dma_clk)
	, edma_cr(this)  
	, edma_es(this)  
	, edma_erqh(this)
	, edma_erql(this)
	, edma_eeih(this)
	, edma_eeil(this)
	, edma_serq(this)
	, edma_cerq(this)
	, edma_seei(this)
	, edma_ceei(this)
	, edma_cint(this)
	, edma_cerr(this)
	, edma_ssrt(this)
	, edma_cdne(this)
	, edma_inth(this)
	, edma_intl(this)
	, edma_errh(this)
	, edma_errl(this)
	, edma_hrsh(this)
	, edma_hrsl(this)
	, edma_dchpri(this)      
	, edma_dchmid(this)      
	, edma_tcd_file(this)
	, reg_addr_map()
	, registers_registry()
	, payload_fabric()
	, schedule()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, master_id(0)
	, param_master_id("master-id", this, master_id, "master ID")
	, default_trans_attr()
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
	, dma_clock_period(10.0, sc_core::SC_NS)
	, dma_clock_start_time(sc_core::SC_ZERO_TIME)
	, dma_clock_posedge_first(true)
	, dma_clock_duty_cycle(0.5)
	, dma_engine_time(sc_core::SC_ZERO_TIME)
	, dma_engine_event("dma_engine_event")
	, gen_irq_event()
	, gen_err_event()
	, gen_dma_channel_ack_event()
	, gen_dma_channel_ack_edge()
	, grp_per_prio_error(true)
	, ch_per_prio_error()
	, sel_grp(0)
	, sel_ch_per_grp()
	, grp_per_prio()
	, ch_per_prio()
{
	default_trans_attr.set_privileged(true);
	default_trans_attr.set_instruction(false);
	default_trans_attr.set_master_id(0);
	
	peripheral_slave_if(*this);
	master_if(*this);
	
	std::stringstream description_sstr;
	description_sstr << "MPC57XX Interrupt Controller (EDMA):" << std::endl;
	description_sstr << "  - " << NUM_DMA_CHANNELS << " DMA channel(s)" << std::endl;
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 24" << std::endl;
	this->SetDescription(description_sstr.str().c_str());
	
	SC_HAS_PROCESS(EDMA);

	unsigned int dma_channel_num;
	unsigned int dma_grp_num;
	
	for(dma_grp_num = 0; dma_grp_num < NUM_GROUPS; dma_grp_num++)
	{
		ch_per_prio_error[dma_grp_num] = true;
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream dma_channel_name_sstr;
		dma_channel_name_sstr << "dma_channel_" << dma_channel_num;
		dma_channel[dma_channel_num] = new sc_core::sc_in<bool>(dma_channel_name_sstr.str().c_str()); 
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream dma_channel_ack_name_sstr;
		dma_channel_ack_name_sstr << "dma_channel_ack_" << dma_channel_num;
		dma_channel_ack[dma_channel_num] = new sc_core::sc_out<bool>(dma_channel_ack_name_sstr.str().c_str()); 
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream irq_name_sstr;
		irq_name_sstr << "irq_" << dma_channel_num;
		irq[dma_channel_num] = new sc_core::sc_out<bool>(irq_name_sstr.str().c_str()); 
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream err_irq_name_sstr;
		err_irq_name_sstr << "err_irq_" << dma_channel_num;
		err_irq[dma_channel_num] = new sc_core::sc_out<bool>(err_irq_name_sstr.str().c_str()); 
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream gen_irq_event_name_sstr;
		gen_irq_event_name_sstr << "gen_irq_event_" << dma_channel_num;
		gen_irq_event[dma_channel_num] = new sc_core::sc_event(gen_irq_event_name_sstr.str().c_str());
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream gen_err_event_name_sstr;
		gen_err_event_name_sstr << "gen_err_event_" << dma_channel_num;
		gen_err_event[dma_channel_num] = new sc_core::sc_event(gen_err_event_name_sstr.str().c_str());
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		std::stringstream gen_dma_channel_ack_event_name_sstr;
		gen_dma_channel_ack_event_name_sstr << "gen_dma_channel_ack_event_" << dma_channel_num;
		gen_dma_channel_ack_event[dma_channel_num] = new sc_core::sc_event(gen_dma_channel_ack_event_name_sstr.str().c_str());
	}

	// Map EDMA registers regarding there address offsets
	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	reg_addr_map.MapRegister(edma_cr  .ADDRESS_OFFSET, &edma_cr  ); 
	reg_addr_map.MapRegister(edma_es  .ADDRESS_OFFSET, &edma_es  ); 
	reg_addr_map.MapRegister(edma_erqh.ADDRESS_OFFSET, &edma_erqh);
	reg_addr_map.MapRegister(edma_erql.ADDRESS_OFFSET, &edma_erql);
	reg_addr_map.MapRegister(edma_eeih.ADDRESS_OFFSET, &edma_eeih);
	reg_addr_map.MapRegister(edma_eeil.ADDRESS_OFFSET, &edma_eeil);
	reg_addr_map.MapRegister(edma_serq.ADDRESS_OFFSET, &edma_serq);
	reg_addr_map.MapRegister(edma_cerq.ADDRESS_OFFSET, &edma_cerq);
	reg_addr_map.MapRegister(edma_seei.ADDRESS_OFFSET, &edma_seei);
	reg_addr_map.MapRegister(edma_ceei.ADDRESS_OFFSET, &edma_ceei);
	reg_addr_map.MapRegister(edma_cint.ADDRESS_OFFSET, &edma_cint);
	reg_addr_map.MapRegister(edma_cerr.ADDRESS_OFFSET, &edma_cerr);
	reg_addr_map.MapRegister(edma_ssrt.ADDRESS_OFFSET, &edma_ssrt);
	reg_addr_map.MapRegister(edma_cdne.ADDRESS_OFFSET, &edma_cdne);
	reg_addr_map.MapRegister(edma_inth.ADDRESS_OFFSET, &edma_inth);
	reg_addr_map.MapRegister(edma_intl.ADDRESS_OFFSET, &edma_intl);
	reg_addr_map.MapRegister(edma_errh.ADDRESS_OFFSET, &edma_errh);
	reg_addr_map.MapRegister(edma_errl.ADDRESS_OFFSET, &edma_errl);
	reg_addr_map.MapRegister(edma_hrsh.ADDRESS_OFFSET, &edma_hrsh);
	reg_addr_map.MapRegister(edma_hrsl.ADDRESS_OFFSET, &edma_hrsl);
	reg_addr_map.MapRegisterFile(EDMA_DCHPRI      ::ADDRESS_OFFSET, &edma_dchpri      );
	reg_addr_map.MapRegisterFile(EDMA_DCHMID      ::ADDRESS_OFFSET, &edma_dchmid      );
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_SADDR   ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_saddr   );
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_ATTR    ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_attr    );
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_SOFF    ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_soff    );
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_NBYTES  ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_nbytes  );
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_SLAST   ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_slast   );
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_DADDR   ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_daddr   );
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_CITER   ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_citer   );
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_DOFF    ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_doff    );
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_DLASTSGA::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_dlastsga);
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_BITER   ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_biter   );
		reg_addr_map.MapRegister(EDMA_TCD::ADDRESS_OFFSET + EDMA_TCD_CSR     ::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), &edma_tcd_file[dma_channel_num].edma_tcd_csr     );
	}
	
	registers_registry.AddRegisterInterface(edma_cr  .CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_es  .CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_erqh.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_erql.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_eeih.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_eeil.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_serq.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_cerq.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_seei.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_ceei.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_cint.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_cerr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_ssrt.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_cdne.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_inth.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_intl.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_errh.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_errl.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_hrsh.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(edma_hrsl.CreateRegisterInterface());
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		registers_registry.AddRegisterInterface(edma_dchpri[dma_channel_num].CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_dchmid[dma_channel_num].CreateRegisterInterface());
		
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_saddr   .CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_attr    .CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_soff    .CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_nbytes  .CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_slast   .CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_daddr   .CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_citer   .CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_doff    .CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_dlastsga.CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_biter   .CreateRegisterInterface());
		registers_registry.AddRegisterInterface(edma_tcd_file[dma_channel_num].edma_tcd_csr     .CreateRegisterInterface());
	}
	
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
	
	SC_METHOD(RESET_B_Process);
	sensitive << reset_b.pos();
	
	SC_THREAD(DMA_Engine_Process);
	sensitive << dma_engine_event;
	
	// Spawn an DMA_CHANNEL_Process for each DMA channel
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		sc_core::sc_spawn_options dma_channel_process_spawn_options;
		dma_channel_process_spawn_options.spawn_method();
		dma_channel_process_spawn_options.set_sensitivity(dma_channel[dma_channel_num]);
		dma_channel_process_spawn_options.set_sensitivity(dma_channel_ack[dma_channel_num]);
		
		std::stringstream dma_channel_process_name_sstr;
		dma_channel_process_name_sstr << "DMA_CHANNEL_Process_" << dma_channel_num;
		sc_core::sc_spawn(sc_bind(&EDMA<CONFIG>::DMA_CHANNEL_Process, this, dma_channel_num), dma_channel_process_name_sstr.str().c_str(), &dma_channel_process_spawn_options);
	}
	
	// Spawn an IRQ_Process for each DMA channel
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		sc_core::sc_spawn_options irq_process_spawn_options;
		irq_process_spawn_options.spawn_method();
		irq_process_spawn_options.set_sensitivity(gen_irq_event[dma_channel_num]);
		
		std::stringstream irq_process_name_sstr;
		irq_process_name_sstr << "IRQ_Process_" << dma_channel_num;
		sc_core::sc_spawn(sc_bind(&EDMA<CONFIG>::IRQ_Process, this, dma_channel_num), irq_process_name_sstr.str().c_str(), &irq_process_spawn_options);
	}

	// Spawn an ERR_IRQ_Process for each DMA channel
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		sc_core::sc_spawn_options err_irq_process_spawn_options;
		err_irq_process_spawn_options.spawn_method();
		err_irq_process_spawn_options.set_sensitivity(gen_err_event[dma_channel_num]);
		
		std::stringstream err_irq_process_name_sstr;
		err_irq_process_name_sstr << "ERR_IRQ_Process_" << dma_channel_num;
		sc_core::sc_spawn(sc_bind(&EDMA<CONFIG>::ERR_IRQ_Process, this, dma_channel_num), err_irq_process_name_sstr.str().c_str(), &err_irq_process_spawn_options);
	}

	// Spawn an DMA_CHANNEL_ACK_Process for each DMA channel
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		sc_core::sc_spawn_options dma_channel_ack_process_spawn_options;
		dma_channel_ack_process_spawn_options.spawn_method();
		dma_channel_ack_process_spawn_options.set_sensitivity(gen_dma_channel_ack_event[dma_channel_num]);
		
		std::stringstream dma_channel_ack_process_name_sstr;
		dma_channel_ack_process_name_sstr << "DMA_CHANNEL_ACK_Process_" << dma_channel_num;
		sc_core::sc_spawn(sc_bind(&EDMA<CONFIG>::DMA_CHANNEL_ACK_Process, this, dma_channel_num), dma_channel_ack_process_name_sstr.str().c_str(), &dma_channel_ack_process_spawn_options);
	}
}

template <typename CONFIG>
EDMA<CONFIG>::~EDMA()
{
	unsigned int dma_channel_num;
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete dma_channel[dma_channel_num];
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete dma_channel_ack[dma_channel_num];
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete irq[dma_channel_num];
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete err_irq[dma_channel_num];
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete gen_irq_event[dma_channel_num];
	}

	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete gen_err_event[dma_channel_num];
	}
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		delete gen_dma_channel_ack_event[dma_channel_num];
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;
	
	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&EDMA<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	// Spawn DMAClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options dma_clock_properties_changed_process_spawn_options;
	
	dma_clock_properties_changed_process_spawn_options.spawn_method();
	dma_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&EDMA<CONFIG>::DMAClockPropertiesChangedProcess, this), "DMAClockPropertiesChangedProcess", &dma_clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void EDMA<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool EDMA<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int EDMA<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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

	tlm_trans_attr *trans_attr = payload.template get_extension<tlm_trans_attr>();
	if(!trans_attr) trans_attr = &default_trans_attr;
	
	switch(cmd)
	{
		case tlm::TLM_WRITE_COMMAND:
			return reg_addr_map.DebugWrite(*trans_attr, start_addr, data_ptr, data_length);
		case tlm::TLM_READ_COMMAND:
			return reg_addr_map.DebugRead(*trans_attr, start_addr, data_ptr, data_length);
		default:
			break;
	}
	
	return 0;
}

template <typename CONFIG>
tlm::tlm_sync_enum EDMA<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
tlm::tlm_sync_enum EDMA<CONFIG>::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
	if(phase == tlm::BEGIN_RESP)
	{
		return tlm::TLM_COMPLETED;
	}
	return tlm::TLM_ACCEPTED;
}

template <typename CONFIG>
void EDMA<CONFIG>::invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range)
{
}

//////////////// unisim::service::interface::Registers ////////////////////

template <typename CONFIG>
unisim::service::interfaces::Register *EDMA<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void EDMA<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}

template <typename CONFIG>
void EDMA<CONFIG>::ProcessEvent(Event *event)
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
			tlm_trans_attr *trans_attr = event->GetAttributes();
			if(!trans_attr) trans_attr = &default_trans_attr;
			
			switch(cmd)
			{
				case tlm::TLM_WRITE_COMMAND:
					rws = reg_addr_map.Write(*trans_attr, start_addr, data_ptr, data_length);
					break;
				case tlm::TLM_READ_COMMAND:
					rws = reg_addr_map.Read(*trans_attr, start_addr, data_ptr, data_length);
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
void EDMA<CONFIG>::ProcessEvents()
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
void EDMA<CONFIG>::Process()
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
void EDMA<CONFIG>::Reset()
{
	unsigned int i, j;
	grp_per_prio_error = true;
	sel_grp = 0;
	for(i = 0; i < NUM_CHANNELS_PER_GROUP; i++)
	{
		sel_ch_per_grp[i] = 0;
	}
	for(i = 0; i < NUM_GROUPS; i++)
	{
		grp_per_prio[i] = 0;
		ch_per_prio_error[i] = true;
		
		for(j = 0; j < NUM_CHANNELS_PER_GROUP; j++)
		{
			ch_per_prio[i][j] = 0;
		}
	}

	edma_cr  .Reset();
	edma_es  .Reset();
	edma_erqh.Reset();
	edma_erql.Reset();
	edma_eeih.Reset();
	edma_eeil.Reset();
	edma_serq.Reset();
	edma_cerq.Reset();
	edma_seei.Reset();
	edma_ceei.Reset();
	edma_cint.Reset();
	edma_cerr.Reset();
	edma_ssrt.Reset();
	edma_cdne.Reset();
	edma_inth.Reset();
	edma_intl.Reset();
	edma_errh.Reset();
	edma_errl.Reset();
	edma_hrsh.Reset();
	edma_hrsl.Reset();
	
	unsigned int dma_channel_num;
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		edma_dchpri      [dma_channel_num].Reset();
		edma_dchmid      [dma_channel_num].Reset();
		
		gen_dma_channel_ack_edge[dma_channel_num] = NEG;
		UpdateAcknowledge(dma_channel_num);
	}

	edma_tcd_file.Reset();

	channel_tcd = 0;
	
	schedule.Clear();
	
	if(!m_clk_prop_proxy.IsClockCompatible())
	{
		logger << DebugError << "clock port is not bound to a unisim::kernel::tlm2::Clock" << EndDebugError;
		unisim::kernel::Object::Stop(-1);
		return;
	}
	
	UpdateMasterClock();
}

template <typename CONFIG>
void EDMA<CONFIG>::EnableAllRequests()
{
	edma_erqh = ~uint32_t(0);
	edma_erql = ~uint32_t(0);
	UpdateAllHardwareRequestStatus();
}

template <typename CONFIG>
void EDMA<CONFIG>::EnableRequest(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": enabling request #" << dma_channel_num << EndDebugInfo;
	}
	
	if(dma_channel_num >= 32)
	{
		edma_erqh.Set(dma_channel_num - 32, 1);
	}
	else
	{
		edma_erql.Set(dma_channel_num, 1);
	}
	
	UpdateHardwareRequestStatus(dma_channel_num);
}

template <typename CONFIG>
void EDMA<CONFIG>::DisableAllRequests()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": disabling all requests" << EndDebugInfo;
	}
	
	edma_erqh = 0;
	edma_erql = 0;
	
	UpdateAllHardwareRequestStatus();
}

template <typename CONFIG>
void EDMA<CONFIG>::DisableRequest(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": disabling request #" << dma_channel_num << EndDebugInfo;
	}

	if(dma_channel_num >= 32)
	{
		edma_erqh.Set(dma_channel_num - 32, 0);
	}
	else
	{
		edma_erql.Set(dma_channel_num, 0);
	}
	
	UpdateHardwareRequestStatus(dma_channel_num);
}

template <typename CONFIG>
void EDMA<CONFIG>::EnableAllErrorInterrupts()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": enabling all error interrupts" << EndDebugInfo;
	}
	
	edma_eeih = ~uint32_t(0);
	edma_eeil = ~uint32_t(0);
	
	UpdateAllHardwareRequestStatus();
}

template <typename CONFIG>
void EDMA<CONFIG>::EnableErrorInterrupt(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": enabling error interrupt #" << dma_channel_num << EndDebugInfo;
	}

	if(dma_channel_num >= 32)
	{
		edma_eeih.Set(dma_channel_num - 32, 1);
	}
	else
	{
		edma_eeil.Set(dma_channel_num, 1);
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::DisableAllErrorInterrupts()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": disable all error interrupts" << EndDebugInfo;
	}
	
	edma_eeih = 0;
	edma_eeil = 0;
}

template <typename CONFIG>
void EDMA<CONFIG>::DisableErrorInterrupt(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": disable error interrupt #" << dma_channel_num << EndDebugInfo;
	}
	
	if(dma_channel_num >= 32)
	{
		edma_eeih.Set(dma_channel_num - 32, 0);
	}
	else
	{
		edma_eeil.Set(dma_channel_num, 0);
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::SetInterruptRequest(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": setting interrupt request #" << dma_channel_num << EndDebugInfo;
	}

	if(dma_channel_num >= 32)
	{
		edma_inth.Set(dma_channel_num - 32, 1);
	}
	else
	{
		edma_intl.Set(dma_channel_num, 1);
	}
	
	UpdateInterruptRequest(dma_channel_num);
}

template <typename CONFIG>
void EDMA<CONFIG>::ClearAllInterruptRequests()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": clearing all interrupt requests" << EndDebugInfo;
	}
	
	edma_inth = 0;
	edma_intl = 0;
}

template <typename CONFIG>
void EDMA<CONFIG>::ClearInterruptRequest(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": clearing interrupt request #" << dma_channel_num << EndDebugInfo;
	}

	if(dma_channel_num >= 32)
	{
		edma_inth.Set(dma_channel_num - 32, 0);
	}
	else
	{
		edma_intl.Set(dma_channel_num, 0);
	}
	
	UpdateInterruptRequest(dma_channel_num);
}

template <typename CONFIG>
void EDMA<CONFIG>::SetErrorIndicator(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": setting error indicator #" << dma_channel_num << EndDebugInfo;
	}

	if(dma_channel_num >= 32)
	{
		edma_errh.Set(dma_channel_num - 32, 1);
	}
	else
	{
		edma_errl.Set(dma_channel_num, 1);
	}
	
	edma_es.template Set<typename EDMA_ES::ERRCHN>(dma_channel_num);
	
	UpdateVLD();
	UpdateError(dma_channel_num);
}

template <typename CONFIG>
bool EDMA<CONFIG>::ErrorIndicator(unsigned int dma_channel_num)
{
	return (dma_channel_num >= 32) ? edma_errh.Get(dma_channel_num - 32) : edma_errl.Get(dma_channel_num);
}

template <typename CONFIG>
void EDMA<CONFIG>::ClearAllErrorIndicators()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": clearing all error indicators" << EndDebugInfo;
	}
	
	edma_errh = 0;
	edma_errl = 0;
	UpdateVLD();
	
	unsigned int dma_channel_num;
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		UpdateError(dma_channel_num);
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::ClearErrorIndicator(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": clear error indicator #" << dma_channel_num << EndDebugInfo;
	}

	if(dma_channel_num >= 32)
	{
		edma_errh.Set(dma_channel_num - 32, 0);
	}
	else
	{
		edma_errl.Set(dma_channel_num, 0);
	}
	
	UpdateVLD();
	UpdateError(dma_channel_num);
}

template <typename CONFIG>
void EDMA<CONFIG>::SetAllStartBits()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": setting all START bits" << EndDebugInfo;
	}

	unsigned int dma_channel_num;
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::START>(1);
	}
	
	NotifyEngine();
}

template <typename CONFIG>
void EDMA<CONFIG>::SetStartBit(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": setting START bit #" << dma_channel_num << EndDebugInfo;
	}

	edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::START>(1);
	NotifyEngine();
}

template <typename CONFIG>
void EDMA<CONFIG>::ClearAllDoneBits()
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": clearing all DONE bits" << EndDebugInfo;
	}

	unsigned int dma_channel_num;
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::DONE>(0);
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::ClearDoneBit(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": clearing DONE bit #" << dma_channel_num << EndDebugInfo;
	}
	
	edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::DONE>(0);
}

template <typename CONFIG>
bool EDMA<CONFIG>::HardwareRequestStatus(unsigned int dma_channel_num)
{
	bool dma_channel_value = dma_channel[dma_channel_num]->read();
	bool dma_channel_ack_value = dma_channel_ack[dma_channel_num]->read();
	bool enable_request_value = ((dma_channel_num >= 32) ? edma_erqh.Get(dma_channel_num - 32) : edma_erql.Get(dma_channel_num));

#if 0
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << dma_channel[dma_channel_num]->name() << " = " << dma_channel_value << EndDebugInfo;
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << dma_channel_ack[dma_channel_num]->name() << " = " << dma_channel_ack_value << EndDebugInfo;
	}
#endif
	
	return enable_request_value && dma_channel_value && !dma_channel_ack_value;
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateAllHardwareRequestStatus()
{
	unsigned int dma_channel_num;
	
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		UpdateHardwareRequestStatus(dma_channel_num);
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateHardwareRequestStatus(unsigned int dma_channel_num)
{
	bool hw_req_status = HardwareRequestStatus(dma_channel_num);
	
	// Mirror Hardware request status in HRSH and HRSL registers
	if(dma_channel_num >= 32)
	{
		edma_hrsh.Set(dma_channel_num - 32, hw_req_status);
	}
	else
	{
		edma_hrsl.Set(dma_channel_num, hw_req_status);
	}
	
	if(hw_req_status)
	{
		// Wake-up engine if a request is pending
		NotifyEngine();
	}
}

template <typename CONFIG>
bool EDMA<CONFIG>::RequestStatus(unsigned int dma_channel_num)
{
	return edma_tcd_file[dma_channel_num].edma_tcd_csr.template Get<typename EDMA_TCD_CSR::START>() || HardwareRequestStatus(dma_channel_num);
}

template <typename CONFIG>
bool EDMA<CONFIG>::InterruptRequestStatus(unsigned int dma_channel_num)
{
	if(dma_channel_num >= 32)
	{
		return edma_inth.Get(dma_channel_num - 32);
	}
	else
	{
		return edma_intl.Get(dma_channel_num);
	}
}

template <typename CONFIG>
bool EDMA<CONFIG>::ErrorStatus(unsigned int dma_channel_num)
{
	if(dma_channel_num >= 32)
	{
		return edma_errh.Get(dma_channel_num - 32);
	}
	else
	{
		return edma_errl.Get(dma_channel_num);
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateInterruptRequests()
{
	unsigned int dma_channel_num;
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		UpdateInterruptRequest(dma_channel_num);
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateInterruptRequest(unsigned int dma_channel_num)
{
	gen_irq_event[dma_channel_num]->notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateErrors()
{
	unsigned int dma_channel_num;
	for(dma_channel_num = 0; dma_channel_num < NUM_DMA_CHANNELS; dma_channel_num++)
	{
		UpdateError(dma_channel_num);
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateError(unsigned int dma_channel_num)
{
	gen_err_event[dma_channel_num]->notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateAcknowledge(unsigned int dma_channel_num, const sc_core::sc_time& delay)
{
	gen_dma_channel_ack_event[dma_channel_num]->notify(delay);
}

template <typename CONFIG>
void EDMA<CONFIG>::Acknowledge(unsigned int dma_channel_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":channel #" << dma_channel_num << ": acknowledging" << EndDebugInfo;
	}
	
	gen_dma_channel_ack_edge[dma_channel_num] = POS;
	UpdateAcknowledge(dma_channel_num);
}

template <typename CONFIG>
void EDMA<CONFIG>::NotifyEngine()
{
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": notifying DMA engine" << EndDebugInfo;
	}
	
	dma_engine_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateVLD()
{
	edma_es.template Set<typename EDMA_ES::VLD>((edma_errh != 0) || (edma_errl != 0));
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateChannelPriority(unsigned int dma_channel_num)
{
	unsigned int prio_coverage_map = 0;
	unsigned int i;
	unsigned int dma_grp_num = dma_channel_num / NUM_CHANNELS_PER_GROUP;
	for(i = 0; i < NUM_CHANNELS_PER_GROUP; i++)
	{
		unsigned int ch = (NUM_CHANNELS_PER_GROUP * dma_grp_num) + i;
		unsigned int prio = edma_dchpri[ch].template Get<typename EDMA_DCHPRI::CHPRI>();
		prio_coverage_map |= (1 << prio);
		ch_per_prio[dma_grp_num][prio] = ch;
	}
	
	ch_per_prio_error[dma_grp_num] = (prio_coverage_map != ((1 << NUM_CHANNELS_PER_GROUP) - 1));
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateGroupPriority()
{
	unsigned int prio_coverage_map = 0;
	unsigned int grppri = edma_cr.template Get<typename EDMA_CR::GRPPRI>();
	unsigned int grp;
	for(grp = 0; grp < NUM_GROUPS; grp++)
	{
		unsigned int prio = (grppri >> (CeilLog2<NUM_GROUPS>::value * grp)) % NUM_GROUPS;
		prio_coverage_map |= (1 << prio);
		grp_per_prio[prio] = grp;
	}
	
	grp_per_prio_error = (prio_coverage_map != ((1 << NUM_GROUPS) - 1));
}

template <typename CONFIG>
bool EDMA<CONFIG>::SelectChannel(unsigned int dma_grp_num, unsigned int& dma_channel_num, bool preempt)
{
	if(edma_cr.template Get<typename EDMA_CR::ERCA>()) // round-robin channel arbitration ?
	{
		// round-robin channel arbitration
		unsigned int i;
		unsigned int ch = sel_ch_per_grp[dma_grp_num];
		for(i = 0; i < NUM_CHANNELS_PER_GROUP; i++)
		{
			ch = (ch + 1) % NUM_CHANNELS_PER_GROUP;
			
			if(RequestStatus(ch) && !ErrorIndicator(ch))
			{
				dma_channel_num = sel_ch_per_grp[dma_grp_num] = ch;
				return true;
			}
		}
	}
	else if(ch_per_prio_error[dma_grp_num])
	{
		// channel priority error
		if(verbose)
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ":channel priority error" << EndDebugWarning;
		}
		edma_es.template Set<typename EDMA_ES::CPE>(1);
	}
	else // fixed-priority channel arbitration
	{
		int prio;
		for(prio = (int)(NUM_CHANNELS_PER_GROUP - 1); prio >= 0; prio--)
		{
			unsigned int ch = ch_per_prio[dma_grp_num][prio];
			
			if(!preempt || edma_dchpri[ch].template Get<typename EDMA_DCHPRI::DPA>()) // not preempting or preempt ability ?
			{
				if(RequestStatus(ch) && !ErrorIndicator(ch))
				{
					dma_channel_num = sel_ch_per_grp[dma_grp_num] = ch;
					return true;
				}
			}
		}
	}
	
	return false;
}

template <typename CONFIG>
bool EDMA<CONFIG>::SelectChannel(unsigned int& dma_channel_num, bool preempt)
{
	if(edma_cr.template Get<typename EDMA_CR::ERGA>()) // round-robin group arbitration ?
	{
		// round-robin group arbitration
		unsigned int i;
		unsigned int grp = sel_grp;
		for(i = 0; i < NUM_GROUPS; i++)
		{
			grp = (grp + 1) % NUM_GROUPS;
			
			if(SelectChannel(grp, dma_channel_num))
			{
				sel_grp = grp;
				return true;
			}
		}
	}
	else if(grp_per_prio_error)
	{
		// group priority error
		logger << DebugWarning << sc_core::sc_time_stamp() << ":group priority error" << EndDebugWarning;
		edma_es.template Set<typename EDMA_ES::GPE>(1);
	}
	else
	{
		// fixed-priority group arbitration
		int prio;
		
		for(prio = (int)(NUM_GROUPS - 1); prio >= 0; prio--)
		{
			unsigned int grp = grp_per_prio[prio];
			if(SelectChannel(grp, dma_channel_num, preempt))
			{
				sel_grp = grp;
				return true;
			}
		}
	}
	
	return false;
}

template <typename CONFIG>
void EDMA<CONFIG>::RESET_B_Process()
{
	if(reset_b.posedge())
	{
		Reset();
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::DMA_CHANNEL_Process(unsigned int dma_channel_num)
{
	UpdateHardwareRequestStatus(dma_channel_num);
}

template <typename CONFIG>
void EDMA<CONFIG>::IRQ_Process(unsigned int dma_channel_num)
{
	bool irq_value = InterruptRequestStatus(dma_channel_num);
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << irq[dma_channel_num]->name() << " <- " << irq_value << EndDebugInfo;
	}
	
	irq[dma_channel_num]->write(irq_value);
}

template <typename CONFIG>
void EDMA<CONFIG>::ERR_IRQ_Process(unsigned int dma_channel_num)
{
	bool err_status = ErrorStatus(dma_channel_num);
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << err_irq[dma_channel_num]->name() << " <- " << err_status << EndDebugInfo;
	}
	
	err_irq[dma_channel_num]->write(err_status);
}

template <typename CONFIG>
void EDMA<CONFIG>::DMA_CHANNEL_ACK_Process(unsigned int dma_channel_num)
{
	bool dma_channel_ack_value = false;
	
	if(gen_dma_channel_ack_edge[dma_channel_num] == POS)
	{
		dma_channel_ack_value = true;
		gen_dma_channel_ack_edge[dma_channel_num] = NEG;
		UpdateAcknowledge(dma_channel_num, master_clock_period);
	}
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << dma_channel_ack[dma_channel_num]->name() << " <- " << dma_channel_ack_value << EndDebugInfo;
	}
	dma_channel_ack[dma_channel_num]->write(dma_channel_ack_value);
}

template <typename CONFIG>
void EDMA<CONFIG>::SetMasterID(unsigned int dma_channel_num, MasterID mid)
{
	edma_dchmid[dma_channel_num].template Set<typename EDMA_DCHMID::MID>(mid);
}

template <typename CONFIG>
void EDMA<CONFIG>::SetPrivilegeAccessLevel(unsigned int dma_channel_num, PrivilegeAccessLevel pal)
{
	edma_dchmid[dma_channel_num].template Set<typename EDMA_DCHMID::PAL>(pal);
}

template <typename CONFIG>
typename EDMA<CONFIG>::MasterID EDMA<CONFIG>::GetMasterID(unsigned int dma_channel_num)
{
	if(edma_dchmid[dma_channel_num].template Get<typename EDMA_DCHMID::EMI>()) // master ID replication ?
	{
		return edma_dchmid[dma_channel_num].template Get<typename EDMA_DCHMID::MID>();
	}
	else
	{
		return master_id;
	}
}

template <typename CONFIG>
typename EDMA<CONFIG>::PrivilegeAccessLevel EDMA<CONFIG>::GetPrivilegeAccessLevel(unsigned int dma_channel_num)
{
	return (typename EDMA<CONFIG>::PrivilegeAccessLevel) edma_dchmid[dma_channel_num].template Get<typename EDMA_DCHMID::PAL>();
}

template <typename CONFIG>
bool EDMA<CONFIG>::Transfer(tlm::tlm_command cmd, MasterID mid, PrivilegeAccessLevel pal, uint32_t& addr, uint8_t *data_ptr, unsigned int size, unsigned int tsize, int32_t addr_signed_offset, uint32_t addr_mask, sc_core::sc_time& t)
{
	while(size > 0)
	{
		if(verbose)
		{
			logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": ";
			switch(cmd)
			{
				case tlm::TLM_READ_COMMAND: logger << "reading"; break;
				case tlm::TLM_WRITE_COMMAND: logger << "writing"; break;
				default: logger << "?"; break;
			}
			logger << " @0x" << std::hex << addr << std::dec << " (" << tsize << " bytes)" << EndDebugInfo;
		}
		
		tlm::tlm_generic_payload *payload = payload_fabric.allocate();
		
		payload->set_address(addr);
		payload->set_data_length(tsize);
		payload->set_streaming_width(tsize);
		payload->set_data_ptr(data_ptr);
		payload->set_command(cmd);
		
		unisim::kernel::tlm2::tlm_trans_attr *trans_attr = 0;
		payload->get_extension(trans_attr);
		if(!trans_attr)
		{
			trans_attr = new unisim::kernel::tlm2::tlm_trans_attr();
			payload->set_extension(trans_attr);
		}
		trans_attr->set_master_id(mid);
		trans_attr->set_instruction(false);
		trans_attr->set_privileged(pal == PAL_PRIVILEGED_PROTECTION_LEVEL);

		master_if->b_transport(*payload, t);
		
		tlm::tlm_response_status resp_status = payload->get_response_status();
		
		payload->release();
		
		if(resp_status != tlm::TLM_OK_RESPONSE)
		{
			t += dma_clock_period;
			return false;
		}
		
		addr = (addr & ~addr_mask) | ((addr + addr_signed_offset) & addr_mask);
		size -= tsize;
		data_ptr += tsize;
	}
	
	return true;
}

template <typename CONFIG>
bool EDMA<CONFIG>::LoadTCD(unsigned int dma_channel_num, sc_dt::uint64 addr, sc_core::sc_time& t)
{
	uint8_t data[EDMA_TCD::SIZE];
	tlm::tlm_generic_payload *payload = payload_fabric.allocate();
	
	payload->set_address(addr);
	payload->set_data_length(EDMA_TCD::SIZE);
	payload->set_streaming_width(EDMA_TCD::SIZE);
	payload->set_data_ptr((unsigned char *) data);
	payload->set_command(tlm::TLM_READ_COMMAND);
	
	unisim::kernel::tlm2::tlm_trans_attr *trans_attr = 0;
	payload->get_extension(trans_attr);
	if(!trans_attr)
	{
		trans_attr = new unisim::kernel::tlm2::tlm_trans_attr();
		payload->set_extension(trans_attr);
	}
	trans_attr->set_master_id(master_id);
	trans_attr->set_instruction(false);
	trans_attr->set_privileged(true);
	
	master_if->b_transport(*payload, t);
	
	tlm::tlm_response_status resp_status = payload->get_response_status();
	
	payload->release();
	
	if(resp_status != tlm::TLM_OK_RESPONSE)
	{
		t += dma_clock_period;
		return false;
	}
	
	tlm_trans_attr int_trans_attr;
	int_trans_attr.set_master_id(GetMasterID(dma_channel_num)); // Note:  operation does not affect the DCHMIDn registers
	int_trans_attr.set_privileged(true);
	int_trans_attr.set_instruction(false);
	/*ReadWriteStatus rws =*/ reg_addr_map.Write(int_trans_attr, EDMA_TCD::ADDRESS_OFFSET + (EDMA_TCD::SIZE * dma_channel_num), data, EDMA_TCD::SIZE);
	
	return true;
}

template <typename CONFIG>
bool EDMA<CONFIG>::CheckTCD(EDMA_TCD& tcd)
{
	bool check_status = true;
	
	if(!tcd.CheckSourceAddress())
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":channel #" << tcd.dma_channel_num << ": source address configuration error" << EndDebugWarning;
		
		edma_es.template Set<typename EDMA_ES::SAE>(1);
		check_status = false;
	}
	
	if(!tcd.CheckSourceOffset())
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":channel #" << tcd.dma_channel_num << ": source offset configuration error" << EndDebugWarning;
		
		edma_es.template Set<typename EDMA_ES::SOE>(1);
		check_status = false;
	}

	if(!tcd.CheckDestinationAddress())
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":channel #" << tcd.dma_channel_num << ": destination address configuration error" << EndDebugWarning;
		
		edma_es.template Set<typename EDMA_ES::DAE>(1);
		check_status = false;
	}
	
	if(!tcd.CheckDestinationOffset())
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":channel #" << tcd.dma_channel_num << ": destination offset configuration error" << EndDebugWarning;
		
		edma_es.template Set<typename EDMA_ES::DOE>(1);
		check_status = false;
	}
	
	if(!tcd.CheckMinorLoopByteCount())
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ":channel #" << tcd.dma_channel_num << ": minor loop byte count configuration error" << EndDebugWarning;
		
		edma_es.template Set<typename EDMA_ES::NCE>(1);
		check_status = false;
	}
	
	if(!check_status)
	{
		SetErrorIndicator(tcd.dma_channel_num);
	}
	
	return check_status;
}

template <typename CONFIG>
void EDMA<CONFIG>::DMA_Engine_Process()
{
	while(1)
	{
		if(channel_tcd) // is there an active channel?
		{
			if(channel_tcd == &channel_x_tcd) // is channel x active?
			{
				// channel x is active
				if(edma_dchpri[channel_tcd->dma_channel_num].template Get<typename EDMA_DCHPRI::ECP>())
				{
					// channel x preemption is enabled
					if(!edma_cr.template Get<typename EDMA_CR::ERCA>() && !edma_cr.template Get<typename EDMA_CR::ERGA>())
					{
						// Fixed-priority arbitration for both groups and channels
						
						// give a chance to another channel to preempt channel x
						//wait(sc_core::SC_ZERO_TIME);
						
						// Select a channel that have preempt ability
						unsigned int dma_channel_num = 0;
						if(SelectChannel(dma_channel_num, /* preempt */ true))
						{
							dma_engine_time += dma_clock_period;
							dma_engine_time += dma_clock_period;
							
							// a channel can be selected
							if(dma_channel_num != channel_tcd->dma_channel_num)
							{
								// a channel different from current channel can be selected
								if(edma_dchpri[dma_channel_num].template Get<typename EDMA_DCHPRI::DPA>())
								{
									// new channel have preempt ability
								
									if(verbose)
									{
										logger << DebugInfo << sc_core::sc_time_stamp() << ":channel #" << channel_tcd->dma_channel_num << ": channel #" << dma_channel_num << " preempts" << EndDebugInfo;
									}
									
									Acknowledge(dma_channel_num);
									wait(sc_core::SC_ZERO_TIME);
									
									// preempt channel x
									
									// check TCD
									if(CheckTCD(edma_tcd_file[dma_channel_num]))
									{
										// start channel y
										dma_engine_time += dma_clock_period;
										dma_engine_time += dma_clock_period;
										if(verbose)
										{
											logger << DebugInfo << sc_core::sc_time_stamp() << ":channel #" << dma_channel_num << ": beginning" << EndDebugInfo;
										}
										
										edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::ACTIVE>(1); // channel y is active
										
										channel_y_tcd = edma_tcd_file[dma_channel_num];
										channel_tcd = &channel_y_tcd;
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			if(edma_cr.template Get<typename EDMA_CR::HALT>()) // halt DMA operations ?
			{
				if(verbose)
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ": DMA operations halted" << EndDebugInfo;
				}
			}
			else
			{
				// Select a DMA channel
				unsigned int dma_channel_num = 0;
				if(SelectChannel(dma_channel_num))
				{
					dma_engine_time += dma_clock_period;
					dma_engine_time += dma_clock_period;
					
					if(verbose)
					{
						logger << DebugInfo << sc_core::sc_time_stamp() << ": selecting channel #" << dma_channel_num << EndDebugInfo;
					}
					
					if(CheckTCD(edma_tcd_file[dma_channel_num]))
					{
						// Start channel x
						dma_engine_time += dma_clock_period;
						dma_engine_time += dma_clock_period;
						
						if(verbose)
						{
							logger << DebugInfo << sc_core::sc_time_stamp() << ":channel #" << dma_channel_num << ": beginning" << EndDebugInfo;
						}
						
						if(!edma_tcd_file[dma_channel_num].edma_tcd_csr.template Get<typename EDMA_TCD_CSR::START>())
						{
							Acknowledge(dma_channel_num);
							wait(sc_core::SC_ZERO_TIME);
						}
						
						// START=0
						edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::START>(0);
						
						// DONE=0
						edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::DONE>(0);
						
						// ACTIVE=1
						edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::ACTIVE>(1);
						
						channel_x_tcd = edma_tcd_file[dma_channel_num];
						channel_tcd = &channel_x_tcd;
					}
				}
			}
		}
		
		if(channel_tcd)
		{
			unsigned int dma_channel_num = channel_tcd->dma_channel_num;
			bool end_of_minor_loop = false;
			bool transfer_error = false;
			//sc_core::sc_time t;
			uint32_t saddr = channel_tcd->edma_tcd_saddr.template Get<typename EDMA_TCD_SADDR::SADDR>();
			uint32_t daddr = channel_tcd->edma_tcd_daddr.template Get<typename EDMA_TCD_DADDR::DADDR>();
			
			if(edma_cr.template Get<typename EDMA_CR::CX>() || edma_cr.template Get<typename EDMA_CR::ECX>()) // cancel transfer ?
			{
				if(verbose)
				{
					logger << DebugInfo << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": cancelling" << EndDebugInfo;
				}
				
				// force minor loop to finish
				end_of_minor_loop = true;
				
				if(edma_cr.template Get<typename EDMA_CR::CX>())
				{
					// Clear cancel request
					edma_cr.template Set<typename EDMA_CR::CX>(0);
				}
				
				if(edma_cr.template Get<typename EDMA_CR::ECX>())
				{
					// Clear error cancel request
					edma_cr.template Set<typename EDMA_CR::ECX>(0);
					
					// Transfer cancelled
					edma_es.template Set<typename EDMA_ES::ECX>(1);
					
					SetErrorIndicator(dma_channel_num);
				}
			}
			else
			{
				uint32_t citer = channel_tcd->GetCITER();
				
				if(citer != 0)
				{
					uint64_t nbytes = channel_tcd->GetNBYTES();
					
					unsigned int ssize = channel_tcd->GetSourceDataTransferSize();
					unsigned int dsize = channel_tcd->GetDestinationDataTransferSize();
					int32_t soff = unisim::util::arithmetic::SignExtend(channel_tcd->edma_tcd_soff.template Get<typename EDMA_TCD_SOFF::SOFF>(), EDMA_TCD_SOFF::SOFF::BITWIDTH);
					int32_t doff = unisim::util::arithmetic::SignExtend(channel_tcd->edma_tcd_doff.template Get<typename EDMA_TCD_DOFF::DOFF>(), EDMA_TCD_DOFF::DOFF::BITWIDTH);
					unsigned int smod = channel_tcd->edma_tcd_attr.template Get<typename EDMA_TCD_ATTR::SMOD>();
					unsigned int dmod = channel_tcd->edma_tcd_attr.template Get<typename EDMA_TCD_ATTR::DMOD>();
					uint32_t saddr_mask = smod ? ((uint32_t(1) << smod) - 1) : ~uint32_t(0);
					uint32_t daddr_mask = dmod ? ((uint32_t(1) << dmod) - 1) : ~uint32_t(0);
					
					unsigned int size = (ssize > dsize) ? ssize : dsize;
					
					assert(nbytes >= size);
					
					uint8_t data[size];
					
					if(verbose)
					{
						logger << DebugInfo << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": minor-loop byte count of " << nbytes << " bytes" << EndDebugInfo;
					}
					
					if(Transfer(tlm::TLM_READ_COMMAND, GetMasterID(dma_channel_num), GetPrivilegeAccessLevel(dma_channel_num), saddr, data, size, ssize, soff, saddr_mask, dma_engine_time))
					{
						if(Transfer(tlm::TLM_WRITE_COMMAND, GetMasterID(dma_channel_num), GetPrivilegeAccessLevel(dma_channel_num), daddr, data, size, dsize, doff, daddr_mask, dma_engine_time))
						{
							// Decrement minor loop byte transfer counter
							nbytes -= size;
							
							channel_tcd->SetNBYTES(nbytes);
							channel_tcd->edma_tcd_saddr.template Set<typename EDMA_TCD_SADDR::SADDR>(saddr);
							channel_tcd->edma_tcd_daddr.template Set<typename EDMA_TCD_DADDR::DADDR>(daddr);
							
							end_of_minor_loop = (nbytes == 0);
						}
						else
						{
							logger << DebugWarning << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": destination bus error" << EndDebugWarning;
							
							transfer_error = true;
							edma_es.template Set<typename EDMA_ES::DBE>(1); // destination bus error

							SetErrorIndicator(dma_channel_num);
						}
					}
					else
					{
						logger << DebugWarning << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": source bus error" << EndDebugWarning;
						
						transfer_error = true;
						edma_es.template Set<typename EDMA_ES::SBE>(1); // source bus error

						SetErrorIndicator(dma_channel_num);
					}
				}
				else
				{
					logger << DebugWarning << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": null major-loop iteration count" << EndDebugWarning;
					if(channel_tcd == &channel_y_tcd)
					{
						// resume preempted channel x
						channel_tcd = &channel_x_tcd;
					}
					else
					{
						// exit
						dma_engine_time += dma_clock_period;
						dma_engine_time += dma_clock_period;
						dma_engine_time += dma_clock_period;
						channel_tcd = 0;
					}
				}
			}
			
			if(end_of_minor_loop || transfer_error)
			{
				// end of minor-loop or transfer error
				
				uint32_t citer = channel_tcd->GetCITER();
				
				if(end_of_minor_loop)
				{
					// Decrement major loop iteration counter
					if(unlikely(citer == 0))
					{
						logger << DebugError << "Internal error! decrementing CITER would result in an underflow" << EndDebugError;
						this->Stop(-1);
					}
					citer = citer - 1;
					channel_tcd->SetCITER(citer);
				}
				
				// write back SADDR, DADDR and CITER to TCD memory
				edma_tcd_file[dma_channel_num].edma_tcd_saddr = channel_tcd->edma_tcd_saddr;
				edma_tcd_file[dma_channel_num].edma_tcd_daddr = channel_tcd->edma_tcd_daddr;
				edma_tcd_file[dma_channel_num].edma_tcd_citer = channel_tcd->edma_tcd_citer;
				
				// ACTIVE=0
				edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::ACTIVE>(0);

				if(channel_tcd == &channel_y_tcd)
				{
					// resume preempted channel x
					channel_tcd = &channel_x_tcd;
				}
				else
				{
					// exit
					dma_engine_time += dma_clock_period;
					dma_engine_time += dma_clock_period;
					dma_engine_time += dma_clock_period;
					channel_tcd = 0;
				}

				if(end_of_minor_loop)
				{
					// end of minor-loop
					if(verbose)
					{
						logger << DebugInfo << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": end of minor-loop" << EndDebugInfo;
						logger << DebugInfo << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": current major-loop count is " << citer << EndDebugInfo;
					}
					
					if(citer == 0)
					{
						// end of major-loop
						if(verbose)
						{
							logger << DebugInfo << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": end of major-loop" << EndDebugInfo;
						}
						
						// DONE=1
						edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::DONE>(1);
						
						if(edma_tcd_file[dma_channel_num].edma_tcd_csr.template Get<typename EDMA_TCD_CSR::INTMAJOR>()) // interrupt when major iteration count completes ?
						{
							SetInterruptRequest(dma_channel_num);
						}
						
						if(edma_tcd_file[dma_channel_num].edma_tcd_csr.template Get<typename EDMA_TCD_CSR::DREQ>()) // clear ERQ ?
						{
							if(verbose)
							{
								logger << DebugInfo << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": autodisabling DMA requests" << EndDebugInfo;
							}
							DisableRequest(dma_channel_num);
						}
						
						if(edma_tcd_file[dma_channel_num].edma_tcd_csr.template Get<typename EDMA_TCD_CSR::ESG>()) // scatter/gather ?
						{
							// scatter/gather
							if(verbose)
							{
								logger << DebugInfo << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ":scatter/gather" << EndDebugInfo;
							}
							if(edma_tcd_file[dma_channel_num].CheckScatterGatherAddress())
							{
								uint32_t tcd_addr = edma_tcd_file[dma_channel_num].edma_tcd_dlastsga.template Get<typename EDMA_TCD_DLASTSGA::DLASTSGA>();
								if(!LoadTCD(dma_channel_num, tcd_addr, dma_engine_time))
								{
									logger << DebugWarning << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": TCD transfer error while scatter/gather" << EndDebugWarning;
								}
							}
							else
							{
								logger << DebugWarning << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": scatter/gather configuration error" << EndDebugWarning;
								
								edma_es.template Set<typename EDMA_ES::SGE>(1);
								SetErrorIndicator(dma_channel_num);
							}
						}
						else
						{
							// Adjust source address
							uint32_t slast = edma_tcd_file[dma_channel_num].edma_tcd_slast.template Get<typename EDMA_TCD_SLAST::SLAST>();
							saddr += slast;
							if(verbose)
							{
								logger << DebugInfo << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": last source address adjustment @0x" << std::hex << saddr << std::dec << EndDebugInfo;
							}
							edma_tcd_file[dma_channel_num].edma_tcd_saddr.template Set<typename EDMA_TCD_SADDR::SADDR>(saddr);
							
							// Adjust destination address
							uint32_t dlast = edma_tcd_file[dma_channel_num].edma_tcd_dlastsga.template Get<typename EDMA_TCD_DLASTSGA::DLASTSGA>();
							daddr += dlast;
							if(verbose)
							{
								logger << DebugInfo << (sc_core::sc_time_stamp() + dma_engine_time) << ":channel #" << dma_channel_num << ": last destination address adjustment @0x" << std::hex << daddr << std::dec << EndDebugInfo;
							}
							edma_tcd_file[dma_channel_num].edma_tcd_daddr.template Set<typename EDMA_TCD_DADDR::DADDR>(daddr);
							
							// Reload BITER into CITER
							edma_tcd_file[dma_channel_num].edma_tcd_citer = edma_tcd_file[dma_channel_num].edma_tcd_biter;
						}
						
						if(edma_tcd_file[dma_channel_num].edma_tcd_csr.template Get<typename EDMA_TCD_CSR::MAJORELINK>()) // channel-to-channel linking on major loop complete ?
						{
							unsigned int majorlinkch = edma_tcd_file[dma_channel_num].edma_tcd_csr.template Get<typename EDMA_TCD_CSR::MAJORLINKCH>();
							edma_tcd_file[majorlinkch].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::START>(1);
							NotifyEngine();
						}
					}
					else
					{
						// not end of major-loop
						
						if(edma_tcd_file[dma_channel_num].edma_tcd_csr.template Get<typename EDMA_TCD_CSR::INTHALF>()) // interrupt when major counter is half complete ?
						{
							uint32_t biter = edma_tcd_file[dma_channel_num].GetBITER();
							
							if(citer == (biter / 2)) // half of major-loop ?
							{
								SetInterruptRequest(dma_channel_num);
							}
						}
						
						if(edma_tcd_file[dma_channel_num].CheckMinorLoopChannelLinking())
						{
							if(edma_tcd_file[dma_channel_num].edma_tcd_citer.template Get<typename EDMA_TCD_CITER::ELINKNO::ELINK>()) // channel-to-channel linking on minor-loop complete ?
							{
								unsigned int linkch = edma_tcd_file[dma_channel_num].edma_tcd_citer.template Get<typename EDMA_TCD_CITER::ELINKYES::LINKCH>();
								edma_tcd_file[linkch].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::START>(1);
								NotifyEngine();
								
								if((dma_channel_num == linkch) && edma_cr.template Get<typename EDMA_CR::CLM>()) // link to same channel and continuous link mode ?
								{
									// START=0
									edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::START>(0);
									
									// DONE=0
									edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::DONE>(0);
									
									// ACTIVE=1
									edma_tcd_file[dma_channel_num].edma_tcd_csr.template Set<typename EDMA_TCD_CSR::ACTIVE>(1);
									
									if(channel_tcd == &channel_y_tcd)
									{
										channel_y_tcd = edma_tcd_file[dma_channel_num];
										channel_tcd = &channel_y_tcd;
									}
									else
									{
										channel_x_tcd = edma_tcd_file[dma_channel_num];
										channel_tcd = &channel_x_tcd;
									}
								}
							}
						}
						else
						{
							edma_es.template Set<typename EDMA_ES::NCE>(1);
							SetErrorIndicator(dma_channel_num);
						}
					}
				}
			}
		}
		
		if(dma_engine_time != sc_core::SC_ZERO_TIME)
		{
			wait(dma_engine_time);
			dma_engine_time = sc_core::SC_ZERO_TIME;
		}
		else
		{
			wait();
		}
	}
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void EDMA<CONFIG>::UpdateDMAClock()
{
	dma_clock_period = dma_clk_prop_proxy.GetClockPeriod();
	dma_clock_start_time = dma_clk_prop_proxy.GetClockStartTime();
	dma_clock_posedge_first = dma_clk_prop_proxy.GetClockPosEdgeFirst();
	dma_clock_duty_cycle = dma_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void EDMA<CONFIG>::MasterClockPropertiesChangedProcess()
{
	// Master Clock properties have changed
	UpdateMasterClock();
}

template <typename CONFIG>
void EDMA<CONFIG>::DMAClockPropertiesChangedProcess()
{
	// DMA Clock properties have changed
	UpdateDMAClock();
}

} // end of namespace edma
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace dma
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_DMA_FREESCALE_MPC57XX_EDMA_EDMA_TCC__
