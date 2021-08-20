/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */


#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CONFIG_HH__

#ifdef GCC_INLINE
#undef GCC_INLINE
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define GCC_INLINE __attribute__((always_inline))
#else
#define GCC_INLINE
#endif

#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <inttypes.h>

// ToRemove: Used Tempory for development purposes to force MC9S12XDP512
#define MC9S12XDP512
// *** end to remove ***


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

struct CONFIG {
	/*
	 * static initialization may rise problems in SMP architectures !!!
	 */
	static const bool DEBUG_EXCEPTION_ENABLE		= false;

	static const bool HAS_RESET						= true;
	static const bool HAS_NON_MASKABLE_XIRQ_INTERRUPT	= true;
	static const bool HAS_NON_MASKABLE_ACCESS_ERROR_INTERRUPT = true;
	static const bool HAS_MASKABLE_IBIT_INTERRUPT	= true;
	static const bool HAS_NONMASKABLE_SWI_INTERRUPT	= true;
	static const bool HAS_TRAP_INTERRUPT			= true;
	static const bool HAS_SYSCALL_INTERRUPT			= true;
	static const bool HAS_SPURIOUS_INTERRUPT		= true;

	static const clock_t MINIMAL_BUS_CLOCK_TIME		= 250000;	// Time unit is PS.  250000 PS => 4 MHz

	//===========================================================
	//=            PWM external configuration and constants     =
	//===========================================================

	static const unsigned int UNISIM2EXTERNAL_BUS_WIDTH = 32; // in bits (unused in this example)
	static const unsigned int EXTERNAL2UNISIM_BUS_WIDTH = 32; // in bits (unused in this example)

};


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CONFIG_HH__

