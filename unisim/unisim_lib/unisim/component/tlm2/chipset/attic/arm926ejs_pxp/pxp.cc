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
 
#include "unisim/component/tlm2/chipset/arm926ejs_pxp/pxp.hh"
#include "unisim/kernel/tlm2/tlm.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/util/endian/endian.hh"
#include <inttypes.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace chipset {
namespace arm926ejs_pxp {

using unisim::util::endian::Host2LittleEndian;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

PXP ::
PXP(const sc_module_name &name, Object *parent)
	: unisim::kernel::Object(name, parent)
	, sc_module(name)
	, cpu_target_socket("cpu-target-socket")
	, dmac_master1_target_socket("dmac_master1_target_socket")
	, dmac_master2_target_socket("dmac_master2_target_socket")
	, ssmc0_init_socket("ssmc0-init-socket")
	, mpmc0_init_socket("mpmc0-init-socket")
	, eth_init_socket("eth-init-socket")
	, sc_init_socket("sc-init-socket")
	, wd_init_socket("wd-init-socket")
	, pic_init_socket("pic-init-socket")
	, sic_init_socket("sic-init-socket")
	, dmac_init_socket("dmac-init-socket")
	, eth("ethernet", this)
	, sc("system-controller", this)
	, uart0("uart0", this)
	, uart1("uart1", this)
	, uart2("uart2", this)
	, dt1("timer1-2", this)
	, dt2("timer3-4", this)
	, wd("watchdog", this)
	, pic("pic", this)
	, sic("sic", this)
	, dmac("dmac", this)
	, tss("time-signal-splitter")
	, sc_to_dt_signal("sc_to_dt_signal")
	, wdog_to_pic0_signal("wdog_to_pic0_signal")
	, software_to_pic1_signal("software_to_pic1_signal")
	, commrx_to_pic2_signal("commrx_to_pic2_signal")
	, commtx_to_pic3_signal("commtx_to_pic3_signal")
	, timint12_to_pic4_signal("timint12_to_pic4_signal")
	, timint34_to_pic5_signal("timint34_to_pic5_signal")
	, gpio0_to_pic6_signal("gpio0_to_pic6_signal")
	, gpio1_to_pic7_signal("gpio1_to_pic7_signal")
	, gpio2_to_pic8_signal("gpio2_to_pic8_signal")
	, gpio3_to_pic9_signal("gpio3_to_pic9_signal")
	, rtc_to_pic10_signal("rtc_to_pic10_signal")
	, ssp_to_pic11_signal("ssp_to_pic11_signal")
	, uart0_uartintr_to_pic12_signal("uart0_uartintr_to_pic12_signal")
	, uart1_uartintr_to_pic13_signal("uart1_uartintr_to_pic13_signal")
	, uart2_uartintr_to_pic14_signal("uart2_uartintr_to_pic14_signal")
	, sci0_to_pic15_signal("sci0_to_pic15_signal")
	, clcd_to_pic16_signal("clcd_to_pic16_signal")
	, dmacintr_to_pic17_signal("dmacintr_to_pic17_signal")
	, pwrfail_to_pic18_signal("pwrfail_to_pic18_signal")
	, mbx_to_pic19_signal("mbx_to_pic19_signal")
	, gnd_to_pic20_signal("gnd_to_pic20_signal")
	, sic0_to_pic21_signal("sic0_to_pic21_signal")
	, sic1_to_pic22_signal("sic1_to_pic22_signal")
	, sic2_to_pic23_signal("sic2_to_pic23_signal")
	, sic3_to_pic24_signal("sic3_to_pic24_signal")
	, sic4_to_pic25_signal("sic4_to_pic25_signal")
	, sic5_to_pic26_signal("sic5_to_pic26_signal")
	, sic6_to_pic27_signal("sic6_to_pic27_signal")
	, sic7_to_pic28_signal("sic7_to_pic28_signal")
	, sic8_to_pic29_signal("sic8_to_pic29_signal")
	, sic9_to_pic30_signal("sic9_to_pic30_signal")
	, sic10_to_pic31_signal("sic10_to_pic31_signal")
	, wdog_int_stub("wdog_int_stub", this, false)
	, software_int_stub("software_int_stub", this, false)
	, commrx_int_stub("commrx_int_stub", this, false)
	, commtx_int_stub("commtx_int_stub", this, false)
	, gpio0_int_stub("gpio0_int_stub", this, false)
	, gpio1_int_stub("gpio1_int_stub", this, false)
	, gpio2_int_stub("gpio2_int_stub", this, false)
	, gpio3_int_stub("gpio3_int_stub", this, false)
	, rtc_int_stub("rtc_int_stub", this, false)
	, ssp_int_stub("ssp_int_stub", this, false)
	, sci0_int_stub("sci0_int_stub", this, false)
	, clcd_int_stub("clcd_int_stub", this, false)
	, pwrfail_int_stub("pwrfail_int_stub", this, false)
	, mbx_int_stub("mbx_int_stub", this, false)
	, gnd_int_stub("gnd_int_stub", this, false)
	, nvicfiqin_stub("nvicfiqin_stub", this, true)
	, nvicfiqin_signal("nvicfiqin_signal")
	, nvicirqin_stub("nvicirqin_stub", this, true)
	, nvicirqin_signal("nvicirqin_signal")
	, vicvectaddrin_stub("vicvectaddrin_stub", this)
	, vicvectaddrin_signal("vicvectaddrin_signal")
	, nvicfiq_signal("nvicfiq_signal")
	, nvicirq_signal("nvicirq_signal")
	, vicvectaddrout_stub("vicvectaddrout_stub", this)
	, vicvectaddrout_signal("vicvectaddrout_signal")
	, sic_software_to_sic0_signal("sic_software_to_sic0_signal")
	, mmci0b_to_sic1_signal("mmci0b_to_sic1_signal")
	, mmci1b_to_sic2_signal("mmci1b_to_sic2_signal")
	, kmi0_to_sic3_signal("kmi0_to_sic3_signal")
	, kmi1_to_sic4_signal("kmi1_to_sic4_signal")
	, sci1_to_sic5_signal("sci1_to_sic5_signal")
	, uart3_to_sic6_signal("uart3_to_sic6_signal")
	, charlcd_to_sic7_signal("charlcd_to_sic7_signal")
	, tspen_to_sic8_signal("tspen_to_sic8_signal")
	, tskeypad_to_sic9_signal("tskeypad_to_sic9_signal")
	, reserved0_to_sic10_signal("reserved0_to_sic10_signal")
	, reserved1_to_sic11_signal("reserved1_to_sic11_signal")
	, reserved2_to_sic12_signal("reserved2_to_sic12_signal")
	, reserved3_to_sic13_signal("reserved3_to_sic13_signal")
	, reserved4_to_sic14_signal("reserved4_to_sic14_signal")
	, reserved5_to_sic15_signal("reserved5_to_sic15_signal")
	, reserved6_to_sic16_signal("reserved6_to_sic16_signal")
	, reserved7_to_sic17_signal("reserved7_to_sic17_signal")
	, reserved8_to_sic18_signal("reserved8_to_sic18_signal")
	, reserved9_to_sic19_signal("reserved9_to_sic19_signal")
	, reserved10_to_sic20_signal("reserved10_to_sic20_signal")
	, diskonchip_to_sic21_signal("diskonchip_to_sic21_signal")
	, mmci0a_to_sic22_signal("mmci0a_to_sic22_signal")
	, mmci1a_to_sic23_signal("mmci1a_to_sic23_signal")
	, aaci_to_sic24_signal("aaci_to_sic24_signal")
	, ethernet_to_sic25_signal("ethernet_to_sic25_signal")
	, usb_to_sic26_signal("usb_to_sic26_signal")
	, pci0_to_sic27_signal("pci0_to_sic27_signal")
	, pci1_to_sic28_signal("pci1_to_sic28_signal")
	, pci2_to_sic29_signal("pci2_to_sic29_signal")
	, pci3_to_sic30_signal("pci3_to_sic30_signal")
	, zu206_to_sic21_signal("zu206_to_sic21_signal")
	, zu207_to_sic22_signal("zu207_to_sic22_signal")
	, zu208_to_sic23_signal("zu208_to_sic23_signal")
	, zu209_to_sic24_signal("zu209_to_sic24_signal")
	, zu210_to_sic25_signal("zu210_to_sic25_signal")
	, zu211_to_sic26_signal("zu211_to_sic26_signal")
	, zu212_to_sic27_signal("zu212_to_sic27_signal")
	, zu213_to_sic28_signal("zu213_to_sic28_signal")
	, zu214_to_sic29_signal("zu214_to_sic29_signal")
	, zu215_to_sic30_signal("zu215_to_sic30_signal")
	, zu216_to_sic31_signal("zu216_to_sic31_signal")
	, sic_software_int_stub("sic_software_int_stub", this, false)
	, mmci0b_int_stub("mmci0b_int_stub", this, false)
	, mmci1b_int_stub("mmci1b_int_stub", this, false)
	, kmi0_int_stub("kmi0_int_stub", this, false)
	, kmi1_int_stub("kmi1_int_stub", this, false)
	, sci1_int_stub("sci1_int_stub", this, false)
	, uart3_int_stub("uart3_int_stub", this, false)
	, charlcd_int_stub("charlcd_int_stub", this, false)
	, tspen_int_stub("tspen_int_stub", this, false)
	, tskeypad_int_stub("tskeypad_int_stub", this, false)
	, reserved0_int_stub("reserved0_int_stub", this, false)
	, reserved1_int_stub("reserved1_int_stub", this, false)
	, reserved2_int_stub("reserved2_int_stub", this, false)
	, reserved3_int_stub("reserved3_int_stub", this, false)
	, reserved4_int_stub("reserved4_int_stub", this, false)
	, reserved5_int_stub("reserved5_int_stub", this, false)
	, reserved6_int_stub("reserved6_int_stub", this, false)
	, reserved7_int_stub("reserved7_int_stub", this, false)
	, reserved8_int_stub("reserved8_int_stub", this, false)
	, reserved9_int_stub("reserved9_int_stub", this, false)
	, reserved10_int_stub("reserved10_int_stub", this, false)
	, diskonchip_int_stub("diskonchip_int_stub", this, false)
	, mmci0a_int_stub("mmci0a_int_stub", this, false)
	, mmci1a_int_stub("mmci1a_int_stub", this, false)
	, aaci_int_stub("aaci_int_stub", this, false)
	, ethernet_int_stub("ethernet_int_stub", this, false)
	, usb_int_stub("usb_int_stub", this, false)
	, pci0_int_stub("pci0_int_stub", this, false)
	, pci1_int_stub("pci1_int_stub", this, false)
	, pci2_int_stub("pci2_int_stub", this, false)
	, pci3_int_stub("pci3_int_stub", this, false)
	, zu206_int_stub("zu206_int_stub", this, false)
	, zu207_int_stub("zu207_int_stub", this, false)
	, zu208_int_stub("zu208_int_stub", this, false)
	, zu209_int_stub("zu209_int_stub", this, false)
	, zu210_int_stub("zu210_int_stub", this, false)
	, zu211_int_stub("zu211_int_stub", this, false)
	, zu212_int_stub("zu212_int_stub", this, false)
	, zu213_int_stub("zu213_int_stub", this, false)
	, zu214_int_stub("zu214_int_stub", this, false)
	, zu215_int_stub("zu215_int_stub", this, false)
	, zu216_int_stub("zu216_int_stub", this, false)
	, timint1_in_port("timint1_in_port")
	, timint2_in_port("timint2_in_port")
	, timint1_signal("timint1_signal")
	, timint2_signal("timint2_signal")
	, timint3_in_port("timint3_in_port")
	, timint4_in_port("timint4_in_port")
	, timint3_signal("timint3_signal")
	, timint4_signal("timint4_signal")
	, dmacinterr_in_port("dmacinterr_in_port")
	, dmacinttc_in_port("dmacinttc_in_port")
	, dmacinterr_signal("dmacinterr_signal")
	, dmacinttc_signal("dmacinttc_signal")
	, uart0_uartrxintr_in_port("uart0_uartrxintr_in_port")
	, uart0_uarttxintr_in_port("uart0_uarttxintr_in_port")
	, uart0_uartrtintr_in_port("uart0_uartrtintr_in_port")
	, uart0_uartmsintr_in_port("uart0_uartmsintr_in_port")
	, uart0_uarteintr_in_port("uart0_uarteintr_in_port")
	, uart0_uartrxintr_signal("uart0_uartrxintr_signal")
	, uart0_uarttxintr_signal("uart0_uarttxintr_signal")
	, uart0_uartrtintr_signal("uart0_uartrtintr_signal")
	, uart0_uartmsintr_signal("uart0_uartmsintr_signal")
	, uart0_uarteintr_signal("uart0_uarteintr_signal")
	, uart1_uartrxintr_in_port("uart1_uartrxintr_in_port")
	, uart1_uarttxintr_in_port("uart1_uarttxintr_in_port")
	, uart1_uartrtintr_in_port("uart1_uartrtintr_in_port")
	, uart1_uartmsintr_in_port("uart1_uartmsintr_in_port")
	, uart1_uarteintr_in_port("uart1_uarteintr_in_port")
	, uart1_uartrxintr_signal("uart1_uartrxintr_signal")
	, uart1_uarttxintr_signal("uart1_uarttxintr_signal")
	, uart1_uartrtintr_signal("uart1_uartrtintr_signal")
	, uart1_uartmsintr_signal("uart1_uartmsintr_signal")
	, uart1_uarteintr_signal("uart1_uarteintr_signal")
	, uart2_uartrxintr_in_port("uart2_uartrxintr_in_port")
	, uart2_uarttxintr_in_port("uart2_uarttxintr_in_port")
	, uart2_uartrtintr_in_port("uart2_uartrtintr_in_port")
	, uart2_uartmsintr_in_port("uart2_uartmsintr_in_port")
	, uart2_uarteintr_in_port("uart2_uarteintr_in_port")
	, uart2_uartrxintr_signal("uart2_uartrxintr_signal")
	, uart2_uarttxintr_signal("uart2_uarttxintr_signal")
	, uart2_uartrtintr_signal("uart2_uartrtintr_signal")
	, uart2_uartmsintr_signal("uart2_uartmsintr_signal")
	, uart2_uarteintr_signal("uart2_uarteintr_signal")
	, logger(*this)
{

	sic["base-addr"]       = 0x10003000UL;
	eth["base-addr"]       = 0x10010000UL;
	pic["base-addr"]       = 0x10140000UL;
	sc["base-addr"]        = 0x101e0000UL;
	dt1["base-addr"]       = 0x101e2000UL;
	dt2["base-addr"]       = 0x101e3000UL;
	uart0["base-addr"]     = 0x101f1000UL;
	uart1["base-addr"]     = 0x101f2000UL;
	uart2["base-addr"]     = 0x101f3000UL;
	dmac["base-addr"]      = 0x10130000UL;

	cpu_target_socket.register_nb_transport_fw(this,
			&PXP::cpu_target_nb_transport_fw);
	cpu_target_socket.register_b_transport(this,
			&PXP::cpu_target_b_transport);
	cpu_target_socket.register_get_direct_mem_ptr(this,
			&PXP::cpu_target_get_direct_mem_ptr);
	cpu_target_socket.register_transport_dbg(this,
			&PXP::cpu_target_transport_dbg);

	dmac_master1_target_socket.register_nb_transport_fw(this,
			&PXP::dmac_master1_target_nb_transport_fw);
	dmac_master1_target_socket.register_b_transport(this,
			&PXP::dmac_master1_target_b_transport);
	dmac_master1_target_socket.register_get_direct_mem_ptr(this,
			&PXP::dmac_master1_target_get_direct_mem_ptr);
	dmac_master1_target_socket.register_transport_dbg(this,
			&PXP::dmac_master1_target_transport_dbg);

	dmac_master2_target_socket.register_nb_transport_fw(this,
			&PXP::dmac_master2_target_nb_transport_fw);
	dmac_master2_target_socket.register_b_transport(this,
			&PXP::dmac_master2_target_b_transport);
	dmac_master2_target_socket.register_get_direct_mem_ptr(this,
			&PXP::dmac_master2_target_get_direct_mem_ptr);
	dmac_master2_target_socket.register_transport_dbg(this,
			&PXP::dmac_master2_target_transport_dbg);

	ssmc0_init_socket.register_nb_transport_bw(this,
			&PXP::ssmc0_init_nb_transport_bw);
	ssmc0_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::ssmc0_init_invalidate_direct_mem_ptr);

	mpmc0_init_socket.register_nb_transport_bw(this,
			&PXP::mpmc0_init_nb_transport_bw);
	mpmc0_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::mpmc0_init_invalidate_direct_mem_ptr);

	eth_init_socket.register_nb_transport_bw(this,
			&PXP::eth_init_nb_transport_bw);
	eth_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::eth_init_invalidate_direct_mem_ptr);

	sc_init_socket.register_nb_transport_bw(this,
			&PXP::sc_init_nb_transport_bw);
	sc_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::sc_init_invalidate_direct_mem_ptr);

	uart0_init_socket.register_nb_transport_bw(this,
			&PXP::uart0_init_nb_transport_bw);
	uart0_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::uart0_init_invalidate_direct_mem_ptr);

	uart1_init_socket.register_nb_transport_bw(this,
			&PXP::uart1_init_nb_transport_bw);
	uart1_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::uart1_init_invalidate_direct_mem_ptr);

	uart2_init_socket.register_nb_transport_bw(this,
			&PXP::uart2_init_nb_transport_bw);
	uart2_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::uart2_init_invalidate_direct_mem_ptr);

	wd_init_socket.register_nb_transport_bw(this,
			&PXP::wd_init_nb_transport_bw);
	wd_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::wd_init_invalidate_direct_mem_ptr);

	dt1_init_socket.register_nb_transport_bw(this,
			&PXP::dt1_init_nb_transport_bw);
	dt1_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::dt1_init_invalidate_direct_mem_ptr);

	dt2_init_socket.register_nb_transport_bw(this,
			&PXP::dt2_init_nb_transport_bw);
	dt2_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::dt2_init_invalidate_direct_mem_ptr);

	pic_init_socket.register_nb_transport_bw(this,
			&PXP::pic_init_nb_transport_bw);
	pic_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::pic_init_invalidate_direct_mem_ptr);

	sic_init_socket.register_nb_transport_bw(this,
			&PXP::sic_init_nb_transport_bw);
	sic_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::sic_init_invalidate_direct_mem_ptr);

	dmac_init_socket.register_nb_transport_bw(this,
			&PXP::dmac_init_nb_transport_bw);
	dmac_init_socket.register_invalidate_direct_mem_ptr(this,
			&PXP::dmac_init_invalidate_direct_mem_ptr);

	sc.refclk_out_port(sc_to_dt_signal);
	tss.sc_ref_in_port(sc_to_dt_signal);
	
	tss.dt1_timclk_out_port(sc_to_dt1_signal[0]);
	dt1.timclk_in_port(sc_to_dt1_signal[0]);
	sc.timclken0_out_port(sc_to_dt1_signal[1]);
	dt1.timclken1_in_port(sc_to_dt1_signal[1]);
	sc.timclken1_out_port(sc_to_dt1_signal[2]);
	dt1.timclken2_in_port(sc_to_dt1_signal[2]);
	// dt1 interruptions usage:
	// - timint1 (unused) -> attached to stub timint1_in_port
	// - timint2 (unused) -> attached to stub timint2_in_port
	// - timintc -> attached to pic.vicintsource[4]
	dt1.timint1_out_port(timint1_signal);
	timint1_in_port(timint1_signal);
	dt1.timint2_out_port(timint2_signal);
	timint2_in_port(timint2_signal);
	dt1.timintc_out_port(timint12_to_pic4_signal);

	tss.dt2_timclk_out_port(sc_to_dt2_signal[0]);
	dt2.timclk_in_port(sc_to_dt2_signal[0]);
	sc.timclken2_out_port(sc_to_dt2_signal[1]);
	dt2.timclken1_in_port(sc_to_dt2_signal[1]);
	sc.timclken3_out_port(sc_to_dt2_signal[2]);
	dt2.timclken2_in_port(sc_to_dt2_signal[2]);
	// dt2 interruptions usage:
	// - timint1 (unused) -> attached to stub timint3_in_port
	// - timint2 (unused) -> attached to stub timint4_in_port
	// - timintc -> attached to pic.vicintsource[5]
	dt2.timint1_out_port(timint3_signal);
	timint3_in_port(timint3_signal);
	dt2.timint2_out_port(timint4_signal);
	timint4_in_port(timint4_signal);
	dt2.timintc_out_port(timint34_to_pic5_signal);

	// dmac interrupts usage:
	// - dmacintr -> attached to pic.vicintsource[17]
	// - dmacinterr (unused) -> attached to stub dmacinterr_in_port
	// - dmacinttc (unused) -> attached to stub dmacinttc_in_pot
	dmac.dmacintr(dmacintr_to_pic17_signal);
	dmacinterr_in_port(dmacinterr_signal);
	dmac.dmacinterr(dmacinterr_signal);
	dmacinttc_in_port(dmacinttc_signal);
	dmac.dmacinttc(dmacinttc_signal);

	// uart0 interrupts usage:
	// - uartrxintr (unused) -> attached to stub uart0_uartrxintr_in_port
	// - uarttxintr (unused) -> attached to stub uart0_uarttxintr_in_port
	// - uartrtintr (unused) -> attached to stub uart0_uartrtintr_in_port
	// - uartmsintr (unused) -> attached to stub uart0_uartmsintr_in_port
	// - uarteintr (unused) -> attached to stub uart0_uarteintr_in_port
	// - uartintr -> attached to pic.vicintsource[12]
	uart0.uartintr(uart0_uartintr_to_pic12_signal);
	uart0.uartrxintr(uart0_uartrxintr_signal);
	uart0_uartrxintr_in_port(uart0_uartrxintr_signal);
	uart0.uarttxintr(uart0_uarttxintr_signal);
	uart0_uarttxintr_in_port(uart0_uarttxintr_signal);
	uart0.uartrtintr(uart0_uartrtintr_signal);
	uart0_uartrtintr_in_port(uart0_uartrtintr_signal);
	uart0.uartmsintr(uart0_uartmsintr_signal);
	uart0_uartmsintr_in_port(uart0_uartmsintr_signal);
	uart0.uarteintr(uart0_uarteintr_signal);
	uart0_uarteintr_in_port(uart0_uarteintr_signal);

	// uart1 interrupts usage:
	// - uartrxintr (unused) -> attached to stub uart1_uartrxintr_in_port
	// - uarttxintr (unused) -> attached to stub uart1_uarttxintr_in_port
	// - uartrtintr (unused) -> attached to stub uart1_uartrtintr_in_port
	// - uartmsintr (unused) -> attached to stub uart1_uartmsintr_in_port
	// - uarteintr (unused) -> attached to stub uart1_uarteintr_in_port
	// - uartintr -> attached to pic.vicintsource[13]
	uart1.uartintr(uart1_uartintr_to_pic13_signal);
	uart1.uartrxintr(uart1_uartrxintr_signal);
	uart1_uartrxintr_in_port(uart1_uartrxintr_signal);
	uart1.uarttxintr(uart1_uarttxintr_signal);
	uart1_uarttxintr_in_port(uart1_uarttxintr_signal);
	uart1.uartrtintr(uart1_uartrtintr_signal);
	uart1_uartrtintr_in_port(uart1_uartrtintr_signal);
	uart1.uartmsintr(uart1_uartmsintr_signal);
	uart1_uartmsintr_in_port(uart1_uartmsintr_signal);
	uart1.uarteintr(uart1_uarteintr_signal);
	uart1_uarteintr_in_port(uart1_uarteintr_signal);

	// uart2 interrupts usage:
	// - uartrxintr (unused) -> attached to stub uart2_uartrxintr_in_port
	// - uarttxintr (unused) -> attached to stub uart2_uarttxintr_in_port
	// - uartrtintr (unused) -> attached to stub uart2_uartrtintr_in_port
	// - uartmsintr (unused) -> attached to stub uart2_uartmsintr_in_port
	// - uarteintr (unused) -> attached to stub uart2_uarteintr_in_port
	// - uartintr -> attached to pic.vicintsource[14]
	uart2.uartintr(uart2_uartintr_to_pic14_signal);
	uart2.uartrxintr(uart2_uartrxintr_signal);
	uart2_uartrxintr_in_port(uart2_uartrxintr_signal);
	uart2.uarttxintr(uart2_uarttxintr_signal);
	uart2_uarttxintr_in_port(uart2_uarttxintr_signal);
	uart2.uartrtintr(uart2_uartrtintr_signal);
	uart2_uartrtintr_in_port(uart2_uartrtintr_signal);
	uart2.uartmsintr(uart2_uartmsintr_signal);
	uart2_uartmsintr_in_port(uart2_uartmsintr_signal);
	uart2.uarteintr(uart2_uarteintr_signal);
	uart2_uarteintr_in_port(uart2_uarteintr_signal);

	/* PIC connections START */
	// connection list (so do not need stubs)
	// - 4 (dt1.timintc through timint12_to_pic4_signal)
	// - 5 (dt2.timintc through timint34_to_pic5_signal)
	// - 12 (uart0.uartintr through uart0_uartintr_to_pic12_signal)
	// - 13 (uart1.uartintr through uart1_uartintr_to_pic13_signal)
	// - 14 (uart2.uartintr through uart2_uartintr_to_pic13_signal)
	// - 17 (dmac.dmacintr through dmacintr_to_pic17_signal)
	// - 21 (sic.sicinttarget[0] through sic0_to_pic21_signal)
	// - 22 (sic.sicinttarget[1] through sic1_to_pic22_signal)
	// - 23 (sic.sicinttarget[2] through sic2_to_pic23_signal)
	// - 24 (sic.sicinttarget[3] through sic3_to_pic24_signal)
	// - 25 (sic.sicinttarget[4] through sic4_to_pic25_signal)
	// - 26 (sic.sicinttarget[5] through sic5_to_pic26_signal)
	// - 27 (sic.sicinttarget[6] through sic6_to_pic27_signal)
	// - 28 (sic.sicinttarget[7] through sic7_to_pic28_signal)
	// - 29 (sic.sicinttarget[8] through sic8_to_pic29_signal)
	// - 30 (sic.sicinttarget[9] through sic9_to_pic30_signal)
	// - 31 (sic.sicinttarget[10] through sic10_to_pic31_signal)
	(*pic.vicintsource[4])(timint12_to_pic4_signal);
	(*pic.vicintsource[5])(timint34_to_pic5_signal);
	(*pic.vicintsource[12])(uart0_uartintr_to_pic12_signal);
	(*pic.vicintsource[13])(uart1_uartintr_to_pic13_signal);
	(*pic.vicintsource[14])(uart2_uartintr_to_pic14_signal);
	(*pic.vicintsource[17])(dmacintr_to_pic17_signal);
	(*pic.vicintsource[21])(sic0_to_pic21_signal);
	(*pic.vicintsource[22])(sic1_to_pic22_signal);
	(*pic.vicintsource[23])(sic2_to_pic23_signal);
	(*pic.vicintsource[24])(sic3_to_pic24_signal);
	(*pic.vicintsource[25])(sic4_to_pic25_signal);
	(*pic.vicintsource[26])(sic5_to_pic26_signal);
	(*pic.vicintsource[27])(sic6_to_pic27_signal);
	(*pic.vicintsource[28])(sic7_to_pic28_signal);
	(*pic.vicintsource[29])(sic8_to_pic29_signal);
	(*pic.vicintsource[30])(sic9_to_pic30_signal);
	(*pic.vicintsource[31])(sic10_to_pic31_signal);
	// and those that need a stub
	(*pic.vicintsource[0])(wdog_to_pic0_signal);
	(*pic.vicintsource[1])(software_to_pic1_signal);
	(*pic.vicintsource[2])(commrx_to_pic2_signal);
	(*pic.vicintsource[3])(commtx_to_pic3_signal);
	(*pic.vicintsource[6])(gpio0_to_pic6_signal);
	(*pic.vicintsource[7])(gpio1_to_pic7_signal);
	(*pic.vicintsource[8])(gpio2_to_pic8_signal);
	(*pic.vicintsource[9])(gpio3_to_pic9_signal);
	(*pic.vicintsource[10])(rtc_to_pic10_signal);
	(*pic.vicintsource[11])(ssp_to_pic11_signal);
	(*pic.vicintsource[15])(sci0_to_pic15_signal);
	(*pic.vicintsource[16])(clcd_to_pic16_signal);
	(*pic.vicintsource[18])(pwrfail_to_pic18_signal);
	(*pic.vicintsource[19])(mbx_to_pic19_signal);
	(*pic.vicintsource[20])(gnd_to_pic20_signal);
	// and the required stubs
	wdog_int_stub.vicinttarget(wdog_to_pic0_signal);
	software_int_stub.vicinttarget(software_to_pic1_signal);
	commrx_int_stub.vicinttarget(commrx_to_pic2_signal);
	commtx_int_stub.vicinttarget(commtx_to_pic3_signal);
	gpio0_int_stub.vicinttarget(gpio0_to_pic6_signal);
	gpio1_int_stub.vicinttarget(gpio1_to_pic7_signal);
	gpio2_int_stub.vicinttarget(gpio2_to_pic8_signal);
	gpio3_int_stub.vicinttarget(gpio3_to_pic9_signal);
	rtc_int_stub.vicinttarget(rtc_to_pic10_signal);
	ssp_int_stub.vicinttarget(ssp_to_pic11_signal);
	sci0_int_stub.vicinttarget(sci0_to_pic15_signal);
	clcd_int_stub.vicinttarget(clcd_to_pic16_signal);
	pwrfail_int_stub.vicinttarget(pwrfail_to_pic18_signal);
	mbx_int_stub.vicinttarget(mbx_to_pic19_signal);
	gnd_int_stub.vicinttarget(gnd_to_pic20_signal);
	nvicfiqin_stub.vicinttarget(nvicfiqin_signal);

	pic.nvicfiqin(nvicfiqin_signal);
	nvicirqin_stub.vicinttarget(nvicirqin_signal);
	pic.nvicirqin(nvicirqin_signal);
	vicvectaddrin_stub.vicaddrtarget(vicvectaddrin_signal);
	pic.vicvectaddrin(vicvectaddrin_signal);

	pic.nvicfiq(nvicfiq_signal);
	pic.nvicirq(nvicirq_signal);
	pic.vicvectaddrout(vicvectaddrout_signal);
	vicvectaddrout_stub.vicaddrsource(vicvectaddrout_signal);
	/* PIC connections   END */

	/* SIC connections START */
	// connections to the PIC
	(*sic.sicinttarget[0])(sic0_to_pic21_signal);
	(*sic.sicinttarget[1])(sic1_to_pic22_signal);
	(*sic.sicinttarget[2])(sic2_to_pic23_signal);
	(*sic.sicinttarget[3])(sic3_to_pic24_signal);
	(*sic.sicinttarget[4])(sic4_to_pic25_signal);
	(*sic.sicinttarget[5])(sic5_to_pic26_signal);
	(*sic.sicinttarget[6])(sic6_to_pic27_signal);
	(*sic.sicinttarget[7])(sic7_to_pic28_signal);
	(*sic.sicinttarget[8])(sic8_to_pic29_signal);
	(*sic.sicinttarget[9])(sic9_to_pic30_signal);
	(*sic.sicinttarget[10])(sic10_to_pic31_signal);

	// connections to the peripherals
	(*sic.sicintsource[0])(sic_software_to_sic0_signal);
	(*sic.sicintsource[1])(mmci0b_to_sic1_signal);
	(*sic.sicintsource[2])(mmci1b_to_sic2_signal);
	(*sic.sicintsource[3])(kmi0_to_sic3_signal);
	(*sic.sicintsource[4])(kmi1_to_sic4_signal);
	(*sic.sicintsource[5])(sci1_to_sic5_signal);
	(*sic.sicintsource[6])(uart3_to_sic6_signal);
	(*sic.sicintsource[7])(charlcd_to_sic7_signal);
	(*sic.sicintsource[8])(tspen_to_sic8_signal);
	(*sic.sicintsource[9])(tskeypad_to_sic9_signal);
	(*sic.sicintsource[10])(reserved0_to_sic10_signal);
	(*sic.sicintsource[11])(reserved1_to_sic11_signal);
	(*sic.sicintsource[12])(reserved2_to_sic12_signal);
	(*sic.sicintsource[13])(reserved3_to_sic13_signal);
	(*sic.sicintsource[14])(reserved4_to_sic14_signal);
	(*sic.sicintsource[15])(reserved5_to_sic15_signal);
	(*sic.sicintsource[16])(reserved6_to_sic16_signal);
	(*sic.sicintsource[17])(reserved7_to_sic17_signal);
	(*sic.sicintsource[18])(reserved8_to_sic18_signal);
	(*sic.sicintsource[19])(reserved9_to_sic19_signal);
	(*sic.sicintsource[20])(reserved10_to_sic20_signal);
	(*sic.sicintsource[21])(diskonchip_to_sic21_signal);
	(*sic.sicintsource[22])(mmci0a_to_sic22_signal);
	(*sic.sicintsource[23])(mmci1a_to_sic23_signal);
	(*sic.sicintsource[24])(aaci_to_sic24_signal);
	(*sic.sicintsource[25])(ethernet_to_sic25_signal);
	(*sic.sicintsource[26])(usb_to_sic26_signal);
	(*sic.sicintsource[27])(pci0_to_sic27_signal);
	(*sic.sicintsource[28])(pci1_to_sic28_signal);
	(*sic.sicintsource[29])(pci2_to_sic29_signal);
	(*sic.sicintsource[30])(pci3_to_sic30_signal);
	// and the required stub connections
	sic_software_int_stub(sic_software_to_sic0_signal);
	mmci0b_int_stub(mmci0b_to_sic1_signal);
	mmci1b_int_stub(mmci1b_to_sic2_signal);
	kmi0_int_stub(kmi0_to_sic3_signal);
	kmi1_int_stub(kmi1_to_sic4_signal);
	sci1_int_stub(sci1_to_sic5_signal);
	uart3_int_stub(uart3_to_sic6_signal);
	charlcd_int_stub(charlcd_to_sic7_signal);
	tspen_int_stub(tspen_to_sic8_signal);
	tskeypad_int_stub(tskeypad_to_sic9_signal);
	reserved0_int_stub(reserved0_to_sic10_signal);
	reserved1_int_stub(reserved1_to_sic11_signal);
	reserved2_int_stub(reserved2_to_sic12_signal);
	reserved3_int_stub(reserved3_to_sic13_signal);
	reserved4_int_stub(reserved4_to_sic14_signal);
	reserved5_int_stub(reserved5_to_sic15_signal);
	reserved6_int_stub(reserved6_to_sic16_signal);
	reserved7_int_stub(reserved7_to_sic17_signal);
	reserved8_int_stub(reserved8_to_sic18_signal);
	reserved9_int_stub(reserved9_to_sic19_signal);
	reserved10_int_stub(reserved10_to_sic20_signal);
	diskonchip_int_stub(diskonchip_to_sic21_signal);
	mmci0a_int_stub(mmci0a_to_sic22_signal);
	mmci1a_int_stub(mmci1a_to_sic23_signal);
	aaci_int_stub(aaci_to_sic24_signal);
	ethernet_int_stub(ethernet_to_sic25_signal);
	usb_int_stub(usb_to_sic26_signal);
	pci0_int_stub(pci0_to_sic27_signal);
	pci1_int_stub(pci1_to_sic28_signal);
	pci2_int_stub(pci2_to_sic29_signal);
	pci3_int_stub(pci3_to_sic30_signal);

	// connections to the RealView Logic Tile
	(*sic.sicptintsource[0])(zu206_to_sic21_signal);
	(*sic.sicptintsource[1])(zu207_to_sic22_signal);
	(*sic.sicptintsource[2])(zu208_to_sic23_signal);
	(*sic.sicptintsource[3])(zu209_to_sic24_signal);
	(*sic.sicptintsource[4])(zu210_to_sic25_signal);
	(*sic.sicptintsource[5])(zu211_to_sic26_signal);
	(*sic.sicptintsource[6])(zu212_to_sic27_signal);
	(*sic.sicptintsource[7])(zu213_to_sic28_signal);
	(*sic.sicptintsource[8])(zu214_to_sic29_signal);
	(*sic.sicptintsource[9])(zu215_to_sic30_signal);
	(*sic.sicptintsource[10])(zu216_to_sic31_signal);
	// and the required stub connections
	zu206_int_stub(zu206_to_sic21_signal);
	zu207_int_stub(zu207_to_sic22_signal);
	zu208_int_stub(zu208_to_sic23_signal);
	zu209_int_stub(zu209_to_sic24_signal);
	zu210_int_stub(zu210_to_sic25_signal);
	zu211_int_stub(zu211_to_sic26_signal);
	zu212_int_stub(zu212_to_sic27_signal);
	zu213_int_stub(zu213_to_sic28_signal);
	zu214_int_stub(zu214_to_sic29_signal);
	zu215_int_stub(zu215_to_sic30_signal);
	zu216_int_stub(zu216_to_sic31_signal);
	
	/* SIC connections   END */

	eth_init_socket(eth.bus_target_socket);
	sc_init_socket(sc.bus_target_socket);
	uart0_init_socket(uart0.bus_target_socket);
	uart1_init_socket(uart1.bus_target_socket);
	uart2_init_socket(uart2.bus_target_socket);
	wd_init_socket(wd.bus_target_socket);
	dt1_init_socket(dt1.bus_target_socket);
	dt2_init_socket(dt2.bus_target_socket);
	pic_init_socket(pic.bus_target_socket);
	sic_init_socket(sic.bus_target_socket);
	dmac_init_socket(dmac.bus_target_socket);

	dmac_master1_target_socket(dmac.bus_master_1_socket);
	dmac_master2_target_socket(dmac.bus_master_2_socket);
}

PXP ::
~PXP()
{
}

bool 
PXP ::
BeginSetup()
{
	return true;
}

/** Blocking access to the system registers.
 *
 * @param trans the transaction
 * @param delay current delta time
 */
void 
PXP ::
AccessSystemRegisters(transaction_type &trans,
		sc_core::sc_time &delay)
{
	bool handled = false;

	switch ( trans.get_address() & 0x00000fffUL )
	{
		case 0x005c:
			Access24MHzCounter(trans, delay);
			handled = true;
			break;
	}

	if ( unlikely(!handled) )
	{
		logger << DebugError
			<< "Trying to access System Register at 0x" << std::hex
			<< (trans.get_address() & 0x00000fffUL) 
			<< " (0x" << trans.get_address() << ")" << std::dec
			<< EndDebugError;
		unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	}
}

/** Access to the 24MHz counter register.
 *
 * @param trans the transaction
 * @param delay current delta time
 */
void
PXP ::
Access24MHzCounter(transaction_type & trans,
		sc_core::sc_time &delay)
{
	uint8_t *data = trans.get_data_ptr();
	uint32_t size = trans.get_data_length();
	bool is_read = trans.is_read();

	if ( unlikely(!is_read) )
	{
		trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
		return;
	}

	if ( unlikely(size != 4) )
	{
		trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
		return;
	}

	static const sc_time clock(41666.66667, SC_PS);
	sc_time time = sc_time_stamp() + delay;

	uint64_t ticks = time/clock;
	uint32_t reg = ticks & 0x00000000ffffffffULL;
	
	logger << DebugInfo
		<< "Reading 24MHz counter:" << std::endl
		<< " - current time = " << sc_time_stamp() << std::endl
		<< " - delay = " << delay << std::endl
		<< " - computed time = " << clock << std::endl
		<< " - ticks = " << ticks << std::endl
		<< " - reg = 0x" << std::hex << reg << std::dec
		<< " (" << reg << ")"
		<< EndDebugInfo;

	reg = Host2LittleEndian(reg);
	memcpy(data, &reg, 4);
	trans.set_response_status(tlm::TLM_OK_RESPONSE);
}

/**************************************************************************/
/* Virtual methods for the target socket for the cpu connection     START */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
cpu_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO: cpu_target_nb_transport_fw"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
cpu_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	bool handled = false;
	if ( trans.get_address() < 0x08000000UL )
	{
		mpmc0_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x10000000UL &&
			trans.get_address()   <  0x10001000UL )
	{
		AccessSystemRegisters(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x10003000UL &&
			trans.get_address()   <  0x10003030UL )
	{
		sic_init_socket->b_transport(trans,delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x10010000UL &&
			trans.get_address()   <  0x10020000UL )
	{
		eth_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x10130000UL &&
			trans.get_address()   <  0x10140000UL )
	{
		dmac_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x10140000UL &&
			trans.get_address()   <  0x10150000UL )
	{
		pic_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101e0000UL &&
			trans.get_address()   <  0x101e1000UL )
	{
		sc_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101e1000UL && 
			trans.get_address()   <  0x101e2000UL )
	{
		wd_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101e2000UL &&
			trans.get_address()   <  0x101e3000UL )
	{
		dt1_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101e3000UL &&
			trans.get_address()   <  0x101e4000UL )
	{
		dt2_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101f1000UL &&
			trans.get_address()   <  0x101f2000UL )
	{
		uart0_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101f2000UL &&
			trans.get_address()   <  0x101f3000UL )
	{
		uart1_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x101f3000UL &&
			trans.get_address()   <  0x101f4000UL )
	{
		uart2_init_socket->b_transport(trans, delay);
		handled = true;
	}
	else if ( trans.get_address() >= 0x34000000UL &&
			trans.get_address()   <  0x38000000UL )
	{
		ssmc1_init_socket->b_transport(trans, delay);
		handled = true;
	}
	if ( !handled )
	{
		logger << DebugError
			<< "Unhandled address 0x" 
			<< std::hex << trans.get_address() << std::dec
			<< EndDebugError;
		unisim::kernel::Simulator::Instance()->Stop(this, 2);
		// assert("TODO" == 0);
	}
}

bool 
PXP ::
cpu_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	logger << DebugError
		<< "TLM2 Direct memory access not supported."
		<< EndDebugError;
	return false;
}

unsigned int 
PXP ::
cpu_target_transport_dbg(transaction_type &trans)
{
	if ( trans.get_address() < 0x08000000UL )
		return mpmc0_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x10003000UL &&
			trans.get_address()     <  0x10003030UL )
		return sic_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x10010000UL &&
			trans.get_address()     <  0x10020000UL )
		return eth_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x10140000UL &&
			trans.get_address()     <  0x10150000UL )
		return pic_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101e0000UL &&
			trans.get_address()     <  0x101e1000UL )
		return sc_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101e1000UL && 
			trans.get_address()     <  0x101e2000UL )
		return wd_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101e2000UL &&
			trans.get_address()     <  0x101e3000UL )
		return dt1_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101e3000UL &&
			trans.get_address()		<  0x101e4000UL )
		return dt2_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101f1000UL &&
			trans.get_address()     <  0x101f2000UL )
		return uart0_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101f2000UL &&
			trans.get_address()     <  0x101f3000UL )
		return uart1_init_socket->transport_dbg(trans);
	else if ( trans.get_address()   >= 0x101f3000UL &&
			trans.get_address()     <  0x101f4000UL )
		return uart2_init_socket->transport_dbg(trans);
	else if ( trans.get_address() >= 0x34000000UL &&
			trans.get_address()   <  0x38000000UL )
		return ssmc1_init_socket->transport_dbg(trans);
	return 0;
}

/**************************************************************************/
/* Virtual methods for the target socket for the cpu connection       END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the target sockets of the dmac connections   START */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
dmac_master1_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase, sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO: dmac_master1_target_nb_transport_fw"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
dmac_master1_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	cpu_target_b_transport(trans, delay);
}

bool 
PXP ::
dmac_master1_target_get_direct_mem_ptr(transaction_type &trans,
		tlm::tlm_dmi &dmi_data)
{
	logger << DebugError
		<< "TLM2 Direct memory access not supported."
		<< EndDebugError;
	return false;
}

unsigned int 
PXP ::
dmac_master1_target_transport_dbg(transaction_type &trans)
{
	logger << DebugError
		<< "TLM2 debug interface not supported from dmac access."
		<< EndDebugError;
	return 0;
}

tlm::tlm_sync_enum
PXP ::
dmac_master2_target_nb_transport_fw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO: dmac_master2_target_nb_transport_fw"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
dmac_master2_target_b_transport(transaction_type &trans, 
		sc_core::sc_time &delay)
{
	cpu_target_b_transport(trans, delay);
}

bool
PXP ::
dmac_master2_target_get_direct_mem_ptr(transaction_type &trans, 
		tlm::tlm_dmi &dmi_data)
{
	logger << DebugError
		<< "TLM2 Direct memory access not supported."
		<< EndDebugError;
	return false;
}

unsigned int
PXP ::
dmac_master2_target_transport_dbg(transaction_type &trans)
{
	logger << DebugError
		<< "TLM2 debug interface not supported from dmac access."
		<< EndDebugError;
	return 0;
}

/**************************************************************************/
/* Virtual methods for the target sockets of the dmac connections     END */
/**************************************************************************/
	
/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the SSMC Chip Select 0                                               */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
ssmc0_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
ssmc0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the SSMC Chip Select 0                                           END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the MPMC Chip Select 0                                               */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
mpmc0_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
mpmc0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the MPMC Chip Select 0                                           END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the Ethernet Controller                                              */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
eth_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
eth_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the Ethernet Controller                                          END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the System Controller                                                */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
sc_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
sc_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the System Controller                                            END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the UART 0                                                           */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
uart0_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
uart0_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the UART 0                                                       END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the UART 1                                                           */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
uart1_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
uart1_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the UART 1                                                       END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the UART 2                                                           */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
uart2_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
uart2_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the UART 2                                                       END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the Dual Timers                                                      */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
dt1_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
dt1_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

tlm::tlm_sync_enum
PXP ::
dt2_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
dt2_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the Dual Timers                                                  END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the Watchdog                                                         */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
wd_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
wd_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the Watchdog                                                     END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the PIC                                                              */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
pic_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
pic_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the PIC                                                          END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the SIC                                                              */
/**************************************************************************/

tlm::tlm_sync_enum
PXP ::
sic_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void 
PXP ::
sic_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the SIC                                                          END */
/**************************************************************************/

/**************************************************************************/
/* Virtual methods for the initiator socket for                     START */
/*   the DMAC                                                             */
/**************************************************************************/
tlm::tlm_sync_enum
PXP ::
dmac_init_nb_transport_bw(transaction_type &trans,
		phase_type &phase,
		sc_core::sc_time &time)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
	return tlm::TLM_COMPLETED;
}

void
PXP ::
dmac_init_invalidate_direct_mem_ptr(sc_dt::uint64,
		sc_dt::uint64)
{
	logger << DebugError
		<< "TODO"
		<< EndDebugError;
	unisim::kernel::Simulator::Instance()->Stop(this, __LINE__);
}

/**************************************************************************/
/* Virtual methods for the initiator socket for                           */
/*   the DMAC                                                         END */
/**************************************************************************/

} // end of namespace arm926ejs_pxp
} // end of namespace chipset
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim


