/*
 *  Copyright (c) 2010,
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

#include <unisim/util/debug/dwarf/class.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

const char *DWARF_GetCLASSName(unsigned int dw_class)
{
	switch(dw_class)
	{
		case DW_CLASS_ADDRESS: return "address";
		case DW_CLASS_BLOCK: return "block";
		case DW_CLASS_EXPRESSION: return "expression";
		case DW_CLASS_UNSIGNED_CONSTANT:
		case DW_CLASS_UNSIGNED_LEB128_CONSTANT: return "unsigned constant";
		case DW_CLASS_SIGNED_CONSTANT:
		case DW_CLASS_SIGNED_LEB128_CONSTANT:  return "signed constant";
		case DW_CLASS_FLAG: return "flag";
		case DW_CLASS_LINEPTR: return "lineptr";
		case DW_CLASS_LOCLISTPTR: return "loclistptr";
		case DW_CLASS_MACPTR: return "macptr";
		case DW_CLASS_RANGELISTPTR: return "rangelistptr";
		case DW_CLASS_REFERENCE: return "reference";
		case DW_CLASS_STRING: return "string";
	}
	return "";
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
