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
 
#ifndef __AV32UC3C_CONFIG_HH__
#define __AV32UC3C_CONFIG_HH__

#include <unisim/component/cxx/processor/avr32/avr32a/avr32uc/config.hh>
#include <unisim/component/tlm2/interconnect/generic_router/config.hh>

//=========================================================================
//===         Memory Router compile time configuration                  ===
//=========================================================================

class MemoryRouterConfig : public unisim::component::tlm2::interconnect::generic_router::Config
{
public:
	typedef uint32_t ADDRESS;
	static const unsigned int INPUT_SOCKETS = 2;
	static const unsigned int OUTPUT_SOCKETS = 1;
	static const unsigned int MAX_NUM_MAPPINGS = 1;
	static const unsigned int BUSWIDTH = 32;
};

class MemoryRouterDebugConfig : public MemoryRouterConfig
{
public:
		static const bool VERBOSE = true;
};

//=========================================================================
//===                   Simulator config                                ===
//=========================================================================

class SimConfig
{
public:
	typedef uint64_t MEMORY_ADDR;
	static const bool DEBUG_INFORMATION = false;

	//=========================================================================
	//===               AVR32UC compile time configuration                  ===
	//=========================================================================

	typedef unisim::component::cxx::processor::avr32::avr32a::avr32uc::Config CPU_CONFIG;

	//=========================================================================
	//===           Memory router compile time configuration                ===
	//=========================================================================

	typedef MemoryRouterConfig MEMORY_ROUTER_CONFIG;

	//=========================================================================
	//===                 RAM compile time configuration                    ===
	//=========================================================================

	static const MEMORY_ADDR RAM_BASE_ADDR = 0x00000000ULL;
	static const MEMORY_ADDR RAM_BYTE_SIZE = 256 * 1024 * 1024; // 256 MB

};

//=========================================================================
//===                    Simulator config (Debug)                       ===
//=========================================================================

class SimConfigDebug : public SimConfig
{
public:
	static const bool DEBUG_INFORMATION = true;

	//=========================================================================
	//===               AVR32UC compile time configuration                  ===
	//=========================================================================

	typedef unisim::component::cxx::processor::avr32::avr32a::avr32uc::DebugConfig CPU_CONFIG;

	//=========================================================================
	//===           Memory router compile time configuration                ===
	//=========================================================================

	typedef MemoryRouterDebugConfig MEMORY_ROUTER_CONFIG;
};

#endif
