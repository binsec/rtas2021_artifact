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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_ENCODING_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_ENCODING_HH__

#include <inttypes.h>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

///////////////////////////////////////////////////////////////////////////////
//                             Version encoding                              //
///////////////////////////////////////////////////////////////////////////////

//   - .debug_aranges
const uint8_t DW_DEBUG_ARANGES_VER2 = 2;
//   - .debug_frame
const uint8_t DW_DEBUG_FRAME_VER2 = 1;
const uint8_t DW_DEBUG_FRAME_VER3 = 3;
const uint8_t DW_DEBUG_FRAME_VER4 = 4;
//   - .debug_info
const uint8_t DW_DEBUG_INFO_VER2 = 2;
const uint8_t DW_DEBUG_INFO_VER3 = 3;
const uint8_t DW_DEBUG_INFO_VER4 = 4;
//   - .debug_line
const uint8_t DW_DEBUG_LINE_VER2 = 2;
const uint8_t DW_DEBUG_LINE_VER3 = 3;
const uint8_t DW_DEBUG_LINE_VER4 = 4;
//   - .debug_pubnames and .debug_pubtypes
const uint8_t DW_DEBUG_PUBS_VER2 = 2;
//   - .debug_types
const uint8_t DW_DEBUG_TYPES_VER4 = 4;

///////////////////////////////////////////////////////////////////////////////
//                   Children determination encoding                         //
///////////////////////////////////////////////////////////////////////////////

const uint8_t DW_CHILDREN_no  = 0;
const uint8_t DW_CHILDREN_yes = 1;

///////////////////////////////////////////////////////////////////////////////
//                             Tag encoding                                  //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint16_t DW_TAG_array_type                  = 0x01;
const uint16_t DW_TAG_class_type                  = 0x02;
const uint16_t DW_TAG_entry_point                 = 0x03;
const uint16_t DW_TAG_enumeration_type            = 0x04;
const uint16_t DW_TAG_formal_parameter            = 0x05;
const uint16_t DW_TAG_imported_declaration        = 0x08;
const uint16_t DW_TAG_label                       = 0x0a;
const uint16_t DW_TAG_lexical_block               = 0x0b;
const uint16_t DW_TAG_member                      = 0x0d;
const uint16_t DW_TAG_pointer_type                = 0x0f;
const uint16_t DW_TAG_reference_type              = 0x10;
const uint16_t DW_TAG_compile_unit                = 0x11;
const uint16_t DW_TAG_string_type                 = 0x12;
const uint16_t DW_TAG_structure_type              = 0x13;
const uint16_t DW_TAG_subroutine_type             = 0x15;
const uint16_t DW_TAG_typedef                     = 0x16;
const uint16_t DW_TAG_union_type                  = 0x17;
const uint16_t DW_TAG_unspecified_parameters      = 0x18;
const uint16_t DW_TAG_variant                     = 0x19;
const uint16_t DW_TAG_common_block                = 0x1a;
const uint16_t DW_TAG_common_inclusion            = 0x1b;
const uint16_t DW_TAG_inheritance                 = 0x1c;
const uint16_t DW_TAG_inlined_subroutine          = 0x1d;
const uint16_t DW_TAG_module                      = 0x1e;
const uint16_t DW_TAG_ptr_to_member_type          = 0x1f;
const uint16_t DW_TAG_set_type                    = 0x20;
const uint16_t DW_TAG_subrange_type               = 0x21;
const uint16_t DW_TAG_with_stmt                   = 0x22;
const uint16_t DW_TAG_access_declaration          = 0x23;
const uint16_t DW_TAG_base_type                   = 0x24;
const uint16_t DW_TAG_catch_block                 = 0x25;
const uint16_t DW_TAG_const_type                  = 0x26;
const uint16_t DW_TAG_constant                    = 0x27;
const uint16_t DW_TAG_enumerator                  = 0x28;
const uint16_t DW_TAG_file_type                   = 0x29;
const uint16_t DW_TAG_friend                      = 0x2a;
const uint16_t DW_TAG_namelist                    = 0x2b;
const uint16_t DW_TAG_namelist_item               = 0x2c;
const uint16_t DW_TAG_packed_type                 = 0x2d;
const uint16_t DW_TAG_subprogram                  = 0x2e;
const uint16_t DW_TAG_template_type_param         = 0x2f;
const uint16_t DW_TAG_template_value_param        = 0x30;
const uint16_t DW_TAG_thrown_type                 = 0x31;
const uint16_t DW_TAG_try_block                   = 0x32;
const uint16_t DW_TAG_variant_part                = 0x33;
const uint16_t DW_TAG_variable                    = 0x34;
const uint16_t DW_TAG_volatile_type               = 0x35;

// DWARF v3
const uint16_t DW_TAG_dwarf_procedure             = 0x36;
const uint16_t DW_TAG_restrict_type               = 0x37;
const uint16_t DW_TAG_interface_type              = 0x38;
const uint16_t DW_TAG_namespace                   = 0x39;
const uint16_t DW_TAG_imported_module             = 0x3a;
const uint16_t DW_TAG_unspecified_type            = 0x3b;
const uint16_t DW_TAG_partial_unit                = 0x3c;
const uint16_t DW_TAG_imported_unit               = 0x3d;
const uint16_t DW_TAG_condition                   = 0x3f;
const uint16_t DW_TAG_shared_type                 = 0x40;
const uint16_t DW_TAG_type_unit                   = 0x41;

// DWARF v4
const uint16_t DW_TAG_rvalue_reference_type       = 0x42;
const uint16_t DW_TAG_template_alias              = 0x43;

// Vendor extensions
const uint16_t DW_TAG_lo_user                     = 0x4080;
const uint16_t DW_TAG_hi_user                     = 0xffff;

// SGI/MIPS Extensions
const uint16_t DW_TAG_MIPS_loop                   = 0x4081;

// HP extensions
const uint16_t DW_TAG_HP_array_descriptor         = 0x4090;
const uint16_t DW_TAG_HP_Bliss_field              = 0x4091;
const uint16_t DW_TAG_HP_Bliss_field_set          = 0x4092;

// GNU extensions
const uint16_t DW_TAG_format_label                = 0x4101;
const uint16_t DW_TAG_function_template           = 0x4102;
const uint16_t DW_TAG_class_template              = 0x4103;
const uint16_t DW_TAG_GNU_BINCL                   = 0x4104;
const uint16_t DW_TAG_GNU_EINCL                   = 0x4105;
const uint16_t DW_TAG_GNU_template_template_param = 0x4106;
const uint16_t DW_TAG_GNU_template_parameter_pack = 0x4107;
const uint16_t DW_TAG_GNU_formal_parameter_pack   = 0x4108;
const uint16_t DW_TAG_GNU_call_site               = 0x4109;
const uint16_t DW_TAG_GNU_call_site_parameter     = 0x410a;

// Extensions for UPC
const uint16_t DW_TAG_upc_shared_type             = 0x8765;
const uint16_t DW_TAG_upc_strict_type             = 0x8766;
const uint16_t DW_TAG_upc_relaxed_type            = 0x8767;

// PGI (STMicroelectronics) extensions
const uint16_t DW_TAG_PGI_kanji_type              = 0xa000;
const uint16_t DW_TAG_PGI_interface_block         = 0xa020;

///////////////////////////////////////////////////////////////////////////////
//                        Attribute name encoding                            //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint16_t DW_AT_sibling              = 0x01;
const uint16_t DW_AT_location             = 0x02;
const uint16_t DW_AT_name                 = 0x03;
const uint16_t DW_AT_ordering             = 0x09;
const uint16_t DW_AT_byte_size            = 0x0b;
const uint16_t DW_AT_bit_offset           = 0x0c;
const uint16_t DW_AT_bit_size             = 0x0d;
const uint16_t DW_AT_stmt_list            = 0x10;
const uint16_t DW_AT_low_pc               = 0x11;
const uint16_t DW_AT_high_pc              = 0x12;
const uint16_t DW_AT_language             = 0x13;
const uint16_t DW_AT_discr                = 0x15;
const uint16_t DW_AT_discr_value          = 0x16;
const uint16_t DW_AT_visibility           = 0x17;
const uint16_t DW_AT_import               = 0x18;
const uint16_t DW_AT_string_length        = 0x19;
const uint16_t DW_AT_common_reference     = 0x1a;
const uint16_t DW_AT_comp_dir             = 0x1b;
const uint16_t DW_AT_const_value          = 0x1c;
const uint16_t DW_AT_containing_type      = 0x1d;
const uint16_t DW_AT_default_value        = 0x1e;
const uint16_t DW_AT_inline               = 0x20;
const uint16_t DW_AT_is_optional          = 0x21;
const uint16_t DW_AT_lower_bound          = 0x22;
const uint16_t DW_AT_producer             = 0x25;
const uint16_t DW_AT_prototyped           = 0x27;
const uint16_t DW_AT_return_addr          = 0x2a;
const uint16_t DW_AT_start_scope          = 0x2c;
const uint16_t DW_AT_stride_size          = 0x2e;

// DWARF v3
const uint16_t DW_AT_bit_stride           = 0x2e;
const uint16_t DW_AT_upper_bound          = 0x2f;
const uint16_t DW_AT_abstract_origin      = 0x31;
const uint16_t DW_AT_accessibility        = 0x32;
const uint16_t DW_AT_address_class        = 0x33;
const uint16_t DW_AT_artificial           = 0x34;
const uint16_t DW_AT_base_types           = 0x35;
const uint16_t DW_AT_calling_convention   = 0x36;
const uint16_t DW_AT_count                = 0x37;
const uint16_t DW_AT_data_member_location = 0x38;
const uint16_t DW_AT_decl_column          = 0x39;
const uint16_t DW_AT_decl_file            = 0x3a;
const uint16_t DW_AT_decl_line            = 0x3b;
const uint16_t DW_AT_declaration          = 0x3c;
const uint16_t DW_AT_discr_list           = 0x3d;
const uint16_t DW_AT_encoding             = 0x3e;
const uint16_t DW_AT_external             = 0x3f;
const uint16_t DW_AT_frame_base           = 0x40;
const uint16_t DW_AT_friend               = 0x41;
const uint16_t DW_AT_identifier_case      = 0x42;
const uint16_t DW_AT_macro_info           = 0x43;
const uint16_t DW_AT_namelist_item        = 0x44;
const uint16_t DW_AT_priority             = 0x45;
const uint16_t DW_AT_segment              = 0x46;
const uint16_t DW_AT_specification        = 0x47;
const uint16_t DW_AT_static_link          = 0x48;
const uint16_t DW_AT_type                 = 0x49;
const uint16_t DW_AT_use_location         = 0x4a;
const uint16_t DW_AT_variable_parameter   = 0x4b;
const uint16_t DW_AT_virtuality           = 0x4c;
const uint16_t DW_AT_vtable_elem_location = 0x4d;
const uint16_t DW_AT_allocated            = 0x4e;
const uint16_t DW_AT_associated           = 0x4f;
const uint16_t DW_AT_data_location        = 0x50;
const uint16_t DW_AT_byte_stride          = 0x51;
const uint16_t DW_AT_entry_pc             = 0x52;
const uint16_t DW_AT_use_UTF8             = 0x53;
const uint16_t DW_AT_extension            = 0x54;
const uint16_t DW_AT_ranges               = 0x55;
const uint16_t DW_AT_trampoline           = 0x56;
const uint16_t DW_AT_call_column          = 0x57;
const uint16_t DW_AT_call_file            = 0x58;
const uint16_t DW_AT_call_line            = 0x59;
const uint16_t DW_AT_description          = 0x5a;
const uint16_t DW_AT_binary_scale         = 0x5b;
const uint16_t DW_AT_decimal_scale        = 0x5c;
const uint16_t DW_AT_small                = 0x5d;
const uint16_t DW_AT_decimal_sign         = 0x5e;
const uint16_t DW_AT_digit_count          = 0x5f;
const uint16_t DW_AT_picture_string       = 0x60;
const uint16_t DW_AT_mutable              = 0x61;
const uint16_t DW_AT_threads_scaled       = 0x62;
const uint16_t DW_AT_explicit             = 0x63;
const uint16_t DW_AT_object_pointer       = 0x64;
const uint16_t DW_AT_endianity            = 0x65;
const uint16_t DW_AT_elemental            = 0x66;
const uint16_t DW_AT_pure                 = 0x67;
const uint16_t DW_AT_recursive            = 0x68;

// DWARF v4
const uint16_t DW_AT_signature            = 0x69;
const uint16_t DW_AT_main_subprogram      = 0x6a;
const uint16_t DW_AT_data_bit_offset      = 0x6b;
const uint16_t DW_AT_const_expr           = 0x6c;
const uint16_t DW_AT_enum_class           = 0x6d;
const uint16_t DW_AT_linkage_name         = 0x6e;

// Vendor extensions
const uint16_t DW_AT_lo_user              = 0x2000;
const uint16_t DW_AT_hi_user              = 0x3fff;

// SGI/MIPS extensions
const uint16_t DW_AT_MIPS_fde                     = 0x2001;
const uint16_t DW_AT_MIPS_loop_begin              = 0x2002;
const uint16_t DW_AT_MIPS_tail_loop_begin         = 0x2003;
const uint16_t DW_AT_MIPS_epilog_begin            = 0x2004;
const uint16_t DW_AT_MIPS_loop_unroll_factor      = 0x2005;
const uint16_t DW_AT_MIPS_software_pipeline_depth = 0x2006;
const uint16_t DW_AT_MIPS_linkage_name            = 0x2007;
const uint16_t DW_AT_MIPS_stride                  = 0x2008;
const uint16_t DW_AT_MIPS_abstract_name           = 0x2009;
const uint16_t DW_AT_MIPS_clone_origin            = 0x200a;
const uint16_t DW_AT_MIPS_has_inlines             = 0x200b;

// HP extensions
const uint16_t DW_AT_HP_block_index              = 0x2000;
const uint16_t DW_AT_HP_unmodifiable             = 0x2001;
const uint16_t DW_AT_HP_prologue                 = 0x2005;
const uint16_t DW_AT_HP_epilogue                 = 0x2008;
const uint16_t DW_AT_HP_actuals_stmt_list        = 0x2010;
const uint16_t DW_AT_HP_proc_per_section         = 0x2011;
const uint16_t DW_AT_HP_raw_data_ptr             = 0x2012;
const uint16_t DW_AT_HP_pass_by_reference        = 0x2013;
const uint16_t DW_AT_HP_opt_level                = 0x2014;
const uint16_t DW_AT_HP_prof_version_id          = 0x2015;
const uint16_t DW_AT_HP_opt_flags                = 0x2016;
const uint16_t DW_AT_HP_cold_region_low_pc       = 0x2017;
const uint16_t DW_AT_HP_cold_region_high_pc      = 0x2018;
const uint16_t DW_AT_HP_all_variables_modifiable = 0x2019;
const uint16_t DW_AT_HP_linkage_name             = 0x201a;
const uint16_t DW_AT_HP_prof_flags               = 0x201b;
const uint16_t DW_AT_HP_unit_name                = 0x201f;
const uint16_t DW_AT_HP_unit_size                = 0x2020;
const uint16_t DW_AT_HP_widened_byte_size        = 0x2021;
const uint16_t DW_AT_HP_definition_points        = 0x2022;
const uint16_t DW_AT_HP_default_location         = 0x2023;
const uint16_t DW_AT_HP_is_result_param          = 0x2029;

// GNU extensions
const uint16_t DW_AT_sf_names                       = 0x2101;
const uint16_t DW_AT_src_info                       = 0x2102;
const uint16_t DW_AT_mac_info                       = 0x2103;
const uint16_t DW_AT_src_coords                     = 0x2104;
const uint16_t DW_AT_body_begin                     = 0x2105;
const uint16_t DW_AT_body_end                       = 0x2106;
const uint16_t DW_AT_GNU_vector                     = 0x2107;
const uint16_t DW_AT_GNU_guarded_by                 = 0x2108;
const uint16_t DW_AT_GNU_pt_guarded_by              = 0x2109;
const uint16_t DW_AT_GNU_guarded                    = 0x210a;
const uint16_t DW_AT_GNU_pt_guarded                 = 0x210b;
const uint16_t DW_AT_GNU_locks_excluded             = 0x210c;
const uint16_t DW_AT_GNU_exclusive_locks_required   = 0x210d;
const uint16_t DW_AT_GNU_shared_locks_required      = 0x210e;
const uint16_t DW_AT_GNU_odr_signature              = 0x210f;
const uint16_t DW_AT_GNU_template_name              = 0x2110;
const uint16_t DW_AT_GNU_call_site_value            = 0x2111;
const uint16_t DW_AT_GNU_call_site_data_value       = 0x2112;
const uint16_t DW_AT_GNU_call_site_target           = 0x2113;
const uint16_t DW_AT_GNU_call_site_target_clobbered = 0x2114;
const uint16_t DW_AT_GNU_tail_call                  = 0x2115;
const uint16_t DW_AT_GNU_all_tail_call_sites        = 0x2116;
const uint16_t DW_AT_GNU_all_call_sites             = 0x2117;
const uint16_t DW_AT_GNU_all_source_call_sites      = 0x2118;
const uint16_t DW_AT_GNU_macros                     = 0x2119;

// GNU Extensions for Fission
const uint16_t DW_AT_GNU_dwo_name                   = 0x2130;
const uint16_t DW_AT_GNU_dwo_id                     = 0x2131;
const uint16_t DW_AT_GNU_ref_base                   = 0x2132;
const uint16_t DW_AT_GNU_addr_base                  = 0x2133;
const uint16_t DW_AT_GNU_pubnames                   = 0x2134;
const uint16_t DW_AT_GNU_pubtypes                   = 0x2135;

// VMS extensions
const uint16_t DW_AT_VMS_rtnbeg_pd_address = 0x2201;

// GNAT extensions
const uint16_t DW_AT_use_GNAT_descriptive_type = 0x2301;
const uint16_t DW_AT_GNAT_descriptive_type     = 0x2302;

// UPC extension
const uint16_t DW_AT_upc_threads_scaled = 0x3210;

// PGI (STMicroelectronics) extensions
const uint16_t DW_AT_PGI_lbase    = 0x3a00;
const uint16_t DW_AT_PGI_soffset  = 0x3a01;
const uint16_t DW_AT_PGI_lstride  = 0x3a02;

///////////////////////////////////////////////////////////////////////////////
//                                   Forms                                   //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint16_t DW_FORM_addr         = 0x01;
const uint16_t DW_FORM_block2       = 0x03;
const uint16_t DW_FORM_block4       = 0x04;
const uint16_t DW_FORM_data2        = 0x05;
const uint16_t DW_FORM_data4        = 0x06;
const uint16_t DW_FORM_data8        = 0x07;
const uint16_t DW_FORM_string       = 0x08;
const uint16_t DW_FORM_block        = 0x09;
const uint16_t DW_FORM_block1       = 0x0a;
const uint16_t DW_FORM_data1        = 0x0b;
const uint16_t DW_FORM_flag         = 0x0c;
const uint16_t DW_FORM_sdata        = 0x0d;
const uint16_t DW_FORM_strp         = 0x0e;
const uint16_t DW_FORM_udata        = 0x0f;
const uint16_t DW_FORM_ref_addr     = 0x10;
const uint16_t DW_FORM_ref1         = 0x11;
const uint16_t DW_FORM_ref2         = 0x12;
const uint16_t DW_FORM_ref4         = 0x13;
const uint16_t DW_FORM_ref8         = 0x14;
const uint16_t DW_FORM_ref_udata    = 0x15;
const uint16_t DW_FORM_indirect     = 0x16;

// DWARF v4
const uint16_t DW_FORM_sec_offset   = 0x17;
const uint16_t DW_FORM_expr_loc     = 0x18;
const uint16_t DW_FORM_flag_present = 0x19;
const uint16_t DW_FORM_ref_sig8     = 0x20;

///////////////////////////////////////////////////////////////////////////////
//                       DWARF expression operations                         //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_OP_addr        = 0x03;
const uint8_t DW_OP_deref       = 0x06;
const uint8_t DW_OP_const1u     = 0x08;
const uint8_t DW_OP_const1s     = 0x09;
const uint8_t DW_OP_const2u     = 0x0a;
const uint8_t DW_OP_const2s     = 0x0b;
const uint8_t DW_OP_const4u     = 0x0c;
const uint8_t DW_OP_const4s     = 0x0d;
const uint8_t DW_OP_const8u     = 0x0e;
const uint8_t DW_OP_const8s     = 0x0f;
const uint8_t DW_OP_constu      = 0x10;
const uint8_t DW_OP_consts      = 0x11;
const uint8_t DW_OP_dup         = 0x12;
const uint8_t DW_OP_drop        = 0x13;
const uint8_t DW_OP_over        = 0x14;
const uint8_t DW_OP_pick        = 0x15;
const uint8_t DW_OP_swap        = 0x16;
const uint8_t DW_OP_rot         = 0x17;
const uint8_t DW_OP_xderef      = 0x18;
const uint8_t DW_OP_abs         = 0x19;
const uint8_t DW_OP_and         = 0x1a;
const uint8_t DW_OP_div         = 0x1b;
const uint8_t DW_OP_minus       = 0x1c;
const uint8_t DW_OP_mod         = 0x1d;
const uint8_t DW_OP_mul         = 0x1e;
const uint8_t DW_OP_neg         = 0x1f;
const uint8_t DW_OP_not         = 0x20;
const uint8_t DW_OP_or          = 0x21;
const uint8_t DW_OP_plus        = 0x22;
const uint8_t DW_OP_plus_uconst = 0x23;
const uint8_t DW_OP_shl         = 0x24;
const uint8_t DW_OP_shr         = 0x25;
const uint8_t DW_OP_shra        = 0x26;
const uint8_t DW_OP_xor         = 0x27;
const uint8_t DW_OP_skip        = 0x2f;
const uint8_t DW_OP_bra         = 0x28;
const uint8_t DW_OP_eq          = 0x29;
const uint8_t DW_OP_ge          = 0x2a;
const uint8_t DW_OP_gt          = 0x2b;
const uint8_t DW_OP_le          = 0x2c;
const uint8_t DW_OP_lt          = 0x2d;
const uint8_t DW_OP_ne          = 0x2e;
const uint8_t DW_OP_lit0        = 0x30;
const uint8_t DW_OP_lit1        = 0x31;
const uint8_t DW_OP_lit2        = 0x32;
const uint8_t DW_OP_lit3        = 0x33;
const uint8_t DW_OP_lit4        = 0x34;
const uint8_t DW_OP_lit5        = 0x35;
const uint8_t DW_OP_lit6        = 0x36;
const uint8_t DW_OP_lit7        = 0x37;
const uint8_t DW_OP_lit8        = 0x38;
const uint8_t DW_OP_lit9        = 0x39;
const uint8_t DW_OP_lit10       = 0x3a;
const uint8_t DW_OP_lit11       = 0x3b;
const uint8_t DW_OP_lit12       = 0x3c;
const uint8_t DW_OP_lit13       = 0x3d;
const uint8_t DW_OP_lit14       = 0x3e;
const uint8_t DW_OP_lit15       = 0x3f;
const uint8_t DW_OP_lit16       = 0x40;
const uint8_t DW_OP_lit17       = 0x41;
const uint8_t DW_OP_lit18       = 0x42;
const uint8_t DW_OP_lit19       = 0x43;
const uint8_t DW_OP_lit20       = 0x44;
const uint8_t DW_OP_lit21       = 0x45;
const uint8_t DW_OP_lit22       = 0x46;
const uint8_t DW_OP_lit23       = 0x47;
const uint8_t DW_OP_lit24       = 0x48;
const uint8_t DW_OP_lit25       = 0x49;
const uint8_t DW_OP_lit26       = 0x4a;
const uint8_t DW_OP_lit27       = 0x4b;
const uint8_t DW_OP_lit28       = 0x4c;
const uint8_t DW_OP_lit29       = 0x4d;
const uint8_t DW_OP_lit30       = 0x4e;
const uint8_t DW_OP_lit31       = 0x4f;
const uint8_t DW_OP_reg0        = 0x50;
const uint8_t DW_OP_reg1        = 0x51;
const uint8_t DW_OP_reg2        = 0x52;
const uint8_t DW_OP_reg3        = 0x53;  
const uint8_t DW_OP_reg4        = 0x54;  
const uint8_t DW_OP_reg5        = 0x55;  
const uint8_t DW_OP_reg6        = 0x56;  
const uint8_t DW_OP_reg7        = 0x57;  
const uint8_t DW_OP_reg8        = 0x58;  
const uint8_t DW_OP_reg9        = 0x59;  
const uint8_t DW_OP_reg10       = 0x5a; 
const uint8_t DW_OP_reg11       = 0x5b; 
const uint8_t DW_OP_reg12       = 0x5c; 
const uint8_t DW_OP_reg13       = 0x5d; 
const uint8_t DW_OP_reg14       = 0x5e; 
const uint8_t DW_OP_reg15       = 0x5f; 
const uint8_t DW_OP_reg16       = 0x60; 
const uint8_t DW_OP_reg17       = 0x61; 
const uint8_t DW_OP_reg18       = 0x62; 
const uint8_t DW_OP_reg19       = 0x63; 
const uint8_t DW_OP_reg20       = 0x64; 
const uint8_t DW_OP_reg21       = 0x65; 
const uint8_t DW_OP_reg22       = 0x66; 
const uint8_t DW_OP_reg23       = 0x67; 
const uint8_t DW_OP_reg24       = 0x68; 
const uint8_t DW_OP_reg25       = 0x69; 
const uint8_t DW_OP_reg26       = 0x6a; 
const uint8_t DW_OP_reg27       = 0x6b; 
const uint8_t DW_OP_reg28       = 0x6c; 
const uint8_t DW_OP_reg29       = 0x6d; 
const uint8_t DW_OP_reg30       = 0x6e; 
const uint8_t DW_OP_reg31       = 0x6f; 
const uint8_t DW_OP_breg0       = 0x70; 
const uint8_t DW_OP_breg1       = 0x71; 
const uint8_t DW_OP_breg2       = 0x72; 
const uint8_t DW_OP_breg3       = 0x73; 
const uint8_t DW_OP_breg4       = 0x74; 
const uint8_t DW_OP_breg5       = 0x75; 
const uint8_t DW_OP_breg6       = 0x76; 
const uint8_t DW_OP_breg7       = 0x77; 
const uint8_t DW_OP_breg8       = 0x78; 
const uint8_t DW_OP_breg9       = 0x79; 
const uint8_t DW_OP_breg10      = 0x7a;
const uint8_t DW_OP_breg11      = 0x7b;
const uint8_t DW_OP_breg12      = 0x7c;
const uint8_t DW_OP_breg13      = 0x7d;
const uint8_t DW_OP_breg14      = 0x7e;
const uint8_t DW_OP_breg15      = 0x7f;
const uint8_t DW_OP_breg16      = 0x80;
const uint8_t DW_OP_breg17      = 0x81;
const uint8_t DW_OP_breg18      = 0x82;
const uint8_t DW_OP_breg19      = 0x83;
const uint8_t DW_OP_breg20      = 0x84;
const uint8_t DW_OP_breg21      = 0x85;
const uint8_t DW_OP_breg22      = 0x86;
const uint8_t DW_OP_breg23      = 0x87;
const uint8_t DW_OP_breg24      = 0x88;
const uint8_t DW_OP_breg25      = 0x89;
const uint8_t DW_OP_breg26      = 0x8a;
const uint8_t DW_OP_breg27      = 0x8b;
const uint8_t DW_OP_breg28      = 0x8c;
const uint8_t DW_OP_breg29      = 0x8d;
const uint8_t DW_OP_breg30      = 0x8e;
const uint8_t DW_OP_breg31      = 0x8f;
const uint8_t DW_OP_regx        = 0x90;
const uint8_t DW_OP_fbreg       = 0x91;
const uint8_t DW_OP_bregx       = 0x92;
const uint8_t DW_OP_piece       = 0x93;
const uint8_t DW_OP_deref_size  = 0x94;
const uint8_t DW_OP_xderef_size = 0x95;
const uint8_t DW_OP_nop         = 0x96;

// DWARF v3
const uint8_t DW_OP_push_object_address = 0x97;
const uint8_t DW_OP_call2               = 0x98;
const uint8_t DW_OP_call4               = 0x99;
const uint8_t DW_OP_call_ref            = 0x9a;
const uint8_t DW_OP_form_tls_address    = 0x9b;
const uint8_t DW_OP_call_frame_cfa      = 0x9c;
const uint8_t DW_OP_bit_piece           = 0x9d;
const uint8_t DW_OP_implicit_value      = 0x9e;
const uint8_t DW_OP_stack_value         = 0x9f;

// Vendor extensions
const uint8_t DW_OP_lo_user = 0xe0;
const uint8_t DW_OP_hi_user = 0xff;

// GNU extensions
const uint8_t DW_OP_GNU_push_tls_address = 0xe0;
const uint8_t DW_OP_GNU_uninit           = 0xf0;
const uint8_t DW_OP_GNU_encoded_addr     = 0xf1;
const uint8_t DW_OP_GNU_implicit_pointer = 0xf2;
const uint8_t DW_OP_GNU_entry_value      = 0xf3;
const uint8_t DW_OP_GNU_const_type       = 0xf4;
const uint8_t DW_OP_GNU_regval_type      = 0xf5;
const uint8_t DW_OP_GNU_deref_type       = 0xf6;
const uint8_t DW_OP_GNU_convert          = 0xf7;
const uint8_t DW_OP_GNU_reinterpret      = 0xf9;
const uint8_t DW_OP_GNU_parameter_ref    = 0xfa;
const uint8_t DW_OP_GNU_addr_index       = 0xfb;
const uint8_t DW_OP_GNU_const_index      = 0xfc;

///////////////////////////////////////////////////////////////////////////////
//                           Encoding attribute                              //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_ATE_address       = 0x01;
const uint8_t DW_ATE_boolean       = 0x02;
const uint8_t DW_ATE_complex_float = 0x03;
const uint8_t DW_ATE_float         = 0x04;
const uint8_t DW_ATE_signed        = 0x05;
const uint8_t DW_ATE_signed_char   = 0x06;
const uint8_t DW_ATE_unsigned      = 0x07;
const uint8_t DW_ATE_unsigned_char = 0x08;

// DWARF v3
const uint8_t DW_ATE_imaginary_float = 0x09;
const uint8_t DW_ATE_packed_decimal  = 0x0a;
const uint8_t DW_ATE_numeric_string  = 0x0b;
const uint8_t DW_ATE_edited          = 0x0c;
const uint8_t DW_ATE_signed_fixed    = 0x0d;
const uint8_t DW_ATE_unsigned_fixed  = 0x0e;
const uint8_t DW_ATE_decimal_float   = 0x0f;

// DWARF v4
const uint8_t DW_ATE_UTF             = 0x10;

// Vendor extensions
const uint8_t DW_ATE_lo_user = 0x80;
const uint8_t DW_ATE_hi_user = 0xff;

// HP extensions
const uint8_t DW_ATE_HP_float80             = 0x80;
const uint8_t DW_ATE_HP_complex_float80     = 0x81;
const uint8_t DW_ATE_HP_float128            = 0x82;
const uint8_t DW_ATE_HP_complex_float128    = 0x83;
const uint8_t DW_ATE_HP_floathpintel        = 0x84;
const uint8_t DW_ATE_HP_imaginary_float80   = 0x85;
const uint8_t DW_ATE_HP_imaginary_float128  = 0x86;
const uint8_t DW_ATE_HP_VAX_float           = 0x88;
const uint8_t DW_ATE_HP_VAX_float_d         = 0x89;
const uint8_t DW_ATE_HP_packed_decimal      = 0x8a;
const uint8_t DW_ATE_HP_zoned_decimal       = 0x8b;
const uint8_t DW_ATE_HP_edited              = 0x8c;
const uint8_t DW_ATE_HP_signed_fixed        = 0x8d;
const uint8_t DW_ATE_HP_unsigned_fixed      = 0x8e;
const uint8_t DW_ATE_HP_VAX_complex_float   = 0x8f;
const uint8_t DW_ATE_HP_VAX_complex_float_d = 0x90;

///////////////////////////////////////////////////////////////////////////////
//                        Decimal sign attribute                             //
///////////////////////////////////////////////////////////////////////////////

// DWARF v3
const uint8_t DW_DS_unsigned           = 0x01;
const uint8_t DW_DS_leading_overpunch  = 0x02;
const uint8_t DW_DS_trailing_overpunch = 0x03;
const uint8_t DW_DS_leading_separate   = 0x04;
const uint8_t DW_DS_trailing_separate  = 0x05;

///////////////////////////////////////////////////////////////////////////////
//                          Endianity encoding                               //
///////////////////////////////////////////////////////////////////////////////

// DWARF v3
const uint8_t DW_END_default = 0x00;
const uint8_t DW_END_big     = 0x01;
const uint8_t DW_END_little  = 0x02;

// Vendor extensions
const uint8_t DW_END_lo_user = 0x40;
const uint8_t DW_END_hi_user = 0xff;

///////////////////////////////////////////////////////////////////////////////
//                        Accessibility encoding                             //
///////////////////////////////////////////////////////////////////////////////

// DWARF v3
const uint8_t DW_ACCESS_public    = 0x01;
const uint8_t DW_ACCESS_protected = 0x02;
const uint8_t DW_ACCESS_private   = 0x03;

///////////////////////////////////////////////////////////////////////////////
//                          Visibility encoding                              //
///////////////////////////////////////////////////////////////////////////////

// DWARF v3
const uint8_t DW_VIS_local     = 0x01;
const uint8_t DW_VIS_exported  = 0x02;
const uint8_t DW_VIS_qualified = 0x03;

///////////////////////////////////////////////////////////////////////////////
//                           Vituality encoding                              //
///////////////////////////////////////////////////////////////////////////////

// DWARF v3
const uint8_t DW_VIRTUALITY_none         = 0x00;
const uint8_t DW_VIRTUALITY_virtual      = 0x01;
const uint8_t DW_VIRTUALITY_pure_virtual = 0x02;

///////////////////////////////////////////////////////////////////////////////
//                        Source language encoding                           //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint16_t DW_LANG_C89         = 0x0001;
const uint16_t DW_LANG_C           = 0x0002;
const uint16_t DW_LANG_Ada83       = 0x0003;
const uint16_t DW_LANG_C_plus_plus = 0x0004;
const uint16_t DW_LANG_Cobol74     = 0x0005;
const uint16_t DW_LANG_Cobol85     = 0x0006;
const uint16_t DW_LANG_Fortran77   = 0x0007;
const uint16_t DW_LANG_Fortran90   = 0x0008;
const uint16_t DW_LANG_Pascal83    = 0x0009;
const uint16_t DW_LANG_Modula2     = 0x000a;

// DWARF v3
const uint16_t DW_LANG_Java           = 0x000b;
const uint16_t DW_LANG_C99            = 0x000c;
const uint16_t DW_LANG_Ada95          = 0x000d;
const uint16_t DW_LANG_Fortran95      = 0x000e;
const uint16_t DW_LANG_PLI            = 0x000f;
const uint16_t DW_LANG_ObjC           = 0x0010;
const uint16_t DW_LANG_ObjC_plus_plus = 0x0011;
const uint16_t DW_LANG_UPC            = 0x0012;
const uint16_t DW_LANG_D              = 0x0013;

// DWARF v4
const uint16_t DW_LANG_python         = 0x0014;

// DWARF v5 (draft)
const uint16_t DW_LANG_Go             = 0x0016;

// Vendor extensions
const uint16_t DW_LANG_lo_user        = 0x8000;
const uint16_t DW_LANG_hi_user        = 0xffff;

// MIPS extensions
const uint16_t DW_LANG_Mips_Assembler = 0x8001;

// Upc extensions
const uint16_t DW_LANG_Upc            = 0x8765;

// HP extensions
const uint16_t DW_LANG_HP_Bliss       = 0x8003;
const uint16_t DW_LANG_HP_Basic91     = 0x8004;
const uint16_t DW_LANG_HP_Pascal91    = 0x8005;
const uint16_t DW_LANG_HP_IMacro      = 0x8006;
const uint16_t DW_LANG_HP_Assembler   = 0x8007;

///////////////////////////////////////////////////////////////////////////////
//                         Identifier case encoding                          //
///////////////////////////////////////////////////////////////////////////////

const uint8_t DW_ID_case_sensitive   = 0x00;
const uint8_t DW_ID_up_case          = 0x01;
const uint8_t DW_ID_down_case        = 0x02;
const uint8_t DW_ID_case_insensitive = 0x03;

///////////////////////////////////////////////////////////////////////////////
//                         Calling convention encoding                       //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_CC_normal  = 0x01;
const uint8_t DW_CC_program = 0x02;
const uint8_t DW_CC_nocall  = 0x03;

// Vendor extensions
const uint8_t DW_CC_lo_user = 0x40;
const uint8_t DW_CC_hi_user = 0xff;

///////////////////////////////////////////////////////////////////////////////
//                             Inline encoding                               //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_INL_not_inlined          = 0x00;
const uint8_t DW_INL_inlined              = 0x01;
const uint8_t DW_INL_declared_not_inlined = 0x02;
const uint8_t DW_INL_declared_inlined     = 0x03;

///////////////////////////////////////////////////////////////////////////////
//                            Ordering encoding                              //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_ORD_row_major = 0x00;
const uint8_t DW_ORD_col_major = 0x01;

///////////////////////////////////////////////////////////////////////////////
//                       Disciminant descriptor encoding                     //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_DSC_label = 0x00;
const uint8_t DW_DSC_range = 0x01;

///////////////////////////////////////////////////////////////////////////////
//                Line number standard opcode encoding                       //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_LNS_copy             = 0x01;
const uint8_t DW_LNS_advance_pc       = 0x02;
const uint8_t DW_LNS_advance_line     = 0x03;
const uint8_t DW_LNS_set_file         = 0x04;
const uint8_t DW_LNS_set_column       = 0x05;
const uint8_t DW_LNS_negate_stmt      = 0x06;
const uint8_t DW_LNS_set_basic_block  = 0x07;
const uint8_t DW_LNS_const_add_pc     = 0x08;
const uint8_t DW_LNS_fixed_advance_pc = 0x09;

// DWARF v3
const uint8_t DW_LNS_set_prologue_end   = 0x0a;
const uint8_t DW_LNS_set_epilogue_begin = 0x0b;
const uint8_t DW_LNS_set_isa            = 0x0c;

///////////////////////////////////////////////////////////////////////////////
//                Line number extended opcode encoding                       //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_LNE_end_sequence      = 0x01;
const uint8_t DW_LNE_set_address       = 0x02;
const uint8_t DW_LNE_define_file       = 0x03;

// DWARF v4
const uint8_t DW_LNE_set_discriminator = 0x04;

// Vendor extensions
const uint8_t DW_LNE_lo_user = 0x80;
const uint8_t DW_LNE_hi_user = 0xff;

///////////////////////////////////////////////////////////////////////////////
//                          Macinfo type encoding                            //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_MACINFO_define     = 0x01;
const uint8_t DW_MACINFO_undef      = 0x02;
const uint8_t DW_MACINFO_start_file = 0x03;
const uint8_t DW_MACINFO_end_file   = 0x04;
const uint8_t DW_MACINFO_vendor_ext = 0xff;

///////////////////////////////////////////////////////////////////////////////
//                       Call frame instruction encoding                     //
///////////////////////////////////////////////////////////////////////////////

// DWARF v2
const uint8_t DW_CFA_advance_loc      = (0x1 << 6);
const uint8_t DW_CFA_offset           = (0x2 << 6);
const uint8_t DW_CFA_restore          = (0x3 << 6);
const uint8_t DW_CFA_nop              = 0x00;
const uint8_t DW_CFA_set_loc          = 0x01;
const uint8_t DW_CFA_advance_loc1     = 0x02;
const uint8_t DW_CFA_advance_loc2     = 0x03;
const uint8_t DW_CFA_advance_loc4     = 0x04;
const uint8_t DW_CFA_offset_extended  = 0x05;
const uint8_t DW_CFA_restore_extended = 0x06;
const uint8_t DW_CFA_undefined        = 0x07;
const uint8_t DW_CFA_same_value       = 0x08;
const uint8_t DW_CFA_register         = 0x09;
const uint8_t DW_CFA_remember_state   = 0xa;
const uint8_t DW_CFA_restore_state    = 0x0b;
const uint8_t DW_CFA_def_cfa          = 0x0c;
const uint8_t DW_CFA_def_cfa_register = 0x0d;
const uint8_t DW_CFA_def_cfa_offset   = 0x0e;

// DWARF v3
const uint8_t DW_CFA_def_cfa_expression = 0x0f;
const uint8_t DW_CFA_expression         = 0x10;
const uint8_t DW_CFA_offset_extended_sf = 0x11;
const uint8_t DW_CFA_def_cfa_sf         = 0x12;
const uint8_t DW_CFA_def_cfa_offset_sf  = 0x13;
const uint8_t DW_CFA_val_offset         = 0x14;
const uint8_t DW_CFA_val_offset_sf      = 0x15;
const uint8_t DW_CFA_val_expression     = 0x16;

// Vendor extensions
const uint8_t DW_CFA_lo_user = 0x1c;
const uint8_t DW_CFA_hi_user = 0x3f;

// DWARF extension from Linux Standard Base Core Specification
const uint8_t DW_CFA_GNU_args_size = 0x2e;      

///////////////////////////////////////////////////////////////////////////////
//               GNU .eh_frame augmentation encoding                         //
///////////////////////////////////////////////////////////////////////////////

// GNU .eh_frame pointer encoding
// see: http://refspecs.linuxfoundation.org/LSB_3.2.0/LSB-Core-generic/LSB-Core-generic/dwarfext.html
const uint8_t DW_EH_PE_absptr = 0x00; // the Value is a literal pointer whose size is determined by the architecture
const uint8_t DW_EH_PE_omit   = 0xff; // special encoding, shall be used to indicate that no value is present

// lower 4 bits (basic encodings)
const uint8_t DW_EH_PE_uleb128 = 0x01; // unsigned LEB128 value
const uint8_t DW_EH_PE_udata2  = 0x02; // 2 bytes unsigned value
const uint8_t DW_EH_PE_udata4  = 0x03; // 4 bytes unsigned value
const uint8_t DW_EH_PE_udata8  = 0x04; // 8 bytes unsigned value
const uint8_t DW_EH_PE_sleb128 = 0x09; // Signed LEB128 value
const uint8_t DW_EH_PE_sdata2  = 0x0a; // 2 bytes signed value
const uint8_t DW_EH_PE_sdata4  = 0x0b; // 4 bytes signed value
const uint8_t DW_EH_PE_sdata8  = 0x0c; // 8 bytes signed value

const uint8_t DW_EH_PE_signed = 0x08;  // binary mask for a signed value

// upper 4 bits (modifiers)
const uint8_t DW_EH_PE_pcrel   = 0x10; // value is relative to the current program counter
const uint8_t DW_EH_PE_textrel = 0x20; // value is relative to the beginning of the .text section
const uint8_t DW_EH_PE_datarel = 0x30; // value is relative to the beginning of the .got or .eh_frame_hdr section
const uint8_t DW_EH_PE_funcrel = 0x40; // value is relative to the beginning of the function
const uint8_t DW_EH_PE_aligned = 0x50; // value is aligned to an address unit sized boundary

const uint8_t DW_EH_PE_indirect = 0x80;

///////////////////////////////////////////////////////////////////////////////
//                           Pretty print functions                          //
///////////////////////////////////////////////////////////////////////////////

const char *DWARF_GetTagName(uint16_t dw_tag);
const char *DWARF_GetATName(uint16_t dw_at);
const char *DWARF_GetFORMName(uint16_t dw_form);
const char *DWARF_GetATEName(uint8_t dw_ate);
const char *DWARF_GetFriendlyATEName(uint8_t dw_ate);
const char *DWARF_GetLANGName(uint16_t dw_lang);
const char *DWARF_GetFriendlyLANGName(uint16_t dw_lang);
const char *DWARF_GetVIRTUALITYName(uint8_t dw_virtuality);
const char *DWARF_GetVISName(uint8_t dw_vis);
const char *DWARF_GetACCESSName(uint8_t dw_access);
const char *DWARF_GetENDName(uint8_t dw_end);
const char *DWARF_GetDSName(uint8_t dw_ds);
const char *DWARF_GetIDName(uint8_t dw_id);
const char *DWARF_GetCCName(uint8_t dw_cc);
const char *DWARF_GetINLName(uint8_t dw_inl);
const char *DWARF_GetORDName(uint8_t dw_ord);
const char *DWARF_GetDSCName(uint8_t dw_dsc);
const char *DWARF_GetCHILDRENName(uint8_t dw_children);
const char *DWARF_GetOPName(uint8_t dw_opcode);

///////////////////////////////////////////////////////////////////////////////
//                           HC12 vendor extensions                          //
///////////////////////////////////////////////////////////////////////////////

namespace hc12
{
	// 68HC12 specific
	const uint8_t DW_CC_far = 0x40;           // Calling convention for banked function (i.e. called with the CALL instruction)
	const uint8_t DW_LANG_CW_DBG_ASM = 0x41;  // Assembler generated by CodeWarrior for use with the CodeWarrior Simulator/Debugger
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
