module rec Dba_piqi:
  sig
    type uint64 = int64
    type uint32 = int32
    type codeaddress_typeid_codeaddress =
      [
        | `local
        | `non_local
      ]
    type dbatag_typeid_dbatag =
      [
        | `dba_call
        | `dba_return
      ]
    type dbastopstate_typeid_dbastate =
      [
        | `ok
        | `ko
        | `undefined
        | `unsupported
      ]
    type dbaexpr_typeid_dbaexpr =
      [
        | `dba_expr_var
        | `dba_load
        | `dba_expr_cst
        | `dba_expr_unary
        | `dba_expr_binary
        | `dba_expr_restrict
        | `dba_expr_ext_u
        | `dba_expr_ext_s
        | `dba_expr_ite
        | `dba_expr_alternative
      ]
    type dbaexpr_dbaunary =
      [
        | `dba_unary_minus
        | `dba_unary_not
      ]
    type dbaexpr_dbabinary =
      [
        | `dba_plus
        | `dba_minus
        | `dba_mult_u
        | `dba_mult_s
        | `dba_div_u
        | `dba_div_s
        | `dba_mod_u
        | `dba_mod_s
        | `dba_or
        | `dba_and
        | `dba_xor
        | `dba_concat
        | `dba_lshift_u
        | `dba_rshift_u
        | `dba_rshift_s
        | `dba_left_rotate
        | `dba_right_rotate
        | `dba_eq
        | `dba_diff
        | `dba_leq_u
        | `dba_lt_u
        | `dba_geq_u
        | `dba_gt_u
        | `dba_leq_s
        | `dba_lt_s
        | `dba_geq_s
        | `dba_gt_s
      ]
    type dbacond_typeid_dbacond =
      [
        | `dba_cond_reif
        | `dba_cond_not
        | `dba_cond_and
        | `dba_cond_or
        | `dba_true
        | `dba_false
      ]
    type dba_lhs_typeid_dbalhs =
      [
        | `dba_lhs_var
        | `dba_lhs_var_restrict
        | `dba_store
      ]
    type dbainstr_typeid_instrkind =
      [
        | `dba_ik_assign
        | `dba_ik_sjump
        | `dba_ik_djump
        | `dba_ik_if
        | `dba_ik_stop
        | `dba_ik_assert
        | `dba_ik_assume
        | `dba_ik_nondet_assume
        | `dba_ik_nondet
        | `dba_ik_undef
        | `dba_ik_malloc
        | `dba_ik_free
        | `dba_ik_print
      ]
    type endianness =
      [
        | `little
        | `big
      ]
    type bitvector = Bitvector.t
    type dbacodeaddress = Dbacodeaddress.t
    type codeaddress = Codeaddress.t
    type dbatag = Dbatag.t
    type dbastopstate = Dbastopstate.t
    type dbaexpr = Dbaexpr.t
    type dbacond = Dbacond.t
    type dba_lhs = Dba_lhs.t
    type dbainstr = Dbainstr.t
    type dba_list = Dba_list.t
  end = Dba_piqi
and Bitvector:
  sig
    type t = {
      mutable bv: Dba_piqi.uint64;
      mutable size: Dba_piqi.uint32;
    }
  end = Bitvector
and Dbacodeaddress:
  sig
    type t = {
      mutable bitvector: Dba_piqi.uint64;
      mutable dbaoffset: Dba_piqi.uint32;
    }
  end = Dbacodeaddress
and Codeaddress:
  sig
    type t = {
      mutable typeid: Dba_piqi.codeaddress_typeid_codeaddress;
      mutable offset: Dba_piqi.uint32 option;
      mutable address: Dba_piqi.dbacodeaddress option;
    }
  end = Codeaddress
and Dbatag:
  sig
    type t = {
      mutable typeid: Dba_piqi.dbatag_typeid_dbatag;
      mutable address: Dba_piqi.dbacodeaddress option;
    }
  end = Dbatag
and Dbastopstate:
  sig
    type t = {
      mutable typeid: Dba_piqi.dbastopstate_typeid_dbastate;
      mutable infos: string option;
    }
  end = Dbastopstate
and Dbaexpr:
  sig
    type t = {
      mutable typeid: Dba_piqi.dbaexpr_typeid_dbaexpr;
      mutable name: string option;
      mutable size: Dba_piqi.uint32 option;
      mutable endian: Dba_piqi.endianness option;
      mutable bitvector: Dba_piqi.bitvector option;
      mutable expr1: Dba_piqi.dbaexpr option;
      mutable expr2: Dba_piqi.dbaexpr option;
      mutable unaryop: Dba_piqi.dbaexpr_dbaunary option;
      mutable binaryop: Dba_piqi.dbaexpr_dbabinary option;
      mutable low: Dba_piqi.uint32 option;
      mutable high: Dba_piqi.uint32 option;
      mutable cond: Dba_piqi.dbacond option;
    }
  end = Dbaexpr
and Dbacond:
  sig
    type t = {
      mutable typeid: Dba_piqi.dbacond_typeid_dbacond;
      mutable expr: Dba_piqi.dbaexpr option;
      mutable cond1: Dba_piqi.dbacond option;
      mutable cond2: Dba_piqi.dbacond option;
    }
  end = Dbacond
and Dba_lhs:
  sig
    type t = {
      mutable typeid: Dba_piqi.dba_lhs_typeid_dbalhs;
      mutable name: string option;
      mutable size: Dba_piqi.uint32 option;
      mutable low: Dba_piqi.uint32 option;
      mutable high: Dba_piqi.uint32 option;
      mutable endian: Dba_piqi.endianness option;
      mutable expr: Dba_piqi.dbaexpr option;
    }
  end = Dba_lhs
and Dbainstr:
  sig
    type t = {
      mutable typeid: Dba_piqi.dbainstr_typeid_instrkind;
      mutable location: Dba_piqi.dbacodeaddress;
      mutable lhs: Dba_piqi.dba_lhs option;
      mutable expr: Dba_piqi.dbaexpr option;
      mutable offset: Dba_piqi.uint32 option;
      mutable address: Dba_piqi.codeaddress option;
      mutable tags: Dba_piqi.dbatag option;
      mutable cond: Dba_piqi.dbacond option;
      mutable stopinfos: Dba_piqi.dbastopstate option;
      mutable exprs: Dba_piqi.dbaexpr list;
    }
  end = Dbainstr
and Dba_list:
  sig
    type t = {
      mutable instrs: Dba_piqi.dbainstr list;
    }
  end = Dba_list


let rec parse_uint64 x = Piqirun.int64_of_varint x
and packed_parse_uint64 x = Piqirun.int64_of_packed_varint x

and parse_uint32 x = Piqirun.int32_of_varint x
and packed_parse_uint32 x = Piqirun.int32_of_packed_varint x

and parse_string x = Piqirun.string_of_block x

and parse_bitvector x =
  let x = Piqirun.parse_record x in
  let _bv, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _size, x = Piqirun.parse_required_field 2 parse_uint32 x in
  Piqirun.check_unparsed_fields x;
  {
    Bitvector.bv = _bv;
    Bitvector.size = _size;
  }

and parse_dbacodeaddress x =
  let x = Piqirun.parse_record x in
  let _bitvector, x = Piqirun.parse_required_field 1 parse_uint64 x in
  let _dbaoffset, x = Piqirun.parse_required_field 2 parse_uint32 x in
  Piqirun.check_unparsed_fields x;
  {
    Dbacodeaddress.bitvector = _bitvector;
    Dbacodeaddress.dbaoffset = _dbaoffset;
  }

and parse_codeaddress x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_codeaddress_typeid_codeaddress x in
  let _offset, x = Piqirun.parse_optional_field 2 parse_uint32 x in
  let _address, x = Piqirun.parse_optional_field 3 parse_dbacodeaddress x in
  Piqirun.check_unparsed_fields x;
  {
    Codeaddress.typeid = _typeid;
    Codeaddress.offset = _offset;
    Codeaddress.address = _address;
  }

and parse_codeaddress_typeid_codeaddress x =
  match Piqirun.int32_of_signed_varint x with
    | 0l -> `local
    | 1l -> `non_local
    | x -> Piqirun.error_enum_const x
and packed_parse_codeaddress_typeid_codeaddress x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 0l -> `local
    | 1l -> `non_local
    | x -> Piqirun.error_enum_const x

and parse_dbatag x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_dbatag_typeid_dbatag x in
  let _address, x = Piqirun.parse_optional_field 2 parse_dbacodeaddress x in
  Piqirun.check_unparsed_fields x;
  {
    Dbatag.typeid = _typeid;
    Dbatag.address = _address;
  }

and parse_dbatag_typeid_dbatag x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `dba_call
    | 2l -> `dba_return
    | x -> Piqirun.error_enum_const x
and packed_parse_dbatag_typeid_dbatag x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `dba_call
    | 2l -> `dba_return
    | x -> Piqirun.error_enum_const x

and parse_dbastopstate x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_dbastopstate_typeid_dbastate x in
  let _infos, x = Piqirun.parse_optional_field 2 parse_string x in
  Piqirun.check_unparsed_fields x;
  {
    Dbastopstate.typeid = _typeid;
    Dbastopstate.infos = _infos;
  }

and parse_dbastopstate_typeid_dbastate x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `ok
    | 2l -> `ko
    | 3l -> `undefined
    | 4l -> `unsupported
    | x -> Piqirun.error_enum_const x
and packed_parse_dbastopstate_typeid_dbastate x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `ok
    | 2l -> `ko
    | 3l -> `undefined
    | 4l -> `unsupported
    | x -> Piqirun.error_enum_const x

and parse_dbaexpr x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_dbaexpr_typeid_dbaexpr x in
  let _name, x = Piqirun.parse_optional_field 2 parse_string x in
  let _size, x = Piqirun.parse_optional_field 3 parse_uint32 x in
  let _endian, x = Piqirun.parse_optional_field 4 parse_endianness x in
  let _bitvector, x = Piqirun.parse_optional_field 5 parse_bitvector x in
  let _expr1, x = Piqirun.parse_optional_field 6 parse_dbaexpr x in
  let _expr2, x = Piqirun.parse_optional_field 7 parse_dbaexpr x in
  let _unaryop, x = Piqirun.parse_optional_field 9 parse_dbaexpr_dbaunary x in
  let _binaryop, x = Piqirun.parse_optional_field 10 parse_dbaexpr_dbabinary x in
  let _low, x = Piqirun.parse_optional_field 11 parse_uint32 x in
  let _high, x = Piqirun.parse_optional_field 12 parse_uint32 x in
  let _cond, x = Piqirun.parse_optional_field 13 parse_dbacond x in
  Piqirun.check_unparsed_fields x;
  {
    Dbaexpr.typeid = _typeid;
    Dbaexpr.name = _name;
    Dbaexpr.size = _size;
    Dbaexpr.endian = _endian;
    Dbaexpr.bitvector = _bitvector;
    Dbaexpr.expr1 = _expr1;
    Dbaexpr.expr2 = _expr2;
    Dbaexpr.unaryop = _unaryop;
    Dbaexpr.binaryop = _binaryop;
    Dbaexpr.low = _low;
    Dbaexpr.high = _high;
    Dbaexpr.cond = _cond;
  }

and parse_dbaexpr_typeid_dbaexpr x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `dba_expr_var
    | 2l -> `dba_load
    | 3l -> `dba_expr_cst
    | 4l -> `dba_expr_unary
    | 5l -> `dba_expr_binary
    | 6l -> `dba_expr_restrict
    | 7l -> `dba_expr_ext_u
    | 8l -> `dba_expr_ext_s
    | 9l -> `dba_expr_ite
    | 10l -> `dba_expr_alternative
    | x -> Piqirun.error_enum_const x
and packed_parse_dbaexpr_typeid_dbaexpr x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `dba_expr_var
    | 2l -> `dba_load
    | 3l -> `dba_expr_cst
    | 4l -> `dba_expr_unary
    | 5l -> `dba_expr_binary
    | 6l -> `dba_expr_restrict
    | 7l -> `dba_expr_ext_u
    | 8l -> `dba_expr_ext_s
    | 9l -> `dba_expr_ite
    | 10l -> `dba_expr_alternative
    | x -> Piqirun.error_enum_const x

and parse_dbaexpr_dbaunary x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `dba_unary_minus
    | 2l -> `dba_unary_not
    | x -> Piqirun.error_enum_const x
and packed_parse_dbaexpr_dbaunary x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `dba_unary_minus
    | 2l -> `dba_unary_not
    | x -> Piqirun.error_enum_const x

and parse_dbaexpr_dbabinary x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `dba_plus
    | 2l -> `dba_minus
    | 3l -> `dba_mult_u
    | 4l -> `dba_mult_s
    | 6l -> `dba_div_u
    | 7l -> `dba_div_s
    | 8l -> `dba_mod_u
    | 9l -> `dba_mod_s
    | 10l -> `dba_or
    | 11l -> `dba_and
    | 12l -> `dba_xor
    | 13l -> `dba_concat
    | 14l -> `dba_lshift_u
    | 15l -> `dba_rshift_u
    | 16l -> `dba_rshift_s
    | 17l -> `dba_left_rotate
    | 18l -> `dba_right_rotate
    | 19l -> `dba_eq
    | 20l -> `dba_diff
    | 21l -> `dba_leq_u
    | 22l -> `dba_lt_u
    | 23l -> `dba_geq_u
    | 24l -> `dba_gt_u
    | 25l -> `dba_leq_s
    | 26l -> `dba_lt_s
    | 27l -> `dba_geq_s
    | 28l -> `dba_gt_s
    | x -> Piqirun.error_enum_const x
and packed_parse_dbaexpr_dbabinary x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `dba_plus
    | 2l -> `dba_minus
    | 3l -> `dba_mult_u
    | 4l -> `dba_mult_s
    | 6l -> `dba_div_u
    | 7l -> `dba_div_s
    | 8l -> `dba_mod_u
    | 9l -> `dba_mod_s
    | 10l -> `dba_or
    | 11l -> `dba_and
    | 12l -> `dba_xor
    | 13l -> `dba_concat
    | 14l -> `dba_lshift_u
    | 15l -> `dba_rshift_u
    | 16l -> `dba_rshift_s
    | 17l -> `dba_left_rotate
    | 18l -> `dba_right_rotate
    | 19l -> `dba_eq
    | 20l -> `dba_diff
    | 21l -> `dba_leq_u
    | 22l -> `dba_lt_u
    | 23l -> `dba_geq_u
    | 24l -> `dba_gt_u
    | 25l -> `dba_leq_s
    | 26l -> `dba_lt_s
    | 27l -> `dba_geq_s
    | 28l -> `dba_gt_s
    | x -> Piqirun.error_enum_const x

and parse_dbacond x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_dbacond_typeid_dbacond x in
  let _expr, x = Piqirun.parse_optional_field 2 parse_dbaexpr x in
  let _cond1, x = Piqirun.parse_optional_field 3 parse_dbacond x in
  let _cond2, x = Piqirun.parse_optional_field 4 parse_dbacond x in
  Piqirun.check_unparsed_fields x;
  {
    Dbacond.typeid = _typeid;
    Dbacond.expr = _expr;
    Dbacond.cond1 = _cond1;
    Dbacond.cond2 = _cond2;
  }

and parse_dbacond_typeid_dbacond x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `dba_cond_reif
    | 2l -> `dba_cond_not
    | 3l -> `dba_cond_and
    | 4l -> `dba_cond_or
    | 5l -> `dba_true
    | 6l -> `dba_false
    | x -> Piqirun.error_enum_const x
and packed_parse_dbacond_typeid_dbacond x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `dba_cond_reif
    | 2l -> `dba_cond_not
    | 3l -> `dba_cond_and
    | 4l -> `dba_cond_or
    | 5l -> `dba_true
    | 6l -> `dba_false
    | x -> Piqirun.error_enum_const x

and parse_dba_lhs x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_dba_lhs_typeid_dbalhs x in
  let _name, x = Piqirun.parse_optional_field 2 parse_string x in
  let _size, x = Piqirun.parse_optional_field 3 parse_uint32 x in
  let _low, x = Piqirun.parse_optional_field 4 parse_uint32 x in
  let _high, x = Piqirun.parse_optional_field 5 parse_uint32 x in
  let _endian, x = Piqirun.parse_optional_field 6 parse_endianness x in
  let _expr, x = Piqirun.parse_optional_field 7 parse_dbaexpr x in
  Piqirun.check_unparsed_fields x;
  {
    Dba_lhs.typeid = _typeid;
    Dba_lhs.name = _name;
    Dba_lhs.size = _size;
    Dba_lhs.low = _low;
    Dba_lhs.high = _high;
    Dba_lhs.endian = _endian;
    Dba_lhs.expr = _expr;
  }

and parse_dba_lhs_typeid_dbalhs x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `dba_lhs_var
    | 2l -> `dba_lhs_var_restrict
    | 3l -> `dba_store
    | x -> Piqirun.error_enum_const x
and packed_parse_dba_lhs_typeid_dbalhs x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `dba_lhs_var
    | 2l -> `dba_lhs_var_restrict
    | 3l -> `dba_store
    | x -> Piqirun.error_enum_const x

and parse_dbainstr x =
  let x = Piqirun.parse_record x in
  let _typeid, x = Piqirun.parse_required_field 1 parse_dbainstr_typeid_instrkind x in
  let _location, x = Piqirun.parse_required_field 2 parse_dbacodeaddress x in
  let _lhs, x = Piqirun.parse_optional_field 3 parse_dba_lhs x in
  let _expr, x = Piqirun.parse_optional_field 4 parse_dbaexpr x in
  let _offset, x = Piqirun.parse_optional_field 5 parse_uint32 x in
  let _address, x = Piqirun.parse_optional_field 6 parse_codeaddress x in
  let _tags, x = Piqirun.parse_optional_field 7 parse_dbatag x in
  let _cond, x = Piqirun.parse_optional_field 8 parse_dbacond x in
  let _stopinfos, x = Piqirun.parse_optional_field 9 parse_dbastopstate x in
  let _exprs, x = Piqirun.parse_repeated_field 10 parse_dbaexpr x in
  Piqirun.check_unparsed_fields x;
  {
    Dbainstr.typeid = _typeid;
    Dbainstr.location = _location;
    Dbainstr.lhs = _lhs;
    Dbainstr.expr = _expr;
    Dbainstr.offset = _offset;
    Dbainstr.address = _address;
    Dbainstr.tags = _tags;
    Dbainstr.cond = _cond;
    Dbainstr.stopinfos = _stopinfos;
    Dbainstr.exprs = _exprs;
  }

and parse_dbainstr_typeid_instrkind x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `dba_ik_assign
    | 2l -> `dba_ik_sjump
    | 3l -> `dba_ik_djump
    | 4l -> `dba_ik_if
    | 5l -> `dba_ik_stop
    | 6l -> `dba_ik_assert
    | 7l -> `dba_ik_assume
    | 8l -> `dba_ik_nondet_assume
    | 9l -> `dba_ik_nondet
    | 10l -> `dba_ik_undef
    | 11l -> `dba_ik_malloc
    | 12l -> `dba_ik_free
    | 13l -> `dba_ik_print
    | x -> Piqirun.error_enum_const x
and packed_parse_dbainstr_typeid_instrkind x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `dba_ik_assign
    | 2l -> `dba_ik_sjump
    | 3l -> `dba_ik_djump
    | 4l -> `dba_ik_if
    | 5l -> `dba_ik_stop
    | 6l -> `dba_ik_assert
    | 7l -> `dba_ik_assume
    | 8l -> `dba_ik_nondet_assume
    | 9l -> `dba_ik_nondet
    | 10l -> `dba_ik_undef
    | 11l -> `dba_ik_malloc
    | 12l -> `dba_ik_free
    | 13l -> `dba_ik_print
    | x -> Piqirun.error_enum_const x

and parse_dba_list x =
  let x = Piqirun.parse_record x in
  let _instrs, x = Piqirun.parse_repeated_field 1 parse_dbainstr x in
  Piqirun.check_unparsed_fields x;
  {
    Dba_list.instrs = _instrs;
  }

and parse_endianness x =
  match Piqirun.int32_of_signed_varint x with
    | 1l -> `little
    | 2l -> `big
    | x -> Piqirun.error_enum_const x
and packed_parse_endianness x =
  match Piqirun.int32_of_packed_signed_varint x with
    | 1l -> `little
    | 2l -> `big
    | x -> Piqirun.error_enum_const x


let rec gen__uint64 code x = Piqirun.int64_to_varint code x
and packed_gen__uint64 x = Piqirun.int64_to_packed_varint x

and gen__uint32 code x = Piqirun.int32_to_varint code x
and packed_gen__uint32 x = Piqirun.int32_to_packed_varint x

and gen__string code x = Piqirun.string_to_block code x

and gen__bitvector code x =
  let _bv = Piqirun.gen_required_field 1 gen__uint64 x.Bitvector.bv in
  let _size = Piqirun.gen_required_field 2 gen__uint32 x.Bitvector.size in
  Piqirun.gen_record code (_bv :: _size :: [])

and gen__dbacodeaddress code x =
  let _bitvector = Piqirun.gen_required_field 1 gen__uint64 x.Dbacodeaddress.bitvector in
  let _dbaoffset = Piqirun.gen_required_field 2 gen__uint32 x.Dbacodeaddress.dbaoffset in
  Piqirun.gen_record code (_bitvector :: _dbaoffset :: [])

and gen__codeaddress code x =
  let _typeid = Piqirun.gen_required_field 1 gen__codeaddress_typeid_codeaddress x.Codeaddress.typeid in
  let _offset = Piqirun.gen_optional_field 2 gen__uint32 x.Codeaddress.offset in
  let _address = Piqirun.gen_optional_field 3 gen__dbacodeaddress x.Codeaddress.address in
  Piqirun.gen_record code (_typeid :: _offset :: _address :: [])

and gen__codeaddress_typeid_codeaddress code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `local -> 0l
    | `non_local -> 1l
  )
and packed_gen__codeaddress_typeid_codeaddress x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `local -> 0l
    | `non_local -> 1l
  )

and gen__dbatag code x =
  let _typeid = Piqirun.gen_required_field 1 gen__dbatag_typeid_dbatag x.Dbatag.typeid in
  let _address = Piqirun.gen_optional_field 2 gen__dbacodeaddress x.Dbatag.address in
  Piqirun.gen_record code (_typeid :: _address :: [])

and gen__dbatag_typeid_dbatag code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `dba_call -> 1l
    | `dba_return -> 2l
  )
and packed_gen__dbatag_typeid_dbatag x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `dba_call -> 1l
    | `dba_return -> 2l
  )

and gen__dbastopstate code x =
  let _typeid = Piqirun.gen_required_field 1 gen__dbastopstate_typeid_dbastate x.Dbastopstate.typeid in
  let _infos = Piqirun.gen_optional_field 2 gen__string x.Dbastopstate.infos in
  Piqirun.gen_record code (_typeid :: _infos :: [])

and gen__dbastopstate_typeid_dbastate code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `ok -> 1l
    | `ko -> 2l
    | `undefined -> 3l
    | `unsupported -> 4l
  )
and packed_gen__dbastopstate_typeid_dbastate x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `ok -> 1l
    | `ko -> 2l
    | `undefined -> 3l
    | `unsupported -> 4l
  )

and gen__dbaexpr code x =
  let _typeid = Piqirun.gen_required_field 1 gen__dbaexpr_typeid_dbaexpr x.Dbaexpr.typeid in
  let _name = Piqirun.gen_optional_field 2 gen__string x.Dbaexpr.name in
  let _size = Piqirun.gen_optional_field 3 gen__uint32 x.Dbaexpr.size in
  let _endian = Piqirun.gen_optional_field 4 gen__endianness x.Dbaexpr.endian in
  let _bitvector = Piqirun.gen_optional_field 5 gen__bitvector x.Dbaexpr.bitvector in
  let _expr1 = Piqirun.gen_optional_field 6 gen__dbaexpr x.Dbaexpr.expr1 in
  let _expr2 = Piqirun.gen_optional_field 7 gen__dbaexpr x.Dbaexpr.expr2 in
  let _unaryop = Piqirun.gen_optional_field 9 gen__dbaexpr_dbaunary x.Dbaexpr.unaryop in
  let _binaryop = Piqirun.gen_optional_field 10 gen__dbaexpr_dbabinary x.Dbaexpr.binaryop in
  let _low = Piqirun.gen_optional_field 11 gen__uint32 x.Dbaexpr.low in
  let _high = Piqirun.gen_optional_field 12 gen__uint32 x.Dbaexpr.high in
  let _cond = Piqirun.gen_optional_field 13 gen__dbacond x.Dbaexpr.cond in
  Piqirun.gen_record code (_typeid :: _name :: _size :: _endian :: _bitvector :: _expr1 :: _expr2 :: _unaryop :: _binaryop :: _low :: _high :: _cond :: [])

and gen__dbaexpr_typeid_dbaexpr code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `dba_expr_var -> 1l
    | `dba_load -> 2l
    | `dba_expr_cst -> 3l
    | `dba_expr_unary -> 4l
    | `dba_expr_binary -> 5l
    | `dba_expr_restrict -> 6l
    | `dba_expr_ext_u -> 7l
    | `dba_expr_ext_s -> 8l
    | `dba_expr_ite -> 9l
    | `dba_expr_alternative -> 10l
  )
and packed_gen__dbaexpr_typeid_dbaexpr x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `dba_expr_var -> 1l
    | `dba_load -> 2l
    | `dba_expr_cst -> 3l
    | `dba_expr_unary -> 4l
    | `dba_expr_binary -> 5l
    | `dba_expr_restrict -> 6l
    | `dba_expr_ext_u -> 7l
    | `dba_expr_ext_s -> 8l
    | `dba_expr_ite -> 9l
    | `dba_expr_alternative -> 10l
  )

and gen__dbaexpr_dbaunary code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `dba_unary_minus -> 1l
    | `dba_unary_not -> 2l
  )
and packed_gen__dbaexpr_dbaunary x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `dba_unary_minus -> 1l
    | `dba_unary_not -> 2l
  )

and gen__dbaexpr_dbabinary code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `dba_plus -> 1l
    | `dba_minus -> 2l
    | `dba_mult_u -> 3l
    | `dba_mult_s -> 4l
    | `dba_div_u -> 6l
    | `dba_div_s -> 7l
    | `dba_mod_u -> 8l
    | `dba_mod_s -> 9l
    | `dba_or -> 10l
    | `dba_and -> 11l
    | `dba_xor -> 12l
    | `dba_concat -> 13l
    | `dba_lshift_u -> 14l
    | `dba_rshift_u -> 15l
    | `dba_rshift_s -> 16l
    | `dba_left_rotate -> 17l
    | `dba_right_rotate -> 18l
    | `dba_eq -> 19l
    | `dba_diff -> 20l
    | `dba_leq_u -> 21l
    | `dba_lt_u -> 22l
    | `dba_geq_u -> 23l
    | `dba_gt_u -> 24l
    | `dba_leq_s -> 25l
    | `dba_lt_s -> 26l
    | `dba_geq_s -> 27l
    | `dba_gt_s -> 28l
  )
and packed_gen__dbaexpr_dbabinary x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `dba_plus -> 1l
    | `dba_minus -> 2l
    | `dba_mult_u -> 3l
    | `dba_mult_s -> 4l
    | `dba_div_u -> 6l
    | `dba_div_s -> 7l
    | `dba_mod_u -> 8l
    | `dba_mod_s -> 9l
    | `dba_or -> 10l
    | `dba_and -> 11l
    | `dba_xor -> 12l
    | `dba_concat -> 13l
    | `dba_lshift_u -> 14l
    | `dba_rshift_u -> 15l
    | `dba_rshift_s -> 16l
    | `dba_left_rotate -> 17l
    | `dba_right_rotate -> 18l
    | `dba_eq -> 19l
    | `dba_diff -> 20l
    | `dba_leq_u -> 21l
    | `dba_lt_u -> 22l
    | `dba_geq_u -> 23l
    | `dba_gt_u -> 24l
    | `dba_leq_s -> 25l
    | `dba_lt_s -> 26l
    | `dba_geq_s -> 27l
    | `dba_gt_s -> 28l
  )

and gen__dbacond code x =
  let _typeid = Piqirun.gen_required_field 1 gen__dbacond_typeid_dbacond x.Dbacond.typeid in
  let _expr = Piqirun.gen_optional_field 2 gen__dbaexpr x.Dbacond.expr in
  let _cond1 = Piqirun.gen_optional_field 3 gen__dbacond x.Dbacond.cond1 in
  let _cond2 = Piqirun.gen_optional_field 4 gen__dbacond x.Dbacond.cond2 in
  Piqirun.gen_record code (_typeid :: _expr :: _cond1 :: _cond2 :: [])

and gen__dbacond_typeid_dbacond code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `dba_cond_reif -> 1l
    | `dba_cond_not -> 2l
    | `dba_cond_and -> 3l
    | `dba_cond_or -> 4l
    | `dba_true -> 5l
    | `dba_false -> 6l
  )
and packed_gen__dbacond_typeid_dbacond x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `dba_cond_reif -> 1l
    | `dba_cond_not -> 2l
    | `dba_cond_and -> 3l
    | `dba_cond_or -> 4l
    | `dba_true -> 5l
    | `dba_false -> 6l
  )

and gen__dba_lhs code x =
  let _typeid = Piqirun.gen_required_field 1 gen__dba_lhs_typeid_dbalhs x.Dba_lhs.typeid in
  let _name = Piqirun.gen_optional_field 2 gen__string x.Dba_lhs.name in
  let _size = Piqirun.gen_optional_field 3 gen__uint32 x.Dba_lhs.size in
  let _low = Piqirun.gen_optional_field 4 gen__uint32 x.Dba_lhs.low in
  let _high = Piqirun.gen_optional_field 5 gen__uint32 x.Dba_lhs.high in
  let _endian = Piqirun.gen_optional_field 6 gen__endianness x.Dba_lhs.endian in
  let _expr = Piqirun.gen_optional_field 7 gen__dbaexpr x.Dba_lhs.expr in
  Piqirun.gen_record code (_typeid :: _name :: _size :: _low :: _high :: _endian :: _expr :: [])

and gen__dba_lhs_typeid_dbalhs code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `dba_lhs_var -> 1l
    | `dba_lhs_var_restrict -> 2l
    | `dba_store -> 3l
  )
and packed_gen__dba_lhs_typeid_dbalhs x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `dba_lhs_var -> 1l
    | `dba_lhs_var_restrict -> 2l
    | `dba_store -> 3l
  )

and gen__dbainstr code x =
  let _typeid = Piqirun.gen_required_field 1 gen__dbainstr_typeid_instrkind x.Dbainstr.typeid in
  let _location = Piqirun.gen_required_field 2 gen__dbacodeaddress x.Dbainstr.location in
  let _lhs = Piqirun.gen_optional_field 3 gen__dba_lhs x.Dbainstr.lhs in
  let _expr = Piqirun.gen_optional_field 4 gen__dbaexpr x.Dbainstr.expr in
  let _offset = Piqirun.gen_optional_field 5 gen__uint32 x.Dbainstr.offset in
  let _address = Piqirun.gen_optional_field 6 gen__codeaddress x.Dbainstr.address in
  let _tags = Piqirun.gen_optional_field 7 gen__dbatag x.Dbainstr.tags in
  let _cond = Piqirun.gen_optional_field 8 gen__dbacond x.Dbainstr.cond in
  let _stopinfos = Piqirun.gen_optional_field 9 gen__dbastopstate x.Dbainstr.stopinfos in
  let _exprs = Piqirun.gen_repeated_field 10 gen__dbaexpr x.Dbainstr.exprs in
  Piqirun.gen_record code (_typeid :: _location :: _lhs :: _expr :: _offset :: _address :: _tags :: _cond :: _stopinfos :: _exprs :: [])

and gen__dbainstr_typeid_instrkind code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `dba_ik_assign -> 1l
    | `dba_ik_sjump -> 2l
    | `dba_ik_djump -> 3l
    | `dba_ik_if -> 4l
    | `dba_ik_stop -> 5l
    | `dba_ik_assert -> 6l
    | `dba_ik_assume -> 7l
    | `dba_ik_nondet_assume -> 8l
    | `dba_ik_nondet -> 9l
    | `dba_ik_undef -> 10l
    | `dba_ik_malloc -> 11l
    | `dba_ik_free -> 12l
    | `dba_ik_print -> 13l
  )
and packed_gen__dbainstr_typeid_instrkind x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `dba_ik_assign -> 1l
    | `dba_ik_sjump -> 2l
    | `dba_ik_djump -> 3l
    | `dba_ik_if -> 4l
    | `dba_ik_stop -> 5l
    | `dba_ik_assert -> 6l
    | `dba_ik_assume -> 7l
    | `dba_ik_nondet_assume -> 8l
    | `dba_ik_nondet -> 9l
    | `dba_ik_undef -> 10l
    | `dba_ik_malloc -> 11l
    | `dba_ik_free -> 12l
    | `dba_ik_print -> 13l
  )

and gen__dba_list code x =
  let _instrs = Piqirun.gen_repeated_field 1 gen__dbainstr x.Dba_list.instrs in
  Piqirun.gen_record code (_instrs :: [])

and gen__endianness code x =
  Piqirun.int32_to_signed_varint code (match x with
    | `little -> 1l
    | `big -> 2l
  )
and packed_gen__endianness x =
  Piqirun.int32_to_packed_signed_varint (match x with
    | `little -> 1l
    | `big -> 2l
  )


let gen_uint64 x = gen__uint64 (-1) x
let gen_uint32 x = gen__uint32 (-1) x
let gen_string x = gen__string (-1) x
let gen_bitvector x = gen__bitvector (-1) x
let gen_dbacodeaddress x = gen__dbacodeaddress (-1) x
let gen_codeaddress x = gen__codeaddress (-1) x
let gen_codeaddress_typeid_codeaddress x = gen__codeaddress_typeid_codeaddress (-1) x
let gen_dbatag x = gen__dbatag (-1) x
let gen_dbatag_typeid_dbatag x = gen__dbatag_typeid_dbatag (-1) x
let gen_dbastopstate x = gen__dbastopstate (-1) x
let gen_dbastopstate_typeid_dbastate x = gen__dbastopstate_typeid_dbastate (-1) x
let gen_dbaexpr x = gen__dbaexpr (-1) x
let gen_dbaexpr_typeid_dbaexpr x = gen__dbaexpr_typeid_dbaexpr (-1) x
let gen_dbaexpr_dbaunary x = gen__dbaexpr_dbaunary (-1) x
let gen_dbaexpr_dbabinary x = gen__dbaexpr_dbabinary (-1) x
let gen_dbacond x = gen__dbacond (-1) x
let gen_dbacond_typeid_dbacond x = gen__dbacond_typeid_dbacond (-1) x
let gen_dba_lhs x = gen__dba_lhs (-1) x
let gen_dba_lhs_typeid_dbalhs x = gen__dba_lhs_typeid_dbalhs (-1) x
let gen_dbainstr x = gen__dbainstr (-1) x
let gen_dbainstr_typeid_instrkind x = gen__dbainstr_typeid_instrkind (-1) x
let gen_dba_list x = gen__dba_list (-1) x
let gen_endianness x = gen__endianness (-1) x


let rec default_uint64 () = 0L
and default_uint32 () = 0l
and default_string () = ""
and default_bitvector () =
  {
    Bitvector.bv = default_uint64 ();
    Bitvector.size = default_uint32 ();
  }
and default_dbacodeaddress () =
  {
    Dbacodeaddress.bitvector = default_uint64 ();
    Dbacodeaddress.dbaoffset = default_uint32 ();
  }
and default_codeaddress () =
  {
    Codeaddress.typeid = default_codeaddress_typeid_codeaddress ();
    Codeaddress.offset = None;
    Codeaddress.address = None;
  }
and default_codeaddress_typeid_codeaddress () = `local
and default_dbatag () =
  {
    Dbatag.typeid = default_dbatag_typeid_dbatag ();
    Dbatag.address = None;
  }
and default_dbatag_typeid_dbatag () = `dba_call
and default_dbastopstate () =
  {
    Dbastopstate.typeid = default_dbastopstate_typeid_dbastate ();
    Dbastopstate.infos = None;
  }
and default_dbastopstate_typeid_dbastate () = `ok
and default_dbaexpr () =
  {
    Dbaexpr.typeid = default_dbaexpr_typeid_dbaexpr ();
    Dbaexpr.name = None;
    Dbaexpr.size = None;
    Dbaexpr.endian = None;
    Dbaexpr.bitvector = None;
    Dbaexpr.expr1 = None;
    Dbaexpr.expr2 = None;
    Dbaexpr.unaryop = None;
    Dbaexpr.binaryop = None;
    Dbaexpr.low = None;
    Dbaexpr.high = None;
    Dbaexpr.cond = None;
  }
and default_dbaexpr_typeid_dbaexpr () = `dba_expr_var
and default_dbaexpr_dbaunary () = `dba_unary_minus
and default_dbaexpr_dbabinary () = `dba_plus
and default_dbacond () =
  {
    Dbacond.typeid = default_dbacond_typeid_dbacond ();
    Dbacond.expr = None;
    Dbacond.cond1 = None;
    Dbacond.cond2 = None;
  }
and default_dbacond_typeid_dbacond () = `dba_cond_reif
and default_dba_lhs () =
  {
    Dba_lhs.typeid = default_dba_lhs_typeid_dbalhs ();
    Dba_lhs.name = None;
    Dba_lhs.size = None;
    Dba_lhs.low = None;
    Dba_lhs.high = None;
    Dba_lhs.endian = None;
    Dba_lhs.expr = None;
  }
and default_dba_lhs_typeid_dbalhs () = `dba_lhs_var
and default_dbainstr () =
  {
    Dbainstr.typeid = default_dbainstr_typeid_instrkind ();
    Dbainstr.location = default_dbacodeaddress ();
    Dbainstr.lhs = None;
    Dbainstr.expr = None;
    Dbainstr.offset = None;
    Dbainstr.address = None;
    Dbainstr.tags = None;
    Dbainstr.cond = None;
    Dbainstr.stopinfos = None;
    Dbainstr.exprs = [];
  }
and default_dbainstr_typeid_instrkind () = `dba_ik_assign
and default_dba_list () =
  {
    Dba_list.instrs = [];
  }
and default_endianness () = `little


let piqi = "\226\202\2304\003dba\226\231\249\238\001\rpiqi/dba.piqi\162\244\146\155\011\003dba\218\244\134\182\012\164\001\138\233\142\251\014\157\001\210\203\242$A\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\002bv\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\tbitvector\218\244\134\182\012\181\001\138\233\142\251\014\174\001\210\203\242$H\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tbitvector\208\215\133\174\005\000\210\171\158\194\006\006uint64\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$H\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\tdbaoffset\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\014dbacodeaddress\218\244\134\182\012\151\002\138\233\142\251\014\144\002\210\203\242$]\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\030codeaddress-typeid-codeaddress\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006offset\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007address\208\215\133\174\005\000\210\171\158\194\006\014dbacodeaddress\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\011codeaddress\218\244\134\182\012\151\001\138\176\205\197\001\144\001\218\164\238\191\004\030codeaddress-typeid-codeaddress\170\183\218\222\005-\232\146\150q\000\152\247\223\136\002\000\234\188\204\215\002\017codeaddress_Local\218\164\238\191\004\005Local\170\183\218\222\0053\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\020codeaddress_NonLocal\218\164\238\191\004\bNonLocal\218\244\134\182\012\190\001\138\233\142\251\014\183\001\210\203\242$S\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\020dbatag-typeid-dbatag\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007address\208\215\133\174\005\000\210\171\158\194\006\014dbacodeaddress\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\006dbatag\218\244\134\182\012\137\001\138\176\205\197\001\130\001\218\164\238\191\004\020dbatag-typeid-dbatag\170\183\218\222\005,\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\014dbatag_DbaCall\218\164\238\191\004\007DbaCall\170\183\218\222\0050\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\016dbatag_DbaReturn\218\164\238\191\004\tDbaReturn\218\244\134\182\012\194\001\138\233\142\251\014\187\001\210\203\242$[\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\028dbastopstate-typeid-dbastate\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005infos\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\012dbastopstate\218\244\134\182\012\129\002\138\176\205\197\001\250\001\218\164\238\191\004\028dbastopstate-typeid-dbastate\170\183\218\222\005(\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\015dbastopstate_Ok\218\164\238\191\004\002Ok\170\183\218\222\005(\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\015dbastopstate_Ko\218\164\238\191\004\002Ko\170\183\218\222\0056\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\022dbastopstate_Undefined\218\164\238\191\004\tUndefined\170\183\218\222\005:\232\146\150q\b\152\247\223\136\002\000\234\188\204\215\002\024dbastopstate_Unsupported\218\164\238\191\004\011Unsupported\218\244\134\182\012\180\007\138\233\142\251\014\173\007\210\203\242$U\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\022dbaexpr-typeid-dbaexpr\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006endian\208\215\133\174\005\000\210\171\158\194\006\nendianness\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tbitvector\208\215\133\174\005\000\210\171\158\194\006\tbitvector\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005expr1\208\215\133\174\005\000\210\171\158\194\006\007dbaexpr\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005expr2\208\215\133\174\005\000\210\171\158\194\006\007dbaexpr\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$P\232\146\150q\018\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007unaryop\208\215\133\174\005\000\210\171\158\194\006\016dbaexpr-dbaunary\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$R\232\146\150q\020\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\bbinaryop\208\215\133\174\005\000\210\171\158\194\006\017dbaexpr-dbabinary\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\022\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\003low\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\024\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004high\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\026\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004cond\208\215\133\174\005\000\210\171\158\194\006\007dbacond\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007dbaexpr\218\244\134\182\012\255\004\138\176\205\197\001\248\004\218\164\238\191\004\022dbaexpr-typeid-dbaexpr\170\183\218\222\0053\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\018dbaexpr_DbaExprVar\218\164\238\191\004\nDbaExprVar\170\183\218\222\005-\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaLoad\218\164\238\191\004\007DbaLoad\170\183\218\222\0053\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\018dbaexpr_DbaExprCst\218\164\238\191\004\nDbaExprCst\170\183\218\222\0057\232\146\150q\b\152\247\223\136\002\000\234\188\204\215\002\020dbaexpr_DbaExprUnary\218\164\238\191\004\012DbaExprUnary\170\183\218\222\0059\232\146\150q\n\152\247\223\136\002\000\234\188\204\215\002\021dbaexpr_DbaExprBinary\218\164\238\191\004\rDbaExprBinary\170\183\218\222\005=\232\146\150q\012\152\247\223\136\002\000\234\188\204\215\002\023dbaexpr_DbaExprRestrict\218\164\238\191\004\015DbaExprRestrict\170\183\218\222\0055\232\146\150q\014\152\247\223\136\002\000\234\188\204\215\002\019dbaexpr_DbaExprExtU\218\164\238\191\004\011DbaExprExtU\170\183\218\222\0055\232\146\150q\016\152\247\223\136\002\000\234\188\204\215\002\019dbaexpr_DbaExprExtS\218\164\238\191\004\011DbaExprExtS\170\183\218\222\0053\232\146\150q\018\152\247\223\136\002\000\234\188\204\215\002\018dbaexpr_DbaExprIte\218\164\238\191\004\nDbaExprIte\170\183\218\222\005C\232\146\150q\020\152\247\223\136\002\000\234\188\204\215\002\026dbaexpr_DbaExprAlternative\218\164\238\191\004\018DbaExprAlternative\218\244\134\182\012\151\001\138\176\205\197\001\144\001\218\164\238\191\004\016dbaexpr-dbaunary\170\183\218\222\0059\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\021dbaexpr_DbaUnaryMinus\218\164\238\191\004\rDbaUnaryMinus\170\183\218\222\0055\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\019dbaexpr_DbaUnaryNot\218\164\238\191\004\011DbaUnaryNot\218\244\134\182\012\161\011\138\176\205\197\001\154\011\218\164\238\191\004\017dbaexpr-dbabinary\170\183\218\222\005-\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaPlus\218\164\238\191\004\007DbaPlus\170\183\218\222\005/\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\016dbaexpr_DbaMinus\218\164\238\191\004\bDbaMinus\170\183\218\222\005/\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\016dbaexpr_DbaMultU\218\164\238\191\004\bDbaMultU\170\183\218\222\005/\232\146\150q\b\152\247\223\136\002\000\234\188\204\215\002\016dbaexpr_DbaMultS\218\164\238\191\004\bDbaMultS\170\183\218\222\005-\232\146\150q\012\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaDivU\218\164\238\191\004\007DbaDivU\170\183\218\222\005-\232\146\150q\014\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaDivS\218\164\238\191\004\007DbaDivS\170\183\218\222\005-\232\146\150q\016\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaModU\218\164\238\191\004\007DbaModU\170\183\218\222\005-\232\146\150q\018\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaModS\218\164\238\191\004\007DbaModS\170\183\218\222\005)\232\146\150q\020\152\247\223\136\002\000\234\188\204\215\002\rdbaexpr_DbaOr\218\164\238\191\004\005DbaOr\170\183\218\222\005+\232\146\150q\022\152\247\223\136\002\000\234\188\204\215\002\014dbaexpr_DbaAnd\218\164\238\191\004\006DbaAnd\170\183\218\222\005+\232\146\150q\024\152\247\223\136\002\000\234\188\204\215\002\014dbaexpr_DbaXor\218\164\238\191\004\006DbaXor\170\183\218\222\0051\232\146\150q\026\152\247\223\136\002\000\234\188\204\215\002\017dbaexpr_DbaConcat\218\164\238\191\004\tDbaConcat\170\183\218\222\0053\232\146\150q\028\152\247\223\136\002\000\234\188\204\215\002\018dbaexpr_DbaLShiftU\218\164\238\191\004\nDbaLShiftU\170\183\218\222\0053\232\146\150q\030\152\247\223\136\002\000\234\188\204\215\002\018dbaexpr_DbaRShiftU\218\164\238\191\004\nDbaRShiftU\170\183\218\222\0053\232\146\150q \152\247\223\136\002\000\234\188\204\215\002\018dbaexpr_DbaRShiftS\218\164\238\191\004\nDbaRShiftS\170\183\218\222\0059\232\146\150q\"\152\247\223\136\002\000\234\188\204\215\002\021dbaexpr_DbaLeftRotate\218\164\238\191\004\rDbaLeftRotate\170\183\218\222\005;\232\146\150q$\152\247\223\136\002\000\234\188\204\215\002\022dbaexpr_DbaRightRotate\218\164\238\191\004\014DbaRightRotate\170\183\218\222\005)\232\146\150q&\152\247\223\136\002\000\234\188\204\215\002\rdbaexpr_DbaEq\218\164\238\191\004\005DbaEq\170\183\218\222\005-\232\146\150q(\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaDiff\218\164\238\191\004\007DbaDiff\170\183\218\222\005-\232\146\150q*\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaLeqU\218\164\238\191\004\007DbaLeqU\170\183\218\222\005+\232\146\150q,\152\247\223\136\002\000\234\188\204\215\002\014dbaexpr_DbaLtU\218\164\238\191\004\006DbaLtU\170\183\218\222\005-\232\146\150q.\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaGeqU\218\164\238\191\004\007DbaGeqU\170\183\218\222\005+\232\146\150q0\152\247\223\136\002\000\234\188\204\215\002\014dbaexpr_DbaGtU\218\164\238\191\004\006DbaGtU\170\183\218\222\005-\232\146\150q2\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaLeqS\218\164\238\191\004\007DbaLeqS\170\183\218\222\005+\232\146\150q4\152\247\223\136\002\000\234\188\204\215\002\014dbaexpr_DbaLtS\218\164\238\191\004\006DbaLtS\170\183\218\222\005-\232\146\150q6\152\247\223\136\002\000\234\188\204\215\002\015dbaexpr_DbaGeqS\218\164\238\191\004\007DbaGeqS\170\183\218\222\005+\232\146\150q8\152\247\223\136\002\000\234\188\204\215\002\014dbaexpr_DbaGtS\218\164\238\191\004\006DbaGtS\218\244\134\182\012\203\002\138\233\142\251\014\196\002\210\203\242$U\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\022dbacond-typeid-dbacond\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004expr\208\215\133\174\005\000\210\171\158\194\006\007dbaexpr\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005cond1\208\215\133\174\005\000\210\171\158\194\006\007dbacond\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\005cond2\208\215\133\174\005\000\210\171\158\194\006\007dbacond\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\007dbacond\218\244\134\182\012\239\002\138\176\205\197\001\232\002\218\164\238\191\004\022dbacond-typeid-dbacond\170\183\218\222\0055\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\019dbacond_DbaCondReif\218\164\238\191\004\011DbaCondReif\170\183\218\222\0053\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\018dbacond_DbaCondNot\218\164\238\191\004\nDbaCondNot\170\183\218\222\0053\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\018dbacond_DbaCondAnd\218\164\238\191\004\nDbaCondAnd\170\183\218\222\0051\232\146\150q\b\152\247\223\136\002\000\234\188\204\215\002\017dbacond_DbaCondOr\218\164\238\191\004\tDbaCondOr\170\183\218\222\005-\232\146\150q\n\152\247\223\136\002\000\234\188\204\215\002\015dbacond_DbaTrue\218\164\238\191\004\007DbaTrue\170\183\218\222\005/\232\146\150q\012\152\247\223\136\002\000\234\188\204\215\002\016dbacond_DbaFalse\218\164\238\191\004\bDbaFalse\218\244\134\182\012\161\004\138\233\142\251\014\154\004\210\203\242$S\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\020dbaLhs-typeid-dbalhs\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004name\208\215\133\174\005\000\210\171\158\194\006\006string\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004size\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\003low\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004high\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$I\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006endian\208\215\133\174\005\000\210\171\158\194\006\nendianness\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004expr\208\215\133\174\005\000\210\171\158\194\006\007dbaexpr\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\006dbaLhs\218\244\134\182\012\209\001\138\176\205\197\001\202\001\218\164\238\191\004\020dbaLhs-typeid-dbalhs\170\183\218\222\0050\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\016dbaLhs_DbaLhsVar\218\164\238\191\004\tDbaLhsVar\170\183\218\222\005@\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\024dbaLhs_DbaLhsVarRestrict\218\164\238\191\004\017DbaLhsVarRestrict\170\183\218\222\005.\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\015dbaLhs_DbaStore\218\164\238\191\004\bDbaStore\218\244\134\182\012\158\006\138\233\142\251\014\151\006\210\203\242$X\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\006typeid\208\215\133\174\005\000\210\171\158\194\006\025dbainstr-typeid-instrkind\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$O\232\146\150q\004\152\247\223\136\002\000\152\182\154\152\004\223\162\138\147\001\232\243\204\157\004\000\218\164\238\191\004\blocation\208\215\133\174\005\000\210\171\158\194\006\014dbacodeaddress\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$B\232\146\150q\006\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\003lhs\208\215\133\174\005\000\210\171\158\194\006\006dbaLhs\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\b\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004expr\208\215\133\174\005\000\210\171\158\194\006\007dbaexpr\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\n\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\006offset\208\215\133\174\005\000\210\171\158\194\006\006uint32\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$K\232\146\150q\012\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\007address\208\215\133\174\005\000\210\171\158\194\006\011codeaddress\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$C\232\146\150q\014\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004tags\208\215\133\174\005\000\210\171\158\194\006\006dbatag\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$D\232\146\150q\016\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\004cond\208\215\133\174\005\000\210\171\158\194\006\007dbacond\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$N\232\146\150q\018\152\247\223\136\002\000\152\182\154\152\004\160\223\186\243\001\232\243\204\157\004\000\218\164\238\191\004\tstopinfos\208\215\133\174\005\000\210\171\158\194\006\012dbastopstate\192\139\160\247\t\000\136\158\147\199\014\000\210\203\242$E\232\146\150q\020\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\005exprs\208\215\133\174\005\000\210\171\158\194\006\007dbaexpr\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bdbainstr\218\244\134\182\012\166\006\138\176\205\197\001\159\006\218\164\238\191\004\025dbainstr-typeid-instrkind\170\183\218\222\0056\232\146\150q\002\152\247\223\136\002\000\234\188\204\215\002\020dbainstr_DbaIkAssign\218\164\238\191\004\011DbaIkAssign\170\183\218\222\0054\232\146\150q\004\152\247\223\136\002\000\234\188\204\215\002\019dbainstr_DbaIkSJump\218\164\238\191\004\nDbaIkSJump\170\183\218\222\0054\232\146\150q\006\152\247\223\136\002\000\234\188\204\215\002\019dbainstr_DbaIkDJump\218\164\238\191\004\nDbaIkDJump\170\183\218\222\005.\232\146\150q\b\152\247\223\136\002\000\234\188\204\215\002\016dbainstr_DbaIkIf\218\164\238\191\004\007DbaIkIf\170\183\218\222\0052\232\146\150q\n\152\247\223\136\002\000\234\188\204\215\002\018dbainstr_DbaIkStop\218\164\238\191\004\tDbaIkStop\170\183\218\222\0056\232\146\150q\012\152\247\223\136\002\000\234\188\204\215\002\020dbainstr_DbaIkAssert\218\164\238\191\004\011DbaIkAssert\170\183\218\222\0056\232\146\150q\014\152\247\223\136\002\000\234\188\204\215\002\020dbainstr_DbaIkAssume\218\164\238\191\004\011DbaIkAssume\170\183\218\222\005B\232\146\150q\016\152\247\223\136\002\000\234\188\204\215\002\026dbainstr_DbaIkNondetAssume\218\164\238\191\004\017DbaIkNondetAssume\170\183\218\222\0056\232\146\150q\018\152\247\223\136\002\000\234\188\204\215\002\020dbainstr_DbaIkNondet\218\164\238\191\004\011DbaIkNondet\170\183\218\222\0054\232\146\150q\020\152\247\223\136\002\000\234\188\204\215\002\019dbainstr_DbaIkUndef\218\164\238\191\004\nDbaIkUndef\170\183\218\222\0056\232\146\150q\022\152\247\223\136\002\000\234\188\204\215\002\020dbainstr_DbaIkMalloc\218\164\238\191\004\011DbaIkMalloc\170\183\218\222\0052\232\146\150q\024\152\247\223\136\002\000\234\188\204\215\002\018dbainstr_DbaIkFree\218\164\238\191\004\tDbaIkFree\170\183\218\222\0054\232\146\150q\026\152\247\223\136\002\000\234\188\204\215\002\019dbainstr_DbaIkPrint\218\164\238\191\004\nDbaIkPrint\218\244\134\182\012`\138\233\142\251\014Z\210\203\242$G\232\146\150q\002\152\247\223\136\002\000\152\182\154\152\004\250\248\214\130\001\232\243\204\157\004\000\218\164\238\191\004\006instrs\208\215\133\174\005\000\210\171\158\194\006\bdbainstr\192\139\160\247\t\000\136\158\147\199\014\000\218\164\238\191\004\bdba-list\218\244\134\182\012M\138\176\205\197\001G\218\164\238\191\004\nendianness\170\183\218\222\005\023\232\146\150q\002\152\247\223\136\002\000\218\164\238\191\004\006Little\170\183\218\222\005\020\232\146\150q\004\152\247\223\136\002\000\218\164\238\191\004\003Big"
include Dba_piqi
