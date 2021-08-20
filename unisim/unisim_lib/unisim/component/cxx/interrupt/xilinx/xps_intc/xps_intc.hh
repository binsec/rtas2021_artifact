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

#ifndef __UNISIM_COMPONENT_CXX_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_HH__
#define __UNISIM_COMPONENT_CXX_INTERRUPT_XILINX_XPS_INTC_XPS_INTC_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

#include <unisim/service/interfaces/memory.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace interrupt {
namespace xilinx {
namespace xps_intc {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::service::interfaces::Memory;

template <class CONFIG>
class XPS_IntC
	: public Service<Memory<typename CONFIG::MEMORY_ADDR> >
{
public:
	// Mask
	static const uint32_t MASK = (1ULL << CONFIG::C_NUM_INTR_INPUTS) - 1; // ISR, IPR, IER, IAR, SIE, CIE bit masks

	ServiceExport<Memory<typename CONFIG::MEMORY_ADDR> > memory_export;
	
	XPS_IntC(const char *name, Object *parent = 0);
	virtual ~XPS_IntC();
	
	void Reset();
	
	virtual bool BeginSetup();

	virtual void ResetMemory();
	virtual bool ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	virtual bool WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	
	void Read(typename CONFIG::MEMORY_ADDR addr, uint32_t& value);
	void Write(typename CONFIG::MEMORY_ADDR addr, uint32_t value);
protected:
	void SetInterruptInput(unsigned int irq, bool level);
	void DetectInterruptInput();
	virtual void SetOutputLevel(bool level);
	virtual void SetOutputEdge(bool final_level);
	void GenerateRequest();
	unisim::kernel::logger::Logger logger;
	bool IsVerbose() const;
	bool IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const;
private:
	
	uint32_t isr;   // Interrupt Status Register
	uint32_t ier;   // Interrupt Enable Register
	uint32_t ivr;   // Interrupt Vector Register
	uint32_t mer;   // Master Enable Register
	
	uint32_t intr[2];  // Interrupt inputs
	
	uint32_t GetISR() const;
	uint32_t GetIPR() const;
	uint32_t GetIER() const;
	uint32_t GetIVR() const;
	uint32_t GetMER() const;
	uint32_t GetMER_ME() const;
	uint32_t GetMER_HIE() const;
	
	void SetISR(uint32_t value);
	void SetIER(uint32_t value);
	void SetIAR(uint32_t value);
	void SetSIE(uint32_t value);
	void SetCIE(uint32_t value);
	void SetMER(uint32_t value);
	
	bool verbose;
	typename CONFIG::MEMORY_ADDR c_baseaddr;
	typename CONFIG::MEMORY_ADDR c_highaddr;
	Parameter<bool> param_verbose;
	Parameter<typename CONFIG::MEMORY_ADDR> param_c_baseaddr;
	Parameter<typename CONFIG::MEMORY_ADDR> param_c_highaddr;
	
};

} // end of namespace xps_intc
} // end of namespace xilinx
} // end of namespace interrupt
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
