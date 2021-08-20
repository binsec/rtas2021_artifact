/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_PXP_HH__
#define __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_PXP_HH__

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/passthrough_target_socket.h>
#include "unisim/kernel/kernel.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include <inttypes.h>
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/system_controller/sc.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/uart/pl011/pl011.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/ethernet/smsc_lan91c111.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/dual_timer/dt.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/vic/vic_stubs.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/sic/sic.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/watchdog/watchdog.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/dma/pl080/pl080.hh"
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/util/time_signal_splitter.hh"

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {

class PXP
	: public unisim::kernel::Object
	, public sc_module
{
public:
	typedef tlm::tlm_base_protocol_types::tlm_payload_type transaction_type;
	typedef tlm::tlm_base_protocol_types::tlm_phase_type   phase_type;
	typedef tlm::tlm_sync_enum                             sync_enum_type;

	/** Target socket for the cpu connection */
	tlm_utils::passthrough_target_socket<PXP, 32>
		cpu_target_socket;
	/** Target socket for the DMAC connection (1st) */
	tlm_utils::passthrough_target_socket<PXP, 32>
		dmac_master1_target_socket;
	/** Target socket for the DMAC connection (2nd) */
	tlm_utils::passthrough_target_socket<PXP, 32>
		dmac_master2_target_socket;

	/** Initiator socket for the SSMC Chip Select 0, NOR flash 2 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		ssmc0_init_socket;
	/** Initiator socket for the SSMC Chip Select 1, NOR flash 1 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		ssmc1_init_socket;
	/** Initiator socket for the MPMC Chip Select 0, SDRAM 128MB */
	tlm_utils::simple_initiator_socket<PXP, 32>
		mpmc0_init_socket;
	/** Initiator socket for the Ethernet Controller */
	tlm_utils::simple_initiator_socket<PXP, 32>
		eth_init_socket;
	/** Initiator socket for the System Controller */
	tlm_utils::simple_initiator_socket<PXP, 32>
		sc_init_socket;
	/** Initiator socket for the UART0 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		uart0_init_socket;
	/** Initiator socket for the UART1 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		uart1_init_socket;
	/** Initiator socket for the UART2 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		uart2_init_socket;
	/** Initiator socket for the Timer 1-2 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		dt1_init_socket;
	/** Initiator socket for the Timer 3-4 */
	tlm_utils::simple_initiator_socket<PXP, 32>
		dt2_init_socket;
	/** Initiator socket for the Watchdog */
	tlm_utils::simple_initiator_socket<PXP, 32>
		wd_init_socket;
	/** Initiator socket for the PIC (VIC, Vectored Interrupt Controller) */
	tlm_utils::simple_initiator_socket<PXP, 32>
		pic_init_socket;
	/** Initiator socket for the SIC (Secondry Interrupt Controller) */
	tlm_utils::simple_initiator_socket<PXP, 32>
		sic_init_socket;
	/** Initiator socket for the DMAC (PL080) */
	tlm_utils::simple_initiator_socket<PXP, 32>
		dmac_init_socket;

	/** The Ethernet Controller */
	unisim::component::tlm2::chipset::arm926ejs_pxp::ethernet::SMSC_LAN91C111 eth;
	/** The System Controller */
	unisim::component::tlm2::chipset::arm926ejs_pxp::system_controller::SystemController sc;
	/** The UART 0 */
	unisim::component::tlm2::chipset::arm926ejs_pxp::uart::pl011::PL011 uart0;
	/** The UART 1 */
	unisim::component::tlm2::chipset::arm926ejs_pxp::uart::pl011::PL011 uart1;
	/** The UART 2 */
	unisim::component::tlm2::chipset::arm926ejs_pxp::uart::pl011::PL011 uart2;
	/** Dual Timer for Timer 1 and 2 */
	unisim::component::tlm2::chipset::arm926ejs_pxp::dual_timer::DualTimer dt1;
	/** Dual Timer for Timer 3 and 4 */
	unisim::component::tlm2::chipset::arm926ejs_pxp::dual_timer::DualTimer dt2;
	/** Watchdog module */
	unisim::component::tlm2::chipset::arm926ejs_pxp::watchdog::Watchdog wd;
	/** VIC (Vectored Interrupt Controller) module */
	unisim::component::tlm2::chipset::arm926ejs_pxp::vic::VIC pic;
	/** SIC (Secondary Interrupt Controller) module */
	unisim::component::tlm2::chipset::arm926ejs_pxp::sic::SIC sic;
	/** DMAC (PL080) module */
	unisim::component::tlm2::chipset::arm926ejs_pxp::dma::pl080::PL080 dmac;

	/** Timers signal splitter */
	unisim::component::tlm2::chipset::arm926ejs_pxp::util::TSS tss;
	/** System controller to dual timers signals */
	sc_signal<uint64_t> sc_to_dt_signal;
	sc_signal<uint64_t> sc_to_dt1_signal[3];
	sc_signal<uint64_t> sc_to_dt2_signal[3];
	/** Watchdog to pic signal */
	sc_signal<bool> wdog_to_pic0_signal;
	/** Software to pic signal */
	sc_signal<bool> software_to_pic1_signal;
	/** commrx to pic signal */
	sc_signal<bool> commrx_to_pic2_signal;
	/** commtx to pic signal */
	sc_signal<bool> commtx_to_pic3_signal;
	/** Dual Timer 1 to pic signal */
	sc_signal<bool> timint12_to_pic4_signal;
	/** Dual Timer 2 to pic signal */
	sc_signal<bool> timint34_to_pic5_signal;
	/** GPIO 0 to pic signal */
	sc_signal<bool> gpio0_to_pic6_signal;
	/** GPIO 1 to pic signal */
	sc_signal<bool> gpio1_to_pic7_signal;
	/** GPIO 2 to pic signal */
	sc_signal<bool> gpio2_to_pic8_signal;
	/** GPIO 3 to pic signal */
	sc_signal<bool> gpio3_to_pic9_signal;
	/** RTC to pic signal */
	sc_signal<bool> rtc_to_pic10_signal;
	/** SSP to pic signal */
	sc_signal<bool> ssp_to_pic11_signal;
	/** UART0 to pic signal */
	sc_signal<bool> uart0_uartintr_to_pic12_signal;
	/** UART1 to pic signal */
	sc_signal<bool> uart1_uartintr_to_pic13_signal;
	/** UART2 to pic signal */
	sc_signal<bool> uart2_uartintr_to_pic14_signal;
	/** SCI0 to pic signal */
	sc_signal<bool> sci0_to_pic15_signal;
	/** CLCD to pic signal */
	sc_signal<bool> clcd_to_pic16_signal;
	/** DMA to pic signal */
	sc_signal<bool> dmacintr_to_pic17_signal;
	/** Power Fail to pic signal */
	sc_signal<bool> pwrfail_to_pic18_signal;
	/** MBX to pic signal */
	sc_signal<bool> mbx_to_pic19_signal;
	/** GND to pic signal */
	sc_signal<bool> gnd_to_pic20_signal;
	/** SIC 0 to pic signal */
	sc_signal<bool> sic0_to_pic21_signal;
	/** SIC 1 to pic signal */
	sc_signal<bool> sic1_to_pic22_signal;
	/** SIC 2 to pic signal */
	sc_signal<bool> sic2_to_pic23_signal;
	/** SIC 3 to pic signal */
	sc_signal<bool> sic3_to_pic24_signal;
	/** SIC 4 to pic signal */
	sc_signal<bool> sic4_to_pic25_signal;
	/** SIC 5 to pic signal */
	sc_signal<bool> sic5_to_pic26_signal;
	/** SIC 6 to pic signal */
	sc_signal<bool> sic6_to_pic27_signal;
	/** SIC 7 to pic signal */
	sc_signal<bool> sic7_to_pic28_signal;
	/** SIC 8 to pic signal */
	sc_signal<bool> sic8_to_pic29_signal;
	/** SIC 9 to pic signal */
	sc_signal<bool> sic9_to_pic30_signal;
	/** SIC 10 to pic signal */
	sc_signal<bool> sic10_to_pic31_signal;

	/* PIC stubs  START */
	vic::VICIntSourceStub
		wdog_int_stub, // pic0
		software_int_stub, // pic1
		commrx_int_stub, // pic2
		commtx_int_stub, // pic3
		gpio0_int_stub, // pic6
		gpio1_int_stub, // pic7
		gpio2_int_stub, // pic8
		gpio3_int_stub, // pic9
		rtc_int_stub, // pic10
		ssp_int_stub, // pic11
		sci0_int_stub, // pic15
		clcd_int_stub, // pic16
		pwrfail_int_stub, // pic18
		mbx_int_stub, // pic19
		gnd_int_stub; // pic20
	vic::VICIntSourceStub nvicfiqin_stub;
	sc_signal<bool> nvicfiqin_signal;
	vic::VICIntSourceStub nvicirqin_stub;
	sc_signal<bool> nvicirqin_signal;
	vic::VICAddrSourceStub vicvectaddrin_stub;
	sc_signal<uint32_t> vicvectaddrin_signal;
	
	sc_signal<bool> nvicfiq_signal;
	sc_signal<bool> nvicirq_signal;
	vic::VICAddrTargetStub vicvectaddrout_stub;
	sc_signal<uint32_t> vicvectaddrout_signal;

	/* PIC stubs    END */

	/** software stub to sic signal */
	sc_signal<bool> sic_software_to_sic0_signal;
	/** MMCI0B to sic signal */
	sc_signal<bool> mmci0b_to_sic1_signal;
	/** MMCI1B to sic signal */
	sc_signal<bool> mmci1b_to_sic2_signal;
	/** KMI0 to sic signal */
	sc_signal<bool> kmi0_to_sic3_signal;
	/** KMI1 to sic signal */
	sc_signal<bool> kmi1_to_sic4_signal;
	/** SCI1 to sic signal */
	sc_signal<bool> sci1_to_sic5_signal;
	/** UART3 to sic signal */
	sc_signal<bool> uart3_to_sic6_signal;
	/** CharLCD to sic signal */
	sc_signal<bool> charlcd_to_sic7_signal;
	/** Touchscreen pen to sic signal */
	sc_signal<bool> tspen_to_sic8_signal;
	/** Touchscreen keypad to sic signal */
	sc_signal<bool> tskeypad_to_sic9_signal;
	/** Reserved 0 stub to sic signal */
	sc_signal<bool> reserved0_to_sic10_signal;
	/** Reserved 1 stub to sic signal */
	sc_signal<bool> reserved1_to_sic11_signal;
	/** Reserved 2_stub to sic signal */
	sc_signal<bool> reserved2_to_sic12_signal;
	/** Reserved 3 stub to sic signal */
	sc_signal<bool> reserved3_to_sic13_signal;
	/** Reserved 4 stub to sic signal */
	sc_signal<bool> reserved4_to_sic14_signal;
	/** Reserved 5 stub to sic signal */
	sc_signal<bool> reserved5_to_sic15_signal;
	/** Reserved 6 stub to sic signal */
	sc_signal<bool> reserved6_to_sic16_signal;
	/** Reserved 7 stub to sic signal */
	sc_signal<bool> reserved7_to_sic17_signal;
	/** Reserved 8 stub to sic signal */
	sc_signal<bool> reserved8_to_sic18_signal;
	/** Reserved 9 stub to sic signal */
	sc_signal<bool> reserved9_to_sic19_signal;
	/** Reserved 10 stub to sic signal */
	sc_signal<bool> reserved10_to_sic20_signal;
	/** DiskOnChip to sic signal */
	sc_signal<bool> diskonchip_to_sic21_signal;
	/** MMCI0A to sic signal */
	sc_signal<bool> mmci0a_to_sic22_signal;
	/** MMCI1A to sic signal */
	sc_signal<bool> mmci1a_to_sic23_signal;
	/** AACI to sic signal */
	sc_signal<bool> aaci_to_sic24_signal;
	/** Ethernet to sic signal */
	sc_signal<bool> ethernet_to_sic25_signal;
	/** USB to sic signal */
	sc_signal<bool> usb_to_sic26_signal;
	/** PCI0 to sic signal */
	sc_signal<bool> pci0_to_sic27_signal;
	/** PCI1 to sic signal */
	sc_signal<bool> pci1_to_sic28_signal;
	/** PCI2 to sic signal */
	sc_signal<bool> pci2_to_sic29_signal;
	/** PCI3 to sic signal */
	sc_signal<bool> pci3_to_sic30_signal;
	/** RealViewLogicTile ZU206 to sic signal */
	sc_signal<bool> zu206_to_sic21_signal;
	/** RealViewLogicTile ZU207 to sic signal */
	sc_signal<bool> zu207_to_sic22_signal;
	/** RealViewLogicTile ZU208 to sic signal */
	sc_signal<bool> zu208_to_sic23_signal;
	/** RealViewLogicTile ZU209 to sic signal */
	sc_signal<bool> zu209_to_sic24_signal;
	/** RealViewLogicTile ZU210 to sic signal */
	sc_signal<bool> zu210_to_sic25_signal;
	/** RealViewLogicTile ZU211 to sic signal */
	sc_signal<bool> zu211_to_sic26_signal;
	/** RealViewLogicTile ZU212 to sic signal */
	sc_signal<bool> zu212_to_sic27_signal;
	/** RealViewLogicTile ZU213 to sic signal */
	sc_signal<bool> zu213_to_sic28_signal;
	/** RealViewLogicTile ZU214 to sic signal */
	sc_signal<bool> zu214_to_sic29_signal;
	/** RealViewLogicTile ZU215 to sic signal */
	sc_signal<bool> zu215_to_sic30_signal;
	/** RealViewLogicTile ZU216 to sic signal */
	sc_signal<bool> zu216_to_sic31_signal;

	/* SIC stubs  START */
	vic::VICIntSourceStub
		sic_software_int_stub, // sic0
		mmci0b_int_stub, // sic1
		mmci1b_int_stub, // sic2
		kmi0_int_stub, // sic3
		kmi1_int_stub, // sic4
		sci1_int_stub, // sic5
		uart3_int_stub, // sic6
		charlcd_int_stub, // sic7
		tspen_int_stub, // sic8
		tskeypad_int_stub, // sic9
		reserved0_int_stub, // sic10
		reserved1_int_stub, // sic11
		reserved2_int_stub, // sic12
		reserved3_int_stub, // sic13
		reserved4_int_stub, // sic14
		reserved5_int_stub, // sic15
		reserved6_int_stub, // sic16
		reserved7_int_stub, // sic17
		reserved8_int_stub, // sic18
		reserved9_int_stub, // sic19
		reserved10_int_stub, // sic20
		diskonchip_int_stub, // sic21
		mmci0a_int_stub, // sic22
		mmci1a_int_stub, // sic23
		aaci_int_stub, // sic24
		ethernet_int_stub, // sic25
		usb_int_stub, // sic26
		pci0_int_stub, // sic27
		pci1_int_stub, // sic28
		pci2_int_stub, // sic29
		pci3_int_stub; // sic30
	// RealView Logic Tile interrupts passthrough stubs
	vic:: VICIntSourceStub
		zu206_int_stub, // sic21
		zu207_int_stub, // sic22
		zu208_int_stub, // sic23
		zu209_int_stub, // sic24
		zu210_int_stub, // sic25
		zu211_int_stub, // sic26
		zu212_int_stub, // sic27
		zu213_int_stub, // sic28
		zu214_int_stub, // sic29
		zu215_int_stub, // sic30
		zu216_int_stub; // sic31
	/* SIC stubs    END */

	/** Timers unused interrupts connections */
	sc_in<bool> timint1_in_port; // for unused port dt1.timint1_out_port
	sc_in<bool> timint2_in_port; // for unused port dt1.timint2_out_port
	sc_signal<bool> timint1_signal, timint2_signal;
	sc_in<bool> timint3_in_port; // for unused port dt2.timint1_out_port
	sc_in<bool> timint4_in_port; // for unused port dt2.timint2_out_port
	sc_signal<bool> timint3_signal, timint4_signal;

	/** DMAC unused interrupt connections */
	sc_in<bool> dmacinterr_in_port;
	sc_in<bool> dmacinttc_in_port;
	sc_signal<bool> dmacinterr_signal, dmacinttc_signal;

	/** uart0 unused interrupt connections */
	sc_in<bool> uart0_uartrxintr_in_port;
	sc_in<bool> uart0_uarttxintr_in_port;
	sc_in<bool> uart0_uartrtintr_in_port;
	sc_in<bool> uart0_uartmsintr_in_port;
	sc_in<bool> uart0_uarteintr_in_port;
	sc_signal<bool> uart0_uartrxintr_signal,
		uart0_uarttxintr_signal,
		uart0_uartrtintr_signal,
		uart0_uartmsintr_signal,
		uart0_uarteintr_signal;

	/** uart1 unused interrupt connections */
	sc_in<bool> uart1_uartrxintr_in_port;
	sc_in<bool> uart1_uarttxintr_in_port;
	sc_in<bool> uart1_uartrtintr_in_port;
	sc_in<bool> uart1_uartmsintr_in_port;
	sc_in<bool> uart1_uarteintr_in_port;
	sc_signal<bool> uart1_uartrxintr_signal,
		uart1_uarttxintr_signal,
		uart1_uartrtintr_signal,
		uart1_uartmsintr_signal,
		uart1_uarteintr_signal;

	/** uart2 unused interrupt connections */
	sc_in<bool> uart2_uartrxintr_in_port;
	sc_in<bool> uart2_uarttxintr_in_port;
	sc_in<bool> uart2_uartrtintr_in_port;
	sc_in<bool> uart2_uartmsintr_in_port;
	sc_in<bool> uart2_uarteintr_in_port;
	sc_signal<bool> uart2_uartrxintr_signal,
		uart2_uarttxintr_signal,
		uart2_uartrtintr_signal,
		uart2_uartmsintr_signal,
		uart2_uarteintr_signal;

	SC_HAS_PROCESS(PXP);
	PXP(const sc_module_name &name, Object *parent = 0);
	~PXP();

	virtual bool BeginSetup();

private:

	/** Blocking access to the system registers.
	 *
	 * @param trans the transaction
	 * @param delay current delta time
	 */
	void AccessSystemRegisters(transaction_type &trans,
			sc_core::sc_time &delay);
	/** Access to the 24MHz counter register.
	 *
	 * @param trans the transaction
	 * @param delay current delta time
	 */
	void Access24MHzCounter(transaction_type & trans,
			sc_core::sc_time &delay);

	/**************************************************************************/
	/* Virtual methods for the target socket for the cpu connection     START */
	/**************************************************************************/

	sync_enum_type cpu_target_nb_transport_fw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void cpu_target_b_transport(transaction_type &trans, 
			sc_core::sc_time &delay);
	bool cpu_target_get_direct_mem_ptr(transaction_type &trans, 
			tlm::tlm_dmi &dmi_data);
	unsigned int cpu_target_transport_dbg(transaction_type &trans);

	/**************************************************************************/
	/* Virtual methods for the target socket for the cpu connection       END */
	/**************************************************************************/
	
	/**************************************************************************/
	/* Virtual methods for the target sockets of the dmac connections   START */
	/**************************************************************************/

	sync_enum_type dmac_master1_target_nb_transport_fw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void dmac_master1_target_b_transport(transaction_type &trans, 
			sc_core::sc_time &delay);
	bool dmac_master1_target_get_direct_mem_ptr(transaction_type &trans, 
			tlm::tlm_dmi &dmi_data);
	unsigned int dmac_master1_target_transport_dbg(transaction_type &trans);

	sync_enum_type dmac_master2_target_nb_transport_fw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void dmac_master2_target_b_transport(transaction_type &trans, 
			sc_core::sc_time &delay);
	bool dmac_master2_target_get_direct_mem_ptr(transaction_type &trans, 
			tlm::tlm_dmi &dmi_data);
	unsigned int dmac_master2_target_transport_dbg(transaction_type &trans);

	/**************************************************************************/
	/* Virtual methods for the target sockets of the dmac connections     END */
	/**************************************************************************/
	
	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the SSMC Chip Select 0                                               */
	/**************************************************************************/

	sync_enum_type ssmc0_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void ssmc0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the SSMC Chip Select 0                                           END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the MPMC Chip Select 0                                               */
	/**************************************************************************/

	sync_enum_type mpmc0_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void mpmc0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the MPMC Chip Select 0                                           END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the Ethernet Controller                                              */
	/**************************************************************************/

	sync_enum_type eth_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void eth_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the Ethenet Controller                                           END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the System Controller                                                */
	/**************************************************************************/

	sync_enum_type sc_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void sc_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the System Controller                                            END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the UART 0                                                           */
	/**************************************************************************/

	sync_enum_type uart0_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void uart0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the UART 0                                                       END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the UART 1                                                           */
	/**************************************************************************/

	sync_enum_type uart1_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void uart1_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the UART 1                                                       END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the UART 2                                                           */
	/**************************************************************************/

	sync_enum_type uart2_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void uart2_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the UART 2                                                       END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the Dual Timers                                                      */
	/**************************************************************************/

	sync_enum_type dt1_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void dt1_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);
	sync_enum_type dt2_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void dt2_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the Dual Timers                                                  END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the Watchdog                                                         */
	/**************************************************************************/

	sync_enum_type wd_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void wd_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the Watchdog                                                     END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the PIC                                                              */
	/**************************************************************************/

	sync_enum_type pic_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void pic_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the PIC                                                          END */
	/**************************************************************************/

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the SIC                                                              */
	/**************************************************************************/

	sync_enum_type sic_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void sic_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the SIC                                                          END */
	/**************************************************************************/
	
	/**************************************************************************/
	/* Virtual methods for the initiator socket for                     START */
	/*   the DMAC                                                             */
	/**************************************************************************/

	sync_enum_type dmac_init_nb_transport_bw(transaction_type &trans,
			phase_type &phase,
			sc_core::sc_time &time);
	void dmac_init_invalidate_direct_mem_ptr(sc_dt::uint64,
			sc_dt::uint64);

	/**************************************************************************/
	/* Virtual methods for the initiator socket for                           */
	/*   the DMAC                                                         END */
	/**************************************************************************/

	/** Interface to the UNISIM logger */
	unisim::kernel::logger::Logger logger;
};

} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_CHIPSET_ARM926EJS_PXP_PXP_HH__

