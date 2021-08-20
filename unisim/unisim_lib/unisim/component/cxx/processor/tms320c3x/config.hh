/*
 *  Copyright (c) 2009,
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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CONFIG_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320c3x {

typedef enum {
	TMS320C30 = 0,
	TMS320C31,
	TMS320C32,
	TMS320VC33
} Model;

class TMS320C3XBase {
public:
	typedef uint32_t address_t;
	typedef uint32_t insn_t;
	static const uint32_t FSB_BURST_SIZE = 32;
};

class TMS320VC33_Config :
	public TMS320C3XBase {
public:
	static const Model MODEL = TMS320VC33;

	static const bool ENABLE_INSN_CACHE = true;
	static const uint32_t INSN_CACHE_SIZE = 64;            // 64 32-bit instructions
	static const uint32_t INSN_CACHE_BLOCKS_PER_LINE = 32;  // 32 32-bit instructions
	static const uint32_t INSN_CACHE_ASSOCIATIVITY = 2;    // 2-way set associative
};

} // end of namespace tms320c3x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
 
#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_TMS320_CONFIG_HH__
