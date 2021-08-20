/*
 *  Copyright (c) 2007-2017,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_DMEM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_DMEM_HH__

#include <inttypes.h>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

template <typename TYPES, typename CONFIG>
struct DMEM
	: unisim::kernel::Object
	, unisim::util::cache::LocalMemory<TYPES, CONFIG, DMEM<TYPES, CONFIG> >
{
	typedef unisim::util::cache::LocalMemory<TYPES, CONFIG, DMEM<TYPES, CONFIG> > SuperLocalMemory;
	typedef typename CONFIG::CPU CPU;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	
	typedef typename CPU::DMEMCTL1 DMEMCTL1;
	
	static const char *GetLocalMemoryName() { return "DMEM"; }
	
	inline PHYSICAL_ADDRESS GetBasePhysicalAddress() const ALWAYS_INLINE { return cur_base_addr; }
	
	inline unsigned int GetSize() const ALWAYS_INLINE { return size; }

	bool IsVerbose() const ALWAYS_INLINE { return verbose; }

	DMEM(CPU *_cpu)
		: unisim::kernel::Object("DMEM", _cpu, "Data local memory")
		, cpu(_cpu)
		, dmemcfg0(_cpu, this)
		, dmemctl0(_cpu, this)
		, dmemctl1(_cpu)
		, base_addr(CONFIG::BASE_ADDRESS)
		, param_base_addr("base-addr", this, base_addr, "base address at reset")
		, cur_base_addr(base_addr)
		, size(CONFIG::SIZE)
		, param_size("size", this, size)
		, verbose(false)
		, param_verbose("verbose", this, verbose, "enable/disable verbosity")
		, stat_num_load_accesses("num-load-accesses", this, this->num_load_accesses, "number of load accesses")
		, stat_num_store_accesses("num-store-accesses", this, this->num_store_accesses, "number of store accesses")
	{
		param_base_addr.SetMutable(false);
		param_size.SetMutable(false);
		param_size.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
		
		std::stringstream param_size_description_sstr;
		param_size_description_sstr << "size in bytes (at most " << CONFIG::SIZE << " bytes)";
		param_size.SetDescription(param_size_description_sstr.str().c_str());
	}

	// DMEM Configuration Register 0
	struct DMEMCFG0 : CPU::DMEMCFG0
	{
		typedef typename CPU::DMEMCFG0 Super;
		
		DMEMCFG0(CPU *_cpu, DMEM *_dmem) : Super(_cpu), dmem(_dmem) {}
		DMEMCFG0(CPU *_cpu, DMEM *_dmem, uint32_t _value) : Super(_cpu, _value), dmem(_dmem) {}
		
		virtual void Reset()
		{
			this->template Set<typename DMEMCFG0::DMEM_BASE_ADDR>(DMEMCFG0::DMEM_BASE_ADDR::Get(dmem->cur_base_addr));
			this->template Set<typename DMEMCFG0::DECUA>(1);
			this->template Set<typename DMEMCFG0::DECA>(1);
			this->template Set<typename DMEMCFG0::DMSIZE>(CONFIG::SIZE / 4096);
		}
	private:
		DMEM *dmem;
	};

	// DMEM Control Register 0
	struct DMEMCTL0 : CPU::DMEMCTL0
	{
		typedef typename CPU::DMEMCTL0 Super;
		
		DMEMCTL0(CPU *_cpu, DMEM *_dmem) : Super(_cpu), dmem(_dmem) {}
		DMEMCTL0(CPU *_cpu, DMEM *_dmem, uint32_t _value) : Super(_cpu, _value), dmem(_dmem) {}
		
		void Effect()
		{
			if(this->template Get<typename DMEMCTL0::DBAPD>())
			{
				dmem->cur_base_addr = 0; // clear lower bits
				DMEMCTL0::DMEM_BASE_ADDR::template Set<PHYSICAL_ADDRESS>(dmem->cur_base_addr, this->template Get<typename DMEMCTL0::DMEM_BASE_ADDR>()); // copy higher bits
			}
			else
			{
				dmem->cur_base_addr = dmem->base_addr;
			}
			
			dmem->dmemcfg0.Reset(); // regenerate DMEMCFG0 read-only register
		}
		
		virtual void Reset()
		{
			this->template Set<typename DMEMCTL0::DMEM_BASE_ADDR>(0);
			this->template Set<typename DMEMCTL0::DBYPCB        >(0);
			this->template Set<typename DMEMCTL0::DBYPAT        >(0);
			this->template Set<typename DMEMCTL0::DBYPDEC       >(1);
			this->template Set<typename DMEMCTL0::DECUE         >(0);
			this->template Set<typename DMEMCTL0::DBAPD         >(0);
			this->template Set<typename DMEMCTL0::DPEIE         >(0);
			this->template Set<typename DMEMCTL0::DICWE         >(0);
			this->template Set<typename DMEMCTL0::DSWCE         >(3);
			this->template Set<typename DMEMCTL0::DDAUEC        >(0);
			this->template Set<typename DMEMCTL0::DCPECE        >(1);
			this->template Set<typename DMEMCTL0::DSECE         >(1);
			
			Effect();
		}
		
		DMEMCTL0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	private:
		DMEM *dmem;
	};

private:
	CPU *cpu;
	DMEMCFG0 dmemcfg0;
	DMEMCTL0 dmemctl0;
	DMEMCTL1 dmemctl1;

	PHYSICAL_ADDRESS base_addr;
	unisim::kernel::variable::Parameter<PHYSICAL_ADDRESS> param_base_addr;
	PHYSICAL_ADDRESS cur_base_addr;
	unsigned int size;
	unisim::kernel::variable::Parameter<unsigned int> param_size;
	bool verbose;
	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_load_accesses;
	unisim::kernel::variable::Statistic<uint64_t> stat_num_store_accesses;
};

} // end of namespace unisim
} // end of namespace component
} // end of namespace cxx
} // end of namespace processor
} // end of namespace powerpc
} // end of namespace e200
} // end of namespace mpc57xx

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_DMEM_HH__
