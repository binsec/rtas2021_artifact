/*
 *  Copyright (c) 2017,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_LSP_LSP_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_LSP_LSP_HH__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

enum LSP_HS_EVEN_ODD
{
	LSP_HS_EVEN_ODD_00 = 0, // 00=even     -> "e"
	LSP_HS_EVEN_ODD_01 = 1, // 01=even/odd -> "eo"
	LSP_HS_EVEN_ODD_10 = 2, // 10=odd      -> "o"
	LSP_HS_EVEN_ODD_11 = 3  // 11=unused/unspecified
};

enum LSP_HS_UPPER_LOWER_EXCHANGED
{
	LSP_HS_UPPER_LOWER_EXCHANGED_00 = 0, // 00=upper/lower     -> "ul"
	LSP_HS_UPPER_LOWER_EXCHANGED_01 = 1, // 01=lower/lower     -> "ll"
	LSP_HS_UPPER_LOWER_EXCHANGED_10 = 2, // 10=upper/upper     -> "uu"
	LSP_HS_UPPER_LOWER_EXCHANGED_11 = 3  // 11=exchanged/lower -> "xl"
};

enum LSP_TY
{
	LSP_TY_00 = 0, // 00=unsigned integer           -> "ui"
	LSP_TY_01 = 1, // 01=signed integer             -> "si"
	LSP_TY_10 = 2, // 10=signed by unsigned integer -> "sui"
	LSP_TY_11 = 3  // 11=signed modulo fractional   -> "smf"
};

enum LSP_ACC
{
	LSP_ACC_00 = 0, // 00=no accumulate
	LSP_ACC_01 = 1, // 01=accumulate                   -> "aa"
	LSP_ACC_10 = 2, // 10=accumulate negative          -> "an"
	LSP_ACC_11 = 3, // 11=accumulate negative/positive -> "anp"
};

inline std::ostream& operator << (std::ostream& os, const LSP_HS_EVEN_ODD& hs)
{
	switch(hs)
	{
		case LSP_HS_EVEN_ODD_00: os << "e"; break;
		case LSP_HS_EVEN_ODD_01: os << "eo"; break;
		case LSP_HS_EVEN_ODD_10: os << "o"; break;
		case LSP_HS_EVEN_ODD_11: os << "?"; break;
		default: os << "?"; break;
	}
	return os;
}


inline std::ostream& operator << (std::ostream& os, const LSP_HS_UPPER_LOWER_EXCHANGED& hs)
{
	switch(hs)
	{
		case LSP_HS_UPPER_LOWER_EXCHANGED_00: os << "ul"; break;
		case LSP_HS_UPPER_LOWER_EXCHANGED_01: os << "ll"; break;
		case LSP_HS_UPPER_LOWER_EXCHANGED_10: os << "uu"; break;
		case LSP_HS_UPPER_LOWER_EXCHANGED_11: os << "xl"; break;
		default: os << "?"; break;
	}
	return os;
}

inline std::ostream& operator << (std::ostream& os, const LSP_TY& ty)
{
	switch(ty)
	{
		case LSP_TY_00: os << "ui"; break;
		case LSP_TY_01: os << "si"; break;
		case LSP_TY_10: os << "sui"; break;
		case LSP_TY_11: os << "smf"; break;
		default: os << "?"; break;
	}
	return os;
}

inline std::ostream& operator << (std::ostream& os, const LSP_ACC& acc)
{
	switch(acc)
	{
		case LSP_ACC_00: os << ""; break;
		case LSP_ACC_01: os << "a"; break;
		case LSP_ACC_10: os << "an"; break;
		case LSP_ACC_11: os << "anp"; break;
		default: os << "?"; break;
	}
	return os;
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
