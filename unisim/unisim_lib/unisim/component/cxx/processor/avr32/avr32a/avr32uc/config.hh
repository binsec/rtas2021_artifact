/*
 *  Copyright (c) 2014,
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
 * Authors: Julien Lisita (julien.lisita@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_AVR32_AVR32A_AVR32UC_CONFIG_HH__

#include "unisim/component/cxx/processor/avr32/avr32a/config.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace avr32 {
namespace avr32a {
namespace avr32uc {

template <class CONFIG>
class CPU; // Forward declaration

//=====================================================================
//=                  AVR32UC common configuration                     =
//=====================================================================

class Config : public unisim::component::cxx::processor::avr32::avr32a::Config
{
public:
	typedef CPU<Config> STATE;
	
	static const unsigned int HSB_WIDTH = 4;
	static const unsigned int HSB_BURST_SIZE = 4;
	
	static const bool DEBUG_ENABLE = false;
	static const bool DEBUG_SETUP_ENABLE = false;
	static const bool DEBUG_INTERRUPT_ENABLE = false;
	static const bool DEBUG_STEP_ENABLE = false;
};

class DebugConfig : public Config
{
public:
	typedef CPU<DebugConfig> STATE;

	static const bool DEBUG_ENABLE = true;
	static const bool DEBUG_SETUP_ENABLE = true;
	static const bool DEBUG_INTERRUPT_ENABLE = true;
	static const bool DEBUG_STEP_ENABLE = true;
	
	static const bool DISAMBIGUATE_DISASM = true;
};

} // end of namespace avr32uc
} // end of namespace avr32a
} // end of namespace avr32
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
