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

#include <unisim/util/debug/dwarf/abbrev.hh>
#include <unisim/util/debug/dwarf/addr_range.hh>
#include <unisim/util/debug/dwarf/attr.hh>
#include <unisim/util/debug/dwarf/call_frame_prog.hh>
#include <unisim/util/debug/dwarf/call_frame_vm.hh>
#include <unisim/util/debug/dwarf/cie.hh>
#include <unisim/util/debug/dwarf/cu.hh>
#include <unisim/util/debug/dwarf/die.hh>
#include <unisim/util/debug/dwarf/dwarf.hh>
#include <unisim/util/debug/dwarf/expr_vm.hh>
#include <unisim/util/debug/dwarf/fde.hh>
#include <unisim/util/debug/dwarf/loc.hh>
#include <unisim/util/debug/dwarf/macinfo.hh>
#include <unisim/util/debug/dwarf/pub.hh>
#include <unisim/util/debug/dwarf/range.hh>
#include <unisim/util/debug/dwarf/stmt_prog.hh>
#include <unisim/util/debug/dwarf/stmt_vm.hh>
#include <unisim/util/debug/dwarf/frame.hh>
#include <unisim/util/debug/dwarf/data_object.hh>
#include <unisim/util/debug/dwarf/subprogram.hh>

#include <unisim/util/debug/dwarf/addr_range.tcc>
#include <unisim/util/debug/dwarf/attr.tcc>
#include <unisim/util/debug/dwarf/call_frame_prog.tcc>
#include <unisim/util/debug/dwarf/call_frame_vm.tcc>
#include <unisim/util/debug/dwarf/cie.tcc>
#include <unisim/util/debug/dwarf/cu.tcc>
#include <unisim/util/debug/dwarf/die.tcc>
#include <unisim/util/debug/dwarf/dwarf.tcc>
#include <unisim/util/debug/dwarf/expr_vm.tcc>
#include <unisim/util/debug/dwarf/fde.tcc>
#include <unisim/util/debug/dwarf/loc.tcc>
#include <unisim/util/debug/dwarf/macinfo.tcc>
#include <unisim/util/debug/dwarf/pub.tcc>
#include <unisim/util/debug/dwarf/range.tcc>
#include <unisim/util/debug/dwarf/stmt_prog.tcc>
#include <unisim/util/debug/dwarf/stmt_vm.tcc>
#include <unisim/util/debug/dwarf/frame.tcc>
#include <unisim/util/debug/dwarf/data_object.tcc>
#include <unisim/util/debug/dwarf/subprogram.tcc>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template class DWARF_StatementProgram<uint64_t>;
template class DWARF_StatementVM<uint64_t>;
template class DWARF_AttributeValue<uint64_t>;
template class DWARF_Address<uint64_t>;
template class DWARF_Block<uint64_t>;
template class DWARF_UnsignedConstant<uint64_t>;
template class DWARF_SignedConstant<uint64_t>;
template class DWARF_UnsignedLEB128Constant<uint64_t>;
template class DWARF_SignedLEB128Constant<uint64_t>;
template class DWARF_Flag<uint64_t>;
template class DWARF_LinePtr<uint64_t>;
template class DWARF_LocListPtr<uint64_t>;
template class DWARF_MacPtr<uint64_t>;
template class DWARF_RangeListPtr<uint64_t>;
template class DWARF_Reference<uint64_t>;
template class DWARF_String<uint64_t>;
template class DWARF_Expression<uint64_t>;
template class DWARF_Attribute<uint64_t>;
template class DWARF_DIE<uint64_t>;
template class DWARF_CompilationUnit<uint64_t>;
template class DWARF_ExpressionVM<uint64_t>;
template class DWARF_CallFrameProgram<uint64_t>;
template class DWARF_CIE<uint64_t>;
template class DWARF_FDE<uint64_t>;
template class DWARF_RegisterRule<uint64_t>;
template class DWARF_CFARule<uint64_t>;
template class DWARF_RegisterRuleUndefined<uint64_t>;
template class DWARF_RegisterRuleSameValue<uint64_t>;
template class DWARF_RegisterRuleOffset<uint64_t>;
template class DWARF_RegisterRuleValOffset<uint64_t>;
template class DWARF_RegisterRuleRegister<uint64_t>;
template class DWARF_RegisterRuleExpression<uint64_t>;
template class DWARF_RegisterRuleValExpression<uint64_t>;
template class DWARF_CFIRow<uint64_t>;
template class DWARF_CFI<uint64_t>;
template class DWARF_CallFrameVM<uint64_t>;
template class DWARF_Frame<uint64_t>;
template class DWARF_RangeListEntry<uint64_t>;
template class DWARF_MacInfoListEntry<uint64_t>;
template class DWARF_MacInfoListEntryDefine<uint64_t>;
template class DWARF_MacInfoListEntryUndef<uint64_t>;
template class DWARF_MacInfoListEntryStartFile<uint64_t>;
template class DWARF_MacInfoListEntryEndFile<uint64_t>;
template class DWARF_MacInfoListEntryVendorExtension<uint64_t>;
template class DWARF_AddressRangeDescriptor<uint64_t>;
template class DWARF_AddressRanges<uint64_t>;
template class DWARF_Pub<uint64_t>;
template class DWARF_Pubs<uint64_t>;
template class DWARF_LocListEntry<uint64_t>;
template class DWARF_Handler<uint64_t>;
template class DWARF_DataObject<uint64_t>;

template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_AddressRangeDescriptor<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_AddressRanges<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_AttributeValue<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_Attribute<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_CallFrameProgram<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_CFARule<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_RegisterRule<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_CFIRow<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_CFI<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_CIE<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_CompilationUnit<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_DIE<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_FDE<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_RegSet<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_LocListEntry<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_MacInfoListEntry<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_Pub<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_Pubs<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_RangeListEntry<uint64_t>&);
template std::ostream& operator << <uint64_t>(std::ostream&, const DWARF_StatementProgram<uint64_t>&);

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
