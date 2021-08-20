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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CLASS_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_CLASS_HH__

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

const unsigned int DW_CLASS_ADDRESS = 1;                   // DWARF_Address (machine address)
const unsigned int DW_CLASS_BLOCK = 2;                     // DWARF_Block (raw data block)
const unsigned int DW_CLASS_EXPRESSION = 3;                // DWARF_Expression (DWARF Expression)
const unsigned int DW_CLASS_UNSIGNED_CONSTANT = 4;         // DWARF_UnsignedConstant (unsigned integer up to 64-bit)
const unsigned int DW_CLASS_SIGNED_CONSTANT = 8;           // DWARF_SignedConstant (signed integer up to 64-bit)
const unsigned int DW_CLASS_UNSIGNED_LEB128_CONSTANT = 16; // DWARF_UnsignedLEB128Constant (variable length unsigned integer)
const unsigned int DW_CLASS_SIGNED_LEB128_CONSTANT = 32;   // DWARF_SignedLEB128Constant (variable length signed integer)
const unsigned int DW_CLASS_CONSTANT = DW_CLASS_UNSIGNED_CONSTANT | DW_CLASS_SIGNED_CONSTANT | DW_CLASS_UNSIGNED_LEB128_CONSTANT | DW_CLASS_SIGNED_LEB128_CONSTANT;
const unsigned int DW_CLASS_FLAG = 64;                     // DWARF_Flag (boolean)
const unsigned int DW_CLASS_LINEPTR = 65;                  // DWARF_LinePtr (reference to a statement program)
const unsigned int DW_CLASS_LOCLISTPTR = 66;               // DWARF_LocListPtr
const unsigned int DW_CLASS_MACPTR = 67;                   // DWARF_MacPtr
const unsigned int DW_CLASS_RANGELISTPTR = 128;            // DWARF_RangeListPtr
const unsigned int DW_CLASS_REFERENCE = 129;               // DWARF_Reference (reference to a DWARF_DIE)
const unsigned int DW_CLASS_STRING = 130;                  // DWARF_String (C string)

const char *DWARF_GetCLASSName(unsigned int dw_class);

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
