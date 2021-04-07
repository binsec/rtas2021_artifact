
module MenhirBasics = struct
  
  exception Error = Parsing.Parse_error
  
  type token = 
    | XOR
    | UNDEF
    | TRUE
    | SUPER
    | STRING of (
# 233 "parser/parser_infos.mly"
       (string)
# 15 "parser/parser_infos.ml"
  )
    | STORELOAD
    | STOP
    | STATE_OK
    | STATE_KO
    | STACK
    | SEMICOLON
    | RSHIFTU
    | RSHIFTS
    | RROTATE
    | RPAR
    | RETURNFLAG
    | REPLACEINSTRAT
    | RBRACKET
    | RBRACE
    | PRINT
    | PLUS
    | OR
    | NOT
    | NONDETASSUME
    | NONDET
    | NEXT
    | NEQ
    | MULTU
    | MULTS
    | MODU
    | MODS
    | MINUS
    | MALLOC
    | LTU
    | LTS
    | LSHIFT
    | LROTATE
    | LPAR
    | LOWUNSIGNEDTHRESHOLDS
    | LOWSIGNEDTHRESHOLDS
    | LOCALWIDENINGDELAY
    | LOCALTHRESHOLDS
    | LNEXT
    | LINEAR
    | LEU
    | LES
    | LBRACKET
    | LBRACE
    | JUMP
    | INT of (
# 231 "parser/parser_infos.mly"
       (string)
# 64 "parser/parser_infos.ml"
  )
    | INFER
    | IFJUMP
    | IDENT of (
# 232 "parser/parser_infos.mly"
       (string)
# 71 "parser/parser_infos.ml"
  )
    | HIGHUNSIGNEDTHRESHOLDS
    | HIGHSIGNEDTHRESHOLDS
    | HEXADDRESS of (
# 235 "parser/parser_infos.mly"
       (string * int)
# 78 "parser/parser_infos.ml"
  )
    | HEXA of (
# 234 "parser/parser_infos.mly"
       (string * int)
# 83 "parser/parser_infos.ml"
  )
    | GTU
    | GTS
    | GLOBALWIDENINGDELAY
    | GLOBALTHRESHOLDS
    | GEU
    | GES
    | FREE
    | FALSE
    | EXTU
    | EXTS
    | EQQ
    | EOF
    | ENTR
    | ELSE
    | DIVU
    | DIVS
    | CONSTANT
    | CONCAT
    | COMMA
    | COLON
    | CLOSEDJUMPS
    | CALLFLAG
    | ASSUME
    | ASSIGN
    | ASSERT
    | ARROWINV
    | ARROW
    | ANNOT
    | AND
    | ADDINSTRAT
  
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
  | MenhirState320
  | MenhirState318
  | MenhirState316
  | MenhirState314
  | MenhirState312
  | MenhirState310
  | MenhirState308
  | MenhirState306
  | MenhirState305
  | MenhirState304
  | MenhirState303
  | MenhirState301
  | MenhirState298
  | MenhirState295
  | MenhirState291
  | MenhirState285
  | MenhirState281
  | MenhirState279
  | MenhirState275
  | MenhirState273
  | MenhirState271
  | MenhirState270
  | MenhirState264
  | MenhirState262
  | MenhirState259
  | MenhirState256
  | MenhirState251
  | MenhirState249
  | MenhirState247
  | MenhirState246
  | MenhirState243
  | MenhirState240
  | MenhirState237
  | MenhirState231
  | MenhirState227
  | MenhirState221
  | MenhirState209
  | MenhirState204
  | MenhirState199
  | MenhirState194
  | MenhirState175
  | MenhirState173
  | MenhirState168
  | MenhirState162
  | MenhirState155
  | MenhirState153
  | MenhirState145
  | MenhirState133
  | MenhirState127
  | MenhirState124
  | MenhirState122
  | MenhirState91
  | MenhirState88
  | MenhirState41
  | MenhirState39
  | MenhirState34
  | MenhirState33
  | MenhirState30
  | MenhirState25
  | MenhirState23
  | MenhirState22
  | MenhirState21
  | MenhirState19
  | MenhirState17
  | MenhirState13
  | MenhirState11
  | MenhirState8
  | MenhirState2
  | MenhirState0

# 22 "parser/parser_infos.mly"
  
  open Infos
  open Dba

  type labeladdress =
  | Label of string
  | Address of id jump_target

  type labeloffset =
  | LabelIf of string
  | OffsetIf of id

  type parserinstrkind =
  | Assign of LValue.t * Expr.t * id
  | SJump of labeladdress * Tag.t option
  | DJump of Expr.t * Tag.t option
  | If of Expr.t * labeladdress * labeloffset
  | Stop of state option
  | Assert of Expr.t * id
  | Assume of Expr.t * id
  | NondetAssume of LValue.t list * Expr.t * id
  | Nondet of LValue.t * region * id
  | Undef of LValue.t * id
  | Malloc of LValue.t * Expr.t * id
  | Free of Expr.t * id
  | Print of printable list * id


  let incindex addr i =
    Dba_types.Caddress.reid addr (addr.id + i)

  let index = ref 0

  let locallabelMap = ref Basic_types.String.Map.empty

  let reset_label_map = locallabelMap := Basic_types.String.Map.empty

  let rec resolve_labels insns =
    match insns with
    | [] -> []
    | i :: l ->
      let h =
        match i with
        | If(cond, thn, els) -> (
            match thn, els with
            | Address a, OffsetIf b ->
              Instr.ite cond a b
            | Address a, LabelIf b -> (
              match Basic_types.String.Map.find b !locallabelMap with
              | b' -> Instr.ite cond a b'
              | exception Not_found ->
                 let message = "parser_infos.mly: jump to undefined label " ^ b in
                 failwith message
            )
            | Label b, OffsetIf a -> (
              match Basic_types.String.Map.find b !locallabelMap with
              | b' -> Instr.ite cond (Jump_target.inner b') a
              | exception Not_found ->
                 let message = "parser_infos.mly: jump to undefined label " ^ b in
                 failwith message
            )
            | Label a, LabelIf b -> (
              try
                let a' = Basic_types.String.Map.find a !locallabelMap in
                let b' = Basic_types.String.Map.find b !locallabelMap in
                Instr.ite cond (Jump_target.inner a') b'
              with Not_found ->
                let message = "parsel_infos.mly: jump to undefined label" in
                failwith message
            )
          )
        | SJump(dst, tag) -> (
          match dst with
          | Address a -> Instr.static_jump a ?tag
          | Label b -> (
            match Basic_types.String.Map.find b !locallabelMap with
            | b' -> Instr.static_jump (Jump_target.inner b') ?tag
            | exception Not_found ->
              let message = "parser_infos.mly: jump to undefined label " ^ b in
              failwith message
          )
        )
        | Assign(lhs, expr, a) -> Instr.assign lhs expr a
        | DJump(dst, tag) -> Instr.dynamic_jump dst ?tag
        | Stop tag -> Instr.stop tag
        | Undef (lhs, a) -> Instr.undefined lhs a
        | Malloc (a, b, c) -> Instr.malloc a b c
        | Free (a, b) -> Instr.free a b
        | Assert (a, b) -> Instr._assert a b
        | Assume (a, b) -> Instr.assume a b
        | NondetAssume (a, b, c) -> Instr.non_deterministic_assume a b c
        | Nondet (a, region, c) -> Instr.non_deterministic a ~region c
        | Print (a, b) -> Instr.print a b
      in let c = try (Dba_utils.checksize_instruction h)
        with
        | Errors.Assignment_size_conflict s ->
            failwith("Assignment_size_conflict " ^ s ^" in info file")
        | Errors.Undeclared_variable s ->
          failwith ("Undeclared_variable " ^ s ^" in info file")
        | Errors.Bad_address_size ->
          failwith ("Bad_address_size in info file")
        | Errors.Bad_exp_size ->
          failwith ("Bad_expression_size in info file")
        | Errors.Operands_size_conflict s ->
          failwith("Operands_size_conflict " ^s^" in info file")
        | Errors.Size_error _ ->
          failwith ("size error in info file")
         in
         if c then h :: (resolve_labels l)
         else	failwith ("size error in info file")


  let chain_insns addr insns =
    let rec aux addr insns l =
      let last_instr =
        Instr.dynamic_jump (Expr.var "ret" 32) ~tag:Dba.Return in
      match insns with
      | [] -> [addr, last_instr]
      | i :: [] -> (
        match i with
        | Dba.Instr.If(cond, thn, els) -> l @ [addr, Instr.ite cond thn els]
        | Dba.Instr.Assign(lhs, expr, _) ->
          l @
            [addr, Instr.assign lhs expr (addr.id + 1);
             incindex addr 1, last_instr
            ]
        | Dba.Instr.Undef (lhs, _) ->
          l @
            [addr, Instr.undefined lhs (addr.id + 1);
             incindex addr 1, last_instr]
        | Dba.Instr.SJump _
        | Dba.Instr.DJump _ as i -> l @ [(addr, i)]
        | Dba.Instr.Stop _ -> l @ [addr, i]
        | Dba.Instr.Malloc (a, b, _) ->
           l @ [addr, Instr.malloc a b (addr.id + 1);
                incindex addr 1, last_instr]
        | Dba.Instr.Free (a, _) ->
           l @ [addr, Instr.free a (addr.id + 1);
                incindex addr 1, last_instr]
        | Dba.Instr.Assert (a, _) ->
           l @ [addr, Instr._assert a (addr.id + 1);
                incindex addr 1, last_instr]
        | Dba.Instr.Assume (a, _) ->
           l @ [addr, Instr.assume a (addr.id + 1);
                incindex addr 1, last_instr]
        | Dba.Instr.NondetAssume (a, b, _) ->
           l @ [addr, Instr.non_deterministic_assume a b (addr.id + 1);
                incindex addr 1, last_instr]
        | Dba.Instr.Nondet (a, region, _) ->
           l @ [addr, Instr.non_deterministic a ~region (addr.id + 1);
                incindex addr 1, last_instr]
        | Dba.Instr.Print (a, _) ->
           l @ [addr, Instr.print a (addr.id + 1);
                incindex addr 1, last_instr]
      )
      | i :: insns ->
        let chained_i =
          match i with
          | Dba.Instr.SJump _
          | Dba.Instr.DJump _
          | Dba.Instr.Stop  _
          | Dba.Instr.If    _ -> i
          | Dba.Instr.Assign _
          | Dba.Instr.Undef _
          | Dba.Instr.Malloc _
          | Dba.Instr.Free _
          | Dba.Instr.Assert _
          | Dba.Instr.Assume _
          | Dba.Instr.NondetAssume _
          | Dba.Instr.Nondet _
          | Dba.Instr.Print _ -> Dba_types.Instruction.set_successor i (addr.id + 1)
        in aux (incindex addr 1) insns (l @ [addr, chained_i])
    in
    aux addr insns []

  let threshold_merge (los1, his1, lou1, hiu1) (los2, his2, lou2, hiu2) =
      (los1 @ los2, his1 @ his2, lou1 @ lou2, hiu1 @ hiu2)

  let mk_threshold (los, his, lou, hiu) =
      let signed = BoundThreshold.mk_from_list los his
      and unsigned = BoundThreshold.mk_from_list lou hiu in
      WideningThreshold.mk signed unsigned

  let empty_threshold = [], [], [], []

  let add_assoc_list_to_map initial_map add assoc =
      List.fold_left (fun m (k, v) -> add k v m) initial_map assoc

# 390 "parser/parser_infos.ml"

let rec _menhir_goto_nonempty_list_located_threshold_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_located_threshold_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState240 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1277 * _menhir_state)) * _menhir_state * 'tv_nonempty_list_located_threshold_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1273 * _menhir_state)) * _menhir_state * 'tv_nonempty_list_located_threshold_) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1271 * _menhir_state)) * _menhir_state * 'tv_nonempty_list_located_threshold_) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (local_tholds : 'tv_nonempty_list_located_threshold_)) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_local_widening_thresholds = 
# 333 "parser/parser_infos.mly"
  ( add_assoc_list_to_map Dba_types.Caddress.Map.empty Dba_types.Caddress.Map.add local_tholds )
# 415 "parser/parser_infos.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1269) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_local_widening_thresholds) = _v in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1267 * _menhir_state * 'tv_local_widening_thresholds) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ADDINSTRAT ->
                _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | CLOSEDJUMPS ->
                _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | ENTR ->
                _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | EOF ->
                _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | GLOBALTHRESHOLDS ->
                _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | GLOBALWIDENINGDELAY ->
                _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | JUMP ->
                _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | LINEAR ->
                _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | LOCALTHRESHOLDS ->
                _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | LOCALWIDENINGDELAY ->
                _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | REPLACEINSTRAT ->
                _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | STOP ->
                _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState304
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState304) : 'freshtv1268)) : 'freshtv1270)) : 'freshtv1272)) : 'freshtv1274)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1275 * _menhir_state)) * _menhir_state * 'tv_nonempty_list_located_threshold_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1276)) : 'freshtv1278)
    | MenhirState243 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1281 * _menhir_state * 'tv_located_threshold) * _menhir_state * 'tv_nonempty_list_located_threshold_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1279 * _menhir_state * 'tv_located_threshold) * _menhir_state * 'tv_nonempty_list_located_threshold_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_located_threshold)), _, (xs : 'tv_nonempty_list_located_threshold_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_located_threshold_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 471 "parser/parser_infos.ml"
         in
        _menhir_goto_nonempty_list_located_threshold_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1280)) : 'freshtv1282)
    | _ ->
        _menhir_fail ()

and _menhir_goto_nonempty_list_threshold_spec_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_threshold_spec_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState264 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1237 * _menhir_state * 'tv_threshold_spec) * _menhir_state * 'tv_nonempty_list_threshold_spec_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1235 * _menhir_state * 'tv_threshold_spec) * _menhir_state * 'tv_nonempty_list_threshold_spec_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_threshold_spec)), _, (xs : 'tv_nonempty_list_threshold_spec_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_threshold_spec_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 490 "parser/parser_infos.ml"
         in
        _menhir_goto_nonempty_list_threshold_spec_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1236)) : 'freshtv1238)
    | MenhirState247 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1265 * _menhir_state) * _menhir_state * 'tv_nonempty_list_threshold_spec_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1261 * _menhir_state) * _menhir_state * 'tv_nonempty_list_threshold_spec_) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1259 * _menhir_state) * _menhir_state * 'tv_nonempty_list_threshold_spec_) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (tspecs : 'tv_nonempty_list_threshold_spec_)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_threshold_decl = 
# 314 "parser/parser_infos.mly"
  ( mk_threshold (List.fold_left threshold_merge empty_threshold tspecs)  )
# 511 "parser/parser_infos.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1257) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_threshold_decl) = _v in
            ((match _menhir_s with
            | MenhirState246 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv1247 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : 'tv_threshold_decl) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv1245 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                let (_ : _menhir_state) = _menhir_s in
                let ((_3 : 'tv_threshold_decl) : 'tv_threshold_decl) = _v in
                ((let (_menhir_stack, _menhir_s, (_1 : 'tv_address)) = _menhir_stack in
                let _2 = () in
                let _v : 'tv_located_threshold = 
# 336 "parser/parser_infos.mly"
                                ( (_1, _3) )
# 532 "parser/parser_infos.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1243) = _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : 'tv_located_threshold) = _v in
                ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1241 * _menhir_state * 'tv_located_threshold) = Obj.magic _menhir_stack in
                ((assert (not _menhir_env._menhir_error);
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | HEXADDRESS _v ->
                    _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState243 _v
                | RBRACE ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv1239 * _menhir_state * 'tv_located_threshold) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, (x : 'tv_located_threshold)) = _menhir_stack in
                    let _v : 'tv_nonempty_list_located_threshold_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 553 "parser/parser_infos.ml"
                     in
                    _menhir_goto_nonempty_list_located_threshold_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1240)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState243) : 'freshtv1242)) : 'freshtv1244)) : 'freshtv1246)) : 'freshtv1248)
            | MenhirState291 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1255 * _menhir_state) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : 'tv_threshold_decl) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1253 * _menhir_state) = Obj.magic _menhir_stack in
                let (_ : _menhir_state) = _menhir_s in
                let ((_2 : 'tv_threshold_decl) : 'tv_threshold_decl) = _v in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                let _1 = () in
                let _v : 'tv_global_widening_thresholds = 
# 310 "parser/parser_infos.mly"
                                   ( _2 )
# 574 "parser/parser_infos.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1251) = _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : 'tv_global_widening_thresholds) = _v in
                ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1249 * _menhir_state * 'tv_global_widening_thresholds) = Obj.magic _menhir_stack in
                ((assert (not _menhir_env._menhir_error);
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | ADDINSTRAT ->
                    _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | CLOSEDJUMPS ->
                    _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | ENTR ->
                    _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | EOF ->
                    _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | GLOBALTHRESHOLDS ->
                    _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | GLOBALWIDENINGDELAY ->
                    _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | JUMP ->
                    _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | LINEAR ->
                    _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | LOCALTHRESHOLDS ->
                    _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | LOCALWIDENINGDELAY ->
                    _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | REPLACEINSTRAT ->
                    _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | STOP ->
                    _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState308
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState308) : 'freshtv1250)) : 'freshtv1252)) : 'freshtv1254)) : 'freshtv1256)
            | _ ->
                _menhir_fail ()) : 'freshtv1258)) : 'freshtv1260)) : 'freshtv1262)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1263 * _menhir_state) * _menhir_state * 'tv_nonempty_list_threshold_spec_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1264)) : 'freshtv1266)
    | _ ->
        _menhir_fail ()

and _menhir_goto_endianness : _menhir_env -> 'ttv_tail -> 'tv_endianness -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (((('freshtv1233 * _menhir_state)) * _menhir_state * 'tv_expr)) * 'tv_endianness) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COMMA ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv1229 * _menhir_state)) * _menhir_state * 'tv_expr)) * 'tv_endianness) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv1225 * _menhir_state)) * _menhir_state * 'tv_expr)) * 'tv_endianness)) = Obj.magic _menhir_stack in
            let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 646 "parser/parser_infos.ml"
            )) = _v in
            ((let _menhir_stack = (_menhir_stack, _v) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RBRACKET ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((((('freshtv1221 * _menhir_state)) * _menhir_state * 'tv_expr)) * 'tv_endianness)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 657 "parser/parser_infos.ml"
                )) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((((('freshtv1219 * _menhir_state)) * _menhir_state * 'tv_expr)) * 'tv_endianness)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 664 "parser/parser_infos.ml"
                )) = Obj.magic _menhir_stack in
                ((let ((((_menhir_stack, _menhir_s), _, (e : 'tv_expr)), (x : 'tv_endianness)), (bytes : (
# 231 "parser/parser_infos.mly"
       (string)
# 669 "parser/parser_infos.ml"
                ))) = _menhir_stack in
                let _7 = () in
                let _2_inlined1 = () in
                let _4 = () in
                let _2 = () in
                let _1 = () in
                let _v : 'tv_lhs = let endianness =
                  let _2 = _2_inlined1 in
                  let x = 
# 191 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 681 "parser/parser_infos.ml"
                   in
                  
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 686 "parser/parser_infos.ml"
                  
                in
                
# 451 "parser/parser_infos.mly"
                       (
   let size =  Size.Byte.of_string bytes in
   let endia =
     match endianness with
     | Some _endianness -> _endianness
     | None -> Kernel_options.Machine.endianness () in
   Dba.LValue.store size endia e
 )
# 699 "parser/parser_infos.ml"
                 in
                _menhir_goto_lhs _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1220)) : 'freshtv1222)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((((('freshtv1223 * _menhir_state)) * _menhir_state * 'tv_expr)) * 'tv_endianness)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 709 "parser/parser_infos.ml"
                )) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1224)) : 'freshtv1226)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv1227 * _menhir_state)) * _menhir_state * 'tv_expr)) * 'tv_endianness)) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1228)) : 'freshtv1230)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv1231 * _menhir_state)) * _menhir_state * 'tv_expr)) * 'tv_endianness) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1232)) : 'freshtv1234)

and _menhir_goto_bin_op : _menhir_env -> 'ttv_tail -> 'tv_bin_op -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (('freshtv1217 * _menhir_state) * _menhir_state * 'tv_expr) * 'tv_bin_op) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState88
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState88
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState88 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState88 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState88
    | INT _v ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState88 _v
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState88
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState88
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState88
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState88
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState88) : 'freshtv1218)

and _menhir_goto_threshold_spec : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_threshold_spec -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1215 * _menhir_state * 'tv_threshold_spec) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | HIGHSIGNEDTHRESHOLDS ->
        _menhir_run261 _menhir_env (Obj.magic _menhir_stack) MenhirState264
    | HIGHUNSIGNEDTHRESHOLDS ->
        _menhir_run258 _menhir_env (Obj.magic _menhir_stack) MenhirState264
    | LOWSIGNEDTHRESHOLDS ->
        _menhir_run255 _menhir_env (Obj.magic _menhir_stack) MenhirState264
    | LOWUNSIGNEDTHRESHOLDS ->
        _menhir_run248 _menhir_env (Obj.magic _menhir_stack) MenhirState264
    | RBRACE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1213 * _menhir_state * 'tv_threshold_spec) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_threshold_spec)) = _menhir_stack in
        let _v : 'tv_nonempty_list_threshold_spec_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 784 "parser/parser_infos.ml"
         in
        _menhir_goto_nonempty_list_threshold_spec_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1214)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState264) : 'freshtv1216)

and _menhir_goto_regionnondet : _menhir_env -> 'ttv_tail -> 'tv_regionnondet -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (((('freshtv1211 * _menhir_state * 'tv_lhs)) * _menhir_state)) * 'tv_regionnondet) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | RPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv1207 * _menhir_state * 'tv_lhs)) * _menhir_state)) * 'tv_regionnondet) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv1203 * _menhir_state * 'tv_lhs)) * _menhir_state)) * 'tv_regionnondet)) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv1201 * _menhir_state * 'tv_lhs)) * _menhir_state)) * 'tv_regionnondet)) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (_1 : 'tv_lhs)), _), (_5 : 'tv_regionnondet)) = _menhir_stack in
            let _7 = () in
            let _6 = () in
            let _4 = () in
            let _3 = () in
            let _2 = () in
            let _v : 'tv_inst = 
# 418 "parser/parser_infos.mly"
                                                      ( Nondet (_1, _5, 0) )
# 821 "parser/parser_infos.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1202)) : 'freshtv1204)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv1205 * _menhir_state * 'tv_lhs)) * _menhir_state)) * 'tv_regionnondet)) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1206)) : 'freshtv1208)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv1209 * _menhir_state * 'tv_lhs)) * _menhir_state)) * 'tv_regionnondet) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1210)) : 'freshtv1212)

and _menhir_goto_lhslist : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_lhslist -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState133 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1195 * _menhir_state))) * _menhir_state * 'tv_lhslist) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1191 * _menhir_state))) * _menhir_state * 'tv_lhslist) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | COMMA ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv1187 * _menhir_state))) * _menhir_state * 'tv_lhslist)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | EXTS ->
                    _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState145
                | EXTU ->
                    _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState145
                | FALSE ->
                    _menhir_run147 _menhir_env (Obj.magic _menhir_stack) MenhirState145
                | HEXA _v ->
                    _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState145 _v
                | IDENT _v ->
                    _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState145 _v
                | IFJUMP ->
                    _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState145
                | INT _v ->
                    _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState145 _v
                | LBRACE ->
                    _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState145
                | LPAR ->
                    _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState145
                | NOT ->
                    _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState145
                | STORELOAD ->
                    _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState145
                | TRUE ->
                    _menhir_run146 _menhir_env (Obj.magic _menhir_stack) MenhirState145
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState145) : 'freshtv1188)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv1189 * _menhir_state))) * _menhir_state * 'tv_lhslist)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1190)) : 'freshtv1192)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1193 * _menhir_state))) * _menhir_state * 'tv_lhslist) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1194)) : 'freshtv1196)
    | MenhirState153 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1199 * _menhir_state * 'tv_lhs)) * _menhir_state * 'tv_lhslist) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1197 * _menhir_state * 'tv_lhs)) * _menhir_state * 'tv_lhslist) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_lhs)), _, (_3 : 'tv_lhslist)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_lhslist = 
# 422 "parser/parser_infos.mly"
                     ( _1 :: _3 )
# 913 "parser/parser_infos.ml"
         in
        _menhir_goto_lhslist _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1198)) : 'freshtv1200)
    | _ ->
        _menhir_fail ()

and _menhir_goto_printargs : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_printargs -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState124 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1173 * _menhir_state * (
# 233 "parser/parser_infos.mly"
       (string)
# 928 "parser/parser_infos.ml"
        ))) * _menhir_state * 'tv_printargs) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1171 * _menhir_state * (
# 233 "parser/parser_infos.mly"
       (string)
# 934 "parser/parser_infos.ml"
        ))) * _menhir_state * 'tv_printargs) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (_1 : (
# 233 "parser/parser_infos.mly"
       (string)
# 939 "parser/parser_infos.ml"
        ))), _, (_3 : 'tv_printargs)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_printargs = 
# 433 "parser/parser_infos.mly"
                           ( (Str _1) :: _3 )
# 945 "parser/parser_infos.ml"
         in
        _menhir_goto_printargs _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1172)) : 'freshtv1174)
    | MenhirState127 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1177 * _menhir_state * 'tv_expr)) * _menhir_state * 'tv_printargs) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1175 * _menhir_state * 'tv_expr)) * _menhir_state * 'tv_printargs) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_expr)), _, (_3 : 'tv_printargs)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_printargs = 
# 432 "parser/parser_infos.mly"
                           ( (Exp _1) :: _3 )
# 958 "parser/parser_infos.ml"
         in
        _menhir_goto_printargs _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1176)) : 'freshtv1178)
    | MenhirState122 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1185 * _menhir_state) * _menhir_state * 'tv_printargs) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1181 * _menhir_state) * _menhir_state * 'tv_printargs) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1179 * _menhir_state) * _menhir_state * 'tv_printargs) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_printargs)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_inst = 
# 419 "parser/parser_infos.mly"
                             ( Print (_2, 0) )
# 979 "parser/parser_infos.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1180)) : 'freshtv1182)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1183 * _menhir_state) * _menhir_state * 'tv_printargs) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1184)) : 'freshtv1186)
    | _ ->
        _menhir_fail ()

and _menhir_goto_region : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_region -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ('freshtv1169 * _menhir_state) * _menhir_state * 'tv_region) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COMMA ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1165 * _menhir_state) * _menhir_state * 'tv_region) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | HEXA _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1141 * _menhir_state) * _menhir_state * 'tv_region)) = Obj.magic _menhir_stack in
            let (_v : (
# 234 "parser/parser_infos.mly"
       (string * int)
# 1012 "parser/parser_infos.ml"
            )) = _v in
            ((let _menhir_stack = (_menhir_stack, _v) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1137 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 234 "parser/parser_infos.mly"
       (string * int)
# 1023 "parser/parser_infos.ml"
                )) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1135 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 234 "parser/parser_infos.mly"
       (string * int)
# 1030 "parser/parser_infos.ml"
                )) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s), _, (region : 'tv_region)), (_4 : (
# 234 "parser/parser_infos.mly"
       (string * int)
# 1035 "parser/parser_infos.ml"
                ))) = _menhir_stack in
                let _5 = () in
                let _3 = () in
                let _1 = () in
                let _v : 'tv_expr = 
# 488 "parser/parser_infos.mly"
                                       (
   let s, size = _4 in
   let bigint = (Bigint.big_int_of_string s) in
   let bv = Bitvector.create bigint size in
   Dba.Expr.constant ~region bv
 )
# 1048 "parser/parser_infos.ml"
                 in
                _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1136)) : 'freshtv1138)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1139 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 234 "parser/parser_infos.mly"
       (string * int)
# 1058 "parser/parser_infos.ml"
                )) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1140)) : 'freshtv1142)
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1161 * _menhir_state) * _menhir_state * 'tv_region)) = Obj.magic _menhir_stack in
            let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 1068 "parser/parser_infos.ml"
            )) = _v in
            ((let _menhir_stack = (_menhir_stack, _v) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INFER ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1157 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1079 "parser/parser_infos.ml"
                )) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | INT _v ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1153 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1089 "parser/parser_infos.ml"
                    ))) = Obj.magic _menhir_stack in
                    let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 1094 "parser/parser_infos.ml"
                    )) = _v in
                    ((let _menhir_stack = (_menhir_stack, _v) in
                    let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | SUPER ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv1149 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1105 "parser/parser_infos.ml"
                        ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1109 "parser/parser_infos.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RPAR ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv1145 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1119 "parser/parser_infos.ml"
                            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1123 "parser/parser_infos.ml"
                            ))) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv1143 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1130 "parser/parser_infos.ml"
                            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1134 "parser/parser_infos.ml"
                            ))) = Obj.magic _menhir_stack in
                            ((let ((((_menhir_stack, _menhir_s), _, (region : 'tv_region)), (_4 : (
# 231 "parser/parser_infos.mly"
       (string)
# 1139 "parser/parser_infos.ml"
                            ))), (_6 : (
# 231 "parser/parser_infos.mly"
       (string)
# 1143 "parser/parser_infos.ml"
                            ))) = _menhir_stack in
                            let _8 = () in
                            let _7 = () in
                            let _5 = () in
                            let _3 = () in
                            let _1 = () in
                            let _v : 'tv_expr = 
# 481 "parser/parser_infos.mly"
                                                      (
   let bigint = Bigint.big_int_of_string _4 in
   let size = int_of_string _6 in
   let bv = Bitvector.create bigint size in
   Dba.Expr.constant ~region bv
 )
# 1158 "parser/parser_infos.ml"
                             in
                            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1144)) : 'freshtv1146)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv1147 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1168 "parser/parser_infos.ml"
                            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1172 "parser/parser_infos.ml"
                            ))) = Obj.magic _menhir_stack in
                            ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1148)) : 'freshtv1150)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv1151 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1183 "parser/parser_infos.ml"
                        ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1187 "parser/parser_infos.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1152)) : 'freshtv1154)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1155 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1198 "parser/parser_infos.ml"
                    ))) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1156)) : 'freshtv1158)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1159 * _menhir_state) * _menhir_state * 'tv_region)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1209 "parser/parser_infos.ml"
                )) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1160)) : 'freshtv1162)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1163 * _menhir_state) * _menhir_state * 'tv_region)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1164)) : 'freshtv1166)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1167 * _menhir_state) * _menhir_state * 'tv_region) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1168)) : 'freshtv1170)

and _menhir_goto_expr : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState34 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv823 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv819 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 1244 "parser/parser_infos.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv817 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let ((_3 : (
# 231 "parser/parser_infos.mly"
       (string)
# 1252 "parser/parser_infos.ml"
            )) : (
# 231 "parser/parser_infos.mly"
       (string)
# 1256 "parser/parser_infos.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 520 "parser/parser_infos.mly"
                 ( let size = int_of_string _3 in Dba.Expr.sext size _2 )
# 1263 "parser/parser_infos.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv818)) : 'freshtv820)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv821 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv822)) : 'freshtv824)
    | MenhirState33 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv831 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv827 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 1285 "parser/parser_infos.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv825 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let ((_3 : (
# 231 "parser/parser_infos.mly"
       (string)
# 1293 "parser/parser_infos.ml"
            )) : (
# 231 "parser/parser_infos.mly"
       (string)
# 1297 "parser/parser_infos.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 519 "parser/parser_infos.mly"
                 ( let size = int_of_string _3 in Dba.Expr.uext size _2 )
# 1304 "parser/parser_infos.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv826)) : 'freshtv828)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv829 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv830)) : 'freshtv832)
    | MenhirState30 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv833 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState39
        | EXTU ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState39
        | HEXA _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState39 _v
        | IDENT _v ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState39 _v
        | IFJUMP ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState39
        | INT _v ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState39 _v
        | LBRACE ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState39
        | LPAR ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState39
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState39
        | STORELOAD ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState39
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState39) : 'freshtv834)
    | MenhirState39 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv839 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ELSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv835 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState41
            | EXTU ->
                _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState41
            | HEXA _v ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState41 _v
            | IDENT _v ->
                _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState41 _v
            | IFJUMP ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState41
            | INT _v ->
                _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState41 _v
            | LBRACE ->
                _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState41
            | LPAR ->
                _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState41
            | NOT ->
                _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState41
            | STORELOAD ->
                _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState41
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState41) : 'freshtv836)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv837 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv838)) : 'freshtv840)
    | MenhirState41 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv843 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_expr)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv841 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_expr)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)), _, (_3 : 'tv_expr)), _, (_5 : 'tv_expr)) = _menhir_stack in
        let _4 = () in
        let _1 = () in
        let _v : 'tv_expr = 
# 521 "parser/parser_infos.mly"
                              ( Dba.Expr.ite _2 _3 _5 )
# 1398 "parser/parser_infos.ml"
         in
        _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv842)) : 'freshtv844)
    | MenhirState25 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv867 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv863 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv859 * _menhir_state) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 1419 "parser/parser_infos.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | COMMA ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv855 * _menhir_state) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1430 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | INT _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv851 * _menhir_state) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1440 "parser/parser_infos.ml"
                        ))) = Obj.magic _menhir_stack in
                        let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 1445 "parser/parser_infos.ml"
                        )) = _v in
                        ((let _menhir_stack = (_menhir_stack, _v) in
                        let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RBRACE ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv847 * _menhir_state) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1456 "parser/parser_infos.ml"
                            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1460 "parser/parser_infos.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv845 * _menhir_state) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1467 "parser/parser_infos.ml"
                            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1471 "parser/parser_infos.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)), (_4 : (
# 231 "parser/parser_infos.mly"
       (string)
# 1476 "parser/parser_infos.ml"
                            ))), (_6 : (
# 231 "parser/parser_infos.mly"
       (string)
# 1480 "parser/parser_infos.ml"
                            ))) = _menhir_stack in
                            let _7 = () in
                            let _5 = () in
                            let _3 = () in
                            let _1 = () in
                            let _v : 'tv_expr = 
# 514 "parser/parser_infos.mly"
                                          (
   let off1 = int_of_string _4 in
   let off2 = int_of_string _6 in
   Dba.Expr.restrict off1 off2 _2
 )
# 1493 "parser/parser_infos.ml"
                             in
                            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv846)) : 'freshtv848)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv849 * _menhir_state) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1503 "parser/parser_infos.ml"
                            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1507 "parser/parser_infos.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv850)) : 'freshtv852)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv853 * _menhir_state) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1518 "parser/parser_infos.ml"
                        ))) = Obj.magic _menhir_stack in
                        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv854)) : 'freshtv856)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv857 * _menhir_state) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 1529 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv858)) : 'freshtv860)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv861 * _menhir_state) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv862)) : 'freshtv864)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv865 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv866)) : 'freshtv868)
    | MenhirState23 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv983 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv871) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv869) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 530 "parser/parser_infos.mly"
       ( Dba.Binary_op.And )
# 1563 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv870)) : 'freshtv872)
        | CONCAT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv875) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv873) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 532 "parser/parser_infos.mly"
          ( Dba.Binary_op.Concat )
# 1576 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv874)) : 'freshtv876)
        | DIVS ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv879) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv877) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 548 "parser/parser_infos.mly"
            ( Dba.Binary_op.DivS)
# 1589 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv878)) : 'freshtv880)
        | DIVU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv883) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv881) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 547 "parser/parser_infos.mly"
          ( Dba.Binary_op.DivU)
# 1602 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv882)) : 'freshtv884)
        | EQQ ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv887) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv885) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 533 "parser/parser_infos.mly"
       ( Dba.Binary_op.Eq )
# 1615 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv886)) : 'freshtv888)
        | GES ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv891) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv889) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 541 "parser/parser_infos.mly"
       ( Dba.Binary_op.GeqS )
# 1628 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv890)) : 'freshtv892)
        | GEU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv895) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv893) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 537 "parser/parser_infos.mly"
       ( Dba.Binary_op.GeqU )
# 1641 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv894)) : 'freshtv896)
        | GTS ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv899) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv897) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 542 "parser/parser_infos.mly"
       ( Dba.Binary_op.GtS )
# 1654 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv898)) : 'freshtv900)
        | GTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv903) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv901) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 538 "parser/parser_infos.mly"
       ( Dba.Binary_op.GtU )
# 1667 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv902)) : 'freshtv904)
        | LES ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv907) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv905) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 539 "parser/parser_infos.mly"
       ( Dba.Binary_op.LeqS )
# 1680 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv906)) : 'freshtv908)
        | LEU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv911) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv909) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 535 "parser/parser_infos.mly"
       ( Dba.Binary_op.LeqU )
# 1693 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv910)) : 'freshtv912)
        | LROTATE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv915) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv913) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 552 "parser/parser_infos.mly"
            ( Dba.Binary_op.LeftRotate )
# 1706 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv914)) : 'freshtv916)
        | LSHIFT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv919) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv917) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 549 "parser/parser_infos.mly"
           ( Dba.Binary_op.LShift )
# 1719 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv918)) : 'freshtv920)
        | LTS ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv923) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv921) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 540 "parser/parser_infos.mly"
       ( Dba.Binary_op.LtS )
# 1732 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv922)) : 'freshtv924)
        | LTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv927) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv925) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 536 "parser/parser_infos.mly"
       ( Dba.Binary_op.LtU  )
# 1745 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv926)) : 'freshtv928)
        | MINUS ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv931) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv929) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 544 "parser/parser_infos.mly"
         ( Dba.Binary_op.Minus )
# 1758 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv930)) : 'freshtv932)
        | MODS ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv935) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv933) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 528 "parser/parser_infos.mly"
        ( Dba.Binary_op.ModS )
# 1771 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv934)) : 'freshtv936)
        | MODU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv939) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv937) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 527 "parser/parser_infos.mly"
        ( Dba.Binary_op.ModU )
# 1784 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv938)) : 'freshtv940)
        | MULTS ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv943) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv941) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 546 "parser/parser_infos.mly"
         ( Dba.Binary_op.Mult)
# 1797 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv942)) : 'freshtv944)
        | MULTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv947) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv945) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 545 "parser/parser_infos.mly"
         ( Dba.Binary_op.Mult)
# 1810 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv946)) : 'freshtv948)
        | NEQ ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv951) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv949) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 534 "parser/parser_infos.mly"
       ( Dba.Binary_op.Diff )
# 1823 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv950)) : 'freshtv952)
        | OR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv955) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv953) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 529 "parser/parser_infos.mly"
       ( Dba.Binary_op.Or )
# 1836 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv954)) : 'freshtv956)
        | PLUS ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv959) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv957) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 543 "parser/parser_infos.mly"
         ( Dba.Binary_op.Plus )
# 1849 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv958)) : 'freshtv960)
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv963 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv961 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 522 "parser/parser_infos.mly"
                              ( _2 )
# 1864 "parser/parser_infos.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv962)) : 'freshtv964)
        | RROTATE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv967) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv965) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 553 "parser/parser_infos.mly"
            ( Dba.Binary_op.RightRotate )
# 1877 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv966)) : 'freshtv968)
        | RSHIFTS ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv971) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv969) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 551 "parser/parser_infos.mly"
             ( Dba.Binary_op.RShiftS )
# 1890 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv970)) : 'freshtv972)
        | RSHIFTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv975) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv973) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 550 "parser/parser_infos.mly"
            ( Dba.Binary_op.RShiftU)
# 1903 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv974)) : 'freshtv976)
        | XOR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv979) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv977) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_bin_op = 
# 531 "parser/parser_infos.mly"
       ( Dba.Binary_op.Xor )
# 1916 "parser/parser_infos.ml"
             in
            _menhir_goto_bin_op _menhir_env _menhir_stack _v) : 'freshtv978)) : 'freshtv980)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv981 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv982)) : 'freshtv984)
    | MenhirState88 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv991 * _menhir_state) * _menhir_state * 'tv_expr) * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv987 * _menhir_state) * _menhir_state * 'tv_expr) * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv985 * _menhir_state) * _menhir_state * 'tv_expr) * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let ((((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)), (_3 : 'tv_bin_op)), _, (_4 : 'tv_expr)) = _menhir_stack in
            let _5 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 523 "parser/parser_infos.mly"
                              ( Dba.Expr.binary _3 _2 _4 )
# 1944 "parser/parser_infos.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv986)) : 'freshtv988)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv989 * _menhir_state) * _menhir_state * 'tv_expr) * 'tv_bin_op) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv990)) : 'freshtv992)
    | MenhirState22 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv993 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | EXTU ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | HEXA _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState91 _v
        | IDENT _v ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState91 _v
        | IFJUMP ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | INT _v ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState91 _v
        | LBRACE ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | LPAR ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | STORELOAD ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState91) : 'freshtv994)
    | MenhirState91 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv997 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv995 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)), _, (_3 : 'tv_expr)) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_expr = 
# 512 "parser/parser_infos.mly"
                 ( Dba.Expr.lognot _2 )
# 1994 "parser/parser_infos.ml"
         in
        _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv996)) : 'freshtv998)
    | MenhirState21 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1045 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1041 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ARROW ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1013 * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | COMMA ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1009 * _menhir_state)) * _menhir_state * 'tv_expr))) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | INT _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv1005 * _menhir_state)) * _menhir_state * 'tv_expr)))) = Obj.magic _menhir_stack in
                        let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 2027 "parser/parser_infos.ml"
                        )) = _v in
                        ((let _menhir_stack = (_menhir_stack, _v) in
                        let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RBRACKET ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv1001 * _menhir_state)) * _menhir_state * 'tv_expr)))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2038 "parser/parser_infos.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv999 * _menhir_state)) * _menhir_state * 'tv_expr)))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2045 "parser/parser_infos.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let (((_menhir_stack, _menhir_s), _, (_3 : 'tv_expr)), (_7 : (
# 231 "parser/parser_infos.mly"
       (string)
# 2050 "parser/parser_infos.ml"
                            ))) = _menhir_stack in
                            let _8 = () in
                            let _6 = () in
                            let _5 = () in
                            let _4 = () in
                            let _2 = () in
                            let _1 = () in
                            let _v : 'tv_expr = 
# 497 "parser/parser_infos.mly"
                                                          (
   let size = int_of_string _7 |> Size.Byte.create in
   Dba.Expr.load size Machine.BigEndian _3
 )
# 2064 "parser/parser_infos.ml"
                             in
                            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1000)) : 'freshtv1002)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv1003 * _menhir_state)) * _menhir_state * 'tv_expr)))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2074 "parser/parser_infos.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1004)) : 'freshtv1006)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv1007 * _menhir_state)) * _menhir_state * 'tv_expr)))) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1008)) : 'freshtv1010)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1011 * _menhir_state)) * _menhir_state * 'tv_expr))) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1012)) : 'freshtv1014)
            | ARROWINV ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1029 * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | COMMA ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1025 * _menhir_state)) * _menhir_state * 'tv_expr))) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | INT _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv1021 * _menhir_state)) * _menhir_state * 'tv_expr)))) = Obj.magic _menhir_stack in
                        let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 2110 "parser/parser_infos.ml"
                        )) = _v in
                        ((let _menhir_stack = (_menhir_stack, _v) in
                        let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RBRACKET ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv1017 * _menhir_state)) * _menhir_state * 'tv_expr)))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2121 "parser/parser_infos.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv1015 * _menhir_state)) * _menhir_state * 'tv_expr)))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2128 "parser/parser_infos.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let (((_menhir_stack, _menhir_s), _, (_3 : 'tv_expr)), (_7 : (
# 231 "parser/parser_infos.mly"
       (string)
# 2133 "parser/parser_infos.ml"
                            ))) = _menhir_stack in
                            let _8 = () in
                            let _6 = () in
                            let _5 = () in
                            let _4 = () in
                            let _2 = () in
                            let _1 = () in
                            let _v : 'tv_expr = 
# 501 "parser/parser_infos.mly"
                                                             (
   let size = int_of_string _7 |> Size.Byte.create in
   Dba.Expr.load size Machine.LittleEndian _3
 )
# 2147 "parser/parser_infos.ml"
                             in
                            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1016)) : 'freshtv1018)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv1019 * _menhir_state)) * _menhir_state * 'tv_expr)))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2157 "parser/parser_infos.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1020)) : 'freshtv1022)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv1023 * _menhir_state)) * _menhir_state * 'tv_expr)))) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1024)) : 'freshtv1026)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1027 * _menhir_state)) * _menhir_state * 'tv_expr))) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1028)) : 'freshtv1030)
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1037 * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 2181 "parser/parser_infos.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RBRACKET ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1033 * _menhir_state)) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2192 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1031 * _menhir_state)) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2199 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _menhir_s), _, (_3 : 'tv_expr)), (_5 : (
# 231 "parser/parser_infos.mly"
       (string)
# 2204 "parser/parser_infos.ml"
                    ))) = _menhir_stack in
                    let _6 = () in
                    let _4 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : 'tv_expr = 
# 505 "parser/parser_infos.mly"
                                              (
   let size = int_of_string _5 |> Size.Byte.create in
   let endia = Kernel_options.Machine.endianness () in
   Dba.Expr.load size endia _3
 )
# 2217 "parser/parser_infos.ml"
                     in
                    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1032)) : 'freshtv1034)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1035 * _menhir_state)) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2227 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1036)) : 'freshtv1038)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1039 * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1040)) : 'freshtv1042)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1043 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1044)) : 'freshtv1046)
    | MenhirState19 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1069 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1065 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ARROW ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1049) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1047) = Obj.magic _menhir_stack in
                ((let _1 = () in
                let _v : 'tv_endianness = 
# 462 "parser/parser_infos.mly"
             ( Machine.BigEndian )
# 2267 "parser/parser_infos.ml"
                 in
                _menhir_goto_endianness _menhir_env _menhir_stack _v) : 'freshtv1048)) : 'freshtv1050)
            | ARROWINV ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1053) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1051) = Obj.magic _menhir_stack in
                ((let _1 = () in
                let _v : 'tv_endianness = 
# 463 "parser/parser_infos.mly"
             ( Machine.LittleEndian )
# 2280 "parser/parser_infos.ml"
                 in
                _menhir_goto_endianness _menhir_env _menhir_stack _v) : 'freshtv1052)) : 'freshtv1054)
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1061 * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 2289 "parser/parser_infos.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RBRACKET ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1057 * _menhir_state)) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2300 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1055 * _menhir_state)) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2307 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _menhir_s), _, (e : 'tv_expr)), (bytes : (
# 231 "parser/parser_infos.mly"
       (string)
# 2312 "parser/parser_infos.ml"
                    ))) = _menhir_stack in
                    let _7 = () in
                    let _4 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : 'tv_lhs = let endianness = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 2321 "parser/parser_infos.ml"
                     in
                    
# 451 "parser/parser_infos.mly"
                       (
   let size =  Size.Byte.of_string bytes in
   let endia =
     match endianness with
     | Some _endianness -> _endianness
     | None -> Kernel_options.Machine.endianness () in
   Dba.LValue.store size endia e
 )
# 2333 "parser/parser_infos.ml"
                     in
                    _menhir_goto_lhs _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1056)) : 'freshtv1058)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1059 * _menhir_state)) * _menhir_state * 'tv_expr)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2343 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1060)) : 'freshtv1062)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1063 * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1064)) : 'freshtv1066)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1067 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1068)) : 'freshtv1070)
    | MenhirState122 | MenhirState127 | MenhirState124 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1077 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | CONCAT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1071 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState127
            | EXTU ->
                _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState127
            | HEXA _v ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState127 _v
            | IDENT _v ->
                _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState127 _v
            | IFJUMP ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState127
            | INT _v ->
                _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState127 _v
            | LBRACE ->
                _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState127
            | LPAR ->
                _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState127
            | NOT ->
                _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState127
            | STORELOAD ->
                _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState127
            | STRING _v ->
                _menhir_run123 _menhir_env (Obj.magic _menhir_stack) MenhirState127 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState127) : 'freshtv1072)
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1073 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (_1 : 'tv_expr)) = _menhir_stack in
            let _v : 'tv_printargs = 
# 434 "parser/parser_infos.mly"
                           ( [Exp _1] )
# 2406 "parser/parser_infos.ml"
             in
            _menhir_goto_printargs _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1074)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1075 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1076)) : 'freshtv1078)
    | MenhirState204 | MenhirState199 | MenhirState173 | MenhirState145 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1081 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1079 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (e : 'tv_expr)) = _menhir_stack in
        let _v : 'tv_cond = 
# 563 "parser/parser_infos.mly"
           ( e )
# 2425 "parser/parser_infos.ml"
         in
        _menhir_goto_cond _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1080)) : 'freshtv1082)
    | MenhirState155 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1101 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1097 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ANNOT ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1091 * _menhir_state) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | CALLFLAG ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv1083 * _menhir_state) * _menhir_state * 'tv_expr))) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | HEXADDRESS _v ->
                        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState162 _v
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState162) : 'freshtv1084)
                | RETURNFLAG ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv1087 * _menhir_state) * _menhir_state * 'tv_expr))) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv1085 * _menhir_state) * _menhir_state * 'tv_expr))) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
                    let _5 = () in
                    let _4 = () in
                    let _3 = () in
                    let _1 = () in
                    let _v : 'tv_inst = 
# 400 "parser/parser_infos.mly"
                                        ( DJump (_2, Some (Dba.Return)) )
# 2472 "parser/parser_infos.ml"
                     in
                    _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1086)) : 'freshtv1088)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv1089 * _menhir_state) * _menhir_state * 'tv_expr))) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1090)) : 'freshtv1092)
            | ASSERT | ASSUME | FREE | IDENT _ | IFJUMP | LNEXT | NEXT | NONDETASSUME | PRINT | RBRACE | STOP | STORELOAD ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1093 * _menhir_state) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)) = _menhir_stack in
                let _3 = () in
                let _1 = () in
                let _v : 'tv_inst = 
# 397 "parser/parser_infos.mly"
                       ( DJump (_2, None) )
# 2491 "parser/parser_infos.ml"
                 in
                _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1094)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1095 * _menhir_state) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1096)) : 'freshtv1098)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1099 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1100)) : 'freshtv1102)
    | MenhirState194 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1113 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1109 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | SEMICOLON ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1105 * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1103 * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _, (_3 : 'tv_expr)) = _menhir_stack in
                let _5 = () in
                let _4 = () in
                let _2 = () in
                let _1 = () in
                let _v : 'tv_inst = 
# 394 "parser/parser_infos.mly"
                                 ( Free (_3, 0) )
# 2534 "parser/parser_infos.ml"
                 in
                _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1104)) : 'freshtv1106)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1107 * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1108)) : 'freshtv1110)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1111 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1112)) : 'freshtv1114)
    | MenhirState221 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv1125 * _menhir_state * 'tv_lhs)) * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv1121 * _menhir_state * 'tv_lhs)) * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | SEMICOLON ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((((('freshtv1117 * _menhir_state * 'tv_lhs)) * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((((('freshtv1115 * _menhir_state * 'tv_lhs)) * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s, (_1 : 'tv_lhs)), _), _, (_5 : 'tv_expr)) = _menhir_stack in
                let _7 = () in
                let _6 = () in
                let _4 = () in
                let _3 = () in
                let _2 = () in
                let _v : 'tv_inst = 
# 390 "parser/parser_infos.mly"
                                              (
   (* let size = Bigint.big_int_of_string $5 in *)
   Malloc (_1, _5, 0)
 )
# 2581 "parser/parser_infos.ml"
                 in
                _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1116)) : 'freshtv1118)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((((('freshtv1119 * _menhir_state * 'tv_lhs)) * _menhir_state)) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1120)) : 'freshtv1122)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv1123 * _menhir_state * 'tv_lhs)) * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1124)) : 'freshtv1126)
    | MenhirState209 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1133 * _menhir_state * 'tv_lhs)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1129 * _menhir_state * 'tv_lhs)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1127 * _menhir_state * 'tv_lhs)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_lhs)), _, (_3 : 'tv_expr)) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _v : 'tv_inst = 
# 388 "parser/parser_infos.mly"
                             ( Assign (_1, _3, 0) )
# 2616 "parser/parser_infos.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1128)) : 'freshtv1130)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1131 * _menhir_state * 'tv_lhs)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1132)) : 'freshtv1134)
    | _ ->
        _menhir_fail ()

and _menhir_goto_cond : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_cond -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState145 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv745 * _menhir_state))) * _menhir_state * 'tv_lhslist))) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((((('freshtv741 * _menhir_state))) * _menhir_state * 'tv_lhslist))) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | SEMICOLON ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((((((('freshtv737 * _menhir_state))) * _menhir_state * 'tv_lhslist))) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((((((('freshtv735 * _menhir_state))) * _menhir_state * 'tv_lhslist))) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s), _, (_4 : 'tv_lhslist)), _, (_7 : 'tv_cond)) = _menhir_stack in
                let _9 = () in
                let _8 = () in
                let _6 = () in
                let _5 = () in
                let _3 = () in
                let _2 = () in
                let _1 = () in
                let _v : 'tv_inst = 
# 417 "parser/parser_infos.mly"
                                                                     ( NondetAssume(_4,_7,0) )
# 2662 "parser/parser_infos.ml"
                 in
                _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv736)) : 'freshtv738)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((((((('freshtv739 * _menhir_state))) * _menhir_state * 'tv_lhslist))) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv740)) : 'freshtv742)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((((('freshtv743 * _menhir_state))) * _menhir_state * 'tv_lhslist))) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv744)) : 'freshtv746)
    | MenhirState173 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv791 * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | NEXT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv787 * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | HEXADDRESS _v ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState175 _v
            | IDENT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv765 * _menhir_state) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState175 in
                let (_v : (
# 232 "parser/parser_infos.mly"
       (string)
# 2700 "parser/parser_infos.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | ELSE ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv761 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 2711 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | NEXT ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv757 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 2721 "parser/parser_infos.ml"
                        ))) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | IDENT _v ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv753 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 2731 "parser/parser_infos.ml"
                            )))) = Obj.magic _menhir_stack in
                            let (_v : (
# 232 "parser/parser_infos.mly"
       (string)
# 2736 "parser/parser_infos.ml"
                            )) = _v in
                            ((let _menhir_stack = (_menhir_stack, _v) in
                            let _menhir_env = _menhir_discard _menhir_env in
                            let _tok = _menhir_env._menhir_token in
                            match _tok with
                            | SEMICOLON ->
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : (((((('freshtv749 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 2747 "parser/parser_infos.ml"
                                )))) * (
# 232 "parser/parser_infos.mly"
       (string)
# 2751 "parser/parser_infos.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let _menhir_env = _menhir_discard _menhir_env in
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : (((((('freshtv747 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 2758 "parser/parser_infos.ml"
                                )))) * (
# 232 "parser/parser_infos.mly"
       (string)
# 2762 "parser/parser_infos.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let ((((_menhir_stack, _menhir_s), _, (_2 : 'tv_cond)), _, (_4 : (
# 232 "parser/parser_infos.mly"
       (string)
# 2767 "parser/parser_infos.ml"
                                ))), (_7 : (
# 232 "parser/parser_infos.mly"
       (string)
# 2771 "parser/parser_infos.ml"
                                ))) = _menhir_stack in
                                let _8 = () in
                                let _6 = () in
                                let _5 = () in
                                let _3 = () in
                                let _1 = () in
                                let _v : 'tv_inst = 
# 409 "parser/parser_infos.mly"
                                                     (
   If (_2, (Label _4), (LabelIf _7))
 )
# 2783 "parser/parser_infos.ml"
                                 in
                                _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv748)) : 'freshtv750)
                            | _ ->
                                assert (not _menhir_env._menhir_error);
                                _menhir_env._menhir_error <- true;
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : (((((('freshtv751 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 2793 "parser/parser_infos.ml"
                                )))) * (
# 232 "parser/parser_infos.mly"
       (string)
# 2797 "parser/parser_infos.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv752)) : 'freshtv754)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv755 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 2808 "parser/parser_infos.ml"
                            )))) = Obj.magic _menhir_stack in
                            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv756)) : 'freshtv758)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv759 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 2819 "parser/parser_infos.ml"
                        ))) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv760)) : 'freshtv762)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv763 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 2830 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv764)) : 'freshtv766)
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv785 * _menhir_state) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState175 in
                let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 2841 "parser/parser_infos.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | ELSE ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv781 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 2852 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | NEXT ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv777 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 2862 "parser/parser_infos.ml"
                        ))) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | INT _v ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv773 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 2872 "parser/parser_infos.ml"
                            )))) = Obj.magic _menhir_stack in
                            let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 2877 "parser/parser_infos.ml"
                            )) = _v in
                            ((let _menhir_stack = (_menhir_stack, _v) in
                            let _menhir_env = _menhir_discard _menhir_env in
                            let _tok = _menhir_env._menhir_token in
                            match _tok with
                            | SEMICOLON ->
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : (((((('freshtv769 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 2888 "parser/parser_infos.ml"
                                )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2892 "parser/parser_infos.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let _menhir_env = _menhir_discard _menhir_env in
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : (((((('freshtv767 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 2899 "parser/parser_infos.ml"
                                )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2903 "parser/parser_infos.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let ((((_menhir_stack, _menhir_s), _, (_2 : 'tv_cond)), _, (_4 : (
# 231 "parser/parser_infos.mly"
       (string)
# 2908 "parser/parser_infos.ml"
                                ))), (_7 : (
# 231 "parser/parser_infos.mly"
       (string)
# 2912 "parser/parser_infos.ml"
                                ))) = _menhir_stack in
                                let _8 = () in
                                let _6 = () in
                                let _5 = () in
                                let _3 = () in
                                let _1 = () in
                                let _v : 'tv_inst = 
# 406 "parser/parser_infos.mly"
                                                (
   If (_2, Address(JInner (int_of_string _4)), OffsetIf(int_of_string _7))
 )
# 2924 "parser/parser_infos.ml"
                                 in
                                _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv768)) : 'freshtv770)
                            | _ ->
                                assert (not _menhir_env._menhir_error);
                                _menhir_env._menhir_error <- true;
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : (((((('freshtv771 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 2934 "parser/parser_infos.ml"
                                )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 2938 "parser/parser_infos.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv772)) : 'freshtv774)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : ((((('freshtv775 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 2949 "parser/parser_infos.ml"
                            )))) = Obj.magic _menhir_stack in
                            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv776)) : 'freshtv778)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((('freshtv779 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 2960 "parser/parser_infos.ml"
                        ))) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv780)) : 'freshtv782)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv783 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 2971 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv784)) : 'freshtv786)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState175) : 'freshtv788)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv789 * _menhir_state) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv790)) : 'freshtv792)
    | MenhirState199 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv803 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv799 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | SEMICOLON ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv795 * _menhir_state)) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv793 * _menhir_state)) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _, (_3 : 'tv_cond)) = _menhir_stack in
                let _5 = () in
                let _4 = () in
                let _2 = () in
                let _1 = () in
                let _v : 'tv_inst = 
# 416 "parser/parser_infos.mly"
                                   ( Assume (_3, 0) )
# 3012 "parser/parser_infos.ml"
                 in
                _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv794)) : 'freshtv796)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv797 * _menhir_state)) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv798)) : 'freshtv800)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv801 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv802)) : 'freshtv804)
    | MenhirState204 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv815 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv811 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | SEMICOLON ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv807 * _menhir_state)) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv805 * _menhir_state)) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _, (_3 : 'tv_cond)) = _menhir_stack in
                let _5 = () in
                let _4 = () in
                let _2 = () in
                let _1 = () in
                let _v : 'tv_inst = 
# 415 "parser/parser_infos.mly"
                                   ( Assert (_3, 0) )
# 3055 "parser/parser_infos.ml"
                 in
                _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv806)) : 'freshtv808)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv809 * _menhir_state)) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv810)) : 'freshtv812)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv813 * _menhir_state)) * _menhir_state * 'tv_cond) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv814)) : 'freshtv816)
    | _ ->
        _menhir_fail ()

and _menhir_goto_separated_nonempty_list_COMMA_INT_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_separated_nonempty_list_COMMA_INT_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState251 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv711 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3083 "parser/parser_infos.ml"
        ))) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_separated_nonempty_list_COMMA_INT_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv709 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3091 "parser/parser_infos.ml"
        ))) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_separated_nonempty_list_COMMA_INT_) : 'tv_separated_nonempty_list_COMMA_INT_) = _v in
        ((let (_menhir_stack, _menhir_s, (x : (
# 231 "parser/parser_infos.mly"
       (string)
# 3098 "parser/parser_infos.ml"
        ))) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_separated_nonempty_list_COMMA_INT_ = 
# 243 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3104 "parser/parser_infos.ml"
         in
        _menhir_goto_separated_nonempty_list_COMMA_INT_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv710)) : 'freshtv712)
    | MenhirState262 | MenhirState259 | MenhirState256 | MenhirState249 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv733) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_separated_nonempty_list_COMMA_INT_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv731) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((_1 : 'tv_separated_nonempty_list_COMMA_INT_) : 'tv_separated_nonempty_list_COMMA_INT_) = _v in
        ((let _v : 'tv_threshold_values = 
# 328 "parser/parser_infos.mly"
   ( List.map int_of_string _1 )
# 3119 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv729) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_threshold_values) = _v in
        ((match _menhir_s with
        | MenhirState249 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv715 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_threshold_values) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv713 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((_3 : 'tv_threshold_values) : 'tv_threshold_values) = _v in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_threshold_spec = 
# 324 "parser/parser_infos.mly"
     ( [], [], _3, [])
# 3141 "parser/parser_infos.ml"
             in
            _menhir_goto_threshold_spec _menhir_env _menhir_stack _menhir_s _v) : 'freshtv714)) : 'freshtv716)
        | MenhirState256 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv719 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_threshold_values) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv717 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((_3 : 'tv_threshold_values) : 'tv_threshold_values) = _v in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_threshold_spec = 
# 320 "parser/parser_infos.mly"
     ( _3, [], [], [] )
# 3159 "parser/parser_infos.ml"
             in
            _menhir_goto_threshold_spec _menhir_env _menhir_stack _menhir_s _v) : 'freshtv718)) : 'freshtv720)
        | MenhirState259 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv723 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_threshold_values) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv721 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((_3 : 'tv_threshold_values) : 'tv_threshold_values) = _v in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_threshold_spec = 
# 322 "parser/parser_infos.mly"
     ( [], [], [], _3 )
# 3177 "parser/parser_infos.ml"
             in
            _menhir_goto_threshold_spec _menhir_env _menhir_stack _menhir_s _v) : 'freshtv722)) : 'freshtv724)
        | MenhirState262 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv727 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_threshold_values) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv725 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((_3 : 'tv_threshold_values) : 'tv_threshold_values) = _v in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_threshold_spec = 
# 318 "parser/parser_infos.mly"
     ( ([], _3, [], []) )
# 3195 "parser/parser_infos.ml"
             in
            _menhir_goto_threshold_spec _menhir_env _menhir_stack _menhir_s _v) : 'freshtv726)) : 'freshtv728)
        | _ ->
            _menhir_fail ()) : 'freshtv730)) : 'freshtv732)) : 'freshtv734)
    | _ ->
        _menhir_fail ()

and _menhir_goto_lhs : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_lhs -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState153 | MenhirState133 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv667 * _menhir_state * 'tv_lhs) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv661 * _menhir_state * 'tv_lhs) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | IDENT _v ->
                _menhir_run134 _menhir_env (Obj.magic _menhir_stack) MenhirState153 _v
            | STORELOAD ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState153
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState153) : 'freshtv662)
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv663 * _menhir_state * 'tv_lhs) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (_1 : 'tv_lhs)) = _menhir_stack in
            let _v : 'tv_lhslist = 
# 423 "parser/parser_infos.mly"
       ( [_1] )
# 3234 "parser/parser_infos.ml"
             in
            _menhir_goto_lhslist _menhir_env _menhir_stack _menhir_s _v) : 'freshtv664)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv665 * _menhir_state * 'tv_lhs) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv666)) : 'freshtv668)
    | MenhirState227 | MenhirState17 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv707 * _menhir_state * 'tv_lhs) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ASSIGN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv703 * _menhir_state * 'tv_lhs) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EXTS ->
                _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState209
            | EXTU ->
                _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState209
            | HEXA _v ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState209 _v
            | IDENT _v ->
                _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState209 _v
            | IFJUMP ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState209
            | INT _v ->
                _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState209 _v
            | LBRACE ->
                _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState209
            | LPAR ->
                _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState209
            | MALLOC ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv673 * _menhir_state * 'tv_lhs)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState209 in
                ((let _menhir_stack = (_menhir_stack, _menhir_s) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAR ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv669 * _menhir_state * 'tv_lhs)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | EXTS ->
                        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState221
                    | EXTU ->
                        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState221
                    | HEXA _v ->
                        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState221 _v
                    | IDENT _v ->
                        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState221 _v
                    | IFJUMP ->
                        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState221
                    | INT _v ->
                        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState221 _v
                    | LBRACE ->
                        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState221
                    | LPAR ->
                        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState221
                    | NOT ->
                        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState221
                    | STORELOAD ->
                        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState221
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState221) : 'freshtv670)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv671 * _menhir_state * 'tv_lhs)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv672)) : 'freshtv674)
            | NONDET ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv693 * _menhir_state * 'tv_lhs)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState209 in
                ((let _menhir_stack = (_menhir_stack, _menhir_s) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAR ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv689 * _menhir_state * 'tv_lhs)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | CONSTANT ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : 'freshtv677) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : 'freshtv675) = Obj.magic _menhir_stack in
                        ((let _1 = () in
                        let _v : 'tv_regionnondet = 
# 426 "parser/parser_infos.mly"
            ( `Constant )
# 3341 "parser/parser_infos.ml"
                         in
                        _menhir_goto_regionnondet _menhir_env _menhir_stack _v) : 'freshtv676)) : 'freshtv678)
                    | MALLOC ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : 'freshtv681) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : 'freshtv679) = Obj.magic _menhir_stack in
                        ((let _1 = () in
                        let _v : 'tv_regionnondet = 
# 428 "parser/parser_infos.mly"
            ( Dba_types.Region.malloc (Kernel_options.Machine.word_size ()) )
# 3354 "parser/parser_infos.ml"
                         in
                        _menhir_goto_regionnondet _menhir_env _menhir_stack _v) : 'freshtv680)) : 'freshtv682)
                    | STACK ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : 'freshtv685) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : 'freshtv683) = Obj.magic _menhir_stack in
                        ((let _1 = () in
                        let _v : 'tv_regionnondet = 
# 427 "parser/parser_infos.mly"
            ( `Stack )
# 3367 "parser/parser_infos.ml"
                         in
                        _menhir_goto_regionnondet _menhir_env _menhir_stack _v) : 'freshtv684)) : 'freshtv686)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((('freshtv687 * _menhir_state * 'tv_lhs)) * _menhir_state)) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv688)) : 'freshtv690)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv691 * _menhir_state * 'tv_lhs)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv692)) : 'freshtv694)
            | NOT ->
                _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState209
            | STORELOAD ->
                _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState209
            | UNDEF ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv701 * _menhir_state * 'tv_lhs)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState209 in
                ((let _menhir_stack = (_menhir_stack, _menhir_s) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | SEMICOLON ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv697 * _menhir_state * 'tv_lhs)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv695 * _menhir_state * 'tv_lhs)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_lhs)), _) = _menhir_stack in
                    let _4 = () in
                    let _3 = () in
                    let _2 = () in
                    let _v : 'tv_inst = 
# 389 "parser/parser_infos.mly"
                              ( Undef (_1, 0) )
# 3409 "parser/parser_infos.ml"
                     in
                    _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv696)) : 'freshtv698)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv699 * _menhir_state * 'tv_lhs)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv700)) : 'freshtv702)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState209) : 'freshtv704)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv705 * _menhir_state * 'tv_lhs) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv706)) : 'freshtv708)
    | _ ->
        _menhir_fail ()

and _menhir_run123 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 233 "parser/parser_infos.mly"
       (string)
# 3436 "parser/parser_infos.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | CONCAT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv655 * _menhir_state * (
# 233 "parser/parser_infos.mly"
       (string)
# 3448 "parser/parser_infos.ml"
        )) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | EXTU ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | HEXA _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState124 _v
        | IDENT _v ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState124 _v
        | IFJUMP ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | INT _v ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState124 _v
        | LBRACE ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | LPAR ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | STORELOAD ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | STRING _v ->
            _menhir_run123 _menhir_env (Obj.magic _menhir_stack) MenhirState124 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState124) : 'freshtv656)
    | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv657 * _menhir_state * (
# 233 "parser/parser_infos.mly"
       (string)
# 3484 "parser/parser_infos.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (_1 : (
# 233 "parser/parser_infos.mly"
       (string)
# 3489 "parser/parser_infos.ml"
        ))) = _menhir_stack in
        let _v : 'tv_printargs = 
# 435 "parser/parser_infos.mly"
                           ( [Str _1] )
# 3494 "parser/parser_infos.ml"
         in
        _menhir_goto_printargs _menhir_env _menhir_stack _menhir_s _v) : 'freshtv658)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv659 * _menhir_state * (
# 233 "parser/parser_infos.mly"
       (string)
# 3504 "parser/parser_infos.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv660)

and _menhir_run27 : _menhir_env -> 'ttv_tail * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3512 "parser/parser_infos.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv651 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3523 "parser/parser_infos.ml"
        ))) = Obj.magic _menhir_stack in
        let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 3528 "parser/parser_infos.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SUPER ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv647 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3539 "parser/parser_infos.ml"
            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 3543 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv645 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3550 "parser/parser_infos.ml"
            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 3554 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (_1 : (
# 231 "parser/parser_infos.mly"
       (string)
# 3559 "parser/parser_infos.ml"
            ))), (_3 : (
# 231 "parser/parser_infos.mly"
       (string)
# 3563 "parser/parser_infos.ml"
            ))) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _v : 'tv_expr = 
# 467 "parser/parser_infos.mly"
                       (
   let size = int_of_string _3 in
   let bigint = (Bigint.big_int_of_string _1) in
   let bv = Bitvector.create bigint size in
   Dba.Expr.constant bv
 )
# 3575 "parser/parser_infos.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv646)) : 'freshtv648)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv649 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3585 "parser/parser_infos.ml"
            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 3589 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv650)) : 'freshtv652)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv653 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3600 "parser/parser_infos.ml"
        ))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv654)

and _menhir_run146 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv643) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_cond = 
# 561 "parser/parser_infos.mly"
           ( Dba.Expr.one )
# 3615 "parser/parser_infos.ml"
     in
    _menhir_goto_cond _menhir_env _menhir_stack _menhir_s _v) : 'freshtv644)

and _menhir_run20 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACKET ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv639 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState21
        | EXTU ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState21
        | HEXA _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState21 _v
        | IDENT _v ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState21 _v
        | IFJUMP ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState21
        | INT _v ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState21 _v
        | LBRACE ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState21
        | LPAR ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState21
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState21
        | STORELOAD ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState21
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState21) : 'freshtv640)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv641 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv642)

and _menhir_run22 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState22 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState22 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | INT _v ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState22 _v
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState22

and _menhir_run23 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | CONSTANT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv633) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState23 in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv631) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        ((let _1 = () in
        let _v : 'tv_region = 
# 556 "parser/parser_infos.mly"
            ( `Constant )
# 3712 "parser/parser_infos.ml"
         in
        _menhir_goto_region _menhir_env _menhir_stack _menhir_s _v) : 'freshtv632)) : 'freshtv634)
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState23 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState23 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | INT _v ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState23 _v
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | STACK ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv637) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState23 in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv635) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        ((let _1 = () in
        let _v : 'tv_region = 
# 557 "parser/parser_infos.mly"
            ( `Stack )
# 3745 "parser/parser_infos.ml"
         in
        _menhir_goto_region _menhir_env _menhir_stack _menhir_s _v) : 'freshtv636)) : 'freshtv638)
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState23

and _menhir_run25 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState25
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState25
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState25 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState25 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState25
    | INT _v ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState25 _v
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState25
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState25
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState25
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState25
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState25

and _menhir_run26 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 231 "parser/parser_infos.mly"
       (string)
# 3789 "parser/parser_infos.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INFER ->
        _menhir_run27 _menhir_env (Obj.magic _menhir_stack)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv629 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3805 "parser/parser_infos.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv630)

and _menhir_run30 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState30 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState30 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | INT _v ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState30 _v
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState30

and _menhir_run31 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 232 "parser/parser_infos.mly"
       (string)
# 3844 "parser/parser_infos.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv627) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 232 "parser/parser_infos.mly"
       (string)
# 3854 "parser/parser_infos.ml"
    )) : (
# 232 "parser/parser_infos.mly"
       (string)
# 3858 "parser/parser_infos.ml"
    )) = _v in
    ((let _v : 'tv_expr = 
# 495 "parser/parser_infos.mly"
         ( Dba.Expr.var _1 32 )
# 3863 "parser/parser_infos.ml"
     in
    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv628)

and _menhir_run32 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 234 "parser/parser_infos.mly"
       (string * int)
# 3870 "parser/parser_infos.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv625) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 234 "parser/parser_infos.mly"
       (string * int)
# 3880 "parser/parser_infos.ml"
    )) : (
# 234 "parser/parser_infos.mly"
       (string * int)
# 3884 "parser/parser_infos.ml"
    )) = _v in
    ((let _v : 'tv_expr = 
# 474 "parser/parser_infos.mly"
        (
   let s, size = _1 in
   let bigint = (Bigint.big_int_of_string s) in
   let bv = Bitvector.create bigint size in
   Dba.Expr.constant bv
 )
# 3894 "parser/parser_infos.ml"
     in
    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv626)

and _menhir_run147 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv623) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_cond = 
# 562 "parser/parser_infos.mly"
           ( Dba.Expr.zero )
# 3908 "parser/parser_infos.ml"
     in
    _menhir_goto_cond _menhir_env _menhir_stack _menhir_s _v) : 'freshtv624)

and _menhir_run33 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState33 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState33 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | INT _v ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState33 _v
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState33

and _menhir_run34 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState34
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState34
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState34 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState34 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState34
    | INT _v ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState34 _v
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState34
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState34
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState34
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState34
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState34

and _menhir_run250 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 231 "parser/parser_infos.mly"
       (string)
# 3977 "parser/parser_infos.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COMMA ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv617 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 3989 "parser/parser_infos.ml"
        )) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            _menhir_run250 _menhir_env (Obj.magic _menhir_stack) MenhirState251 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState251) : 'freshtv618)
    | HIGHSIGNEDTHRESHOLDS | HIGHUNSIGNEDTHRESHOLDS | LOWSIGNEDTHRESHOLDS | LOWUNSIGNEDTHRESHOLDS | RBRACE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv619 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 4005 "parser/parser_infos.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (x : (
# 231 "parser/parser_infos.mly"
       (string)
# 4010 "parser/parser_infos.ml"
        ))) = _menhir_stack in
        let _v : 'tv_separated_nonempty_list_COMMA_INT_ = 
# 241 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 4015 "parser/parser_infos.ml"
         in
        _menhir_goto_separated_nonempty_list_COMMA_INT_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv620)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv621 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 4025 "parser/parser_infos.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv622)

and _menhir_goto_nonempty_list_jump_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_jump_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState279 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv611 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_nonempty_list_jump_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv609 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((jumps : 'tv_nonempty_list_jump_) : 'tv_nonempty_list_jump_) = _v in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_jumpslist = 
# 282 "parser/parser_infos.mly"
    ( add_assoc_list_to_map Dba_types.Caddress.Map.empty Dba_types.Caddress.Map.add jumps  )
# 4048 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv607) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_jumpslist) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv605 * _menhir_state * 'tv_jumpslist) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ADDINSTRAT ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | CLOSEDJUMPS ->
            _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | ENTR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | EOF ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | GLOBALTHRESHOLDS ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | GLOBALWIDENINGDELAY ->
            _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | JUMP ->
            _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | LINEAR ->
            _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | LOCALTHRESHOLDS ->
            _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | LOCALWIDENINGDELAY ->
            _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | REPLACEINSTRAT ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | STOP ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState306
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState306) : 'freshtv606)) : 'freshtv608)) : 'freshtv610)) : 'freshtv612)
    | MenhirState281 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv615 * _menhir_state * 'tv_jump) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_nonempty_list_jump_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv613 * _menhir_state * 'tv_jump) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_nonempty_list_jump_) : 'tv_nonempty_list_jump_) = _v in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_jump)) = _menhir_stack in
        let _v : 'tv_nonempty_list_jump_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 4101 "parser/parser_infos.ml"
         in
        _menhir_goto_nonempty_list_jump_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv614)) : 'freshtv616)
    | _ ->
        _menhir_fail ()

and _menhir_goto_nonempty_list_located_instruction_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_located_instruction_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState11 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv591 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_nonempty_list_located_instruction_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv589 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((located_instrs : 'tv_nonempty_list_located_instruction_) : 'tv_nonempty_list_located_instruction_) = _v in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_replaceinstrslist = 
# 297 "parser/parser_infos.mly"
  (
    let add loc instr addrmap =
      let added_value = chain_insns loc (resolve_labels instr) in
       Dba_types.Caddress.Map.add loc added_value addrmap
    in add_assoc_list_to_map Dba_types.Caddress.Map.empty add located_instrs  )
# 4129 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv587) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_replaceinstrslist) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv585 * _menhir_state * 'tv_replaceinstrslist) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ADDINSTRAT ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | CLOSEDJUMPS ->
            _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | ENTR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | EOF ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | GLOBALTHRESHOLDS ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | GLOBALWIDENINGDELAY ->
            _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | JUMP ->
            _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | LINEAR ->
            _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | LOCALTHRESHOLDS ->
            _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | LOCALWIDENINGDELAY ->
            _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | REPLACEINSTRAT ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | STOP ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState303
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState303) : 'freshtv586)) : 'freshtv588)) : 'freshtv590)) : 'freshtv592)
    | MenhirState13 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv595 * _menhir_state * 'tv_located_instruction) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_nonempty_list_located_instruction_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv593 * _menhir_state * 'tv_located_instruction) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_nonempty_list_located_instruction_) : 'tv_nonempty_list_located_instruction_) = _v in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_located_instruction)) = _menhir_stack in
        let _v : 'tv_nonempty_list_located_instruction_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 4182 "parser/parser_infos.ml"
         in
        _menhir_goto_nonempty_list_located_instruction_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv594)) : 'freshtv596)
    | MenhirState301 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv603 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_nonempty_list_located_instruction_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv601 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((located_instrs : 'tv_nonempty_list_located_instruction_) : 'tv_nonempty_list_located_instruction_) = _v in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_addinstrslist = 
# 289 "parser/parser_infos.mly"
  ( let add loc instr addrmap =
      let added_value = resolve_labels instr in
      Dba_types.Caddress.Map.add loc added_value addrmap
    in add_assoc_list_to_map Dba_types.Caddress.Map.empty add located_instrs
  )
# 4204 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv599) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_addinstrslist) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv597 * _menhir_state * 'tv_addinstrslist) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ADDINSTRAT ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | CLOSEDJUMPS ->
            _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | ENTR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | EOF ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | GLOBALTHRESHOLDS ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | GLOBALWIDENINGDELAY ->
            _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | JUMP ->
            _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | LINEAR ->
            _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | LOCALTHRESHOLDS ->
            _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | LOCALWIDENINGDELAY ->
            _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | REPLACEINSTRAT ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | STOP ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState320
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState320) : 'freshtv598)) : 'freshtv600)) : 'freshtv602)) : 'freshtv604)
    | _ ->
        _menhir_fail ()

and _menhir_run135 : _menhir_env -> 'ttv_tail * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4250 "parser/parser_infos.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv581 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4261 "parser/parser_infos.ml"
        ))) = Obj.magic _menhir_stack in
        let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 4266 "parser/parser_infos.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SUPER ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv577 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4277 "parser/parser_infos.ml"
            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4281 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LBRACE ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv571 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4291 "parser/parser_infos.ml"
                ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4295 "parser/parser_infos.ml"
                ))) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | INT _v ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv567 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4305 "parser/parser_infos.ml"
                    ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4309 "parser/parser_infos.ml"
                    )))) = Obj.magic _menhir_stack in
                    let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 4314 "parser/parser_infos.ml"
                    )) = _v in
                    ((let _menhir_stack = (_menhir_stack, _v) in
                    let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | COMMA ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv563 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4325 "parser/parser_infos.ml"
                        ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4329 "parser/parser_infos.ml"
                        )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4333 "parser/parser_infos.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | INT _v ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv559 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4343 "parser/parser_infos.ml"
                            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4347 "parser/parser_infos.ml"
                            )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4351 "parser/parser_infos.ml"
                            ))) = Obj.magic _menhir_stack in
                            let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 4356 "parser/parser_infos.ml"
                            )) = _v in
                            ((let _menhir_stack = (_menhir_stack, _v) in
                            let _menhir_env = _menhir_discard _menhir_env in
                            let _tok = _menhir_env._menhir_token in
                            match _tok with
                            | RBRACE ->
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : ((((((('freshtv555 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4367 "parser/parser_infos.ml"
                                ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4371 "parser/parser_infos.ml"
                                )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4375 "parser/parser_infos.ml"
                                ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4379 "parser/parser_infos.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let _menhir_env = _menhir_discard _menhir_env in
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : ((((((('freshtv553 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4386 "parser/parser_infos.ml"
                                ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4390 "parser/parser_infos.ml"
                                )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4394 "parser/parser_infos.ml"
                                ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4398 "parser/parser_infos.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let ((((_menhir_stack, _menhir_s, (id : (
# 232 "parser/parser_infos.mly"
       (string)
# 4403 "parser/parser_infos.ml"
                                ))), (sz : (
# 231 "parser/parser_infos.mly"
       (string)
# 4407 "parser/parser_infos.ml"
                                ))), (lo : (
# 231 "parser/parser_infos.mly"
       (string)
# 4411 "parser/parser_infos.ml"
                                ))), (hi : (
# 231 "parser/parser_infos.mly"
       (string)
# 4415 "parser/parser_infos.ml"
                                ))) = _menhir_stack in
                                let _9 = () in
                                let _7 = () in
                                let _5 = () in
                                let _4 = () in
                                let _2 = () in
                                let _v : 'tv_lhs = 
# 443 "parser/parser_infos.mly"
                                                                     (
   let off1 = int_of_string lo in
   let off2 = int_of_string hi in
   let size = Size.Bit.of_string sz in
   Dba.LValue._restrict id size off1 off2
 )
# 4430 "parser/parser_infos.ml"
                                 in
                                _menhir_goto_lhs _menhir_env _menhir_stack _menhir_s _v) : 'freshtv554)) : 'freshtv556)
                            | _ ->
                                assert (not _menhir_env._menhir_error);
                                _menhir_env._menhir_error <- true;
                                let (_menhir_env : _menhir_env) = _menhir_env in
                                let (_menhir_stack : ((((((('freshtv557 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4440 "parser/parser_infos.ml"
                                ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4444 "parser/parser_infos.ml"
                                )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4448 "parser/parser_infos.ml"
                                ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4452 "parser/parser_infos.ml"
                                )) = Obj.magic _menhir_stack in
                                ((let ((((_menhir_stack, _menhir_s, _), _), _), _) = _menhir_stack in
                                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv558)) : 'freshtv560)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv561 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4463 "parser/parser_infos.ml"
                            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4467 "parser/parser_infos.ml"
                            )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4471 "parser/parser_infos.ml"
                            ))) = Obj.magic _menhir_stack in
                            ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv562)) : 'freshtv564)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv565 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4482 "parser/parser_infos.ml"
                        ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4486 "parser/parser_infos.ml"
                        )))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4490 "parser/parser_infos.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv566)) : 'freshtv568)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv569 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4501 "parser/parser_infos.ml"
                    ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4505 "parser/parser_infos.ml"
                    )))) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv570)) : 'freshtv572)
            | ASSIGN | COMMA | RBRACE ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv573 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4514 "parser/parser_infos.ml"
                ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4518 "parser/parser_infos.ml"
                ))) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, (id : (
# 232 "parser/parser_infos.mly"
       (string)
# 4523 "parser/parser_infos.ml"
                ))), (sz : (
# 231 "parser/parser_infos.mly"
       (string)
# 4527 "parser/parser_infos.ml"
                ))) = _menhir_stack in
                let _4 = () in
                let _2 = () in
                let _v : 'tv_lhs = 
# 440 "parser/parser_infos.mly"
 ( let bitsize = Size.Bit.of_string sz in
   Dba.LValue.var id ~bitsize
 )
# 4536 "parser/parser_infos.ml"
                 in
                _menhir_goto_lhs _menhir_env _menhir_stack _menhir_s _v) : 'freshtv574)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv575 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4546 "parser/parser_infos.ml"
                ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4550 "parser/parser_infos.ml"
                ))) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv576)) : 'freshtv578)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv579 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4561 "parser/parser_infos.ml"
            ))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 4565 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv580)) : 'freshtv582)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv583 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4576 "parser/parser_infos.ml"
        ))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv584)

and _menhir_run18 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACKET ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv549 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState19
        | EXTU ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState19
        | HEXA _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState19 _v
        | IDENT _v ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState19 _v
        | IFJUMP ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState19
        | INT _v ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState19 _v
        | LBRACE ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState19
        | LPAR ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState19
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState19
        | STORELOAD ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState19
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState19) : 'freshtv550)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv551 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv552)

and _menhir_run115 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv529 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv527 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_inst = 
# 414 "parser/parser_infos.mly"
                  ( Stop None )
# 4643 "parser/parser_infos.ml"
         in
        _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv528)) : 'freshtv530)
    | STATE_KO ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv537 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv533 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv531 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _3 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_inst = 
# 413 "parser/parser_infos.mly"
                           ( Stop (Some KO) )
# 4665 "parser/parser_infos.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv532)) : 'freshtv534)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv535 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv536)) : 'freshtv538)
    | STATE_OK ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv545 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv541 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv539 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _3 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_inst = 
# 412 "parser/parser_infos.mly"
                           ( Stop (Some OK) )
# 4694 "parser/parser_infos.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv540)) : 'freshtv542)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv543 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv544)) : 'freshtv546)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv547 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv548)

and _menhir_run122 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState122
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState122
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState122 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState122 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState122
    | INT _v ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState122 _v
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState122
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState122
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState122
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState122
    | STRING _v ->
        _menhir_run123 _menhir_env (Obj.magic _menhir_stack) MenhirState122 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState122

and _menhir_run131 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv523 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv519 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | IDENT _v ->
                _menhir_run134 _menhir_env (Obj.magic _menhir_stack) MenhirState133 _v
            | STORELOAD ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState133
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState133) : 'freshtv520)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv521 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv522)) : 'freshtv524)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv525 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv526)

and _menhir_run155 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState155
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState155
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState155 _v
    | HEXADDRESS _v ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState155 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState155 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState155
    | INT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv517 * _menhir_state) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState155 in
        let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 4811 "parser/parser_infos.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INFER ->
            _menhir_run27 _menhir_env (Obj.magic _menhir_stack)
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv513 * _menhir_state) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 4824 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv511 * _menhir_state) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 4831 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (_2 : (
# 231 "parser/parser_infos.mly"
       (string)
# 4836 "parser/parser_infos.ml"
            ))) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_inst = 
# 396 "parser/parser_infos.mly"
                      ( SJump (Address (JInner (int_of_string _2)), None) )
# 4843 "parser/parser_infos.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv512)) : 'freshtv514)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv515 * _menhir_state) * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 4853 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv516)) : 'freshtv518)
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState155
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState155
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState155
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState155
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState155

and _menhir_run170 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | IDENT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv507 * _menhir_state) = Obj.magic _menhir_stack in
        let (_v : (
# 232 "parser/parser_infos.mly"
       (string)
# 4882 "parser/parser_infos.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv503 * _menhir_state) * (
# 232 "parser/parser_infos.mly"
       (string)
# 4893 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv501 * _menhir_state) * (
# 232 "parser/parser_infos.mly"
       (string)
# 4900 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), (_2 : (
# 232 "parser/parser_infos.mly"
       (string)
# 4905 "parser/parser_infos.ml"
            ))) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_inst = 
# 402 "parser/parser_infos.mly"
                         ( SJump ((Label _2), None) )
# 4912 "parser/parser_infos.ml"
             in
            _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv502)) : 'freshtv504)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv505 * _menhir_state) * (
# 232 "parser/parser_infos.mly"
       (string)
# 4922 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv506)) : 'freshtv508)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv509 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv510)

and _menhir_run173 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXTS ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState173
    | EXTU ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState173
    | FALSE ->
        _menhir_run147 _menhir_env (Obj.magic _menhir_stack) MenhirState173
    | HEXA _v ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState173 _v
    | IDENT _v ->
        _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState173 _v
    | IFJUMP ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState173
    | INT _v ->
        _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState173 _v
    | LBRACE ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState173
    | LPAR ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState173
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState173
    | STORELOAD ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState173
    | TRUE ->
        _menhir_run146 _menhir_env (Obj.magic _menhir_stack) MenhirState173
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState173

and _menhir_run134 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 232 "parser/parser_infos.mly"
       (string)
# 4972 "parser/parser_infos.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INFER ->
        _menhir_run135 _menhir_env (Obj.magic _menhir_stack)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv499 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 4988 "parser/parser_infos.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv500)

and _menhir_run193 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv495 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState194
        | EXTU ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState194
        | HEXA _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState194 _v
        | IDENT _v ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState194 _v
        | IFJUMP ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState194
        | INT _v ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState194 _v
        | LBRACE ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState194
        | LPAR ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState194
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState194
        | STORELOAD ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState194
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState194) : 'freshtv496)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv497 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv498)

and _menhir_run198 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv491 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | EXTU ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | FALSE ->
            _menhir_run147 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | HEXA _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState199 _v
        | IDENT _v ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState199 _v
        | IFJUMP ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | INT _v ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState199 _v
        | LBRACE ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | LPAR ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | STORELOAD ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | TRUE ->
            _menhir_run146 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState199) : 'freshtv492)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv493 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv494)

and _menhir_run203 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv487 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EXTS ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState204
        | EXTU ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState204
        | FALSE ->
            _menhir_run147 _menhir_env (Obj.magic _menhir_stack) MenhirState204
        | HEXA _v ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState204 _v
        | IDENT _v ->
            _menhir_run31 _menhir_env (Obj.magic _menhir_stack) MenhirState204 _v
        | IFJUMP ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState204
        | INT _v ->
            _menhir_run26 _menhir_env (Obj.magic _menhir_stack) MenhirState204 _v
        | LBRACE ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState204
        | LPAR ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState204
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState204
        | STORELOAD ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState204
        | TRUE ->
            _menhir_run146 _menhir_env (Obj.magic _menhir_stack) MenhirState204
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState204) : 'freshtv488)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv489 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv490)

and _menhir_run248 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv483 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            _menhir_run250 _menhir_env (Obj.magic _menhir_stack) MenhirState249 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState249) : 'freshtv484)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv485 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv486)

and _menhir_run255 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv479 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            _menhir_run250 _menhir_env (Obj.magic _menhir_stack) MenhirState256 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState256) : 'freshtv480)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv481 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv482)

and _menhir_run258 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv475 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            _menhir_run250 _menhir_env (Obj.magic _menhir_stack) MenhirState259 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState259) : 'freshtv476)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv477 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv478)

and _menhir_run261 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv471 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            _menhir_run250 _menhir_env (Obj.magic _menhir_stack) MenhirState262 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState262) : 'freshtv472)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv473 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv474)

and _menhir_goto_loption_separated_nonempty_list_SEMICOLON_address__ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_loption_separated_nonempty_list_SEMICOLON_address__ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv469) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_loption_separated_nonempty_list_SEMICOLON_address__) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv467) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((xs : 'tv_loption_separated_nonempty_list_SEMICOLON_address__) : 'tv_loption_separated_nonempty_list_SEMICOLON_address__) = _v in
    ((let _v : 'tv_addresses = let addr = 
# 232 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( xs )
# 5250 "parser/parser_infos.ml"
     in
    
# 307 "parser/parser_infos.mly"
                                            ( addr )
# 5255 "parser/parser_infos.ml"
     in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv465) = _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_addresses) = _v in
    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState2 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv441 * _menhir_state)) * _menhir_state * 'tv_addresses) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv439 * _menhir_state)) * _menhir_state * 'tv_addresses) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _, (addrs : 'tv_addresses)) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_addresses_section_STOP_ = 
# 273 "parser/parser_infos.mly"
                                          ( addrs )
# 5274 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv437) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_addresses_section_STOP_) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv435 * _menhir_state * 'tv_addresses_section_STOP_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ADDINSTRAT ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | CLOSEDJUMPS ->
            _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | ENTR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | EOF ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | GLOBALTHRESHOLDS ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | GLOBALWIDENINGDELAY ->
            _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | JUMP ->
            _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | LINEAR ->
            _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | LOCALTHRESHOLDS ->
            _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | LOCALWIDENINGDELAY ->
            _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | REPLACEINSTRAT ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | STOP ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState312
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState312) : 'freshtv436)) : 'freshtv438)) : 'freshtv440)) : 'freshtv442)
    | MenhirState285 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv455 * _menhir_state * 'tv_address))) * _menhir_state * 'tv_addresses) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RBRACKET ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv451 * _menhir_state * 'tv_address))) * _menhir_state * 'tv_addresses) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv449 * _menhir_state * 'tv_address))) * _menhir_state * 'tv_addresses) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_address)), _, (_4 : 'tv_addresses)) = _menhir_stack in
            let _5 = () in
            let _3 = () in
            let _2 = () in
            let _v : 'tv_jump = 
# 285 "parser/parser_infos.mly"
                                           ( (_1, _4) )
# 5333 "parser/parser_infos.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv447) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_jump) = _v in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv445 * _menhir_state * 'tv_jump) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | HEXADDRESS _v ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState281 _v
            | ADDINSTRAT | CLOSEDJUMPS | ENTR | EOF | GLOBALTHRESHOLDS | GLOBALWIDENINGDELAY | JUMP | LINEAR | LOCALTHRESHOLDS | LOCALWIDENINGDELAY | REPLACEINSTRAT | STOP ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv443 * _menhir_state * 'tv_jump) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, (x : 'tv_jump)) = _menhir_stack in
                let _v : 'tv_nonempty_list_jump_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 5354 "parser/parser_infos.ml"
                 in
                _menhir_goto_nonempty_list_jump_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv444)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState281) : 'freshtv446)) : 'freshtv448)) : 'freshtv450)) : 'freshtv452)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv453 * _menhir_state * 'tv_address))) * _menhir_state * 'tv_addresses) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv454)) : 'freshtv456)
    | MenhirState295 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv463 * _menhir_state)) * _menhir_state * 'tv_addresses) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv461 * _menhir_state)) * _menhir_state * 'tv_addresses) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _, (addrs : 'tv_addresses)) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_addresses_section_ENTR_ = 
# 273 "parser/parser_infos.mly"
                                          ( addrs )
# 5379 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv459) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_addresses_section_ENTR_) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv457 * _menhir_state * 'tv_addresses_section_ENTR_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ADDINSTRAT ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | CLOSEDJUMPS ->
            _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | ENTR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | EOF ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | GLOBALTHRESHOLDS ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | GLOBALWIDENINGDELAY ->
            _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | JUMP ->
            _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | LINEAR ->
            _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | LOCALTHRESHOLDS ->
            _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | LOCALWIDENINGDELAY ->
            _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | REPLACEINSTRAT ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | STOP ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState314
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState314) : 'freshtv458)) : 'freshtv460)) : 'freshtv462)) : 'freshtv464)
    | _ ->
        _menhir_fail ()) : 'freshtv466)) : 'freshtv468)) : 'freshtv470)

and _menhir_goto_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState275 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv417 * _menhir_state) * _menhir_state * 'tv_address)) * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv415 * _menhir_state) * _menhir_state * 'tv_address)) * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__) : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__) = _v in
        ((let (((_menhir_stack, _menhir_s), _, (x : 'tv_address)), _, (y : 'tv_address)) = _menhir_stack in
        let _3 = () in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__ = let x =
          let x = 
# 175 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( (x, y) )
# 5442 "parser/parser_infos.ml"
           in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 5447 "parser/parser_infos.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 5453 "parser/parser_infos.ml"
         in
        _menhir_goto_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv416)) : 'freshtv418)
    | MenhirState270 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv425 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv423 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((addr_pairs : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__) : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__) = _v in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_addresses_pairs_section_LINEAR_ = 
# 278 "parser/parser_infos.mly"
   ( addr_pairs )
# 5471 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv421) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_addresses_pairs_section_LINEAR_) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv419 * _menhir_state * 'tv_addresses_pairs_section_LINEAR_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ADDINSTRAT ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | CLOSEDJUMPS ->
            _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | ENTR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | EOF ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | GLOBALTHRESHOLDS ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | GLOBALWIDENINGDELAY ->
            _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | JUMP ->
            _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | LINEAR ->
            _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | LOCALTHRESHOLDS ->
            _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | LOCALWIDENINGDELAY ->
            _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | REPLACEINSTRAT ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | STOP ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState316
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState316) : 'freshtv420)) : 'freshtv422)) : 'freshtv424)) : 'freshtv426)
    | MenhirState298 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv433 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv431 * _menhir_state)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((addr_pairs : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__) : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__) = _v in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_addresses_pairs_section_CLOSEDJUMPS_ = 
# 278 "parser/parser_infos.mly"
   ( addr_pairs )
# 5526 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv429) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_addresses_pairs_section_CLOSEDJUMPS_) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv427 * _menhir_state * 'tv_addresses_pairs_section_CLOSEDJUMPS_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ADDINSTRAT ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | CLOSEDJUMPS ->
            _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | ENTR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | EOF ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | GLOBALTHRESHOLDS ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | GLOBALWIDENINGDELAY ->
            _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | JUMP ->
            _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | LINEAR ->
            _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | LOCALTHRESHOLDS ->
            _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | LOCALWIDENINGDELAY ->
            _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | REPLACEINSTRAT ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | STOP ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState318
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState318) : 'freshtv428)) : 'freshtv430)) : 'freshtv432)) : 'freshtv434)
    | _ ->
        _menhir_fail ()

and _menhir_fail : unit -> 'a =
  fun () ->
    Printf.fprintf stderr "Internal failure -- please contact the parser generator's developers.\n%!";
    assert false

and _menhir_goto_local_delays : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_local_delays -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState231 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv409 * _menhir_state)) * _menhir_state * 'tv_local_delays) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv405 * _menhir_state)) * _menhir_state * 'tv_local_delays) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv403 * _menhir_state)) * _menhir_state * 'tv_local_delays) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (_3 : 'tv_local_delays)) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_local_widening_delay = 
# 342 "parser/parser_infos.mly"
                                                 ( _3 )
# 5597 "parser/parser_infos.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv401) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_local_widening_delay) = _v in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv399 * _menhir_state * 'tv_local_widening_delay) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ADDINSTRAT ->
                _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | CLOSEDJUMPS ->
                _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | ENTR ->
                _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | EOF ->
                _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | GLOBALTHRESHOLDS ->
                _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | GLOBALWIDENINGDELAY ->
                _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | JUMP ->
                _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | LINEAR ->
                _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | LOCALTHRESHOLDS ->
                _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | LOCALWIDENINGDELAY ->
                _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | REPLACEINSTRAT ->
                _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | STOP ->
                _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState305
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState305) : 'freshtv400)) : 'freshtv402)) : 'freshtv404)) : 'freshtv406)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv407 * _menhir_state)) * _menhir_state * 'tv_local_delays) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv408)) : 'freshtv410)
    | MenhirState237 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv413 * _menhir_state * 'tv_address)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 5649 "parser/parser_infos.ml"
        ))) * _menhir_state * 'tv_local_delays) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv411 * _menhir_state * 'tv_address)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 5655 "parser/parser_infos.ml"
        ))) * _menhir_state * 'tv_local_delays) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, (_1 : 'tv_address)), (_3 : (
# 231 "parser/parser_infos.mly"
       (string)
# 5660 "parser/parser_infos.ml"
        ))), _, (_5 : 'tv_local_delays)) = _menhir_stack in
        let _4 = () in
        let _2 = () in
        let _v : 'tv_local_delays = 
# 350 "parser/parser_infos.mly"
   ( let delay_value = int_of_string _3 in
     Dba_types.Caddress.Map.add _1 delay_value _5)
# 5668 "parser/parser_infos.ml"
         in
        _menhir_goto_local_delays _menhir_env _menhir_stack _menhir_s _v) : 'freshtv412)) : 'freshtv414)
    | _ ->
        _menhir_fail ()

and _menhir_goto_inst : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_inst -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState227 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv393 * 'tv_insts) * _menhir_state * 'tv_label) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_inst) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv391 * 'tv_insts) * _menhir_state * 'tv_label) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((_3 : 'tv_inst) : 'tv_inst) = _v in
        ((let ((_menhir_stack, (_1 : 'tv_insts)), _, (_2 : 'tv_label)) = _menhir_stack in
        let _v : 'tv_insts = 
# 369 "parser/parser_infos.mly"
                    (
   if Basic_types.String.Map.mem _2 !locallabelMap then
     let message = Format.asprintf "label locally already defined" in
     failwith message
   else (
     locallabelMap := Basic_types.String.Map.add _2 !index !locallabelMap;
     index := !index + 1;
     _1 @ [_3]
   )
 )
# 5699 "parser/parser_infos.ml"
         in
        _menhir_goto_insts _menhir_env _menhir_stack _v) : 'freshtv392)) : 'freshtv394)
    | MenhirState17 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv397 * 'tv_insts) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_inst) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv395 * 'tv_insts) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((_2 : 'tv_inst) : 'tv_inst) = _v in
        ((let (_menhir_stack, (_1 : 'tv_insts)) = _menhir_stack in
        let _v : 'tv_insts = 
# 365 "parser/parser_infos.mly"
              (
   index := !index + 1;
   _1 @ [_2]
 )
# 5718 "parser/parser_infos.ml"
         in
        _menhir_goto_insts _menhir_env _menhir_stack _v) : 'freshtv396)) : 'freshtv398)
    | _ ->
        _menhir_fail ()

and _menhir_goto_insts : _menhir_env -> 'ttv_tail -> 'tv_insts -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (('freshtv389 * _menhir_state * 'tv_address)) * 'tv_insts) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | ASSERT ->
        _menhir_run203 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | ASSUME ->
        _menhir_run198 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | FREE ->
        _menhir_run193 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | IDENT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv377) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState17 in
        let (_v : (
# 232 "parser/parser_infos.mly"
       (string)
# 5745 "parser/parser_infos.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv373 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 5756 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv371 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 5763 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (_1 : (
# 232 "parser/parser_infos.mly"
       (string)
# 5768 "parser/parser_infos.ml"
            ))) = _menhir_stack in
            let _2 = () in
            let _v : 'tv_label = 
# 385 "parser/parser_infos.mly"
               ( _1 )
# 5774 "parser/parser_infos.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv369) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_label) = _v in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv367 * 'tv_insts) * _menhir_state * 'tv_label) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ASSERT ->
                _menhir_run203 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | ASSUME ->
                _menhir_run198 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | FREE ->
                _menhir_run193 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | IDENT _v ->
                _menhir_run134 _menhir_env (Obj.magic _menhir_stack) MenhirState227 _v
            | IFJUMP ->
                _menhir_run173 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | LNEXT ->
                _menhir_run170 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | NEXT ->
                _menhir_run155 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | NONDETASSUME ->
                _menhir_run131 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | PRINT ->
                _menhir_run122 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | STOP ->
                _menhir_run115 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | STORELOAD ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState227
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState227) : 'freshtv368)) : 'freshtv370)) : 'freshtv372)) : 'freshtv374)
        | INFER ->
            _menhir_run135 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv375 * _menhir_state * (
# 232 "parser/parser_infos.mly"
       (string)
# 5821 "parser/parser_infos.ml"
            )) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv376)) : 'freshtv378)
    | IFJUMP ->
        _menhir_run173 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | LNEXT ->
        _menhir_run170 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | NEXT ->
        _menhir_run155 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | NONDETASSUME ->
        _menhir_run131 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | PRINT ->
        _menhir_run122 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | RBRACE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv387 * _menhir_state * 'tv_address)) * 'tv_insts) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState17 in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv385 * _menhir_state * 'tv_address)) * 'tv_insts) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        ((let ((_menhir_stack, _menhir_s, (loc : 'tv_address)), (instr : 'tv_insts)) = _menhir_stack in
        let _4 = () in
        let _2 = () in
        let _v : 'tv_located_instruction = 
# 304 "parser/parser_infos.mly"
                                            ( loc, instr )
# 5849 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv383) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_located_instruction) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv381 * _menhir_state * 'tv_located_instruction) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | HEXADDRESS _v ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState13 _v
        | ADDINSTRAT | CLOSEDJUMPS | ENTR | EOF | GLOBALTHRESHOLDS | GLOBALWIDENINGDELAY | JUMP | LINEAR | LOCALTHRESHOLDS | LOCALWIDENINGDELAY | REPLACEINSTRAT | STOP ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv379 * _menhir_state * 'tv_located_instruction) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (x : 'tv_located_instruction)) = _menhir_stack in
            let _v : 'tv_nonempty_list_located_instruction_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 5870 "parser/parser_infos.ml"
             in
            _menhir_goto_nonempty_list_located_instruction_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv380)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState13) : 'freshtv382)) : 'freshtv384)) : 'freshtv386)) : 'freshtv388)
    | STOP ->
        _menhir_run115 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | STORELOAD ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) MenhirState17
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState17) : 'freshtv390)

and _menhir_goto_separated_nonempty_list_SEMICOLON_address_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_separated_nonempty_list_SEMICOLON_address_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState295 | MenhirState285 | MenhirState2 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv361) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_separated_nonempty_list_SEMICOLON_address_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv359) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((x : 'tv_separated_nonempty_list_SEMICOLON_address_) : 'tv_separated_nonempty_list_SEMICOLON_address_) = _v in
        ((let _v : 'tv_loption_separated_nonempty_list_SEMICOLON_address__ = 
# 144 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 5901 "parser/parser_infos.ml"
         in
        _menhir_goto_loption_separated_nonempty_list_SEMICOLON_address__ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv360)) : 'freshtv362)
    | MenhirState8 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv365 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_separated_nonempty_list_SEMICOLON_address_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv363 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_separated_nonempty_list_SEMICOLON_address_) : 'tv_separated_nonempty_list_SEMICOLON_address_) = _v in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_address)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_separated_nonempty_list_SEMICOLON_address_ = 
# 243 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 5918 "parser/parser_infos.ml"
         in
        _menhir_goto_separated_nonempty_list_SEMICOLON_address_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv364)) : 'freshtv366)
    | _ ->
        _menhir_fail ()

and _menhir_run247 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | HIGHSIGNEDTHRESHOLDS ->
        _menhir_run261 _menhir_env (Obj.magic _menhir_stack) MenhirState247
    | HIGHUNSIGNEDTHRESHOLDS ->
        _menhir_run258 _menhir_env (Obj.magic _menhir_stack) MenhirState247
    | LOWSIGNEDTHRESHOLDS ->
        _menhir_run255 _menhir_env (Obj.magic _menhir_stack) MenhirState247
    | LOWUNSIGNEDTHRESHOLDS ->
        _menhir_run248 _menhir_env (Obj.magic _menhir_stack) MenhirState247
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState247

and _menhir_goto_infos : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_infos -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState306 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv307 * _menhir_state * 'tv_jumpslist) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv305 * _menhir_state * 'tv_jumpslist) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (jmps : 'tv_jumpslist)) = _menhir_stack in
        let _v : 'tv_infos = 
# 259 "parser/parser_infos.mly"
  ( Infos.set_jumps jmps infos )
# 5959 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv306)) : 'freshtv308)
    | MenhirState308 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv311 * _menhir_state * 'tv_global_widening_thresholds) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv309 * _menhir_state * 'tv_global_widening_thresholds) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (gwthr : 'tv_global_widening_thresholds)) = _menhir_stack in
        let _v : 'tv_infos = 
# 265 "parser/parser_infos.mly"
  ( Infos.set_global_widening_thresholds gwthr infos )
# 5975 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv310)) : 'freshtv312)
    | MenhirState310 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv315 * _menhir_state * 'tv_global_widening_delay) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv313 * _menhir_state * 'tv_global_widening_delay) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (gwdel : 'tv_global_widening_delay)) = _menhir_stack in
        let _v : 'tv_infos = 
# 267 "parser/parser_infos.mly"
  ( Infos.set_global_widening_delay gwdel infos )
# 5991 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv314)) : 'freshtv316)
    | MenhirState312 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv319 * _menhir_state * 'tv_addresses_section_STOP_) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv317 * _menhir_state * 'tv_addresses_section_STOP_) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (stops : 'tv_addresses_section_STOP_)) = _menhir_stack in
        let _v : 'tv_infos = 
# 253 "parser/parser_infos.mly"
  ( Infos.set_stops (Dba_types.Caddress.Set.of_list stops) infos )
# 6007 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv318)) : 'freshtv320)
    | MenhirState314 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv323 * _menhir_state * 'tv_addresses_section_ENTR_) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv321 * _menhir_state * 'tv_addresses_section_ENTR_) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (eps : 'tv_addresses_section_ENTR_)) = _menhir_stack in
        let _v : 'tv_infos = 
# 248 "parser/parser_infos.mly"
      ( Infos.set_entry_points
          (Virtual_address.Set.of_list
           @@ List.map Dba_types.Caddress.to_virtual_address eps)
          infos )
# 6026 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv322)) : 'freshtv324)
    | MenhirState316 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv327 * _menhir_state * 'tv_addresses_pairs_section_LINEAR_) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv325 * _menhir_state * 'tv_addresses_pairs_section_LINEAR_) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (laddrs : 'tv_addresses_pairs_section_LINEAR_)) = _menhir_stack in
        let _v : 'tv_infos = 
# 257 "parser/parser_infos.mly"
  ( Infos.set_linear_addresses laddrs infos )
# 6042 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv326)) : 'freshtv328)
    | MenhirState318 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv331 * _menhir_state * 'tv_addresses_pairs_section_CLOSEDJUMPS_) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv329 * _menhir_state * 'tv_addresses_pairs_section_CLOSEDJUMPS_) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (cjumps : 'tv_addresses_pairs_section_CLOSEDJUMPS_)) = _menhir_stack in
        let _v : 'tv_infos = 
# 255 "parser/parser_infos.mly"
  ( Infos.set_allowed_jumpzones cjumps infos )
# 6058 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv330)) : 'freshtv332)
    | MenhirState320 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv335 * _menhir_state * 'tv_addinstrslist) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv333 * _menhir_state * 'tv_addinstrslist) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (pstubs : 'tv_addinstrslist)) = _menhir_stack in
        let _v : 'tv_infos = 
# 261 "parser/parser_infos.mly"
  ( Infos.set_prepend_stubs pstubs infos )
# 6074 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv334)) : 'freshtv336)
    | MenhirState305 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv339 * _menhir_state * 'tv_local_widening_delay) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv337 * _menhir_state * 'tv_local_widening_delay) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (_1 : 'tv_local_widening_delay)) = _menhir_stack in
        let _v : 'tv_infos = 
# 269 "parser/parser_infos.mly"
                                            ( infos )
# 6090 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv338)) : 'freshtv340)
    | MenhirState304 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv343 * _menhir_state * 'tv_local_widening_thresholds) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv341 * _menhir_state * 'tv_local_widening_thresholds) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (_1 : 'tv_local_widening_thresholds)) = _menhir_stack in
        let _v : 'tv_infos = 
# 268 "parser/parser_infos.mly"
                                            ( infos )
# 6106 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv342)) : 'freshtv344)
    | MenhirState303 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv347 * _menhir_state * 'tv_replaceinstrslist) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv345 * _menhir_state * 'tv_replaceinstrslist) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((infos : 'tv_infos) : 'tv_infos) = _v in
        ((let (_menhir_stack, _menhir_s, (sstubs : 'tv_replaceinstrslist)) = _menhir_stack in
        let _v : 'tv_infos = 
# 263 "parser/parser_infos.mly"
  ( Infos.set_substitute_stubs sstubs infos )
# 6122 "parser/parser_infos.ml"
         in
        _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv346)) : 'freshtv348)
    | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv357) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_infos) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv355) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((_1 : 'tv_infos) : 'tv_infos) = _v in
        ((let _v : (
# 237 "parser/parser_infos.mly"
      (Infos.t)
# 6137 "parser/parser_infos.ml"
        ) = 
# 244 "parser/parser_infos.mly"
         ( _1 )
# 6141 "parser/parser_infos.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv353) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : (
# 237 "parser/parser_infos.mly"
      (Infos.t)
# 6149 "parser/parser_infos.ml"
        )) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv351) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : (
# 237 "parser/parser_infos.mly"
      (Infos.t)
# 6157 "parser/parser_infos.ml"
        )) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv349) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((_1 : (
# 237 "parser/parser_infos.mly"
      (Infos.t)
# 6165 "parser/parser_infos.ml"
        )) : (
# 237 "parser/parser_infos.mly"
      (Infos.t)
# 6169 "parser/parser_infos.ml"
        )) = _v in
        (Obj.magic _1 : 'freshtv350)) : 'freshtv352)) : 'freshtv354)) : 'freshtv356)) : 'freshtv358)
    | _ ->
        _menhir_fail ()

and _menhir_reduce113 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_loption_separated_nonempty_list_SEMICOLON_address__ = 
# 142 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 6180 "parser/parser_infos.ml"
     in
    _menhir_goto_loption_separated_nonempty_list_SEMICOLON_address__ _menhir_env _menhir_stack _menhir_s _v

and _menhir_reduce105 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 6189 "parser/parser_infos.ml"
     in
    _menhir_goto_list_delimited_LPAR_separated_pair_address_COMMA_address__RPAR__ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run271 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | HEXADDRESS _v ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState271 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState271

and _menhir_run3 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 235 "parser/parser_infos.mly"
       (string * int)
# 6209 "parser/parser_infos.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv303) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 235 "parser/parser_infos.mly"
       (string * int)
# 6219 "parser/parser_infos.ml"
    )) : (
# 235 "parser/parser_infos.mly"
       (string * int)
# 6223 "parser/parser_infos.ml"
    )) = _v in
    ((let _v : 'tv_address = 
# 355 "parser/parser_infos.mly"
              (
   let s, size = _1 in
   let s = String.sub s 1 (String.length s - 1) in
   let bigint = Bigint.big_int_of_string s in
   let bv = Bitvector.create bigint size in
   let vaddr = Virtual_address.of_bitvector bv in
   Dba_types.Caddress.block_start vaddr
 )
# 6235 "parser/parser_infos.ml"
     in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv301) = _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_address) = _v in
    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState295 | MenhirState285 | MenhirState8 | MenhirState2 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv199 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv193 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | HEXADDRESS _v ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState8 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState8) : 'freshtv194)
        | ADDINSTRAT | CLOSEDJUMPS | ENTR | EOF | GLOBALTHRESHOLDS | GLOBALWIDENINGDELAY | JUMP | LINEAR | LOCALTHRESHOLDS | LOCALWIDENINGDELAY | RBRACKET | REPLACEINSTRAT | STOP ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv195 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (x : 'tv_address)) = _menhir_stack in
            let _v : 'tv_separated_nonempty_list_SEMICOLON_address_ = 
# 241 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 6268 "parser/parser_infos.ml"
             in
            _menhir_goto_separated_nonempty_list_SEMICOLON_address_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv196)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv197 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv198)) : 'freshtv200)
    | MenhirState301 | MenhirState11 | MenhirState13 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv207 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv203 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv201) = Obj.magic _menhir_stack in
            ((let _v : 'tv_insts = 
# 379 "parser/parser_infos.mly"
   (  reset_label_map;
      index := 0;
      []
   )
# 6296 "parser/parser_infos.ml"
             in
            _menhir_goto_insts _menhir_env _menhir_stack _v) : 'freshtv202)) : 'freshtv204)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv205 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv206)) : 'freshtv208)
    | MenhirState162 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv211 * _menhir_state) * _menhir_state * 'tv_expr)))) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv209 * _menhir_state) * _menhir_state * 'tv_expr)))) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s), _, (_2 : 'tv_expr)), _, (_6 : 'tv_address)) = _menhir_stack in
        let _5 = () in
        let _4 = () in
        let _3 = () in
        let _1 = () in
        let _v : 'tv_inst = 
# 401 "parser/parser_infos.mly"
                                              ( DJump (_2, Some (Dba.Call _6)) )
# 6319 "parser/parser_infos.ml"
         in
        _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv210)) : 'freshtv212)
    | MenhirState155 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv231 * _menhir_state) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv227 * _menhir_state) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ANNOT ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv221 * _menhir_state) * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | CALLFLAG ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv213 * _menhir_state) * _menhir_state * 'tv_address))) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | HEXADDRESS _v ->
                        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState168 _v
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState168) : 'freshtv214)
                | RETURNFLAG ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv217 * _menhir_state) * _menhir_state * 'tv_address))) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv215 * _menhir_state) * _menhir_state * 'tv_address))) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_address)) = _menhir_stack in
                    let _5 = () in
                    let _4 = () in
                    let _3 = () in
                    let _1 = () in
                    let _v : 'tv_inst = 
# 399 "parser/parser_infos.mly"
                                           ( SJump (Address (JOuter _2), Some (Dba.Return)) )
# 6366 "parser/parser_infos.ml"
                     in
                    _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv216)) : 'freshtv218)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv219 * _menhir_state) * _menhir_state * 'tv_address))) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv220)) : 'freshtv222)
            | ASSERT | ASSUME | FREE | IDENT _ | IFJUMP | LNEXT | NEXT | NONDETASSUME | PRINT | RBRACE | STOP | STORELOAD ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv223 * _menhir_state) * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _, (_2 : 'tv_address)) = _menhir_stack in
                let _3 = () in
                let _1 = () in
                let _v : 'tv_inst = 
# 395 "parser/parser_infos.mly"
                          ( SJump (Address (JOuter _2), None) )
# 6385 "parser/parser_infos.ml"
                 in
                _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv224)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv225 * _menhir_state) * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv226)) : 'freshtv228)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv229 * _menhir_state) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv230)) : 'freshtv232)
    | MenhirState168 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv235 * _menhir_state) * _menhir_state * 'tv_address)))) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv233 * _menhir_state) * _menhir_state * 'tv_address)))) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s), _, (_2 : 'tv_address)), _, (_6 : 'tv_address)) = _menhir_stack in
        let _5 = () in
        let _4 = () in
        let _3 = () in
        let _1 = () in
        let _v : 'tv_inst = 
# 398 "parser/parser_infos.mly"
                                                 ( SJump (Address (JOuter _2), Some (Dba.Call _6)) )
# 6415 "parser/parser_infos.ml"
         in
        _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv234)) : 'freshtv236)
    | MenhirState175 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv255 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ELSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv251 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | NEXT ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv247 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | INT _v ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv243 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address))) = Obj.magic _menhir_stack in
                    let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 6442 "parser/parser_infos.ml"
                    )) = _v in
                    ((let _menhir_stack = (_menhir_stack, _v) in
                    let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | SEMICOLON ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((((('freshtv239 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 6453 "parser/parser_infos.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((((('freshtv237 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 6460 "parser/parser_infos.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let ((((_menhir_stack, _menhir_s), _, (_2 : 'tv_cond)), _, (_4 : 'tv_address)), (_7 : (
# 231 "parser/parser_infos.mly"
       (string)
# 6465 "parser/parser_infos.ml"
                        ))) = _menhir_stack in
                        let _8 = () in
                        let _6 = () in
                        let _5 = () in
                        let _3 = () in
                        let _1 = () in
                        let _v : 'tv_inst = 
# 403 "parser/parser_infos.mly"
                                                    (
   If (_2, Address(JOuter _4), OffsetIf(int_of_string _7))
 )
# 6477 "parser/parser_infos.ml"
                         in
                        _menhir_goto_inst _menhir_env _menhir_stack _menhir_s _v) : 'freshtv238)) : 'freshtv240)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : (((((('freshtv241 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address))) * (
# 231 "parser/parser_infos.mly"
       (string)
# 6487 "parser/parser_infos.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv242)) : 'freshtv244)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv245 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address))) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv246)) : 'freshtv248)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv249 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv250)) : 'freshtv252)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv253 * _menhir_state) * _menhir_state * 'tv_cond)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv254)) : 'freshtv256)
    | MenhirState237 | MenhirState231 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv271 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv267 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv263 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 6530 "parser/parser_infos.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | SEMICOLON ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv257 * _menhir_state * 'tv_address)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 6541 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | HEXADDRESS _v ->
                        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState237 _v
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState237) : 'freshtv258)
                | RBRACE ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv259 * _menhir_state * 'tv_address)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 6557 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s, (_1 : 'tv_address)), (_3 : (
# 231 "parser/parser_infos.mly"
       (string)
# 6562 "parser/parser_infos.ml"
                    ))) = _menhir_stack in
                    let _2 = () in
                    let _v : 'tv_local_delays = 
# 347 "parser/parser_infos.mly"
   ( let delay_value = int_of_string _3 in
     Dba_types.Caddress.Map.add _1 delay_value Dba_types.Caddress.Map.empty )
# 6569 "parser/parser_infos.ml"
                     in
                    _menhir_goto_local_delays _menhir_env _menhir_stack _menhir_s _v) : 'freshtv260)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (('freshtv261 * _menhir_state * 'tv_address)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 6579 "parser/parser_infos.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv262)) : 'freshtv264)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv265 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv266)) : 'freshtv268)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv269 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv270)) : 'freshtv272)
    | MenhirState240 | MenhirState243 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv277 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv273 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LBRACE ->
                _menhir_run247 _menhir_env (Obj.magic _menhir_stack) MenhirState246
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState246) : 'freshtv274)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv275 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv276)) : 'freshtv278)
    | MenhirState271 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv283 * _menhir_state) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv279 * _menhir_state) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | HEXADDRESS _v ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState273 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState273) : 'freshtv280)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv281 * _menhir_state) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv282)) : 'freshtv284)
    | MenhirState273 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv289 * _menhir_state) * _menhir_state * 'tv_address)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv285 * _menhir_state) * _menhir_state * 'tv_address)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAR ->
                _menhir_run271 _menhir_env (Obj.magic _menhir_stack) MenhirState275
            | ADDINSTRAT | CLOSEDJUMPS | ENTR | EOF | GLOBALTHRESHOLDS | GLOBALWIDENINGDELAY | JUMP | LINEAR | LOCALTHRESHOLDS | LOCALWIDENINGDELAY | REPLACEINSTRAT | STOP ->
                _menhir_reduce105 _menhir_env (Obj.magic _menhir_stack) MenhirState275
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState275) : 'freshtv286)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv287 * _menhir_state) * _menhir_state * 'tv_address)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv288)) : 'freshtv290)
    | MenhirState279 | MenhirState281 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv299 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | NEXT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv295 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LBRACKET ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv291 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | HEXADDRESS _v ->
                    _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState285 _v
                | RBRACKET ->
                    _menhir_reduce113 _menhir_env (Obj.magic _menhir_stack) MenhirState285
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState285) : 'freshtv292)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv293 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv294)) : 'freshtv296)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv297 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv298)) : 'freshtv300)
    | _ ->
        _menhir_fail ()) : 'freshtv302)) : 'freshtv304)

and _menhir_errorcase : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    match _menhir_s with
    | MenhirState320 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv55 * _menhir_state * 'tv_addinstrslist) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv56)
    | MenhirState318 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv57 * _menhir_state * 'tv_addresses_pairs_section_CLOSEDJUMPS_) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv58)
    | MenhirState316 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv59 * _menhir_state * 'tv_addresses_pairs_section_LINEAR_) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv60)
    | MenhirState314 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv61 * _menhir_state * 'tv_addresses_section_ENTR_) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv62)
    | MenhirState312 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv63 * _menhir_state * 'tv_addresses_section_STOP_) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv64)
    | MenhirState310 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv65 * _menhir_state * 'tv_global_widening_delay) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv66)
    | MenhirState308 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv67 * _menhir_state * 'tv_global_widening_thresholds) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv68)
    | MenhirState306 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv69 * _menhir_state * 'tv_jumpslist) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv70)
    | MenhirState305 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv71 * _menhir_state * 'tv_local_widening_delay) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv72)
    | MenhirState304 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv73 * _menhir_state * 'tv_local_widening_thresholds) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv74)
    | MenhirState303 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv75 * _menhir_state * 'tv_replaceinstrslist) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv76)
    | MenhirState301 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv77 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv78)
    | MenhirState298 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv79 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv80)
    | MenhirState295 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv81 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv82)
    | MenhirState291 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv83 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv84)
    | MenhirState285 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv85 * _menhir_state * 'tv_address))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv86)
    | MenhirState281 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv87 * _menhir_state * 'tv_jump) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv88)
    | MenhirState279 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv89 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv90)
    | MenhirState275 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv91 * _menhir_state) * _menhir_state * 'tv_address)) * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv92)
    | MenhirState273 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv93 * _menhir_state) * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv94)
    | MenhirState271 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv95 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv96)
    | MenhirState270 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv97 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv98)
    | MenhirState264 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv99 * _menhir_state * 'tv_threshold_spec) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv100)
    | MenhirState262 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv101 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv102)
    | MenhirState259 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv103 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv104)
    | MenhirState256 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv105 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv106)
    | MenhirState251 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv107 * _menhir_state * (
# 231 "parser/parser_infos.mly"
       (string)
# 6855 "parser/parser_infos.ml"
        ))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv108)
    | MenhirState249 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv109 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv110)
    | MenhirState247 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv111 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv112)
    | MenhirState246 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv113 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv114)
    | MenhirState243 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv115 * _menhir_state * 'tv_located_threshold) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv116)
    | MenhirState240 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv117 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv118)
    | MenhirState237 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv119 * _menhir_state * 'tv_address)) * (
# 231 "parser/parser_infos.mly"
       (string)
# 6889 "parser/parser_infos.ml"
        ))) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv120)
    | MenhirState231 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv121 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv122)
    | MenhirState227 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv123 * 'tv_insts) * _menhir_state * 'tv_label) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv124)
    | MenhirState221 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv125 * _menhir_state * 'tv_lhs)) * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv126)
    | MenhirState209 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv127 * _menhir_state * 'tv_lhs)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv128)
    | MenhirState204 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv129 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv130)
    | MenhirState199 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv131 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv132)
    | MenhirState194 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv133 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv134)
    | MenhirState175 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv135 * _menhir_state) * _menhir_state * 'tv_cond)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv136)
    | MenhirState173 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv137 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv138)
    | MenhirState168 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv139 * _menhir_state) * _menhir_state * 'tv_address)))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv140)
    | MenhirState162 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv141 * _menhir_state) * _menhir_state * 'tv_expr)))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv142)
    | MenhirState155 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv143 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv144)
    | MenhirState153 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv145 * _menhir_state * 'tv_lhs)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv146)
    | MenhirState145 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv147 * _menhir_state))) * _menhir_state * 'tv_lhslist))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv148)
    | MenhirState133 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv149 * _menhir_state))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv150)
    | MenhirState127 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv151 * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv152)
    | MenhirState124 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv153 * _menhir_state * (
# 233 "parser/parser_infos.mly"
       (string)
# 6978 "parser/parser_infos.ml"
        ))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv154)
    | MenhirState122 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv155 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv156)
    | MenhirState91 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv157 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv158)
    | MenhirState88 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv159 * _menhir_state) * _menhir_state * 'tv_expr) * 'tv_bin_op) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv160)
    | MenhirState41 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv161 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv162)
    | MenhirState39 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv163 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv164)
    | MenhirState34 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv165 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv166)
    | MenhirState33 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv167 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv168)
    | MenhirState30 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv169 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv170)
    | MenhirState25 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv171 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv172)
    | MenhirState23 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv173 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv174)
    | MenhirState22 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv175 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv176)
    | MenhirState21 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv177 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv178)
    | MenhirState19 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv179 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv180)
    | MenhirState17 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv181 * _menhir_state * 'tv_address)) * 'tv_insts) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv182)
    | MenhirState13 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv183 * _menhir_state * 'tv_located_instruction) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv184)
    | MenhirState11 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv185 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv186)
    | MenhirState8 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv187 * _menhir_state * 'tv_address)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv188)
    | MenhirState2 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv189 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv190)
    | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv191) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv192)

and _menhir_run1 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv51 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | HEXADDRESS _v ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState2 _v
        | ADDINSTRAT | CLOSEDJUMPS | ENTR | EOF | GLOBALTHRESHOLDS | GLOBALWIDENINGDELAY | JUMP | LINEAR | LOCALTHRESHOLDS | LOCALWIDENINGDELAY | REPLACEINSTRAT | STOP ->
            _menhir_reduce113 _menhir_env (Obj.magic _menhir_stack) MenhirState2
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState2) : 'freshtv52)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv53 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv54)

and _menhir_run10 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv47 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | HEXADDRESS _v ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState11 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState11) : 'freshtv48)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv49 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv50)

and _menhir_run230 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv43 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | HEXADDRESS _v ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState231 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState231) : 'freshtv44)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv45 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv46)

and _menhir_run239 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv39 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | HEXADDRESS _v ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState240 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState240) : 'freshtv40)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv41 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv42)

and _menhir_run269 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv35 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run271 _menhir_env (Obj.magic _menhir_stack) MenhirState270
        | ADDINSTRAT | CLOSEDJUMPS | ENTR | EOF | GLOBALTHRESHOLDS | GLOBALWIDENINGDELAY | JUMP | LINEAR | LOCALTHRESHOLDS | LOCALWIDENINGDELAY | REPLACEINSTRAT | STOP ->
            _menhir_reduce105 _menhir_env (Obj.magic _menhir_stack) MenhirState270
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState270) : 'freshtv36)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv37 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv38)

and _menhir_run278 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv31 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | HEXADDRESS _v ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState279 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState279) : 'freshtv32)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv33 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv34)

and _menhir_run288 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv27 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv23 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_v : (
# 231 "parser/parser_infos.mly"
       (string)
# 7255 "parser/parser_infos.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv21 * _menhir_state)) = Obj.magic _menhir_stack in
            let ((_3 : (
# 231 "parser/parser_infos.mly"
       (string)
# 7263 "parser/parser_infos.ml"
            )) : (
# 231 "parser/parser_infos.mly"
       (string)
# 7267 "parser/parser_infos.ml"
            )) = _v in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_global_widening_delay = 
# 339 "parser/parser_infos.mly"
                                 ( int_of_string _3 )
# 7275 "parser/parser_infos.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv19) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_global_widening_delay) = _v in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv17 * _menhir_state * 'tv_global_widening_delay) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ADDINSTRAT ->
                _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | CLOSEDJUMPS ->
                _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | ENTR ->
                _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | EOF ->
                _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | GLOBALTHRESHOLDS ->
                _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | GLOBALWIDENINGDELAY ->
                _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | JUMP ->
                _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | LINEAR ->
                _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | LOCALTHRESHOLDS ->
                _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | LOCALWIDENINGDELAY ->
                _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | REPLACEINSTRAT ->
                _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | STOP ->
                _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState310
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState310) : 'freshtv18)) : 'freshtv20)) : 'freshtv22)) : 'freshtv24)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv25 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv26)) : 'freshtv28)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv29 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv30)

and _menhir_run291 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACE ->
        _menhir_run247 _menhir_env (Obj.magic _menhir_stack) MenhirState291
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState291

and _menhir_run293 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv15) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_infos = 
# 270 "parser/parser_infos.mly"
                                            ( Infos.default )
# 7352 "parser/parser_infos.ml"
     in
    _menhir_goto_infos _menhir_env _menhir_stack _menhir_s _v) : 'freshtv16)

and _menhir_run294 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv11 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | HEXADDRESS _v ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState295 _v
        | ADDINSTRAT | CLOSEDJUMPS | ENTR | EOF | GLOBALTHRESHOLDS | GLOBALWIDENINGDELAY | JUMP | LINEAR | LOCALTHRESHOLDS | LOCALWIDENINGDELAY | REPLACEINSTRAT | STOP ->
            _menhir_reduce113 _menhir_env (Obj.magic _menhir_stack) MenhirState295
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState295) : 'freshtv12)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv13 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv14)

and _menhir_run297 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv7 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run271 _menhir_env (Obj.magic _menhir_stack) MenhirState298
        | ADDINSTRAT | CLOSEDJUMPS | ENTR | EOF | GLOBALTHRESHOLDS | GLOBALWIDENINGDELAY | JUMP | LINEAR | LOCALTHRESHOLDS | LOCALWIDENINGDELAY | REPLACEINSTRAT | STOP ->
            _menhir_reduce105 _menhir_env (Obj.magic _menhir_stack) MenhirState298
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState298) : 'freshtv8)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv9 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv10)

and _menhir_run300 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv3 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | HEXADDRESS _v ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState301 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState301) : 'freshtv4)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv5 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv6)

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

and configuration : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 237 "parser/parser_infos.mly"
      (Infos.t)
# 7453 "parser/parser_infos.ml"
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
    let (_menhir_stack : 'freshtv1) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | ADDINSTRAT ->
        _menhir_run300 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | CLOSEDJUMPS ->
        _menhir_run297 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | ENTR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | EOF ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | GLOBALTHRESHOLDS ->
        _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | GLOBALWIDENINGDELAY ->
        _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | JUMP ->
        _menhir_run278 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | LINEAR ->
        _menhir_run269 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | LOCALTHRESHOLDS ->
        _menhir_run239 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | LOCALWIDENINGDELAY ->
        _menhir_run230 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | REPLACEINSTRAT ->
        _menhir_run10 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | STOP ->
        _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState0) : 'freshtv2))

# 269 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
  

# 7504 "parser/parser_infos.ml"
