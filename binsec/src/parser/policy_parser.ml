
module MenhirBasics = struct
  
  exception Error = Parsing.Parse_error
  
  type token = 
    | XOR
    | WILDCARD
    | UMINUS
    | TRUE
    | TOACTION
    | TAINTP
    | TAINTI
    | TAINTCHECK
    | SYMB
    | SUPER
    | SUBTERM
    | STORELOAD
    | RSHIFTU
    | RSHIFTS
    | RROTATE
    | RPAR
    | RBRACKET
    | RBRACE
    | PROPS
    | PROPC
    | PROP
    | PLUS
    | OR2
    | OR
    | NOT2
    | NOT
    | NEXT
    | NEQ
    | MULTU
    | MULTS
    | MODU
    | MODS
    | MINUS
    | LTU
    | LTS
    | LSHIFT
    | LROTATE
    | LPAR
    | LEU
    | LES
    | LBRACKET
    | LBRACE
    | INTERVALSEP
    | INT of (
# 93 "parser/policy_parser.mly"
       (string)
# 54 "parser/policy_parser.ml"
  )
    | INFER
    | IFJUMP
    | IDENT of (
# 94 "parser/policy_parser.mly"
       (string)
# 61 "parser/policy_parser.ml"
  )
    | HEXA of (
# 95 "parser/policy_parser.mly"
       (string * int)
# 66 "parser/policy_parser.ml"
  )
    | GTU
    | GTS
    | GEU
    | GES
    | FALSE
    | EXTU
    | EXTS
    | EQQ
    | EOF
    | ELSE
    | DIVU
    | DIVS
    | DEFAULT
    | DCOLON
    | CONCAT
    | CONC
    | COMMA
    | ASSIGN
    | AND2
    | AND
  
end

include MenhirBasics

let _eRR =
  MenhirBasics.Error

type _menhir_env = {
  _menhir_lexer: Lexing.lexbuf -> token;
  _menhir_lexbuf: Lexing.lexbuf;
  _menhir_token: token;
  mutable _menhir_error: bool
}

and _menhir_state = 
  | MenhirState163
  | MenhirState161
  | MenhirState157
  | MenhirState156
  | MenhirState155
  | MenhirState153
  | MenhirState147
  | MenhirState145
  | MenhirState143
  | MenhirState140
  | MenhirState138
  | MenhirState137
  | MenhirState134
  | MenhirState133
  | MenhirState126
  | MenhirState123
  | MenhirState121
  | MenhirState120
  | MenhirState112
  | MenhirState111
  | MenhirState109
  | MenhirState108
  | MenhirState106
  | MenhirState105
  | MenhirState104
  | MenhirState99
  | MenhirState94
  | MenhirState93
  | MenhirState92
  | MenhirState90
  | MenhirState89
  | MenhirState87
  | MenhirState81
  | MenhirState80
  | MenhirState79
  | MenhirState78
  | MenhirState77
  | MenhirState76
  | MenhirState74
  | MenhirState73
  | MenhirState72
  | MenhirState43
  | MenhirState42
  | MenhirState41
  | MenhirState36
  | MenhirState31
  | MenhirState30
  | MenhirState29
  | MenhirState28
  | MenhirState27
  | MenhirState25
  | MenhirState19
  | MenhirState10
  | MenhirState5
  | MenhirState2
  | MenhirState0

# 22 "parser/policy_parser.mly"
  
  open Dba
  open Policy_type



  let infer_size_variable (name:string): int =
    match name with
    | "eax" | "ebx" | "ecx" | "edx" | "edi"
    | "esi" | "ebp" | "esp" -> 32
    | "btemp" -> 8
    | "stemp" -> 16
    | "temp" -> 32
    | "dtemp" -> 64
    |  _ -> failwith "Unknown variable"

  let infer_variable_lhs (name:string): LValue.t =
    let open Dba.LValue in
    let open Size.Bit in
    let first_char = String.get name 0 in
    if first_char = '_' || first_char =  '?' ||  first_char  = '!' then
      let name = if first_char = '_' then "*" else name in
      var name ~bitsize:(Size.Bit.create 0)
    else
      let reg name = var name ~bitsize:bits32 in
      let restrict = _restrict in
      match name with
      | "al" -> restrict "eax" bits32 0 7
      | "ah" -> restrict "eax" bits32 8 15
      | "ax" -> restrict "eax" bits32 0 15
      | "eax" -> reg "eax"
      | "bl" -> restrict "ebx" bits32 0 7
      | "bh" -> restrict "ebx" bits32 8 15
      | "bx" -> restrict "ebx" bits32 0 15
      | "ebx" -> reg "ebx"
      | "cl" -> restrict "ecx" bits32 0 7
      | "ch" -> restrict "ecx" bits32 8 15
      | "cx" -> restrict "ecx" bits32 0 15
      | "ecx" -> reg "ecx"
      | "dl" -> restrict "edx" bits32 0 7
      | "dh" -> restrict "edx" bits32 8 15
      | "dx" -> restrict "edx" bits32 0 15
      | "edx" -> reg "edx"
      | "di"  -> restrict "edi" bits32 0 15
      | "edi" -> reg "edi"
      | "si" -> restrict "esi" bits32 0 15
      | "esi" -> reg "esi"
      | "bp"  -> restrict "ebp" bits32 0 15
      | "ebp" -> reg "ebp"
      | "sp"  -> restrict "esp" bits32 0 15
      | "esp" -> reg "esp"
      | "btemp" -> var "btemp" ~bitsize:bits8
      | "stemp" -> var "stemp" ~bitsize:bits16
      | "temp" ->  var "temp"  ~bitsize:bits32
      | "dtemp" -> var "dtemp" ~bitsize:bits64
      | _ -> Kernel_options.Logger.error "Unknown LHS variable";
             raise Parsing.Parse_error


# 221 "parser/policy_parser.ml"

let rec _menhir_goto_taint : _menhir_env -> 'ttv_tail -> 'tv_taint -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (('freshtv587 * _menhir_state)) * 'tv_taint) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COMMA ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv583 * _menhir_state)) * 'tv_taint) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState133
        | EXTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState133
        | HEXA _v ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState133 _v
        | IDENT _v ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState133 _v
        | IFJUMP ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState133
        | INT _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState133 _v
        | LBRACE ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState133
        | LPAR ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState133
        | MINUS ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState133
        | NOT ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState133
        | STORELOAD ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState133
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState133) : 'freshtv584)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv585 * _menhir_state)) * 'tv_taint) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv586)) : 'freshtv588)

and _menhir_run143 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_sigma_p -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        _menhir_run138 _menhir_env (Obj.magic _menhir_stack) MenhirState143
    | NOT2 ->
        _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState143
    | TAINTCHECK ->
        _menhir_run128 _menhir_env (Obj.magic _menhir_stack) MenhirState143
    | WILDCARD ->
        _menhir_run127 _menhir_env (Obj.magic _menhir_stack) MenhirState143
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState143

and _menhir_run145 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_sigma_p -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        _menhir_run138 _menhir_env (Obj.magic _menhir_stack) MenhirState145
    | NOT2 ->
        _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState145
    | TAINTCHECK ->
        _menhir_run128 _menhir_env (Obj.magic _menhir_stack) MenhirState145
    | WILDCARD ->
        _menhir_run127 _menhir_env (Obj.magic _menhir_stack) MenhirState145
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState145

and _menhir_goto_bin_op : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_bin_op -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState155 | MenhirState157 | MenhirState134 | MenhirState121 | MenhirState109 | MenhirState112 | MenhirState105 | MenhirState99 | MenhirState94 | MenhirState93 | MenhirState92 | MenhirState90 | MenhirState81 | MenhirState78 | MenhirState80 | MenhirState76 | MenhirState74 | MenhirState73 | MenhirState43 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv579 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | EXTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | HEXA _v ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState72 _v
        | IDENT _v ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState72 _v
        | IFJUMP ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | INT _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState72 _v
        | LBRACE ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | LPAR ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | MINUS ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | NOT ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | STORELOAD ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState72) : 'freshtv580)
    | MenhirState87 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv581 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState89
        | EXTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState89
        | HEXA _v ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState89 _v
        | IDENT _v ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState89 _v
        | IFJUMP ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState89
        | INT _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState89 _v
        | LBRACE ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState89
        | LPAR ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState89
        | MINUS ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState89
        | NOT ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState89
        | STORELOAD ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState89
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState89) : 'freshtv582)
    | _ ->
        _menhir_fail ()

and _menhir_run127 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv577) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_sigma_p = 
# 157 "parser/policy_parser.mly"
             ( SigmaWildcard )
# 388 "parser/policy_parser.ml"
     in
    _menhir_goto_sigma_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv578)

and _menhir_run128 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv573 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | TAINTI ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv565) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv563) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_taint = 
# 167 "parser/policy_parser.mly"
           ( Taint_types.TaintI )
# 414 "parser/policy_parser.ml"
             in
            _menhir_goto_taint _menhir_env _menhir_stack _v) : 'freshtv564)) : 'freshtv566)
        | TAINTP ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv569) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv567) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_taint = 
# 166 "parser/policy_parser.mly"
           ( Taint_types.TaintP )
# 427 "parser/policy_parser.ml"
             in
            _menhir_goto_taint _menhir_env _menhir_stack _v) : 'freshtv568)) : 'freshtv570)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv571 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv572)) : 'freshtv574)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv575 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv576)

and _menhir_run136 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv559 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run138 _menhir_env (Obj.magic _menhir_stack) MenhirState137
        | NOT2 ->
            _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState137
        | TAINTCHECK ->
            _menhir_run128 _menhir_env (Obj.magic _menhir_stack) MenhirState137
        | WILDCARD ->
            _menhir_run127 _menhir_env (Obj.magic _menhir_stack) MenhirState137
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState137) : 'freshtv560)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv561 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv562)

and _menhir_run138 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        _menhir_run138 _menhir_env (Obj.magic _menhir_stack) MenhirState138
    | NOT2 ->
        _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState138
    | TAINTCHECK ->
        _menhir_run128 _menhir_env (Obj.magic _menhir_stack) MenhirState138
    | WILDCARD ->
        _menhir_run127 _menhir_env (Obj.magic _menhir_stack) MenhirState138
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState138

and _menhir_goto_sigma_p : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_sigma_p -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState138 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv515 * _menhir_state) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND2 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv509 * _menhir_state) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAR ->
                _menhir_run138 _menhir_env (Obj.magic _menhir_stack) MenhirState147
            | NOT2 ->
                _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState147
            | TAINTCHECK ->
                _menhir_run128 _menhir_env (Obj.magic _menhir_stack) MenhirState147
            | WILDCARD ->
                _menhir_run127 _menhir_env (Obj.magic _menhir_stack) MenhirState147
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState147) : 'freshtv510)
        | OR2 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv511 * _menhir_state) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAR ->
                _menhir_run138 _menhir_env (Obj.magic _menhir_stack) MenhirState140
            | NOT2 ->
                _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState140
            | TAINTCHECK ->
                _menhir_run128 _menhir_env (Obj.magic _menhir_stack) MenhirState140
            | WILDCARD ->
                _menhir_run127 _menhir_env (Obj.magic _menhir_stack) MenhirState140
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState140) : 'freshtv512)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv513 * _menhir_state) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv514)) : 'freshtv516)
    | MenhirState140 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv523 * _menhir_state) * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND2 ->
            _menhir_run145 _menhir_env (Obj.magic _menhir_stack)
        | OR2 ->
            _menhir_run143 _menhir_env (Obj.magic _menhir_stack)
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv519 * _menhir_state) * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv517 * _menhir_state) * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s), _, (_2 : 'tv_sigma_p)), _, (_4 : 'tv_sigma_p)) = _menhir_stack in
            let _5 = () in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_sigma_p = 
# 161 "parser/policy_parser.mly"
                                  ( SigmaBinary(Policy_type.Or,_2,_4) )
# 572 "parser/policy_parser.ml"
             in
            _menhir_goto_sigma_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv518)) : 'freshtv520)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv521 * _menhir_state) * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv522)) : 'freshtv524)
    | MenhirState143 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv529 * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND2 ->
            _menhir_run145 _menhir_env (Obj.magic _menhir_stack)
        | OR2 ->
            _menhir_run143 _menhir_env (Obj.magic _menhir_stack)
        | RPAR | TOACTION ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv525 * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_sigma_p)), _, (_3 : 'tv_sigma_p)) = _menhir_stack in
            let _2 = () in
            let _v : 'tv_sigma_p = 
# 159 "parser/policy_parser.mly"
                        ( SigmaBinary(Policy_type.Or,_1,_3) )
# 600 "parser/policy_parser.ml"
             in
            _menhir_goto_sigma_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv526)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv527 * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv528)) : 'freshtv530)
    | MenhirState145 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv535 * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND2 ->
            _menhir_run145 _menhir_env (Obj.magic _menhir_stack)
        | OR2 ->
            _menhir_run143 _menhir_env (Obj.magic _menhir_stack)
        | RPAR | TOACTION ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv531 * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_sigma_p)), _, (_3 : 'tv_sigma_p)) = _menhir_stack in
            let _2 = () in
            let _v : 'tv_sigma_p = 
# 160 "parser/policy_parser.mly"
                         ( SigmaBinary(Policy_type.And,_1,_3) )
# 628 "parser/policy_parser.ml"
             in
            _menhir_goto_sigma_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv532)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv533 * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv534)) : 'freshtv536)
    | MenhirState147 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv543 * _menhir_state) * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND2 ->
            _menhir_run145 _menhir_env (Obj.magic _menhir_stack)
        | OR2 ->
            _menhir_run143 _menhir_env (Obj.magic _menhir_stack)
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv539 * _menhir_state) * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv537 * _menhir_state) * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s), _, (_2 : 'tv_sigma_p)), _, (_4 : 'tv_sigma_p)) = _menhir_stack in
            let _5 = () in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_sigma_p = 
# 162 "parser/policy_parser.mly"
                                   ( SigmaBinary(Policy_type.And,_2,_4) )
# 661 "parser/policy_parser.ml"
             in
            _menhir_goto_sigma_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv538)) : 'freshtv540)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv541 * _menhir_state) * _menhir_state * 'tv_sigma_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv542)) : 'freshtv544)
    | MenhirState137 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv551 * _menhir_state)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND2 ->
            _menhir_run145 _menhir_env (Obj.magic _menhir_stack)
        | OR2 ->
            _menhir_run143 _menhir_env (Obj.magic _menhir_stack)
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv547 * _menhir_state)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv545 * _menhir_state)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (_3 : 'tv_sigma_p)) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_sigma_p = 
# 158 "parser/policy_parser.mly"
                           ( SigmaUnary(Policy_type.Not,_3) )
# 694 "parser/policy_parser.ml"
             in
            _menhir_goto_sigma_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv546)) : 'freshtv548)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv549 * _menhir_state)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv550)) : 'freshtv552)
    | MenhirState126 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv557 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND2 ->
            _menhir_run145 _menhir_env (Obj.magic _menhir_stack)
        | OR2 ->
            _menhir_run143 _menhir_env (Obj.magic _menhir_stack)
        | TOACTION ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((((('freshtv553 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | CONC ->
                _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState153
            | PROP ->
                _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState153
            | PROPC ->
                _menhir_run13 _menhir_env (Obj.magic _menhir_stack) MenhirState153
            | PROPS ->
                _menhir_run12 _menhir_env (Obj.magic _menhir_stack) MenhirState153
            | SYMB ->
                _menhir_run11 _menhir_env (Obj.magic _menhir_stack) MenhirState153
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState153) : 'freshtv554)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((((('freshtv555 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p)) * _menhir_state * 'tv_sigma_p) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv556)) : 'freshtv558)
    | _ ->
        _menhir_fail ()

and _menhir_goto_inst : _menhir_env -> 'ttv_tail -> 'tv_inst -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv507) = Obj.magic _menhir_stack in
    let (_v : 'tv_inst) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv505) = Obj.magic _menhir_stack in
    let ((_1 : 'tv_inst) : 'tv_inst) = _v in
    ((let _v : 'tv_instruction_p = 
# 149 "parser/policy_parser.mly"
             ( InstPattern(_1) )
# 755 "parser/policy_parser.ml"
     in
    _menhir_goto_instruction_p _menhir_env _menhir_stack _v) : 'freshtv506)) : 'freshtv508)

and _menhir_run44 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv503) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 308 "parser/policy_parser.mly"
       ( Dba.Binary_op.Xor )
# 769 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv504)

and _menhir_run45 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv501) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 329 "parser/policy_parser.mly"
            ( Dba.Binary_op.RShiftU )
# 783 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv502)

and _menhir_run46 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv499) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 330 "parser/policy_parser.mly"
            ( Dba.Binary_op.RShiftS )
# 797 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv500)

and _menhir_run47 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv497) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 332 "parser/policy_parser.mly"
            ( Dba.Binary_op.RightRotate )
# 811 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv498)

and _menhir_run48 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv495) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 309 "parser/policy_parser.mly"
        ( Dba.Binary_op.Plus )
# 825 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv496)

and _menhir_run49 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv493) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 306 "parser/policy_parser.mly"
       ( Dba.Binary_op.Or )
# 839 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv494)

and _menhir_run50 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv491) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 319 "parser/policy_parser.mly"
       ( Dba.Binary_op.Diff )
# 853 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv492)

and _menhir_run51 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv489) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 311 "parser/policy_parser.mly"
         ( Dba.Binary_op.Mult )
# 867 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv490)

and _menhir_run52 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv487) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 312 "parser/policy_parser.mly"
         ( Dba.Binary_op.Mult )
# 881 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv488)

and _menhir_run53 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv485) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 304 "parser/policy_parser.mly"
        ( Dba.Binary_op.ModU )
# 895 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv486)

and _menhir_run54 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv483) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 305 "parser/policy_parser.mly"
        ( Dba.Binary_op.ModS )
# 909 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv484)

and _menhir_run55 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv481) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 310 "parser/policy_parser.mly"
         ( Dba.Binary_op.Minus )
# 923 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv482)

and _menhir_run56 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv479) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 321 "parser/policy_parser.mly"
       ( Dba.Binary_op.LtU )
# 937 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv480)

and _menhir_run57 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv477) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 325 "parser/policy_parser.mly"
       ( Dba.Binary_op.LtS )
# 951 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv478)

and _menhir_run58 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv475) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 328 "parser/policy_parser.mly"
            ( Dba.Binary_op.LShift )
# 965 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv476)

and _menhir_run59 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv473) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 331 "parser/policy_parser.mly"
            ( Dba.Binary_op.LeftRotate )
# 979 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv474)

and _menhir_run60 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv471) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 320 "parser/policy_parser.mly"
       ( Dba.Binary_op.LeqU )
# 993 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv472)

and _menhir_run61 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv469) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 324 "parser/policy_parser.mly"
       ( Dba.Binary_op.LeqS )
# 1007 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv470)

and _menhir_run63 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv467) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 323 "parser/policy_parser.mly"
       ( Dba.Binary_op.GtU )
# 1021 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv468)

and _menhir_run64 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv465) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 327 "parser/policy_parser.mly"
       ( Dba.Binary_op.GtS )
# 1035 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv466)

and _menhir_run65 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv463) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 322 "parser/policy_parser.mly"
       ( Dba.Binary_op.GeqU )
# 1049 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv464)

and _menhir_run66 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv461) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 326 "parser/policy_parser.mly"
       ( Dba.Binary_op.GeqS )
# 1063 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv462)

and _menhir_run67 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv459) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 318 "parser/policy_parser.mly"
       ( Dba.Binary_op.Eq )
# 1077 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv460)

and _menhir_run68 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv457) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 313 "parser/policy_parser.mly"
        ( Dba.Binary_op.DivU )
# 1091 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv458)

and _menhir_run69 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv455) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 314 "parser/policy_parser.mly"
        ( Dba.Binary_op.DivS )
# 1105 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv456)

and _menhir_run70 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv453) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 316 "parser/policy_parser.mly"
          ( Dba.Binary_op.Concat )
# 1119 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv454)

and _menhir_run71 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv451) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 307 "parser/policy_parser.mly"
       ( Dba.Binary_op.And )
# 1133 "parser/policy_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv452)

and _menhir_goto_rules : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_rules -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv445 * _menhir_state * 'tv_rules) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv441 * _menhir_state * 'tv_rules) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv439 * _menhir_state * 'tv_rules) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (_1 : 'tv_rules)) = _menhir_stack in
            let _2 = () in
            let _v : (
# 111 "parser/policy_parser.mly"
      (Policy_type.policy)
# 1157 "parser/policy_parser.ml"
            ) = 
# 117 "parser/policy_parser.mly"
              ( _1 )
# 1161 "parser/policy_parser.ml"
             in
            _menhir_goto_policy _menhir_env _menhir_stack _menhir_s _v) : 'freshtv440)) : 'freshtv442)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv443 * _menhir_state * 'tv_rules) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv444)) : 'freshtv446)
    | MenhirState19 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv449 * _menhir_state * 'tv_rule) * _menhir_state * 'tv_rules) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv447 * _menhir_state * 'tv_rule) * _menhir_state * 'tv_rules) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_rule)), _, (_2 : 'tv_rules)) = _menhir_stack in
        let _v : 'tv_rules = 
# 120 "parser/policy_parser.mly"
               ( _1::_2 )
# 1180 "parser/policy_parser.ml"
         in
        _menhir_goto_rules _menhir_env _menhir_stack _menhir_s _v) : 'freshtv448)) : 'freshtv450)
    | _ ->
        _menhir_fail ()

and _menhir_goto_expression_p : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_expression_p -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (((('freshtv437 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | DCOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv433 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run138 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | NOT2 ->
            _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | TAINTCHECK ->
            _menhir_run128 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | WILDCARD ->
            _menhir_run127 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState126) : 'freshtv434)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv435 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv436)) : 'freshtv438)

and _menhir_goto_expr : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState42 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv295 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv293 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState43 in
            let (_v : (
# 93 "parser/policy_parser.mly"
       (string)
# 1255 "parser/policy_parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv291 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((_3 : (
# 93 "parser/policy_parser.mly"
       (string)
# 1264 "parser/policy_parser.ml"
            )) : (
# 93 "parser/policy_parser.mly"
       (string)
# 1268 "parser/policy_parser.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 278 "parser/policy_parser.mly"
                  (
    let size = int_of_string _3 in
    Dba.Expr.sext size _2
  )
# 1278 "parser/policy_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv292)) : 'freshtv294)
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState43) : 'freshtv296)
    | MenhirState72 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv299 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | COMMA | DCOLON | ELSE | EXTS | EXTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LPAR | NEXT | NOT | RBRACKET | RPAR | STORELOAD | SUBTERM ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv297 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (e1 : 'tv_expr)), _, (bop : 'tv_bin_op)), _, (e2 : 'tv_expr)) = _menhir_stack in
            let _v : 'tv_expr = 
# 285 "parser/policy_parser.mly"
                                           (
    let sz1 = Dba.Expr.size_of e1 in
    let sz2 = Dba.Expr.size_of e2 in
    if sz1 = 0 && sz2 <> 0 || sz1 <> 0 && sz2 = 0 then
      match bop with
      | Dba.Binary_op.ModU | Dba.Binary_op.ModS | Dba.Binary_op.Or
        | Dba.Binary_op.And | Dba.Binary_op.Xor | Dba.Binary_op.Plus
        | Dba.Binary_op.Minus | Dba.Binary_op.Mult
        | Dba.Binary_op.DivU | Dba.Binary_op.DivS ->
        if sz1 = 0 then
          Dba.Expr.binary bop (Parse_helpers.patch_expr_size e1 sz2) e2
        else
          Dba.Expr.binary bop e1 (Parse_helpers.patch_expr_size e2 sz1)
      | _ -> Dba.Expr.binary bop e1 e2
    else Dba.Expr.binary bop e1 e2
  )
# 1403 "parser/policy_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv298)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState73) : 'freshtv300)
    | MenhirState41 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv305 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv303 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState74 in
            let (_v : (
# 93 "parser/policy_parser.mly"
       (string)
# 1441 "parser/policy_parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv301 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((_3 : (
# 93 "parser/policy_parser.mly"
       (string)
# 1450 "parser/policy_parser.ml"
            )) : (
# 93 "parser/policy_parser.mly"
       (string)
# 1454 "parser/policy_parser.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 274 "parser/policy_parser.mly"
                  (
    let size = int_of_string _3 in
    Dba.Expr.uext size _2
  )
# 1464 "parser/policy_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv302)) : 'freshtv304)
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState74) : 'freshtv306)
    | MenhirState106 | MenhirState36 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv309 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState76
        | EXTS | EXTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LPAR | NEXT | NOT | STORELOAD ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv307 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (e : 'tv_expr)) = _menhir_stack in
            let _v : 'tv_cond = 
# 337 "parser/policy_parser.mly"
           ( e )
# 1574 "parser/policy_parser.ml"
             in
            _menhir_goto_cond _menhir_env _menhir_stack _menhir_s _v) : 'freshtv308)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState76) : 'freshtv310)
    | MenhirState77 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv313 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | ELSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv311 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState78 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState79
            | EXTU ->
                _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState79
            | HEXA _v ->
                _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState79 _v
            | IDENT _v ->
                _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState79 _v
            | IFJUMP ->
                _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState79
            | INT _v ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState79 _v
            | LBRACE ->
                _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState79
            | LPAR ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState79
            | MINUS ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState79
            | NOT ->
                _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState79
            | STORELOAD ->
                _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState79
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState79) : 'freshtv312)
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState78
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState78) : 'freshtv314)
    | MenhirState79 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv317 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState80
        | COMMA | DCOLON | ELSE | EXTS | EXTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LPAR | NEXT | NOT | RBRACKET | RPAR | STORELOAD | SUBTERM ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv315 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((((_menhir_stack, _menhir_s), _, (_2 : 'tv_cond)), _, (_3 : 'tv_expr)), _), _, (_5 : 'tv_expr)) = _menhir_stack in
            let _4 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 282 "parser/policy_parser.mly"
                               ( Dba.Expr.ite _2 _3 _5 )
# 1748 "parser/policy_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv316)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState80) : 'freshtv318)
    | MenhirState31 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv339 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv337 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState81 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv333 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                let (_v : (
# 93 "parser/policy_parser.mly"
       (string)
# 1777 "parser/policy_parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | COMMA ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv329 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1788 "parser/policy_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | INT _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv325 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1798 "parser/policy_parser.ml"
                        ))) = Obj.magic _menhir_stack in
                        let (_v : (
# 93 "parser/policy_parser.mly"
       (string)
# 1803 "parser/policy_parser.ml"
                        )) = _v in
                        ((let _menhir_stack = (_menhir_stack, _v) in
                        let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RBRACE ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv321 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1814 "parser/policy_parser.ml"
                            ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1818 "parser/policy_parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv319 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1825 "parser/policy_parser.ml"
                            ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1829 "parser/policy_parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let (((((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)), _), (_4 : (
# 93 "parser/policy_parser.mly"
       (string)
# 1834 "parser/policy_parser.ml"
                            ))), (_6 : (
# 93 "parser/policy_parser.mly"
       (string)
# 1838 "parser/policy_parser.ml"
                            ))) = _menhir_stack in
                            let _7 = () in
                            let _5 = () in
                            let _3 = () in
                            let _1 = () in
                            let _v : 'tv_expr = 
# 269 "parser/policy_parser.mly"
                                           (
    let off1 = int_of_string _4 in
    let off2 = int_of_string _6 in
    Dba.Expr.restrict off1 off2 _2
  )
# 1851 "parser/policy_parser.ml"
                             in
                            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv320)) : 'freshtv322)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv323 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1861 "parser/policy_parser.ml"
                            ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1865 "parser/policy_parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let (((_menhir_stack, _menhir_s), _), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv324)) : 'freshtv326)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv327 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1876 "parser/policy_parser.ml"
                        ))) = Obj.magic _menhir_stack in
                        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv328)) : 'freshtv330)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv331 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 1887 "parser/policy_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv332)) : 'freshtv334)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv335 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv336)) : 'freshtv338)
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState81) : 'freshtv340)
    | MenhirState30 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv345 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv343 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState87 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv341 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 283 "parser/policy_parser.mly"
                   ( _2 )
# 2020 "parser/policy_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv342)) : 'freshtv344)
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState87
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState87) : 'freshtv346)
    | MenhirState89 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv351 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv349 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState90 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv347 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((((_menhir_stack, _menhir_s), _, (e1 : 'tv_expr)), _, (bop : 'tv_bin_op)), _, (e2 : 'tv_expr)) = _menhir_stack in
            let _5 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 285 "parser/policy_parser.mly"
                                           (
    let sz1 = Dba.Expr.size_of e1 in
    let sz2 = Dba.Expr.size_of e2 in
    if sz1 = 0 && sz2 <> 0 || sz1 <> 0 && sz2 = 0 then
      match bop with
      | Dba.Binary_op.ModU | Dba.Binary_op.ModS | Dba.Binary_op.Or
        | Dba.Binary_op.And | Dba.Binary_op.Xor | Dba.Binary_op.Plus
        | Dba.Binary_op.Minus | Dba.Binary_op.Mult
        | Dba.Binary_op.DivU | Dba.Binary_op.DivS ->
        if sz1 = 0 then
          Dba.Expr.binary bop (Parse_helpers.patch_expr_size e1 sz2) e2
        else
          Dba.Expr.binary bop e1 (Parse_helpers.patch_expr_size e2 sz1)
      | _ -> Dba.Expr.binary bop e1 e2
    else Dba.Expr.binary bop e1 e2
  )
# 2116 "parser/policy_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv348)) : 'freshtv350)
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState90
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState90) : 'freshtv352)
    | MenhirState29 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv355 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv353 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_expr = 
# 268 "parser/policy_parser.mly"
                            ( Dba.Expr.uminus _2 )
# 2141 "parser/policy_parser.ml"
         in
        _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv354)) : 'freshtv356)
    | MenhirState28 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv359 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv357 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_expr = 
# 267 "parser/policy_parser.mly"
                            ( Dba.Expr.lognot _2 )
# 2154 "parser/policy_parser.ml"
         in
        _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv358)) : 'freshtv360)
    | MenhirState27 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv377 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv371 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState94 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv367 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                let (_v : (
# 93 "parser/policy_parser.mly"
       (string)
# 2179 "parser/policy_parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RBRACKET ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv363 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 2190 "parser/policy_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv361 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 2197 "parser/policy_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((((_menhir_stack, _menhir_s), _, (_3 : 'tv_expr)), _), (_5 : (
# 93 "parser/policy_parser.mly"
       (string)
# 2202 "parser/policy_parser.ml"
                    ))) = _menhir_stack in
                    let _6 = () in
                    let _4 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : 'tv_expr = 
# 260 "parser/policy_parser.mly"
                                               (
    let size = int_of_string _5 |> Size.Byte.create in
    Dba.Expr.load size Machine.LittleEndian _3
  )
# 2214 "parser/policy_parser.ml"
                     in
                    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv362)) : 'freshtv364)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv365 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 2224 "parser/policy_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv366)) : 'freshtv368)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv369 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv370)) : 'freshtv372)
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | RBRACKET ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv375 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState94 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv373 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((_menhir_stack, _menhir_s), _, (_3 : 'tv_expr)) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 264 "parser/policy_parser.mly"
                                     (
    Dba.Expr.load (Size.Byte.create 0) Machine.LittleEndian _3
  )
# 2296 "parser/policy_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv374)) : 'freshtv376)
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState94
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState94) : 'freshtv378)
    | MenhirState25 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv395)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv389)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState99 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv385)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                let (_v : (
# 93 "parser/policy_parser.mly"
       (string)
# 2333 "parser/policy_parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RBRACKET ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv381)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 2344 "parser/policy_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv379)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 2351 "parser/policy_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _, (e : 'tv_expr)), _), (x : (
# 93 "parser/policy_parser.mly"
       (string)
# 2356 "parser/policy_parser.ml"
                    ))) = _menhir_stack in
                    let _5 = () in
                    let _1_inlined1 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : 'tv_lhs = let sz =
                      let _1 = _1_inlined1 in
                      let x = 
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2367 "parser/policy_parser.ml"
                       in
                      
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 2372 "parser/policy_parser.ml"
                      
                    in
                    
# 230 "parser/policy_parser.mly"
  (
    let bytesize =
      match sz with
      | None -> Size.Byte.create 0
      | Some v -> Size.Byte.of_string v
   in Dba.LValue.store bytesize Machine.LittleEndian e
  )
# 2384 "parser/policy_parser.ml"
                     in
                    _menhir_goto_lhs _menhir_env _menhir_stack _v) : 'freshtv380)) : 'freshtv382)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv383)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 93 "parser/policy_parser.mly"
       (string)
# 2394 "parser/policy_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv384)) : 'freshtv386)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv387)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv388)) : 'freshtv390)
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | RBRACKET ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv393)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState99 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv391)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let (_menhir_stack, _, (e : 'tv_expr)) = _menhir_stack in
            let _5 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_lhs = let sz = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 2464 "parser/policy_parser.ml"
             in
            
# 230 "parser/policy_parser.mly"
  (
    let bytesize =
      match sz with
      | None -> Size.Byte.create 0
      | Some v -> Size.Byte.of_string v
   in Dba.LValue.store bytesize Machine.LittleEndian e
  )
# 2475 "parser/policy_parser.ml"
             in
            _menhir_goto_lhs _menhir_env _menhir_stack _v) : 'freshtv392)) : 'freshtv394)
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState99) : 'freshtv396)
    | MenhirState104 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv399) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState105
        | DCOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv397) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, (_2 : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_inst = 
# 207 "parser/policy_parser.mly"
              ( Dba.Instr.dynamic_jump _2 )
# 2558 "parser/policy_parser.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _v) : 'freshtv398)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState105) : 'freshtv400)
    | MenhirState108 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv407) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | ELSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv405) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState109 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | NEXT ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv401) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | EXTS ->
                    _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState111
                | EXTU ->
                    _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState111
                | HEXA _v ->
                    _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState111 _v
                | IDENT _v ->
                    _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState111 _v
                | IFJUMP ->
                    _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState111
                | INT _v ->
                    _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState111 _v
                | LBRACE ->
                    _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState111
                | LPAR ->
                    _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState111
                | MINUS ->
                    _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState111
                | NOT ->
                    _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState111
                | STORELOAD ->
                    _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState111
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState111) : 'freshtv402)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv403) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv404)) : 'freshtv406)
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState109
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState109) : 'freshtv408)
    | MenhirState111 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv411) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_expr) * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState112
        | DCOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((((('freshtv409) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_expr) * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let ((((_menhir_stack, _, (c : 'tv_cond)), _, (_4 : 'tv_expr)), _), _, (_7 : 'tv_expr)) = _menhir_stack in
            let _6 = () in
            let _5 = () in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_inst = 
# 208 "parser/policy_parser.mly"
                                            (
    Dba.Instr.ite c
    (Dba.Jump_target.outer (Dba_types.Caddress.block_start @@ Virtual_address.create 0)) 0
  )
# 2750 "parser/policy_parser.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _v) : 'freshtv410)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState112) : 'freshtv412)
    | MenhirState120 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv415 * 'tv_lhs)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState121
        | DCOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv413 * 'tv_lhs)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, (lv : 'tv_lhs)), _, (e : 'tv_expr)) = _menhir_stack in
            let _2 = () in
            let _v : 'tv_inst = 
# 171 "parser/policy_parser.mly"
                           (
  let size = Dba.Expr.size_of e in
  let sizelhs =
    match lv with
    | Dba.LValue.Var { size = sz; _}
    | Dba.LValue.Restrict({Dba.size = sz; _},_)
    | Dba.LValue.Store(sz,_,_) -> sz in
  Kernel_options.Logger.debug ~level:2 "Lhs:%d | Expr (infered):%d" sizelhs size;
  let lhs =
    if size <> 0 then
    match lv with
    | Dba.LValue.Var { size = sz; _} ->
      Kernel_options.Logger.debug ~level:2 "Var Size:%d" sz;
      if sz <> size && sz <> 0 then raise Parsing.Parse_error else lv
    | Dba.LValue.Restrict({Dba.size = sz; _}, {Interval.lo=l; Interval.hi=h}) ->
      Kernel_options.Logger.debug ~level:2 "VarR Size:%d" sz;
      if size <> h - l + 1 && sz <> 0 then raise Parsing.Parse_error else lv
    | Dba.LValue.Store(sz,en,e) ->
       Kernel_options.Logger.debug ~level:2 "LhsStore Size:%d" sz;
       if sz = 0 then
         let bytes = Size.Byte.create size in
         Dba.LValue.store bytes en e
       else lv
    else lv
  in
  let exp =
    if size = 0 && sizelhs <> 0 then
    match lv with
    | Dba.LValue.Var { size; _} -> Parse_helpers.patch_expr_size e size
    | Dba.LValue.Restrict(_, {Interval.lo=l; Interval.hi=h}) ->
    Parse_helpers.patch_expr_size e (h-l+1)
    | Dba.LValue.Store(sz,_en,_e) ->
       if sz <> 0 then Parse_helpers.patch_expr_size e sz else e
    else e
  in Dba.Instr.assign lhs exp 0
  )
# 2860 "parser/policy_parser.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _v) : 'freshtv414)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState121) : 'freshtv416)
    | MenhirState133 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv421 * _menhir_state)) * 'tv_taint)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv419 * _menhir_state)) * 'tv_taint)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState134 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv417 * _menhir_state)) * 'tv_taint)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let (((_menhir_stack, _menhir_s), (_3 : 'tv_taint)), _, (_5 : 'tv_expr)) = _menhir_stack in
            let _6 = () in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_sigma_p = 
# 163 "parser/policy_parser.mly"
                                          ( TaintCheck(_3, _5) )
# 2935 "parser/policy_parser.ml"
             in
            _menhir_goto_sigma_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv418)) : 'freshtv420)
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState134
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState134) : 'freshtv422)
    | MenhirState123 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv427 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | SUBTERM ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv423 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState155 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState156
            | EXTU ->
                _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState156
            | HEXA _v ->
                _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState156 _v
            | IDENT _v ->
                _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState156 _v
            | IFJUMP ->
                _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState156
            | INT _v ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState156 _v
            | LBRACE ->
                _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState156
            | LPAR ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState156
            | MINUS ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState156
            | NOT ->
                _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState156
            | STORELOAD ->
                _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState156
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState156) : 'freshtv424)
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState155
        | DCOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv425 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (_1 : 'tv_expr)) = _menhir_stack in
            let _v : 'tv_expression_p = 
# 153 "parser/policy_parser.mly"
         ( ExpDba _1 )
# 3051 "parser/policy_parser.ml"
             in
            _menhir_goto_expression_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv426)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState155) : 'freshtv428)
    | MenhirState156 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv431 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run71 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | CONCAT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | DIVS ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | DIVU ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | EQQ ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | GES ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | GEU ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | GTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | GTU ->
            _menhir_run63 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | LES ->
            _menhir_run61 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | LEU ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | LROTATE ->
            _menhir_run59 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | LSHIFT ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | LTS ->
            _menhir_run57 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | LTU ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | MINUS ->
            _menhir_run55 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | MODS ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | MODU ->
            _menhir_run53 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | MULTS ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | MULTU ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | NEQ ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | OR ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | PLUS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | RROTATE ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | RSHIFTS ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | RSHIFTU ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | XOR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState157
        | DCOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv429 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (_1 : 'tv_expr)), _), _, (_3 : 'tv_expr)) = _menhir_stack in
            let _2 = () in
            let _v : 'tv_expression_p = 
# 154 "parser/policy_parser.mly"
                      ( ExpSubTerm(_1,_3) )
# 3126 "parser/policy_parser.ml"
             in
            _menhir_goto_expression_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv430)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState157) : 'freshtv432)
    | _ ->
        _menhir_fail ()

and _menhir_goto_cond : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_cond -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState36 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv283 * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | EXTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | HEXA _v ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState77 _v
        | IDENT _v ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState77 _v
        | IFJUMP ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | INT _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState77 _v
        | LBRACE ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | LPAR ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | MINUS ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | NOT ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | STORELOAD ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState77) : 'freshtv284)
    | MenhirState106 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv289) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | NEXT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv285) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState108
            | EXTU ->
                _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState108
            | HEXA _v ->
                _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState108 _v
            | IDENT _v ->
                _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState108 _v
            | IFJUMP ->
                _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState108
            | INT _v ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState108 _v
            | LBRACE ->
                _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState108
            | LPAR ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState108
            | MINUS ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState108
            | NOT ->
                _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState108
            | STORELOAD ->
                _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState108
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState108) : 'freshtv286)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv287) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv288)) : 'freshtv290)
    | _ ->
        _menhir_fail ()

and _menhir_goto_rule : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_rule -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv281 * _menhir_state * 'tv_rule) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | DEFAULT ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | HEXA _v ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState19 _v
    | LBRACKET ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | WILDCARD ->
        _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | EOF ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv279 * _menhir_state * 'tv_rule) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (_1 : 'tv_rule)) = _menhir_stack in
        let _v : 'tv_rules = 
# 121 "parser/policy_parser.mly"
         ( [_1] )
# 3243 "parser/policy_parser.ml"
         in
        _menhir_goto_rules _menhir_env _menhir_stack _menhir_s _v) : 'freshtv280)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState19) : 'freshtv282)

and _menhir_goto_addrlist : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_addrlist -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState0 | MenhirState19 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv273) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_addrlist) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv271) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((_1 : 'tv_addrlist) : 'tv_addrlist) = _v in
        ((let _v : 'tv_location_p = 
# 136 "parser/policy_parser.mly"
             ( LocSet(_1) )
# 3266 "parser/policy_parser.ml"
         in
        _menhir_goto_location_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv272)) : 'freshtv274)
    | MenhirState163 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv277 * _menhir_state * 'tv_addr)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_addrlist) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv275 * _menhir_state * 'tv_addr)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((_3 : 'tv_addrlist) : 'tv_addrlist) = _v in
        ((let (_menhir_stack, _menhir_s, (_1 : 'tv_addr)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_addrlist = 
# 141 "parser/policy_parser.mly"
                       ( _1 :: _3 )
# 3283 "parser/policy_parser.ml"
         in
        _menhir_goto_addrlist _menhir_env _menhir_stack _menhir_s _v) : 'freshtv276)) : 'freshtv278)
    | _ ->
        _menhir_fail ()

and _menhir_goto_instruction_p : _menhir_env -> 'ttv_tail -> 'tv_instruction_p -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (('freshtv269 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | DCOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv265 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState123
        | EXTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState123
        | HEXA _v ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState123 _v
        | IDENT _v ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState123 _v
        | IFJUMP ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState123
        | INT _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState123 _v
        | LBRACE ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState123
        | LPAR ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState123
        | MINUS ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState123
        | NOT ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState123
        | STORELOAD ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState123
        | WILDCARD ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv263) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState123 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv261) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            ((let _1 = () in
            let _v : 'tv_expression_p = 
# 152 "parser/policy_parser.mly"
             ( ExpWildcard )
# 3337 "parser/policy_parser.ml"
             in
            _menhir_goto_expression_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv262)) : 'freshtv264)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState123) : 'freshtv266)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv267 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv268)) : 'freshtv270)

and _menhir_run37 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv259) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_cond = 
# 335 "parser/policy_parser.mly"
           ( Dba.Expr.one )
# 3362 "parser/policy_parser.ml"
     in
    _menhir_goto_cond _menhir_env _menhir_stack _menhir_s _v) : 'freshtv260)

and _menhir_run26 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACKET ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv255 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState27
        | EXTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState27
        | HEXA _v ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState27 _v
        | IDENT _v ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState27 _v
        | IFJUMP ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState27
        | INT _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState27 _v
        | LBRACE ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState27
        | LPAR ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState27
        | MINUS ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState27
        | NOT ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState27
        | STORELOAD ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState27
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState27) : 'freshtv256)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv257 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv258)

and _menhir_run28 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState28
    | EXTU ->
        _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState28
    | HEXA _v ->
        _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState28 _v
    | IDENT _v ->
        _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState28 _v
    | IFJUMP ->
        _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState28
    | INT _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState28 _v
    | LBRACE ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState28
    | LPAR ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState28
    | MINUS ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState28
    | NOT ->
        _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState28
    | STORELOAD ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState28
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState28

and _menhir_run29 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | EXTU ->
        _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | HEXA _v ->
        _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState29 _v
    | IDENT _v ->
        _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState29 _v
    | IFJUMP ->
        _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | INT _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState29 _v
    | LBRACE ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | LPAR ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | MINUS ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | NOT ->
        _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | STORELOAD ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState29

and _menhir_run30 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | EXTU ->
        _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | HEXA _v ->
        _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState30 _v
    | IDENT _v ->
        _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState30 _v
    | IFJUMP ->
        _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | INT _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState30 _v
    | LBRACE ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | LPAR ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | MINUS ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | NOT ->
        _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | STORELOAD ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState30

and _menhir_run31 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState31
    | EXTU ->
        _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState31
    | HEXA _v ->
        _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState31 _v
    | IDENT _v ->
        _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState31 _v
    | IFJUMP ->
        _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState31
    | INT _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState31 _v
    | LBRACE ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState31
    | LPAR ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState31
    | MINUS ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState31
    | NOT ->
        _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState31
    | STORELOAD ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState31
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState31

and _menhir_run32 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 93 "parser/policy_parser.mly"
       (string)
# 3547 "parser/policy_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INFER ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv249 * _menhir_state * (
# 93 "parser/policy_parser.mly"
       (string)
# 3559 "parser/policy_parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv245 * _menhir_state * (
# 93 "parser/policy_parser.mly"
       (string)
# 3569 "parser/policy_parser.ml"
            ))) = Obj.magic _menhir_stack in
            let (_v : (
# 93 "parser/policy_parser.mly"
       (string)
# 3574 "parser/policy_parser.ml"
            )) = _v in
            ((let _menhir_stack = (_menhir_stack, _v) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | SUPER ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv241 * _menhir_state * (
# 93 "parser/policy_parser.mly"
       (string)
# 3585 "parser/policy_parser.ml"
                ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 3589 "parser/policy_parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv239 * _menhir_state * (
# 93 "parser/policy_parser.mly"
       (string)
# 3596 "parser/policy_parser.ml"
                ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 3600 "parser/policy_parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, (_1 : (
# 93 "parser/policy_parser.mly"
       (string)
# 3605 "parser/policy_parser.ml"
                ))), (_3 : (
# 93 "parser/policy_parser.mly"
       (string)
# 3609 "parser/policy_parser.ml"
                ))) = _menhir_stack in
                let _4 = () in
                let _2 = () in
                let _v : 'tv_expr = 
# 240 "parser/policy_parser.mly"
                        (
    let size = int_of_string _3 in
    let bigint = Bigint.big_int_of_string _1 in
    let bv = Bitvector.create bigint size in
    Dba.Expr.constant bv
  )
# 3621 "parser/policy_parser.ml"
                 in
                _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv240)) : 'freshtv242)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv243 * _menhir_state * (
# 93 "parser/policy_parser.mly"
       (string)
# 3631 "parser/policy_parser.ml"
                ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 3635 "parser/policy_parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv244)) : 'freshtv246)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv247 * _menhir_state * (
# 93 "parser/policy_parser.mly"
       (string)
# 3646 "parser/policy_parser.ml"
            ))) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv248)) : 'freshtv250)
    | AND | COMMA | CONCAT | DCOLON | DIVS | DIVU | ELSE | EQQ | EXTS | EXTU | GES | GEU | GTS | GTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | MULTS | MULTU | NEQ | NEXT | NOT | OR | PLUS | RBRACKET | RPAR | RROTATE | RSHIFTS | RSHIFTU | STORELOAD | SUBTERM | XOR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv251 * _menhir_state * (
# 93 "parser/policy_parser.mly"
       (string)
# 3655 "parser/policy_parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (_1 : (
# 93 "parser/policy_parser.mly"
       (string)
# 3660 "parser/policy_parser.ml"
        ))) = _menhir_stack in
        let _v : 'tv_expr = 
# 246 "parser/policy_parser.mly"
        (
    let bigint = Bigint.big_int_of_string _1 in
    Dba.Expr.constant (Bitvector.create bigint 0)
  )
# 3668 "parser/policy_parser.ml"
         in
        _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv252)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv253 * _menhir_state * (
# 93 "parser/policy_parser.mly"
       (string)
# 3678 "parser/policy_parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv254)

and _menhir_run36 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | EXTU ->
        _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | FALSE ->
        _menhir_run40 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | HEXA _v ->
        _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState36 _v
    | IDENT _v ->
        _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState36 _v
    | IFJUMP ->
        _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | INT _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState36 _v
    | LBRACE ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | LPAR ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | MINUS ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | NOT ->
        _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | STORELOAD ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | TRUE ->
        _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState36
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState36

and _menhir_run38 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 94 "parser/policy_parser.mly"
       (string)
# 3723 "parser/policy_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv237) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 94 "parser/policy_parser.mly"
       (string)
# 3733 "parser/policy_parser.ml"
    )) : (
# 94 "parser/policy_parser.mly"
       (string)
# 3737 "parser/policy_parser.ml"
    )) = _v in
    ((let _v : 'tv_expr = 
# 256 "parser/policy_parser.mly"
          (
    let var = Parse_helpers.expr_of_name _1 in
    var
  )
# 3745 "parser/policy_parser.ml"
     in
    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv238)

and _menhir_run39 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 95 "parser/policy_parser.mly"
       (string * int)
# 3752 "parser/policy_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv235) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 95 "parser/policy_parser.mly"
       (string * int)
# 3762 "parser/policy_parser.ml"
    )) : (
# 95 "parser/policy_parser.mly"
       (string * int)
# 3766 "parser/policy_parser.ml"
    )) = _v in
    ((let _v : 'tv_expr = 
# 250 "parser/policy_parser.mly"
         (
    let s, size = _1 in
    let bigint = Bigint.big_int_of_string s in
    let bv = Bitvector.create bigint size in
    Dba.Expr.constant bv
  )
# 3776 "parser/policy_parser.ml"
     in
    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv236)

and _menhir_run40 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv233) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_cond = 
# 336 "parser/policy_parser.mly"
           ( Dba.Expr.zero )
# 3790 "parser/policy_parser.ml"
     in
    _menhir_goto_cond _menhir_env _menhir_stack _menhir_s _v) : 'freshtv234)

and _menhir_run41 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState41
    | EXTU ->
        _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState41
    | HEXA _v ->
        _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState41 _v
    | IDENT _v ->
        _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState41 _v
    | IFJUMP ->
        _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState41
    | INT _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState41 _v
    | LBRACE ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState41
    | LPAR ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState41
    | MINUS ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState41
    | NOT ->
        _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState41
    | STORELOAD ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState41
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState41

and _menhir_run42 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | EXTU ->
        _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | HEXA _v ->
        _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState42 _v
    | IDENT _v ->
        _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState42 _v
    | IFJUMP ->
        _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | INT _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState42 _v
    | LBRACE ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | LPAR ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | MINUS ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | NOT ->
        _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | STORELOAD ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState42

and _menhir_goto_lhs : _menhir_env -> 'ttv_tail -> 'tv_lhs -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv231 * 'tv_lhs) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | ASSIGN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv227 * 'tv_lhs) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState120
        | EXTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState120
        | HEXA _v ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState120 _v
        | IDENT _v ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState120 _v
        | IFJUMP ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState120
        | INT _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState120 _v
        | LBRACE ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState120
        | LPAR ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState120
        | MINUS ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState120
        | NOT ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState120
        | STORELOAD ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState120
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState120) : 'freshtv228)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv229 * 'tv_lhs) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv230)) : 'freshtv232)

and _menhir_goto_action : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_action -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState10 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv221 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_action) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv219 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((_3 : 'tv_action) : 'tv_action) = _v in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_rule = 
# 125 "parser/policy_parser.mly"
                            ( {loc_p=LocWildcard; inst_p=InstWildcard; exp_p=ExpWildcard; sigma_p=SigmaWildcard; action=_3,""} )
# 3925 "parser/policy_parser.ml"
         in
        _menhir_goto_rule _menhir_env _menhir_stack _menhir_s _v) : 'freshtv220)) : 'freshtv222)
    | MenhirState153 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((((('freshtv225 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p)) * _menhir_state * 'tv_sigma_p)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_action) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((((('freshtv223 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p)) * _menhir_state * 'tv_sigma_p)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((_9 : 'tv_action) : 'tv_action) = _v in
        ((let ((((_menhir_stack, _menhir_s, (_1 : 'tv_location_p)), (_3 : 'tv_instruction_p)), _, (_5 : 'tv_expression_p)), _, (_7 : 'tv_sigma_p)) = _menhir_stack in
        let _8 = () in
        let _6 = () in
        let _4 = () in
        let _2 = () in
        let _v : 'tv_rule = 
# 124 "parser/policy_parser.mly"
                                                                                       ( {loc_p=_1; inst_p=_3; exp_p=_5; sigma_p=_7; action=_9,""} )
# 3945 "parser/policy_parser.ml"
         in
        _menhir_goto_rule _menhir_env _menhir_stack _menhir_s _v) : 'freshtv224)) : 'freshtv226)
    | _ ->
        _menhir_fail ()

and _menhir_fail : unit -> 'a =
  fun () ->
    Printf.fprintf stderr "Internal failure -- please contact the parser generator's developers.\n%!";
    assert false

and _menhir_reduce8 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_addr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let (_menhir_stack, _menhir_s, (_1 : 'tv_addr)) = _menhir_stack in
    let _v : 'tv_addrlist = 
# 142 "parser/policy_parser.mly"
        ( [_1] )
# 3962 "parser/policy_parser.ml"
     in
    _menhir_goto_addrlist _menhir_env _menhir_stack _menhir_s _v

and _menhir_run163 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_addr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | HEXA _v ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState163 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState163

and _menhir_goto_location_p : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_location_p -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv217 * _menhir_state * 'tv_location_p) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | DCOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv213 * _menhir_state * 'tv_location_p) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | IDENT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv195) = Obj.magic _menhir_stack in
            let (_v : (
# 94 "parser/policy_parser.mly"
       (string)
# 3998 "parser/policy_parser.ml"
            )) = _v in
            ((let _menhir_stack = (_menhir_stack, _v) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LBRACE ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv189 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4009 "parser/policy_parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | INT _v ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ('freshtv185 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4019 "parser/policy_parser.ml"
                    ))) = Obj.magic _menhir_stack in
                    let (_v : (
# 93 "parser/policy_parser.mly"
       (string)
# 4024 "parser/policy_parser.ml"
                    )) = _v in
                    ((let _menhir_stack = (_menhir_stack, _v) in
                    let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | COMMA ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (('freshtv181 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4035 "parser/policy_parser.ml"
                        ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4039 "parser/policy_parser.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | INT _v ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((('freshtv177 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4049 "parser/policy_parser.ml"
                            ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4053 "parser/policy_parser.ml"
                            ))) = Obj.magic _menhir_stack in
                            let (_v : (
# 93 "parser/policy_parser.mly"
       (string)
# 4058 "parser/policy_parser.ml"
                            )) = _v in
                            ((let _menhir_stack = (_menhir_stack, _v) in
                            let _menhir_env = _menhir_discard _menhir_env in
                            let _tok = _menhir_env._menhir_token in
                            match _tok with
                            | RBRACE ->
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : (((('freshtv173 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4069 "parser/policy_parser.ml"
                                ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4073 "parser/policy_parser.ml"
                                ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4077 "parser/policy_parser.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let _menhir_env = _menhir_discard _menhir_env in
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : (((('freshtv171 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4084 "parser/policy_parser.ml"
                                ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4088 "parser/policy_parser.ml"
                                ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4092 "parser/policy_parser.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let (((_menhir_stack, (id : (
# 94 "parser/policy_parser.mly"
       (string)
# 4097 "parser/policy_parser.ml"
                                ))), (lo : (
# 93 "parser/policy_parser.mly"
       (string)
# 4101 "parser/policy_parser.ml"
                                ))), (hi : (
# 93 "parser/policy_parser.mly"
       (string)
# 4105 "parser/policy_parser.ml"
                                ))) = _menhir_stack in
                                let _6 = () in
                                let _4 = () in
                                let _2 = () in
                                let _v : 'tv_lhs = 
# 223 "parser/policy_parser.mly"
  (
    let off1 = int_of_string lo in
    let off2 = int_of_string hi in
    let size = infer_size_variable id |> Size.Bit.create in
    Dba.LValue._restrict id size off1 off2
  )
# 4118 "parser/policy_parser.ml"
                                 in
                                _menhir_goto_lhs _menhir_env _menhir_stack _v) : 'freshtv172)) : 'freshtv174)
                            | _ ->
                                assert (not _menhir_env._menhir_error);
                                _menhir_env._menhir_error <- true;
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : (((('freshtv175 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4128 "parser/policy_parser.ml"
                                ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4132 "parser/policy_parser.ml"
                                ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4136 "parser/policy_parser.ml"
                                )) = Obj.magic _menhir_stack in
                                (raise _eRR : 'freshtv176)) : 'freshtv178)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((('freshtv179 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4146 "parser/policy_parser.ml"
                            ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4150 "parser/policy_parser.ml"
                            ))) = Obj.magic _menhir_stack in
                            (raise _eRR : 'freshtv180)) : 'freshtv182)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (('freshtv183 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4160 "parser/policy_parser.ml"
                        ))) * (
# 93 "parser/policy_parser.mly"
       (string)
# 4164 "parser/policy_parser.ml"
                        )) = Obj.magic _menhir_stack in
                        (raise _eRR : 'freshtv184)) : 'freshtv186)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ('freshtv187 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4174 "parser/policy_parser.ml"
                    ))) = Obj.magic _menhir_stack in
                    (raise _eRR : 'freshtv188)) : 'freshtv190)
            | ASSIGN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv191 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4182 "parser/policy_parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, (v : (
# 94 "parser/policy_parser.mly"
       (string)
# 4187 "parser/policy_parser.ml"
                ))) = _menhir_stack in
                let _v : 'tv_lhs = 
# 221 "parser/policy_parser.mly"
            ( infer_variable_lhs v )
# 4192 "parser/policy_parser.ml"
                 in
                _menhir_goto_lhs _menhir_env _menhir_stack _v) : 'freshtv192)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv193 * (
# 94 "parser/policy_parser.mly"
       (string)
# 4202 "parser/policy_parser.ml"
                )) = Obj.magic _menhir_stack in
                (raise _eRR : 'freshtv194)) : 'freshtv196)
        | IFJUMP ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv197) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | EXTU ->
                _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | FALSE ->
                _menhir_run40 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | HEXA _v ->
                _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState106 _v
            | IDENT _v ->
                _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState106 _v
            | IFJUMP ->
                _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | INT _v ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState106 _v
            | LBRACE ->
                _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | LPAR ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | MINUS ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | NOT ->
                _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | STORELOAD ->
                _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | TRUE ->
                _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState106
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState106) : 'freshtv198)
        | NEXT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv199) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState104
            | EXTU ->
                _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState104
            | HEXA _v ->
                _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState104 _v
            | IDENT _v ->
                _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState104 _v
            | IFJUMP ->
                _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState104
            | INT _v ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState104 _v
            | LBRACE ->
                _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState104
            | LPAR ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState104
            | MINUS ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState104
            | NOT ->
                _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState104
            | STORELOAD ->
                _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState104
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState104) : 'freshtv200)
        | STORELOAD ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv205) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LBRACKET ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv201) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | EXTS ->
                    _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState25
                | EXTU ->
                    _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState25
                | HEXA _v ->
                    _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState25 _v
                | IDENT _v ->
                    _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState25 _v
                | IFJUMP ->
                    _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState25
                | INT _v ->
                    _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState25 _v
                | LBRACE ->
                    _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState25
                | LPAR ->
                    _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState25
                | MINUS ->
                    _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState25
                | NOT ->
                    _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState25
                | STORELOAD ->
                    _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState25
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState25) : 'freshtv202)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv203) = Obj.magic _menhir_stack in
                (raise _eRR : 'freshtv204)) : 'freshtv206)
        | WILDCARD ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv209) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv207) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_instruction_p = 
# 148 "parser/policy_parser.mly"
             ( InstWildcard )
# 4327 "parser/policy_parser.ml"
             in
            _menhir_goto_instruction_p _menhir_env _menhir_stack _v) : 'freshtv208)) : 'freshtv210)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv211 * _menhir_state * 'tv_location_p)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv212)) : 'freshtv214)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv215 * _menhir_state * 'tv_location_p) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv216)) : 'freshtv218)

and _menhir_run11 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv169) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_action = 
# 128 "parser/policy_parser.mly"
         ( Symb )
# 4355 "parser/policy_parser.ml"
     in
    _menhir_goto_action _menhir_env _menhir_stack _menhir_s _v) : 'freshtv170)

and _menhir_run12 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv167) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_action = 
# 132 "parser/policy_parser.mly"
          ( KeepOrSymb )
# 4369 "parser/policy_parser.ml"
     in
    _menhir_goto_action _menhir_env _menhir_stack _menhir_s _v) : 'freshtv168)

and _menhir_run13 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv165) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_action = 
# 131 "parser/policy_parser.mly"
          ( KeepOrConc )
# 4383 "parser/policy_parser.ml"
     in
    _menhir_goto_action _menhir_env _menhir_stack _menhir_s _v) : 'freshtv166)

and _menhir_run14 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv163) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_action = 
# 130 "parser/policy_parser.mly"
         ( KeepOrSymb )
# 4397 "parser/policy_parser.ml"
     in
    _menhir_goto_action _menhir_env _menhir_stack _menhir_s _v) : 'freshtv164)

and _menhir_run15 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv161) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_action = 
# 129 "parser/policy_parser.mly"
         ( Conc )
# 4411 "parser/policy_parser.ml"
     in
    _menhir_goto_action _menhir_env _menhir_stack _menhir_s _v) : 'freshtv162)

and _menhir_errorcase : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    match _menhir_s with
    | MenhirState163 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv49 * _menhir_state * 'tv_addr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv50)
    | MenhirState161 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv51 * _menhir_state * 'tv_addr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv52)
    | MenhirState157 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv53 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv54)
    | MenhirState156 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv55 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv56)
    | MenhirState155 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv57 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv58)
    | MenhirState153 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((((('freshtv59 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p)) * _menhir_state * 'tv_sigma_p)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv60)
    | MenhirState147 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv61 * _menhir_state) * _menhir_state * 'tv_sigma_p)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv62)
    | MenhirState145 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv63 * _menhir_state * 'tv_sigma_p)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv64)
    | MenhirState143 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv65 * _menhir_state * 'tv_sigma_p)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv66)
    | MenhirState140 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv67 * _menhir_state) * _menhir_state * 'tv_sigma_p)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv68)
    | MenhirState138 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv69 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv70)
    | MenhirState137 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv71 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv72)
    | MenhirState134 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv73 * _menhir_state)) * 'tv_taint)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv74)
    | MenhirState133 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv75 * _menhir_state)) * 'tv_taint)) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv76)
    | MenhirState126 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv77 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) * _menhir_state * 'tv_expression_p)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv78)
    | MenhirState123 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv79 * _menhir_state * 'tv_location_p)) * 'tv_instruction_p)) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv80)
    | MenhirState121 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv81 * 'tv_lhs)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv82)
    | MenhirState120 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv83 * 'tv_lhs)) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv84)
    | MenhirState112 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv85) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_expr) * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv86)
    | MenhirState111 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv87) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_expr) * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv88)
    | MenhirState109 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv89) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv90)
    | MenhirState108 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv91) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv92)
    | MenhirState106 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv93) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv94)
    | MenhirState105 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv95) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv96)
    | MenhirState104 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv97) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv98)
    | MenhirState99 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv99)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv100)
    | MenhirState94 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv101 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv102)
    | MenhirState93 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv103 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv104)
    | MenhirState92 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv105 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv106)
    | MenhirState90 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv107 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv108)
    | MenhirState89 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv109 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv110)
    | MenhirState87 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv111 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv112)
    | MenhirState81 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv113 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv114)
    | MenhirState80 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv115 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv116)
    | MenhirState79 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv117 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv118)
    | MenhirState78 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv119 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv120)
    | MenhirState77 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv121 * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv122)
    | MenhirState76 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv123 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv124)
    | MenhirState74 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv125 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv126)
    | MenhirState73 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv127 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv128)
    | MenhirState72 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv129 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv130)
    | MenhirState43 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv131 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv132)
    | MenhirState42 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv133 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv134)
    | MenhirState41 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv135 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv136)
    | MenhirState36 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv137 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv138)
    | MenhirState31 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv139 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv140)
    | MenhirState30 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv141 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv142)
    | MenhirState29 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv143 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv144)
    | MenhirState28 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv145 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv146)
    | MenhirState27 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv147 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv148)
    | MenhirState25 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv149)) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv150)
    | MenhirState19 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv151 * _menhir_state * 'tv_rule) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv152)
    | MenhirState10 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv153 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv154)
    | MenhirState5 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv155 * _menhir_state) * _menhir_state * 'tv_addr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv156)
    | MenhirState2 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv157 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv158)
    | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv159) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv160)

and _menhir_run1 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv47) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_location_p = 
# 135 "parser/policy_parser.mly"
             ( LocWildcard )
# 4704 "parser/policy_parser.ml"
     in
    _menhir_goto_location_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv48)

and _menhir_run2 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | HEXA _v ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState2 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState2

and _menhir_run3 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 95 "parser/policy_parser.mly"
       (string * int)
# 4724 "parser/policy_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv45) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 95 "parser/policy_parser.mly"
       (string * int)
# 4734 "parser/policy_parser.ml"
    )) : (
# 95 "parser/policy_parser.mly"
       (string * int)
# 4738 "parser/policy_parser.ml"
    )) = _v in
    ((let _v : 'tv_addr = 
# 145 "parser/policy_parser.mly"
         ( Int64.of_string (fst _1))
# 4743 "parser/policy_parser.ml"
     in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv43) = _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_addr) = _v in
    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState2 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv19 * _menhir_state) * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INTERVALSEP ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv15 * _menhir_state) * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | HEXA _v ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState5 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState5) : 'freshtv16)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv17 * _menhir_state) * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv18)) : 'freshtv20)
    | MenhirState5 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv27 * _menhir_state) * _menhir_state * 'tv_addr)) * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RBRACKET ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv23 * _menhir_state) * _menhir_state * 'tv_addr)) * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv21 * _menhir_state) * _menhir_state * 'tv_addr)) * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s), _, (_2 : 'tv_addr)), _, (_4 : 'tv_addr)) = _menhir_stack in
            let _5 = () in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_location_p = 
# 138 "parser/policy_parser.mly"
                                            ( LocInterval(_2,_4) )
# 4795 "parser/policy_parser.ml"
             in
            _menhir_goto_location_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv22)) : 'freshtv24)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv25 * _menhir_state) * _menhir_state * 'tv_addr)) * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv26)) : 'freshtv28)
    | MenhirState0 | MenhirState19 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv33 * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            _menhir_run163 _menhir_env (Obj.magic _menhir_stack)
        | INTERVALSEP ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv29 * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | HEXA _v ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState161 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState161) : 'freshtv30)
        | DCOLON ->
            _menhir_reduce8 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv31 * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv32)) : 'freshtv34)
    | MenhirState161 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv37 * _menhir_state * 'tv_addr)) * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv35 * _menhir_state * 'tv_addr)) * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_addr)), _, (_3 : 'tv_addr)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_location_p = 
# 137 "parser/policy_parser.mly"
                          ( LocInterval(_1,_3) )
# 4844 "parser/policy_parser.ml"
         in
        _menhir_goto_location_p _menhir_env _menhir_stack _menhir_s _v) : 'freshtv36)) : 'freshtv38)
    | MenhirState163 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv41 * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            _menhir_run163 _menhir_env (Obj.magic _menhir_stack)
        | DCOLON ->
            _menhir_reduce8 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv39 * _menhir_state * 'tv_addr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv40)) : 'freshtv42)
    | _ ->
        _menhir_fail ()) : 'freshtv44)) : 'freshtv46)

and _menhir_goto_policy : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 111 "parser/policy_parser.mly"
      (Policy_type.policy)
# 4870 "parser/policy_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv13) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : (
# 111 "parser/policy_parser.mly"
      (Policy_type.policy)
# 4879 "parser/policy_parser.ml"
    )) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv11) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 111 "parser/policy_parser.mly"
      (Policy_type.policy)
# 4887 "parser/policy_parser.ml"
    )) : (
# 111 "parser/policy_parser.mly"
      (Policy_type.policy)
# 4891 "parser/policy_parser.ml"
    )) = _v in
    (Obj.magic _1 : 'freshtv12)) : 'freshtv14)

and _menhir_run9 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | TOACTION ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv7 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | CONC ->
            _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState10
        | PROP ->
            _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState10
        | PROPC ->
            _menhir_run13 _menhir_env (Obj.magic _menhir_stack) MenhirState10
        | PROPS ->
            _menhir_run12 _menhir_env (Obj.magic _menhir_stack) MenhirState10
        | SYMB ->
            _menhir_run11 _menhir_env (Obj.magic _menhir_stack) MenhirState10
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState10) : 'freshtv8)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv9 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv10)

and _menhir_discard : _menhir_env -> _menhir_env =
  fun _menhir_env ->
    let lexer = _menhir_env._menhir_lexer in
    let lexbuf = _menhir_env._menhir_lexbuf in
    let _tok = lexer lexbuf in
    {
      _menhir_lexer = lexer;
      _menhir_lexbuf = lexbuf;
      _menhir_token = _tok;
      _menhir_error = false;
    }

and policy : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 111 "parser/policy_parser.mly"
      (Policy_type.policy)
# 4944 "parser/policy_parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env =
      let (lexer : Lexing.lexbuf -> token) = lexer in
      let (lexbuf : Lexing.lexbuf) = lexbuf in
      ((let _tok = Obj.magic () in
      {
        _menhir_lexer = lexer;
        _menhir_lexbuf = lexbuf;
        _menhir_token = _tok;
        _menhir_error = false;
      }) : _menhir_env)
    in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv5) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | DEFAULT ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | EOF ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv3) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState0 in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        ((let _1 = () in
        let _v : (
# 111 "parser/policy_parser.mly"
      (Policy_type.policy)
# 4976 "parser/policy_parser.ml"
        ) = 
# 116 "parser/policy_parser.mly"
        ( [] )
# 4980 "parser/policy_parser.ml"
         in
        _menhir_goto_policy _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2)) : 'freshtv4)
    | HEXA _v ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState0 _v
    | LBRACKET ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | WILDCARD ->
        _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState0) : 'freshtv6))

# 269 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
  

# 4997 "parser/policy_parser.ml"
