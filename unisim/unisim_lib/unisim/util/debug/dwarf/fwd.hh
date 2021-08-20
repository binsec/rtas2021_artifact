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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_FWD_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_FWD_HH__

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

class DWARF_LEB128;
class DWARF_Filename;
template <class MEMORY_ADDR> class DWARF_StatementProgram;
template <class MEMORY_ADDR> class DWARF_StatementVM;
template <class MEMORY_ADDR> class DWARF_AttributeValue;
template <class MEMORY_ADDR> class DWARF_Address;
template <class MEMORY_ADDR> class DWARF_Block;
template <class MEMORY_ADDR> class DWARF_UnsignedConstant;
template <class MEMORY_ADDR> class DWARF_SignedConstant;
template <class MEMORY_ADDR> class DWARF_UnsignedLEB128Constant;
template <class MEMORY_ADDR> class DWARF_SignedLEB128Constant;
template <class MEMORY_ADDR> class DWARF_Flag;
template <class MEMORY_ADDR> class DWARF_LinePtr;
template <class MEMORY_ADDR> class DWARF_LocListPtr;
template <class MEMORY_ADDR> class DWARF_MacPtr;
template <class MEMORY_ADDR> class DWARF_RangeListPtr;
template <class MEMORY_ADDR> class DWARF_Reference;
template <class MEMORY_ADDR> class DWARF_String;
template <class MEMORY_ADDR> class DWARF_Expression;
template <class MEMORY_ADDR> class DWARF_Attribute;
template <class MEMORY_ADDR> class DWARF_DIE;
template <class MEMORY_ADDR> class DWARF_CompilationUnit;
class DWARF_Abbrev;
class DWARF_AbbrevAttribute;
template <class MEMORY_ADDR> class DWARF_Location;
template <class MEMORY_ADDR> class DWARF_ExpressionVM;
template <class MEMORY_ADDR> class DWARF_CallFrameProgram;
template <class MEMORY_ADDR> class DWARF_CIE;
template <class MEMORY_ADDR> class DWARF_FDE;
template <class MEMORY_ADDR> class DWARF_RegisterRule;
template <class MEMORY_ADDR> class DWARF_CFARule;
template <class MEMORY_ADDR> class DWARF_RegisterRuleOffset;
template <class MEMORY_ADDR> class DWARF_RegisterRuleValOffset;
template <class MEMORY_ADDR> class DWARF_RegisterRuleRegister;
template <class MEMORY_ADDR> class DWARF_RegisterRuleExpression;
template <class MEMORY_ADDR> class DWARF_RegisterRuleValExpression;
template <class MEMORY_ADDR> class DWARF_CFIRow;
template <class MEMORY_ADDR> class DWARF_CFI;
template <class MEMORY_ADDR> class DWARF_CallFrameVM;
class DWARF_RegisterNumberMapping;
template <class MEMORY_ADDR> class DWARF_RegSet;
template <class MEMORY_ADDR> class DWARF_Frame;
template <class MEMORY_ADDR> class DWARF_RangeListEntry;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntry;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryDefine;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryUndef;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryStartFile;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryEndFile;
template <class MEMORY_ADDR> class DWARF_MacInfoListEntryVendorExtension;
template <class MEMORY_ADDR> class DWARF_AddressRangeDescriptor;
template <class MEMORY_ADDR> class DWARF_AddressRanges;
template <class MEMORY_ADDR> class DWARF_Pub;
template <class MEMORY_ADDR> class DWARF_Pubs;
template <class MEMORY_ADDR> class DWARF_LocListEntry;
template <class MEMORY_ADDR> class DWARF_Handler;
template <class MEMORY_ADDR> class DWARF_DataObject;
template <class MEMORY_ADDR> class DWARF_SubProgram;

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
