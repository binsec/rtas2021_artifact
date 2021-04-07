
module MenhirBasics = struct
  
  exception Error = Parsing.Parse_error
  
  type token = 
    | XOR
    | UMINUS
    | TRUE
    | TERM_TOKEN
    | SUPER
    | STORELOAD
    | RSHIFTU
    | RSHIFTS
    | RROTATE
    | RPAR
    | RBRACKET
    | RBRACE
    | PLUS
    | OR
    | NOT
    | NEQ2
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
    | INT of (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 42 "parser/dbacsl_parser.ml"
  )
    | INFER
    | IFJUMP
    | IDENT of (
# 71 "parser/dbacsl_parser.mly"
       (string)
# 49 "parser/dbacsl_parser.ml"
  )
    | HEXA of (
# 72 "parser/dbacsl_parser.mly"
       (string * int)
# 54 "parser/dbacsl_parser.ml"
  )
    | GTU
    | GTS
    | GEU
    | GES
    | FALSE
    | EXTU
    | EXTS
    | EQQ2
    | EQQ
    | EOF
    | ELSE
    | DIVU
    | DIVS
    | CONCAT
    | COMMA
    | ASSUME
    | ASSERT
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
  | MenhirState86
  | MenhirState85
  | MenhirState79
  | MenhirState78
  | MenhirState73
  | MenhirState72
  | MenhirState71
  | MenhirState69
  | MenhirState68
  | MenhirState66
  | MenhirState60
  | MenhirState59
  | MenhirState58
  | MenhirState57
  | MenhirState56
  | MenhirState55
  | MenhirState53
  | MenhirState52
  | MenhirState51
  | MenhirState20
  | MenhirState19
  | MenhirState18
  | MenhirState14
  | MenhirState9
  | MenhirState8
  | MenhirState7
  | MenhirState6
  | MenhirState5
  | MenhirState3
  | MenhirState0

# 22 "parser/dbacsl_parser.mly"
  
  open Dba

  let is_same_size_op op =
    let open Dba.Binary_op in
    match op with
    | Concat | LShift | RShiftU
    | RShiftS  | LeftRotate | RightRotate -> false
    | _ -> true


  let infer_binary_op_size left op right =
    let open Dba.Expr in
    let default () = binary op left right in
    if is_same_size_op op then
      let sz1 = Dba.Expr.size_of left in
      let sz2 = Dba.Expr.size_of right in
      if sz1 = 0 || sz2 = 0 then
        if sz1 = 0 then
          binary op (Parse_helpers.patch_expr_size left sz2) right
        else
          binary op left (Parse_helpers.patch_expr_size right sz1)
      else
        if sz1 <> sz2 then
          match left, right with
          | (_,Dba.Expr.Cst(`Constant, bv)) ->
             binary op left
                    (constant (Bitvector.create (Bitvector.value_of bv) sz1))
          | (Dba.Expr.Cst(`Constant, bv),_) ->
             binary op
                    (constant (Bitvector.create (Bitvector.value_of bv) sz2)) right
          | _ -> Printf.printf "Cannot infer size (mismatch remaining"; default ()
        else default ()
    else default ()


# 158 "parser/dbacsl_parser.ml"

let rec _menhir_goto_bin_op : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_bin_op -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState73 | MenhirState72 | MenhirState71 | MenhirState69 | MenhirState60 | MenhirState57 | MenhirState59 | MenhirState55 | MenhirState53 | MenhirState52 | MenhirState20 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv303 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | EXTU ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | HEXA _v ->
            _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState51 _v
        | IDENT _v ->
            _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState51 _v
        | IFJUMP ->
            _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | INT _v ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState51 _v
        | LBRACE ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | LPAR ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | MINUS ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | NOT ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | STORELOAD ->
            _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState51) : 'freshtv304)
    | MenhirState79 | MenhirState66 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv305 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState68
        | EXTU ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState68
        | HEXA _v ->
            _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState68 _v
        | IDENT _v ->
            _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState68 _v
        | IFJUMP ->
            _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState68
        | INT _v ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState68 _v
        | LBRACE ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState68
        | LPAR ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState68
        | MINUS ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState68
        | NOT ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState68
        | STORELOAD ->
            _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState68
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState68) : 'freshtv306)
    | _ ->
        _menhir_fail ()

and _menhir_run67 : _menhir_env -> ('ttv_tail * _menhir_state) * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ('freshtv301 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
    let (_ : _menhir_state) = _menhir_s in
    ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
    let _3 = () in
    let _1 = () in
    let _v : 'tv_expr = 
# 144 "parser/dbacsl_parser.mly"
                   ( _2 )
# 243 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv302)

and _menhir_run21 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv299) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 157 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.Xor )
# 257 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv300)

and _menhir_run22 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv297) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 181 "parser/dbacsl_parser.mly"
            ( Dba.Binary_op.RShiftU )
# 271 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv298)

and _menhir_run23 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv295) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 182 "parser/dbacsl_parser.mly"
             ( Dba.Binary_op.RShiftS )
# 285 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv296)

and _menhir_run24 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv293) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 184 "parser/dbacsl_parser.mly"
            ( Dba.Binary_op.RightRotate )
# 299 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv294)

and _menhir_run25 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv291) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 158 "parser/dbacsl_parser.mly"
        ( Dba.Binary_op.Plus )
# 313 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv292)

and _menhir_run26 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv289) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 155 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.Or )
# 327 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv290)

and _menhir_run27 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv287) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 170 "parser/dbacsl_parser.mly"
        ( Dba.Binary_op.Diff )
# 341 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv288)

and _menhir_run28 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv285) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 169 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.Diff )
# 355 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv286)

and _menhir_run29 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv283) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 160 "parser/dbacsl_parser.mly"
         ( Dba.Binary_op.Mult )
# 369 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv284)

and _menhir_run30 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv281) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 161 "parser/dbacsl_parser.mly"
         ( Dba.Binary_op.Mult )
# 383 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv282)

and _menhir_run31 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv279) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 153 "parser/dbacsl_parser.mly"
        ( Dba.Binary_op.ModU )
# 397 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv280)

and _menhir_run32 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv277) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 154 "parser/dbacsl_parser.mly"
        ( Dba.Binary_op.ModS )
# 411 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv278)

and _menhir_run33 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv275) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 159 "parser/dbacsl_parser.mly"
         ( Dba.Binary_op.Minus )
# 425 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv276)

and _menhir_run34 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv273) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 172 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.LtU )
# 439 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv274)

and _menhir_run35 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv271) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 176 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.LtS )
# 453 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv272)

and _menhir_run36 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv269) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 180 "parser/dbacsl_parser.mly"
           ( Dba.Binary_op.LShift )
# 467 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv270)

and _menhir_run37 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv267) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 183 "parser/dbacsl_parser.mly"
            ( Dba.Binary_op.LeftRotate )
# 481 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv268)

and _menhir_run38 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv265) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 171 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.LeqU )
# 495 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv266)

and _menhir_run39 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv263) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 175 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.LeqS )
# 509 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv264)

and _menhir_run41 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv261) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 174 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.GtU )
# 523 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv262)

and _menhir_run42 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv259) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 178 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.GtS )
# 537 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv260)

and _menhir_run43 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv257) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 173 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.GeqU )
# 551 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv258)

and _menhir_run44 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv255) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 177 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.GeqS )
# 565 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv256)

and _menhir_run45 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv253) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 168 "parser/dbacsl_parser.mly"
        ( Dba.Binary_op.Eq )
# 579 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv254)

and _menhir_run46 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv251) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 167 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.Eq )
# 593 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv252)

and _menhir_run47 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv249) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 162 "parser/dbacsl_parser.mly"
        ( Dba.Binary_op.DivU )
# 607 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv250)

and _menhir_run48 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv247) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 163 "parser/dbacsl_parser.mly"
        ( Dba.Binary_op.DivS )
# 621 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv248)

and _menhir_run49 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv245) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 165 "parser/dbacsl_parser.mly"
          ( Dba.Binary_op.Concat )
# 635 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv246)

and _menhir_run50 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv243) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_bin_op = 
# 156 "parser/dbacsl_parser.mly"
       ( Dba.Binary_op.And )
# 649 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_bin_op _menhir_env _menhir_stack _menhir_s _v) : 'freshtv244)

and _menhir_fail : unit -> 'a =
  fun () ->
    Printf.fprintf stderr "Internal failure -- please contact the parser generator's developers.\n%!";
    assert false

and _menhir_goto_term : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 661 "parser/dbacsl_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv241) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 670 "parser/dbacsl_parser.ml"
    )) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv239) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 678 "parser/dbacsl_parser.ml"
    )) : (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 682 "parser/dbacsl_parser.ml"
    )) = _v in
    (Obj.magic _1 : 'freshtv240)) : 'freshtv242)

and _menhir_goto_expr : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState19 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv157 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv155 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState20 in
            let (_v : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 723 "parser/dbacsl_parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv153 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((_3 : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 732 "parser/dbacsl_parser.ml"
            )) : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 736 "parser/dbacsl_parser.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 139 "parser/dbacsl_parser.mly"
                  (
    let size = int_of_string _3 in
    Dba.Expr.sext size _2
  )
# 746 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv154)) : 'freshtv156)
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState20
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState20) : 'freshtv158)
    | MenhirState51 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv161 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState52
        | COMMA | ELSE | EOF | EXTS | EXTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LPAR | NOT | RBRACKET | RPAR | STORELOAD ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv159 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (_1 : 'tv_expr)), _, (_2 : 'tv_bin_op)), _, (_3 : 'tv_expr)) = _menhir_stack in
            let _v : 'tv_expr = 
# 145 "parser/dbacsl_parser.mly"
                     (
    infer_binary_op_size _1 _2 _3
  )
# 864 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv160)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState52) : 'freshtv162)
    | MenhirState18 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv167 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv165 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState53 in
            let (_v : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 904 "parser/dbacsl_parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv163 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((_3 : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 913 "parser/dbacsl_parser.ml"
            )) : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 917 "parser/dbacsl_parser.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 135 "parser/dbacsl_parser.mly"
                  (
    let size = int_of_string _3 in
    Dba.Expr.uext size _2
  )
# 927 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv164)) : 'freshtv166)
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState53) : 'freshtv168)
    | MenhirState0 | MenhirState85 | MenhirState3 | MenhirState14 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv171 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | EOF | EXTS | EXTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LPAR | NOT | STORELOAD ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv169 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (e : 'tv_expr)) = _menhir_stack in
            let _v : 'tv_cond = 
# 189 "parser/dbacsl_parser.mly"
           ( e )
# 1043 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_cond _menhir_env _menhir_stack _menhir_s _v) : 'freshtv170)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState55) : 'freshtv172)
    | MenhirState56 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv175 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | ELSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv173 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState57 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState58
            | EXTU ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState58
            | HEXA _v ->
                _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState58 _v
            | IDENT _v ->
                _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState58 _v
            | IFJUMP ->
                _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState58
            | INT _v ->
                _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState58 _v
            | LBRACE ->
                _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState58
            | LPAR ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState58
            | MINUS ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState58
            | NOT ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState58
            | STORELOAD ->
                _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState58
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState58) : 'freshtv174)
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState57) : 'freshtv176)
    | MenhirState58 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv179 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | COMMA | ELSE | EOF | EXTS | EXTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LPAR | NOT | RBRACKET | RPAR | STORELOAD ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv177 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((((_menhir_stack, _menhir_s), _, (_2 : 'tv_cond)), _, (_3 : 'tv_expr)), _), _, (_5 : 'tv_expr)) = _menhir_stack in
            let _4 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 143 "parser/dbacsl_parser.mly"
                               ( Dba.Expr.ite _2 _3 _5 )
# 1225 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv178)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState59) : 'freshtv180)
    | MenhirState9 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv201 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv199 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState60 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv195 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                let (_v : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1254 "parser/dbacsl_parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | COMMA ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv191 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1265 "parser/dbacsl_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | INT _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv187 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1275 "parser/dbacsl_parser.ml"
                        ))) = Obj.magic _menhir_stack in
                        let (_v : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1280 "parser/dbacsl_parser.ml"
                        )) = _v in
                        ((let _menhir_stack = (_menhir_stack, _v) in
                        let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RBRACE ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv183 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1291 "parser/dbacsl_parser.ml"
                            ))) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1295 "parser/dbacsl_parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv181 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1302 "parser/dbacsl_parser.ml"
                            ))) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1306 "parser/dbacsl_parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let (((((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)), _), (_4 : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1311 "parser/dbacsl_parser.ml"
                            ))), (_6 : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1315 "parser/dbacsl_parser.ml"
                            ))) = _menhir_stack in
                            let _7 = () in
                            let _5 = () in
                            let _3 = () in
                            let _1 = () in
                            let _v : 'tv_expr = 
# 130 "parser/dbacsl_parser.mly"
                                           (
    let off1 = int_of_string _4 in
    let off2 = int_of_string _6 in
    Dba.Expr.restrict off1 off2 _2
  )
# 1328 "parser/dbacsl_parser.ml"
                             in
                            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv182)) : 'freshtv184)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv185 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1338 "parser/dbacsl_parser.ml"
                            ))) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1342 "parser/dbacsl_parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let (((_menhir_stack, _menhir_s), _), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv186)) : 'freshtv188)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv189 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1353 "parser/dbacsl_parser.ml"
                        ))) = Obj.magic _menhir_stack in
                        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv190)) : 'freshtv192)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv193 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1364 "parser/dbacsl_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv194)) : 'freshtv196)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv197 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv198)) : 'freshtv200)
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState60
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState60) : 'freshtv202)
    | MenhirState8 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv203 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | RPAR ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState66
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState66) : 'freshtv204)
    | MenhirState68 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv209 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv207 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState69 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv205 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)), _, (_3 : 'tv_bin_op)), _, (_4 : 'tv_expr)) = _menhir_stack in
            let _5 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 148 "parser/dbacsl_parser.mly"
                               (
    infer_binary_op_size _2 _3 _4
  )
# 1577 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv206)) : 'freshtv208)
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState69) : 'freshtv210)
    | MenhirState7 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv213 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | AND | COMMA | CONCAT | DIVS | DIVU | ELSE | EOF | EQQ | EXTS | EXTU | GES | GEU | GTS | GTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | MULTS | MULTU | NEQ | NOT | OR | PLUS | RBRACKET | RPAR | RROTATE | RSHIFTS | RSHIFTU | STORELOAD | XOR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv211 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 129 "parser/dbacsl_parser.mly"
                               ( Dba.Expr.uminus e )
# 1610 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv212)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState71) : 'freshtv214)
    | MenhirState6 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv217 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState72
        | AND | COMMA | CONCAT | DIVS | DIVU | ELSE | EOF | EQQ | EXTS | EXTU | GES | GEU | GTS | GTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | MULTS | MULTU | NEQ | NOT | OR | PLUS | RBRACKET | RPAR | RROTATE | RSHIFTS | RSHIFTU | STORELOAD | XOR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv215 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 128 "parser/dbacsl_parser.mly"
                               ( Dba.Expr.lognot _2 )
# 1635 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv216)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState72) : 'freshtv218)
    | MenhirState5 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv235 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv229 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState73 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv225 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                let (_v : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1664 "parser/dbacsl_parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RBRACKET ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv221 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1675 "parser/dbacsl_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv219 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1682 "parser/dbacsl_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((((_menhir_stack, _menhir_s), _, (_3 : 'tv_expr)), _), (_5 : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1687 "parser/dbacsl_parser.ml"
                    ))) = _menhir_stack in
                    let _6 = () in
                    let _4 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : 'tv_expr = 
# 118 "parser/dbacsl_parser.mly"
                                               (
    let size =
      int_of_string _5
      |> Size.Bit.create
      |> Size.Byte.of_bitsize in
    Dba.Expr.load size Machine.LittleEndian _3
  )
# 1702 "parser/dbacsl_parser.ml"
                     in
                    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv220)) : 'freshtv222)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv223 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 1712 "parser/dbacsl_parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv224)) : 'freshtv226)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv227 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv228)) : 'freshtv230)
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | RBRACKET ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv233 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState73 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv231 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((_menhir_stack, _menhir_s), _, (_3 : 'tv_expr)) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 125 "parser/dbacsl_parser.mly"
                                     (
    Dba.Expr.load (Size.Byte.create 0) Machine.LittleEndian _3
  )
# 1788 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv232)) : 'freshtv234)
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState73) : 'freshtv236)
    | MenhirState86 | MenhirState78 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv237 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | CONCAT ->
            _menhir_run49 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | DIVS ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | DIVU ->
            _menhir_run47 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | EQQ ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | EQQ2 ->
            _menhir_run45 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | GES ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | GEU ->
            _menhir_run43 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | GTS ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | GTU ->
            _menhir_run41 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | LES ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | LEU ->
            _menhir_run38 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | LROTATE ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | LSHIFT ->
            _menhir_run36 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | LTS ->
            _menhir_run35 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | LTU ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | MINUS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | MODS ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | MODU ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | MULTS ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | MULTU ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | NEQ ->
            _menhir_run28 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | NEQ2 ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | OR ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | PLUS ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | RPAR ->
            _menhir_run67 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | RROTATE ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | RSHIFTS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | RSHIFTU ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | XOR ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState79) : 'freshtv238)
    | _ ->
        _menhir_fail ()

and _menhir_goto_cond : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_cond -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState14 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv103 * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState56
        | EXTU ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState56
        | HEXA _v ->
            _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState56 _v
        | IDENT _v ->
            _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState56 _v
        | IFJUMP ->
            _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState56
        | INT _v ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState56 _v
        | LBRACE ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState56
        | LPAR ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState56
        | MINUS ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState56
        | NOT ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState56
        | STORELOAD ->
            _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState56
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState56) : 'freshtv104)
    | MenhirState78 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv115 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv111 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EOF ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv107 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv105 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s), _), _, (c : 'tv_cond)) = _menhir_stack in
                let _6 = () in
                let _5 = () in
                let _3 = () in
                let _2 = () in
                let _1 = () in
                let _v : (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 1938 "parser/dbacsl_parser.ml"
                ) = 
# 94 "parser/dbacsl_parser.mly"
                                            ( Dba.Instr.assume c 0 )
# 1942 "parser/dbacsl_parser.ml"
                 in
                _menhir_goto_term _menhir_env _menhir_stack _menhir_s _v) : 'freshtv106)) : 'freshtv108)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv109 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv110)) : 'freshtv112)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv113 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv114)) : 'freshtv116)
    | MenhirState3 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv123 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv119 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv117 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (c : 'tv_cond)) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 1977 "parser/dbacsl_parser.ml"
            ) = 
# 94 "parser/dbacsl_parser.mly"
                                            ( Dba.Instr.assume c 0 )
# 1981 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_term _menhir_env _menhir_stack _menhir_s _v) : 'freshtv118)) : 'freshtv120)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv121 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv122)) : 'freshtv124)
    | MenhirState86 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv135 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv131 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EOF ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv127 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv125 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s), _), _, (c : 'tv_cond)) = _menhir_stack in
                let _6 = () in
                let _5 = () in
                let _3 = () in
                let _2 = () in
                let _1 = () in
                let _v : (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 2017 "parser/dbacsl_parser.ml"
                ) = 
# 97 "parser/dbacsl_parser.mly"
               ( Dba.Instr._assert c 0 )
# 2021 "parser/dbacsl_parser.ml"
                 in
                _menhir_goto_term _menhir_env _menhir_stack _menhir_s _v) : 'freshtv126)) : 'freshtv128)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv129 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv130)) : 'freshtv132)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv133 * _menhir_state)) * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv134)) : 'freshtv136)
    | MenhirState85 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv143 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv139 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv137 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (c : 'tv_cond)) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 2056 "parser/dbacsl_parser.ml"
            ) = 
# 97 "parser/dbacsl_parser.mly"
               ( Dba.Instr._assert c 0 )
# 2060 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_term _menhir_env _menhir_stack _menhir_s _v) : 'freshtv138)) : 'freshtv140)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv141 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv142)) : 'freshtv144)
    | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv151 * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv147 * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv145 * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (c : 'tv_cond)) = _menhir_stack in
            let _2 = () in
            let _v : (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 2086 "parser/dbacsl_parser.ml"
            ) = 
# 97 "parser/dbacsl_parser.mly"
               ( Dba.Instr._assert c 0 )
# 2090 "parser/dbacsl_parser.ml"
             in
            _menhir_goto_term _menhir_env _menhir_stack _menhir_s _v) : 'freshtv146)) : 'freshtv148)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv149 * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv150)) : 'freshtv152)
    | _ ->
        _menhir_fail ()

and _menhir_errorcase : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    match _menhir_s with
    | MenhirState86 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv43 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv44)
    | MenhirState85 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv45 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv46)
    | MenhirState79 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv47 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv48)
    | MenhirState78 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv49 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv50)
    | MenhirState73 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv51 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv52)
    | MenhirState72 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv53 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv54)
    | MenhirState71 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv55 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv56)
    | MenhirState69 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv57 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv58)
    | MenhirState68 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv59 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv60)
    | MenhirState66 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv61 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv62)
    | MenhirState60 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv63 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv64)
    | MenhirState59 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv65 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv66)
    | MenhirState58 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv67 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv68)
    | MenhirState57 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv69 * _menhir_state) * _menhir_state * 'tv_cond) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv70)
    | MenhirState56 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv71 * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv72)
    | MenhirState55 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv73 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv74)
    | MenhirState53 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv75 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv76)
    | MenhirState52 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv77 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv78)
    | MenhirState51 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv79 * _menhir_state * 'tv_expr) * _menhir_state * 'tv_bin_op) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv80)
    | MenhirState20 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv81 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv82)
    | MenhirState19 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv83 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv84)
    | MenhirState18 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv85 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv86)
    | MenhirState14 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv87 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv88)
    | MenhirState9 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv89 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv90)
    | MenhirState8 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv91 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv92)
    | MenhirState7 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv93 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv94)
    | MenhirState6 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv95 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv96)
    | MenhirState5 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv97 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv98)
    | MenhirState3 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv99 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv100)
    | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv101) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv102)

and _menhir_run1 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv41) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_cond = 
# 187 "parser/dbacsl_parser.mly"
           ( Dba.Expr._true )
# 2266 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_cond _menhir_env _menhir_stack _menhir_s _v) : 'freshtv42)

and _menhir_run4 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACKET ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv37 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState5
        | EXTU ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState5
        | HEXA _v ->
            _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState5 _v
        | IDENT _v ->
            _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState5 _v
        | IFJUMP ->
            _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState5
        | INT _v ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState5 _v
        | LBRACE ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState5
        | LPAR ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState5
        | MINUS ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState5
        | NOT ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState5
        | STORELOAD ->
            _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState5
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState5) : 'freshtv38)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv39 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv40)

and _menhir_run6 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState6
    | EXTU ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState6
    | HEXA _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState6 _v
    | IDENT _v ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState6 _v
    | IFJUMP ->
        _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState6
    | INT _v ->
        _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState6 _v
    | LBRACE ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState6
    | LPAR ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState6
    | MINUS ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState6
    | NOT ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState6
    | STORELOAD ->
        _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState6
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState6

and _menhir_run7 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState7
    | EXTU ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState7
    | HEXA _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState7 _v
    | IDENT _v ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState7 _v
    | IFJUMP ->
        _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState7
    | INT _v ->
        _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState7 _v
    | LBRACE ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState7
    | LPAR ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState7
    | MINUS ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState7
    | NOT ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState7
    | STORELOAD ->
        _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState7
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState7

and _menhir_run8 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState8
    | EXTU ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState8
    | HEXA _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState8 _v
    | IDENT _v ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState8 _v
    | IFJUMP ->
        _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState8
    | INT _v ->
        _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState8 _v
    | LBRACE ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState8
    | LPAR ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState8
    | MINUS ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState8
    | NOT ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState8
    | STORELOAD ->
        _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState8
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState8

and _menhir_run9 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState9
    | EXTU ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState9
    | HEXA _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState9 _v
    | IDENT _v ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState9 _v
    | IFJUMP ->
        _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState9
    | INT _v ->
        _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState9 _v
    | LBRACE ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState9
    | LPAR ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState9
    | MINUS ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState9
    | NOT ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState9
    | STORELOAD ->
        _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState9
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState9

and _menhir_run10 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2451 "parser/dbacsl_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INFER ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv31 * _menhir_state * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2463 "parser/dbacsl_parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv27 * _menhir_state * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2473 "parser/dbacsl_parser.ml"
            ))) = Obj.magic _menhir_stack in
            let (_v : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2478 "parser/dbacsl_parser.ml"
            )) = _v in
            ((let _menhir_stack = (_menhir_stack, _v) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | SUPER ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv23 * _menhir_state * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2489 "parser/dbacsl_parser.ml"
                ))) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2493 "parser/dbacsl_parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv21 * _menhir_state * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2500 "parser/dbacsl_parser.ml"
                ))) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2504 "parser/dbacsl_parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, (_1 : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2509 "parser/dbacsl_parser.ml"
                ))), (_3 : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2513 "parser/dbacsl_parser.ml"
                ))) = _menhir_stack in
                let _4 = () in
                let _2 = () in
                let _v : 'tv_expr = 
# 101 "parser/dbacsl_parser.mly"
                        (
    let size = int_of_string _3 in
    let bigint = (Bigint.big_int_of_string _1) in
    let bv = Bitvector.create bigint size in
    Dba.Expr.constant bv
  )
# 2525 "parser/dbacsl_parser.ml"
                 in
                _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv22)) : 'freshtv24)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv25 * _menhir_state * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2535 "parser/dbacsl_parser.ml"
                ))) * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2539 "parser/dbacsl_parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv26)) : 'freshtv28)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv29 * _menhir_state * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2550 "parser/dbacsl_parser.ml"
            ))) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv30)) : 'freshtv32)
    | AND | COMMA | CONCAT | DIVS | DIVU | ELSE | EOF | EQQ | EQQ2 | EXTS | EXTU | GES | GEU | GTS | GTU | HEXA _ | IDENT _ | IFJUMP | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | MULTS | MULTU | NEQ | NEQ2 | NOT | OR | PLUS | RBRACKET | RPAR | RROTATE | RSHIFTS | RSHIFTU | STORELOAD | XOR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv33 * _menhir_state * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2559 "parser/dbacsl_parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (_1 : (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2564 "parser/dbacsl_parser.ml"
        ))) = _menhir_stack in
        let _v : 'tv_expr = 
# 107 "parser/dbacsl_parser.mly"
        (
    let bigint = Bigint.big_int_of_string _1 in
    Expr.constant (Bitvector.create bigint 0)
  )
# 2572 "parser/dbacsl_parser.ml"
         in
        _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv34)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv35 * _menhir_state * (
# 70 "parser/dbacsl_parser.mly"
       (string)
# 2582 "parser/dbacsl_parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv36)

and _menhir_run14 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | EXTU ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | FALSE ->
        _menhir_run17 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | HEXA _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState14 _v
    | IDENT _v ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState14 _v
    | IFJUMP ->
        _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | INT _v ->
        _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState14 _v
    | LBRACE ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | LPAR ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | MINUS ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | NOT ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | STORELOAD ->
        _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | TRUE ->
        _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState14
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState14

and _menhir_run15 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 71 "parser/dbacsl_parser.mly"
       (string)
# 2627 "parser/dbacsl_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv19) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 71 "parser/dbacsl_parser.mly"
       (string)
# 2637 "parser/dbacsl_parser.ml"
    )) : (
# 71 "parser/dbacsl_parser.mly"
       (string)
# 2641 "parser/dbacsl_parser.ml"
    )) = _v in
    ((let _v : 'tv_expr = 
# 117 "parser/dbacsl_parser.mly"
          ( Parse_helpers.expr_of_name _1 )
# 2646 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv20)

and _menhir_run16 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 72 "parser/dbacsl_parser.mly"
       (string * int)
# 2653 "parser/dbacsl_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv17) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 72 "parser/dbacsl_parser.mly"
       (string * int)
# 2663 "parser/dbacsl_parser.ml"
    )) : (
# 72 "parser/dbacsl_parser.mly"
       (string * int)
# 2667 "parser/dbacsl_parser.ml"
    )) = _v in
    ((let _v : 'tv_expr = 
# 111 "parser/dbacsl_parser.mly"
         (
    let s, size = _1 in
    let bigint = Bigint.big_int_of_string s in
    let bv = Bitvector.create bigint size in
    Expr.constant bv
  )
# 2677 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv18)

and _menhir_run17 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv15) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_cond = 
# 188 "parser/dbacsl_parser.mly"
           ( Dba.Expr._false )
# 2691 "parser/dbacsl_parser.ml"
     in
    _menhir_goto_cond _menhir_env _menhir_stack _menhir_s _v) : 'freshtv16)

and _menhir_run18 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState18
    | EXTU ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState18
    | HEXA _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState18 _v
    | IDENT _v ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState18 _v
    | IFJUMP ->
        _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState18
    | INT _v ->
        _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState18 _v
    | LBRACE ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState18
    | LPAR ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState18
    | MINUS ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState18
    | NOT ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState18
    | STORELOAD ->
        _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState18
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState18

and _menhir_run19 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | EXTU ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | HEXA _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState19 _v
    | IDENT _v ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState19 _v
    | IFJUMP ->
        _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | INT _v ->
        _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState19 _v
    | LBRACE ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | LPAR ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | MINUS ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | NOT ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | STORELOAD ->
        _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState19
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState19

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

and term : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 88 "parser/dbacsl_parser.mly"
      (Dba.Instr.t)
# 2776 "parser/dbacsl_parser.ml"
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
    let (_menhir_stack : 'freshtv13) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | EXTU ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | FALSE ->
        _menhir_run17 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | HEXA _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState0 _v
    | IDENT _v ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState0 _v
    | IFJUMP ->
        _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | INT _v ->
        _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState0 _v
    | LBRACE ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | LPAR ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | MINUS ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | NOT ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | STORELOAD ->
        _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | TERM_TOKEN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv11) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState0 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ASSERT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv3 * _menhir_state) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState85
            | EXTU ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState85
            | FALSE ->
                _menhir_run17 _menhir_env (Obj.magic _menhir_stack) MenhirState85
            | HEXA _v ->
                _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState85 _v
            | IDENT _v ->
                _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState85 _v
            | IFJUMP ->
                _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState85
            | INT _v ->
                _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState85 _v
            | LBRACE ->
                _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState85
            | LPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv1 * _menhir_state)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState85 in
                ((let _menhir_stack = (_menhir_stack, _menhir_s) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | EXTS ->
                    _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | EXTU ->
                    _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | FALSE ->
                    _menhir_run17 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | HEXA _v ->
                    _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState86 _v
                | IDENT _v ->
                    _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState86 _v
                | IFJUMP ->
                    _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | INT _v ->
                    _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState86 _v
                | LBRACE ->
                    _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | LPAR ->
                    _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | MINUS ->
                    _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | NOT ->
                    _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | STORELOAD ->
                    _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | TRUE ->
                    _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState86
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState86) : 'freshtv2)
            | MINUS ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState85
            | NOT ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState85
            | STORELOAD ->
                _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState85
            | TRUE ->
                _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState85
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState85) : 'freshtv4)
        | ASSUME ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv7 * _menhir_state) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState3
            | EXTU ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState3
            | FALSE ->
                _menhir_run17 _menhir_env (Obj.magic _menhir_stack) MenhirState3
            | HEXA _v ->
                _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState3 _v
            | IDENT _v ->
                _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState3 _v
            | IFJUMP ->
                _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState3
            | INT _v ->
                _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState3 _v
            | LBRACE ->
                _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState3
            | LPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv5 * _menhir_state)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState3 in
                ((let _menhir_stack = (_menhir_stack, _menhir_s) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | EXTS ->
                    _menhir_run19 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | EXTU ->
                    _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | FALSE ->
                    _menhir_run17 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | HEXA _v ->
                    _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState78 _v
                | IDENT _v ->
                    _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState78 _v
                | IFJUMP ->
                    _menhir_run14 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | INT _v ->
                    _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState78 _v
                | LBRACE ->
                    _menhir_run9 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | LPAR ->
                    _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | MINUS ->
                    _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | NOT ->
                    _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | STORELOAD ->
                    _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | TRUE ->
                    _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState78
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState78) : 'freshtv6)
            | MINUS ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState3
            | NOT ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) MenhirState3
            | STORELOAD ->
                _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState3
            | TRUE ->
                _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState3
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState3) : 'freshtv8)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv9 * _menhir_state) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv10)) : 'freshtv12)
    | TRUE ->
        _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState0) : 'freshtv14))

# 269 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
  

# 2988 "parser/dbacsl_parser.ml"
