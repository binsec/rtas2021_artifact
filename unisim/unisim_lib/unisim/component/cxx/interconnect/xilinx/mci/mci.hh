/*
 *  Copyright (c) 2010-2011,
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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_MCI_MCI_HH__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_MCI_MCI_HH__

#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/kernel/logger/logger.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace mci {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::service::interfaces::Memory;

template <class CONFIG>
class MCI
	: public Service<Memory<typename CONFIG::ADDRESS> >
	, public Client<Memory<typename CONFIG::ADDRESS> >
{
public:
	ServiceExport<Memory<typename CONFIG::ADDRESS> > memory_export;
	ServiceImport<Memory<typename CONFIG::ADDRESS> > memory_import;
	
	MCI(const char *name, Object *parent = 0);
	virtual ~MCI();
	
	void Reset();
	
	virtual void ResetMemory();
	virtual bool ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size);

	bool IsMappedDCR(uint32_t dcrn) const;
	uint32_t ReadDCR(unsigned int dcrn);
	void WriteDCR(unsigned int dcrn, uint32_t value);
	
protected:
	typedef enum
	{
		IF_MASTER_MCI,
		IF_SLAVE_MCI,
		IF_DCR,
	} Interface;
	
	unisim::kernel::logger::Logger logger;
	bool IsVerbose() const;
	const char *GetInterfaceName(Interface intf) const;
private:
	uint32_t mi_rowconflict_mask;
	uint32_t mi_bankconflict_mask;
	uint32_t mi_control;
protected:
	bool verbose;
	Parameter<bool> param_verbose;
	
	uint32_t GetMI_ROWCONFLICT_MASK() const;
	uint32_t GetMI_BANKCONFLICT_MASK() const;
	uint32_t GetMI_CONTROL() const;
	uint32_t GetMI_CONTROL_ENABLE() const;

	void SetMI_ROWCONFLICT_MASK(uint32_t value);
	void SetMI_BANKCONFLICT_MASK(uint32_t value);
	void SetMI_CONTROL(uint32_t value);

	void LogMI_CONTROL(uint32_t old_value, uint32_t new_value);
};

} // end of namespace mci
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_MCI_MCI_HH__
