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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_DIE_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_DIE_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/stmt.hh>
#include <unisim/util/debug/data_object.hh>
#include <unisim/util/debug/subprogram.hh>
#include <list>
#include <vector>
#include <set>
#include <string>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_DIE<MEMORY_ADDR>& dw_die);

// - DW_TAG_access_declaration
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_array_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_bit_stride : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_ordering : DW_CLASS_CONSTANT = DW_ORD_row_major | DW_ORD_col_major
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_specification : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_base_type
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_binary_scale : DW_CLASS_CONSTANT
//   - DW_AT_bit_offset : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_bit_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_decimal_scale : DW_CLASS_CONSTANT
//   - DW_AT_decimal_sign : DW_CLASS_CONSTANT = DW_DS_unsigned | DW_DS_leading_overpunch | DW_DS_trailing_overpunch | DW_DS_leading_separate | DW_DS_trailing_separate
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_digit_count : DW_CLASS_CONSTANT
//   - DW_AT_encoding : DW_CLASS_CONSTANT = DW_ATE_address | DW_ATE_boolean | DW_ATE_complex_float | DW_ATE_float | DW_ATE_signed | DW_ATE_signed_char | DW_ATE_unsigned | DW_ATE_unsigned_char | DW_ATE_imaginary_float | DW_ATE_packed_decimal | DW_ATE_numeric_string | DW_ATE_edited | DW_ATE_signed_fixed | DW_ATE_unsigned_fixed | DW_ATE_decimal_float
//   - DW_AT_endianity : DW_CLASS_CONSTANT = DW_END_default | DW_END_big | DW_END_little
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_picture_string : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_small : DW_CLASS_REFERENCE 

// - DW_TAG_catch_block
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_high_pc : DW_CLASS_ADDRESS
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_ranges : DW_CLASS_RANGELISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_class_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_specification : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_common_block
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_location : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_common_inclusion
//   - DW_AT_common_reference : DW_CLASS_REFERENCE
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_compile_unit
//   - DW_AT_base_types : DW_CLASS_REFERENCE
//   - DW_AT_base_types : DW_CLASS_STRING
//   - DW_AT_identifier_case : DW_CLASS_CONSTANT = DW_ID_case_sensitive | DW_ID_up_case | DW_ID_down_case | DW_ID_case_insensitive
//   - DW_AT_high_pc : DW_CLASS_ADDRESS
//   - DW_AT_language : DW_CLASS_CONSTANT = DW_LANG_C89 | DW_LANG_C | DW_LANG_Ada83 | DW_LANG_C_plus_plus | DW_LANG_Cobol74 | DW_LANG_Cobol85 | DW_LANG_Fortran77 | DW_LANG_Fortran90 | DW_LANG_Pascal83 | DW_LANG_Modula2 | DW_LANG_Java | DW_LANG_C99 | DW_LANG_Ada95 | DW_LANG_Fortran95 | DW_LANG_PLI | DW_LANG_ObjC | DW_LANG_ObjC_plus_plus | DW_LANG_UPC | DW_LANG_D
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_macro_info : DW_CLASS_MACPTR
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_producer : DW_CLASS_STRING
//   - DW_AT_ranges : DW_CLASS_RANGELISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_stmt_list : DW_CLASS_LINEPTR
//   - DW_AT_use_UTF8 : DW_CLASS_FLAG

// - DW_TAG_condition
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_const_type
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_constant
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_const_value : DW_CLASS_BLOCK | DW_CLASS_CONSTANT | DW_CLASS_STRING
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_endianity : DW_CLASS_CONSTANT = DW_END_default | DW_END_big | DW_END_little
//   - DW_AT_external : DW_CLASS_FLAG
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_dwarf_procedure
//   - DW_AT_location : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR

// - DW_TAG_entry_point
//   - DW_AT_address_class : DW_CLASS_CONSTANT = arch
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_frame_base : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_return_addr : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_static_link : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_enumeration_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_bit_stride : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_stride : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_specification : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_enumerator
//   - DW_AT_const_value : DW_CLASS_BLOCK | DW_CLASS_CONSTANT | DW_CLASS_STRING
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_file_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_formal_parameter
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_artificial : DW_CLASS_FLAG
//   - DW_AT_const_value : DW_CLASS_BLOCK | DW_CLASS_CONSTANT | DW_CLASS_STRING
//   - DW_AT_default_value : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_endianity : DW_CLASS_CONSTANT = DW_END_default | DW_END_big | DW_END_little
//   - DW_AT_is_optional : DW_CLASS_FLAG
//   - DW_AT_location : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_variable_parameter : DW_CLASS_FLAG

// - DW_TAG_friend
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_friend : DW_CLASS_REFERENCE
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_imported_declaration
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_import : DW_CLASS_REFERENCE
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT

// - DW_TAG_imported_module
//   - DW_AT_import : DW_CLASS_REFERENCE
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT

// - DW_TAG_imported_unit
//   - DW_AT_import : DW_CLASS_REFERENCE

// - DW_TAG_inheritance
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_data_member_location : DW_CLASS_CONSTANT | DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_virtuality : DW_CLASS_CONSTANT = DW_VIRTUALITY_none | DW_VIRTUALITY_virtual | DW_VIRTUALITY_pure_virtual

// - DW_TAG_inlined_subroutine
//   - DW_AT_call_column : DW_CLASS_CONSTANT
//   - DW_AT_call_file : DW_CLASS_CONSTANT
//   - DW_AT_call_line : DW_CLASS_CONSTANT
//   - DW_AT_entry_pc : DW_CLASS_ADDRESS
//   - DW_AT_high_pc : DW_CLASS_ADDRESS
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_ranges : DW_CLASS_RANGELISTPTR
//   - DW_AT_return_addr : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_trampoline : DW_CLASS_ADDRESS | DW_CLASS_FLAG | DW_CLASS_REFERENCE | DW_CLASS_STRING

// - DW_TAG_interface_type
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT

// - DW_TAG_label
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_lexical_block
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_high_pc : DW_CLASS_ADDRESS
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_ranges : DW_CLASS_RANGELISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_member
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_bit_offset : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_bit_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_member_location : DW_CLASS_CONSTANT | DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_mutable : DW_CLASS_FLAG
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_module
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_entry_pc : DW_CLASS_ADDRESS
//   - DW_AT_high_pc : DW_CLASS_ADDRESS
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_priority : DW_CLASS_REFERENCE
//   - DW_AT_ranges : DW_CLASS_RANGELISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_specification : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_namelist
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_namelist_item
//   - DW_AT_name : DW_CLASS_STRINGlist_item
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_namespace
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_extension : DW_CLASS_REFERENCE
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT

// - DW_TAG_packed_type
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_partial_unit
//   - DW_AT_base_types : DW_CLASS_REFERENCE
//   - DW_AT_base_types : DW_CLASS_STRING
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_identifier_case : DW_CLASS_CONSTANT = DW_ID_case_sensitive | DW_ID_up_case | DW_ID_down_case | DW_ID_case_insensitive
//   - DW_AT_high_pc : DW_CLASS_ADDRESS
//   - DW_AT_language : DW_CLASS_CONSTANT = DW_LANG_C89 | DW_LANG_C | DW_LANG_Ada83 | DW_LANG_C_plus_plus | DW_LANG_Cobol74 | DW_LANG_Cobol85 | DW_LANG_Fortran77 | DW_LANG_Fortran90 | DW_LANG_Pascal83 | DW_LANG_Modula2 | DW_LANG_Java | DW_LANG_C99 | DW_LANG_Ada95 | DW_LANG_Fortran95 | DW_LANG_PLI | DW_LANG_ObjC | DW_LANG_ObjC_plus_plus | DW_LANG_UPC | DW_LANG_D
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_macro_info : DW_CLASS_MACPTR
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_producer : DW_CLASS_STRING
//   - DW_AT_ranges : DW_CLASS_RANGELISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_stmt_list : DW_CLASS_LINEPTR
//   - DW_AT_use_UTF8 : DW_CLASS_FLAG

// - DW_TAG_pointer_type
//   - DW_AT_address_class : DW_CLASS_CONSTANT = arch
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_specification : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_ptr_to_member_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_address_class : DW_CLASS_CONSTANT = arch
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_containing_type : DW_CLASS_REFERENCE
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_use_location : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_reference_type
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_restrict_type
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_set_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_shared_type
//   - DW_AT_count : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_string_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_string_length : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_structure_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_specification : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_subprogram
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_address_class : DW_CLASS_CONSTANT = arch
//   - DW_AT_artificial : DW_CLASS_FLAG
//   - DW_AT_calling_convention : DW_CLASS_CONSTANT = DW_CC_normal | DW_CC_program | DW_CC_nocall
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_elemental : DW_CLASS_FLAG
//   - DW_AT_entry_pc : DW_CLASS_ADDRESS
//   - DW_AT_explicit : DW_CLASS_FLAG
//   - DW_AT_external : DW_CLASS_FLAG
//   - DW_AT_frame_base : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_high_pc : DW_CLASS_ADDRESS
//   - DW_AT_inline : DW_CLASS_CONSTANT = DW_INL_not_inlined | DW_INL_inlined | DW_INL_declared_not_inlined | DW_INL_declared_inlined
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_object_pointer : DW_CLASS_REFERENCE
//   - DW_AT_prototyped : DW_CLASS_FLAG
//   - DW_AT_pure : DW_CLASS_FLAG
//   - DW_AT_ranges : DW_CLASS_RANGELISTPTR
//   - DW_AT_recursive : DW_CLASS_FLAG
//   - DW_AT_return_addr : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_specification : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_static_link : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_trampoline : DW_CLASS_ADDRESS | DW_CLASS_FLAG | DW_CLASS_REFERENCE | DW_CLASS_STRING
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified
//   - DW_AT_virtuality : DW_CLASS_CONSTANT = DW_VIRTUALITY_none | DW_VIRTUALITY_virtual | DW_VIRTUALITY_pure_virtual
//   - DW_AT_vtable_elem_location : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR

// - DW_TAG_subrange_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_bit_stride : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_stride : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_count : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_lower_bound : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_threads_scaled : DW_CLASS_FLAG
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_upper_bound : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_subroutine_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_address_class : DW_CLASS_CONSTANT = arch
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_prototyped : DW_CLASS_FLAG
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_template_type_parameter
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_template_value_parameter
//   - DW_AT_const_value : DW_CLASS_BLOCK | DW_CLASS_CONSTANT | DW_CLASS_STRING
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_thrown_type
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_try_block
//   - DW_AT_high_pc : DW_CLASS_ADDRESS
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_ranges : DW_CLASS_RANGELISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_typedef
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_union_type
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_byte_size : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING 
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_specification : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_unspecified_parameters
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_artificial : DW_CLASS_FLAG
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_unspecified_type
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_name : DW_CLASS_STRING

// - DW_TAG_variable
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_const_value : DW_CLASS_BLOCK | DW_CLASS_CONSTANT | DW_CLASS_STRING
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_description : DW_CLASS_STRING
//   - DW_AT_endianity : DW_CLASS_CONSTANT = DW_END_default | DW_END_big | DW_END_little
//   - DW_AT_external : DW_CLASS_FLAG
//   - DW_AT_location : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_specification : DW_CLASS_REFERENCE
//   - DW_AT_start_scope : DW_CLASS_CONSTANT
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

// - DW_TAG_variant
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_discr_list : DW_CLASS_BLOCK
//   - DW_AT_discr_value : DW_CLASS_CONSTANT
//   - DW_AT_sibling : DW_CLASS_REFERENCE

// - DW_TAG_variant_part
//   - DW_AT_abstract_origin : DW_CLASS_REFERENCE
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_discr : DW_CLASS_REFERENCE
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_volatile_type  
//   - DW_AT_allocated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_associated : DW_CLASS_CONSTANT | DW_CLASS_REFERENCE | DW_CLASS_EXPRESSION
//   - DW_AT_data_location : DW_CLASS_EXPRESSION
//   - DW_AT_name : DW_CLASS_STRING
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE

// - DW_TAG_with_stmt  
//   - DW_AT_accessibility : DW_CLASS_CONSTANT = DW_ACCESS_public | DW_ACCESS_protected | DW_ACCESS_private
//   - DW_AT_address_class : DW_CLASS_CONSTANT = arch
//   - DW_AT_declaration : DW_CLASS_FLAG
//   - DW_AT_high_pc : DW_CLASS_ADDRESS
//   - DW_AT_location : DW_CLASS_EXPRESSION | DW_CLASS_LOCLISTPTR
//   - DW_AT_low_pc : DW_CLASS_ADDRESS
//   - DW_AT_ranges : DW_CLASS_RANGELISTPTR
//   - DW_AT_segment : DW_CLASS_EXPRESSION
//   - DW_AT_sibling : DW_CLASS_REFERENCE
//   - DW_AT_type : DW_CLASS_REFERENCE
//   - DW_AT_visibility : DW_CLASS_CONSTANT = DW_VIS_local | DW_VIS_exported | DW_VIS_qualified

template <class MEMORY_ADDR>
class DWARF_DIE // Debug Info Entry
{
public:
	DWARF_DIE(DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, DWARF_DIE<MEMORY_ADDR> *dw_parent_die = 0);
	virtual ~DWARF_DIE();
	const DWARF_CompilationUnit<MEMORY_ADDR> *GetCompilationUnit() const;
	const DWARF_DIE<MEMORY_ADDR> *GetRootDIE() const;
	const DWARF_DIE<MEMORY_ADDR> *GetParentDIE() const;
	const DWARF_Abbrev *GetAbbrev() const;
	const std::vector<DWARF_DIE<MEMORY_ADDR> *>& GetChildren() const;
	const DWARF_DIE<MEMORY_ADDR> *GetChild(uint16_t _dw_tag) const;
	const std::vector<DWARF_Attribute<MEMORY_ADDR> *>& GetAttributes() const;
	const DWARF_Attribute<MEMORY_ADDR> *FindAttribute(uint16_t dw_at) const;
	uint16_t GetTag() const;
	uint64_t GetOffset() const;
	void Add(DWARF_Attribute<MEMORY_ADDR> *dw_attribute);
	void Add(DWARF_DIE<MEMORY_ADDR> *child);
	int64_t Load(const uint8_t *rawdata, uint64_t max_size, uint64_t offset);
	bool IsNull() const;
	void Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int id);
	unsigned int GetId() const;
	std::string GetHREF() const;
	std::ostream& to_XML(std::ostream& os);
	std::ostream& to_HTML(std::ostream& os) const;
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_DIE<MEMORY_ADDR>& dw_die);
	void BuildStatementMatrix(std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& stmt_matrix);
	bool HasOverlap(MEMORY_ADDR addr, MEMORY_ADDR length) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDIEByAddrRange(unsigned int dw_tag, MEMORY_ADDR addr, MEMORY_ADDR length) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDIEByName(unsigned int dw_tag, const char *name, bool external) const;
	const DWARF_DIE<MEMORY_ADDR> *FindParentDIE(unsigned int dw_tag) const;
	
	const DWARF_DIE<MEMORY_ADDR> *FindDataObject(const char *name) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDataMember(const char *name) const;
	const DWARF_DIE<MEMORY_ADDR> *FindSubProgram(const char *name) const;
	
	void EnumerateDataObjectNames(std::set<std::string>& name_set) const;
	
	const char *GetName() const;
	bool GetLowPC(MEMORY_ADDR& low_pc) const;
	bool GetHighPC(MEMORY_ADDR& high_pc, bool& is_offset) const;
	bool GetContigousAddressRange(MEMORY_ADDR& low_pc, MEMORY_ADDR& high_pc) const;
	bool GetDefaultBaseAddress(MEMORY_ADDR& base_addr) const;
	const DWARF_RangeListEntry<MEMORY_ADDR> *GetNonContigousAddressRange() const;
	const DWARF_Expression<MEMORY_ADDR> *GetSegment() const;
	bool GetCallingConvention(uint8_t& calling_convention) const;
	bool GetFrameBase(unsigned int prc_num, MEMORY_ADDR pc, MEMORY_ADDR& frame_base) const;
	bool GetLowerBound(unsigned int prc_num, int64_t& upper_bound) const;
	bool GetUpperBound(unsigned int prc_num, int64_t& upper_bound) const;
	bool GetCount(unsigned int prc_num, uint64_t& count) const;
	bool GetArrayElementCount(unsigned int prc_num, unsigned int dim, uint64_t& count) const;
	bool GetByteSize(unsigned int prc_num, uint64_t& byte_size) const;
	bool GetBitSize(unsigned int prc_num, uint64_t& bit_size) const;
	bool GetArrayElementBitSize(unsigned int prc_num, uint64_t& bit_size) const;
	bool GetBitOffset(unsigned int prc_num, int64_t& bit_offset) const;
	bool GetBitStride(unsigned int prc_num, uint64_t& bit_stride) const;
	bool GetDataBitOffset(unsigned int prc_num, int64_t& data_bit_offset) const;
	bool GetObjectBitSize(unsigned int prc_num, uint64_t& bit_size) const;
	bool GetLocationExpression(uint16_t dw_at, MEMORY_ADDR pc, const DWARF_Expression<MEMORY_ADDR> * & p_dw_loc_expr, std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges) const;
	bool GetLocation(unsigned int prc_num, MEMORY_ADDR pc, bool has_frame_base, MEMORY_ADDR frame_base, DWARF_Location<MEMORY_ADDR>& loc) const;
	void GetRanges(std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges) const;
	bool GetDataMemberLocation(unsigned int prc_num, MEMORY_ADDR pc, bool has_frame_base, MEMORY_ADDR frame_base, MEMORY_ADDR object_addr, DWARF_Location<MEMORY_ADDR>& loc) const;
	bool GetExternalFlag(bool& external_flag) const;
	bool GetDeclarationFlag(bool& declaration_flag) const;
	bool GetOrdering(uint8_t& ordering) const;
	bool GetEncoding(uint8_t& encoding) const;
	bool GetArrayElementEncoding(uint8_t& encoding) const;
	unsigned int GetSubRangeCount() const;
	const DWARF_DIE<MEMORY_ADDR> *GetAbstractOrigin() const;
	const DWARF_DIE<MEMORY_ADDR> *GetSpecification() const;
	
	const unisim::util::debug::Type *BuildType(unsigned int prc_num, bool following_pointer = false, unsigned int array_dim = 0) const;
	const unisim::util::debug::Type *BuildTypeOf(unsigned int prc_num) const;
	
	const unisim::util::debug::SubProgram<MEMORY_ADDR> *BuildSubProgram(unsigned int prc_num) const;
	
	bool GetAttributeValue(uint16_t dw_at, const DWARF_Address<MEMORY_ADDR> * & p_dw_addr_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_Block<MEMORY_ADDR> * & p_dw_block_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_Constant<MEMORY_ADDR> * & p_dw_const_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_UnsignedConstant<MEMORY_ADDR> * & p_dw_uconst_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_SignedConstant<MEMORY_ADDR> * & p_dw_sconst_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_UnsignedLEB128Constant<MEMORY_ADDR> * & p_dw_uconst_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_SignedLEB128Constant<MEMORY_ADDR> * & p_dw_sconst_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_Flag<MEMORY_ADDR> * & p_dw_flag_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_LinePtr<MEMORY_ADDR> * & p_dw_lineptr_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_LocListPtr<MEMORY_ADDR> * & p_dw_loclist_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_MacPtr<MEMORY_ADDR> * & p_dw_macptr_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_RangeListPtr<MEMORY_ADDR> * & p_dw_ranges_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_Reference<MEMORY_ADDR> * & p_dw_ref_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_String<MEMORY_ADDR> * & p_dw_str_attr) const;
	bool GetAttributeValue(uint16_t dw_at, const DWARF_Expression<MEMORY_ADDR> * & p_dw_expr_attr) const;
	bool GetAttributeStaticDynamicValue(unsigned int prc_num, uint16_t dw_at, uint64_t& value) const;
	bool GetAttributeStaticDynamicValue(unsigned int prc_num, uint16_t dw_at, int64_t& value) const;
private:
	DWARF_Handler<MEMORY_ADDR> *dw_handler;
	DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu;
	DWARF_DIE<MEMORY_ADDR> *dw_parent_die;
	std::ostream& debug_info_stream;
	std::ostream& debug_warning_stream;
	std::ostream& debug_error_stream;
	bool debug;
	uint64_t offset;
	unsigned int id;
	const DWARF_Abbrev *abbrev;
	std::vector<DWARF_DIE<MEMORY_ADDR> *> children;
	std::vector<DWARF_Attribute<MEMORY_ADDR> *> attributes;
};



} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
