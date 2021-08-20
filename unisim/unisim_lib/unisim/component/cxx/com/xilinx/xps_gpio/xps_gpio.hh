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

#ifndef __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_GPIO_XPS_GPIO_HH__
#define __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_GPIO_XPS_GPIO_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>

#include <unisim/service/interfaces/memory.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace com {
namespace xilinx {
namespace xps_gpio {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::StatisticFormula;
using unisim::kernel::Service;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::service::interfaces::Memory;

template <class CONFIG>
class XPS_GPIO
	: public Service<Memory<typename CONFIG::MEMORY_ADDR> >
{
public:
	static const unsigned int NUM_GPIO_CHANNELS = CONFIG::C_IS_DUAL ? 2 : 1;
	static const uint32_t GPIO_MASK = (1ULL << CONFIG::C_GPIO_WIDTH) - 1; // GPIO Channel 1 bit mask
	static const uint32_t GPIO2_MASK = (1ULL << CONFIG::C_GPIO2_WIDTH) - 1; // GPIO Channel 2 bit mask
	
	ServiceExport<Memory<typename CONFIG::MEMORY_ADDR> > memory_export;
	
	XPS_GPIO(const char *name, Object *parent = 0);
	virtual ~XPS_GPIO();
	
	virtual bool BeginSetup();
	
	void Reset();

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
	static const unsigned int GPIO_DATA_IN_IDX_SHIFT = 0;
	static const unsigned int GPIO_DATA_IDX_SHIFT = 1;
	static const unsigned int GPIO_TRI_IDX_SHIFT = 2;
	static const unsigned int GPIO2_DATA_IN_IDX_SHIFT = 3;
	static const unsigned int GPIO2_DATA_IDX_SHIFT = 4;
	static const unsigned int GPIO2_TRI_IDX_SHIFT = 5;
	static const unsigned int GIER_IDX_SHIFT = 6;
	static const unsigned int IP_IER_IDX_SHIFT = 7;
	static const unsigned int IP_ISR_IDX_SHIFT = 8;
	
	unsigned int toggle;
	unsigned int read_idx;
	unsigned int write_idx;

	uint32_t gpio_data_in[2];
	uint32_t gpio_data[2];
	uint32_t gpio_tri[2];
	uint32_t gpio2_data_in[2];
	uint32_t gpio2_data[2];
	uint32_t gpio2_tri[2];
	uint32_t gier[2];
	uint32_t ip_ier[2];
	uint32_t ip_isr[2];
	
	typename CONFIG::MEMORY_ADDR c_baseaddr;
	typename CONFIG::MEMORY_ADDR c_highaddr;
	
	Parameter<bool> param_verbose;
	Parameter<typename CONFIG::MEMORY_ADDR> param_c_baseaddr;
	Parameter<typename CONFIG::MEMORY_ADDR> param_c_highaddr;
	
protected:
	uint32_t GetGPIO_DATA() const;
	uint32_t GetGPIO_TRI() const;
	uint32_t GetGPIO2_DATA() const;
	uint32_t GetGPIO2_TRI() const;
	uint32_t GetGIER() const;
	uint32_t GetGIER_GLOBAL_INTERRUPT_ENABLE() const;
	uint32_t GetIP_IER() const;
	uint32_t GetIP_ISR() const;
	
	void SetGPIO_DATA_IN(uint32_t value);
	void SetGPIO_DATA(uint32_t value);
	void SetGPIO_DATA(unsigned int bitnum, bool value);
	void SetGPIO_TRI(uint32_t value);
	void SetGPIO2_DATA_IN(uint32_t value);
	void SetGPIO2_DATA(uint32_t value);
	void SetGPIO2_DATA(unsigned int bitnum, bool value);
	void SetGPIO2_TRI(uint32_t value);
	void SetGIER(uint32_t value);
	void SetIP_IER(uint32_t value);
	void SetIP_ISR(uint32_t value);
};

} // end of namespace xps_gpio
} // end of namespace xilinx
} // end of namespace com
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
