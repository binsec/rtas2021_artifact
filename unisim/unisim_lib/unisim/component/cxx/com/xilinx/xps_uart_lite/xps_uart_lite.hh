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

#ifndef __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_UART_LITE_XPS_UART_LITE_HH__
#define __UNISIM_COMPONENT_CXX_COM_XILINX_XPS_UART_LITE_XPS_UART_LITE_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/variable/variable.hh>

#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/char_io.hh>
#include <unisim/util/queue/queue.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace com {
namespace xilinx {
namespace xps_uart_lite {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::variable::Statistic;
using unisim::kernel::variable::StatisticFormula;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceExportBase;
using unisim::kernel::ServiceImport;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::CharIO;

template <class CONFIG>
class XPS_UARTLite
	: public Service<Memory<typename CONFIG::MEMORY_ADDR> >
	, public Client<CharIO>
{
public:
	ServiceExport<Memory<typename CONFIG::MEMORY_ADDR> > memory_export;
	ServiceImport<CharIO> char_io_import;
	
	XPS_UARTLite(const char *name, Object *parent = 0);
	virtual ~XPS_UARTLite();
	
	virtual bool BeginSetup();
	
	void ResetRegs();
	void Reset();

	virtual void ResetMemory();
	virtual bool ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size);
	virtual bool ReadMemory(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	virtual bool WriteMemory(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size, const uint8_t *byte_enable, uint32_t byte_enable_length, uint32_t streaming_width);
	
	bool Read(typename CONFIG::MEMORY_ADDR addr, uint8_t& value, bool debug = false);
	void Write(typename CONFIG::MEMORY_ADDR addr, uint8_t value, bool debug = false);
	bool Read(typename CONFIG::MEMORY_ADDR addr, void *buffer, uint32_t size);
	void Write(typename CONFIG::MEMORY_ADDR addr, const void *buffer, uint32_t size);
	
	bool IsMapped(typename CONFIG::MEMORY_ADDR addr, uint32_t size) const;
protected:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	bool IsVerbose() const;
private:
	typename unisim::util::queue::Queue<typename CONFIG::RX_FIFO_CONFIG> rx_fifo;
	typename unisim::util::queue::Queue<typename CONFIG::TX_FIFO_CONFIG> tx_fifo;
	uint8_t stat_reg;
	bool tx_fifo_becomes_empty;
	
	typename CONFIG::MEMORY_ADDR c_baseaddr;
	typename CONFIG::MEMORY_ADDR c_highaddr;
	
	Parameter<bool> param_verbose;
	Parameter<typename CONFIG::MEMORY_ADDR> param_c_baseaddr;
	Parameter<typename CONFIG::MEMORY_ADDR> param_c_highaddr;
	
protected:
	uint8_t GetRX_FIFO() const;
	bool ReadRX_FIFO(uint8_t& value);
	uint8_t GetSTAT_REG() const;
	uint8_t GetSTAT_REG_RX_FIFO_VALID_DATA() const;
	uint8_t GetSTAT_REG_INTR_ENABLED() const;
	uint8_t ReadSTAT_REG();
	bool TX_FIFO_BecomesEmpty() const;
	void ResetTX_FIFO_BecomesEmpty();
	bool HasInterrupt() const;

	void SetTX_FIFO(uint8_t value);
	void WriteTX_FIFO(uint8_t value);
	void WriteCTRL_REG(uint8_t value);
	
	void TelnetProcessInput();
	void TelnetProcessOutput(bool flush_telnet_output);
};

} // end of namespace xps_uart_lite
} // end of namespace xilinx
} // end of namespace com
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
