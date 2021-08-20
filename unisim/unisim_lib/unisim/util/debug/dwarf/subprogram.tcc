/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_TCC__

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_SubProgram<MEMORY_ADDR>::DWARF_SubProgram()
	: name()
	, return_type(0)
	, formal_params()
{
}

template <class MEMORY_ADDR>
DWARF_SubProgram<MEMORY_ADDR>::~DWARF_SubProgram()
{
	unsigned int arity = formal_params.size();
	unsigned int idx;
	
	for(idx = 0; idx < arity; idx++)
	{
		delete formal_params[idx];
	}
}

template <class MEMORY_ADDR>
void DWARF_SubProgram<MEMORY_ADDR>::SetReturnType(const Type *type)
{
	if(return_type) delete return_type;
	return_type = type;
}

template <class MEMORY_ADDR>
void DWARF_SubProgram<MEMORY_ADDR>::AddFormalParameter(const FormalParameter *formal_param)
{
	formal_params.push_back(formal_param);
}

template <class MEMORY_ADDR>
const char *DWARF_SubProgram<MEMORY_ADDR>::GetName() const
{
	return name.c_str();
}

template <class MEMORY_ADDR>
const Type *DWARF_SubProgram<MEMORY_ADDR>::GetReturnType() const
{
	return return_type;
}

template <class MEMORY_ADDR>
unsigned int DWARF_SubProgram<MEMORY_ADDR>::GetArity() const
{
	return formal_params.size();
}

template <class MEMORY_ADDR>
const FormalParameter *DWARF_SubProgram<MEMORY_ADDR>::GetFormalParameter(unsigned int idx) const
{
	if(idx >= formal_params.size()) return 0;
	return formal_params[idx];
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_SUBPROGRAM_TCC__
