/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_INTERFACE_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_INTERFACE_TCC

#include <unisim/component/cxx/processor/tesla/register_interface.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

// VectorRegisterInterface

template<class CONFIG>
VectorRegisterInterface<CONFIG>::VectorRegisterInterface(char const * name,
	unsigned int reg, CPU<CONFIG> & cpu) :
	name(name), cpu(cpu), reg(reg)
{
}

template<class CONFIG>
VectorRegisterInterface<CONFIG>::~VectorRegisterInterface()
{
}

template<class CONFIG>
char const * VectorRegisterInterface<CONFIG>::GetName() const
{
	return name.c_str();
}

template<class CONFIG>
void VectorRegisterInterface<CONFIG>::GetValue(void * buffer) const
{
	// Can't cache the reference, its target depends on the context (current warp)
	VectorRegister<CONFIG> const & value = cpu.GetGPR(reg);
	uint32_t * tbuffer = static_cast<uint32_t*>(buffer);
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		tbuffer[i] = value[i];
	}
}

template<class CONFIG>
void VectorRegisterInterface<CONFIG>::SetValue(void const * buffer)
{
	VectorRegister<CONFIG> & value = cpu.GetGPR(reg);
	uint32_t const * tbuffer = static_cast<uint32_t const *>(buffer);
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		value[i] = tbuffer[i];
	}
}

template<class CONFIG>
int VectorRegisterInterface<CONFIG>::GetSize() const
{
	return 4 * CONFIG::WARP_SIZE;
}

// ScalarRegisterInterface

template<class CONFIG>
ScalarRegisterInterface<CONFIG>::ScalarRegisterInterface(char const * name,
	unsigned int reg, CPU<CONFIG> & cpu) :
	name(name), cpu(cpu), reg(reg)
{
}

template<class CONFIG>
ScalarRegisterInterface<CONFIG>::~ScalarRegisterInterface()
{
}

template<class CONFIG>
char const * ScalarRegisterInterface<CONFIG>::GetName() const
{
	return name.c_str();
}

template<class CONFIG>
void ScalarRegisterInterface<CONFIG>::GetValue(void * buffer) const
{
	// Can't cache the reference, its target depends on the context (current thread)
	uint32_t * tbuffer = static_cast<uint32_t*>(buffer);
	*tbuffer = cpu.GetScalar(reg);
}

template<class CONFIG>
void ScalarRegisterInterface<CONFIG>::SetValue(void const * buffer)
{
	uint32_t const * tbuffer = static_cast<uint32_t const *>(buffer);
	cpu.GetScalar(reg) = *tbuffer;
}

template<class CONFIG>
int ScalarRegisterInterface<CONFIG>::GetSize() const
{
	return 4;
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
