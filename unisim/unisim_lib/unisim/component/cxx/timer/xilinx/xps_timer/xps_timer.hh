/*
 *  Copyright (c) 2011,
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

#ifndef __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_TIMER_XPS_TIMER_HH__
#define __UNISIM_COMPONENT_CXX_TIMER_XILINX_XPS_TIMER_XPS_TIMER_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

#include <unisim/service/interfaces/memory.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace timer {
namespace xilinx {
namespace xps_timer {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::StatisticFormula;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::service::interfaces::Memory;

template <class CONFIG>
class XPS_Timer
	: public Service<Memory<typename CONFIG::MEMORY_ADDR> >
{
public:
	ServiceExport<Memory<typename CONFIG::MEMORY_ADDR> > memory_export;
	
	XPS_Timer(const char *name, Object *parent = 0);
	virtual ~XPS_Timer();
	
	void Reset();
	
	virtual bool BeginSetup();

	virtual void ResetMemory();
	virtual bool ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	virtual bool WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	
	template <typename T> void Read(typename CONFIG::MEMORY_ADDR addr, T& value);
	template <typename T> void Write(typename CONFIG::MEMORY_ADDR addr, T value);
	bool IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const;
	
	void Update();
protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	bool IsVerbose() const;
private:
	static const unsigned int TCSR0_IDX_SHIFT = 0;
	static const unsigned int TLR0_IDX_SHIFT = 1;
	static const unsigned int TCR0_IDX_SHIFT = 2;
	static const unsigned int TCSR1_IDX_SHIFT = 3;
	static const unsigned int TLR1_IDX_SHIFT = 4;
	static const unsigned int TCR1_IDX_SHIFT = 5;
	
	unsigned int toggle;
	unsigned int read_idx;
	unsigned int write_idx;
	
	uint32_t tcsr0[2];   // Control/Status Register 0
	uint32_t tlr0[2];    // Load register 0
	uint32_t tcr0[2];    // Timer/Counter Register 0
	uint32_t tcsr1[2];   // Control/Status Register 1
	uint32_t tlr1[2];    // Load register 1
	uint32_t tcr1[2];    // Timer/Counter Register 1
	
	
	bool tcr0_roll_over;
	bool tcr1_roll_over;
	uint64_t num_tcr0_roll_over;
	uint64_t num_tcr1_roll_over;
	uint64_t num_timer0_generate_interrupts;
	uint64_t num_timer1_generate_interrupts;
	uint64_t num_timer0_generate_interrupt_losses;
	uint64_t num_timer1_generate_interrupt_losses;
	uint64_t num_timer0_captures;
	uint64_t num_timer1_captures;
	uint64_t num_timer0_old_capture_losses;
	uint64_t num_timer1_old_capture_losses;
	uint64_t num_timer0_new_capture_losses;
	uint64_t num_timer1_new_capture_losses;
	typename CONFIG::MEMORY_ADDR c_baseaddr;
	typename CONFIG::MEMORY_ADDR c_highaddr;
	
	Parameter<bool> param_verbose;
	Parameter<typename CONFIG::MEMORY_ADDR> param_c_baseaddr;
	Parameter<typename CONFIG::MEMORY_ADDR> param_c_highaddr;
	Statistic<uint64_t> stat_num_tcr0_roll_over;
	Statistic<uint64_t> stat_num_tcr1_roll_over;
	Statistic<uint64_t> stat_num_timer0_generate_interrupts;
	Statistic<uint64_t> stat_num_timer1_generate_interrupts;
	Statistic<uint64_t> stat_num_timer0_generate_interrupt_losses;
	Statistic<uint64_t> stat_num_timer1_generate_interrupt_losses;
	Statistic<uint64_t> stat_num_timer0_captures;
	Statistic<uint64_t> stat_num_timer1_captures;
	Statistic<uint64_t> stat_num_timer0_old_capture_losses;
	Statistic<uint64_t> stat_num_timer1_old_capture_losses;
	Statistic<uint64_t> stat_num_timer0_new_capture_losses;
	Statistic<uint64_t> stat_num_timer1_new_capture_losses;
	StatisticFormula<uint64_t> stat_num_timer0_capture_losses;
	StatisticFormula<uint64_t> stat_num_timer1_capture_losses;
	
	void LogTCSR();

protected:
	uint32_t GetTCSR0() const;
	uint32_t GetTLR0() const;
	uint32_t GetTCR0() const;
	uint32_t GetTCSR1() const;
	uint32_t GetTLR1() const;
	uint32_t GetTCR1() const;
	
	void SetTCSR0(uint32_t value);
	void SetTLR0(uint32_t value);
	void SetTCR0(uint32_t value);
	void SetTCSR1(uint32_t value);
	void SetTLR1(uint32_t value);
	void SetTCR1(uint32_t value);
	void SetTCSR0_T0INT();
	void SetTCSR1_T1INT();
	
	uint32_t GetTCSR0_ENALL() const;
	uint32_t GetTCSR0_PWMA0() const;
	uint32_t GetTCSR0_T0INT() const;
	uint32_t GetTCSR0_ENT0() const;
	uint32_t GetTCSR0_ENIT0() const;
	uint32_t GetTCSR0_LOAD0() const;
	uint32_t GetTCSR0_ARHT0() const;
	uint32_t GetTCSR0_CAPT0() const;
	uint32_t GetTCSR0_GENT0() const;
	uint32_t GetTCSR0_UDT0() const;
	uint32_t GetTCSR0_MDT0() const;
	
	uint32_t GetTCSR1_ENALL() const;
	uint32_t GetTCSR1_PWMB0() const;
	uint32_t GetTCSR1_T1INT() const;
	uint32_t GetTCSR1_ENT1() const;
	uint32_t GetTCSR1_ENIT1() const;
	uint32_t GetTCSR1_LOAD1() const;
	uint32_t GetTCSR1_ARHT1() const;
	uint32_t GetTCSR1_CAPT1() const;
	uint32_t GetTCSR1_UDT1() const;
	uint32_t GetTCSR1_MDT1() const;

	bool IsTCSR0_ENT0_Rising() const;
	bool IsTCSR1_ENT1_Rising() const;
	bool IsTCSR0_UDT0_Toggling() const;
	bool IsTCSR1_UDT1_Toggling() const;
	bool NeedsLoadingTCR0() const;
	bool NeedsLoadingTCR1() const;

	bool RunCounter0(uint32_t count);
	bool RunCounter1(uint32_t count);
	void CaptureTrigger0();
	void CaptureTrigger1();
};

} // end of namespace xps_timer
} // end of namespace xilinx
} // end of namespace timer
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
