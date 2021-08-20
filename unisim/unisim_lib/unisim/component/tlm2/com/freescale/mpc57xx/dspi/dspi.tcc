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

#ifndef __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_DSPI_DSPI_TCC__
#define __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_DSPI_DSPI_TCC__

#include <unisim/component/tlm2/com/freescale/mpc57xx/dspi/dspi.hh>
#include <unisim/util/reg/core/register.tcc>

namespace unisim {
namespace component {
namespace tlm2 {
namespace com {
namespace freescale {
namespace mpc57xx {
namespace dspi {

using unisim::kernel::tlm2::tlm_input_bitstream_sync_status;
using unisim::kernel::tlm2::TLM_INPUT_BITSTREAM_SYNC_OK;
using unisim::kernel::tlm2::TLM_INPUT_BITSTREAM_NEED_SYNC;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::TLM2_IP_VERSION_MAJOR;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::TLM2_IP_VERSION_MINOR;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::TLM2_IP_VERSION_PATCH;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::BUSWIDTH;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::NUM_CTARS;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::TX_FIFO_DEPTH;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::RX_FIFO_DEPTH;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::CMD_FIFO_DEPTH;

template <typename CONFIG>
const bool DSPI<CONFIG>::HAS_DATA_SERIALIZATION_SUPPORT;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::NUM_DSI_INPUTS;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::NUM_DSI_OUTPUTS;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::NUM_SLAVE_CONFIGURATIONS;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::SPI_CTAR_SLAVE_NUM;

template <typename CONFIG>
const unsigned int DSPI<CONFIG>::DSI_CTAR_SLAVE_NUM;

template <typename CONFIG>
const bool DSPI<CONFIG>::threaded_model;

template <typename CONFIG>
DSPI<CONFIG>::DSPI(const sc_core::sc_module_name& name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_core::sc_module(name)
	, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
	, peripheral_slave_if("peripheral_slave_if")
	, SOUT("SOUT")
	, SIN("SIN")
	, SS("SS")
	, m_clk("m_clk")
	, dspi_clk("dspi_clk")
	, reset_b("reset_b")
	, debug("debug")
	, HT("HT")
	, DSI_INPUT("DSI_INPUT", NUM_DSI_INPUTS)
	, DMA_ACK_RX("DMA_ACK_RX")
	, DMA_ACK_TX("DMA_ACK_TX")
	, DMA_ACK_CMD("DMA_ACK_CMD")
	, DMA_ACK_DD("DMA_ACK_DD")
	, PCS("PCS", NUM_CTARS)
	, DSI_OUTPUT("DSI_OUTPUT", NUM_DSI_OUTPUTS)
	, INT_EOQF("INT_EOQF")  
	, INT_TFFF("INT_TFFF")  
	, INT_CMDFFF("INT_CMDFFF")
	, INT_TFIWF("INT_TFIWF") 
	, INT_TCF("INT_TCF")   
	, INT_CMDTCF("INT_CMDTCF")
	, INT_SPITCF("INT_SPITCF")
	, INT_DSITCF("INT_DSITCF")
	, INT_TFUF("INT_TFUF")  
	, INT_RFDF("INT_RFDF")  
	, INT_RFOF("INT_RFOF")  
	, INT_SPEF("INT_SPEF")  
	, INT_DPEF("INT_DPEF")  
	, INT_DDIF("INT_DDIF")  
	, DMA_RX("DMA_RX")
	, DMA_TX("DMA_TX")
	, DMA_CMD("DMA_CMD")
	, DMA_DD("DMA_DD")
	, registers_export("registers-export", this)
	, logger(*this)
	, m_clk_prop_proxy(m_clk)
	, dspi_clk_prop_proxy(dspi_clk)
	, dspi_mcr(this)
	, dspi_tcr(this)
	, dspi_ctar(this)
	, dspi_ctar_slave(this)
	, dspi_sr(this)
	, dspi_rser(this)
	, dspi_pushr(this)
	, dspi_pushr_slave(this)
	, dspi_popr(this)
	, dspi_txfr(this)
	, dspi_rxfr(this)
	, dspi_dsicr0(this)
	, dspi_sdr0(this)
	, dspi_asdr0(this)
	, dspi_compr0(this)
	, dspi_ddr0(this)
	, dspi_dsicr1(this)
	, dspi_ssr0(this)
	, dspi_dimr0(this)
	, dspi_dpir0(this)
	, dspi_ctare(this)
	, dspi_srex(this)
	, rxpart(0)
	, prev_dconf(DCONF_RES)
	, curr_dconf(DCONF_RES)
	, prev_cmd(0)
	, cmd(0)
	, cmd_cycling_cnt(0)
//	, trigger_spi_transfer(false)
	, trigger_dsi_transfer(false)
	, sin_bitstream()
	, transfer_event("transfer_event")
	, gen_int_eoqf_event("gen_int_eoqf_event")
	, gen_int_tfff_event("gen_int_tfff_event")
	, gen_int_cmdfff_event("gen_int_cmdfff_event")
	, gen_int_tfiwf_event("gen_int_tfiwf_event")
	, gen_int_tcf_event("gen_int_tcf_event")
	, gen_int_cmdtcf_event("gen_int_cmdtcf_event")
	, gen_int_spitcf_event("gen_int_spitcf_event")
	, gen_int_dsitcf_event("gen_int_dsitcf_event")
	, gen_int_tfuf_event("gen_int_tfuf_event")
	, gen_int_rfdf_event("gen_int_rfdf_event")
	, gen_int_rfof_event("gen_int_rfof_event")
	, gen_int_spef_event("gen_int_spef_event")
	, gen_int_dpef_event("gen_int_dpef_event")
	, gen_int_ddif_event("gen_int_ddif_event")
	, gen_dma_rx_event("gen_dma_rx_event")
	, gen_dma_tx_event("gen_dma_tx_event")
	, gen_dma_cmd_event("gen_dma_cmd_event")
	, gen_dma_dd_event("gen_dma_dd_event")
	, gen_tfff_event("gen_tfff_event")
	, gen_cmdfff_event("gen_cmdfff_event")
	, gen_rfdf_event("gen_rfdf_event")
	, dsi_input_sample_event("dsi_input_sample_event", NUM_DSI_INPUTS)
	, latch_serialized_data_event("latch_serialized_data_event")
	, reg_addr_map()
	, registers_registry()
	, schedule()
	, endian(unisim::util::endian::E_BIG_ENDIAN)
	, param_endian("endian", this, endian, "endian")
	, verbose(false)
	, param_verbose("verbose", this, verbose, "enable/disable verbosity")
	, master_clock_period(10.0, sc_core::SC_NS)
	, master_clock_start_time(sc_core::SC_ZERO_TIME)
	, master_clock_posedge_first(true)
	, master_clock_duty_cycle(0.5)
	, dspi_clock_period(10.0, sc_core::SC_NS)          
	, dspi_clock_start_time(sc_core::SC_ZERO_TIME)
	, dspi_clock_posedge_first(true)
	, dspi_clock_duty_cycle(0.5)
{
	unsigned int i;
	
	peripheral_slave_if(*this);
	
	SIN.register_nb_receive(this, &DSPI<CONFIG>::nb_receive, SIN_IF);
	SS.register_nb_receive(this, &DSPI<CONFIG>::nb_receive, SS_IF);

	std::stringstream description_sstr;
	description_sstr << "MPC57XX DSPI:" << std::endl;
	description_sstr << "  - " << NUM_CTARS << " CTARs" << std::endl;
	description_sstr << "  - TX FIFO Depth of " << TX_FIFO_DEPTH << std::endl;
	description_sstr << "  - RX FIFO Depth of " << RX_FIFO_DEPTH << std::endl;
	description_sstr << "  - CMD FIFO Depth of " << CMD_FIFO_DEPTH << std::endl;
	if(HAS_DATA_SERIALIZATION_SUPPORT)
	{
		description_sstr << "  - Data Serialization support" << std::endl;
		description_sstr << "  - " << NUM_DSI_INPUTS << " parallel inputs" << std::endl;
		description_sstr << "  - " << NUM_DSI_OUTPUTS << " parallel outputs" << std::endl;
	}
	description_sstr << "  - " << BUSWIDTH << "-bit data bus" << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Version: " << TLM2_IP_VERSION_MAJOR << "." << TLM2_IP_VERSION_MINOR << "." << TLM2_IP_VERSION_PATCH << std::endl;
	description_sstr << "  - SystemC TLM-2.0 IP Authors: Gilles Mouchard (gilles.mouchard@cea.fr)" << std::endl;
	description_sstr << "  - Hardware reference manual: MPC5777M Reference Manual, MPC5777MRM, Rev. 4.3, 01/2017, Chapter 49" << std::endl;
	this->SetDescription(description_sstr.str().c_str());

	SC_HAS_PROCESS(DSPI);

	reg_addr_map.SetWarningStream(logger.DebugWarningStream());
	reg_addr_map.SetEndian(endian);
	
	MapRegisters();
	
	ExportRegisters();

	if(threaded_model)
	{
		SC_THREAD(Process);
	}
	else
	{
		SC_METHOD(Process);
	}
	
	SC_METHOD(INT_EOQF_Process);
	sensitive << gen_int_eoqf_event;
	
	SC_METHOD(INT_TFFF_Process);
	sensitive << gen_int_tfff_event;
	
	SC_METHOD(INT_CMDFFF_Process);
	sensitive << gen_int_cmdfff_event;
	
	SC_METHOD(INT_TFIWF_Process); 
	sensitive << gen_int_tfiwf_event;
	
	SC_METHOD(INT_TCF_Process); 
	sensitive << gen_int_tcf_event;
	
	SC_METHOD(INT_CMDTCF_Process);
	sensitive << gen_int_cmdtcf_event;
	
	SC_METHOD(INT_SPITCF_Process);
	sensitive << gen_int_spitcf_event;
	
	SC_METHOD(INT_DSITCF_Process);
	sensitive << gen_int_dsitcf_event;
	
	SC_METHOD(INT_TFUF_Process);  
	sensitive << gen_int_tfuf_event;
	
	SC_METHOD(INT_RFDF_Process);
	sensitive << gen_int_rfdf_event;
	
	SC_METHOD(INT_RFOF_Process);
	sensitive << gen_int_rfof_event;
	
	SC_METHOD(INT_SPEF_Process);
	sensitive << gen_int_spef_event;
	
	SC_METHOD(INT_DPEF_Process);
	sensitive << gen_int_dpef_event;

	SC_METHOD(INT_DDIF_Process);
	sensitive << gen_int_ddif_event;
	
	SC_METHOD(DMA_RX_Process);
	sensitive << gen_dma_rx_event;
	
	SC_METHOD(DMA_TX_Process);
	sensitive << gen_dma_tx_event;
	
	SC_METHOD(DMA_CMD_Process);
	sensitive << gen_dma_cmd_event;
	
	SC_METHOD(DMA_DD_Process);
	sensitive << gen_dma_dd_event;
	
	SC_METHOD(DMA_ACK_RX_Process);
	sensitive << DMA_ACK_RX.pos();
	
	SC_METHOD(DMA_ACK_TX_Process);
	sensitive << DMA_ACK_TX.pos();

	SC_METHOD(DMA_ACK_CMD_Process);
	sensitive << DMA_ACK_CMD.pos();

	SC_METHOD(DMA_ACK_DD_Process);
	sensitive << DMA_ACK_DD.pos();
	
	SC_METHOD(Set_TFFF_Process);
	sensitive << gen_tfff_event;

	SC_METHOD(Set_CMDFFF_Process);
	sensitive << gen_cmdfff_event;

	SC_METHOD(Set_RFDF_Process);
	sensitive << gen_rfdf_event;

	SC_METHOD(HT_Process);
	sensitive << HT;
	
	SC_METHOD(DEBUG_Process);
	sensitive << debug;
	
	SC_THREAD(MasterTransferProcess);
	sensitive << transfer_event;
	
	SC_THREAD(SlaveTransferProcess);
	sensitive << ss_bitstream.event();
	
	// Spawn a DSI_INPUT_Process for each DSI input
	for(i = 0; i < NUM_DSI_INPUTS; i++)
	{
		sc_core::sc_spawn_options dsi_input_process_spawn_options;
		dsi_input_process_spawn_options.spawn_method();
		dsi_input_process_spawn_options.set_sensitivity(&DSI_INPUT[i]);
		
		std::stringstream dsi_input_process_name_sstr;
		dsi_input_process_name_sstr << "DSI_INPUT_Process_" << i;
		sc_core::sc_spawn(sc_bind(&DSPI<CONFIG>::DSI_INPUT_Process, this, i), dsi_input_process_name_sstr.str().c_str(), &dsi_input_process_spawn_options);
	}

	// Spawn a DSI_INPUT_SamplingProcess for each DSI input
	for(i = 0; i < NUM_DSI_INPUTS; i++)
	{
		sc_core::sc_spawn_options dsi_input_sampling_process_spawn_options;
		dsi_input_sampling_process_spawn_options.spawn_method();
		dsi_input_sampling_process_spawn_options.set_sensitivity(&dsi_input_sample_event[i]);
		
		std::stringstream dsi_input_sampling_process_name_sstr;
		dsi_input_sampling_process_name_sstr << "DSI_INPUT_SamplingProcess_" << i;
		sc_core::sc_spawn(sc_bind(&DSPI<CONFIG>::DSI_INPUT_SamplingProcess, this, i), dsi_input_sampling_process_name_sstr.str().c_str(), &dsi_input_sampling_process_spawn_options);
	}
	
	SC_METHOD(LatchSerializedDataProcess);
	sensitive << latch_serialized_data_event;
}

template <typename CONFIG>
DSPI<CONFIG>::~DSPI()
{
}

template <typename CONFIG>
void DSPI<CONFIG>::end_of_elaboration()
{
	logger << DebugInfo << this->GetDescription() << EndDebugInfo;

	// Spawn MasterClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options master_clock_properties_changed_process_spawn_options;
	
	master_clock_properties_changed_process_spawn_options.spawn_method();
	master_clock_properties_changed_process_spawn_options.set_sensitivity(&m_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&DSPI<CONFIG>::MasterClockPropertiesChangedProcess, this), "MasterClockPropertiesChangedProcess", &master_clock_properties_changed_process_spawn_options);

	// Spawn DSPIClockPropertiesChangedProcess Process that monitor clock properties modifications
	sc_core::sc_spawn_options dspi_clock_properties_changed_process_spawn_options;
	
	dspi_clock_properties_changed_process_spawn_options.spawn_method();
	dspi_clock_properties_changed_process_spawn_options.set_sensitivity(&dspi_clk_prop_proxy.GetClockPropertiesChangedEvent());

	sc_core::sc_spawn(sc_bind(&DSPI<CONFIG>::DSPIClockPropertiesChangedProcess, this), "DSPIClockPropertiesChangedProcess", &dspi_clock_properties_changed_process_spawn_options);

	Reset();
}

template <typename CONFIG>
void DSPI<CONFIG>::Reset()
{
	unsigned int i;
	
	dspi_mcr.Reset();
	dspi_tcr.Reset();
	for(i = 0; i < NUM_CTARS; i++)
	{
		dspi_ctar[i].Reset();
		switch(i)
		{
			case SPI_CTAR_SLAVE_NUM:
			case DSI_CTAR_SLAVE_NUM:
				dspi_ctar_slave[i].Reset();
				break;
		}
	}
	dspi_sr.Reset();
	dspi_rser.Reset();
	dspi_pushr.Reset();
	dspi_pushr_slave.Reset();
	dspi_popr.Reset();
	for(i = 0; i < TX_FIFO_DEPTH; i++)
	{
		dspi_txfr[i].Reset();
	}
	for(i = 0; i < RX_FIFO_DEPTH; i++)
	{
		dspi_rxfr[i].Reset();
	}
	dspi_dsicr0.Reset();
	dspi_sdr0.Reset();
	dspi_asdr0.Reset();
	dspi_compr0.Reset();
	dspi_ddr0.Reset();
	dspi_dsicr1.Reset();
	dspi_ssr0.Reset();
	dspi_dimr0.Reset();
	dspi_dpir0.Reset();
	for(i = 0; i < NUM_CTARS; i++)
	{
		dspi_ctare[i].Reset();
	}
	dspi_srex.Reset();
	
	rxpart = 0;
	prev_dconf = DCONF_SPI;
	curr_dconf = DCONF_SPI;
	prev_cmd = 0;
	cmd = 0;
	cmd_cycling_cnt = 0;
	tx_parity_bit = false;
	rx_parity_bit = false;
//	trigger_spi_transfer = false;
	trigger_dsi_transfer = false;
	
	TX_FIFO_Clear();
	RX_FIFO_Clear();
}

template <typename CONFIG>
void DSPI<CONFIG>::ModeSwitch()
{
	TX_FIFO_Clear();
	RX_FIFO_Clear();
	rxpart = 0;
	prev_dconf = DCONF_SPI;
	curr_dconf = DCONF_SPI;
	prev_cmd = 0;
	cmd = 0;
	cmd_cycling_cnt = 0;
	tx_parity_bit = false;
	rx_parity_bit = false;
//	trigger_spi_transfer = false;
	trigger_dsi_transfer = false;
	
	UpdateBSYF();
}

template <typename CONFIG>
void DSPI<CONFIG>::MapRegisters()
{
	// Map DSPI registers regarding there address offsets
	reg_addr_map.ClearRegisterMap();
	reg_addr_map.MapRegister(dspi_mcr.ADDRESS_OFFSET, &dspi_mcr);  
	reg_addr_map.MapRegister(dspi_tcr.ADDRESS_OFFSET, &dspi_tcr);
	unsigned int i;
	for(i = 0; i < NUM_CTARS; i++)
	{
		switch(i)
		{
			case SPI_CTAR_SLAVE_NUM:
			case DSI_CTAR_SLAVE_NUM:
				if(SlaveMode()) // slave mode?
				{
					// slave mode
					reg_addr_map.MapRegister(DSPI_CTAR::ADDRESS_OFFSET + (4 * i), &dspi_ctar_slave[i]);
					break;
				}
			default:
				reg_addr_map.MapRegister(DSPI_CTAR::ADDRESS_OFFSET + (4 * i), &dspi_ctar[i]);
				break;
		}
	}
	
	reg_addr_map.MapRegister(dspi_sr  .ADDRESS_OFFSET, &dspi_sr  );
	reg_addr_map.MapRegister(dspi_rser.ADDRESS_OFFSET, &dspi_rser);
	
	if(MasterMode()) // master mode?
	{
		// master mode
		reg_addr_map.MapRegister(dspi_pushr.ADDRESS_OFFSET, &dspi_pushr);
	}
	else
	{
		reg_addr_map.MapRegister(dspi_pushr_slave.ADDRESS_OFFSET, &dspi_pushr_slave);
	}
	
	reg_addr_map.MapRegister(dspi_popr.ADDRESS_OFFSET, &dspi_popr);
	
	if(!dspi_mcr.template Get<typename DSPI_MCR::DIS_TXF>())
	{
		reg_addr_map.MapRegisterFile(DSPI_TXFR ::ADDRESS_OFFSET, &dspi_txfr);
	}
	if(!dspi_mcr.template Get<typename DSPI_MCR::DIS_RXF>())
	{
		reg_addr_map.MapRegisterFile(DSPI_RXFR ::ADDRESS_OFFSET, &dspi_rxfr);
	}
	if(HAS_DATA_SERIALIZATION_SUPPORT)
	{
		reg_addr_map.MapRegister(dspi_dsicr0    .ADDRESS_OFFSET, &dspi_dsicr0);
		reg_addr_map.MapRegister(dspi_sdr0      .ADDRESS_OFFSET, &dspi_sdr0  );
		reg_addr_map.MapRegister(dspi_asdr0     .ADDRESS_OFFSET, &dspi_asdr0 );
		reg_addr_map.MapRegister(dspi_compr0    .ADDRESS_OFFSET, &dspi_compr0);
		reg_addr_map.MapRegister(dspi_ddr0      .ADDRESS_OFFSET, &dspi_ddr0  );
		reg_addr_map.MapRegister(dspi_dsicr1    .ADDRESS_OFFSET, &dspi_dsicr1);
		reg_addr_map.MapRegister(dspi_ssr0      .ADDRESS_OFFSET, &dspi_ssr0  );
		reg_addr_map.MapRegister(dspi_dimr0     .ADDRESS_OFFSET, &dspi_dimr0 );
		reg_addr_map.MapRegister(dspi_dpir0     .ADDRESS_OFFSET, &dspi_dpir0 );
	}
	reg_addr_map.MapRegisterFile(DSPI_CTARE::ADDRESS_OFFSET, &dspi_ctare );
	reg_addr_map.MapRegister(dspi_srex      .ADDRESS_OFFSET, &dspi_srex  );
}

template <typename CONFIG>
void DSPI<CONFIG>::ExportRegisters()
{
	unsigned int i;
	
	registers_registry.AddRegisterInterface(dspi_mcr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_tcr.CreateRegisterInterface());
	for(i = 0; i < NUM_CTARS; i++)
	{
		registers_registry.AddRegisterInterface(dspi_ctar[i].CreateRegisterInterface());
		switch(i)
		{
			case SPI_CTAR_SLAVE_NUM:
			case DSI_CTAR_SLAVE_NUM:
				registers_registry.AddRegisterInterface(dspi_ctar_slave[i].CreateRegisterInterface());
				break;
		}
	}
	registers_registry.AddRegisterInterface(dspi_sr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_rser.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_pushr.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_pushr_slave.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_popr.CreateRegisterInterface());
	for(i = 0; i < TX_FIFO_DEPTH; i++)
	{
		registers_registry.AddRegisterInterface(dspi_txfr[i].CreateRegisterInterface());
	}
	for(i = 0; i < RX_FIFO_DEPTH; i++)
	{
		registers_registry.AddRegisterInterface(dspi_rxfr[i].CreateRegisterInterface());
	}
	registers_registry.AddRegisterInterface(dspi_dsicr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_sdr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_asdr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_compr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_ddr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_dsicr1.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_ssr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_dimr0.CreateRegisterInterface());
	registers_registry.AddRegisterInterface(dspi_dpir0.CreateRegisterInterface());
	for(i = 0; i < NUM_CTARS; i++)
	{
		registers_registry.AddRegisterInterface(dspi_ctare[i].CreateRegisterInterface());
	}
	registers_registry.AddRegisterInterface(dspi_srex.CreateRegisterInterface());
}

template <typename CONFIG>
bool DSPI<CONFIG>::Running() const
{
	return dspi_sr.template Get<typename DSPI_SR::TXRXS>() != 0;
}

template <typename CONFIG>
bool DSPI<CONFIG>::Stopped() const
{
	return dspi_sr.template Get<typename DSPI_SR::TXRXS>() == 0;
}

template <typename CONFIG>
DSPI_State DSPI<CONFIG>::GetState() const
{
	return dspi_sr.template Get<typename DSPI_SR::TXRXS>() ? DSPI_RUNNING : DSPI_STOPPED;
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateState()
{
	bool eoqf = dspi_sr.template Get<typename DSPI_SR::EOQF>();
	bool frz = dspi_mcr.template Get<typename DSPI_MCR::FRZ>();
	bool halt = dspi_mcr.template Get<typename DSPI_MCR::HALT>();
	
	bool running = !eoqf && (!debug || !frz) && !halt;
	dspi_sr.template Set<typename DSPI_SR::TXRXS>(running);
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << (running ? "running" : "stopped") << EndDebugInfo;
	}

	if(running) ScheduleTransfer();
}

template <typename CONFIG>
bool DSPI<CONFIG>::ModuleDisabled() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::MDIS>() != 0;
}

template <typename CONFIG>
bool DSPI<CONFIG>::MasterMode() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::MSTR>() != 0;
}

template <typename CONFIG>
bool DSPI<CONFIG>::SlaveMode() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::MSTR>() == 0;
}

template <typename CONFIG>
bool DSPI<CONFIG>::ExtendedMode() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::XSPI>() != 0;
}

template <typename CONFIG>
bool DSPI<CONFIG>::SPI_FrameTransmissionsStopped() const
{
	bool spef = dspi_sr.template Get<typename DSPI_SR::SPEF>(); // SPI parity error flag
	
	if(spef) // parity error?
	{
		bool pes = dspi_mcr.template Get<typename DSPI_MCR::PES>(); // parity error stop
		return pes; // stops SPI frame transmission?
	}
	
	return false;
}

template <typename CONFIG>
bool DSPI<CONFIG>::DSI_OperationsStopped() const
{
	bool dpef = dspi_sr.template Get<typename DSPI_SR::DPEF>(); // DSI parity error flag
	
	if(dpef) // parity error?
	{
		bool pes = dspi_dsicr0.template Get<typename DSPI_DSICR0::PES>(); // parity error stop
		return pes; // Stop DSI operations?
	}
	
	return false;
}

template <typename CONFIG>
bool DSPI<CONFIG>::DSI_FrameTransmissionsStopped() const
{
	return dspi_dsicr0.template Get<typename DSPI_DSICR0::DMS>(); // data match stop
}

template <typename CONFIG>
DSPI_Configuration DSPI<CONFIG>::Configuration() const
{
	return DSPI_Configuration(dspi_mcr.template Get<typename DSPI_MCR::DCONF>());
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::MasterFrameSize(DSPI_Configuration dconf, unsigned int ctar_num) const
{
	if(!HAS_DATA_SERIALIZATION_SUPPORT || (dconf == DCONF_SPI))
	{
		unsigned int fmsz = dspi_ctar[ctar_num].template Get<typename DSPI_CTAR::FMSZ>();
		if(ExtendedMode())
		{
			unsigned int fmsze = dspi_ctare[ctar_num].template Get<typename DSPI_CTARE::FMSZE>();
			return fmsz + (fmsze << DSPI_CTAR::FMSZ::BITWIDTH) + 1;
		}
		
		return fmsz + 1;
	}
	else if(dconf == DCONF_DSI)
	{
		unsigned int fmsz = dspi_ctar[ctar_num].template Get<typename DSPI_CTAR::FMSZ>();
		unsigned int fmsz4 = dspi_dsicr0.template Get<typename DSPI_DSICR0::FMSZ4>();
		return fmsz + (fmsz4 << DSPI_CTAR::FMSZ::BITWIDTH) + 1;
	}
	
	return 0;
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::SlaveFrameSize(DSPI_Configuration dconf, unsigned int ctar_num) const
{
	if(!HAS_DATA_SERIALIZATION_SUPPORT || (dconf == DCONF_SPI))
	{
		unsigned int fmsz = dspi_ctar_slave[ctar_num].template Get<typename DSPI_CTAR::FMSZ>();
		if(ExtendedMode())
		{
			unsigned int fmsze = dspi_ctare[ctar_num].template Get<typename DSPI_CTARE::FMSZE>();
			return fmsz + (fmsze << DSPI_CTAR::FMSZ::BITWIDTH) + 1;
		}
		
		return fmsz + 1;
	}
	else if(dconf == DCONF_DSI)
	{
		unsigned int fmsz = dspi_ctar_slave[ctar_num].template Get<typename DSPI_CTAR::FMSZ>();
		unsigned int fmsz4 = dspi_dsicr0.template Get<typename DSPI_DSICR0::FMSZ4>();
		return fmsz + (fmsz4 << DSPI_CTAR::FMSZ::BITWIDTH) + 1;
	}
	
	return 0;
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::TX_FIFO_Depth() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::DIS_TXF>() ? 1 : TX_FIFO_DEPTH;
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::CMD_FIFO_Depth() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::DIS_TXF>() ? 1 : CMD_FIFO_DEPTH;
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::RX_FIFO_Depth() const
{
	return dspi_mcr.template Get<typename DSPI_MCR::DIS_RXF>() ? 1 : RX_FIFO_DEPTH;
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_Master_TX_FIFO_Push(uint16_t txcmd, uint16_t txdata)
{
	assert(MasterMode());
	assert(!ExtendedMode());
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Push(txcmd=0x" << std::hex << txcmd << std::dec << ", txdata=0x" << std::hex << txdata << std::hex << ")" << EndDebugInfo;
	}
	
	DSPI_Configuration dconf = Configuration();
	switch(dconf)
	{
		case DCONF_SPI:
		case DCONF_CSI:
		{
			unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
			if(txctr < TX_FIFO_Depth()) // TX FIFO is not full?
			{
				// Fill-in TX FIFO
				unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>(); // Transmit Next Pointer
				unsigned int txbackptr = (txctr + txnxtptr) % TX_FIFO_Depth();
				
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXCMD_TXDATA>(txcmd);
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXDATA>(txdata);
				
				// Increment TX FIFO Counter
				txctr++;
				dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
				
				// Set TFFF if TX FIFO is not full
				Set_TFFF_If_TX_FIFO_Not_Full();
				
				ScheduleTransfer();
			}
			else
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push command/data to a full TX FIFO" << EndDebugWarning;
			}
			break;
		}
			
		default:
			logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push command/data" << EndDebugWarning;
			break;
	}
	
	if(verbose)
	{
		TX_FIFO_Dump();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::XSPI_Master_TX_FIFO_Push(uint16_t txdata)
{
	assert(MasterMode());
	assert(ExtendedMode());
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Push(txdata=0x" << std::hex << txdata << std::hex << ")" << EndDebugInfo;
	}

	DSPI_Configuration dconf = Configuration();
	switch(dconf)
	{
		case DCONF_SPI:
		case DCONF_CSI:
		{
			unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
			if(txctr < TX_FIFO_Depth())
			{
				// Fill-in TX FIFO
				unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>(); // Transmit Next Pointer
				unsigned int txbackptr = (txctr + txnxtptr) % TX_FIFO_Depth();
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXDATA>(txdata);
				
				// Increment TX FIFO Counter
				txctr++;
				dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
				
				// Set TFFF if TX FIFO is not full
				Set_TFFF_If_TX_FIFO_Not_Full();

				ScheduleTransfer();
			}
			else
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push data to a full TX FIFO" << EndDebugWarning;
			}
			
			break;
		}
		
		default:
			logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push data" << EndDebugWarning;
			break;
	}
	
	if(verbose)
	{
		TX_FIFO_Dump();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::XSPI_Master_CMD_FIFO_Push(uint16_t txcmd)
{
	assert(MasterMode());
	assert(ExtendedMode());
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":CMD FIFO:Push(txcmd=0x" << std::hex << txcmd << std::hex << ")" << EndDebugInfo;
	}
	
	DSPI_Configuration dconf = Configuration();
	switch(dconf)
	{
		case DCONF_SPI:
		case DCONF_CSI:
		{
			unsigned int cmdctr = dspi_srex.template Get<typename DSPI_SREX::CMDCTR>();
			if(cmdctr < CMD_FIFO_Depth())
			{
				// Fill-in CMD FIFO
				unsigned int cmdnxtptr = dspi_srex.template Get<typename DSPI_SREX::CMDNXTPTR>(); // Command Next Pointer
				unsigned int cmdbackptr = (cmdctr + cmdnxtptr) % CMD_FIFO_Depth();
				dspi_txfr[cmdbackptr].template Set<typename DSPI_TXFR::TXCMD_TXDATA>(txcmd);
				
				// Increment CMD FIFO counter
				cmdctr++;
				dspi_srex.template Set<typename DSPI_SREX::CMDCTR>(cmdctr);
				
				// Set TFFF if CMD FIFO is not full
				Set_CMDFFF_If_CMD_FIFO_Not_Full();

				ScheduleTransfer();
			}
			else
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": While in " << dconf << ", master attempt to push cmd to a full CMD FIFO" << EndDebugWarning;
			}
			break;
		}
		
		default:
			logger << DebugWarning << sc_core::sc_time_stamp() << ": while in " << dconf << ", master attempt to push command" << EndDebugWarning;
			break;
	}
	
	if(verbose)
	{
		CMD_FIFO_Dump();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_Slave_TX_FIFO_Push(uint16_t txdata)
{
	assert(SlaveMode());
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Push(txdata=0x" << std::hex << txdata << std::hex << ")" << EndDebugInfo;
	}
	unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
	if(txctr < TX_FIFO_Depth()) // TX FIFO is not full?
	{
		unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>(); // Transmit Next Pointer
		unsigned int txbackptr = (txctr + txnxtptr) % TX_FIFO_Depth();
		
		if(SlaveFrameSize(DCONF_SPI, SPI_CTAR_SLAVE_NUM) > 16) // frame size requires two TX FIFO pushes?
		{
			if(rxpart)
			{
				// Fill-in TX FIFO with 16 MSB bits
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXCMD_TXDATA>(txdata);
				
				// Increment TX FIFO Counter
				txctr++;
				dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
				
				// Set TFFF if TX FIFO is not full
				Set_TFFF_If_TX_FIFO_Not_Full();
				
				ScheduleTransfer();
			}
			else
			{
				// Fill-in TX FIFO with 16 LSB bits
				dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXDATA>(txdata);
			}
			
			rxpart ^= 1;
		}
		else
		{
			// Fill-in TX FIFO
			dspi_txfr[txbackptr].template Set<typename DSPI_TXFR::TXDATA>(txdata);
			
			// Increment TX FIFO Counter
			txctr++;
			dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
			
			// Set TFFF if TX FIFO is not full
			Set_TFFF_If_TX_FIFO_Not_Full();
			
			ScheduleTransfer();
		}
	}
	
	if(verbose)
	{
		TX_FIFO_Dump();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::TX_FIFO_Clear()
{
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Clear()" << EndDebugInfo;
	}
	
	dspi_sr.template Set<typename DSPI_SR::TXCTR>(0);      // TX FIFO Counter <- 0
	dspi_srex.template Set<typename DSPI_SREX::CMDCTR>(0); // CMD FIFO Counter <- 0
	
	// Set TFFF
	dspi_sr.template Set<typename DSPI_SR::TFFF>(1); // Transmit FIFO Fill Flag
	UpdateINT_TFFF();
	UpdateDMA_TX();
	
	// Set CMDFFF
	dspi_sr.template Set<typename DSPI_SR::CMDFFF>(1); // CMD FIFO Fill Flag
	UpdateINT_CMDFFF();
	UpdateDMA_CMD();
	
	if(verbose)
	{
		TX_FIFO_Dump();
		if(ExtendedMode())
		{
			CMD_FIFO_Dump();
		}
	}
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::TX_FIFO_Size() const
{
	return dspi_sr.template Get<typename DSPI_SR::TXCTR>(); // TX FIFO Counter
}

template <typename CONFIG>
bool DSPI<CONFIG>::TX_FIFO_Empty() const
{
	return dspi_sr.template Get<typename DSPI_SR::TXCTR>() == 0; // TX FIFO Counter is null?
}

template <typename CONFIG>
bool DSPI<CONFIG>::CMD_FIFO_Empty() const
{
	return dspi_srex.template Get<typename DSPI_SREX::CMDCTR>() == 0; // CMD FIFO Counter is null?
}

template <typename CONFIG>
bool DSPI<CONFIG>::TX_FIFO_Full() const
{
	unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>(); // TX FIFO Counter
	return txctr >= TX_FIFO_Depth(); // TX FIFO Counter >= TX FIFO Depth?
}

template <typename CONFIG>
bool DSPI<CONFIG>::CMD_FIFO_Full() const
{
	unsigned int cmdctr = dspi_srex.template Get<typename DSPI_SREX::CMDCTR>(); // CMD FIFO Counter
	return cmdctr >= CMD_FIFO_Depth(); // CMD FIFO Counter >= CMD FIFO Depth?
}

template <typename CONFIG>
uint16_t DSPI<CONFIG>::SPI_Master_TX_FIFO_Front() const
{
	unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>(); // Transmit Next Pointer
	return dspi_txfr[txnxtptr].template Get<typename DSPI_TXFR::TXDATA>(); // Transmit FIFO register
}

template <typename CONFIG>
uint16_t DSPI<CONFIG>::SPI_Master_CMD_FIFO_Front() const
{
	if(ExtendedMode()) // Extended mode?
	{
		unsigned int cmdnxtptr = dspi_srex.template Get<typename DSPI_SREX::CMDNXTPTR>(); // Command Next Pointer
		return dspi_txfr[cmdnxtptr].template Get<typename DSPI_TXFR::TXCMD_TXDATA>(); // Transmit FIFO register
	}
	else
	{
		unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>(); // Transmit Next Pointer
		return dspi_txfr[txnxtptr].template Get<typename DSPI_TXFR::TXCMD_TXDATA>(); // Transmit FIFO register
	}
}

template <typename CONFIG>
uint32_t DSPI<CONFIG>::SPI_Slave_TX_FIFO_Front() const
{
	unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>(); // Transmit Next Pointer
	return dspi_txfr[txnxtptr]; // DSPI Transmit FIFO register
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_Master_TX_FIFO_Pop()
{
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Pop()" << EndDebugInfo;
	}
	
	unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>(); // TX FIFO Counter
	
	if(txctr) // TX FIFO is not empty?
	{
		// Pop front
		unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>(); // Transmit Next Pointer
		txnxtptr = (txnxtptr + 1) % TX_FIFO_Depth();
		dspi_sr.template Set<typename DSPI_SR::TXNXTPTR>(txnxtptr);
		
		// Decrement TX FIFO Counter
		txctr--;
		dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
		
		// Set TFFF if TX FIFO is not full
		Set_TFFF_If_TX_FIFO_Not_Full();
		
		ScheduleTransfer();
	}
	else
	{
		logger << DebugError << "Internal error! Attempt to pop from an empty TX FIFO" << EndDebugError;
	}
	
	if(verbose)
	{
		TX_FIFO_Dump();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::XSPI_Master_CMD_FIFO_Pop()
{
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":CMD FIFO:Pop()" << EndDebugInfo;
	}
	
	unsigned int cmdctr = dspi_srex.template Get<typename DSPI_SREX::CMDCTR>(); // CMD FIFO Counter
	
	if(cmdctr) // CMD FIFO is not empty?
	{
		// Pop front
		unsigned int cmdnxtptr = dspi_sr.template Get<typename DSPI_SREX::CMDNXTPTR>(); // Command Next pointer
		cmdnxtptr = (cmdnxtptr + 1) % CMD_FIFO_Depth();
		dspi_srex.template Set<typename DSPI_SREX::CMDNXTPTR>(cmdnxtptr);
		
		// Decrement CMD FIFO Counter
		cmdctr--;
		dspi_srex.template Set<typename DSPI_SREX::CMDCTR>(cmdctr);
		
		// Set TFFF if CMD FIFO is not full
		Set_CMDFFF_If_CMD_FIFO_Not_Full();
		
		ScheduleTransfer();
	}
	else
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ": master attempts to pop data from an empty RX FIFO are ignored and the RX FIFO Counter remains unchanged; The data read from the empty RX FIFO is undetermined." << EndDebugWarning;
	}
	
	if(verbose)
	{
		CMD_FIFO_Dump();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_Slave_TX_FIFO_Pop()
{
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Pop()" << EndDebugInfo;
	}
	
	unsigned int txctr = dspi_sr.template Get<typename DSPI_SR::TXCTR>(); // TX FIFO Counter
	
	if(txctr) // TX FIFO is not empty?
	{
		// Pop front
		unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>(); // Transmit next pointer
		txnxtptr = (txnxtptr + 1) % TX_FIFO_Depth();
		dspi_sr.template Set<typename DSPI_SR::TXNXTPTR>(txnxtptr);
		
		// Decrement TX FIFO Counter
		txctr--;
		dspi_sr.template Set<typename DSPI_SR::TXCTR>(txctr);
		
		// Set TFFF if TX FIFO is not full
		Set_TFFF_If_TX_FIFO_Not_Full();
	}
	
	if(verbose)
	{
		TX_FIFO_Dump();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::TX_FIFO_Dump()
{
	logger << DebugInfo << "TX FIFO = [";
	
	unsigned int n = dspi_sr.template Get<typename DSPI_SR::TXCTR>();
	unsigned int txnxtptr = dspi_sr.template Get<typename DSPI_SR::TXNXTPTR>(); // Transmit next pointer
	unsigned int d = TX_FIFO_Depth();
	unsigned int i;
	
	for(i = 0; i < n; i++)
	{
		if(i) logger << " ";
		
		uint32_t v = dspi_txfr[(txnxtptr + i) % d];
		if(ExtendedMode()) v &= 0xffff;
		
		logger << "0x" << std::hex << v << std::dec;
	}
	
	logger << "]" << EndDebugInfo;
}

template <typename CONFIG>
void DSPI<CONFIG>::CMD_FIFO_Dump()
{
	logger << DebugInfo << "CMD FIFO = [";
	
	unsigned int n = dspi_srex.template Get<typename DSPI_SREX::CMDCTR>();
	unsigned int cmdnxtptr = dspi_sr.template Get<typename DSPI_SREX::CMDNXTPTR>(); // Command Next pointer
	unsigned int d = CMD_FIFO_Depth();
	unsigned int i;
	
	for(i = 0; i < n; i++)
	{
		if(i) logger << " ";
		
		uint32_t v = dspi_txfr[(cmdnxtptr + i) % d];
		v >>= 16;
		
		logger << "0x" << std::hex << v << std::dec;
	}
	
	logger << "]" << EndDebugInfo;
}

template <typename CONFIG>
void DSPI<CONFIG>::RX_FIFO_Dump()
{
	logger << DebugInfo << "RX FIFO = [";
	
	unsigned int n = dspi_sr.template Get<typename DSPI_SR::RXCTR>();
	unsigned int popnxtptr = dspi_sr.template Get<typename DSPI_SR::POPNXTPTR>(); // Transmit next pointer
	unsigned int d = RX_FIFO_Depth();
	unsigned int i;
	
	for(i = 0; i < n; i++)
	{
		if(i) logger << " ";
		
		uint32_t v = dspi_rxfr[(popnxtptr + i) % d];
		
		logger << "0x" << std::hex << v << std::dec;
	}
	
	logger << "]" << EndDebugInfo;
}

template <typename CONFIG>
void DSPI<CONFIG>::RX_FIFO_Clear()
{
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":RX FIFO:Clear()" << EndDebugInfo;
	}

	dspi_sr.template Set<typename DSPI_SR::RXCTR>(0); // RX FIFO Counter <- 0
	rxpart = 0;
	
	// Clear RFDF
	dspi_sr.template Set<typename DSPI_SR::RFDF>(0); // Receive FIFO Drain Flag
	UpdateINT_RFDF();
	UpdateDMA_RX();
	
	if(verbose)
	{
		RX_FIFO_Dump();
	}
}

template <typename CONFIG>
bool DSPI<CONFIG>::RX_FIFO_Empty() const
{
	return dspi_sr.template Get<typename DSPI_SR::RXCTR>() == 0; // RX FIFO Counter is null?
}

template <typename CONFIG>
bool DSPI<CONFIG>::RX_FIFO_Full() const
{
	unsigned int rxctr = dspi_sr.template Get<typename DSPI_SR::RXCTR>(); // RX FIFO Counter
	return rxctr >= RX_FIFO_Depth(); // RX FIFO Counter >= RX FIFO Depth?
}

template <typename CONFIG>
void DSPI<CONFIG>::RX_FIFO_Push(uint32_t rxdata)
{
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":RX FIFO:Push(rxdata=0x" << std::hex << rxdata << std::dec << ")" << EndDebugInfo;
	}
	
	unsigned int rxctr = dspi_sr.template Get<typename DSPI_SR::RXCTR>();
	
	if(rxctr < RX_FIFO_Depth()) // SPI receive FIFO is not full?
	{
		// SPI receive FIFO is not full
		
		// Fill-in RX FIFO
		unsigned int popnxtptr = dspi_sr.template Get<typename DSPI_SR::POPNXTPTR>(); // Pop Next Pointer
		unsigned int rxbackptr = (rxctr + popnxtptr) % RX_FIFO_Depth();
		dspi_rxfr[rxbackptr].template Set<typename DSPI_RXFR::RXDATA>(rxdata); // DSPI transmit FIFO register
		
		// Increment RX FIFO Counter
		rxctr++;
		dspi_sr.template Set<typename DSPI_SR::RXCTR>(rxctr);
		
		// Set RFDF if RX FIFO is not empty
		Set_RFDF_If_RX_FIFO_Not_Empty();
	}
	else
	{
		// SPI receive FIFO is full
		
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": Receive FIFO overflow" << EndDebugInfo;
		}

		dspi_sr.template Set<typename DSPI_SR::RFOF>(1); // receive FIFO overflow
		
		// update INT_RFOF
		UpdateINT_RFOF();
		
		bool rooe = dspi_mcr.template Get<typename DSPI_MCR::ROOE>(); // Receive FIFO overflow overwrite enable
		
		if(rooe) // overwrite?
		{
			// overwrite last in
			unsigned int popnxtptr = dspi_sr.template Get<typename DSPI_SR::POPNXTPTR>(); // Pop Next Pointer
			unsigned int lastinptr = (rxctr + popnxtptr - 1) % RX_FIFO_Depth();
			dspi_rxfr[lastinptr].template Set<typename DSPI_RXFR::RXDATA>(rxdata);
		}
	}
	
	if(verbose)
	{
		RX_FIFO_Dump();
	}
}

template <typename CONFIG>
uint32_t DSPI<CONFIG>::RX_FIFO_Front()
{
	unsigned int popnxtptr = dspi_sr.template Get<typename DSPI_SR::POPNXTPTR>(); // Pop Next Pointer
	return dspi_rxfr[popnxtptr].template Get<typename DSPI_RXFR::RXDATA>(); // DSPI Receive FIFO register
}

template <typename CONFIG>
void DSPI<CONFIG>::RX_FIFO_Pop()
{
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ":RX FIFO:Pop()" << EndDebugInfo;
	}

	unsigned int rxctr = dspi_sr.template Get<typename DSPI_SR::RXCTR>(); // RX FIFO Counter
	
	if(rxctr) // RX FIFO is not empty?
	{
		// Pop front
		unsigned int popnxtptr = dspi_sr.template Get<typename DSPI_SR::POPNXTPTR>(); // Pop Next Pointer
		popnxtptr = (popnxtptr + 1) % RX_FIFO_Depth();
		dspi_sr.template Set<typename DSPI_SR::POPNXTPTR>(popnxtptr);
		
		// Decrement RX FIFO Counter
		rxctr--;
		dspi_sr.template Set<typename DSPI_SR::RXCTR>(rxctr);
		
		// Set RFDF if RX FIFO is not empty
		Set_RFDF_If_RX_FIFO_Not_Empty();
	}
	
	if(verbose)
	{
		RX_FIFO_Dump();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::Set_TFFF_If_TX_FIFO_Not_Full()
{
	gen_tfff_event.notify(master_clock_period);
}

template <typename CONFIG>
void DSPI<CONFIG>::Set_CMDFFF_If_CMD_FIFO_Not_Full()
{
	gen_cmdfff_event.notify(master_clock_period);
}

template <typename CONFIG>
void DSPI<CONFIG>::Set_RFDF_If_RX_FIFO_Not_Empty()
{
	gen_rfdf_event.notify(master_clock_period);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_EOQF()
{
	gen_int_eoqf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_TFFF()
{
	gen_int_tfff_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_CMDFFF()
{
	gen_int_cmdfff_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_TFIWF()
{
	gen_int_tfiwf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_TCF()
{
	gen_int_tcf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_CMDTCF()
{
	gen_int_cmdtcf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_SPITCF()
{
	gen_int_spitcf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_DSITCF()
{
	gen_int_dsitcf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_TFUF()
{
	gen_int_tfuf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_RFDF()
{
	gen_int_rfdf_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_RFOF()
{
	gen_int_rfof_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_SPEF()
{
	gen_int_spef_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_DPEF()
{
	gen_int_dpef_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateINT_DDIF()
{
	gen_int_ddif_event.notify(sc_core::SC_ZERO_TIME);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateDMA_RX(const sc_core::sc_time& delay)
{
	gen_dma_rx_event.notify(delay);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateDMA_TX(const sc_core::sc_time& delay)
{
	gen_dma_tx_event.notify(delay);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateDMA_CMD(const sc_core::sc_time& delay)
{
	gen_dma_cmd_event.notify(delay);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateDMA_DD(const sc_core::sc_time& delay)
{
	gen_dma_dd_event.notify(delay);
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateBSYF()
{
	dspi_sr.template Set<typename DSPI_SR::BSYF>(MasterMode() && ExtendedMode() && (cmd_cycling_cnt > 1));
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateMasterClock()
{
	master_clock_period = m_clk_prop_proxy.GetClockPeriod();
	master_clock_start_time = m_clk_prop_proxy.GetClockStartTime();
	master_clock_posedge_first = m_clk_prop_proxy.GetClockPosEdgeFirst();
	master_clock_duty_cycle = m_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void DSPI<CONFIG>::UpdateDSPIClock()
{
	dspi_clock_period = dspi_clk_prop_proxy.GetClockPeriod();
	dspi_clock_start_time = dspi_clk_prop_proxy.GetClockStartTime();
	dspi_clock_posedge_first = dspi_clk_prop_proxy.GetClockPosEdgeFirst();
	dspi_clock_duty_cycle = dspi_clk_prop_proxy.GetClockDutyCycle();
}

template <typename CONFIG>
void DSPI<CONFIG>::MasterClockPropertiesChangedProcess()
{
	// Master Clock properties have changed
	UpdateMasterClock();
}

template <typename CONFIG>
void DSPI<CONFIG>::DSPIClockPropertiesChangedProcess()
{
	// DSPI Clock properties have changed
	UpdateDSPIClock();
}

template <typename CONFIG>
void DSPI<CONFIG>::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
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
bool DSPI<CONFIG>::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
	// Deny direct memory interface access
	dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
	dmi_data.set_start_address(0);
	dmi_data.set_end_address(sc_dt::uint64(-1));
	return false;
}

template <typename CONFIG>
unsigned int DSPI<CONFIG>::transport_dbg(tlm::tlm_generic_payload& payload)
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
tlm::tlm_sync_enum DSPI<CONFIG>::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
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
void DSPI<CONFIG>::nb_receive(int id, unisim::kernel::tlm2::tlm_serial_payload& payload, const sc_core::sc_time& t)
{
	switch(id)
	{
		case SIN_IF:
			if(verbose)
			{
				const unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();
				const sc_core::sc_time& period = payload.get_period();
				
				logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": receiving [" << data << "] over SIN with a period of " << period << EndDebugInfo;
			}
			
			sin_bitstream.fill(payload, t);
			break;
		
		case SS_IF:
			if(verbose)
			{
				const unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();
				const sc_core::sc_time& period = payload.get_period();
				
				logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": receiving [" << data << "] over SS with a period of " << period << EndDebugInfo;
			}
			
			ss_bitstream.fill(payload, t);
			break;
	}
}

//////////////// unisim::service::interface::Registers ////////////////////

template <typename CONFIG>
unisim::service::interfaces::Register *DSPI<CONFIG>::GetRegister(const char *name)
{
	return registers_registry.GetRegister(name);
}

template <typename CONFIG>
void DSPI<CONFIG>::ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
{
	registers_registry.ScanRegisters(scanner);
}


template <typename CONFIG>
void DSPI<CONFIG>::ProcessEvent(Event *event)
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
void DSPI<CONFIG>::ProcessEvents()
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
void DSPI<CONFIG>::Process()
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
void DSPI<CONFIG>::INT_EOQF_Process()
{
	bool eoqf = dspi_sr.template Get<typename DSPI_SR::EOQF>();
	bool eoqf_re = dspi_rser.template Get<typename DSPI_RSER::EOQF_RE>();
	bool int_eoqf = eoqf && eoqf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_EOQF.name() << " <- " << int_eoqf << EndDebugInfo;
	}
	
	INT_EOQF = int_eoqf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_TFFF_Process()
{
	bool tfff = dspi_sr.template Get<typename DSPI_SR::TFFF>();
	bool tfff_re = dspi_rser.template Get<typename DSPI_RSER::TFFF_RE>();
	bool tfff_dirs = dspi_rser.template Get<typename DSPI_RSER::TFFF_DIRS>();
	bool int_tfff = !tfff_dirs && tfff && tfff_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_TFFF.name() << " <- " << int_tfff << EndDebugInfo;
	}
	
	INT_TFFF = int_tfff;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_CMDFFF_Process()
{
	bool cmdfff = dspi_sr.template Get<typename DSPI_SR::CMDFFF>();
	bool cmdfff_re = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_RE>();
	bool cmdfff_dirs = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_DIRS>();
	bool int_cmdfff = !cmdfff_dirs && cmdfff && cmdfff_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_CMDFFF.name() << " <- " << int_cmdfff << EndDebugInfo;
	}
	
	INT_CMDFFF = int_cmdfff;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_TFIWF_Process()
{
	bool tfiwf = dspi_sr.template Get<typename DSPI_SR::TFIWF>();
	bool tfiwf_re = dspi_rser.template Get<typename DSPI_RSER::TFIWF_RE>();
	bool int_tfiwf = tfiwf && tfiwf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_TFIWF.name() << " <- " << int_tfiwf << EndDebugInfo;
	}
	
	INT_TFIWF = int_tfiwf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_TCF_Process()
{
	bool tcf = dspi_sr.template Get<typename DSPI_SR::TCF>();
	bool tcf_re = dspi_rser.template Get<typename DSPI_RSER::TCF_RE>();
	bool int_tcf = tcf && tcf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_TCF.name() << " <- " << int_tcf << EndDebugInfo;
	}
	
	INT_TCF = int_tcf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_CMDTCF_Process()
{
	bool cmdtcf = dspi_sr.template Get<typename DSPI_SR::CMDTCF>();
	bool cmdtcf_re = dspi_rser.template Get<typename DSPI_RSER::CMDTCF_RE>();
	bool int_cmdtcf = cmdtcf && cmdtcf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_CMDTCF.name() << " <- " << int_cmdtcf << EndDebugInfo;
	}
	
	INT_CMDTCF = int_cmdtcf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_SPITCF_Process()
{
	bool spitcf = dspi_sr.template Get<typename DSPI_SR::SPITCF>();
	bool spitcf_re = dspi_rser.template Get<typename DSPI_RSER::SPITCF_RE>();
	bool int_spitcf = spitcf && spitcf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_SPITCF.name() << " <- " << int_spitcf << EndDebugInfo;
	}
	
	INT_SPITCF = int_spitcf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_DSITCF_Process()
{
	bool dsitcf = dspi_sr.template Get<typename DSPI_SR::DSITCF>();
	bool dsitcf_re = dspi_rser.template Get<typename DSPI_RSER::DSITCF_RE>();
	bool int_dsitcf = dsitcf && dsitcf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_DSITCF.name() << " <- " << int_dsitcf << EndDebugInfo;
	}
	
	INT_DSITCF = int_dsitcf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_TFUF_Process()
{
	bool tfuf = dspi_sr.template Get<typename DSPI_SR::TFUF>();
	bool tfuf_re = dspi_rser.template Get<typename DSPI_RSER::TFUF_RE>();
	bool int_tfuf = tfuf && tfuf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_TFUF.name() << " <- " << int_tfuf << EndDebugInfo;
	}
	
	INT_TFUF = int_tfuf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_RFDF_Process()
{
	bool rfdf = dspi_sr.template Get<typename DSPI_SR::RFDF>();
	bool rfdf_re = dspi_rser.template Get<typename DSPI_RSER::RFDF_RE>();
	bool rfdf_dirs = dspi_rser.template Get<typename DSPI_RSER::RFDF_DIRS>();
	bool int_rfdf = !rfdf_dirs && rfdf && rfdf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_RFDF.name() << " <- " << int_rfdf << EndDebugInfo;
	}
	
	INT_RFDF = int_rfdf;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_RFOF_Process()
{
	bool rfof = dspi_sr.template Get<typename DSPI_SR::RFOF>();
	bool rfof_re = dspi_rser.template Get<typename DSPI_RSER::RFOF_RE>();
	bool int_rfof = rfof && rfof_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_RFOF.name() << " <- " << int_rfof << EndDebugInfo;
	}
	
	INT_RFOF = int_rfof;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_SPEF_Process()
{
	bool spef = dspi_sr.template Get<typename DSPI_SR::SPEF>();
	bool spef_re = dspi_rser.template Get<typename DSPI_RSER::SPEF_RE>();
	bool int_spef = spef && spef_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_SPEF.name() << " <- " << int_spef << EndDebugInfo;
	}
	
	INT_SPEF = int_spef;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_DPEF_Process()
{
	bool dpef = dspi_sr.template Get<typename DSPI_SR::DPEF>();
	bool dpef_re = dspi_rser.template Get<typename DSPI_RSER::DPEF_RE>();
	bool int_dpef = dpef && dpef_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_DPEF.name() << " <- " << int_dpef << EndDebugInfo;
	}
	
	INT_DPEF = int_dpef;
}

template <typename CONFIG>
void DSPI<CONFIG>::INT_DDIF_Process()
{
	bool ddif = dspi_sr.template Get<typename DSPI_SR::DDIF>();
	bool ddif_re = dspi_rser.template Get<typename DSPI_RSER::DDIF_RE>();
	bool int_ddif = ddif && ddif_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << INT_DDIF.name() << " <- " << int_ddif << EndDebugInfo;
	}
	
	INT_DDIF = int_ddif;
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_RX_Process()
{
	bool rfdf = dspi_sr.template Get<typename DSPI_SR::RFDF>();
	bool rfdf_re = dspi_rser.template Get<typename DSPI_RSER::RFDF_RE>();
	bool rfdf_dirs = dspi_rser.template Get<typename DSPI_RSER::RFDF_DIRS>();
	bool dma_rx = rfdf_dirs && rfdf && rfdf_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_RX.name() << " <- " << dma_rx << EndDebugInfo;
	}
	
	DMA_RX = dma_rx;
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_TX_Process()
{
	bool tfff = dspi_sr.template Get<typename DSPI_SR::TFFF>();
	bool tfff_re = dspi_rser.template Get<typename DSPI_RSER::TFFF_RE>();
	bool tfff_dirs = dspi_rser.template Get<typename DSPI_RSER::TFFF_DIRS>();
	bool dma_tx = tfff_dirs && tfff && tfff_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_TX.name() << " <- " << dma_tx << EndDebugInfo;
	}
	
	DMA_TX = dma_tx;
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_CMD_Process()
{
	bool cmdfff = dspi_sr.template Get<typename DSPI_SR::CMDFFF>();
	bool cmdfff_re = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_RE>();
	bool cmdfff_dirs = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_DIRS>();
	bool dma_cmd = cmdfff_dirs && cmdfff && cmdfff_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_CMD.name() << " <- " << dma_cmd << EndDebugInfo;
	}
	
	DMA_CMD = dma_cmd;
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_DD_Process()
{
	bool ddif = dspi_sr.template Get<typename DSPI_SR::DDIF>();
	bool ddif_re = dspi_rser.template Get<typename DSPI_RSER::DDIF_RE>();
	bool ddif_dirs = dspi_rser.template Get<typename DSPI_RSER::DDIF_DIRS>();
	bool dma_dd = ddif_dirs && ddif && ddif_re;
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": " << DMA_DD.name() << " <- " << dma_dd << EndDebugInfo;
	}
	
	DMA_DD = dma_dd;
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_ACK_RX_Process()
{
	if(DMA_ACK_RX.posedge())
	{
		bool rfdf_re = dspi_rser.template Get<typename DSPI_RSER::RFDF_RE>();
		bool rfdf_dirs = dspi_rser.template Get<typename DSPI_RSER::RFDF_DIRS>();
		
		if(rfdf_re && rfdf_dirs)
		{
			dspi_sr.template Set<typename DSPI_SR::RFDF>(0); // DMA RX acknowledgement clears DSPI_SR[RFDF]
			UpdateDMA_RX(/*master_clock_period*/);
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_ACK_TX_Process()
{
	if(DMA_ACK_TX.posedge())
	{
		bool tfff_re = dspi_rser.template Get<typename DSPI_RSER::TFFF_RE>();
		bool tfff_dirs = dspi_rser.template Get<typename DSPI_RSER::TFFF_DIRS>();
		
		if(tfff_re && tfff_dirs)
		{
			dspi_sr.template Set<typename DSPI_SR::TFFF>(0); // DMA TX acknowledgement clears DSPI_SR[TFFF]
			UpdateDMA_TX(/*master_clock_period*/);
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_ACK_CMD_Process()
{
	if(DMA_ACK_CMD.posedge())
	{
		bool cmdfff_re = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_RE>();
		bool cmdfff_dirs = dspi_rser.template Get<typename DSPI_RSER::CMDFFF_DIRS>();
		
		if(cmdfff_re && cmdfff_dirs)
		{
			dspi_sr.template Set<typename DSPI_SR::CMDFFF>(0); // DMA CMD acknowledgement clears DSPI_SR[CMDFFF]
			UpdateDMA_CMD(/*master_clock_period*/);
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::DMA_ACK_DD_Process()
{
	if(DMA_ACK_DD.posedge())
	{
		bool ddif_re = dspi_rser.template Get<typename DSPI_RSER::DDIF_RE>();
		bool ddif_dirs = dspi_rser.template Get<typename DSPI_RSER::DDIF_DIRS>();
		
		if(ddif_re && ddif_dirs)
		{
			dspi_sr.template Set<typename DSPI_SR::DDIF>(0); // Deserialized Data Match DMA acknowledgement clears DSPI_SR[DDIF]
			UpdateDMA_DD(/*master_clock_period*/);
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::Set_TFFF_Process()
{
	if(!TX_FIFO_Full())
	{
		dspi_sr.template Set<typename DSPI_SR::TFFF>(1); // Transmit FIFO Fill Flag
		UpdateINT_TFFF();
		UpdateDMA_TX();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::Set_CMDFFF_Process()
{
	if(!CMD_FIFO_Full())
	{
		dspi_sr.template Set<typename DSPI_SR::CMDFFF>(1); // CMD FIFO Fill Flag
		UpdateINT_CMDFFF();
		UpdateDMA_CMD();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::Set_RFDF_Process()
{
	if(!RX_FIFO_Empty())
	{
		dspi_sr.template Set<typename DSPI_SR::RFDF>(1); // Receive FIFO Drain Flag
		UpdateINT_RFDF();
		UpdateDMA_RX();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::HT_Process()
{
	// In master mode while in DSI or CSI Configurations, the HT signal initiates a data transfer
	// when the DSICR0[TRRE] bit is set and a rising or falling edge is detected on HT. Which
	// edge to trigger on is determined by the DSICR0[TPOL] bit.

	if(MasterMode()) // Master Mode?
	{
		bool trre = dspi_dsicr0.template Get<typename DSPI_DSICR0::TRRE>(); // Trigger Reception Enable

		if(trre)
		{
			bool tpol = dspi_dsicr0.template Get<typename DSPI_DSICR0::TPOL>(); // Trigger Polarity
			
			if(tpol)
			{
				// rising edge
				if(HT.posedge()) DSI_TriggerTransfer();
			}
			else
			{
				// falling edge
				if(HT.negedge()) DSI_TriggerTransfer();
			}
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::DEBUG_Process()
{
	// debug signal affects state of DSPI controller
	UpdateState();
}

template <typename CONFIG>
void DSPI<CONFIG>::SetPCS(unsigned int slave_num, const sc_core::sc_time& t, const sc_core::sc_time& duration, bool value)
{
	unisim::kernel::tlm2::tlm_serial_payload payload;
	payload.set_period(duration);
	unisim::kernel::tlm2::tlm_serial_payload::data_type& data = payload.get_data();
	data.push_back(value);
	
	if(verbose)
	{
		logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": transmitting [" << data << "] over PCS_" << slave_num << " with a period of " << duration << EndDebugInfo;
	}
	PCS[slave_num]->nb_send(payload, t);
}

template <typename CONFIG>
bool DSPI<CONFIG>::SPI_TransferTriggered() const
{
	return !TX_FIFO_Empty() || (ExtendedMode() && !CMD_FIFO_Empty());
}

template <typename CONFIG>
void DSPI<CONFIG>::DSI_TriggerTransfer()
{
	trigger_dsi_transfer = true;
	ScheduleTransfer();
}

template <typename CONFIG>
void DSPI<CONFIG>::ScheduleTransfer()
{
	if(MasterMode() && Running() && !ModuleDisabled() && (SPI_TransferTriggered() || trigger_dsi_transfer))
	{
		const sc_core::sc_time& curr_time_stamp = sc_core::sc_time_stamp();
		sc_core::sc_time notify_delay(curr_time_stamp);
		if(transfer_ready_time > curr_time_stamp) notify_delay = transfer_ready_time;
		unisim::kernel::tlm2::AlignToClock(notify_delay, dspi_clock_period, dspi_clock_start_time, dspi_clock_posedge_first, dspi_clock_duty_cycle);
		notify_delay -= curr_time_stamp;
		
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": scheduling transfer after " << notify_delay << EndDebugInfo;
		}
		
		transfer_event.notify(notify_delay);
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::LatchSerializedData()
{
	serialized_data = 0;
	
	bool txss = dspi_dsicr0.template Get<typename DSPI_DSICR0::TXSS>();                     // Transmit Data Source Select
	uint32_t ss = txss ? ~uint32_t(0) : dspi_ssr0.template Get<typename DSPI_SSR0::SS>(); // Source Select
	
	unsigned int i;
	
	for(i = 0; i < NUM_DSI_INPUTS; i++)
	{
		bool sdr0_bit_value = dspi_sdr0.Get(i);  // Serialized Data
		bool asdr0_bit_value = dspi_asdr0.Get(i); // Alternate Serialized Data
		bool bit_value = ((ss >> i) & 1) ? asdr0_bit_value : sdr0_bit_value;
		serialized_data |= (bit_value << i);
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::CheckChangeInData()
{
	bool cid = dspi_dsicr0.template Get<typename DSPI_DSICR0::CID>();
	
	if(cid)
	{
		if(serialized_data != uint32_t(dspi_compr0))
		{
			DSI_TriggerTransfer();
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::CheckDeserializedData()
{
	uint32_t deser_data = dspi_ddr0.template Get<typename DSPI_DDR0::DESER_DATA>(); // Deserialized Data
	uint32_t dp = dspi_dpir0.template Get<typename DSPI_DPIR0::DP>();               // Data Polarity
	uint32_t mask = dspi_dimr0.template Get<typename DSPI_DIMR0::MASK>();           // Mask
	
	if(((deser_data & dp & mask) != 0) ||   // match 1's
	   ((~deser_data & ~dp & mask) != 0))   // match 0's
	{
		dspi_sr.template Set<typename DSPI_SR::DDIF>(1); // DSI Data Received with Active Bits
		UpdateINT_DDIF();
		UpdateDMA_DD();
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::LatchDSI_INPUT()
{
	unsigned int i;
	for(i = 0; i < NUM_DSI_INPUTS; i++)
	{
		const sc_core::sc_in<bool>& dsi_input = DSI_INPUT[i];
		dspi_sdr0.Set(i, dsi_input.read());
	}
	
	CheckChangeInData();
}

template <typename CONFIG>
void DSPI<CONFIG>::MasterTransfer(uint32_t& rxdata, const uint32_t& txdata, bool& parity_error, const TransferControl& ctrl)
{
	bool cont = ctrl.cont;
	unsigned int ctas = ctrl.ctas;
	bool mtfe = dspi_mcr.template Get<typename DSPI_MCR::MTFE>();
	bool lsbfe = ctrl.lsbfe;
	bool cpha = ctrl.cpha;
	bool pe = ctrl.pe;
	unsigned int frame_size = ctrl.frame_size;
	bool mcsc = ctrl.mcsc;
	bool masc = ctrl.masc;
	unsigned int pcs = ctrl.pcs;
	
	const DSPI_CTAR& ctar = dspi_ctar[ctas];
	
	const sc_core::sc_time& sck_period = ctar.GetSCKPeriod();
	const sc_core::sc_time& sck_half_period = ctar.GetSCKHalfPeriod();
	const sc_core::sc_time& t_DT = ctar.GetDelayAfterTransfer();
	const sc_core::sc_time& t_CSC = ctar.GetPCS2SCKDelay();
	const sc_core::sc_time& t_ASC = ctar.GetAfterSCKDelay();
	unsigned int min_frame_size = ctar.GetMinFrameSize();
	
	sc_core::sc_time sample_point(t_CSC);
	
	if(cpha)
	{
		sample_point += sck_half_period; // CPHA=1: sample point is after one half SCK period
	}

	if(mtfe)
	{
		// If SCK/protocol clock ratio is less than 4, the actually sample
		// point is delayed by 1 protocol clock cycle automatically by the design.
		unsigned int pbr = (2 * ctar.template Get<typename DSPI_CTAR::PBR>()) + 1;
		unsigned int dbr = ctar.template Get<typename DSPI_CTAR::DBR>();
		unsigned int br = 1 << (ctar.template Get<typename DSPI_CTAR::BR>() + 1);
		sample_point += ((pbr * (1 + dbr)) >= (4 * br)) ? dspi_mcr.GetSamplePoint() : dspi_clock_period;
	}

	if(unlikely(verbose))
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": frame size = " << frame_size << EndDebugInfo;
		logger << DebugInfo << sc_core::sc_time_stamp() << ": SCK period = " << sck_period << EndDebugInfo;
		logger << DebugInfo << sc_core::sc_time_stamp() << ": tCSC = " << t_CSC << EndDebugInfo;
		logger << DebugInfo << sc_core::sc_time_stamp() << ": tASC = " << t_ASC << EndDebugInfo;
		logger << DebugInfo << sc_core::sc_time_stamp() << ": tDT = " << t_DT << EndDebugInfo;
		logger << DebugInfo << sc_core::sc_time_stamp() << ": sample point = " << sample_point << EndDebugInfo;
		logger << DebugInfo << sc_core::sc_time_stamp() << ": minimum allowable frame size = " << min_frame_size << EndDebugInfo;
	}
	
	if(unlikely(frame_size < min_frame_size))
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ": frame size (" << frame_size << ") is lower than minimum frame size (" << min_frame_size << ")" << EndDebugWarning;
	}
	
	if(mtfe && (t_ASC < sck_half_period))
	{
		logger << DebugWarning << sc_core::sc_time_stamp() << ": the After SCK (tASC) delay must be greater or equal to half of the SCK period" << EndDebugWarning;
	}
	
	// Continous selection format (controlled by DSPI_MCR[CONT]): no t_DT between frames, slaves keep selected
	
	// Fast continous selection format (controlled by DSPI_MCR[FCPCS])
	//  - no t_CSC at start of next frame (controlled by CMD[MCSC]),
	//  - no t_CSC at start of next frame (controlled by CMD[MCSC]) and no t_ASC at end of current frame (controlled by CMD[MASC])
	
	sc_core::sc_time sof_time = sc_core::sc_time_stamp();
	sc_core::sc_time transmit_time = sc_core::SC_ZERO_TIME;
	
	sc_core::sc_time pcs_time(sck_period);
	pcs_time *= (double) frame_size;
	
	if(mcsc && masc)
	{
		// Masking of both tASC and tCSC delays is allowed. In this case, the delay between two
		// frames is equal to half the baud rate set by the user software.
		pcs_time += sck_period;
		pcs_time += sck_period;
		if(cpha) transmit_time += sck_period;
	}
	else
	{
		if(!mcsc)
		{
			pcs_time += t_CSC;
			if(cpha) transmit_time += t_CSC;
		}
		if(!masc) pcs_time += t_ASC;
	}
	
	// Select slaves
	unsigned int pcsis = dspi_mcr.template Get<typename DSPI_MCR::PCSIS>(); // Peripheral Chip Select Inactive State
	
	unsigned int slave_num;
	for(slave_num = 0; slave_num < NUM_CTARS; slave_num++)
	{
		bool pcs_value = ((pcs >> slave_num) & 1) != 0;
		bool pcsis_value = ((pcsis >> slave_num) & 1) != 0;
		SetPCS(slave_num, sc_core::SC_ZERO_TIME, pcs_time, pcsis_value ? !pcs_value : pcs_value);
	}

	// Transmit frame
	unisim::kernel::tlm2::tlm_serial_payload outcoming_payload;
	
	outcoming_payload.set_period(sck_period);
	
	unisim::kernel::tlm2::tlm_serial_payload::data_type& outcoming_data = outcoming_payload.get_data();
	
	sc_core::sc_time tx_time = sc_core::SC_ZERO_TIME;
	
	uint32_t tx_shift_reg = txdata;
	
	unsigned int i;
	for(i = 0; i < frame_size; i++)
	{
		if(!pe || (i < (frame_size - 1)))
		{
			// transmit a data bit
			bool bit_value = lsbfe ? (tx_shift_reg & 1) : ((tx_shift_reg >> (frame_size - 1)) & 1); // get data bit from shift register
			
			outcoming_data.push_back(bit_value); // transmit data bit
			
			// shift
			if(lsbfe)
			{
				tx_shift_reg >>= 1;
			}
			else
			{
				tx_shift_reg <<= 1;
			}
			
			tx_parity_bit ^= bit_value; // update computed parity bit
		}
		else
		{
			// transmit parity bit
			outcoming_data.push_back(tx_parity_bit);
		}
		
		tx_time += sck_period;
	}
	
	if(verbose)
	{
		logger << DebugInfo << (sc_core::sc_time_stamp() + transmit_time) << ": transmitting [" << outcoming_data << "] over SOUT with a period of " << sck_period << EndDebugInfo;
	}
	
	SOUT->nb_send(outcoming_payload, transmit_time);

	// Receive frame
	wait(sample_point); // Slave sends the response though nb_receive call back in that interval
	sin_bitstream.latch();
	
	sc_core::sc_time rx_time = sc_core::SC_ZERO_TIME;
	
	uint32_t rx_shift_reg = 0;
	parity_error = false;
	
	for(i = 0; i < frame_size; i++)
	{
		if(sin_bitstream.seek(rx_time) == TLM_INPUT_BITSTREAM_NEED_SYNC)
		{
			logger << DebugWarning << (sc_core::sc_time_stamp() + rx_time) << ": missing bit #" << i << " from slave" << EndDebugWarning;
		}
		
		// shift
		if(lsbfe)
		{
			rx_shift_reg >>= 1;
		}
		else
		{
			rx_shift_reg <<= 1;
		}
		
		bool bit_value = sin_bitstream.read(); // sample SIN
		
		if(verbose)
		{
			logger << DebugInfo << (sc_core::sc_time_stamp() + rx_time) << ": SIN = '" << bit_value << "'" << EndDebugInfo;
		}
		
		if(!pe || (i < (frame_size - 1)))
		{
			// receive a data bit
			
			// insert data bit in shift register
			rx_shift_reg |= lsbfe ? (bit_value << (frame_size - 1)) : bit_value;
			
			rx_parity_bit ^= bit_value; // update computed parity bit
		}
		else
		{
			// receive parity bit
			
			if(rx_parity_bit != bit_value)
			{
				if(verbose)
				{
					logger << DebugInfo << (sc_core::sc_time_stamp() + rx_time) << ": parity error" << EndDebugInfo;
				}
				
				parity_error = true;
			}
		}
		
		rx_time += sck_period;
	}
	
	rxdata = rx_shift_reg;
	
	assert(tx_time == rx_time);
	assert(tx_time < pcs_time);
	
	transfer_ready_time = sof_time;
	transfer_ready_time += pcs_time;
	
	// Deselect slaves
	if(!cont) // continuous selection format?
	{
		sc_core::sc_time t(transfer_ready_time);
		t -= sc_core::sc_time_stamp();
		// no: deselect slaves
		for(slave_num = 0; slave_num < NUM_CTARS; slave_num++)
		{
			SetPCS(slave_num, t, t_DT, ((pcsis >> slave_num) & 1) != 0);
		}
		
		transfer_ready_time += t_DT;
	}
	
	sc_core::sc_time time_to_next_transfer(transfer_ready_time);
	time_to_next_transfer -= sc_core::sc_time_stamp();
	wait(time_to_next_transfer);
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": transfer completed" << EndDebugInfo;
	}
	
	dspi_sr.template Set<typename DSPI_SR::TCF>(1); // Transfer Complete Flag
	UpdateINT_TCF();
}

template <typename CONFIG>
bool DSPI<CONFIG>::SelectConfiguration(DSPI_Configuration& sel_dconf) const
{
	bool trigger_spi_transfer = SPI_TransferTriggered();
	
	DSPI_Configuration conf = Configuration();
	
	switch(conf)
	{
		case DCONF_CSI:
			{
				if(HAS_DATA_SERIALIZATION_SUPPORT)
				{
					bool tsbc = dspi_dsicr0.template Get<typename DSPI_DSICR0::TSBC>();
					if(tsbc) // timed serial bus configuration enabled?
					{
						bool itsb = dspi_dsicr0.template Get<typename DSPI_DSICR0::ITSB>();
						if(itsb) // interleave TSB mode enabled?
						{
							// On every trigger, frames from DSI are sent when there
							// are no frames in the SPI or the previous transmission was a frame from SPI
							if(((prev_dconf == DCONF_SPI) || !trigger_spi_transfer) &&
							!DSI_OperationsStopped() &&
							!DSI_FrameTransmissionsStopped())
							{
								sel_dconf = DCONF_DSI;
								return true;
							}
							else if(!SPI_FrameTransmissionsStopped())
							{
								sel_dconf = DCONF_SPI;
								return true;
							}
						}
						else
						{
							// TSB configuration: transmission of SPI data has higher priority than DSI data
							if(trigger_spi_transfer &&
							!SPI_FrameTransmissionsStopped())
							{
								sel_dconf = DCONF_SPI;
								return true;
							}
							else if(!DSI_OperationsStopped() &&
									!DSI_FrameTransmissionsStopped())
							{
								sel_dconf = DCONF_DSI;
								return true;
							}
						}
					}
					else
					{
						// The normal CSI configuration is a combination of the SPI and DSI configuration.
						// The DSPI interleaves DSI data frames with SPI data frames.
						// Interleaving is done on the frame boundaries.
						if(trigger_spi_transfer && trigger_dsi_transfer)
						{
							if((prev_dconf == DCONF_SPI) &&
							!DSI_OperationsStopped() &&
							!DSI_FrameTransmissionsStopped())
							{
								sel_dconf = DCONF_DSI;
								return true;
							}
							else if(!SPI_FrameTransmissionsStopped())
							{
								sel_dconf = DCONF_SPI;
								return true;
							}
						}
						else if(trigger_spi_transfer && !SPI_FrameTransmissionsStopped())
						{
							sel_dconf = DCONF_SPI;
							return true;
						}
						else if(trigger_dsi_transfer && !DSI_OperationsStopped() && !DSI_FrameTransmissionsStopped())
						{
							sel_dconf = DCONF_DSI;
							return true;
						}
					}
				}
				else
				{
					sel_dconf = DCONF_SPI;
					return true;
				}
			}
			break;
			
		case DCONF_SPI:
			if(!SPI_FrameTransmissionsStopped())
			{
				sel_dconf = DCONF_SPI;
				return true;
			}
			break;
			
		case DCONF_DSI:
			if(HAS_DATA_SERIALIZATION_SUPPORT)
			{
				if(!DSI_OperationsStopped() &&
				!DSI_FrameTransmissionsStopped())
				{
					sel_dconf = DCONF_DSI;
					return true;
				}
			}
			break;
			
		case DCONF_RES:
			return false;
	}
	
	return false;
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_MasterTransfer()
{
	bool has_cmd = false;
	
	if(ExtendedMode()) // XSPI?
	{
		// XSPI mode
		if(cmd_cycling_cnt) // is a command cycling in progress?
		{
			// yes: reuse command
			has_cmd = true;
		}
		else if(!CMD_FIFO_Empty()) // is CMD FIFO empty?
		{
			// no: pop command from CMD FIFO
			cmd = SPI_Master_CMD_FIFO_Front();
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":CMD FIFO:Front() = 0x" << std::hex << cmd << std::dec << EndDebugInfo;
			}
			XSPI_Master_CMD_FIFO_Pop();
			has_cmd = true;
		}
	}
	else
	{
		// SPI mode
		if(!TX_FIFO_Empty()) // is CMD FIFO empty?
		{
			// no: get command from CMD FIFO
			cmd = SPI_Master_CMD_FIFO_Front();
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":CMD FIFO:Front() = 0x" << std::hex << cmd << std::dec << EndDebugInfo;
			}
			has_cmd = true;
		}
	}
	
	if(has_cmd)
	{
		TransferControl ctrl;
		
		bool transfer = false;
		
		bool fcpcs = dspi_mcr.template Get<typename DSPI_MCR::FCPCS>();                             // fast continous PCS mode
		ctrl.cont = DSPI_PUSHR::CONT::template Get<uint32_t>(cmd << 16);                            // continuous peripheral chip select
		bool prev_cont = DSPI_PUSHR::CONT::template Get<uint32_t>(prev_cmd << 16);                  // continuous peripheral chip select
		bool cont_posedge = (!prev_cont && ctrl.cont);                                              // first frame in the continuous selection format?
		bool cont_negedge = (prev_cont && !ctrl.cont);                                              // last frame in the continuous selection format?
		
		bool cont_scke = dspi_mcr.template Get<typename DSPI_MCR::CONT_SCKE>();
		
		ctrl.ctas = DSPI_PUSHR::CTAS::template Get<uint32_t>(cmd << 16);                            // clock and transfer attribute select
		
		ctrl.lsbfe = dspi_ctar[ctrl.ctas].template Get<typename DSPI_CTAR::LSBFE>();                // LSB First

		bool ctcnt = DSPI_PUSHR::CTCNT::template Get<uint32_t>(cmd << 16);                          // clear transfer counter
		
		bool masc = DSPI_PUSHR::PE_MASC::template Get<uint32_t>(cmd << 16);                         // mask tASC delay in the current frame
		bool mcsc = DSPI_PUSHR::PP_MCSC::template Get<uint32_t>(prev_cmd << 16);                    // mask tCSC delay in the next frame (from previous command)
		
		if(fcpcs && !ctrl.cont && (mcsc || masc))
		{
			logger << DebugWarning << sc_core::sc_time_stamp() << ": software must not mask the \"After SCK\" (tASC) delay or the \"PCS to SCK\" (tCSC) delay if the continuous selection format is not used and " << dspi_mcr.GetName() << "[FCPCS] is asserted" << EndDebugWarning;
		}
	
		if(ctrl.cont && fcpcs) // continuous selection format?
		{
			if(unlikely(masc && cont_negedge))
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": the \"After SCK\" (tASC) delay must not be masked when the current frame is the last frame in the continuous selection format" << EndDebugWarning;
			}
			
			if(unlikely(mcsc && cont_posedge))
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": the \"PCS to SCK\" (tCSC) delay for the first frame in the continuous selection format cannot be masked" << EndDebugWarning;
			}
			
			if(unlikely(masc && !mcsc))
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": masking of only tASC is not allowed; If tASC is masked then tCSC must be masked too" << EndDebugWarning;
			}
			
			if(unlikely(mcsc && !masc && (dspi_ctar[ctrl.ctas].GetAfterSCKDelay() < dspi_ctar[ctrl.ctas].GetSCKPeriod())))
			{
				logger << DebugWarning << sc_core::sc_time_stamp() << ": when masking only tCSC, software must ensure that the tASC time is greater than the baud rate" << EndDebugWarning;
			}
			
			ctrl.mcsc = mcsc && !cont_posedge;
			ctrl.masc = mcsc && masc && !cont_negedge;
		}
		else
		{
			// classic SPI transfer format
			ctrl.mcsc = false;
			ctrl.masc = false;
		}
		
		ctrl.pcs = DSPI_PUSHR::PCS::template Get<uint32_t>(cmd << 16);                              // peripheral chip select
		ctrl.pe = (!ctrl.cont || !fcpcs) && DSPI_PUSHR::PE_MASC::template Get<uint32_t>(cmd << 16); // parity enable
		bool pp = DSPI_PUSHR::PP_MCSC::template Get<uint32_t>(cmd << 16);                           // parity polarity
		ctrl.cpha = cont_scke || dspi_ctar[ctrl.ctas].template Get<typename DSPI_CTAR::CPHA>();          // clock phase
		
		prev_cmd = cmd;
		
		if(ExtendedMode() && !cmd_cycling_cnt)
		{
			cmd_cycling_cnt = dspi_ctare[ctrl.ctas].template Get<typename DSPI_CTARE::DTCP>(); // data transfer count preload
		}
		
		UpdateBSYF();
		
		uint32_t txdata = 0;
		unsigned int frame_size = MasterFrameSize(DCONF_SPI, ctrl.ctas);
		ctrl.frame_size = frame_size;
	
		if(frame_size > 16)
		{
			// When Extended SPI Mode (DSPIx_MCR[XSPI]) is enabled and the frame size of SPI
			// Data to be transmitted is more than 16 bits, then two Data entries are popped from TX
			// FIFO simultaneously and transferred to the shift register. The first of the two popped
			// entries forms the 16 LSB bits of the SPI frame. Such an operation also causes TX FIFO
			// Counter to decrement by two.
		
			if(TX_FIFO_Size() >= 2)
			{
				uint16_t lsb_txdata = SPI_Master_TX_FIFO_Front();
				if(verbose)
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Front() = 0x" << std::hex << lsb_txdata << std::dec << EndDebugInfo;
				}
				SPI_Master_TX_FIFO_Pop();
				
				uint16_t msb_txdata = SPI_Master_TX_FIFO_Front();
				if(verbose)
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Front() = 0x" << std::hex << msb_txdata << std::dec << EndDebugInfo;
				}
				SPI_Master_TX_FIFO_Pop();
				
				txdata = (uint32_t) lsb_txdata | ((uint32_t) msb_txdata << 16);
				
				transfer = true;
			}
			else
			{
				// Command with no data
				dspi_sr.template Set<typename DSPI_SR::TFIWF>(1); // Transmit FIFO Invalid Write interrupt request
				UpdateINT_TFIWF();
			}
		}
		else if(!TX_FIFO_Empty())
		{
			uint16_t lsb_txdata = SPI_Master_TX_FIFO_Front();
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Front() = 0x" << std::hex << lsb_txdata << std::dec << EndDebugInfo;
			}
			SPI_Master_TX_FIFO_Pop();
			
			txdata = (uint32_t) lsb_txdata;
			
			transfer = true;
		}
		else
		{
			// Command with no data
			dspi_sr.template Set<typename DSPI_SR::TFIWF>(1); // Transmit FIFO Invalid Write interrupt request
			UpdateINT_TFIWF();
		}
		
		if(transfer)
		{
			uint32_t rxdata = 0xdeadbeef;
			bool parity_error = false;
			
			if(!ctrl.cont || cont_posedge)  // NO continuous selection OR first frame in the continuous selection format?
			{
				if(ctcnt) // clear SPI Transfer Counter?
				{
					dspi_tcr.template Set<typename DSPI_TCR::SPI_TCNT>(0); // clear counter
				}
				rx_parity_bit = pp; // initialize parity bit for reception
				tx_parity_bit = pp; // initialize parity bit for transmission
			}
			
			MasterTransfer(rxdata, txdata, parity_error, ctrl);
			
			// Increment SPI Transfer Counter
			dspi_tcr.template Set<typename DSPI_TCR::SPI_TCNT>(dspi_tcr.template Get<typename DSPI_TCR::SPI_TCNT>() + 1);
			
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": SPI transfer completed" << EndDebugInfo;
			}
			
			dspi_sr.template Set<typename DSPI_SR::SPITCF>(1); // SPI Frame Transfer Complete Flag
			UpdateINT_SPITCF();
			
			if(parity_error)
			{
				if(verbose)
				{
					logger << DebugInfo << sc_core::sc_time_stamp() << ": SPI parity error" << EndDebugInfo;
				}
				dspi_sr.template Set<typename DSPI_SR::SPEF>(1); // SPI Parity Error Flag
				UpdateINT_SPEF();
			}
			
			if(ExtendedMode() && !cmd_cycling_cnt)
			{
				if(--cmd_cycling_cnt == 0)
				{
					if(verbose)
					{
						logger << DebugInfo << sc_core::sc_time_stamp() << ": Command transfer completed" << EndDebugInfo;
					}
					dspi_sr.template Set<typename DSPI_SR::CMDTCF>(1); // Command Transfer Complete Flag
					UpdateINT_CMDTCF();
				}
			}
			
			if(!ExtendedMode() || !cmd_cycling_cnt)
			{
				bool eoq = DSPI_PUSHR::EOQ::template Get<uint32_t>(cmd << 16); // End Of Queue
				
				if(eoq)
				{
					if(verbose)
					{
						logger << DebugInfo << sc_core::sc_time_stamp() << ": End of queue" << EndDebugInfo;
					}
					dspi_sr.template Set<typename DSPI_SR::EOQF>(1); // End of Queue Flag
					UpdateINT_EOQF();
				}
			}
			
			UpdateState();
			
			RX_FIFO_Push(rxdata);
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::DSI_MasterTransfer()
{
	TransferControl ctrl;
	
	bool tsbc = dspi_dsicr0.template Get<typename DSPI_DSICR0::TSBC>();
	bool cont_scke = dspi_mcr.template Get<typename DSPI_MCR::CONT_SCKE>();
	
	ctrl.cont = dspi_dsicr0.template Get<typename DSPI_DSICR0::DCONT>();
	ctrl.ctas = dspi_dsicr0.template Get<typename DSPI_DSICR0::DSICTAS>();
	ctrl.pcs = dspi_dsicr0.template Get<typename DSPI_DSICR0::DPCSx>();
	ctrl.mcsc = false;
	ctrl.masc = false;
	ctrl.lsbfe = dspi_ctar[ctrl.ctas].template Get<typename DSPI_CTAR::LSBFE>();                // LSB First
	ctrl.pe = dspi_dsicr0.template Get<typename DSPI_DSICR0::PE>();
	bool pp = dspi_dsicr0.template Get<typename DSPI_DSICR0::PP>();
	ctrl.cpha = cont_scke || tsbc || dspi_ctar[ctrl.ctas].template Get<typename DSPI_CTAR::CPHA>();
	ctrl.frame_size = MasterFrameSize(DCONF_DSI, ctrl.ctas);
	
	uint32_t txdata = serialized_data;

	dspi_compr0 = txdata;
	
	uint32_t rxdata = 0xdeadbeef;
	bool parity_error = false;
	rx_parity_bit = pp; // initialize parity bit for reception
	tx_parity_bit = pp; // initialize parity bit for transmission
	
	MasterTransfer(rxdata, txdata, parity_error, ctrl);
	
	if(parity_error)
	{
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": DSI parity error" << EndDebugInfo;
		}
		dspi_sr.template Set<typename DSPI_SR::DPEF>(1); // DSI Parity Error Flag
		UpdateINT_DPEF();
	}
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": DSI transfer completed" << EndDebugInfo;
	}
	dspi_sr.template Set<typename DSPI_SR::DSITCF>(1); // DSI Frame Transfer Complete Flag
	UpdateINT_DSITCF();
	
	dspi_ddr0 = rxdata;
	
	CheckDeserializedData();
}

template <typename CONFIG>
void DSPI<CONFIG>::SlaveTransfer(uint32_t& rxdata, const uint32_t& txdata, bool& parity_error, const TransferControl& ctrl)
{
	bool pe = ctrl.pe;
	bool cpha = ctrl.cpha;
	unsigned int frame_size = ctrl.frame_size;
	
	bool lsbfe = ctrl.lsbfe;
	
	unisim::kernel::tlm2::tlm_serial_payload outcoming_payload;
	
	if(sin_bitstream.seek() == TLM_INPUT_BITSTREAM_NEED_SYNC) // synchronize SIN with SS
	{
		wait(sin_bitstream.event());
		sin_bitstream.latch();
	}
	
	if(cpha)
	{
		if(sin_bitstream.next() == TLM_INPUT_BITSTREAM_NEED_SYNC) 
		{
			wait(sin_bitstream.event());
			sin_bitstream.latch();
		}
	}

	unisim::kernel::tlm2::tlm_serial_payload::data_type& outcoming_data = outcoming_payload.get_data();
	
	uint32_t tx_shift_reg = txdata;
	uint32_t rx_shift_reg = 0;
	parity_error = false;
	
	sc_core::sc_time transmit_time = sc_core::SC_ZERO_TIME;
	sc_core::sc_time t = sc_core::SC_ZERO_TIME;
	
	unsigned int i;
	
	for(i = 0; i < frame_size; i++)
	{
		// shift
		if(lsbfe)
		{
			rx_shift_reg >>= 1;
		}
		else
		{
			rx_shift_reg <<= 1;
		}

		if(ss_bitstream.seek(t) == TLM_INPUT_BITSTREAM_NEED_SYNC)
		{
			wait(t);
			transmit_time -= t;
			t = sc_core::SC_ZERO_TIME;
			ss_bitstream.latch();
		}
		
		bool ss_value = ss_bitstream.read();
		bool sin_value = sin_bitstream.read();
		bool incoming_bit_value = !ss_value && sin_value;
		
		if(verbose)
		{
			logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": SS = '" << ss_value << "'" << EndDebugInfo;
			logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": SIN = '" << sin_value << "'" << EndDebugInfo;
		}
		
		if(outcoming_payload.get_period() == sc_core::SC_ZERO_TIME)
		{
			outcoming_payload.set_period(sin_bitstream.get_period());
		}
		else if(outcoming_payload.get_period() != sin_bitstream.get_period()) // clock period has changed
		{
			// flush output
			if(verbose)
			{
				logger << DebugInfo << (sc_core::sc_time_stamp() + transmit_time) << ": transmitting [" << outcoming_payload.get_data() << "] over SOUT with a period of " << outcoming_payload.get_period() << EndDebugInfo;
			}
			SOUT->nb_send(outcoming_payload, transmit_time);
			transmit_time = t;
			
			outcoming_payload.reset();
			outcoming_payload.set_period(sin_bitstream.get_period()); // update clock period
		}
		
		if(!pe || (i < (frame_size - 1)))
		{
			// transmit a data bit
			bool outcoming_bit_value = lsbfe ? (tx_shift_reg & 1) : ((tx_shift_reg >> (frame_size - 1)) & 1); // get data bit from shift register
			
			outcoming_data.push_back(outcoming_bit_value); // transmit data bit
		
			// shift
			if(lsbfe)
			{
				tx_shift_reg >>= 1;
			}
			else
			{
				tx_shift_reg <<= 1;
			}

			tx_parity_bit ^= outcoming_bit_value; // update computed parity bit
			
			// receive a data bit
			rx_shift_reg |= lsbfe ? (incoming_bit_value << (frame_size - 1)) : incoming_bit_value; // insert data bit in shift register
			
			rx_parity_bit ^= incoming_bit_value; // update computed parity bit
		}
		else
		{
			// transmit parity bit
			outcoming_data.push_back(tx_parity_bit);
			
			// receive parity bit
			if(rx_parity_bit != incoming_bit_value)
			{
				if(verbose)
				{
					logger << DebugInfo << (sc_core::sc_time_stamp() + t) << ": parity error" << EndDebugInfo;
				}
				
				parity_error = true;
			}
		}

		t += sin_bitstream.get_period();
		
		if(i != (frame_size - 1))
		{
			if(sin_bitstream.next() == TLM_INPUT_BITSTREAM_NEED_SYNC)
			{
				wait(sin_bitstream.event());
				transmit_time -= t;
				t = sc_core::SC_ZERO_TIME;
				sin_bitstream.latch();
				ss_bitstream.seek();
			}
		}
	}
	
	rxdata = rx_shift_reg;
	
	if(verbose)
	{
		logger << DebugInfo << (sc_core::sc_time_stamp() + transmit_time) << ": transmitting [" << outcoming_payload.get_data() << "] over SOUT with a period of " << outcoming_payload.get_period() << EndDebugInfo;
	}
	
	SOUT->nb_send(outcoming_payload, transmit_time);
	
	wait(t);
	
	if(verbose)
	{
		logger << DebugInfo << sc_core::sc_time_stamp() << ": transfer completed" << EndDebugInfo;
	}
	
	dspi_sr.template Set<typename DSPI_SR::TCF>(1); // Transfer Complete Flag
	UpdateINT_TCF();
}

template <typename CONFIG>
void DSPI<CONFIG>::SPI_SlaveTransfer()
{
	TransferControl ctrl;
	ctrl.ctas = SPI_CTAR_SLAVE_NUM;
	ctrl.lsbfe = false; // MSB First
	ctrl.pe = dspi_ctar_slave[ctrl.ctas].template Get<typename DSPI_CTAR_SLAVE::PE>();
	bool pp = dspi_ctar_slave[ctrl.ctas].template Get<typename DSPI_CTAR_SLAVE::PP>();
	ctrl.cpha = dspi_ctar_slave[ctrl.ctas].template Get<typename DSPI_CTAR_SLAVE::CPHA>();
	ctrl.frame_size = SlaveFrameSize(DCONF_SPI, ctrl.ctas);
	
	uint32_t txdata = 0xcacacaca;
	if(TX_FIFO_Empty())
	{
		// Transmit FIFO underflow
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": Transmit FIFO underflow" << EndDebugInfo;
			dspi_sr.ShortPrettyPrint(logger.DebugInfoStream());
			logger.DebugInfoStream() << std::endl;
		}
		dspi_sr.template Set<typename DSPI_SR::TFUF>(1);
		UpdateINT_TFUF();
	}
	else
	{
		txdata = SPI_Slave_TX_FIFO_Front();
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ":TX FIFO:Front() = 0x" << std::hex << txdata << std::dec << EndDebugInfo;
		}
		SPI_Slave_TX_FIFO_Pop();
	}
	
	uint32_t rxdata = 0xdeadbeef;
	bool parity_error = false;
	rx_parity_bit = pp; // initialize parity bit for reception
	tx_parity_bit = pp; // initialize parity bit for transmission
	
	SlaveTransfer(rxdata, txdata, parity_error, ctrl);

	// Increment SPI Transfer Counter
	dspi_tcr.template Set<typename DSPI_TCR::SPI_TCNT>(dspi_tcr.template Get<typename DSPI_TCR::SPI_TCNT>() + 1);

	dspi_sr.template Set<typename DSPI_SR::SPITCF>(1); // SPI Frame Transfer Complete Flag
	UpdateINT_SPITCF();
	
	if(parity_error)
	{
		dspi_sr.template Set<typename DSPI_SR::SPEF>(1); // SPI Parity Error Flag
		UpdateINT_SPEF();
	}

	RX_FIFO_Push(rxdata);
}

template <typename CONFIG>
void DSPI<CONFIG>::DSI_SlaveTransfer()
{
	TransferControl ctrl;
	
	ctrl.ctas = DSI_CTAR_SLAVE_NUM;
	ctrl.lsbfe = false;                // MSB First
	ctrl.pe = dspi_ctar_slave[ctrl.ctas].template Get<typename DSPI_CTAR_SLAVE::PE>();
	bool pp = dspi_ctar_slave[ctrl.ctas].template Get<typename DSPI_CTAR_SLAVE::PP>();
	ctrl.cpha = dspi_ctar_slave[ctrl.ctas].template Get<typename DSPI_CTAR_SLAVE::CPHA>();
	ctrl.frame_size = SlaveFrameSize(DCONF_SPI, ctrl.ctas);
	
	uint32_t txdata = serialized_data;
	
	dspi_compr0 = txdata;
	
	uint32_t rxdata = 0xdeadbeef;
	bool parity_error = false;
	rx_parity_bit = pp; // initialize parity bit for reception
	tx_parity_bit = pp; // initialize parity bit for transmission
	
	SlaveTransfer(rxdata, txdata, parity_error, ctrl);
	
	if(parity_error)
	{
		dspi_sr.template Set<typename DSPI_SR::DPEF>(1); // DSI Parity Error Flag
		UpdateINT_DPEF();
	}

	dspi_sr.template Set<typename DSPI_SR::DSITCF>(1); // DSI Frame Transfer Complete Flag
	UpdateINT_DSITCF();

	dspi_ddr0 = rxdata;
	
	CheckDeserializedData();
}

template <typename CONFIG>
void DSPI<CONFIG>::MasterTransferProcess()
{
	while(1)
	{
		if(!MasterMode() || !Running() || ModuleDisabled() || (!SPI_TransferTriggered() && !trigger_dsi_transfer))
		{
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": waiting for transfer trigger" << EndDebugInfo;
			}
			wait();
		}
		
		if(MasterMode() && Running() && !ModuleDisabled() && (SPI_TransferTriggered() || trigger_dsi_transfer))
		{
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": processing master transfer" << EndDebugInfo;
			}
			
			if(SelectConfiguration(curr_dconf))
			{
				switch(curr_dconf)
				{
					case DCONF_SPI:
						SPI_MasterTransfer();
						break;
					case DCONF_DSI:
						trigger_dsi_transfer = false;
						DSI_MasterTransfer();
						break;
					default:
						assert(false);
				}
				
				ScheduleTransfer();
			}
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::SlaveTransferProcess()
{
	while(1)
	{
		if(verbose)
		{
			logger << DebugInfo << sc_core::sc_time_stamp() << ": waiting for slave selection" << EndDebugInfo;
		}
		
		wait();
		ss_bitstream.latch();
		
		if(!ss_bitstream.read() /*ss_bitstream.negedge()*/ && SlaveMode() && Running())
		{
			if(verbose)
			{
				logger << DebugInfo << sc_core::sc_time_stamp() << ": slave selected for transmit/receive" << EndDebugInfo;
			}
			
			curr_dconf = Configuration();
			
			switch(curr_dconf)
			{
				case DCONF_SPI:
					if(!SPI_FrameTransmissionsStopped())
					{
						SPI_SlaveTransfer();
					}
					break;
					
				case DCONF_DSI:
					if(!DSI_OperationsStopped() && !DSI_FrameTransmissionsStopped())
					{
						DSI_SlaveTransfer();
					}
					break;
					
				default:
					break;
			}
		}
	}
}

template <typename CONFIG>
void DSPI<CONFIG>::DSI_INPUT_Process(unsigned int i)
{
	sc_core::sc_time notify_delay(sc_core::sc_time_stamp());
	unisim::kernel::tlm2::AlignToClock(notify_delay, master_clock_period, master_clock_start_time, master_clock_posedge_first, master_clock_duty_cycle);
	notify_delay -= sc_core::sc_time_stamp();
	dsi_input_sample_event[i].notify(notify_delay);
}

template <typename CONFIG>
void DSPI<CONFIG>::DSI_INPUT_SamplingProcess(unsigned int i)
{
	const sc_core::sc_in<bool>& dsi_input = DSI_INPUT[i];
	dspi_sdr0.Set(i, dsi_input.read());
	
	ScheduleLatchSerializedData();
}

template <typename CONFIG>
void DSPI<CONFIG>::ScheduleLatchSerializedData()
{
	sc_core::sc_time notify_delay(sc_core::sc_time_stamp());
	unisim::kernel::tlm2::AlignToClock(notify_delay, dspi_clock_period, dspi_clock_start_time, dspi_clock_posedge_first, dspi_clock_duty_cycle);
	notify_delay -= sc_core::sc_time_stamp();
	latch_serialized_data_event.notify(notify_delay);
}

template <typename CONFIG>
void DSPI<CONFIG>::LatchSerializedDataProcess()
{
	LatchSerializedData();
	CheckChangeInData();
}

} // end of namespace dspi
} // end of namespace mpc57xx
} // end of namespace freescale
} // end of namespace com
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_COM_FREESCALE_MPC57XX_DSPI_DSPI_TCC__
