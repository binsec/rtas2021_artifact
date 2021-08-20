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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_IMEM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_IMEM_HH__

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
struct IMEM
	: unisim::kernel::Object
	, unisim::util::cache::LocalMemory<TYPES, CONFIG, IMEM<TYPES, CONFIG> >
{
	typedef typename unisim::util::cache::LocalMemory<TYPES, CONFIG, IMEM<TYPES, CONFIG> > SuperLocalMemory;
	typedef typename CONFIG::CPU CPU;
	typedef typename TYPES::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	
	typedef typename CPU::IMEMCTL1 IMEMCTL1;
	
	static const char *GetLocalMemoryName() { return "IMEM"; }
	
	inline PHYSICAL_ADDRESS GetBasePhysicalAddress() const ALWAYS_INLINE { return cur_base_addr; }
	
	inline unsigned int GetSize() const ALWAYS_INLINE { return size; }

	bool IsVerbose() const ALWAYS_INLINE { return verbose; }

	IMEM(CPU *_cpu)
		: unisim::kernel::Object("IMEM", _cpu, "Instruction local memory")
		, cpu(_cpu)
		, imemcfg0(_cpu, this)
		, imemctl0(_cpu, this)
		, imemctl1(_cpu)
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
	
	// IMEM Configuration Register 0
	struct IMEMCFG0 : CPU::IMEMCFG0
	{
		typedef typename CPU::IMEMCFG0 Super;
		
		IMEMCFG0(CPU *_cpu, IMEM *_imem) : Super(_cpu), imem(_imem) {}
		IMEMCFG0(CPU *_cpu, IMEM *_imem, uint32_t _value) : Super(_cpu, _value), imem(_imem) {}
		
		virtual void Reset()
		{
			this->template Set<typename IMEMCFG0::IMEM_BASE_ADDR>(IMEMCFG0::IMEM_BASE_ADDR::Get(imem->cur_base_addr));
			this->template Set<typename IMEMCFG0::IECUA>(1);
			this->template Set<typename IMEMCFG0::IECA>(1);
			this->template Set<typename IMEMCFG0::IMSIZE>(CONFIG::SIZE / 4096);
		}
	private:
		IMEM *imem;
	};
	
	// IMEM Control Register 0
	struct IMEMCTL0 : CPU::IMEMCTL0
	{
		typedef typename CPU::IMEMCTL0 Super;
		
		IMEMCTL0(CPU *_cpu, IMEM *_imem) : Super(_cpu), imem(_imem) {}
		IMEMCTL0(CPU *_cpu, IMEM *_imem, uint32_t _value) : Super(_cpu, _value), imem(_imem) {}
		
		void Effect()
		{
			if(this->template Get<typename IMEMCTL0::IBAPD>())
			{
				imem->cur_base_addr = 0; // clear lower bits
				IMEMCTL0::IMEM_BASE_ADDR::template Set<PHYSICAL_ADDRESS>(imem->cur_base_addr, this->template Get<typename IMEMCTL0::IMEM_BASE_ADDR>()); // copy higher bits
			}
			else
			{
				imem->cur_base_addr = imem->base_addr;
			}
			
			imem->imemcfg0.Reset(); // regenerate IMEMCFG0 read-only register
		}
		
		virtual void Reset()
		{
			this->template Set<typename IMEMCTL0::IMEM_BASE_ADDR>(0);
			this->template Set<typename IMEMCTL0::IECUE         >(0);
			this->template Set<typename IMEMCTL0::IBAPD         >(0);
			this->template Set<typename IMEMCTL0::ISWCE         >(3);
			this->template Set<typename IMEMCTL0::IDAUEC        >(0);
			this->template Set<typename IMEMCTL0::ICPECE        >(1);
			this->template Set<typename IMEMCTL0::ISECE         >(1);
			
			Effect();
		}
		
		IMEMCTL0& operator = (const uint32_t& value) { this->Super::operator = (value); Effect(); return *this; }
		virtual bool MoveTo(uint32_t value) { if(!this->Super::MoveTo(value)) return false; Effect(); return true; }
	private:
		IMEM *imem;
	};

private:
	CPU *cpu;
	IMEMCFG0 imemcfg0;
	IMEMCTL0 imemctl0;
	IMEMCTL1 imemctl1;

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

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200_MPC57XX_IMEM_HH__
