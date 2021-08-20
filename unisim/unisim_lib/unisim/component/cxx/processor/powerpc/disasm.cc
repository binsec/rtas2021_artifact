/*
 *  Copyright (c) 2007-2018,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

/***************************************/
/*** Convenience disassembly methods ***/
/***************************************/

#include <unisim/component/cxx/processor/powerpc/disasm.hh>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

	std::ostream& operator << (std::ostream& sink, DASMPrint const& dap)
	{
		dap.Print(sink);
		return sink;
	}

	void GPRPrint::Print( std::ostream& sink ) const { sink << "r" << std::dec << reg; }
	void FPRPrint::Print( std::ostream& sink ) const { sink << "f" << std::dec << reg; }
	void HexPrint::Print( std::ostream& sink ) const { sink << "0x" << std::hex << num; }
	void CRPrint::Print( std::ostream& sink ) const { sink << "cr" << std::dec << reg; }
	void CondPrint::Print( std::ostream& sink ) const
	{
		char const* condnames[] = {"ge","le","ne","ns","lt","gt","eq","so"};
		char const* condname = condnames[(crb&3)|(expect?4:0)];
		if (crb >= 4)
			sink << "4*" << CRPrint(crb>>2) << '+';
		sink << condname;
	}
	void EAPrint::Print( std::ostream& sink ) const
	{
		sink << std::dec << idx << '(';
		if (reg)
			sink << GPRPrint(reg);
		else
			sink << '0';
		sink << ')';
	}

} /* end of namespace powerpc */
} /* end of namespace processor */
} /* end of namespace cxx */
} /* end of namespace component */
} /* end of namespace unisim */
