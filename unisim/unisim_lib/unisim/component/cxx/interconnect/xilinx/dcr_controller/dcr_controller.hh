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
 
#ifndef __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_HH__
#define __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_HH__

#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/logger/logger.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace interconnect {
namespace xilinx {
namespace dcr_controller {

using unisim::kernel::Object;
using unisim::kernel::variable::Parameter;

template <class CONFIG>
class DCRController
	: public virtual Object
{
public:
	static const uint32_t DCR_CONTROLLER_BASEADDR = (CONFIG::TIEDCRBASEADDR << 8); // DCR controller base address
	static const uint32_t DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_RESET_VALUE = CONFIG::DCR_AUTOLOCK_ENABLE ? CONFIG::DCR_CONTROLLER_STATUS_AND_CONTROL_REGISTER_AUTOLOCK_MASK : 0;
	
	typedef enum
	{
		ST_IDLE = 0,
		ST_C440 = 1,
		ST_XM = 2,
		ST_STATUS_RD = 3
	} State;
	
	typedef enum
	{
		EV_C440_REQ,
		EV_XM_REQ,
		EV_C440_DONE,
		EV_XM_DONE,
		EV_C440_LOCK,
		EV_XM_LOCK,
		EV_C440_STATUS_RD,
		EV_XM_STATUS_RD,
		EV_READ_DONE
	} Event;
	
	DCRController(const char *name, Object *parent = 0);
	virtual ~DCRController();
	
	void SetIndirectModeAddressRegister(unsigned int num_master, uint32_t value);
	void SetDCRControllerStatusAndControlRegister(unsigned int num_master, uint32_t value);
	uint32_t GetIndirectModeAddressRegister() const;
	uint32_t GetDCRControllerStatusAndControlRegister() const;
	
	uint32_t GetDCRControllerStatusAndControlRegister_C440_LOCK() const;
	uint32_t GetDCRControllerStatusAndControlRegister_C440_ALOCK() const;
	uint32_t GetDCRControllerStatusAndControlRegister_XM_LOCK() const;
	uint32_t GetDCRControllerStatusAndControlRegister_XM_ALOCK() const;
	uint32_t GetDCRControllerStatusAndControlRegister_AUTOLOCK() const;
	uint32_t GetDCRControllerStatusAndControlRegister_XM_ASYN() const;
	uint32_t GetDCRControllerStatusAndControlRegister_XS_ASYN() const;
	uint32_t GetDCRControllerStatusAndControlRegister_XM_TOWAIT() const;
	uint32_t GetDCRControllerStatusAndControlRegister_C440_TIMEOUT() const;
	uint32_t GetDCRControllerStatusAndControlRegister_XM_TIMEOUT() const;
	
	void SetDCRControllerStatusAndControlRegister_C440_TIMEOUT();
	void SetDCRControllerStatusAndControlRegister_XM_TIMEOUT();

	void ResetALOCK(unsigned int num_master);
	void ResetDCRControllerStatusAndControlRegister_C440_ALOCK();
	void ResetDCRControllerStatusAndControlRegister_XM_ALOCK();

protected:
	unisim::kernel::logger::Logger logger;
	bool IsVerbose() const;
	bool IsMappedDCR(uint32_t dcrn) const;
	int Route(uint32_t dcrn);
	int GetMasterInterface(unsigned int num_master) const;
	int GetSlaveInterface(unsigned int num_slave) const;
	bool IsMasterInterface(int intf) const;
	bool IsSlaveInterface(int intf) const;
	unsigned int GetSlave(int intf) const;
	unsigned int GetMaster(int intf) const;
	void Read(uint32_t dcrn, uint32_t& value);
	void Write(unsigned int num_master, uint32_t dcrn, uint32_t value);
private:
	uint32_t indirect_mode_address_register;
	uint32_t dcr_controller_status_and_control_register;
	State state;
	
	bool verbose;
	Parameter<bool> param_verbose;
	
	void LogDCRControllerStatusAndControlRegister(uint32_t old_value, uint32_t new_value);
};

} // end of namespace dcr_controller
} // end of namespace xilinx
} // end of namespace interconnect
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_INTERCONNECT_XILINX_DCR_CONTROLLER_DCR_CONTROLLER_HH__
