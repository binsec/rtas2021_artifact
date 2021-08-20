/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_EPIC_EPIC_HH__
#define __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_EPIC_EPIC_HH__

#include <inttypes.h>

#include "unisim/kernel/kernel.hh"
#include <unisim/kernel/variable/variable.hh>
#include "unisim/service/interfaces/memory.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/component/cxx/chipset/mpc107/epic/register.hh"
#include "unisim/component/cxx/chipset/mpc107/epic/inservice_reg.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {
namespace epic {

using unisim::kernel::Object;
using unisim::kernel::Service;
using unisim::kernel::Client;
using unisim::kernel::variable::Parameter;
using unisim::kernel::ServiceExport;
using unisim::kernel::ServiceImport;
using unisim::service::interfaces::Memory;

template <class PHYSICAL_ADDR, 
	bool DEBUG = false>
class EPIC :
	public Service<Memory<PHYSICAL_ADDR> > {
public:
	static const unsigned int NUM_IRQS = 4;
	static const uint32_t ADDR_MASK = (uint32_t)0x0fffff;
		
	/* memory service */
	ServiceExport<Memory<PHYSICAL_ADDR> > memory_export;
	
	EPIC(const char *name, Object *parent = 0);
	virtual ~EPIC();

	/* Initialization methods of the service */
	virtual void Reset();
	virtual bool BeginSetup();
		
	/* Methods to implement for Service<MemoryInterface<PHYSICAL_ADDR> > */
	virtual void ResetMemory();
	virtual bool ReadMemory(PHYSICAL_ADDR addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(PHYSICAL_ADDR addr, const void *buffer, uint32_t size);

	void TimeSignal(uint64_t sdram_cycles);
	void SetIRQ(unsigned int id, unsigned int serial_id);
	void UnsetIRQ(unsigned int id, unsigned int serial_id);
	void WriteRegister(PHYSICAL_ADDR addr, uint32_t data, uint32_t size);
	uint32_t ReadRegister(PHYSICAL_ADDR addr, uint32_t size);
	
	virtual void SetINT() = 0;
	virtual void UnsetINT() = 0;
	virtual void SetSoftReset() = 0;
	virtual void UnsetSoftReset() = 0;
//	virtual void Notify(uint64_t sdram_cycles) = 0;
	virtual void StopSimulation() = 0;
	virtual void ActivateSDRAMClock() = 0;
	virtual void DeactivateSDRAMClock() = 0;

protected:
	void ResetEPIC();

	unisim::kernel::logger::Logger logger;
	bool verbose;
	Parameter<bool> param_verbose;
private:
	Registers regs;
	/* how internal registers work for pending/service interruptions:
	 * - if bit x =0 no interruption
	 * - if bit x =1 interruption in the specified input
	 * 
	 * lower bits have bigger priority than higher bits (0 bigger than 1 and so on)
	 * 
	 * description of each bit:
	 * - bit 0: timer 0
	 * - bit 1: timer 1
	 * - bit 2: timer 2
	 * - bit 3: timer 3
	 * - bit 4: dma 0
	 * - bit 5: dma 1
	 * - bit 6: message unit
	 * - bit 7: i2c
	 * - bit 8: direct irq[0] (or serial irq 0)
	 * - bit 9: direct irq[1] (or serial irq 1)
	 * - bit 10: direct irq[2] (or serial irq 2)
	 * - bit 11: direct irq[3] (or serial irq 3)
	 * - bit 12: direct irq[4] (or serial irq 4)
	 * - bit 13: serial irq 5
	 * - bit 14: serial irq 6
	 * - bit 15: serial irq 7
	 * - bit 16: serial irq 8
	 * - bit 17: serial irq 9
	 * - bit 18: serial irq 10
	 * - bit 19: serial irq 11
	 * - bit 20: serial irq 12
	 * - bit 21: serial irq 13
	 * - bit 22: serial irq 14
	 * - bit 23: serial irq 15
	 */
	uint32_t pending_reg;
	uint32_t irq_selector;
	uint32_t irq_req_reg;
	/* internal register inservice_reg is implemented as a list of inservice
	 *   interruptions */
	InserviceReg inservice_reg;
	/* masks for the different interrupts */
	static const uint32_t IRQ_T0 = (uint32_t)0x01;
	static const uint32_t IRQ_T1 = (uint32_t)0x02;
	static const uint32_t IRQ_T2 = (uint32_t)0x04;
	static const uint32_t IRQ_T3 = (uint32_t)0x08;
	static const uint32_t IRQ_DMA0 = (uint32_t)0x010;
	static const uint32_t IRQ_DMA1 = (uint32_t)0x020;
	static const uint32_t IRQ_MU = (uint32_t)0x040;
	static const uint32_t IRQ_I2C = (uint32_t)0x080;
	static const uint32_t IRQ_0 = (uint32_t)0x0100;
	static const uint32_t IRQ_1 = (uint32_t)0x0200;
	static const uint32_t IRQ_2 = (uint32_t)0x0400;
	static const uint32_t IRQ_3 = (uint32_t)0x0800;
	static const uint32_t IRQ_4 = (uint32_t)0x01000;
	static const uint32_t IRQ_5 = (uint32_t)0x02000;
	static const uint32_t IRQ_6 = (uint32_t)0x04000;
	static const uint32_t IRQ_7 = (uint32_t)0x08000;
	static const uint32_t IRQ_8 = (uint32_t)0x010000;
	static const uint32_t IRQ_9 = (uint32_t)0x020000;
	static const uint32_t IRQ_10 = (uint32_t)0x040000;
	static const uint32_t IRQ_11 = (uint32_t)0x080000;
	static const uint32_t IRQ_12 = (uint32_t)0x0100000;
	static const uint32_t IRQ_13 = (uint32_t)0x0200000;
	static const uint32_t IRQ_14 = (uint32_t)0x0400000;
	static const uint32_t IRQ_15 = (uint32_t)0x0800000;
//	static const uint32_t IRQ_16 = (uint32_t)0x01000000;
	
	uint32_t GetVPRFromIRQMask(uint32_t mask);
	bool PassthroughMode();
	bool MixedMode();
	bool SerialMode();
	bool DirectMode();
	uint32_t GetGlobalTimerIRQMask(uint32_t id);
	uint32_t GetDirectIRQMask(uint32_t id);
	uint32_t GetSerialIRQMask(uint32_t id);
	
	bool EdgeSensitive(uint32_t in_irq, uint32_t reg_val);
	bool EdgeSensitive(uint32_t reg_val);
	bool LevelSensitive(uint32_t reg_val);

	void CheckInterruptions();
	
	void SetGlobalTimerIRQ(unsigned int id);
	void SetDirectIRQ(unsigned int id);
	void UnsetDirectIRQ(unsigned int id);
	void SetSerialIRQ(unsigned int id);
	void UnsetSerialIRQ(unsigned int id);
	
	void WriteGCR(uint32_t data);
	void WriteEICR(uint32_t data);
	void WriteEVI(uint32_t data); 
	void WritePI(uint32_t data); 
	void WriteSVR(uint32_t data); 
	void WriteTFRR(uint32_t data); 
	void WriteGTVPR(uint32_t data, unsigned int id);
	void WriteGTBCR(uint32_t data, unsigned int id);
	void WriteGTCCR(uint32_t data, unsigned int id); 
	void WriteGTDR(uint32_t data, unsigned int id); 
	void WriteDR(uint32_t data, unsigned int id); 
	void WriteVPR(uint32_t data, unsigned int id);
	void WriteIIVPR(uint32_t data, unsigned int id); 
	void WriteIIDR(uint32_t data, unsigned int id); 
	void WritePCTPR(uint32_t data);
	void WriteIACK(uint32_t data);
	void WriteEOI(uint32_t data);
	
	bool GetMask(uint32_t reg_val);
	bool GetActivity(uint32_t reg_val);
	void SetActivity(uint32_t &reg_val);
	void UnsetActivity(uint32_t &reg_val);
	uint32_t GetVector(uint32_t reg_val);
	uint32_t GetProcessorPriority();
	uint32_t GetPriority(uint32_t reg_val);
	uint32_t GetVPR(uint32_t in_irq);
	uint32_t GetHighestIRQ();
	uint32_t ReadIACK();
	
	/**
	 * Non intrusive IACK read method. Used by the memory service method ReadMemory
	 */
	uint32_t ReadMemoryIACK();
	/*
	 * Non intrusive write methods. Used by the memory service method WriteMemory
	 */
	uint32_t writememory_data;
	uint32_t writememory_address;
	uint32_t writememory_mask;
	static const uint32_t WRITEMEMORY_MASK_BYTE = (uint32_t) 0xff;
	static const uint32_t WRITEMEMORY_MASK_FULL = (uint32_t) 0xffffffff;
	void WriteMemory(PHYSICAL_ADDR addr, uint32_t data);
	void WriteMemoryGCR(uint32_t data);
	void WriteMemoryEICR(uint32_t data);
	void WriteMemoryEVI(uint32_t data); 
	void WriteMemoryPI(uint32_t data); 
	void WriteMemorySVR(uint32_t data); 
	void WriteMemoryTFRR(uint32_t data); 
	void WriteMemoryGTVPR(uint32_t data, unsigned int id);
	void WriteMemoryGTBCR(uint32_t data, unsigned int id);
	void WriteMemoryGTCCR(uint32_t data, unsigned int id); 
	void WriteMemoryGTDR(uint32_t data, unsigned int id); 
	void WriteMemoryDR(uint32_t data, unsigned int id); 
	void WriteMemoryVPR(uint32_t data, unsigned int id);
	void WriteMemoryIIVPR(uint32_t data, unsigned int id); 
	void WriteMemoryIIDR(uint32_t data, unsigned int id); 
	void WriteMemoryPCTPR(uint32_t data);
	void WriteMemoryIACK(uint32_t data);
	void WriteMemoryEOI(uint32_t data);
	
};

} // end of namespace epic
} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CHIPSET_MPC107_EPIC_EPIC_HH__
