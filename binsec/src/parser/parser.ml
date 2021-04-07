
module MenhirBasics = struct
  
  exception Error = Parsing.Parse_error
  
  type token = 
    | XOR
    | WRITE
    | WORDSIZE
    | VAR
    | UNDEF
    | UNCONTROLLED
    | TRUE
    | THEN
    | TEMPTAG
    | TEMPORARY
    | SUPER
    | STRING of (
# 65 "parser/parser.mly"
       (string)
# 22 "parser/parser.ml"
  )
    | STOP
    | STAR_U
    | STAR_S
    | STAR
    | STACK
    | SOK
    | SLASH_U
    | SLASH_S
    | SKO
    | SEMICOLON
    | RSHIFTU
    | RSHIFTS
    | RROTATE
    | RPAR
    | RETURNFLAG
    | REGISTER
    | READ
    | REACH
    | RBRACKETU
    | RBRACKETS
    | RBRACKET
    | RBRACE
    | PRINT
    | PLUS
    | PERMISSIONS
    | OR
    | NWRITE
    | NREAD
    | NOT
    | NONDETASSUME
    | NONDET
    | NEXEC
    | NEQ
    | MODU
    | MODS
    | MINUS
    | MALLOC
    | LTU
    | LTS
    | LSHIFT
    | LROTATE
    | LPAR
    | LITTLE
    | LEU
    | LES
    | LBRACKET
    | LBRACE
    | INT of (
# 61 "parser/parser.mly"
       (string)
# 74 "parser/parser.ml"
  )
    | INFER
    | IF
    | IDENT of (
# 62 "parser/parser.mly"
       (string)
# 81 "parser/parser.ml"
  )
    | HEXA of (
# 63 "parser/parser.mly"
       (string)
# 86 "parser/parser.ml"
  )
    | GTU
    | GTS
    | GOTO
    | GEU
    | GES
    | FROMFILE
    | FROM
    | FREE
    | FLAGTAG
    | FLAG
    | FILE
    | FALSE
    | EXTU
    | EXTS
    | EXEC
    | EQUAL
    | EOF
    | ENUMERATE
    | ENTRYPOINT
    | ENDIANNESS
    | END
    | ELSE
    | DOTDOT
    | DOT
    | CUT
    | CONSTANT
    | CONSEQUENT
    | CONCAT
    | COMMA
    | COLON
    | CALLFLAG
    | BIN of (
# 64 "parser/parser.mly"
       (string)
# 122 "parser/parser.ml"
  )
    | BIG
    | BEGIN
    | AT
    | ASSUME
    | ASSIGN
    | ASSERT
    | AS
    | ARROWINV
    | ARROW
    | ANNOT
    | AND
    | ALTERNATIVE
    | ALTERNATE
  
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
  | MenhirState555
  | MenhirState550
  | MenhirState547
  | MenhirState544
  | MenhirState543
  | MenhirState539
  | MenhirState526
  | MenhirState525
  | MenhirState522
  | MenhirState519
  | MenhirState518
  | MenhirState515
  | MenhirState514
  | MenhirState513
  | MenhirState511
  | MenhirState509
  | MenhirState504
  | MenhirState502
  | MenhirState500
  | MenhirState496
  | MenhirState495
  | MenhirState494
  | MenhirState492
  | MenhirState489
  | MenhirState486
  | MenhirState485
  | MenhirState484
  | MenhirState482
  | MenhirState480
  | MenhirState477
  | MenhirState472
  | MenhirState468
  | MenhirState463
  | MenhirState459
  | MenhirState453
  | MenhirState451
  | MenhirState446
  | MenhirState444
  | MenhirState439
  | MenhirState436
  | MenhirState434
  | MenhirState432
  | MenhirState429
  | MenhirState428
  | MenhirState427
  | MenhirState426
  | MenhirState421
  | MenhirState418
  | MenhirState417
  | MenhirState416
  | MenhirState409
  | MenhirState406
  | MenhirState404
  | MenhirState401
  | MenhirState398
  | MenhirState397
  | MenhirState391
  | MenhirState389
  | MenhirState386
  | MenhirState370
  | MenhirState360
  | MenhirState358
  | MenhirState356
  | MenhirState354
  | MenhirState352
  | MenhirState343
  | MenhirState341
  | MenhirState339
  | MenhirState337
  | MenhirState335
  | MenhirState333
  | MenhirState331
  | MenhirState329
  | MenhirState327
  | MenhirState325
  | MenhirState323
  | MenhirState321
  | MenhirState319
  | MenhirState317
  | MenhirState315
  | MenhirState313
  | MenhirState311
  | MenhirState309
  | MenhirState307
  | MenhirState305
  | MenhirState303
  | MenhirState300
  | MenhirState298
  | MenhirState296
  | MenhirState295
  | MenhirState294
  | MenhirState293
  | MenhirState292
  | MenhirState289
  | MenhirState287
  | MenhirState265
  | MenhirState261
  | MenhirState248
  | MenhirState245
  | MenhirState244
  | MenhirState232
  | MenhirState229
  | MenhirState224
  | MenhirState221
  | MenhirState216
  | MenhirState213
  | MenhirState212
  | MenhirState208
  | MenhirState203
  | MenhirState200
  | MenhirState199
  | MenhirState197
  | MenhirState196
  | MenhirState191
  | MenhirState190
  | MenhirState185
  | MenhirState177
  | MenhirState175
  | MenhirState172
  | MenhirState167
  | MenhirState161
  | MenhirState147
  | MenhirState146
  | MenhirState136
  | MenhirState135
  | MenhirState132
  | MenhirState130
  | MenhirState126
  | MenhirState124
  | MenhirState121
  | MenhirState119
  | MenhirState118
  | MenhirState117
  | MenhirState116
  | MenhirState115
  | MenhirState113
  | MenhirState111
  | MenhirState99
  | MenhirState98
  | MenhirState97
  | MenhirState96
  | MenhirState95
  | MenhirState94
  | MenhirState93
  | MenhirState92
  | MenhirState91
  | MenhirState90
  | MenhirState83
  | MenhirState82
  | MenhirState81
  | MenhirState80
  | MenhirState79
  | MenhirState78
  | MenhirState77
  | MenhirState76
  | MenhirState75
  | MenhirState74
  | MenhirState73
  | MenhirState72
  | MenhirState71
  | MenhirState70
  | MenhirState69
  | MenhirState68
  | MenhirState67
  | MenhirState66
  | MenhirState65
  | MenhirState64
  | MenhirState63
  | MenhirState62
  | MenhirState61
  | MenhirState60
  | MenhirState59
  | MenhirState58
  | MenhirState57
  | MenhirState56
  | MenhirState55
  | MenhirState54
  | MenhirState53
  | MenhirState52
  | MenhirState51
  | MenhirState50
  | MenhirState49
  | MenhirState48
  | MenhirState47
  | MenhirState46
  | MenhirState45
  | MenhirState44
  | MenhirState43
  | MenhirState42
  | MenhirState39
  | MenhirState38
  | MenhirState37
  | MenhirState36
  | MenhirState35
  | MenhirState33
  | MenhirState32
  | MenhirState30
  | MenhirState29
  | MenhirState26
  | MenhirState24
  | MenhirState23
  | MenhirState22
  | MenhirState20
  | MenhirState15
  | MenhirState13
  | MenhirState2
  | MenhirState1
  | MenhirState0

# 22 "parser/parser.mly"
  
  open Dba
  open Parse_helpers

  let unknown_successor = -1

  let default_endianness = Utils.get_opt_or_default Machine.LittleEndian

  let mk_declaration (tag:VarTag.t) name size =
     Declarations.add name size tag;
     let bitsize = Size.Bit.create size in
     Dba.LValue.var name ~bitsize ~tag


  let _dummy_addr = Virtual_address.create 0


# 380 "parser/parser.ml"

let rec _menhir_goto_permission : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_permission -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2203 * _menhir_state * 'tv_permission) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | CONSTANT ->
        _menhir_run388 _menhir_env (Obj.magic _menhir_stack) MenhirState391
    | MALLOC ->
        _menhir_run385 _menhir_env (Obj.magic _menhir_stack) MenhirState391
    | STACK ->
        _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState391
    | END ->
        _menhir_reduce196 _menhir_env (Obj.magic _menhir_stack) MenhirState391
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState391) : 'freshtv2204)

and _menhir_goto_nonempty_list_predicate_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_predicate_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState370 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2183 * _menhir_state * 'tv_predicate) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_nonempty_list_predicate_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2181 * _menhir_state * 'tv_predicate) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_nonempty_list_predicate_) : 'tv_nonempty_list_predicate_) = _v in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_predicate)) = _menhir_stack in
        let _v : 'tv_nonempty_list_predicate_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 419 "parser/parser.ml"
         in
        _menhir_goto_nonempty_list_predicate_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2182)) : 'freshtv2184)
    | MenhirState389 | MenhirState386 | MenhirState289 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2201) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_nonempty_list_predicate_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2199) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((predicates : 'tv_nonempty_list_predicate_) : 'tv_nonempty_list_predicate_) = _v in
        ((let _v : 'tv_predicates = 
# 206 "parser/parser.mly"
                                        ( Mk.Predicates.of_list predicates )
# 434 "parser/parser.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2197) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_predicates) = _v in
        ((match _menhir_s with
        | MenhirState289 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv2187 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_predicates) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv2185 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((preds : 'tv_predicates) : 'tv_predicates) = _v in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_permission = let region =
              let region = 
# 261 "parser/parser.mly"
            ( `Stack )
# 457 "parser/parser.ml"
               in
              
# 265 "parser/parser.mly"
                  ( region )
# 462 "parser/parser.ml"
              
            in
            
# 202 "parser/parser.mly"
   ( region, fst preds, snd preds )
# 468 "parser/parser.ml"
             in
            _menhir_goto_permission _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2186)) : 'freshtv2188)
        | MenhirState386 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv2191 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_predicates) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv2189 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((preds : 'tv_predicates) : 'tv_predicates) = _v in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_permission = let region = 
# 266 "parser/parser.mly"
                  ( Dba_types.Region.malloc (Kernel_options.Machine.word_size ()))
# 486 "parser/parser.ml"
             in
            
# 202 "parser/parser.mly"
   ( region, fst preds, snd preds )
# 491 "parser/parser.ml"
             in
            _menhir_goto_permission _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2190)) : 'freshtv2192)
        | MenhirState389 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv2195 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_predicates) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv2193 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((preds : 'tv_predicates) : 'tv_predicates) = _v in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_permission = let region =
              let region = 
# 260 "parser/parser.mly"
            ( `Constant )
# 510 "parser/parser.ml"
               in
              
# 265 "parser/parser.mly"
                  ( region )
# 515 "parser/parser.ml"
              
            in
            
# 202 "parser/parser.mly"
   ( region, fst preds, snd preds )
# 521 "parser/parser.ml"
             in
            _menhir_goto_permission _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2194)) : 'freshtv2196)
        | _ ->
            _menhir_fail ()) : 'freshtv2198)) : 'freshtv2200)) : 'freshtv2202)
    | _ ->
        _menhir_fail ()

and _menhir_goto_exec_permission : _menhir_env -> 'ttv_tail -> 'tv_exec_permission -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ((('freshtv2179 * _menhir_state * 'tv_cond_addr)) * 'tv_read_permission) * 'tv_write_permission) = Obj.magic _menhir_stack in
    let (_v : 'tv_exec_permission) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ((('freshtv2177 * _menhir_state * 'tv_cond_addr)) * 'tv_read_permission) * 'tv_write_permission) = Obj.magic _menhir_stack in
    let ((exec : 'tv_exec_permission) : 'tv_exec_permission) = _v in
    ((let (((_menhir_stack, _menhir_s, (addr : 'tv_cond_addr)), (read : 'tv_read_permission)), (write : 'tv_write_permission)) = _menhir_stack in
    let _2 = () in
    let _v : 'tv_predicate = 
# 227 "parser/parser.mly"
   ( Mk.filemode addr read write exec )
# 542 "parser/parser.ml"
     in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2175) = _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_predicate) = _v in
    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2173 * _menhir_state * 'tv_predicate) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState370 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState370
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState370
    | FALSE ->
        _menhir_run368 _menhir_env (Obj.magic _menhir_stack) MenhirState370
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState370 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState370 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState370
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState370
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState370
    | TRUE ->
        _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState370
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState370
    | CONSTANT | END | MALLOC | STACK ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2171 * _menhir_state * 'tv_predicate) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_predicate)) = _menhir_stack in
        let _v : 'tv_nonempty_list_predicate_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 583 "parser/parser.ml"
         in
        _menhir_goto_nonempty_list_predicate_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2172)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState370) : 'freshtv2174)) : 'freshtv2176)) : 'freshtv2178)) : 'freshtv2180)

and _menhir_goto_list_permission_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_permission_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState391 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv2151 * _menhir_state * 'tv_permission) * _menhir_state * 'tv_list_permission_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv2149 * _menhir_state * 'tv_permission) * _menhir_state * 'tv_list_permission_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_permission)), _, (xs : 'tv_list_permission_)) = _menhir_stack in
        let _v : 'tv_list_permission_ = 
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 604 "parser/parser.ml"
         in
        _menhir_goto_list_permission_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2150)) : 'freshtv2152)
    | MenhirState287 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2169)) * _menhir_state * 'tv_list_permission_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | END ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv2165)) * _menhir_state * 'tv_list_permission_) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | PERMISSIONS ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv2161)) * _menhir_state * 'tv_list_permission_)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv2159)) * _menhir_state * 'tv_list_permission_)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _, (permissions : 'tv_list_permission_)) = _menhir_stack in
                let _5 = () in
                let _4 = () in
                let _2 = () in
                let _1 = () in
                let _v : 'tv_permission_block = 
# 197 "parser/parser.mly"
   ( Mk.Permissions.of_list permissions )
# 633 "parser/parser.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv2157) = _menhir_stack in
                let (_v : 'tv_permission_block) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv2155) = Obj.magic _menhir_stack in
                let (_v : 'tv_permission_block) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv2153) = Obj.magic _menhir_stack in
                let ((x : 'tv_permission_block) : 'tv_permission_block) = _v in
                ((let _v : 'tv_option_permission_block_ = 
# 116 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 647 "parser/parser.ml"
                 in
                _menhir_goto_option_permission_block_ _menhir_env _menhir_stack _v) : 'freshtv2154)) : 'freshtv2156)) : 'freshtv2158)) : 'freshtv2160)) : 'freshtv2162)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv2163)) * _menhir_state * 'tv_list_permission_)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2164)) : 'freshtv2166)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv2167)) * _menhir_state * 'tv_list_permission_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2168)) : 'freshtv2170)
    | _ ->
        _menhir_fail ()

and _menhir_run291 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2147) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_cond_addr = 
# 231 "parser/parser.mly"
               ( Dba.Expr.one )
# 677 "parser/parser.ml"
     in
    _menhir_goto_cond_addr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2148)

and _menhir_run368 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2145) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_cond_addr = 
# 232 "parser/parser.mly"
               ( Dba.Expr.zero )
# 691 "parser/parser.ml"
     in
    _menhir_goto_cond_addr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2146)

and _menhir_goto_write_permission : _menhir_env -> 'ttv_tail -> 'tv_write_permission -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ((('freshtv2143 * _menhir_state * 'tv_cond_addr)) * 'tv_read_permission) * 'tv_write_permission) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | EXEC ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2135) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2133) = Obj.magic _menhir_stack in
        ((let _1 = () in
        let _v : 'tv_exec_permission = 
# 220 "parser/parser.mly"
         ( true )
# 713 "parser/parser.ml"
         in
        _menhir_goto_exec_permission _menhir_env _menhir_stack _v) : 'freshtv2134)) : 'freshtv2136)
    | NEXEC ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2139) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2137) = Obj.magic _menhir_stack in
        ((let _1 = () in
        let _v : 'tv_exec_permission = 
# 221 "parser/parser.mly"
         ( false )
# 726 "parser/parser.ml"
         in
        _menhir_goto_exec_permission _menhir_env _menhir_stack _v) : 'freshtv2138)) : 'freshtv2140)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv2141 * _menhir_state * 'tv_cond_addr)) * 'tv_read_permission) * 'tv_write_permission) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2142)) : 'freshtv2144)

and _menhir_goto_option_permission_block_ : _menhir_env -> 'ttv_tail -> 'tv_option_permission_block_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (('freshtv2131 * 'tv_config) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) * 'tv_option_permission_block_) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run145 _menhir_env (Obj.magic _menhir_stack) MenhirState397
    | IDENT _v ->
        _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState397 _v
    | EOF | LPAR ->
        _menhir_reduce198 _menhir_env (Obj.magic _menhir_stack) MenhirState397
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState397) : 'freshtv2132)

and _menhir_reduce196 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_permission_ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 761 "parser/parser.ml"
     in
    _menhir_goto_list_permission_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run288 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2127 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState289 _v
        | EXTS ->
            _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState289
        | EXTU ->
            _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState289
        | FALSE ->
            _menhir_run368 _menhir_env (Obj.magic _menhir_stack) MenhirState289
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState289 _v
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState289 _v
        | LPAR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState289
        | MINUS ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState289
        | NOT ->
            _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState289
        | TRUE ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState289
        | WORDSIZE ->
            _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState289
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState289) : 'freshtv2128)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2129 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2130)

and _menhir_run385 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2123 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState386 _v
        | EXTS ->
            _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState386
        | EXTU ->
            _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState386
        | FALSE ->
            _menhir_run368 _menhir_env (Obj.magic _menhir_stack) MenhirState386
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState386 _v
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState386 _v
        | LPAR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState386
        | MINUS ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState386
        | NOT ->
            _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState386
        | TRUE ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState386
        | WORDSIZE ->
            _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState386
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState386) : 'freshtv2124)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2125 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2126)

and _menhir_run388 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2119 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState389 _v
        | EXTS ->
            _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState389
        | EXTU ->
            _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState389
        | FALSE ->
            _menhir_run368 _menhir_env (Obj.magic _menhir_stack) MenhirState389
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState389 _v
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState389 _v
        | LPAR ->
            _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState389
        | MINUS ->
            _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState389
        | NOT ->
            _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState389
        | TRUE ->
            _menhir_run291 _menhir_env (Obj.magic _menhir_stack) MenhirState389
        | WORDSIZE ->
            _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState389
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState389) : 'freshtv2120)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2121 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2122)

and _menhir_reduce89 : _menhir_env -> ((('ttv_tail * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let ((((_menhir_stack, _menhir_s, (opcode : 'tv_kv)), _, (mnemonic : 'tv_kv)), _, (address : 'tv_kv)), _, (size : 'tv_kv)) = _menhir_stack in
    let _v : (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 909 "parser/parser.ml"
    ) = 
# 146 "parser/parser.mly"
  ( [opcode; mnemonic; address; size;]  (* Actually the order is not important *) )
# 913 "parser/parser.ml"
     in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2117) = _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 921 "parser/parser.ml"
    )) = _v in
    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState406 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2113 * _menhir_state * (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 930 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2111 * _menhir_state * (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 936 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (_1 : (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 941 "parser/parser.ml"
        ))) = _menhir_stack in
        Obj.magic _1) : 'freshtv2112)) : 'freshtv2114)
    | MenhirState426 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2115 * _menhir_state * (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 949 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState432
        | EOF ->
            _menhir_reduce192 _menhir_env (Obj.magic _menhir_stack) MenhirState432
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState432) : 'freshtv2116)
    | _ ->
        _menhir_fail ()) : 'freshtv2118)

and _menhir_goto_boption_ALTERNATE_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_boption_ALTERNATE_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState472 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2089) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_boption_ALTERNATE_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2087) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((alternate : 'tv_boption_ALTERNATE_) : 'tv_boption_ALTERNATE_) = _v in
        ((let _1 = () in
        let _v : 'tv_directive = let _1 = 
# 597 "parser/parser.mly"
        ()
# 981 "parser/parser.ml"
         in
        
# 618 "parser/parser.mly"
   ( Directive.choose_consequent ~alternate )
# 986 "parser/parser.ml"
         in
        _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv2088)) : 'freshtv2090)
    | MenhirState480 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2093) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_boption_ALTERNATE_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2091) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((alternate : 'tv_boption_ALTERNATE_) : 'tv_boption_ALTERNATE_) = _v in
        ((let _1 = () in
        let _v : 'tv_directive = let _1 = 
# 597 "parser/parser.mly"
        ()
# 1002 "parser/parser.ml"
         in
        
# 618 "parser/parser.mly"
   ( Directive.choose_consequent ~alternate )
# 1007 "parser/parser.ml"
         in
        _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv2092)) : 'freshtv2094)
    | MenhirState482 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2097) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_boption_ALTERNATE_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2095) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((alternate : 'tv_boption_ALTERNATE_) : 'tv_boption_ALTERNATE_) = _v in
        ((let _1 = () in
        let _v : 'tv_directive = let _1 = 
# 602 "parser/parser.mly"
        ()
# 1023 "parser/parser.ml"
         in
        
# 620 "parser/parser.mly"
   ( Directive.choose_alternative ~alternate )
# 1028 "parser/parser.ml"
         in
        _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv2096)) : 'freshtv2098)
    | MenhirState489 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2101) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_boption_ALTERNATE_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2099) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((alternate : 'tv_boption_ALTERNATE_) : 'tv_boption_ALTERNATE_) = _v in
        ((let _1 = () in
        let _v : 'tv_directive = let _1 = 
# 602 "parser/parser.mly"
        ()
# 1044 "parser/parser.ml"
         in
        
# 620 "parser/parser.mly"
   ( Directive.choose_alternative ~alternate )
# 1049 "parser/parser.ml"
         in
        _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv2100)) : 'freshtv2102)
    | MenhirState492 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2105) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_boption_ALTERNATE_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2103) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((alternate : 'tv_boption_ALTERNATE_) : 'tv_boption_ALTERNATE_) = _v in
        ((let _1 = () in
        let _v : 'tv_directive = let _1 = 
# 597 "parser/parser.mly"
        ()
# 1065 "parser/parser.ml"
         in
        
# 618 "parser/parser.mly"
   ( Directive.choose_consequent ~alternate )
# 1070 "parser/parser.ml"
         in
        _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv2104)) : 'freshtv2106)
    | MenhirState496 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2109) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_boption_ALTERNATE_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2107) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((alternate : 'tv_boption_ALTERNATE_) : 'tv_boption_ALTERNATE_) = _v in
        ((let _1 = () in
        let _v : 'tv_directive = let _1 = 
# 602 "parser/parser.mly"
        ()
# 1086 "parser/parser.ml"
         in
        
# 620 "parser/parser.mly"
   ( Directive.choose_alternative ~alternate )
# 1091 "parser/parser.ml"
         in
        _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv2108)) : 'freshtv2110)
    | _ ->
        _menhir_fail ()

and _menhir_goto_list_terminated_assignment_SEMICOLON__ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_terminated_assignment_SEMICOLON__ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState397 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv2081 * 'tv_config) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) * 'tv_option_permission_block_) * _menhir_state * 'tv_list_terminated_assignment_SEMICOLON__) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState398
        | EOF ->
            _menhir_reduce192 _menhir_env (Obj.magic _menhir_stack) MenhirState398
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState398) : 'freshtv2082)
    | MenhirState401 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2085 * _menhir_state * 'tv_assignment)) * _menhir_state * 'tv_list_terminated_assignment_SEMICOLON__) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2083 * _menhir_state * 'tv_assignment)) * _menhir_state * 'tv_list_terminated_assignment_SEMICOLON__) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_assignment)), _, (xs : 'tv_list_terminated_assignment_SEMICOLON__)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_list_terminated_assignment_SEMICOLON__ = let x = 
# 191 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 1125 "parser/parser.ml"
         in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 1130 "parser/parser.ml"
         in
        _menhir_goto_list_terminated_assignment_SEMICOLON__ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2084)) : 'freshtv2086)
    | _ ->
        _menhir_fail ()

and _menhir_goto_read_permission : _menhir_env -> 'ttv_tail -> 'tv_read_permission -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (('freshtv2079 * _menhir_state * 'tv_cond_addr)) * 'tv_read_permission) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | NWRITE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2071) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2069) = Obj.magic _menhir_stack in
        ((let _1 = () in
        let _v : 'tv_write_permission = 
# 216 "parser/parser.mly"
          ( false )
# 1154 "parser/parser.ml"
         in
        _menhir_goto_write_permission _menhir_env _menhir_stack _v) : 'freshtv2070)) : 'freshtv2072)
    | WRITE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2075) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2073) = Obj.magic _menhir_stack in
        ((let _1 = () in
        let _v : 'tv_write_permission = 
# 215 "parser/parser.mly"
          ( true )
# 1167 "parser/parser.ml"
         in
        _menhir_goto_write_permission _menhir_env _menhir_stack _v) : 'freshtv2074)) : 'freshtv2076)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2077 * _menhir_state * 'tv_cond_addr)) * 'tv_read_permission) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2078)) : 'freshtv2080)

and _menhir_run290 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2067) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_constant_expr = 
# 241 "parser/parser.mly"
             ( Dba.Expr.var "\\addr" (Kernel_options.Machine.word_size ()) )
# 1188 "parser/parser.ml"
     in
    _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2068)

and _menhir_run292 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState292 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState292
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState292
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState292 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState292 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState292
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState292
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState292
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState292
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState292

and _menhir_run293 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState293 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState293
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState293
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState293 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState293 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState293
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState293
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState293
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState293
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState293

and _menhir_run294 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState294 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState294
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState294
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState294 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState294 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState294
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState294
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState294
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState294
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState294

and _menhir_run295 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState295 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState295
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState295
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState295 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState295 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState295
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState295
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState295
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState295
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState295

and _menhir_run296 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState296 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState296
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState296
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState296 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState296 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState296
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState296
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState296
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState296
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState296

and _menhir_goto_list_terminated_declaration_SEMICOLON__ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_terminated_declaration_SEMICOLON__ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState265 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv2061 * 'tv_config) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BEGIN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv2055) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | PERMISSIONS ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv2051) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | CONSTANT ->
                    _menhir_run388 _menhir_env (Obj.magic _menhir_stack) MenhirState287
                | MALLOC ->
                    _menhir_run385 _menhir_env (Obj.magic _menhir_stack) MenhirState287
                | STACK ->
                    _menhir_run288 _menhir_env (Obj.magic _menhir_stack) MenhirState287
                | END ->
                    _menhir_reduce196 _menhir_env (Obj.magic _menhir_stack) MenhirState287
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState287) : 'freshtv2052)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv2053) = Obj.magic _menhir_stack in
                (raise _eRR : 'freshtv2054)) : 'freshtv2056)
        | AT | EOF | IDENT _ | LPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv2057) = Obj.magic _menhir_stack in
            ((let _v : 'tv_option_permission_block_ = 
# 114 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 1383 "parser/parser.ml"
             in
            _menhir_goto_option_permission_block_ _menhir_env _menhir_stack _v) : 'freshtv2058)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv2059 * 'tv_config) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2060)) : 'freshtv2062)
    | MenhirState404 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2065 * _menhir_state * 'tv_declaration)) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2063 * _menhir_state * 'tv_declaration)) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_declaration)), _, (xs : 'tv_list_terminated_declaration_SEMICOLON__)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_list_terminated_declaration_SEMICOLON__ = let x = 
# 191 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 1403 "parser/parser.ml"
         in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 1408 "parser/parser.ml"
         in
        _menhir_goto_list_terminated_declaration_SEMICOLON__ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2064)) : 'freshtv2066)
    | _ ->
        _menhir_fail ()

and _menhir_goto_option_tags_ : _menhir_env -> 'ttv_tail -> 'tv_option_tags_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ((('freshtv2049 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 1420 "parser/parser.ml"
    ))) * (
# 61 "parser/parser.mly"
       (string)
# 1424 "parser/parser.ml"
    )) = Obj.magic _menhir_stack in
    let (_v : 'tv_option_tags_) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ((('freshtv2047 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 1431 "parser/parser.ml"
    ))) * (
# 61 "parser/parser.mly"
       (string)
# 1435 "parser/parser.ml"
    )) = Obj.magic _menhir_stack in
    let ((tags : 'tv_option_tags_) : 'tv_option_tags_) = _v in
    ((let (((_menhir_stack, _menhir_s), (id : (
# 62 "parser/parser.mly"
       (string)
# 1441 "parser/parser.ml"
    ))), (size : (
# 61 "parser/parser.mly"
       (string)
# 1445 "parser/parser.ml"
    ))) = _menhir_stack in
    let _3 = () in
    let _1 = () in
    let _v : 'tv_declaration = 
# 185 "parser/parser.mly"
  ( mk_declaration (match tags with None -> VarTag.empty | Some t -> t)  id (int_of_string size) )
# 1452 "parser/parser.ml"
     in
    _menhir_goto_declaration _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2048)) : 'freshtv2050)

and _menhir_goto_declaration : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_declaration -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2045 * _menhir_state * 'tv_declaration) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2041 * _menhir_state * 'tv_declaration) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | FLAG ->
            _menhir_run281 _menhir_env (Obj.magic _menhir_stack) MenhirState404
        | REGISTER ->
            _menhir_run277 _menhir_env (Obj.magic _menhir_stack) MenhirState404
        | TEMPORARY ->
            _menhir_run273 _menhir_env (Obj.magic _menhir_stack) MenhirState404
        | VAR ->
            _menhir_run266 _menhir_env (Obj.magic _menhir_stack) MenhirState404
        | AT | BEGIN | EOF | IDENT _ | LPAR ->
            _menhir_reduce200 _menhir_env (Obj.magic _menhir_stack) MenhirState404
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState404) : 'freshtv2042)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2043 * _menhir_state * 'tv_declaration) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2044)) : 'freshtv2046)

and _menhir_reduce186 : _menhir_env -> ((('ttv_tail * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 1495 "parser/parser.ml"
))) * _menhir_state * 'tv_value -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let (((_menhir_stack, _menhir_s), (key : (
# 62 "parser/parser.mly"
       (string)
# 1501 "parser/parser.ml"
    ))), _, (v : 'tv_value)) = _menhir_stack in
    let _5 = () in
    let _3 = () in
    let _1 = () in
    let _v : 'tv_kv = 
# 141 "parser/parser.mly"
                                    ( key, v )
# 1509 "parser/parser.ml"
     in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2039) = _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_kv) = _v in
    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState406 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2013 * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run407 _menhir_env (Obj.magic _menhir_stack) MenhirState416
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState416) : 'freshtv2014)
    | MenhirState416 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv2015 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run407 _menhir_env (Obj.magic _menhir_stack) MenhirState417
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState417) : 'freshtv2016)
    | MenhirState417 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2027 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv2025) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState418 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | IDENT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv2021 * _menhir_state) = Obj.magic _menhir_stack in
                let (_v : (
# 62 "parser/parser.mly"
       (string)
# 1561 "parser/parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | DOT ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ('freshtv2017 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 1572 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | BIN _v ->
                        _menhir_run413 _menhir_env (Obj.magic _menhir_stack) MenhirState421 _v
                    | HEXA _v ->
                        _menhir_run412 _menhir_env (Obj.magic _menhir_stack) MenhirState421 _v
                    | INT _v ->
                        _menhir_run411 _menhir_env (Obj.magic _menhir_stack) MenhirState421 _v
                    | STRING _v ->
                        _menhir_run410 _menhir_env (Obj.magic _menhir_stack) MenhirState421 _v
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState421) : 'freshtv2018)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ('freshtv2019 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 1596 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2020)) : 'freshtv2022)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv2023 * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2024)) : 'freshtv2026)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState418) : 'freshtv2028)
    | MenhirState418 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv2029 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        (_menhir_reduce89 _menhir_env (Obj.magic _menhir_stack) : 'freshtv2030)
    | MenhirState426 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2031 * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run407 _menhir_env (Obj.magic _menhir_stack) MenhirState427
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState427) : 'freshtv2032)
    | MenhirState427 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv2033 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run407 _menhir_env (Obj.magic _menhir_stack) MenhirState428
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState428) : 'freshtv2034)
    | MenhirState428 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2035 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run407 _menhir_env (Obj.magic _menhir_stack) MenhirState429
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState429) : 'freshtv2036)
    | MenhirState429 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv2037 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        (_menhir_reduce89 _menhir_env (Obj.magic _menhir_stack) : 'freshtv2038)
    | _ ->
        _menhir_fail ()) : 'freshtv2040)

and _menhir_reduce36 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_boption_ALTERNATE_ = 
# 133 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( false )
# 1663 "parser/parser.ml"
     in
    _menhir_goto_boption_ALTERNATE_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run473 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv2011) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_boption_ALTERNATE_ = 
# 135 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( true )
# 1677 "parser/parser.ml"
     in
    _menhir_goto_boption_ALTERNATE_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2012)

and _menhir_goto_separated_nonempty_list_SEMICOLON_located_directive_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_separated_nonempty_list_SEMICOLON_located_directive_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState453 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv2005 * _menhir_state * 'tv_separated_nonempty_list_SEMICOLON_located_directive_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv2001 * _menhir_state * 'tv_separated_nonempty_list_SEMICOLON_located_directive_) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1999 * _menhir_state * 'tv_separated_nonempty_list_SEMICOLON_located_directive_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (l : 'tv_separated_nonempty_list_SEMICOLON_located_directive_)) = _menhir_stack in
            let _2 = () in
            let _v : (
# 112 "parser/parser.mly"
      (Directive.t list)
# 1701 "parser/parser.ml"
            ) = 
# 643 "parser/parser.mly"
                                                                ( l )
# 1705 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1997) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 112 "parser/parser.mly"
      (Directive.t list)
# 1713 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1995) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 112 "parser/parser.mly"
      (Directive.t list)
# 1721 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1993) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let ((_1 : (
# 112 "parser/parser.mly"
      (Directive.t list)
# 1729 "parser/parser.ml"
            )) : (
# 112 "parser/parser.mly"
      (Directive.t list)
# 1733 "parser/parser.ml"
            )) = _v in
            (Obj.magic _1 : 'freshtv1994)) : 'freshtv1996)) : 'freshtv1998)) : 'freshtv2000)) : 'freshtv2002)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv2003 * _menhir_state * 'tv_separated_nonempty_list_SEMICOLON_located_directive_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv2004)) : 'freshtv2006)
    | MenhirState468 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2009 * _menhir_state * 'tv_located_directive)) * _menhir_state * 'tv_separated_nonempty_list_SEMICOLON_located_directive_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv2007 * _menhir_state * 'tv_located_directive)) * _menhir_state * 'tv_separated_nonempty_list_SEMICOLON_located_directive_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_located_directive)), _, (xs : 'tv_separated_nonempty_list_SEMICOLON_located_directive_)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_separated_nonempty_list_SEMICOLON_located_directive_ = 
# 243 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 1753 "parser/parser.ml"
         in
        _menhir_goto_separated_nonempty_list_SEMICOLON_located_directive_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv2008)) : 'freshtv2010)
    | _ ->
        _menhir_fail ()

and _menhir_reduce198 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_terminated_assignment_SEMICOLON__ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 1764 "parser/parser.ml"
     in
    _menhir_goto_list_terminated_assignment_SEMICOLON__ _menhir_env _menhir_stack _menhir_s _v

and _menhir_goto_jump_target : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_jump_target -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1991 * _menhir_state) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_jump_target) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1989 * _menhir_state) = Obj.magic _menhir_stack in
    let (_ : _menhir_state) = _menhir_s in
    let ((jump : 'tv_jump_target) : 'tv_jump_target) = _v in
    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
    let _1 = () in
    let _v : 'tv_explicit_instruction = 
# 390 "parser/parser.mly"
                         ( jump )
# 1783 "parser/parser.ml"
     in
    _menhir_goto_explicit_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1990)) : 'freshtv1992)

and _menhir_reduce211 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_address_lvalue -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let (_menhir_stack, _menhir_s, (v : 'tv_address_lvalue)) = _menhir_stack in
    let _v : 'tv_lvalue = 
# 457 "parser/parser.mly"
                   ( v )
# 1793 "parser/parser.ml"
     in
    _menhir_goto_lvalue _menhir_env _menhir_stack _menhir_s _v

and _menhir_goto_cond_addr : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_cond_addr -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1987 * _menhir_state * 'tv_cond_addr) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1983 * _menhir_state * 'tv_cond_addr) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | NREAD ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1975) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1973) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_read_permission = 
# 210 "parser/parser.mly"
         ( false )
# 1821 "parser/parser.ml"
             in
            _menhir_goto_read_permission _menhir_env _menhir_stack _v) : 'freshtv1974)) : 'freshtv1976)
        | READ ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1979) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1977) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_read_permission = 
# 211 "parser/parser.mly"
         ( true )
# 1834 "parser/parser.ml"
             in
            _menhir_goto_read_permission _menhir_env _menhir_stack _v) : 'freshtv1978)) : 'freshtv1980)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1981 * _menhir_state * 'tv_cond_addr)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1982)) : 'freshtv1984)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1985 * _menhir_state * 'tv_cond_addr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1986)) : 'freshtv1988)

and _menhir_run298 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState298 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState298
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState298
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState298 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState298 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState298
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState298
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState298
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState298
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState298

and _menhir_run303 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState303 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState303
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState303
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState303 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState303 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState303
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState303
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState303
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState303
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState303

and _menhir_run307 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState307 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState307
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState307
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState307 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState307 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState307
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState307
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState307
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState307
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState307

and _menhir_run309 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState309 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState309
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState309
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState309 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState309 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState309
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState309
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState309
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState309
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState309

and _menhir_run311 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState311 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState311
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState311
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState311 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState311 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState311
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState311
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState311
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState311
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState311

and _menhir_run313 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState313 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState313
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState313
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState313 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState313 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState313
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState313
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState313
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState313
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState313

and _menhir_run315 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState315 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState315
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState315
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState315 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState315 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState315
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState315
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState315
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState315
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState315

and _menhir_run325 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState325 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState325
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState325
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState325 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState325 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState325
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState325
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState325
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState325
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState325

and _menhir_run327 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState327 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState327
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState327
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState327 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState327 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState327
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState327
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState327
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState327
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState327

and _menhir_run317 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState317 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState317
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState317
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState317 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState317 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState317
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState317
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState317
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState317
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState317

and _menhir_run305 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState305 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState305
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState305
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState305 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState305 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState305
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState305
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState305
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState305
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState305

and _menhir_run329 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState329 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState329
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState329
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState329 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState329 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState329
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState329
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState329
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState329
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState329

and _menhir_run319 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState319 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState319
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState319
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState319 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState319 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState319
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState319
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState319
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState319
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState319

and _menhir_run321 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState321 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState321
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState321
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState321 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState321 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState321
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState321
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState321
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState321
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState321

and _menhir_run323 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState323 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState323
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState323
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState323 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState323 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState323
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState323
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState323
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState323
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState323

and _menhir_run337 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState337 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState337
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState337
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState337 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState337 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState337
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState337
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState337
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState337
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState337

and _menhir_run339 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState339 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState339
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState339
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState339 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState339 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState339
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState339
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState339
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState339
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState339

and _menhir_run331 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState331 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState331
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState331
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState331 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState331 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState331
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState331
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState331
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState331
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState331

and _menhir_run333 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState333 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState333
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState333
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState333 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState333 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState333
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState333
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState333
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState333
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState333

and _menhir_run341 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState341 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState341
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState341
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState341 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState341 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState341
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState341
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState341
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState341
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState341

and _menhir_run343 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState343 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState343
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState343
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState343 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState343 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState343
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState343
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState343
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState343
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState343

and _menhir_run345 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1969 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        let (_v : (
# 61 "parser/parser.mly"
       (string)
# 2451 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1961 * _menhir_state * 'tv_constant_expr)) * (
# 61 "parser/parser.mly"
       (string)
# 2462 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1957 * _menhir_state * 'tv_constant_expr)) * (
# 61 "parser/parser.mly"
       (string)
# 2472 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                let (_v : (
# 61 "parser/parser.mly"
       (string)
# 2477 "parser/parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RBRACE ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1953 * _menhir_state * 'tv_constant_expr)) * (
# 61 "parser/parser.mly"
       (string)
# 2488 "parser/parser.ml"
                    ))) * (
# 61 "parser/parser.mly"
       (string)
# 2492 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1951 * _menhir_state * 'tv_constant_expr)) * (
# 61 "parser/parser.mly"
       (string)
# 2499 "parser/parser.ml"
                    ))) * (
# 61 "parser/parser.mly"
       (string)
# 2503 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _menhir_s, (e : 'tv_constant_expr)), (loff : (
# 61 "parser/parser.mly"
       (string)
# 2508 "parser/parser.ml"
                    ))), (roff : (
# 61 "parser/parser.mly"
       (string)
# 2512 "parser/parser.ml"
                    ))) = _menhir_stack in
                    let _5 = () in
                    let _3 = () in
                    let _1 = () in
                    let _v : 'tv_constant_expr = let offs = 
# 470 "parser/parser.mly"
  ( int_of_string loff, int_of_string roff )
# 2520 "parser/parser.ml"
                     in
                    
# 245 "parser/parser.mly"
   ( let lo, hi = offs in Dba.Expr.restrict lo hi e )
# 2525 "parser/parser.ml"
                     in
                    _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1952)) : 'freshtv1954)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1955 * _menhir_state * 'tv_constant_expr)) * (
# 61 "parser/parser.mly"
       (string)
# 2535 "parser/parser.ml"
                    ))) * (
# 61 "parser/parser.mly"
       (string)
# 2539 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1956)) : 'freshtv1958)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1959 * _menhir_state * 'tv_constant_expr)) * (
# 61 "parser/parser.mly"
       (string)
# 2550 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1960)) : 'freshtv1962)
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1965 * _menhir_state * 'tv_constant_expr)) * (
# 61 "parser/parser.mly"
       (string)
# 2559 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1963 * _menhir_state * 'tv_constant_expr)) * (
# 61 "parser/parser.mly"
       (string)
# 2566 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (e : 'tv_constant_expr)), (boff : (
# 61 "parser/parser.mly"
       (string)
# 2571 "parser/parser.ml"
            ))) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_constant_expr = let offs = 
# 472 "parser/parser.mly"
  ( let n = int_of_string boff in n, n)
# 2578 "parser/parser.ml"
             in
            
# 245 "parser/parser.mly"
   ( let lo, hi = offs in Dba.Expr.restrict lo hi e )
# 2583 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1964)) : 'freshtv1966)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1967 * _menhir_state * 'tv_constant_expr)) * (
# 61 "parser/parser.mly"
       (string)
# 2593 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1968)) : 'freshtv1970)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1971 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1972)

and _menhir_run352 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState352 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState352
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState352
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState352 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState352 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState352
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState352
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState352
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState352
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState352

and _menhir_run354 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState354 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState354
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState354
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState354 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState354 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState354
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState354
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState354
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState354
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState354

and _menhir_run356 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState356 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState356
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState356
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState356 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState356 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState356
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState356
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState356
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState356
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState356

and _menhir_run358 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState358 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState358
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState358
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState358 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState358 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState358
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState358
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState358
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState358
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState358

and _menhir_run360 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState360 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState360
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState360
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState360 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState360 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState360
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState360
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState360
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState360
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState360

and _menhir_run335 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState335 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState335
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState335
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState335 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState335 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState335
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState335
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState335
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState335
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState335

and _menhir_run300 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState300 _v
    | EXTS ->
        _menhir_run296 _menhir_env (Obj.magic _menhir_stack) MenhirState300
    | EXTU ->
        _menhir_run295 _menhir_env (Obj.magic _menhir_stack) MenhirState300
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState300 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState300 _v
    | LPAR ->
        _menhir_run294 _menhir_env (Obj.magic _menhir_stack) MenhirState300
    | MINUS ->
        _menhir_run293 _menhir_env (Obj.magic _menhir_stack) MenhirState300
    | NOT ->
        _menhir_run292 _menhir_env (Obj.magic _menhir_stack) MenhirState300
    | WORDSIZE ->
        _menhir_run290 _menhir_env (Obj.magic _menhir_stack) MenhirState300
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState300

and _menhir_reduce200 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_terminated_declaration_SEMICOLON__ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 2806 "parser/parser.ml"
     in
    _menhir_goto_list_terminated_declaration_SEMICOLON__ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run266 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | IDENT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1947 * _menhir_state) = Obj.magic _menhir_stack in
        let (_v : (
# 62 "parser/parser.mly"
       (string)
# 2822 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1943 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 2833 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1939 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 2843 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                let (_v : (
# 61 "parser/parser.mly"
       (string)
# 2848 "parser/parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | FLAGTAG ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv1929) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv1927) = Obj.magic _menhir_stack in
                    ((let _1 = () in
                    let _v : 'tv_option_tags_ = let x = 
# 192 "parser/parser.mly"
           ( VarTag.flag Flag.unspecified )
# 2864 "parser/parser.ml"
                     in
                    
# 116 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 2869 "parser/parser.ml"
                     in
                    _menhir_goto_option_tags_ _menhir_env _menhir_stack _v) : 'freshtv1928)) : 'freshtv1930)
                | TEMPTAG ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv1933) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv1931) = Obj.magic _menhir_stack in
                    ((let _1 = () in
                    let _v : 'tv_option_tags_ = let x = 
# 191 "parser/parser.mly"
           ( VarTag.temp )
# 2882 "parser/parser.ml"
                     in
                    
# 116 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 2887 "parser/parser.ml"
                     in
                    _menhir_goto_option_tags_ _menhir_env _menhir_stack _v) : 'freshtv1932)) : 'freshtv1934)
                | SEMICOLON ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv1935) = Obj.magic _menhir_stack in
                    ((let _v : 'tv_option_tags_ = 
# 114 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 2896 "parser/parser.ml"
                     in
                    _menhir_goto_option_tags_ _menhir_env _menhir_stack _v) : 'freshtv1936)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv1937 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 2906 "parser/parser.ml"
                    ))) * (
# 61 "parser/parser.mly"
       (string)
# 2910 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _menhir_s), _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1938)) : 'freshtv1940)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1941 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 2921 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1942)) : 'freshtv1944)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1945 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 2932 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1946)) : 'freshtv1948)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1949 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1950)

and _menhir_run273 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | IDENT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1923 * _menhir_state) = Obj.magic _menhir_stack in
        let (_v : (
# 62 "parser/parser.mly"
       (string)
# 2956 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1919 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 2967 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1915 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 2977 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                let (_v : (
# 61 "parser/parser.mly"
       (string)
# 2982 "parser/parser.ml"
                )) = _v in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1913 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 2989 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                let ((size : (
# 61 "parser/parser.mly"
       (string)
# 2994 "parser/parser.ml"
                )) : (
# 61 "parser/parser.mly"
       (string)
# 2998 "parser/parser.ml"
                )) = _v in
                ((let ((_menhir_stack, _menhir_s), (id : (
# 62 "parser/parser.mly"
       (string)
# 3003 "parser/parser.ml"
                ))) = _menhir_stack in
                let _3 = () in
                let _1 = () in
                let _v : 'tv_declaration = let apply = 
# 178 "parser/parser.mly"
            ( mk_declaration VarTag.temp )
# 3010 "parser/parser.ml"
                 in
                
# 187 "parser/parser.mly"
  ( apply id (int_of_string size) )
# 3015 "parser/parser.ml"
                 in
                _menhir_goto_declaration _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1914)) : 'freshtv1916)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1917 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3025 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1918)) : 'freshtv1920)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1921 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3036 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1922)) : 'freshtv1924)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1925 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1926)

and _menhir_run277 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | IDENT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1909 * _menhir_state) = Obj.magic _menhir_stack in
        let (_v : (
# 62 "parser/parser.mly"
       (string)
# 3060 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1905 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3071 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1901 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3081 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                let (_v : (
# 61 "parser/parser.mly"
       (string)
# 3086 "parser/parser.ml"
                )) = _v in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1899 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3093 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                let ((size : (
# 61 "parser/parser.mly"
       (string)
# 3098 "parser/parser.ml"
                )) : (
# 61 "parser/parser.mly"
       (string)
# 3102 "parser/parser.ml"
                )) = _v in
                ((let ((_menhir_stack, _menhir_s), (id : (
# 62 "parser/parser.mly"
       (string)
# 3107 "parser/parser.ml"
                ))) = _menhir_stack in
                let _3 = () in
                let _1 = () in
                let _v : 'tv_declaration = let apply = 
# 180 "parser/parser.mly"
            ( mk_declaration VarTag.empty )
# 3114 "parser/parser.ml"
                 in
                
# 187 "parser/parser.mly"
  ( apply id (int_of_string size) )
# 3119 "parser/parser.ml"
                 in
                _menhir_goto_declaration _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1900)) : 'freshtv1902)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1903 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3129 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1904)) : 'freshtv1906)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1907 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3140 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1908)) : 'freshtv1910)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1911 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1912)

and _menhir_run281 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | IDENT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1895 * _menhir_state) = Obj.magic _menhir_stack in
        let (_v : (
# 62 "parser/parser.mly"
       (string)
# 3164 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1891 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3175 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1887 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3185 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                let (_v : (
# 61 "parser/parser.mly"
       (string)
# 3190 "parser/parser.ml"
                )) = _v in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1885 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3197 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                let ((size : (
# 61 "parser/parser.mly"
       (string)
# 3202 "parser/parser.ml"
                )) : (
# 61 "parser/parser.mly"
       (string)
# 3206 "parser/parser.ml"
                )) = _v in
                ((let ((_menhir_stack, _menhir_s), (id : (
# 62 "parser/parser.mly"
       (string)
# 3211 "parser/parser.ml"
                ))) = _menhir_stack in
                let _3 = () in
                let _1 = () in
                let _v : 'tv_declaration = let apply = 
# 179 "parser/parser.mly"
            ( mk_declaration (VarTag.flag Flag.unspecified) )
# 3218 "parser/parser.ml"
                 in
                
# 187 "parser/parser.mly"
  ( apply id (int_of_string size) )
# 3223 "parser/parser.ml"
                 in
                _menhir_goto_declaration _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1886)) : 'freshtv1888)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1889 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3233 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1890)) : 'freshtv1892)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1893 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3244 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1894)) : 'freshtv1896)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1897 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1898)

and _menhir_goto_jump_annotation : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_jump_annotation -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState200 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1879 * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_jump_annotation) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1877 * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((x : 'tv_jump_annotation) : 'tv_jump_annotation) = _v in
        ((let (_menhir_stack, _menhir_s, (t : 'tv_static_target)) = _menhir_stack in
        let _v : 'tv_static_jump = let tag = 
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 3272 "parser/parser.ml"
         in
        
# 295 "parser/parser.mly"
  ( Dba.Instr.static_jump t ?tag  )
# 3277 "parser/parser.ml"
         in
        _menhir_goto_static_jump _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1878)) : 'freshtv1880)
    | MenhirState208 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1883) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_jump_annotation) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1881) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((x : 'tv_jump_annotation) : 'tv_jump_annotation) = _v in
        ((let _v : 'tv_option_jump_annotation_ = 
# 116 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 3292 "parser/parser.ml"
         in
        _menhir_goto_option_jump_annotation_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1882)) : 'freshtv1884)
    | _ ->
        _menhir_fail ()

and _menhir_goto_separated_nonempty_list_COMMA_printarg_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_separated_nonempty_list_COMMA_printarg_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState20 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1871 * _menhir_state) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_separated_nonempty_list_COMMA_printarg_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1869 * _menhir_state) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((args : 'tv_separated_nonempty_list_COMMA_printarg_) : 'tv_separated_nonempty_list_COMMA_printarg_) = _v in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_annotable_instruction = 
# 375 "parser/parser.mly"
  ( Dba.Instr.print args unknown_successor )
# 3315 "parser/parser.ml"
         in
        _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1870)) : 'freshtv1872)
    | MenhirState130 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1875 * _menhir_state * 'tv_printarg)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_separated_nonempty_list_COMMA_printarg_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1873 * _menhir_state * 'tv_printarg)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_separated_nonempty_list_COMMA_printarg_) : 'tv_separated_nonempty_list_COMMA_printarg_) = _v in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_printarg)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_separated_nonempty_list_COMMA_printarg_ = 
# 243 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3332 "parser/parser.ml"
         in
        _menhir_goto_separated_nonempty_list_COMMA_printarg_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1874)) : 'freshtv1876)
    | _ ->
        _menhir_fail ()

and _menhir_goto_initialization_assignment : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_initialization_assignment -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1867 * _menhir_state * 'tv_boption_UNCONTROLLED_) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_initialization_assignment) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1865 * _menhir_state * 'tv_boption_UNCONTROLLED_) = Obj.magic _menhir_stack in
    let (_ : _menhir_state) = _menhir_s in
    let ((init : 'tv_initialization_assignment) : 'tv_initialization_assignment) = _v in
    ((let (_menhir_stack, _menhir_s, (uncontrolled : 'tv_boption_UNCONTROLLED_)) = _menhir_stack in
    let _v : 'tv_initialization_directive = 
# 365 "parser/parser.mly"
  ( Initialization.set_control (not uncontrolled) init )
# 3352 "parser/parser.ml"
     in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1863) = _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_initialization_directive) = _v in
    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1861 * _menhir_state * 'tv_initialization_directive) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1857 * _menhir_state * 'tv_initialization_directive) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | UNCONTROLLED ->
            _menhir_run505 _menhir_env (Obj.magic _menhir_stack) MenhirState509
        | EOF ->
            _menhir_reduce202 _menhir_env (Obj.magic _menhir_stack) MenhirState509
        | AT | IDENT _ ->
            _menhir_reduce38 _menhir_env (Obj.magic _menhir_stack) MenhirState509
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState509) : 'freshtv1858)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1859 * _menhir_state * 'tv_initialization_directive) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1860)) : 'freshtv1862)) : 'freshtv1864)) : 'freshtv1866)) : 'freshtv1868)

and _menhir_run233 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1855) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_rvalue = 
# 318 "parser/parser.mly"
          ( Dba.Instr.undefined )
# 3398 "parser/parser.ml"
     in
    _menhir_goto_rvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1856)

and _menhir_run234 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1853) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_rvalue = 
# 324 "parser/parser.mly"
  ( fun lv -> Dba.Instr.non_deterministic lv )
# 3412 "parser/parser.ml"
     in
    _menhir_goto_rvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1854)

and _menhir_run235 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1847 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | CONSTANT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1827 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1823 * _menhir_state))) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1821 * _menhir_state))) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                let _3 = () in
                let _1_inlined2 = () in
                let _1_inlined1 = () in
                let _1 = () in
                let _v : 'tv_rvalue = let region =
                  let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                  let x =
                    let x =
                      let _1 = _1_inlined1 in
                      let region = 
# 260 "parser/parser.mly"
            ( `Constant )
# 3453 "parser/parser.ml"
                       in
                      
# 265 "parser/parser.mly"
                  ( region )
# 3458 "parser/parser.ml"
                      
                    in
                    
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3464 "parser/parser.ml"
                    
                  in
                  
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 3470 "parser/parser.ml"
                  
                in
                
# 322 "parser/parser.mly"
  ( fun lv -> Dba.Instr.non_deterministic lv ?region )
# 3476 "parser/parser.ml"
                 in
                _menhir_goto_rvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1822)) : 'freshtv1824)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1825 * _menhir_state))) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1826)) : 'freshtv1828)
        | MALLOC ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1835 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1831 * _menhir_state))) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1829 * _menhir_state))) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                let _3 = () in
                let _1_inlined2 = () in
                let _1_inlined1 = () in
                let _1 = () in
                let _v : 'tv_rvalue = let region =
                  let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                  let x =
                    let x =
                      let _1 = _1_inlined1 in
                      
# 266 "parser/parser.mly"
                  ( Dba_types.Region.malloc (Kernel_options.Machine.word_size ()))
# 3511 "parser/parser.ml"
                      
                    in
                    
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3517 "parser/parser.ml"
                    
                  in
                  
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 3523 "parser/parser.ml"
                  
                in
                
# 322 "parser/parser.mly"
  ( fun lv -> Dba.Instr.non_deterministic lv ?region )
# 3529 "parser/parser.ml"
                 in
                _menhir_goto_rvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1830)) : 'freshtv1832)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1833 * _menhir_state))) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1834)) : 'freshtv1836)
        | STACK ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1843 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1839 * _menhir_state))) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1837 * _menhir_state))) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                let _3 = () in
                let _1_inlined2 = () in
                let _1_inlined1 = () in
                let _1 = () in
                let _v : 'tv_rvalue = let region =
                  let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                  let x =
                    let x =
                      let _1 = _1_inlined1 in
                      let region = 
# 261 "parser/parser.mly"
            ( `Stack )
# 3564 "parser/parser.ml"
                       in
                      
# 265 "parser/parser.mly"
                  ( region )
# 3569 "parser/parser.ml"
                      
                    in
                    
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3575 "parser/parser.ml"
                    
                  in
                  
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 3581 "parser/parser.ml"
                  
                in
                
# 322 "parser/parser.mly"
  ( fun lv -> Dba.Instr.non_deterministic lv ?region )
# 3587 "parser/parser.ml"
                 in
                _menhir_goto_rvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1838)) : 'freshtv1840)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1841 * _menhir_state))) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1842)) : 'freshtv1844)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1845 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1846)) : 'freshtv1848)
    | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1849 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_rvalue = let region = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 3612 "parser/parser.ml"
         in
        
# 322 "parser/parser.mly"
  ( fun lv -> Dba.Instr.non_deterministic lv ?region )
# 3617 "parser/parser.ml"
         in
        _menhir_goto_rvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1850)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1851 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1852)

and _menhir_run243 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1817 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState244
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState244 _v
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState244
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState244
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState244 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState244 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState244
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState244 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState244
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState244
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState244
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState244) : 'freshtv1818)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1819 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1820)

and _menhir_goto_separated_nonempty_list_COMMA_lvalue_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_separated_nonempty_list_COMMA_lvalue_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState135 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1811) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_separated_nonempty_list_COMMA_lvalue_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1809) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((x : 'tv_separated_nonempty_list_COMMA_lvalue_) : 'tv_separated_nonempty_list_COMMA_lvalue_) = _v in
        ((let _v : 'tv_loption_separated_nonempty_list_COMMA_lvalue__ = 
# 144 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3689 "parser/parser.ml"
         in
        _menhir_goto_loption_separated_nonempty_list_COMMA_lvalue__ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1810)) : 'freshtv1812)
    | MenhirState161 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1815 * _menhir_state * 'tv_lvalue)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_separated_nonempty_list_COMMA_lvalue_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1813 * _menhir_state * 'tv_lvalue)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_separated_nonempty_list_COMMA_lvalue_) : 'tv_separated_nonempty_list_COMMA_lvalue_) = _v in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_lvalue)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_separated_nonempty_list_COMMA_lvalue_ = 
# 243 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3706 "parser/parser.ml"
         in
        _menhir_goto_separated_nonempty_list_COMMA_lvalue_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1814)) : 'freshtv1816)
    | _ ->
        _menhir_fail ()

and _menhir_goto_static_jump : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_static_jump -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1807) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_static_jump) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1805) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((sj : 'tv_static_jump) : 'tv_static_jump) = _v in
    ((let _v : 'tv_jump_target = 
# 300 "parser/parser.mly"
  ( sj )
# 3725 "parser/parser.ml"
     in
    _menhir_goto_jump_target _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1806)) : 'freshtv1808)

and _menhir_goto_value : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_value -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState409 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1797 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3738 "parser/parser.ml"
        ))) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1793 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3748 "parser/parser.ml"
            ))) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            _menhir_reduce186 _menhir_env (Obj.magic _menhir_stack)) : 'freshtv1794)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1795 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3759 "parser/parser.ml"
            ))) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1796)) : 'freshtv1798)
    | MenhirState421 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1803 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3768 "parser/parser.ml"
        ))) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1799 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3778 "parser/parser.ml"
            ))) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
            (_menhir_reduce186 _menhir_env (Obj.magic _menhir_stack) : 'freshtv1800)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1801 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 3788 "parser/parser.ml"
            ))) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1802)) : 'freshtv1804)
    | _ ->
        _menhir_fail ()

and _menhir_goto_nonempty_list_integer_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_integer_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState547 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1787) * _menhir_state * 'tv_nonempty_list_integer_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1783) * _menhir_state * 'tv_nonempty_list_integer_) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1781) * _menhir_state * 'tv_nonempty_list_integer_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, (x : 'tv_nonempty_list_integer_)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_opcode = let integers = 
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3817 "parser/parser.ml"
             in
            
# 578 "parser/parser.mly"
   ( Binstream.of_list integers )
# 3822 "parser/parser.ml"
             in
            _menhir_goto_opcode _menhir_env _menhir_stack _v) : 'freshtv1782)) : 'freshtv1784)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1785) * _menhir_state * 'tv_nonempty_list_integer_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1786)) : 'freshtv1788)
    | MenhirState550 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1791 * _menhir_state * 'tv_integer) * _menhir_state * 'tv_nonempty_list_integer_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1789 * _menhir_state * 'tv_integer) * _menhir_state * 'tv_nonempty_list_integer_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_integer)), _, (xs : 'tv_nonempty_list_integer_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_integer_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3841 "parser/parser.ml"
         in
        _menhir_goto_nonempty_list_integer_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1790)) : 'freshtv1792)
    | _ ->
        _menhir_fail ()

and _menhir_goto_opcode : _menhir_env -> 'ttv_tail -> 'tv_opcode -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (('freshtv1779 * _menhir_state) * _menhir_state * 'tv_integer) * 'tv_opcode) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | RPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1775 * _menhir_state) * _menhir_state * 'tv_integer) * 'tv_opcode) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1773 * _menhir_state) * _menhir_state * 'tv_integer) * 'tv_opcode) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s), _, (address : 'tv_integer)), (opc : 'tv_opcode)) = _menhir_stack in
        let _4 = () in
        let _1 = () in
        let _v : 'tv_patch = 
# 582 "parser/parser.mly"
                                           ( address, opc )
# 3867 "parser/parser.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1771) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_patch) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1769 * _menhir_state * 'tv_patch) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run544 _menhir_env (Obj.magic _menhir_stack) MenhirState555
        | EOF ->
            _menhir_reduce194 _menhir_env (Obj.magic _menhir_stack) MenhirState555
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState555) : 'freshtv1770)) : 'freshtv1772)) : 'freshtv1774)) : 'freshtv1776)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1777 * _menhir_state) * _menhir_state * 'tv_integer) * 'tv_opcode) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1778)) : 'freshtv1780)

and _menhir_goto_binary_loc : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_binary_loc -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1767 * _menhir_state * 'tv_binary_loc) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | ALTERNATIVE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1733) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ALTERNATE ->
            _menhir_run473 _menhir_env (Obj.magic _menhir_stack) MenhirState496
        | EOF | SEMICOLON ->
            _menhir_reduce36 _menhir_env (Obj.magic _menhir_stack) MenhirState496
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState496) : 'freshtv1734)
    | ASSUME ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1735) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState494
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState494 _v
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState494
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState494
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState494 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState494 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState494
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState494 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState494
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState494
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState494
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState494) : 'freshtv1736)
    | CONSEQUENT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1737) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ALTERNATE ->
            _menhir_run473 _menhir_env (Obj.magic _menhir_stack) MenhirState492
        | EOF | SEMICOLON ->
            _menhir_reduce36 _menhir_env (Obj.magic _menhir_stack) MenhirState492
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState492) : 'freshtv1738)
    | CUT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1741) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1739) = Obj.magic _menhir_stack in
        ((let _1 = () in
        let _v : 'tv_directive = 
# 616 "parser/parser.mly"
   ( Directive.cut )
# 3973 "parser/parser.ml"
         in
        _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv1740)) : 'freshtv1742)
    | ELSE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1743) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ALTERNATE ->
            _menhir_run473 _menhir_env (Obj.magic _menhir_stack) MenhirState489
        | EOF | SEMICOLON ->
            _menhir_reduce36 _menhir_env (Obj.magic _menhir_stack) MenhirState489
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState489) : 'freshtv1744)
    | ENUMERATE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1745) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState484
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState484 _v
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState484
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState484
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState484 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState484 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState484
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState484 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState484
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState484
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState484
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState484) : 'freshtv1746)
    | MINUS ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1747) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ALTERNATE ->
            _menhir_run473 _menhir_env (Obj.magic _menhir_stack) MenhirState482
        | EOF | SEMICOLON ->
            _menhir_reduce36 _menhir_env (Obj.magic _menhir_stack) MenhirState482
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState482) : 'freshtv1748)
    | PLUS ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1749) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ALTERNATE ->
            _menhir_run473 _menhir_env (Obj.magic _menhir_stack) MenhirState480
        | EOF | SEMICOLON ->
            _menhir_reduce36 _menhir_env (Obj.magic _menhir_stack) MenhirState480
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState480) : 'freshtv1750)
    | REACH ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1761) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1751) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BIN _v ->
                _menhir_run461 _menhir_env (Obj.magic _menhir_stack) MenhirState477 _v
            | HEXA _v ->
                _menhir_run460 _menhir_env (Obj.magic _menhir_stack) MenhirState477 _v
            | INT _v ->
                _menhir_run454 _menhir_env (Obj.magic _menhir_stack) MenhirState477 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState477) : 'freshtv1752)
        | STAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1755) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1753) = Obj.magic _menhir_stack in
            ((let _2 = () in
            let _1 = () in
            let _v : 'tv_directive = 
# 610 "parser/parser.mly"
   ( Directive.reach_all )
# 4083 "parser/parser.ml"
             in
            _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv1754)) : 'freshtv1756)
        | EOF | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1757) = Obj.magic _menhir_stack in
            ((let _1 = () in
            let _v : 'tv_directive = let times =
              let times = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 4094 "parser/parser.ml"
               in
              
# 591 "parser/parser.mly"
                                                ( times )
# 4099 "parser/parser.ml"
              
            in
            
# 608 "parser/parser.mly"
   ( let n = Utils.get_opt_or_default 1 times in Directive.reach ~n )
# 4105 "parser/parser.ml"
             in
            _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv1758)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1759) = Obj.magic _menhir_stack in
            (raise _eRR : 'freshtv1760)) : 'freshtv1762)
    | THEN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1763) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ALTERNATE ->
            _menhir_run473 _menhir_env (Obj.magic _menhir_stack) MenhirState472
        | EOF | SEMICOLON ->
            _menhir_reduce36 _menhir_env (Obj.magic _menhir_stack) MenhirState472
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState472) : 'freshtv1764)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1765 * _menhir_state * 'tv_binary_loc) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1766)) : 'freshtv1768)

and _menhir_goto_separated_nonempty_list_COMMA_expr_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_separated_nonempty_list_COMMA_expr_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState522 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1727 * _menhir_state) * _menhir_state * 'tv_separated_nonempty_list_COMMA_expr_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1723 * _menhir_state) * _menhir_state * 'tv_separated_nonempty_list_COMMA_expr_) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1721 * _menhir_state) * _menhir_state * 'tv_separated_nonempty_list_COMMA_expr_) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (args : 'tv_separated_nonempty_list_COMMA_expr_)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_interval_or_set = 
# 343 "parser/parser.mly"
  ( Initialization.Set args )
# 4158 "parser/parser.ml"
             in
            _menhir_goto_interval_or_set _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1722)) : 'freshtv1724)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1725 * _menhir_state) * _menhir_state * 'tv_separated_nonempty_list_COMMA_expr_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1726)) : 'freshtv1728)
    | MenhirState526 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1731 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_separated_nonempty_list_COMMA_expr_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1729 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_separated_nonempty_list_COMMA_expr_) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, (x : 'tv_expr)), _), _, (xs : 'tv_separated_nonempty_list_COMMA_expr_)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_separated_nonempty_list_COMMA_expr_ = 
# 243 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 4178 "parser/parser.ml"
         in
        _menhir_goto_separated_nonempty_list_COMMA_expr_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1730)) : 'freshtv1732)
    | _ ->
        _menhir_fail ()

and _menhir_goto_interval_or_set : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_interval_or_set -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (('freshtv1719 * _menhir_state * 'tv_lvalue)) * _menhir_state * 'tv_interval_or_set) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AS ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1713) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | IDENT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1709) = Obj.magic _menhir_stack in
            let (_v : (
# 62 "parser/parser.mly"
       (string)
# 4204 "parser/parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1707) = Obj.magic _menhir_stack in
            let ((id : (
# 62 "parser/parser.mly"
       (string)
# 4212 "parser/parser.ml"
            )) : (
# 62 "parser/parser.mly"
       (string)
# 4216 "parser/parser.ml"
            )) = _v in
            ((let _1 = () in
            let _v : 'tv_as_annotation = 
# 352 "parser/parser.mly"
               ( id )
# 4222 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1705) = _menhir_stack in
            let (_v : 'tv_as_annotation) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1703 * _menhir_state * 'tv_lvalue)) * _menhir_state * 'tv_interval_or_set) = Obj.magic _menhir_stack in
            let (_v : 'tv_as_annotation) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1701 * _menhir_state * 'tv_lvalue)) * _menhir_state * 'tv_interval_or_set) = Obj.magic _menhir_stack in
            let ((x : 'tv_as_annotation) : 'tv_as_annotation) = _v in
            ((let ((_menhir_stack, _menhir_s, (lvalue : 'tv_lvalue)), _, (is : 'tv_interval_or_set)) = _menhir_stack in
            let _2 = () in
            let _v : 'tv_initialization_assignment = let idopt = 
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 4238 "parser/parser.ml"
             in
            
# 359 "parser/parser.mly"
  ( Initialization.assign ?identifier:idopt lvalue is )
# 4243 "parser/parser.ml"
             in
            _menhir_goto_initialization_assignment _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1702)) : 'freshtv1704)) : 'freshtv1706)) : 'freshtv1708)) : 'freshtv1710)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1711) = Obj.magic _menhir_stack in
            (raise _eRR : 'freshtv1712)) : 'freshtv1714)
    | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1715 * _menhir_state * 'tv_lvalue)) * _menhir_state * 'tv_interval_or_set) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (lvalue : 'tv_lvalue)), _, (is : 'tv_interval_or_set)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_initialization_assignment = let idopt = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 4260 "parser/parser.ml"
         in
        
# 359 "parser/parser.mly"
  ( Initialization.assign ?identifier:idopt lvalue is )
# 4265 "parser/parser.ml"
         in
        _menhir_goto_initialization_assignment _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1716)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1717 * _menhir_state * 'tv_lvalue)) * _menhir_state * 'tv_interval_or_set) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1718)) : 'freshtv1720)

and _menhir_goto_either_DOTDOT_COMMA_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_either_DOTDOT_COMMA_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : (('freshtv1699 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_either_DOTDOT_COMMA_) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState518
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState518 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState518
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState518
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState518 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState518 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState518
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState518 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState518
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState518
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState518
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState518) : 'freshtv1700)

and _menhir_goto_directive : _menhir_env -> 'ttv_tail -> 'tv_directive -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1697 * _menhir_state * 'tv_binary_loc) = Obj.magic _menhir_stack in
    let (_v : 'tv_directive) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1695 * _menhir_state * 'tv_binary_loc) = Obj.magic _menhir_stack in
    let ((g : 'tv_directive) : 'tv_directive) = _v in
    ((let (_menhir_stack, _menhir_s, (loc : 'tv_binary_loc)) = _menhir_stack in
    let _v : 'tv_located_directive = 
# 639 "parser/parser.mly"
   ( g loc )
# 4323 "parser/parser.ml"
     in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1693) = _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_located_directive) = _v in
    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1691 * _menhir_state * 'tv_located_directive) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1685 * _menhir_state * 'tv_located_directive) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BIN _v ->
            _menhir_run461 _menhir_env (Obj.magic _menhir_stack) MenhirState468 _v
        | HEXA _v ->
            _menhir_run460 _menhir_env (Obj.magic _menhir_stack) MenhirState468 _v
        | INFER ->
            _menhir_run455 _menhir_env (Obj.magic _menhir_stack) MenhirState468
        | INT _v ->
            _menhir_run454 _menhir_env (Obj.magic _menhir_stack) MenhirState468 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState468) : 'freshtv1686)
    | EOF ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1687 * _menhir_state * 'tv_located_directive) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_located_directive)) = _menhir_stack in
        let _v : 'tv_separated_nonempty_list_SEMICOLON_located_directive_ = 
# 241 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 4360 "parser/parser.ml"
         in
        _menhir_goto_separated_nonempty_list_SEMICOLON_located_directive_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1688)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1689 * _menhir_state * 'tv_located_directive) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1690)) : 'freshtv1692)) : 'freshtv1694)) : 'freshtv1696)) : 'freshtv1698)

and _menhir_goto_rvalue : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_rvalue -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ('freshtv1683 * _menhir_state * 'tv_lvalue)) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_rvalue) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ('freshtv1681 * _menhir_state * 'tv_lvalue)) = Obj.magic _menhir_stack in
    let (_ : _menhir_state) = _menhir_s in
    let ((f_rv : 'tv_rvalue) : 'tv_rvalue) = _v in
    ((let (_menhir_stack, _menhir_s, (lvalue : 'tv_lvalue)) = _menhir_stack in
    let _2 = () in
    let _v : 'tv_assignment = 
# 329 "parser/parser.mly"
  ( f_rv lvalue unknown_successor)
# 4386 "parser/parser.ml"
     in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1679) = _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_assignment) = _v in
    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState539 | MenhirState436 | MenhirState15 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1667 * _menhir_state * 'tv_assignment) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1665 * _menhir_state * 'tv_assignment) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (assign : 'tv_assignment)) = _menhir_stack in
        let _v : 'tv_annotable_instruction = 
# 373 "parser/parser.mly"
                     ( assign )
# 4403 "parser/parser.ml"
         in
        _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1666)) : 'freshtv1668)
    | MenhirState401 | MenhirState397 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1673 * _menhir_state * 'tv_assignment) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1669 * _menhir_state * 'tv_assignment) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | AT ->
                _menhir_run145 _menhir_env (Obj.magic _menhir_stack) MenhirState401
            | IDENT _v ->
                _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState401 _v
            | EOF | LPAR ->
                _menhir_reduce198 _menhir_env (Obj.magic _menhir_stack) MenhirState401
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState401) : 'freshtv1670)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1671 * _menhir_state * 'tv_assignment) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1672)) : 'freshtv1674)
    | MenhirState511 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1677 * _menhir_state * 'tv_assignment) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1675 * _menhir_state * 'tv_assignment) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (a : 'tv_assignment)) = _menhir_stack in
        let _v : 'tv_initialization_assignment = 
# 356 "parser/parser.mly"
                             ( Initialization.from_assignment a )
# 4444 "parser/parser.ml"
         in
        _menhir_goto_initialization_assignment _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1676)) : 'freshtv1678)
    | _ ->
        _menhir_fail ()) : 'freshtv1680)) : 'freshtv1682)) : 'freshtv1684)

and _menhir_goto_option_jump_annotation_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_option_jump_annotation_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1663 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_option_jump_annotation_) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1661 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
    let (_ : _menhir_state) = _menhir_s in
    let ((tag : 'tv_option_jump_annotation_) : 'tv_option_jump_annotation_) = _v in
    ((let (_menhir_stack, _menhir_s, (e : 'tv_expr)) = _menhir_stack in
    let _v : 'tv_jump_target = 
# 302 "parser/parser.mly"
  ( match e with
    | Dba.Expr.Cst (`Constant, bv) ->
       let vaddr = Virtual_address.of_bitvector bv in
       let caddr = Dba_types.Caddress.block_start vaddr in
       let target = Dba.Jump_target.outer caddr in
       Dba.Instr.static_jump target ?tag
    | _ ->  Dba.Instr.dynamic_jump e ?tag )
# 4470 "parser/parser.ml"
     in
    _menhir_goto_jump_target _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1662)) : 'freshtv1664)

and _menhir_run201 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | CALLFLAG ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1653 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState203
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState203) : 'freshtv1654)
    | RETURNFLAG ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1657 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1655 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        let _2 = () in
        let _1 = () in
        let _v : 'tv_jump_annotation = 
# 283 "parser/parser.mly"
                               ( Dba.Return )
# 4504 "parser/parser.ml"
         in
        _menhir_goto_jump_annotation _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1656)) : 'freshtv1658)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1659 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1660)

and _menhir_goto_address_lvalue : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_address_lvalue -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState539 | MenhirState436 | MenhirState397 | MenhirState401 | MenhirState15 | MenhirState135 | MenhirState161 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1635 * _menhir_state * 'tv_address_lvalue) = Obj.magic _menhir_stack in
        (_menhir_reduce211 _menhir_env (Obj.magic _menhir_stack) : 'freshtv1636)
    | MenhirState511 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1651 * _menhir_state * 'tv_address_lvalue) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | FROM ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1643 * _menhir_state * 'tv_address_lvalue) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | FILE ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv1639 * _menhir_state * 'tv_address_lvalue)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv1637 * _menhir_state * 'tv_address_lvalue)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, (v : 'tv_address_lvalue)) = _menhir_stack in
                let _2 = () in
                let _1 = () in
                let _v : 'tv_initialization_assignment = let _2 = 
# 348 "parser/parser.mly"
            ()
# 4547 "parser/parser.ml"
                 in
                
# 357 "parser/parser.mly"
                             ( Initialization.from_store v )
# 4552 "parser/parser.ml"
                 in
                _menhir_goto_initialization_assignment _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1638)) : 'freshtv1640)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv1641 * _menhir_state * 'tv_address_lvalue)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1642)) : 'freshtv1644)
        | FROMFILE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1647 * _menhir_state * 'tv_address_lvalue) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1645 * _menhir_state * 'tv_address_lvalue) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (v : 'tv_address_lvalue)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_initialization_assignment = let _2 = 
# 347 "parser/parser.mly"
           ()
# 4573 "parser/parser.ml"
             in
            
# 357 "parser/parser.mly"
                             ( Initialization.from_store v )
# 4578 "parser/parser.ml"
             in
            _menhir_goto_initialization_assignment _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1646)) : 'freshtv1648)
        | ASSIGN | SEMICOLON ->
            _menhir_reduce211 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1649 * _menhir_state * 'tv_address_lvalue) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1650)) : 'freshtv1652)
    | _ ->
        _menhir_fail ()

and _menhir_run37 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState37
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState37 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState37
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState37
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState37 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState37 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState37
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState37 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState37
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState37
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState37
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState37

and _menhir_run42 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState42 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState42 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState42 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState42 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState42
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState42

and _menhir_run46 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState46
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState46 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState46
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState46
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState46 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState46 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState46
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState46 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState46
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState46
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState46
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState46

and _menhir_run48 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState48
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState48 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState48
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState48
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState48 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState48 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState48
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState48 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState48
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState48
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState48
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState48

and _menhir_run50 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState50
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState50 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState50
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState50
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState50 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState50 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState50
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState50 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState50
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState50
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState50
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState50

and _menhir_run52 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState52
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState52 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState52
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState52
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState52 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState52 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState52
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState52 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState52
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState52
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState52
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState52

and _menhir_run54 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState54
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState54 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState54
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState54
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState54 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState54 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState54
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState54 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState54
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState54
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState54
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState54

and _menhir_run64 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState64
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState64 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState64
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState64
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState64 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState64 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState64
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState64 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState64
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState64
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState64
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState64

and _menhir_run66 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState66
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState66 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState66
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState66
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState66 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState66 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState66
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState66 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState66
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState66
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState66
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState66

and _menhir_run56 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState56
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState56 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState56
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState56
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState56 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState56 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState56
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState56 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState56
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState56
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState56
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState56

and _menhir_run44 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState44
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState44 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState44
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState44
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState44 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState44 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState44
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState44 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState44
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState44
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState44
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState44

and _menhir_run68 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState68
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState68 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState68
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState68
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState68 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState68 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState68
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState68 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState68
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState68
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState68
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState68

and _menhir_run58 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState58
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState58 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState58
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState58
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState58 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState58 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState58
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState58 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState58
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState58
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState58
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState58

and _menhir_run60 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState60
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState60 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState60
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState60
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState60 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState60 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState60
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState60 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState60
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState60
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState60
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState60

and _menhir_run62 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState62
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState62 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState62
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState62
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState62 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState62 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState62
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState62 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState62
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState62
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState62
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState62

and _menhir_run76 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState76
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState76 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState76
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState76
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState76 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState76 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState76
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState76 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState76
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState76
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState76
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState76

and _menhir_run78 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState78
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState78 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState78
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState78
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState78 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState78 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState78
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState78 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState78
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState78
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState78
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState78

and _menhir_run70 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState70
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState70 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState70
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState70
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState70 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState70 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState70
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState70 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState70
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState70
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState70
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState70

and _menhir_run72 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState72
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState72 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState72
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState72
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState72 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState72 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState72
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState72 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState72
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState72
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState72
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState72

and _menhir_run80 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState80
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState80 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState80
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState80
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState80 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState80 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState80
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState80 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState80
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState80
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState80
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState80

and _menhir_run82 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState82
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState82 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState82
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState82
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState82 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState82 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState82
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState82 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState82
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState82
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState82
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState82

and _menhir_run84 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1631 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        let (_v : (
# 61 "parser/parser.mly"
       (string)
# 5298 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1623 * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 5309 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1619 * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 5319 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                let (_v : (
# 61 "parser/parser.mly"
       (string)
# 5324 "parser/parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RBRACE ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1615 * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 5335 "parser/parser.ml"
                    ))) * (
# 61 "parser/parser.mly"
       (string)
# 5339 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1613 * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 5346 "parser/parser.ml"
                    ))) * (
# 61 "parser/parser.mly"
       (string)
# 5350 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((((_menhir_stack, _menhir_s, (e : 'tv_expr)), _), (loff : (
# 61 "parser/parser.mly"
       (string)
# 5355 "parser/parser.ml"
                    ))), (roff : (
# 61 "parser/parser.mly"
       (string)
# 5359 "parser/parser.ml"
                    ))) = _menhir_stack in
                    let _5 = () in
                    let _3 = () in
                    let _1 = () in
                    let _v : 'tv_expr = let offs = 
# 470 "parser/parser.mly"
  ( int_of_string loff, int_of_string roff )
# 5367 "parser/parser.ml"
                     in
                    
# 490 "parser/parser.mly"
  ( let lo, hi = offs in Dba.Expr.restrict lo hi e )
# 5372 "parser/parser.ml"
                     in
                    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1614)) : 'freshtv1616)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1617 * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 5382 "parser/parser.ml"
                    ))) * (
# 61 "parser/parser.mly"
       (string)
# 5386 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _menhir_s), _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1618)) : 'freshtv1620)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1621 * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 5397 "parser/parser.ml"
                ))) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1622)) : 'freshtv1624)
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1627 * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 5406 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1625 * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 5413 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (e : 'tv_expr)), _), (boff : (
# 61 "parser/parser.mly"
       (string)
# 5418 "parser/parser.ml"
            ))) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_expr = let offs = 
# 472 "parser/parser.mly"
  ( let n = int_of_string boff in n, n)
# 5425 "parser/parser.ml"
             in
            
# 490 "parser/parser.mly"
  ( let lo, hi = offs in Dba.Expr.restrict lo hi e )
# 5430 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1626)) : 'freshtv1628)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1629 * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 5440 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1630)) : 'freshtv1632)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1633 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1634)

and _menhir_run90 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState90
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState90 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState90
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState90
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState90 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState90 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState90
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState90 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState90
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState90
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState90
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState90

and _menhir_run92 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState92
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState92 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState92
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState92
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState92 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState92 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState92
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState92 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState92
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState92
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState92
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState92

and _menhir_run94 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState94
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState94 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState94
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState94
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState94 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState94 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState94
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState94 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState94
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState94
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState94
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState94

and _menhir_run96 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState96
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState96 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState96
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState96
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState96 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState96 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState96
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState96 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState96
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState96
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState96
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState96

and _menhir_run98 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState98
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState98 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState98
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState98
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState98 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState98 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState98
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState98 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState98
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState98
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState98
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState98

and _menhir_run74 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState74
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState74 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState74
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState74
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState74 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState74 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState74
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState74 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState74
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState74
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState74
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState74

and _menhir_run39 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_expr -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState39
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState39 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState39
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState39
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState39 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState39 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState39
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState39 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState39
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState39
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState39
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState39

and _menhir_goto_constant_expr : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_constant_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState296 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1415 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | EQUAL ->
            _menhir_run360 _menhir_env (Obj.magic _menhir_stack)
        | GES ->
            _menhir_run358 _menhir_env (Obj.magic _menhir_stack)
        | GEU ->
            _menhir_run356 _menhir_env (Obj.magic _menhir_stack)
        | GTS ->
            _menhir_run354 _menhir_env (Obj.magic _menhir_stack)
        | GTU ->
            _menhir_run352 _menhir_env (Obj.magic _menhir_stack)
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1411 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            let (_v : (
# 61 "parser/parser.mly"
       (string)
# 5713 "parser/parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1409 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            let ((size : (
# 61 "parser/parser.mly"
       (string)
# 5721 "parser/parser.ml"
            )) : (
# 61 "parser/parser.mly"
       (string)
# 5725 "parser/parser.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = 
# 253 "parser/parser.mly"
   ( Dba.Expr.sext (int_of_string size) e )
# 5732 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1410)) : 'freshtv1412)
        | LBRACE ->
            _menhir_run345 _menhir_env (Obj.magic _menhir_stack)
        | LES ->
            _menhir_run343 _menhir_env (Obj.magic _menhir_stack)
        | LEU ->
            _menhir_run341 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | LTS ->
            _menhir_run339 _menhir_env (Obj.magic _menhir_stack)
        | LTU ->
            _menhir_run337 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | NEQ ->
            _menhir_run329 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1413 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1414)) : 'freshtv1416)
    | MenhirState298 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1421 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | OR | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | XOR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1417 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 517 "parser/parser.mly"
           ( Dba.Binary_op.Xor )
# 5802 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 5807 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1418)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1419 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1420)) : 'freshtv1422)
    | MenhirState300 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1425 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1423 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_constant_expr = let bop = 
# 516 "parser/parser.mly"
           ( Dba.Binary_op.And )
# 5827 "parser/parser.ml"
         in
        
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 5832 "parser/parser.ml"
         in
        _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1424)) : 'freshtv1426)
    | MenhirState303 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1431 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1427 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 532 "parser/parser.mly"
           ( Dba.Binary_op.Mult)
# 5855 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 5860 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1428)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1429 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1430)) : 'freshtv1432)
    | MenhirState305 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1437 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | OR | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1433 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 515 "parser/parser.mly"
           ( Dba.Binary_op.Or )
# 5888 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 5893 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1434)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1435 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1436)) : 'freshtv1438)
    | MenhirState307 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1443 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1439 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 533 "parser/parser.mly"
           ( Dba.Binary_op.Mult)
# 5923 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 5928 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1440)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1441 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1442)) : 'freshtv1444)
    | MenhirState309 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1449 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1445 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 532 "parser/parser.mly"
           ( Dba.Binary_op.Mult)
# 5958 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 5963 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1446)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1447 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1448)) : 'freshtv1450)
    | MenhirState311 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1455 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1451 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 534 "parser/parser.mly"
           ( Dba.Binary_op.DivU)
# 5993 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 5998 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1452)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1453 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1454)) : 'freshtv1456)
    | MenhirState313 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1461 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1457 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 535 "parser/parser.mly"
           ( Dba.Binary_op.DivS)
# 6028 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6033 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1458)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1459 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1460)) : 'freshtv1462)
    | MenhirState315 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1467 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | NEQ | RPAR | RROTATE | RSHIFTS | RSHIFTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1463 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 537 "parser/parser.mly"
           ( Dba.Binary_op.RShiftU)
# 6081 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6086 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1464)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1465 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1466)) : 'freshtv1468)
    | MenhirState317 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1473 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | NEQ | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1469 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 529 "parser/parser.mly"
           ( Dba.Binary_op.Plus )
# 6130 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6135 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1470)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1471 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1472)) : 'freshtv1474)
    | MenhirState319 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1479 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1475 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 513 "parser/parser.mly"
           ( Dba.Binary_op.ModU )
# 6165 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6170 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1476)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1477 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1478)) : 'freshtv1480)
    | MenhirState321 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1485 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1481 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 514 "parser/parser.mly"
           ( Dba.Binary_op.ModS )
# 6200 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6205 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1482)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1483 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1484)) : 'freshtv1486)
    | MenhirState323 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1491 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | NEQ | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1487 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 530 "parser/parser.mly"
           ( Dba.Binary_op.Minus )
# 6249 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6254 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1488)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1489 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1490)) : 'freshtv1492)
    | MenhirState325 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1497 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | NEQ | RPAR | RROTATE | RSHIFTS | RSHIFTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1493 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 538 "parser/parser.mly"
           ( Dba.Binary_op.RShiftS )
# 6302 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6307 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1494)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1495 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1496)) : 'freshtv1498)
    | MenhirState327 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1503 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | NEQ | RPAR | RROTATE | RSHIFTS | RSHIFTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1499 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 540 "parser/parser.mly"
           ( Dba.Binary_op.RightRotate )
# 6355 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6360 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1500)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1501 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1502)) : 'freshtv1504)
    | MenhirState329 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1509 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1505 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 520 "parser/parser.mly"
           ( Dba.Binary_op.Diff )
# 6420 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6425 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1506)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1507 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1508)) : 'freshtv1510)
    | MenhirState331 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1515 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | NEQ | RPAR | RROTATE | RSHIFTS | RSHIFTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1511 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 536 "parser/parser.mly"
           ( Dba.Binary_op.LShift )
# 6473 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6478 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1512)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1513 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1514)) : 'freshtv1516)
    | MenhirState333 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1521 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | NEQ | RPAR | RROTATE | RSHIFTS | RSHIFTU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1517 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 539 "parser/parser.mly"
           ( Dba.Binary_op.LeftRotate )
# 6526 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6531 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1518)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1519 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1520)) : 'freshtv1522)
    | MenhirState335 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1527 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | CONCAT | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1523 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 518 "parser/parser.mly"
           ( Dba.Binary_op.Concat )
# 6589 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6594 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1524)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1525 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1526)) : 'freshtv1528)
    | MenhirState337 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1533 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1529 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 522 "parser/parser.mly"
           ( Dba.Binary_op.LtU  )
# 6654 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6659 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1530)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1531 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1532)) : 'freshtv1534)
    | MenhirState339 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1539 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1535 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 526 "parser/parser.mly"
           ( Dba.Binary_op.LtS )
# 6719 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6724 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1536)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1537 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1538)) : 'freshtv1540)
    | MenhirState341 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1545 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1541 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 521 "parser/parser.mly"
           ( Dba.Binary_op.LeqU )
# 6784 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6789 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1542)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1543 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1544)) : 'freshtv1546)
    | MenhirState343 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1551 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1547 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 525 "parser/parser.mly"
           ( Dba.Binary_op.LeqS )
# 6849 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6854 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1548)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1549 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1550)) : 'freshtv1552)
    | MenhirState352 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1557 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1553 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 524 "parser/parser.mly"
           ( Dba.Binary_op.GtU )
# 6914 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6919 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1554)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1555 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1556)) : 'freshtv1558)
    | MenhirState354 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1563 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1559 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 528 "parser/parser.mly"
           ( Dba.Binary_op.GtS )
# 6979 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 6984 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1560)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1561 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1562)) : 'freshtv1564)
    | MenhirState356 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1569 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1565 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 523 "parser/parser.mly"
           ( Dba.Binary_op.GeqU )
# 7044 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 7049 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1566)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1567 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1568)) : 'freshtv1570)
    | MenhirState358 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1575 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1571 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 527 "parser/parser.mly"
           ( Dba.Binary_op.GeqS )
# 7109 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 7114 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1572)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1573 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1574)) : 'freshtv1576)
    | MenhirState360 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1581 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON | EQUAL | GES | GEU | GTS | GTU | INT _ | LBRACE | LES | LEU | LTS | LTU | NEQ | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1577 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (le : 'tv_constant_expr)), _, (re : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = let bop = 
# 519 "parser/parser.mly"
           ( Dba.Binary_op.Eq )
# 7174 "parser/parser.ml"
             in
            
# 256 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 7179 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1578)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1579 * _menhir_state * 'tv_constant_expr)) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1580)) : 'freshtv1582)
    | MenhirState295 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1589 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | EQUAL ->
            _menhir_run360 _menhir_env (Obj.magic _menhir_stack)
        | GES ->
            _menhir_run358 _menhir_env (Obj.magic _menhir_stack)
        | GEU ->
            _menhir_run356 _menhir_env (Obj.magic _menhir_stack)
        | GTS ->
            _menhir_run354 _menhir_env (Obj.magic _menhir_stack)
        | GTU ->
            _menhir_run352 _menhir_env (Obj.magic _menhir_stack)
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1585 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            let (_v : (
# 61 "parser/parser.mly"
       (string)
# 7215 "parser/parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1583 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            let ((size : (
# 61 "parser/parser.mly"
       (string)
# 7223 "parser/parser.ml"
            )) : (
# 61 "parser/parser.mly"
       (string)
# 7227 "parser/parser.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_constant_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_constant_expr = 
# 251 "parser/parser.mly"
   ( Dba.Expr.uext (int_of_string size) e )
# 7234 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1584)) : 'freshtv1586)
        | LBRACE ->
            _menhir_run345 _menhir_env (Obj.magic _menhir_stack)
        | LES ->
            _menhir_run343 _menhir_env (Obj.magic _menhir_stack)
        | LEU ->
            _menhir_run341 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | LTS ->
            _menhir_run339 _menhir_env (Obj.magic _menhir_stack)
        | LTU ->
            _menhir_run337 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | NEQ ->
            _menhir_run329 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1587 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1588)) : 'freshtv1590)
    | MenhirState294 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1597 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | EQUAL ->
            _menhir_run360 _menhir_env (Obj.magic _menhir_stack)
        | GES ->
            _menhir_run358 _menhir_env (Obj.magic _menhir_stack)
        | GEU ->
            _menhir_run356 _menhir_env (Obj.magic _menhir_stack)
        | GTS ->
            _menhir_run354 _menhir_env (Obj.magic _menhir_stack)
        | GTU ->
            _menhir_run352 _menhir_env (Obj.magic _menhir_stack)
        | LBRACE ->
            _menhir_run345 _menhir_env (Obj.magic _menhir_stack)
        | LES ->
            _menhir_run343 _menhir_env (Obj.magic _menhir_stack)
        | LEU ->
            _menhir_run341 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | LTS ->
            _menhir_run339 _menhir_env (Obj.magic _menhir_stack)
        | LTU ->
            _menhir_run337 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | NEQ ->
            _menhir_run329 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1593 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1591 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_constant_expr)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_constant_expr = 
# 254 "parser/parser.mly"
                              ( e )
# 7346 "parser/parser.ml"
             in
            _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1592)) : 'freshtv1594)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1595 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1596)) : 'freshtv1598)
    | MenhirState293 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1601 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1599 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_constant_expr)) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_constant_expr = 
# 247 "parser/parser.mly"
   ( Dba.Expr.uminus e )
# 7384 "parser/parser.ml"
         in
        _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1600)) : 'freshtv1602)
    | MenhirState292 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1605 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1603 * _menhir_state) * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_constant_expr)) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_constant_expr = 
# 249 "parser/parser.mly"
   ( Dba.Expr.lognot e )
# 7397 "parser/parser.ml"
         in
        _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1604)) : 'freshtv1606)
    | MenhirState389 | MenhirState386 | MenhirState289 | MenhirState370 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1611 * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run300 _menhir_env (Obj.magic _menhir_stack)
        | CONCAT ->
            _menhir_run335 _menhir_env (Obj.magic _menhir_stack)
        | EQUAL ->
            _menhir_run360 _menhir_env (Obj.magic _menhir_stack)
        | GES ->
            _menhir_run358 _menhir_env (Obj.magic _menhir_stack)
        | GEU ->
            _menhir_run356 _menhir_env (Obj.magic _menhir_stack)
        | GTS ->
            _menhir_run354 _menhir_env (Obj.magic _menhir_stack)
        | GTU ->
            _menhir_run352 _menhir_env (Obj.magic _menhir_stack)
        | LBRACE ->
            _menhir_run345 _menhir_env (Obj.magic _menhir_stack)
        | LES ->
            _menhir_run343 _menhir_env (Obj.magic _menhir_stack)
        | LEU ->
            _menhir_run341 _menhir_env (Obj.magic _menhir_stack)
        | LROTATE ->
            _menhir_run333 _menhir_env (Obj.magic _menhir_stack)
        | LSHIFT ->
            _menhir_run331 _menhir_env (Obj.magic _menhir_stack)
        | LTS ->
            _menhir_run339 _menhir_env (Obj.magic _menhir_stack)
        | LTU ->
            _menhir_run337 _menhir_env (Obj.magic _menhir_stack)
        | MINUS ->
            _menhir_run323 _menhir_env (Obj.magic _menhir_stack)
        | MODS ->
            _menhir_run321 _menhir_env (Obj.magic _menhir_stack)
        | MODU ->
            _menhir_run319 _menhir_env (Obj.magic _menhir_stack)
        | NEQ ->
            _menhir_run329 _menhir_env (Obj.magic _menhir_stack)
        | OR ->
            _menhir_run305 _menhir_env (Obj.magic _menhir_stack)
        | PLUS ->
            _menhir_run317 _menhir_env (Obj.magic _menhir_stack)
        | RROTATE ->
            _menhir_run327 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTS ->
            _menhir_run325 _menhir_env (Obj.magic _menhir_stack)
        | RSHIFTU ->
            _menhir_run315 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_S ->
            _menhir_run313 _menhir_env (Obj.magic _menhir_stack)
        | SLASH_U ->
            _menhir_run311 _menhir_env (Obj.magic _menhir_stack)
        | STAR ->
            _menhir_run309 _menhir_env (Obj.magic _menhir_stack)
        | STAR_S ->
            _menhir_run307 _menhir_env (Obj.magic _menhir_stack)
        | STAR_U ->
            _menhir_run303 _menhir_env (Obj.magic _menhir_stack)
        | XOR ->
            _menhir_run298 _menhir_env (Obj.magic _menhir_stack)
        | COLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1607 * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (eaddr : 'tv_constant_expr)) = _menhir_stack in
            let _v : 'tv_cond_addr = 
# 233 "parser/parser.mly"
                        ( Mk.checked_cond_expr eaddr )
# 7471 "parser/parser.ml"
             in
            _menhir_goto_cond_addr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1608)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1609 * _menhir_state * 'tv_constant_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1610)) : 'freshtv1612)
    | _ ->
        _menhir_fail ()

and _menhir_reduce122 : _menhir_env -> 'ttv_tail * _menhir_state * 'tv_constant -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let (_menhir_stack, _menhir_s, (cst : 'tv_constant)) = _menhir_stack in
    let _v : 'tv_expr = 
# 487 "parser/parser.mly"
  ( Dba.Expr.constant cst )
# 7490 "parser/parser.ml"
     in
    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v

and _menhir_run10 : _menhir_env -> ('ttv_tail * _menhir_state) * _menhir_state * 'tv_constant -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1405 * _menhir_state) * _menhir_state * 'tv_constant)) = Obj.magic _menhir_stack in
        let (_v : (
# 61 "parser/parser.mly"
       (string)
# 7505 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1401 * _menhir_state) * _menhir_state * 'tv_constant)) * (
# 61 "parser/parser.mly"
       (string)
# 7516 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1399 * _menhir_state) * _menhir_state * 'tv_constant)) * (
# 61 "parser/parser.mly"
       (string)
# 7523 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s), _, (bv : 'tv_constant)), (x : (
# 61 "parser/parser.mly"
       (string)
# 7528 "parser/parser.ml"
            ))) = _menhir_stack in
            let _4 = () in
            let _1_inlined1 = () in
            let _1 = () in
            let _v : 'tv_address = let nid =
              let _1 = _1_inlined1 in
              
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 7538 "parser/parser.ml"
              
            in
            
# 561 "parser/parser.mly"
  (
    let id = int_of_string nid in
    let vaddr = Virtual_address.of_bitvector bv in
    let addr = Dba_types.Caddress.create vaddr id in
    incr_address addr;
    addr
 )
# 7550 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1397) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_address) = _v in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
            match _menhir_s with
            | MenhirState432 | MenhirState398 | MenhirState0 | MenhirState13 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1373 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
                ((assert (not _menhir_env._menhir_error);
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | ASSERT ->
                    _menhir_run223 _menhir_env (Obj.magic _menhir_stack) MenhirState15
                | ASSUME ->
                    _menhir_run215 _menhir_env (Obj.magic _menhir_stack) MenhirState15
                | AT ->
                    _menhir_run145 _menhir_env (Obj.magic _menhir_stack) MenhirState15
                | FREE ->
                    _menhir_run211 _menhir_env (Obj.magic _menhir_stack) MenhirState15
                | GOTO ->
                    _menhir_run196 _menhir_env (Obj.magic _menhir_stack) MenhirState15
                | IDENT _v ->
                    _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState15 _v
                | IF ->
                    _menhir_run175 _menhir_env (Obj.magic _menhir_stack) MenhirState15
                | NONDETASSUME ->
                    _menhir_run133 _menhir_env (Obj.magic _menhir_stack) MenhirState15
                | PRINT ->
                    _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState15
                | STOP ->
                    _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState15
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState15) : 'freshtv1374)
            | MenhirState196 | MenhirState191 | MenhirState185 | MenhirState177 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1377 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1375 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, (addr : 'tv_address)) = _menhir_stack in
                let _v : 'tv_static_target = 
# 288 "parser/parser.mly"
                ( Dba.Jump_target.outer addr )
# 7597 "parser/parser.ml"
                 in
                _menhir_goto_static_target _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1376)) : 'freshtv1378)
            | MenhirState203 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1381 * _menhir_state)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1379 * _menhir_state)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _, (addr : 'tv_address)) = _menhir_stack in
                let _2 = () in
                let _1 = () in
                let _v : 'tv_jump_annotation = 
# 282 "parser/parser.mly"
  ( Dba.Call addr )
# 7611 "parser/parser.ml"
                 in
                _menhir_goto_jump_annotation _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1380)) : 'freshtv1382)
            | MenhirState261 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1395)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv1393)) * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _, (addr : 'tv_address)) = _menhir_stack in
                let _2 = () in
                let _1 = () in
                let _v : 'tv_entry = 
# 163 "parser/parser.mly"
                                  ( addr )
# 7625 "parser/parser.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1391) = _menhir_stack in
                let (_v : 'tv_entry) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1389) = Obj.magic _menhir_stack in
                let (_v : 'tv_entry) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1387) = Obj.magic _menhir_stack in
                let ((entry : 'tv_entry) : 'tv_entry) = _v in
                ((let _v : 'tv_config = 
# 159 "parser/parser.mly"
                                          ( entry )
# 7639 "parser/parser.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1385) = _menhir_stack in
                let (_v : 'tv_config) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1383 * 'tv_config) = Obj.magic _menhir_stack in
                ((assert (not _menhir_env._menhir_error);
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | FLAG ->
                    _menhir_run281 _menhir_env (Obj.magic _menhir_stack) MenhirState265
                | REGISTER ->
                    _menhir_run277 _menhir_env (Obj.magic _menhir_stack) MenhirState265
                | TEMPORARY ->
                    _menhir_run273 _menhir_env (Obj.magic _menhir_stack) MenhirState265
                | VAR ->
                    _menhir_run266 _menhir_env (Obj.magic _menhir_stack) MenhirState265
                | AT | BEGIN | EOF | IDENT _ | LPAR ->
                    _menhir_reduce200 _menhir_env (Obj.magic _menhir_stack) MenhirState265
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState265) : 'freshtv1384)) : 'freshtv1386)) : 'freshtv1388)) : 'freshtv1390)) : 'freshtv1392)) : 'freshtv1394)) : 'freshtv1396)
            | _ ->
                _menhir_fail ()) : 'freshtv1398)) : 'freshtv1400)) : 'freshtv1402)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1403 * _menhir_state) * _menhir_state * 'tv_constant)) * (
# 61 "parser/parser.mly"
       (string)
# 7673 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1404)) : 'freshtv1406)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1407 * _menhir_state) * _menhir_state * 'tv_constant)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1408)

and _menhir_goto_explicit_instruction : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_explicit_instruction -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1371) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_explicit_instruction) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1369) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((instr : 'tv_explicit_instruction) : 'tv_explicit_instruction) = _v in
    ((let _v : 'tv_instruction = 
# 400 "parser/parser.mly"
                              ( instr )
# 7698 "parser/parser.ml"
     in
    _menhir_goto_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1370)) : 'freshtv1372)

and _menhir_goto_printarg : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_printarg -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1367 * _menhir_state * 'tv_printarg) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COMMA ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1361 * _menhir_state * 'tv_printarg) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState130
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState130 _v
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState130
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState130
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState130 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState130 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState130
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState130 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState130
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState130
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState130
        | STRING _v ->
            _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState130 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState130) : 'freshtv1362)
    | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1363 * _menhir_state * 'tv_printarg) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_printarg)) = _menhir_stack in
        let _v : 'tv_separated_nonempty_list_COMMA_printarg_ = 
# 241 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 7751 "parser/parser.ml"
         in
        _menhir_goto_separated_nonempty_list_COMMA_printarg_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1364)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1365 * _menhir_state * 'tv_printarg) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1366)) : 'freshtv1368)

and _menhir_goto_lvalue : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_lvalue -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState161 | MenhirState135 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1341 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1335 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | AT ->
                _menhir_run145 _menhir_env (Obj.magic _menhir_stack) MenhirState161
            | IDENT _v ->
                _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState161 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState161) : 'freshtv1336)
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1337 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (x : 'tv_lvalue)) = _menhir_stack in
            let _v : 'tv_separated_nonempty_list_COMMA_lvalue_ = 
# 241 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 7793 "parser/parser.ml"
             in
            _menhir_goto_separated_nonempty_list_COMMA_lvalue_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1338)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1339 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1340)) : 'freshtv1342)
    | MenhirState539 | MenhirState436 | MenhirState401 | MenhirState397 | MenhirState15 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1347 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ASSIGN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1343 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | AT ->
                _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | BIN _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState232 _v
            | EXTS ->
                _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | EXTU ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | HEXA _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState232 _v
            | IDENT _v ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState232 _v
            | IF ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | INT _v ->
                _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState232 _v
            | LPAR ->
                _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | MALLOC ->
                _menhir_run243 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | MINUS ->
                _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | NONDET ->
                _menhir_run235 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | NOT ->
                _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | UNCONTROLLED ->
                _menhir_run234 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | UNDEF ->
                _menhir_run233 _menhir_env (Obj.magic _menhir_stack) MenhirState232
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState232) : 'freshtv1344)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1345 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1346)) : 'freshtv1348)
    | MenhirState511 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1359 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ASSIGN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1353 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | AT ->
                _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | BIN _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState513 _v
            | EXTS ->
                _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | EXTU ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | HEXA _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState513 _v
            | IDENT _v ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState513 _v
            | IF ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | INT _v ->
                _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState513 _v
            | LBRACE ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1349) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState513 in
                ((let _menhir_stack = (_menhir_stack, _menhir_s) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | AT ->
                    _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState522
                | BIN _v ->
                    _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState522 _v
                | EXTS ->
                    _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState522
                | EXTU ->
                    _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState522
                | HEXA _v ->
                    _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState522 _v
                | IDENT _v ->
                    _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState522 _v
                | IF ->
                    _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState522
                | INT _v ->
                    _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState522 _v
                | LPAR ->
                    _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState522
                | MINUS ->
                    _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState522
                | NOT ->
                    _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState522
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState522) : 'freshtv1350)
            | LBRACKET ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1351) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState513 in
                ((let _menhir_stack = (_menhir_stack, _menhir_s) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | AT ->
                    _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState514
                | BIN _v ->
                    _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState514 _v
                | EXTS ->
                    _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState514
                | EXTU ->
                    _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState514
                | HEXA _v ->
                    _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState514 _v
                | IDENT _v ->
                    _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState514 _v
                | IF ->
                    _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState514
                | INT _v ->
                    _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState514 _v
                | LPAR ->
                    _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState514
                | MINUS ->
                    _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState514
                | NOT ->
                    _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState514
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState514) : 'freshtv1352)
            | LPAR ->
                _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | MALLOC ->
                _menhir_run243 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | MINUS ->
                _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | NONDET ->
                _menhir_run235 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | NOT ->
                _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | UNCONTROLLED ->
                _menhir_run234 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | UNDEF ->
                _menhir_run233 _menhir_env (Obj.magic _menhir_stack) MenhirState513
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState513) : 'freshtv1354)
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1355 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (lv : 'tv_lvalue)) = _menhir_stack in
            let _v : 'tv_initialization_assignment = 
# 360 "parser/parser.mly"
              ( Initialization.universal lv )
# 7977 "parser/parser.ml"
             in
            _menhir_goto_initialization_assignment _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1356)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1357 * _menhir_state * 'tv_lvalue) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1358)) : 'freshtv1360)
    | _ ->
        _menhir_fail ()

and _menhir_goto_static_target : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_static_target -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState177 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1297 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ELSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1293 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | GOTO ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv1289 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | INT _v ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv1285 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target))) = Obj.magic _menhir_stack in
                    let (_v : (
# 61 "parser/parser.mly"
       (string)
# 8018 "parser/parser.ml"
                    )) = _v in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv1283 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target))) = Obj.magic _menhir_stack in
                    let ((next : (
# 61 "parser/parser.mly"
       (string)
# 8026 "parser/parser.ml"
                    )) : (
# 61 "parser/parser.mly"
       (string)
# 8030 "parser/parser.ml"
                    )) = _v in
                    ((let (((_menhir_stack, _menhir_s), _), _, (st : 'tv_static_target)) = _menhir_stack in
                    let _6 = () in
                    let _5 = () in
                    let _3 = () in
                    let _1_inlined1 = () in
                    let _1 = () in
                    let _v : 'tv_explicit_instruction = let condition =
                      let _1 = _1_inlined1 in
                      
# 544 "parser/parser.mly"
            ( Dba.Expr.one )
# 8043 "parser/parser.ml"
                      
                    in
                    
# 392 "parser/parser.mly"
  ( Dba.Instr.ite condition st (int_of_string next) )
# 8049 "parser/parser.ml"
                     in
                    _menhir_goto_explicit_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1284)) : 'freshtv1286)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv1287 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target))) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1288)) : 'freshtv1290)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv1291 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1292)) : 'freshtv1294)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1295 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1296)) : 'freshtv1298)
    | MenhirState185 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1313 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ELSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1309 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | GOTO ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv1305 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | INT _v ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv1301 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target))) = Obj.magic _menhir_stack in
                    let (_v : (
# 61 "parser/parser.mly"
       (string)
# 8097 "parser/parser.ml"
                    )) = _v in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv1299 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target))) = Obj.magic _menhir_stack in
                    let ((next : (
# 61 "parser/parser.mly"
       (string)
# 8105 "parser/parser.ml"
                    )) : (
# 61 "parser/parser.mly"
       (string)
# 8109 "parser/parser.ml"
                    )) = _v in
                    ((let (((_menhir_stack, _menhir_s), _), _, (st : 'tv_static_target)) = _menhir_stack in
                    let _6 = () in
                    let _5 = () in
                    let _3 = () in
                    let _1_inlined1 = () in
                    let _1 = () in
                    let _v : 'tv_explicit_instruction = let condition =
                      let _1 = _1_inlined1 in
                      
# 545 "parser/parser.mly"
            ( Dba.Expr.zero )
# 8122 "parser/parser.ml"
                      
                    in
                    
# 392 "parser/parser.mly"
  ( Dba.Instr.ite condition st (int_of_string next) )
# 8128 "parser/parser.ml"
                     in
                    _menhir_goto_explicit_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1300)) : 'freshtv1302)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv1303 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target))) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1304)) : 'freshtv1306)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv1307 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1308)) : 'freshtv1310)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1311 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1312)) : 'freshtv1314)
    | MenhirState191 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1329 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ELSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1325 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | GOTO ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv1321 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_static_target)) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | INT _v ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv1317 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_static_target))) = Obj.magic _menhir_stack in
                    let (_v : (
# 61 "parser/parser.mly"
       (string)
# 8176 "parser/parser.ml"
                    )) = _v in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv1315 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_static_target))) = Obj.magic _menhir_stack in
                    let ((next : (
# 61 "parser/parser.mly"
       (string)
# 8184 "parser/parser.ml"
                    )) : (
# 61 "parser/parser.mly"
       (string)
# 8188 "parser/parser.ml"
                    )) = _v in
                    ((let ((((_menhir_stack, _menhir_s), _, (e : 'tv_expr)), _), _, (st : 'tv_static_target)) = _menhir_stack in
                    let _6 = () in
                    let _5 = () in
                    let _3 = () in
                    let _1 = () in
                    let _v : 'tv_explicit_instruction = let condition = 
# 546 "parser/parser.mly"
            ( e )
# 8198 "parser/parser.ml"
                     in
                    
# 392 "parser/parser.mly"
  ( Dba.Instr.ite condition st (int_of_string next) )
# 8203 "parser/parser.ml"
                     in
                    _menhir_goto_explicit_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1316)) : 'freshtv1318)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv1319 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_static_target))) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1320)) : 'freshtv1322)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv1323 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_static_target)) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1324)) : 'freshtv1326)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1327 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1328)) : 'freshtv1330)
    | MenhirState196 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1333 * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ANNOT ->
            _menhir_run201 _menhir_env (Obj.magic _menhir_stack) MenhirState200
        | EOF | HEXA _ | INT _ | LPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1331 * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (t : 'tv_static_target)) = _menhir_stack in
            let _v : 'tv_static_jump = let tag = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 8242 "parser/parser.ml"
             in
            
# 295 "parser/parser.mly"
  ( Dba.Instr.static_jump t ?tag  )
# 8247 "parser/parser.ml"
             in
            _menhir_goto_static_jump _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1332)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState200) : 'freshtv1334)
    | _ ->
        _menhir_fail ()

and _menhir_goto_instruction : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_instruction -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState15 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1259 * _menhir_state * 'tv_address) * _menhir_state * 'tv_instruction) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1257 * _menhir_state * 'tv_address) * _menhir_state * 'tv_instruction) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (addr : 'tv_address)), _, (instr : 'tv_instruction)) = _menhir_stack in
        let _v : 'tv_localized_instruction = 
# 273 "parser/parser.mly"
  ( Mk.checked_localized_instruction addr instr )
# 8270 "parser/parser.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1255) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_localized_instruction) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1253 * _menhir_state * 'tv_localized_instruction) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState13
        | EOF ->
            _menhir_reduce192 _menhir_env (Obj.magic _menhir_stack) MenhirState13
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState13) : 'freshtv1254)) : 'freshtv1256)) : 'freshtv1258)) : 'freshtv1260)
    | MenhirState436 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1267 * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 8295 "parser/parser.ml"
        ))) * _menhir_state * 'tv_instruction) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1265 * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 8301 "parser/parser.ml"
        ))) * _menhir_state * 'tv_instruction) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (label : (
# 61 "parser/parser.mly"
       (string)
# 8306 "parser/parser.ml"
        ))), _, (instr : 'tv_instruction)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_labelled_instruction = 
# 407 "parser/parser.mly"
                                       ( (int_of_string label,instr))
# 8312 "parser/parser.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1263) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_labelled_instruction) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1261 * _menhir_state * 'tv_labelled_instruction) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            _menhir_run435 _menhir_env (Obj.magic _menhir_stack) MenhirState439 _v
        | EOF | HEXA _ ->
            _menhir_reduce190 _menhir_env (Obj.magic _menhir_stack) MenhirState439
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState439) : 'freshtv1262)) : 'freshtv1264)) : 'freshtv1266)) : 'freshtv1268)
    | MenhirState539 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1281 * _menhir_state * 'tv_instruction) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1277 * _menhir_state * 'tv_instruction) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1275 * _menhir_state * 'tv_instruction) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (l : 'tv_instruction)) = _menhir_stack in
            let _2 = () in
            let _v : (
# 91 "parser/parser.mly"
      (Dba.Instr.t)
# 8348 "parser/parser.ml"
            ) = 
# 404 "parser/parser.mly"
                     (l)
# 8352 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1273) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 91 "parser/parser.mly"
      (Dba.Instr.t)
# 8360 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1271) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 91 "parser/parser.mly"
      (Dba.Instr.t)
# 8368 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1269) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let ((_1 : (
# 91 "parser/parser.mly"
      (Dba.Instr.t)
# 8376 "parser/parser.ml"
            )) : (
# 91 "parser/parser.mly"
      (Dba.Instr.t)
# 8380 "parser/parser.ml"
            )) = _v in
            (Obj.magic _1 : 'freshtv1270)) : 'freshtv1272)) : 'freshtv1274)) : 'freshtv1276)) : 'freshtv1278)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1279 * _menhir_state * 'tv_instruction) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1280)) : 'freshtv1282)
    | _ ->
        _menhir_fail ()

and _menhir_fail : unit -> 'a =
  fun () ->
    Printf.fprintf stderr "Internal failure -- please contact the parser generator's developers.\n%!";
    assert false

and _menhir_goto_list_localized_instruction_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_localized_instruction_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState13 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1217 * _menhir_state * 'tv_localized_instruction) * _menhir_state * 'tv_list_localized_instruction_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1215 * _menhir_state * 'tv_localized_instruction) * _menhir_state * 'tv_list_localized_instruction_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_localized_instruction)), _, (xs : 'tv_list_localized_instruction_)) = _menhir_stack in
        let _v : 'tv_list_localized_instruction_ = 
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 8411 "parser/parser.ml"
         in
        _menhir_goto_list_localized_instruction_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1216)) : 'freshtv1218)
    | MenhirState432 | MenhirState398 | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1251 * _menhir_state * 'tv_list_localized_instruction_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1247 * _menhir_state * 'tv_list_localized_instruction_) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1245 * _menhir_state * 'tv_list_localized_instruction_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (b : 'tv_list_localized_instruction_)) = _menhir_stack in
            let _2 = () in
            let _v : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8430 "parser/parser.ml"
            ) = 
# 155 "parser/parser.mly"
                                     ( b )
# 8434 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1243) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8442 "parser/parser.ml"
            )) = _v in
            ((match _menhir_s with
            | MenhirState0 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1221) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8452 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1219) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let ((_1 : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8460 "parser/parser.ml"
                )) : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8464 "parser/parser.ml"
                )) = _v in
                (Obj.magic _1 : 'freshtv1220)) : 'freshtv1222)
            | MenhirState398 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1231 * 'tv_config) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) * 'tv_option_permission_block_) * _menhir_state * 'tv_list_terminated_assignment_SEMICOLON__) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8474 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1229 * 'tv_config) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) * 'tv_option_permission_block_) * _menhir_state * 'tv_list_terminated_assignment_SEMICOLON__) = Obj.magic _menhir_stack in
                let (_ : _menhir_state) = _menhir_s in
                let ((instructions : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8482 "parser/parser.ml"
                )) : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8486 "parser/parser.ml"
                )) = _v in
                ((let ((((_menhir_stack, (config : 'tv_config)), _, (decls : 'tv_list_terminated_declaration_SEMICOLON__)), (permissions : 'tv_option_permission_block_)), _, (initialization : 'tv_list_terminated_assignment_SEMICOLON__)) = _menhir_stack in
                let _v : (
# 85 "parser/parser.mly"
      ('a Dba_types.program)
# 8492 "parser/parser.ml"
                ) = 
# 130 "parser/parser.mly"
   ( Mk.program permissions initialization config decls instructions )
# 8496 "parser/parser.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1227) = _menhir_stack in
                let (_v : (
# 85 "parser/parser.mly"
      ('a Dba_types.program)
# 8503 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1225) = Obj.magic _menhir_stack in
                let (_v : (
# 85 "parser/parser.mly"
      ('a Dba_types.program)
# 8510 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1223) = Obj.magic _menhir_stack in
                let ((_1 : (
# 85 "parser/parser.mly"
      ('a Dba_types.program)
# 8517 "parser/parser.ml"
                )) : (
# 85 "parser/parser.mly"
      ('a Dba_types.program)
# 8521 "parser/parser.ml"
                )) = _v in
                (Obj.magic _1 : 'freshtv1224)) : 'freshtv1226)) : 'freshtv1228)) : 'freshtv1230)) : 'freshtv1232)
            | MenhirState432 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1241 * _menhir_state * (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 8529 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8535 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1239 * _menhir_state * (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 8541 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                let (_ : _menhir_state) = _menhir_s in
                let ((instructions : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8547 "parser/parser.ml"
                )) : (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8551 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_stack, _menhir_s, (base : (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 8556 "parser/parser.ml"
                ))) = _menhir_stack in
                let _v : (
# 100 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list * (Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8561 "parser/parser.ml"
                ) = 
# 151 "parser/parser.mly"
  ( base, instructions )
# 8565 "parser/parser.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1237) = _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : (
# 100 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list * (Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8573 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1235) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : (
# 100 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list * (Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8581 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1233) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let ((_1 : (
# 100 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list * (Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8589 "parser/parser.ml"
                )) : (
# 100 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list * (Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 8593 "parser/parser.ml"
                )) = _v in
                (Obj.magic _1 : 'freshtv1234)) : 'freshtv1236)) : 'freshtv1238)) : 'freshtv1240)) : 'freshtv1242)
            | _ ->
                _menhir_fail ()) : 'freshtv1244)) : 'freshtv1246)) : 'freshtv1248)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1249 * _menhir_state * 'tv_list_localized_instruction_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1250)) : 'freshtv1252)
    | _ ->
        _menhir_fail ()

and _menhir_run410 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 65 "parser/parser.mly"
       (string)
# 8611 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1213) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 65 "parser/parser.mly"
       (string)
# 8621 "parser/parser.ml"
    )) : (
# 65 "parser/parser.mly"
       (string)
# 8625 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_value = 
# 136 "parser/parser.mly"
           ( Message.Value.vstr _1)
# 8630 "parser/parser.ml"
     in
    _menhir_goto_value _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1214)

and _menhir_run411 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 61 "parser/parser.mly"
       (string)
# 8637 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1211) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 61 "parser/parser.mly"
       (string)
# 8647 "parser/parser.ml"
    )) : (
# 61 "parser/parser.mly"
       (string)
# 8651 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_value = 
# 137 "parser/parser.mly"
           ( Message.Value.vint _1)
# 8656 "parser/parser.ml"
     in
    _menhir_goto_value _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1212)

and _menhir_run412 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 63 "parser/parser.mly"
       (string)
# 8663 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1209) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 63 "parser/parser.mly"
       (string)
# 8673 "parser/parser.ml"
    )) : (
# 63 "parser/parser.mly"
       (string)
# 8677 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_value = 
# 134 "parser/parser.mly"
           ( Message.Value.vhex _1)
# 8682 "parser/parser.ml"
     in
    _menhir_goto_value _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1210)

and _menhir_run413 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 64 "parser/parser.mly"
       (string)
# 8689 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1207) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 64 "parser/parser.mly"
       (string)
# 8699 "parser/parser.ml"
    )) : (
# 64 "parser/parser.mly"
       (string)
# 8703 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_value = 
# 135 "parser/parser.mly"
           ( Message.Value.vbin _1)
# 8708 "parser/parser.ml"
     in
    _menhir_goto_value _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1208)

and _menhir_goto_list_labelled_instruction_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_labelled_instruction_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState446 | MenhirState434 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1201) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_list_labelled_instruction_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1199) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((l : 'tv_list_labelled_instruction_) : 'tv_list_labelled_instruction_) = _v in
        ((let _v : 'tv_dhunk = 
# 410 "parser/parser.mly"
                                (Dhunk.of_labelled_list l)
# 8727 "parser/parser.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1197) = _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_dhunk) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        match _menhir_s with
        | MenhirState434 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1187 * _menhir_state * 'tv_dhunk) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EOF ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1183 * _menhir_state * 'tv_dhunk) = Obj.magic _menhir_stack in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1181 * _menhir_state * 'tv_dhunk) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, (l : 'tv_dhunk)) = _menhir_stack in
                let _2 = () in
                let _v : (
# 88 "parser/parser.mly"
      (Dhunk.t)
# 8751 "parser/parser.ml"
                ) = 
# 414 "parser/parser.mly"
               ( l )
# 8755 "parser/parser.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1179) = _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : (
# 88 "parser/parser.mly"
      (Dhunk.t)
# 8763 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1177) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : (
# 88 "parser/parser.mly"
      (Dhunk.t)
# 8771 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1175) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let ((_1 : (
# 88 "parser/parser.mly"
      (Dhunk.t)
# 8779 "parser/parser.ml"
                )) : (
# 88 "parser/parser.mly"
      (Dhunk.t)
# 8783 "parser/parser.ml"
                )) = _v in
                (Obj.magic _1 : 'freshtv1176)) : 'freshtv1178)) : 'freshtv1180)) : 'freshtv1182)) : 'freshtv1184)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv1185 * _menhir_state * 'tv_dhunk) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1186)) : 'freshtv1188)
        | MenhirState446 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1195 * _menhir_state * (
# 63 "parser/parser.mly"
       (string)
# 8798 "parser/parser.ml"
            ))) * _menhir_state * 'tv_dhunk) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1193 * _menhir_state * (
# 63 "parser/parser.mly"
       (string)
# 8804 "parser/parser.ml"
            ))) * _menhir_state * 'tv_dhunk) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, (addr : (
# 63 "parser/parser.mly"
       (string)
# 8809 "parser/parser.ml"
            ))), _, (dh : 'tv_dhunk)) = _menhir_stack in
            let _2 = () in
            let _v : 'tv_dhunk_substitution = 
# 419 "parser/parser.mly"
  ( (Virtual_address.of_bitvector @@ Bitvector.of_string addr, dh))
# 8815 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1191) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_dhunk_substitution) = _v in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1189 * _menhir_state * 'tv_dhunk_substitution) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | HEXA _v ->
                _menhir_run445 _menhir_env (Obj.magic _menhir_stack) MenhirState451 _v
            | EOF ->
                _menhir_reduce188 _menhir_env (Obj.magic _menhir_stack) MenhirState451
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState451) : 'freshtv1190)) : 'freshtv1192)) : 'freshtv1194)) : 'freshtv1196)
        | _ ->
            _menhir_fail ()) : 'freshtv1198)) : 'freshtv1200)) : 'freshtv1202)
    | MenhirState439 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1205 * _menhir_state * 'tv_labelled_instruction) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_list_labelled_instruction_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1203 * _menhir_state * 'tv_labelled_instruction) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_list_labelled_instruction_) : 'tv_list_labelled_instruction_) = _v in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_labelled_instruction)) = _menhir_stack in
        let _v : 'tv_list_labelled_instruction_ = 
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 8850 "parser/parser.ml"
         in
        _menhir_goto_list_labelled_instruction_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1204)) : 'freshtv1206)
    | _ ->
        _menhir_fail ()

and _menhir_goto_list_dhunk_substitution_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_dhunk_substitution_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState444 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1169 * _menhir_state * 'tv_list_dhunk_substitution_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1165 * _menhir_state * 'tv_list_dhunk_substitution_) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1163 * _menhir_state * 'tv_list_dhunk_substitution_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (l : 'tv_list_dhunk_substitution_)) = _menhir_stack in
            let _2 = () in
            let _v : (
# 94 "parser/parser.mly"
      ((Virtual_address.t * Dhunk.t) list)
# 8876 "parser/parser.ml"
            ) = 
# 423 "parser/parser.mly"
                                  (l)
# 8880 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1161) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 94 "parser/parser.mly"
      ((Virtual_address.t * Dhunk.t) list)
# 8888 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1159) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 94 "parser/parser.mly"
      ((Virtual_address.t * Dhunk.t) list)
# 8896 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1157) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let ((_1 : (
# 94 "parser/parser.mly"
      ((Virtual_address.t * Dhunk.t) list)
# 8904 "parser/parser.ml"
            )) : (
# 94 "parser/parser.mly"
      ((Virtual_address.t * Dhunk.t) list)
# 8908 "parser/parser.ml"
            )) = _v in
            (Obj.magic _1 : 'freshtv1158)) : 'freshtv1160)) : 'freshtv1162)) : 'freshtv1164)) : 'freshtv1166)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1167 * _menhir_state * 'tv_list_dhunk_substitution_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1168)) : 'freshtv1170)
    | MenhirState451 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1173 * _menhir_state * 'tv_dhunk_substitution) * _menhir_state * 'tv_list_dhunk_substitution_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1171 * _menhir_state * 'tv_dhunk_substitution) * _menhir_state * 'tv_list_dhunk_substitution_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_dhunk_substitution)), _, (xs : 'tv_list_dhunk_substitution_)) = _menhir_stack in
        let _v : 'tv_list_dhunk_substitution_ = 
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 8927 "parser/parser.ml"
         in
        _menhir_goto_list_dhunk_substitution_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1172)) : 'freshtv1174)
    | _ ->
        _menhir_fail ()

and _menhir_goto_bin_loc_with_id : _menhir_env -> 'ttv_tail -> 'tv_bin_loc_with_id -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_stack = (_menhir_stack, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ('freshtv1155 * _menhir_state) * 'tv_bin_loc_with_id) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | MINUS ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1145 * 'tv_bin_loc_with_id) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BIN _v ->
            _menhir_run461 _menhir_env (Obj.magic _menhir_stack) MenhirState463 _v
        | HEXA _v ->
            _menhir_run460 _menhir_env (Obj.magic _menhir_stack) MenhirState463 _v
        | INT _v ->
            _menhir_run454 _menhir_env (Obj.magic _menhir_stack) MenhirState463 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState463) : 'freshtv1146)
    | PLUS ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1147 * 'tv_bin_loc_with_id) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BIN _v ->
            _menhir_run461 _menhir_env (Obj.magic _menhir_stack) MenhirState459 _v
        | HEXA _v ->
            _menhir_run460 _menhir_env (Obj.magic _menhir_stack) MenhirState459 _v
        | INT _v ->
            _menhir_run454 _menhir_env (Obj.magic _menhir_stack) MenhirState459 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState459) : 'freshtv1148)
    | SUPER ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1151 * _menhir_state) * 'tv_bin_loc_with_id) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1149 * _menhir_state) * 'tv_bin_loc_with_id) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), (x : 'tv_bin_loc_with_id)) = _menhir_stack in
        let _3 = () in
        let _1 = () in
        let _v : 'tv_binary_loc = let loc = 
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 8985 "parser/parser.ml"
         in
        
# 634 "parser/parser.mly"
                                                 ( loc )
# 8990 "parser/parser.ml"
         in
        _menhir_goto_binary_loc _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1150)) : 'freshtv1152)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1153 * _menhir_state) * 'tv_bin_loc_with_id) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1154)) : 'freshtv1156)

and _menhir_goto_integer : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_integer -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState459 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1105 * 'tv_bin_loc_with_id)) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1103 * 'tv_bin_loc_with_id)) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, (bloc : 'tv_bin_loc_with_id)), _, (n : 'tv_integer)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_bin_loc_with_id = 
# 626 "parser/parser.mly"
    ( Loader_utils.Binary_loc.offset n bloc )
# 9015 "parser/parser.ml"
         in
        _menhir_goto_bin_loc_with_id _menhir_env _menhir_stack _v) : 'freshtv1104)) : 'freshtv1106)
    | MenhirState463 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1109 * 'tv_bin_loc_with_id)) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1107 * 'tv_bin_loc_with_id)) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, (bloc : 'tv_bin_loc_with_id)), _, (n : 'tv_integer)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_bin_loc_with_id = 
# 628 "parser/parser.mly"
    ( Loader_utils.Binary_loc.offset (-n) bloc )
# 9028 "parser/parser.ml"
         in
        _menhir_goto_bin_loc_with_id _menhir_env _menhir_stack _v) : 'freshtv1108)) : 'freshtv1110)
    | MenhirState453 | MenhirState468 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1113 * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1111 * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (address : 'tv_integer)) = _menhir_stack in
        let _v : 'tv_binary_loc = 
# 633 "parser/parser.mly"
    ( Loader_utils.Binary_loc.address @@ Virtual_address.create address )
# 9040 "parser/parser.ml"
         in
        _menhir_goto_binary_loc _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1112)) : 'freshtv1114)
    | MenhirState477 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1121)) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1117)) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1115)) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, (x : 'tv_integer)) = _menhir_stack in
            let _3 = () in
            let _1_inlined1 = () in
            let _1 = () in
            let _v : 'tv_directive = let times =
              let _1 = _1_inlined1 in
              let times =
                let x = 
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 9065 "parser/parser.ml"
                 in
                
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 9070 "parser/parser.ml"
                
              in
              
# 591 "parser/parser.mly"
                                                ( times )
# 9076 "parser/parser.ml"
              
            in
            
# 608 "parser/parser.mly"
   ( let n = Utils.get_opt_or_default 1 times in Directive.reach ~n )
# 9082 "parser/parser.ml"
             in
            _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv1116)) : 'freshtv1118)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1119)) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1120)) : 'freshtv1122)
    | MenhirState486 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1129) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1125) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1123) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _, (e : 'tv_expr)), _), _, (x : 'tv_integer)) = _menhir_stack in
            let _3 = () in
            let _1_inlined1 = () in
            let _1 = () in
            let _v : 'tv_directive = let times =
              let _1 = _1_inlined1 in
              let times =
                let x = 
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 9114 "parser/parser.ml"
                 in
                
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 9119 "parser/parser.ml"
                
              in
              
# 591 "parser/parser.mly"
                                                ( times )
# 9125 "parser/parser.ml"
              
            in
            
# 612 "parser/parser.mly"
   ( let n = Utils.get_opt_or_default 1 times in Directive.enumerate ~n e )
# 9131 "parser/parser.ml"
             in
            _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv1124)) : 'freshtv1126)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1127) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1128)) : 'freshtv1130)
    | MenhirState544 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1139 * _menhir_state) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1131) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BIN _v ->
                _menhir_run461 _menhir_env (Obj.magic _menhir_stack) MenhirState547 _v
            | HEXA _v ->
                _menhir_run460 _menhir_env (Obj.magic _menhir_stack) MenhirState547 _v
            | INT _v ->
                _menhir_run454 _menhir_env (Obj.magic _menhir_stack) MenhirState547 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState547) : 'freshtv1132)
        | STRING _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1135) = Obj.magic _menhir_stack in
            let (_v : (
# 65 "parser/parser.mly"
       (string)
# 9169 "parser/parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1133) = Obj.magic _menhir_stack in
            let ((s : (
# 65 "parser/parser.mly"
       (string)
# 9177 "parser/parser.ml"
            )) : (
# 65 "parser/parser.mly"
       (string)
# 9181 "parser/parser.ml"
            )) = _v in
            ((let _v : 'tv_opcode = 
# 576 "parser/parser.mly"
             ( Binstream.of_bytes s )
# 9186 "parser/parser.ml"
             in
            _menhir_goto_opcode _menhir_env _menhir_stack _v) : 'freshtv1134)) : 'freshtv1136)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1137 * _menhir_state) * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1138)) : 'freshtv1140)
    | MenhirState550 | MenhirState547 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1143 * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BIN _v ->
            _menhir_run461 _menhir_env (Obj.magic _menhir_stack) MenhirState550 _v
        | HEXA _v ->
            _menhir_run460 _menhir_env (Obj.magic _menhir_stack) MenhirState550 _v
        | INT _v ->
            _menhir_run454 _menhir_env (Obj.magic _menhir_stack) MenhirState550 _v
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1141 * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (x : 'tv_integer)) = _menhir_stack in
            let _v : 'tv_nonempty_list_integer_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 9215 "parser/parser.ml"
             in
            _menhir_goto_nonempty_list_integer_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1142)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState550) : 'freshtv1144)
    | _ ->
        _menhir_fail ()

and _menhir_goto_expr : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_expr -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState35 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv811 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv809 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState36 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ARROW ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv781 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | COMMA ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv777 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state)) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | INT _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv773 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) = Obj.magic _menhir_stack in
                        let (_v : (
# 61 "parser/parser.mly"
       (string)
# 9263 "parser/parser.ml"
                        )) = _v in
                        ((let _menhir_stack = (_menhir_stack, _v) in
                        let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RBRACKET ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv769 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 9274 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv767 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 9281 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((((_menhir_stack, _menhir_s), _, (e : 'tv_expr)), _), (size : (
# 61 "parser/parser.mly"
       (string)
# 9286 "parser/parser.ml"
                            ))) = _menhir_stack in
                            let _7 = () in
                            let _5 = () in
                            let _1_inlined2 = () in
                            let _1_inlined1 = () in
                            let _2 = () in
                            let _1 = () in
                            let _v : 'tv_expr = let end_opt =
                              let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                              let x =
                                let x =
                                  let _1 = _1_inlined1 in
                                  
# 436 "parser/parser.mly"
           ( Machine.BigEndian )
# 9302 "parser/parser.ml"
                                  
                                in
                                
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 9308 "parser/parser.ml"
                                
                              in
                              
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 9314 "parser/parser.ml"
                              
                            in
                            
# 497 "parser/parser.mly"
  ( let size = int_of_string size |> Size.Byte.create in
    let endianness = default_endianness end_opt in
    Dba.Expr.load size endianness e )
# 9322 "parser/parser.ml"
                             in
                            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv768)) : 'freshtv770)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv771 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 9332 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv772)) : 'freshtv774)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv775 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv776)) : 'freshtv778)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv779 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state)) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv780)) : 'freshtv782)
            | ARROWINV ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv797 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | COMMA ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv793 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state)) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | INT _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv789 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) = Obj.magic _menhir_stack in
                        let (_v : (
# 61 "parser/parser.mly"
       (string)
# 9368 "parser/parser.ml"
                        )) = _v in
                        ((let _menhir_stack = (_menhir_stack, _v) in
                        let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RBRACKET ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv785 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 9379 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv783 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 9386 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((((_menhir_stack, _menhir_s), _, (e : 'tv_expr)), _), (size : (
# 61 "parser/parser.mly"
       (string)
# 9391 "parser/parser.ml"
                            ))) = _menhir_stack in
                            let _7 = () in
                            let _5 = () in
                            let _1_inlined2 = () in
                            let _1_inlined1 = () in
                            let _2 = () in
                            let _1 = () in
                            let _v : 'tv_expr = let end_opt =
                              let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                              let x =
                                let x =
                                  let _1 = _1_inlined1 in
                                  
# 437 "parser/parser.mly"
           ( Machine.LittleEndian )
# 9407 "parser/parser.ml"
                                  
                                in
                                
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 9413 "parser/parser.ml"
                                
                              in
                              
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 9419 "parser/parser.ml"
                              
                            in
                            
# 497 "parser/parser.mly"
  ( let size = int_of_string size |> Size.Byte.create in
    let endianness = default_endianness end_opt in
    Dba.Expr.load size endianness e )
# 9427 "parser/parser.ml"
                             in
                            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv784)) : 'freshtv786)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv787 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 9437 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv788)) : 'freshtv790)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv791 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv792)) : 'freshtv794)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv795 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state)) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv796)) : 'freshtv798)
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv805 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                let (_v : (
# 61 "parser/parser.mly"
       (string)
# 9461 "parser/parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RBRACKET ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv801 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 9472 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv799 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 9479 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((((_menhir_stack, _menhir_s), _, (e : 'tv_expr)), _), (size : (
# 61 "parser/parser.mly"
       (string)
# 9484 "parser/parser.ml"
                    ))) = _menhir_stack in
                    let _7 = () in
                    let _5 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : 'tv_expr = let end_opt = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 9493 "parser/parser.ml"
                     in
                    
# 497 "parser/parser.mly"
  ( let size = int_of_string size |> Size.Byte.create in
    let endianness = default_endianness end_opt in
    Dba.Expr.load size endianness e )
# 9500 "parser/parser.ml"
                     in
                    _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv800)) : 'freshtv802)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv803 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 9510 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv804)) : 'freshtv806)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv807 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv808)) : 'freshtv810)
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState36
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState36) : 'freshtv812)
    | MenhirState37 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv815 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState38
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | OR | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN | XOR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv813 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 517 "parser/parser.mly"
           ( Dba.Binary_op.Xor )
# 9597 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9602 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv814)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState38) : 'freshtv816)
    | MenhirState39 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv819 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv817 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_expr = let bop = 
# 516 "parser/parser.mly"
           ( Dba.Binary_op.And )
# 9619 "parser/parser.ml"
         in
        
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9624 "parser/parser.ml"
         in
        _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv818)) : 'freshtv820)
    | MenhirState42 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv823 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState43
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv821 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 532 "parser/parser.mly"
           ( Dba.Binary_op.Mult)
# 9647 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9652 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv822)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState43) : 'freshtv824)
    | MenhirState44 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv827 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState45
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState45
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | OR | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv825 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 515 "parser/parser.mly"
           ( Dba.Binary_op.Or )
# 9677 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9682 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv826)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState45) : 'freshtv828)
    | MenhirState46 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv831 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState47
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState47
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState47
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv829 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 533 "parser/parser.mly"
           ( Dba.Binary_op.Mult)
# 9709 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9714 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv830)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState47) : 'freshtv832)
    | MenhirState48 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv835 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState49
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState49
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState49
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv833 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 532 "parser/parser.mly"
           ( Dba.Binary_op.Mult)
# 9741 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9746 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv834)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState49) : 'freshtv836)
    | MenhirState50 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv839 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState51
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv837 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 534 "parser/parser.mly"
           ( Dba.Binary_op.DivU)
# 9773 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9778 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv838)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState51) : 'freshtv840)
    | MenhirState52 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv843 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState53
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv841 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 535 "parser/parser.mly"
           ( Dba.Binary_op.DivS)
# 9805 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9810 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv842)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState53) : 'freshtv844)
    | MenhirState54 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv847 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv845 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 537 "parser/parser.mly"
           ( Dba.Binary_op.RShiftU)
# 9855 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9860 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv846)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState55) : 'freshtv848)
    | MenhirState56 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv851 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState57
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv849 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 529 "parser/parser.mly"
           ( Dba.Binary_op.Plus )
# 9901 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9906 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv850)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState57) : 'freshtv852)
    | MenhirState58 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv855 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState59
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv853 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 513 "parser/parser.mly"
           ( Dba.Binary_op.ModU )
# 9933 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9938 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv854)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState59) : 'freshtv856)
    | MenhirState60 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv859 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState61
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState61
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState61
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv857 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 514 "parser/parser.mly"
           ( Dba.Binary_op.ModS )
# 9965 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 9970 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv858)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState61) : 'freshtv860)
    | MenhirState62 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv863 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState63
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv861 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 530 "parser/parser.mly"
           ( Dba.Binary_op.Minus )
# 10011 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10016 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv862)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState63) : 'freshtv864)
    | MenhirState64 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv867 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState65
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv865 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 538 "parser/parser.mly"
           ( Dba.Binary_op.RShiftS )
# 10061 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10066 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv866)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState65) : 'freshtv868)
    | MenhirState66 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv871 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState67
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv869 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 540 "parser/parser.mly"
           ( Dba.Binary_op.RightRotate )
# 10111 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10116 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv870)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState67) : 'freshtv872)
    | MenhirState68 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv875 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState69
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv873 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 520 "parser/parser.mly"
           ( Dba.Binary_op.Diff )
# 10173 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10178 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv874)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState69) : 'freshtv876)
    | MenhirState70 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv879 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState71
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv877 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 536 "parser/parser.mly"
           ( Dba.Binary_op.LShift )
# 10223 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10228 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv878)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState71) : 'freshtv880)
    | MenhirState72 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv883 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState73
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv881 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 539 "parser/parser.mly"
           ( Dba.Binary_op.LeftRotate )
# 10273 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10278 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv882)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState73) : 'freshtv884)
    | MenhirState74 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv887 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState75
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv885 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 518 "parser/parser.mly"
           ( Dba.Binary_op.Concat )
# 10333 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10338 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv886)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState75) : 'freshtv888)
    | MenhirState76 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv891 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState77
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv889 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 522 "parser/parser.mly"
           ( Dba.Binary_op.LtU  )
# 10395 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10400 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv890)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState77) : 'freshtv892)
    | MenhirState78 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv895 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState79
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv893 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 526 "parser/parser.mly"
           ( Dba.Binary_op.LtS )
# 10457 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10462 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv894)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState79) : 'freshtv896)
    | MenhirState80 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv899 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState81
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv897 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 521 "parser/parser.mly"
           ( Dba.Binary_op.LeqU )
# 10519 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10524 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv898)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState81) : 'freshtv900)
    | MenhirState82 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv903 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState83
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv901 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 525 "parser/parser.mly"
           ( Dba.Binary_op.LeqS )
# 10581 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10586 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv902)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState83) : 'freshtv904)
    | MenhirState90 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv907 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState91
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv905 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 524 "parser/parser.mly"
           ( Dba.Binary_op.GtU )
# 10643 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10648 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv906)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState91) : 'freshtv908)
    | MenhirState92 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv911 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState93
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv909 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 528 "parser/parser.mly"
           ( Dba.Binary_op.GtS )
# 10705 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10710 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv910)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState93) : 'freshtv912)
    | MenhirState94 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv915 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState95
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv913 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 523 "parser/parser.mly"
           ( Dba.Binary_op.GeqU )
# 10767 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10772 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv914)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState95) : 'freshtv916)
    | MenhirState96 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv919 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState97
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv917 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 527 "parser/parser.mly"
           ( Dba.Binary_op.GeqS )
# 10829 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10834 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv918)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState97) : 'freshtv920)
    | MenhirState98 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv923 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState99
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LTS | LTU | NEQ | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv921 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s, (le : 'tv_expr)), _), _, (re : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = let bop = 
# 519 "parser/parser.mly"
           ( Dba.Binary_op.Eq )
# 10891 "parser/parser.ml"
             in
            
# 509 "parser/parser.mly"
   ( Dba.Expr.binary bop le re )
# 10896 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv922)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState99) : 'freshtv924)
    | MenhirState33 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv929 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv927 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState111 in
            let (_v : (
# 61 "parser/parser.mly"
       (string)
# 10930 "parser/parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv925 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((size : (
# 61 "parser/parser.mly"
       (string)
# 10939 "parser/parser.ml"
            )) : (
# 61 "parser/parser.mly"
       (string)
# 10943 "parser/parser.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 505 "parser/parser.mly"
   ( Dba.Expr.sext (int_of_string size) e )
# 10950 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv926)) : 'freshtv928)
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState111
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState111) : 'freshtv930)
    | MenhirState32 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv935 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv933 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState113 in
            let (_v : (
# 61 "parser/parser.mly"
       (string)
# 11028 "parser/parser.ml"
            )) = _v in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv931 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            let ((size : (
# 61 "parser/parser.mly"
       (string)
# 11037 "parser/parser.ml"
            )) : (
# 61 "parser/parser.mly"
       (string)
# 11041 "parser/parser.ml"
            )) = _v in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 503 "parser/parser.mly"
   ( Dba.Expr.uext (int_of_string size) e )
# 11048 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv932)) : 'freshtv934)
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState113
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState113) : 'freshtv936)
    | MenhirState29 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv939 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv937 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState115 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | AT ->
                _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState116
            | BIN _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState116 _v
            | EXTS ->
                _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState116
            | EXTU ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState116
            | HEXA _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState116 _v
            | IDENT _v ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState116 _v
            | IF ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState116
            | INT _v ->
                _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState116 _v
            | LPAR ->
                _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState116
            | MINUS ->
                _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState116
            | NOT ->
                _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState116
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState116) : 'freshtv938)
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState115
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState115) : 'freshtv940)
    | MenhirState116 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv943 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | ELSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv941 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState117 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | AT ->
                _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState118
            | BIN _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState118 _v
            | EXTS ->
                _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState118
            | EXTU ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState118
            | HEXA _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState118 _v
            | IDENT _v ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState118 _v
            | IF ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState118
            | INT _v ->
                _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState118 _v
            | LPAR ->
                _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState118
            | MINUS ->
                _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState118
            | NOT ->
                _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState118
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState118) : 'freshtv942)
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState117
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState117) : 'freshtv944)
    | MenhirState118 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv947 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState119
        | ANNOT | COMMA | DOTDOT | ELSE | EOF | GOTO | HEXA _ | INT _ | LBRACE | LPAR | RBRACE | RBRACKETS | RBRACKETU | RPAR | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv945 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let ((((((_menhir_stack, _menhir_s), _, (c : 'tv_expr)), _), _, (then_e : 'tv_expr)), _), _, (else_e : 'tv_expr)) = _menhir_stack in
            let _5 = () in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 507 "parser/parser.mly"
   ( Dba.Expr.ite c then_e else_e )
# 11375 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv946)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState119) : 'freshtv948)
    | MenhirState197 | MenhirState24 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv953 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv951 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState124 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv949 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_expr)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_expr = 
# 480 "parser/parser.mly"
                    ( e )
# 11442 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv950)) : 'freshtv952)
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState124
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState124) : 'freshtv954)
    | MenhirState23 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv957 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState126
        | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | NEQ | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | THEN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv955 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_expr = 
# 501 "parser/parser.mly"
                     ( Dba.Expr.uminus e )
# 11501 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv956)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState126) : 'freshtv958)
    | MenhirState22 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv961 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv959 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_expr)) = _menhir_stack in
        let _1 = () in
        let _v : 'tv_expr = 
# 500 "parser/parser.mly"
                     ( Dba.Expr.lognot e )
# 11518 "parser/parser.ml"
         in
        _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv960)) : 'freshtv962)
    | MenhirState20 | MenhirState130 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv965 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState132
        | COMMA | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv963 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (e : 'tv_expr)) = _menhir_stack in
            let _v : 'tv_printarg = 
# 431 "parser/parser.mly"
            ( Exp e )
# 11592 "parser/parser.ml"
             in
            _menhir_goto_printarg _menhir_env _menhir_stack _menhir_s _v) : 'freshtv964)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState132) : 'freshtv966)
    | MenhirState146 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1011 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1009 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState147 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | ARROW ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv981 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | COMMA ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv977 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state)) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | INT _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv973 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) = Obj.magic _menhir_stack in
                        let (_v : (
# 61 "parser/parser.mly"
       (string)
# 11633 "parser/parser.ml"
                        )) = _v in
                        ((let _menhir_stack = (_menhir_stack, _v) in
                        let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RBRACKET ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv969 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 11644 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv967 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 11651 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((((_menhir_stack, _menhir_s), _, (e : 'tv_expr)), _), (size : (
# 61 "parser/parser.mly"
       (string)
# 11656 "parser/parser.ml"
                            ))) = _menhir_stack in
                            let _7 = () in
                            let _5 = () in
                            let _1_inlined2 = () in
                            let _1_inlined1 = () in
                            let _2 = () in
                            let _1 = () in
                            let _v : 'tv_address_lvalue = let end_opt =
                              let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                              let x =
                                let x =
                                  let _1 = _1_inlined1 in
                                  
# 436 "parser/parser.mly"
           ( Machine.BigEndian )
# 11672 "parser/parser.ml"
                                  
                                in
                                
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 11678 "parser/parser.ml"
                                
                              in
                              
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 11684 "parser/parser.ml"
                              
                            in
                            
# 444 "parser/parser.mly"
  ( let sz = int_of_string size |> Size.Byte.create in
    let endianness = default_endianness end_opt in
    Dba.LValue.store sz endianness e
  )
# 11693 "parser/parser.ml"
                             in
                            _menhir_goto_address_lvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv968)) : 'freshtv970)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv971 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 11703 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv972)) : 'freshtv974)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv975 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv976)) : 'freshtv978)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv979 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state)) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv980)) : 'freshtv982)
            | ARROWINV ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv997 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | COMMA ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv993 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state)) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | INT _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv989 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) = Obj.magic _menhir_stack in
                        let (_v : (
# 61 "parser/parser.mly"
       (string)
# 11739 "parser/parser.ml"
                        )) = _v in
                        ((let _menhir_stack = (_menhir_stack, _v) in
                        let _menhir_env = _menhir_discard _menhir_env in
                        let _tok = _menhir_env._menhir_token in
                        match _tok with
                        | RBRACKET ->
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv985 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 11750 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let _menhir_env = _menhir_discard _menhir_env in
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv983 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 11757 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((((_menhir_stack, _menhir_s), _, (e : 'tv_expr)), _), (size : (
# 61 "parser/parser.mly"
       (string)
# 11762 "parser/parser.ml"
                            ))) = _menhir_stack in
                            let _7 = () in
                            let _5 = () in
                            let _1_inlined2 = () in
                            let _1_inlined1 = () in
                            let _2 = () in
                            let _1 = () in
                            let _v : 'tv_address_lvalue = let end_opt =
                              let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                              let x =
                                let x =
                                  let _1 = _1_inlined1 in
                                  
# 437 "parser/parser.mly"
           ( Machine.LittleEndian )
# 11778 "parser/parser.ml"
                                  
                                in
                                
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 11784 "parser/parser.ml"
                                
                              in
                              
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 11790 "parser/parser.ml"
                              
                            in
                            
# 444 "parser/parser.mly"
  ( let sz = int_of_string size |> Size.Byte.create in
    let endianness = default_endianness end_opt in
    Dba.LValue.store sz endianness e
  )
# 11799 "parser/parser.ml"
                             in
                            _menhir_goto_address_lvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv984)) : 'freshtv986)
                        | _ ->
                            assert (not _menhir_env._menhir_error);
                            _menhir_env._menhir_error <- true;
                            let (_menhir_env : _menhir_env) = _menhir_env in
                            let (_menhir_stack : (((((('freshtv987 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) * (
# 61 "parser/parser.mly"
       (string)
# 11809 "parser/parser.ml"
                            )) = Obj.magic _menhir_stack in
                            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv988)) : 'freshtv990)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv991 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state))) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv992)) : 'freshtv994)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv995 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state)) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv996)) : 'freshtv998)
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1005 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                let (_v : (
# 61 "parser/parser.mly"
       (string)
# 11833 "parser/parser.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RBRACKET ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1001 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 11844 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv999 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 11851 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((((_menhir_stack, _menhir_s), _, (e : 'tv_expr)), _), (size : (
# 61 "parser/parser.mly"
       (string)
# 11856 "parser/parser.ml"
                    ))) = _menhir_stack in
                    let _7 = () in
                    let _5 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : 'tv_address_lvalue = let end_opt = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 11865 "parser/parser.ml"
                     in
                    
# 444 "parser/parser.mly"
  ( let sz = int_of_string size |> Size.Byte.create in
    let endianness = default_endianness end_opt in
    Dba.LValue.store sz endianness e
  )
# 11873 "parser/parser.ml"
                     in
                    _menhir_goto_address_lvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1000)) : 'freshtv1002)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv1003 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 11883 "parser/parser.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1004)) : 'freshtv1006)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv1007 * _menhir_state)) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1008)) : 'freshtv1010)
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState147
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState147) : 'freshtv1012)
    | MenhirState167 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv1017 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv1015 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState172 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv1013 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let (((_menhir_stack, _menhir_s), (lvalues : 'tv_set_of_lvalue_)), _, (e : 'tv_expr)) = _menhir_stack in
            let _6 = () in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_annotable_instruction = let condition = 
# 546 "parser/parser.mly"
            ( e )
# 12016 "parser/parser.ml"
             in
            
# 384 "parser/parser.mly"
  ( Dba.Instr.non_deterministic_assume
      lvalues condition unknown_successor
  )
# 12023 "parser/parser.ml"
             in
            _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1014)) : 'freshtv1016)
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState172
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState172) : 'freshtv1018)
    | MenhirState175 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1021 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | GOTO ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1019 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState190 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                _menhir_run178 _menhir_env (Obj.magic _menhir_stack) MenhirState191 _v
            | LPAR ->
                _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState191
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState191) : 'freshtv1020)
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState190) : 'freshtv1022)
    | MenhirState196 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1025 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | ANNOT ->
            _menhir_run201 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState208
        | EOF | HEXA _ | INT _ | LPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1023) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState208 in
            ((let _v : 'tv_option_jump_annotation_ = 
# 114 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 12205 "parser/parser.ml"
             in
            _menhir_goto_option_jump_annotation_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1024)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState208) : 'freshtv1026)
    | MenhirState212 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1031 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1029 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState213 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1027 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((_menhir_stack, _menhir_s), _, (x : 'tv_expr)) = _menhir_stack in
            let _3 = () in
            let _1_inlined1 = () in
            let _1 = () in
            let _v : 'tv_annotable_instruction = let e =
              let _1 = _1_inlined1 in
              
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 12275 "parser/parser.ml"
              
            in
            
# 377 "parser/parser.mly"
  ( Dba.Instr.free e unknown_successor )
# 12281 "parser/parser.ml"
             in
            _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1028)) : 'freshtv1030)
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState213
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState213) : 'freshtv1032)
    | MenhirState216 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1037 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1035 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState221 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1033 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_expr)) = _menhir_stack in
            let _3 = () in
            let _1_inlined1 = () in
            let _1 = () in
            let _v : 'tv_annotable_instruction = let condition =
              let _1 = _1_inlined1 in
              let x = 
# 546 "parser/parser.mly"
            ( e )
# 12369 "parser/parser.ml"
               in
              
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 12374 "parser/parser.ml"
              
            in
            
# 381 "parser/parser.mly"
  ( Dba.Instr.assume condition unknown_successor )
# 12380 "parser/parser.ml"
             in
            _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1034)) : 'freshtv1036)
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState221
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState221) : 'freshtv1038)
    | MenhirState224 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1043 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1041 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState229 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1039 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((_menhir_stack, _menhir_s), _, (e : 'tv_expr)) = _menhir_stack in
            let _3 = () in
            let _1_inlined1 = () in
            let _1 = () in
            let _v : 'tv_annotable_instruction = let condition =
              let _1 = _1_inlined1 in
              let x = 
# 546 "parser/parser.mly"
            ( e )
# 12468 "parser/parser.ml"
               in
              
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 12473 "parser/parser.ml"
              
            in
            
# 379 "parser/parser.mly"
  ( Dba.Instr._assert condition unknown_successor )
# 12479 "parser/parser.ml"
             in
            _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1040)) : 'freshtv1042)
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState229
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState229) : 'freshtv1044)
    | MenhirState244 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv1049 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1047 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState245 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv1045 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((_menhir_stack, _menhir_s), _, (x : 'tv_expr)) = _menhir_stack in
            let _3 = () in
            let _1_inlined1 = () in
            let _1 = () in
            let _v : 'tv_rvalue = let e =
              let _1 = _1_inlined1 in
              
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 12567 "parser/parser.ml"
              
            in
            
# 320 "parser/parser.mly"
  ( fun lv -> Dba.Instr.malloc lv e  )
# 12573 "parser/parser.ml"
             in
            _menhir_goto_rvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1046)) : 'freshtv1048)
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState245
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState245) : 'freshtv1050)
    | MenhirState513 | MenhirState232 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1053 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState248
        | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1051 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (e : 'tv_expr)) = _menhir_stack in
            let _v : 'tv_rvalue = 
# 317 "parser/parser.mly"
          ( fun lv -> Dba.Instr.assign lv e )
# 12669 "parser/parser.ml"
             in
            _menhir_goto_rvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1052)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState248) : 'freshtv1054)
    | MenhirState484 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1059) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | LPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1055) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState485 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BIN _v ->
                _menhir_run461 _menhir_env (Obj.magic _menhir_stack) MenhirState486 _v
            | HEXA _v ->
                _menhir_run460 _menhir_env (Obj.magic _menhir_stack) MenhirState486 _v
            | INT _v ->
                _menhir_run454 _menhir_env (Obj.magic _menhir_stack) MenhirState486 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState486) : 'freshtv1056)
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState485
        | EOF | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1057) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, (e : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_directive = let times =
              let times = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 12767 "parser/parser.ml"
               in
              
# 591 "parser/parser.mly"
                                                ( times )
# 12772 "parser/parser.ml"
              
            in
            
# 612 "parser/parser.mly"
   ( let n = Utils.get_opt_or_default 1 times in Directive.enumerate ~n e )
# 12778 "parser/parser.ml"
             in
            _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv1058)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState485) : 'freshtv1060)
    | MenhirState494 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1063) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState495
        | EOF | SEMICOLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv1061) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, (e : 'tv_expr)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_directive = 
# 614 "parser/parser.mly"
   ( Directive.assume e )
# 12857 "parser/parser.ml"
             in
            _menhir_goto_directive _menhir_env _menhir_stack _v) : 'freshtv1062)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState495) : 'freshtv1064)
    | MenhirState500 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1075 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1073 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState502 in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1071 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let (_menhir_stack, _menhir_s, (e : 'tv_expr)) = _menhir_stack in
            let _2 = () in
            let _v : (
# 82 "parser/parser.mly"
      (Dba.Expr.t)
# 12886 "parser/parser.ml"
            ) = 
# 476 "parser/parser.mly"
              ( e )
# 12890 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1069) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 82 "parser/parser.mly"
      (Dba.Expr.t)
# 12898 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1067) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 82 "parser/parser.mly"
      (Dba.Expr.t)
# 12906 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1065) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let ((_1 : (
# 82 "parser/parser.mly"
      (Dba.Expr.t)
# 12914 "parser/parser.ml"
            )) : (
# 82 "parser/parser.mly"
      (Dba.Expr.t)
# 12918 "parser/parser.ml"
            )) = _v in
            (Obj.magic _1 : 'freshtv1066)) : 'freshtv1068)) : 'freshtv1070)) : 'freshtv1072)) : 'freshtv1074)
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState502
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState502) : 'freshtv1076)
    | MenhirState514 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1085 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1079) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState515 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1077) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            ((let _1 = () in
            let _v : 'tv_either_DOTDOT_COMMA_ = 
# 334 "parser/parser.mly"
    ( _1 )
# 12999 "parser/parser.ml"
             in
            _menhir_goto_either_DOTDOT_COMMA_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1078)) : 'freshtv1080)
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | DOTDOT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1083) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState515 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1081) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            ((let _1 = () in
            let _v : 'tv_either_DOTDOT_COMMA_ = 
# 333 "parser/parser.mly"
    ( _1 )
# 13016 "parser/parser.ml"
             in
            _menhir_goto_either_DOTDOT_COMMA_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1082)) : 'freshtv1084)
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState515
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState515) : 'freshtv1086)
    | MenhirState518 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1095 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_either_DOTDOT_COMMA_) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | RBRACKETS ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1089 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_either_DOTDOT_COMMA_) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState519 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1087 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_either_DOTDOT_COMMA_) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((((_menhir_stack, _menhir_s), _, (e1 : 'tv_expr)), _, (_3 : 'tv_either_DOTDOT_COMMA_)), _, (e2 : 'tv_expr)) = _menhir_stack in
            let _5 = () in
            let _1 = () in
            let _v : 'tv_interval_or_set = 
# 339 "parser/parser.mly"
  ( Initialization.Signed_interval(e1,e2) )
# 13137 "parser/parser.ml"
             in
            _menhir_goto_interval_or_set _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1088)) : 'freshtv1090)
        | RBRACKETU ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1093 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_either_DOTDOT_COMMA_) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState519 in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1091 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_either_DOTDOT_COMMA_) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_ : _menhir_state) = _menhir_s in
            ((let ((((_menhir_stack, _menhir_s), _, (e1 : 'tv_expr)), _, (_3 : 'tv_either_DOTDOT_COMMA_)), _, (e2 : 'tv_expr)) = _menhir_stack in
            let _5 = () in
            let _1 = () in
            let _v : 'tv_interval_or_set = 
# 341 "parser/parser.mly"
  ( Initialization.Unsigned_interval(e1,e2) )
# 13154 "parser/parser.ml"
             in
            _menhir_goto_interval_or_set _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1092)) : 'freshtv1094)
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState519
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState519) : 'freshtv1096)
    | MenhirState526 | MenhirState522 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv1101 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AND ->
            _menhir_run39 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1097 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState525 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | AT ->
                _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState526
            | BIN _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState526 _v
            | EXTS ->
                _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState526
            | EXTU ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState526
            | HEXA _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState526 _v
            | IDENT _v ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState526 _v
            | IF ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState526
            | INT _v ->
                _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState526 _v
            | LPAR ->
                _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState526
            | MINUS ->
                _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState526
            | NOT ->
                _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState526
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState526) : 'freshtv1098)
        | CONCAT ->
            _menhir_run74 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | EQUAL ->
            _menhir_run98 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | GES ->
            _menhir_run96 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | GEU ->
            _menhir_run94 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | GTS ->
            _menhir_run92 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | GTU ->
            _menhir_run90 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | LBRACE ->
            _menhir_run84 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | LES ->
            _menhir_run82 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | LEU ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | LROTATE ->
            _menhir_run72 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | LSHIFT ->
            _menhir_run70 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | LTS ->
            _menhir_run78 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | LTU ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | MINUS ->
            _menhir_run62 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | MODS ->
            _menhir_run60 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | MODU ->
            _menhir_run58 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | NEQ ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | OR ->
            _menhir_run44 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | PLUS ->
            _menhir_run56 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | RROTATE ->
            _menhir_run66 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | RSHIFTS ->
            _menhir_run64 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | RSHIFTU ->
            _menhir_run54 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | SLASH_S ->
            _menhir_run52 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | SLASH_U ->
            _menhir_run50 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | STAR ->
            _menhir_run48 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | STAR_S ->
            _menhir_run46 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | STAR_U ->
            _menhir_run42 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | XOR ->
            _menhir_run37 _menhir_env (Obj.magic _menhir_stack) MenhirState525
        | RBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1099 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (x : 'tv_expr)) = _menhir_stack in
            let _v : 'tv_separated_nonempty_list_COMMA_expr_ = 
# 241 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 13284 "parser/parser.ml"
             in
            _menhir_goto_separated_nonempty_list_COMMA_expr_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1100)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState525) : 'freshtv1102)
    | _ ->
        _menhir_fail ()

and _menhir_goto_constant : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_constant -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState1 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv739 * _menhir_state) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv737 * _menhir_state) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv738)) : 'freshtv740)
    | MenhirState26 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv747 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv743 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv741 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s), _), _, (x : 'tv_constant)) = _menhir_stack in
            let _4 = () in
            let _1_inlined2 = () in
            let _1_inlined1 = () in
            let _1 = () in
            let _v : 'tv_expr = let cst =
              let _1 = _1_inlined2 in
              
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 13335 "parser/parser.ml"
              
            in
            let region =
              let _1 = _1_inlined1 in
              
# 261 "parser/parser.mly"
            ( `Stack )
# 13343 "parser/parser.ml"
              
            in
            
# 492 "parser/parser.mly"
  ( Dba.Expr.constant cst ~region )
# 13349 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv742)) : 'freshtv744)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv745 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv746)) : 'freshtv748)
    | MenhirState513 | MenhirState522 | MenhirState526 | MenhirState514 | MenhirState518 | MenhirState500 | MenhirState494 | MenhirState484 | MenhirState232 | MenhirState244 | MenhirState224 | MenhirState216 | MenhirState212 | MenhirState196 | MenhirState175 | MenhirState167 | MenhirState146 | MenhirState20 | MenhirState130 | MenhirState22 | MenhirState23 | MenhirState24 | MenhirState29 | MenhirState116 | MenhirState118 | MenhirState32 | MenhirState33 | MenhirState35 | MenhirState98 | MenhirState96 | MenhirState94 | MenhirState92 | MenhirState90 | MenhirState82 | MenhirState80 | MenhirState78 | MenhirState76 | MenhirState68 | MenhirState74 | MenhirState72 | MenhirState70 | MenhirState66 | MenhirState64 | MenhirState54 | MenhirState62 | MenhirState56 | MenhirState60 | MenhirState58 | MenhirState52 | MenhirState50 | MenhirState48 | MenhirState46 | MenhirState42 | MenhirState44 | MenhirState37 | MenhirState39 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv749 * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
        (_menhir_reduce122 _menhir_env (Obj.magic _menhir_stack) : 'freshtv750)
    | MenhirState121 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv757 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv753 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv751 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
            ((let (((_menhir_stack, _menhir_s), _), _, (x : 'tv_constant)) = _menhir_stack in
            let _4 = () in
            let _1_inlined2 = () in
            let _1_inlined1 = () in
            let _1 = () in
            let _v : 'tv_expr = let cst =
              let _1 = _1_inlined2 in
              
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 13385 "parser/parser.ml"
              
            in
            let region =
              let _1 = _1_inlined1 in
              
# 260 "parser/parser.mly"
            ( `Constant )
# 13393 "parser/parser.ml"
              
            in
            
# 492 "parser/parser.mly"
  ( Dba.Expr.constant cst ~region )
# 13399 "parser/parser.ml"
             in
            _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv752)) : 'freshtv754)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv755 * _menhir_state) * _menhir_state)) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv756)) : 'freshtv758)
    | MenhirState197 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv761 * _menhir_state) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            _menhir_run10 _menhir_env (Obj.magic _menhir_stack)
        | AND | CONCAT | EQUAL | GES | GEU | GTS | GTU | LBRACE | LES | LEU | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | OR | PLUS | RPAR | RROTATE | RSHIFTS | RSHIFTU | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | XOR ->
            _menhir_reduce122 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv759 * _menhir_state) * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv760)) : 'freshtv762)
    | MenhirState389 | MenhirState386 | MenhirState289 | MenhirState370 | MenhirState292 | MenhirState293 | MenhirState294 | MenhirState295 | MenhirState296 | MenhirState360 | MenhirState358 | MenhirState356 | MenhirState354 | MenhirState352 | MenhirState343 | MenhirState341 | MenhirState339 | MenhirState337 | MenhirState329 | MenhirState335 | MenhirState333 | MenhirState331 | MenhirState327 | MenhirState325 | MenhirState315 | MenhirState323 | MenhirState317 | MenhirState321 | MenhirState319 | MenhirState313 | MenhirState311 | MenhirState309 | MenhirState307 | MenhirState303 | MenhirState305 | MenhirState298 | MenhirState300 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv765 * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv763 * _menhir_state * 'tv_constant) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (cst : 'tv_constant)) = _menhir_stack in
        let _v : 'tv_constant_expr = 
# 243 "parser/parser.mly"
   ( Dba.Expr.constant cst  )
# 13435 "parser/parser.ml"
         in
        _menhir_goto_constant_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv764)) : 'freshtv766)
    | _ ->
        _menhir_fail ()

and _menhir_goto_list_terminated_initialization_directive_SEMICOLON__ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_terminated_initialization_directive_SEMICOLON__ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState504 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv731 * _menhir_state * 'tv_list_terminated_initialization_directive_SEMICOLON__) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv727 * _menhir_state * 'tv_list_terminated_initialization_directive_SEMICOLON__) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv725 * _menhir_state * 'tv_list_terminated_initialization_directive_SEMICOLON__) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (v : 'tv_list_terminated_initialization_directive_SEMICOLON__)) = _menhir_stack in
            let _2 = () in
            let _v : (
# 109 "parser/parser.mly"
      (Parse_helpers.Initialization.t list)
# 13461 "parser/parser.ml"
            ) = 
# 369 "parser/parser.mly"
                                                               ( v )
# 13465 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv723) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 109 "parser/parser.mly"
      (Parse_helpers.Initialization.t list)
# 13473 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv721) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 109 "parser/parser.mly"
      (Parse_helpers.Initialization.t list)
# 13481 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv719) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let ((_1 : (
# 109 "parser/parser.mly"
      (Parse_helpers.Initialization.t list)
# 13489 "parser/parser.ml"
            )) : (
# 109 "parser/parser.mly"
      (Parse_helpers.Initialization.t list)
# 13493 "parser/parser.ml"
            )) = _v in
            (Obj.magic _1 : 'freshtv720)) : 'freshtv722)) : 'freshtv724)) : 'freshtv726)) : 'freshtv728)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv729 * _menhir_state * 'tv_list_terminated_initialization_directive_SEMICOLON__) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv730)) : 'freshtv732)
    | MenhirState509 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv735 * _menhir_state * 'tv_initialization_directive)) * _menhir_state * 'tv_list_terminated_initialization_directive_SEMICOLON__) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv733 * _menhir_state * 'tv_initialization_directive)) * _menhir_state * 'tv_list_terminated_initialization_directive_SEMICOLON__) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_initialization_directive)), _, (xs : 'tv_list_terminated_initialization_directive_SEMICOLON__)) = _menhir_stack in
        let _2 = () in
        let _v : 'tv_list_terminated_initialization_directive_SEMICOLON__ = let x = 
# 191 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 13513 "parser/parser.ml"
         in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 13518 "parser/parser.ml"
         in
        _menhir_goto_list_terminated_initialization_directive_SEMICOLON__ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv734)) : 'freshtv736)
    | _ ->
        _menhir_fail ()

and _menhir_goto_boption_UNCONTROLLED_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_boption_UNCONTROLLED_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv717 * _menhir_state * 'tv_boption_UNCONTROLLED_) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run145 _menhir_env (Obj.magic _menhir_stack) MenhirState511
    | IDENT _v ->
        _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState511 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState511) : 'freshtv718)

and _menhir_goto_option_stop_annot_ : _menhir_env -> 'ttv_tail -> 'tv_option_stop_annot_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv715 * _menhir_state) = Obj.magic _menhir_stack in
    let (_v : 'tv_option_stop_annot_) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv713 * _menhir_state) = Obj.magic _menhir_stack in
    let ((sannot : 'tv_option_stop_annot_) : 'tv_option_stop_annot_) = _v in
    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
    let _1 = () in
    let _v : 'tv_explicit_instruction = 
# 394 "parser/parser.mly"
  ( Dba.Instr.stop sannot )
# 13554 "parser/parser.ml"
     in
    _menhir_goto_explicit_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv714)) : 'freshtv716)

and _menhir_run21 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 65 "parser/parser.mly"
       (string)
# 13561 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv711) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((s : (
# 65 "parser/parser.mly"
       (string)
# 13571 "parser/parser.ml"
    )) : (
# 65 "parser/parser.mly"
       (string)
# 13575 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_printarg = 
# 432 "parser/parser.mly"
            ( Str s )
# 13580 "parser/parser.ml"
     in
    _menhir_goto_printarg _menhir_env _menhir_stack _menhir_s _v) : 'freshtv712)

and _menhir_goto_loption_separated_nonempty_list_COMMA_lvalue__ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_loption_separated_nonempty_list_COMMA_lvalue__ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ('freshtv709) * _menhir_state * 'tv_loption_separated_nonempty_list_COMMA_lvalue__) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | RBRACE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv705) * _menhir_state * 'tv_loption_separated_nonempty_list_COMMA_lvalue__) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv703) * _menhir_state * 'tv_loption_separated_nonempty_list_COMMA_lvalue__) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, (xs : 'tv_loption_separated_nonempty_list_COMMA_lvalue__)) = _menhir_stack in
        let _3 = () in
        let _1 = () in
        let _v : 'tv_bag_of_LBRACE_RBRACE_COMMA_lvalue_ = let _1 =
          let x = 
# 232 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( xs )
# 13605 "parser/parser.ml"
           in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 13610 "parser/parser.ml"
          
        in
        
# 117 "parser/parser.mly"
                                                      ( _1 )
# 13616 "parser/parser.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv701) = _menhir_stack in
        let (_v : 'tv_bag_of_LBRACE_RBRACE_COMMA_lvalue_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv699) = Obj.magic _menhir_stack in
        let (_v : 'tv_bag_of_LBRACE_RBRACE_COMMA_lvalue_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv697) = Obj.magic _menhir_stack in
        let ((v : 'tv_bag_of_LBRACE_RBRACE_COMMA_lvalue_) : 'tv_bag_of_LBRACE_RBRACE_COMMA_lvalue_) = _v in
        ((let _v : 'tv_set_of_lvalue_ = 
# 121 "parser/parser.mly"
                                      ( v )
# 13630 "parser/parser.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv695) = _menhir_stack in
        let (_v : 'tv_set_of_lvalue_) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv693 * _menhir_state)) * 'tv_set_of_lvalue_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COMMA ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv689 * _menhir_state)) * 'tv_set_of_lvalue_) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | AT ->
                _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState167
            | BIN _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState167 _v
            | EXTS ->
                _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState167
            | EXTU ->
                _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState167
            | FALSE ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv679 * _menhir_state)) * 'tv_set_of_lvalue_)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState167 in
                ((let _menhir_stack = (_menhir_stack, _menhir_s) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RPAR ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv675 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv673 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _menhir_s), (lvalues : 'tv_set_of_lvalue_)), _) = _menhir_stack in
                    let _6 = () in
                    let _1_inlined1 = () in
                    let _4 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : 'tv_annotable_instruction = let condition =
                      let _1 = _1_inlined1 in
                      
# 545 "parser/parser.mly"
            ( Dba.Expr.zero )
# 13680 "parser/parser.ml"
                      
                    in
                    
# 384 "parser/parser.mly"
  ( Dba.Instr.non_deterministic_assume
      lvalues condition unknown_successor
  )
# 13688 "parser/parser.ml"
                     in
                    _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv674)) : 'freshtv676)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv677 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv678)) : 'freshtv680)
            | HEXA _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState167 _v
            | IDENT _v ->
                _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState167 _v
            | IF ->
                _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState167
            | INT _v ->
                _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState167 _v
            | LPAR ->
                _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState167
            | MINUS ->
                _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState167
            | NOT ->
                _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState167
            | TRUE ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv687 * _menhir_state)) * 'tv_set_of_lvalue_)) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState167 in
                ((let _menhir_stack = (_menhir_stack, _menhir_s) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RPAR ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv683 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv681 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _menhir_s), (lvalues : 'tv_set_of_lvalue_)), _) = _menhir_stack in
                    let _6 = () in
                    let _1_inlined1 = () in
                    let _4 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : 'tv_annotable_instruction = let condition =
                      let _1 = _1_inlined1 in
                      
# 544 "parser/parser.mly"
            ( Dba.Expr.one )
# 13737 "parser/parser.ml"
                      
                    in
                    
# 384 "parser/parser.mly"
  ( Dba.Instr.non_deterministic_assume
      lvalues condition unknown_successor
  )
# 13745 "parser/parser.ml"
                     in
                    _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv682)) : 'freshtv684)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv685 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv686)) : 'freshtv688)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState167) : 'freshtv690)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv691 * _menhir_state)) * 'tv_set_of_lvalue_) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv692)) : 'freshtv694)) : 'freshtv696)) : 'freshtv698)) : 'freshtv700)) : 'freshtv702)) : 'freshtv704)) : 'freshtv706)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv707) * _menhir_state * 'tv_loption_separated_nonempty_list_COMMA_lvalue__) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv708)) : 'freshtv710)

and _menhir_run178 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 61 "parser/parser.mly"
       (string)
# 13777 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    _menhir_reduce263 _menhir_env (Obj.magic _menhir_stack)

and _menhir_goto_option_size_annot_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_option_size_annot_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ('freshtv671 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13791 "parser/parser.ml"
    )) * _menhir_state * 'tv_option_size_annot_) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv665 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13801 "parser/parser.ml"
        )) * _menhir_state * 'tv_option_size_annot_) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv661 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13811 "parser/parser.ml"
            )) * _menhir_state * 'tv_option_size_annot_)) = Obj.magic _menhir_stack in
            let (_v : (
# 61 "parser/parser.mly"
       (string)
# 13816 "parser/parser.ml"
            )) = _v in
            ((let _menhir_stack = (_menhir_stack, _v) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | COMMA ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv653 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13827 "parser/parser.ml"
                )) * _menhir_state * 'tv_option_size_annot_)) * (
# 61 "parser/parser.mly"
       (string)
# 13831 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | INT _v ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv649 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13841 "parser/parser.ml"
                    )) * _menhir_state * 'tv_option_size_annot_)) * (
# 61 "parser/parser.mly"
       (string)
# 13845 "parser/parser.ml"
                    ))) = Obj.magic _menhir_stack in
                    let (_v : (
# 61 "parser/parser.mly"
       (string)
# 13850 "parser/parser.ml"
                    )) = _v in
                    ((let _menhir_stack = (_menhir_stack, _v) in
                    let _menhir_env = _menhir_discard _menhir_env in
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | RBRACE ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv645 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13861 "parser/parser.ml"
                        )) * _menhir_state * 'tv_option_size_annot_)) * (
# 61 "parser/parser.mly"
       (string)
# 13865 "parser/parser.ml"
                        ))) * (
# 61 "parser/parser.mly"
       (string)
# 13869 "parser/parser.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv643 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13876 "parser/parser.ml"
                        )) * _menhir_state * 'tv_option_size_annot_)) * (
# 61 "parser/parser.mly"
       (string)
# 13880 "parser/parser.ml"
                        ))) * (
# 61 "parser/parser.mly"
       (string)
# 13884 "parser/parser.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let ((((_menhir_stack, _menhir_s, (id : (
# 62 "parser/parser.mly"
       (string)
# 13889 "parser/parser.ml"
                        ))), _, (sz_opt : 'tv_option_size_annot_)), (loff : (
# 61 "parser/parser.mly"
       (string)
# 13893 "parser/parser.ml"
                        ))), (roff : (
# 61 "parser/parser.mly"
       (string)
# 13897 "parser/parser.ml"
                        ))) = _menhir_stack in
                        let _5 = () in
                        let _3 = () in
                        let _1 = () in
                        let _v : 'tv_lvalue = let offs = 
# 470 "parser/parser.mly"
  ( int_of_string loff, int_of_string roff )
# 13905 "parser/parser.ml"
                         in
                        
# 455 "parser/parser.mly"
  ( let bitsize = Utils.get_opt_or_default 1 sz_opt |> Size.Bit.create in
    let lo, hi = offs in Dba.LValue._restrict id bitsize lo hi )
# 13911 "parser/parser.ml"
                         in
                        _menhir_goto_lvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv644)) : 'freshtv646)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : ((((('freshtv647 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13921 "parser/parser.ml"
                        )) * _menhir_state * 'tv_option_size_annot_)) * (
# 61 "parser/parser.mly"
       (string)
# 13925 "parser/parser.ml"
                        ))) * (
# 61 "parser/parser.mly"
       (string)
# 13929 "parser/parser.ml"
                        )) = Obj.magic _menhir_stack in
                        ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv648)) : 'freshtv650)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : (((('freshtv651 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13940 "parser/parser.ml"
                    )) * _menhir_state * 'tv_option_size_annot_)) * (
# 61 "parser/parser.mly"
       (string)
# 13944 "parser/parser.ml"
                    ))) = Obj.magic _menhir_stack in
                    ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv652)) : 'freshtv654)
            | RBRACE ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv657 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13953 "parser/parser.ml"
                )) * _menhir_state * 'tv_option_size_annot_)) * (
# 61 "parser/parser.mly"
       (string)
# 13957 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv655 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 13964 "parser/parser.ml"
                )) * _menhir_state * 'tv_option_size_annot_)) * (
# 61 "parser/parser.mly"
       (string)
# 13968 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s, (id : (
# 62 "parser/parser.mly"
       (string)
# 13973 "parser/parser.ml"
                ))), _, (sz_opt : 'tv_option_size_annot_)), (boff : (
# 61 "parser/parser.mly"
       (string)
# 13977 "parser/parser.ml"
                ))) = _menhir_stack in
                let _3 = () in
                let _1 = () in
                let _v : 'tv_lvalue = let offs = 
# 472 "parser/parser.mly"
  ( let n = int_of_string boff in n, n)
# 13984 "parser/parser.ml"
                 in
                
# 455 "parser/parser.mly"
  ( let bitsize = Utils.get_opt_or_default 1 sz_opt |> Size.Bit.create in
    let lo, hi = offs in Dba.LValue._restrict id bitsize lo hi )
# 13990 "parser/parser.ml"
                 in
                _menhir_goto_lvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv656)) : 'freshtv658)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv659 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 14000 "parser/parser.ml"
                )) * _menhir_state * 'tv_option_size_annot_)) * (
# 61 "parser/parser.mly"
       (string)
# 14004 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv660)) : 'freshtv662)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv663 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 14015 "parser/parser.ml"
            )) * _menhir_state * 'tv_option_size_annot_)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv664)) : 'freshtv666)
    | ASSIGN | COMMA | RBRACE | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv667 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 14024 "parser/parser.ml"
        )) * _menhir_state * 'tv_option_size_annot_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (id : (
# 62 "parser/parser.mly"
       (string)
# 14029 "parser/parser.ml"
        ))), _, (sz_opt : 'tv_option_size_annot_)) = _menhir_stack in
        let _v : 'tv_lvalue = 
# 452 "parser/parser.mly"
  ( let bitsize = Utils.get_opt_or_default 1 sz_opt |> Size.Bit.create in
    Dba.LValue.var id ~bitsize )
# 14035 "parser/parser.ml"
         in
        _menhir_goto_lvalue _menhir_env _menhir_stack _menhir_s _v) : 'freshtv668)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv669 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 14045 "parser/parser.ml"
        )) * _menhir_state * 'tv_option_size_annot_) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv670)) : 'freshtv672)

and _menhir_run25 : _menhir_env -> 'ttv_tail * _menhir_state -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COMMA ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv639 * _menhir_state) * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState26 _v
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState26 _v
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState26 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState26) : 'freshtv640)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv641 * _menhir_state) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv642)

and _menhir_run120 : _menhir_env -> 'ttv_tail * _menhir_state -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COMMA ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv635 * _menhir_state) * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState121 _v
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState121 _v
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState121 _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState121) : 'freshtv636)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv637 * _menhir_state) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv638)

and _menhir_reduce263 : _menhir_env -> 'ttv_tail * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 14113 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack ->
    let (_menhir_stack, _menhir_s, (label : (
# 61 "parser/parser.mly"
       (string)
# 14119 "parser/parser.ml"
    ))) = _menhir_stack in
    let _v : 'tv_static_target = 
# 289 "parser/parser.mly"
                ( Dba.Jump_target.inner (int_of_string label) )
# 14124 "parser/parser.ml"
     in
    _menhir_goto_static_target _menhir_env _menhir_stack _menhir_s _v

and _menhir_run3 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv631 * _menhir_state) = Obj.magic _menhir_stack in
        let (_v : (
# 61 "parser/parser.mly"
       (string)
# 14140 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | SUPER ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv627 * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 14151 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv625 * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 14158 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), (size : (
# 61 "parser/parser.mly"
       (string)
# 14163 "parser/parser.ml"
            ))) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_size_annot = 
# 237 "parser/parser.mly"
                        ( int_of_string size )
# 14170 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv623) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_size_annot) = _v in
            ((match _menhir_s with
            | MenhirState199 | MenhirState2 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv613 * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 14182 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : 'tv_size_annot) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv611 * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 14190 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                let (_ : _menhir_state) = _menhir_s in
                let ((size : 'tv_size_annot) : 'tv_size_annot) = _v in
                ((let (_menhir_stack, _menhir_s, (value : (
# 61 "parser/parser.mly"
       (string)
# 14197 "parser/parser.ml"
                ))) = _menhir_stack in
                let _v : 'tv_constant = 
# 462 "parser/parser.mly"
  ( Bitvector.create (Bigint.big_int_of_string value) size )
# 14202 "parser/parser.ml"
                 in
                _menhir_goto_constant _menhir_env _menhir_stack _menhir_s _v) : 'freshtv612)) : 'freshtv614)
            | MenhirState30 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv617 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 14210 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : 'tv_size_annot) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv615 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 14218 "parser/parser.ml"
                )) = Obj.magic _menhir_stack in
                let (_ : _menhir_state) = _menhir_s in
                let ((x : 'tv_size_annot) : 'tv_size_annot) = _v in
                ((let (_menhir_stack, _menhir_s, (id : (
# 62 "parser/parser.mly"
       (string)
# 14225 "parser/parser.ml"
                ))) = _menhir_stack in
                let _v : 'tv_expr = let sz_opt = 
# 126 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 14230 "parser/parser.ml"
                 in
                
# 483 "parser/parser.mly"
  ( let sz = Utils.get_opt_or_default 1 sz_opt in
    Dba.Expr.var id sz )
# 14236 "parser/parser.ml"
                 in
                _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv616)) : 'freshtv618)
            | MenhirState136 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv621) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : 'tv_size_annot) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv619) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let ((x : 'tv_size_annot) : 'tv_size_annot) = _v in
                ((let _v : 'tv_option_size_annot_ = 
# 116 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 14251 "parser/parser.ml"
                 in
                _menhir_goto_option_size_annot_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv620)) : 'freshtv622)
            | _ ->
                _menhir_fail ()) : 'freshtv624)) : 'freshtv626)) : 'freshtv628)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv629 * _menhir_state) * (
# 61 "parser/parser.mly"
       (string)
# 14263 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv630)) : 'freshtv632)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv633 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv634)

and _menhir_goto_annotable_instruction : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_annotable_instruction -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv609 * _menhir_state * 'tv_annotable_instruction) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv605 * _menhir_state * 'tv_annotable_instruction) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | GOTO ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv599 * _menhir_state * 'tv_annotable_instruction)) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv595 * _menhir_state * 'tv_annotable_instruction))) = Obj.magic _menhir_stack in
                let (_v : (
# 61 "parser/parser.mly"
       (string)
# 14301 "parser/parser.ml"
                )) = _v in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv593 * _menhir_state * 'tv_annotable_instruction))) = Obj.magic _menhir_stack in
                let ((_2 : (
# 61 "parser/parser.mly"
       (string)
# 14309 "parser/parser.ml"
                )) : (
# 61 "parser/parser.mly"
       (string)
# 14313 "parser/parser.ml"
                )) = _v in
                ((let (_menhir_stack, _menhir_s, (instr : 'tv_annotable_instruction)) = _menhir_stack in
                let _1_inlined1 = () in
                let _1 = () in
                let _v : 'tv_instruction = let addr =
                  let addr =
                    let x =
                      let _1 = _1_inlined1 in
                      
# 426 "parser/parser.mly"
           ( int_of_string _2 )
# 14325 "parser/parser.ml"
                      
                    in
                    
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 14331 "parser/parser.ml"
                    
                  in
                  
# 277 "parser/parser.mly"
                                           ( addr )
# 14337 "parser/parser.ml"
                  
                in
                
# 399 "parser/parser.mly"
  ( Dba_types.Instruction.set_successor instr addr )
# 14343 "parser/parser.ml"
                 in
                _menhir_goto_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv594)) : 'freshtv596)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv597 * _menhir_state * 'tv_annotable_instruction))) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv598)) : 'freshtv600)
        | EOF | HEXA _ | INT _ | LPAR ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv601 * _menhir_state * 'tv_annotable_instruction)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (instr : 'tv_annotable_instruction)) = _menhir_stack in
            let _1 = () in
            let _v : 'tv_instruction = let addr =
              let addr =
                let x = 
# 427 "parser/parser.mly"
           ( cur_address () )
# 14363 "parser/parser.ml"
                 in
                
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 14368 "parser/parser.ml"
                
              in
              
# 277 "parser/parser.mly"
                                           ( addr )
# 14374 "parser/parser.ml"
              
            in
            
# 399 "parser/parser.mly"
  ( Dba_types.Instruction.set_successor instr addr )
# 14380 "parser/parser.ml"
             in
            _menhir_goto_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv602)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv603 * _menhir_state * 'tv_annotable_instruction)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv604)) : 'freshtv606)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv607 * _menhir_state * 'tv_annotable_instruction) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv608)) : 'freshtv610)

and _menhir_goto_list_patch_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_patch_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState555 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv577 * _menhir_state * 'tv_patch) * _menhir_state * 'tv_list_patch_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv575 * _menhir_state * 'tv_patch) * _menhir_state * 'tv_list_patch_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_patch)), _, (xs : 'tv_list_patch_)) = _menhir_stack in
        let _v : 'tv_list_patch_ = 
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 14411 "parser/parser.ml"
         in
        _menhir_goto_list_patch_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv576)) : 'freshtv578)
    | MenhirState543 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv591 * _menhir_state * 'tv_list_patch_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv587 * _menhir_state * 'tv_list_patch_) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv585 * _menhir_state * 'tv_list_patch_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, (patches : 'tv_list_patch_)) = _menhir_stack in
            let _2 = () in
            let _v : (
# 106 "parser/parser.mly"
      (Binstream.t Virtual_address.Map.t)
# 14430 "parser/parser.ml"
            ) = 
# 586 "parser/parser.mly"
                            ( mk_patches patches )
# 14434 "parser/parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv583) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 106 "parser/parser.mly"
      (Binstream.t Virtual_address.Map.t)
# 14442 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv581) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 106 "parser/parser.mly"
      (Binstream.t Virtual_address.Map.t)
# 14450 "parser/parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv579) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let ((_1 : (
# 106 "parser/parser.mly"
      (Binstream.t Virtual_address.Map.t)
# 14458 "parser/parser.ml"
            )) : (
# 106 "parser/parser.mly"
      (Binstream.t Virtual_address.Map.t)
# 14462 "parser/parser.ml"
            )) = _v in
            (Obj.magic _1 : 'freshtv580)) : 'freshtv582)) : 'freshtv584)) : 'freshtv586)) : 'freshtv588)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv589 * _menhir_state * 'tv_list_patch_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv590)) : 'freshtv592)
    | _ ->
        _menhir_fail ()

and _menhir_reduce192 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_localized_instruction_ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 14480 "parser/parser.ml"
     in
    _menhir_goto_list_localized_instruction_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run1 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState1 _v
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState1 _v
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState1 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState1

and _menhir_run407 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | IDENT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv571 * _menhir_state) = Obj.magic _menhir_stack in
        let (_v : (
# 62 "parser/parser.mly"
       (string)
# 14513 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | DOT ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv567 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 14524 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BIN _v ->
                _menhir_run413 _menhir_env (Obj.magic _menhir_stack) MenhirState409 _v
            | HEXA _v ->
                _menhir_run412 _menhir_env (Obj.magic _menhir_stack) MenhirState409 _v
            | INT _v ->
                _menhir_run411 _menhir_env (Obj.magic _menhir_stack) MenhirState409 _v
            | STRING _v ->
                _menhir_run410 _menhir_env (Obj.magic _menhir_stack) MenhirState409 _v
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState409) : 'freshtv568)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv569 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 14548 "parser/parser.ml"
            )) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv570)) : 'freshtv572)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv573 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv574)

and _menhir_reduce190 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_labelled_instruction_ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 14565 "parser/parser.ml"
     in
    _menhir_goto_list_labelled_instruction_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run435 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 61 "parser/parser.mly"
       (string)
# 14572 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | COLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv563 * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 14584 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ASSERT ->
            _menhir_run223 _menhir_env (Obj.magic _menhir_stack) MenhirState436
        | ASSUME ->
            _menhir_run215 _menhir_env (Obj.magic _menhir_stack) MenhirState436
        | AT ->
            _menhir_run145 _menhir_env (Obj.magic _menhir_stack) MenhirState436
        | FREE ->
            _menhir_run211 _menhir_env (Obj.magic _menhir_stack) MenhirState436
        | GOTO ->
            _menhir_run196 _menhir_env (Obj.magic _menhir_stack) MenhirState436
        | IDENT _v ->
            _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState436 _v
        | IF ->
            _menhir_run175 _menhir_env (Obj.magic _menhir_stack) MenhirState436
        | NONDETASSUME ->
            _menhir_run133 _menhir_env (Obj.magic _menhir_stack) MenhirState436
        | PRINT ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState436
        | STOP ->
            _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState436
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState436) : 'freshtv564)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv565 * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 14620 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv566)

and _menhir_reduce188 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_dhunk_substitution_ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 14630 "parser/parser.ml"
     in
    _menhir_goto_list_dhunk_substitution_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run445 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 63 "parser/parser.mly"
       (string)
# 14637 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | ARROW ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv559 * _menhir_state * (
# 63 "parser/parser.mly"
       (string)
# 14649 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INT _v ->
            _menhir_run435 _menhir_env (Obj.magic _menhir_stack) MenhirState446 _v
        | EOF | HEXA _ ->
            _menhir_reduce190 _menhir_env (Obj.magic _menhir_stack) MenhirState446
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState446) : 'freshtv560)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv561 * _menhir_state * (
# 63 "parser/parser.mly"
       (string)
# 14669 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv562)

and _menhir_run454 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 61 "parser/parser.mly"
       (string)
# 14677 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv557) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((n : (
# 61 "parser/parser.mly"
       (string)
# 14687 "parser/parser.ml"
    )) : (
# 61 "parser/parser.mly"
       (string)
# 14691 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_integer = 
# 572 "parser/parser.mly"
             ( int_of_string n )
# 14696 "parser/parser.ml"
     in
    _menhir_goto_integer _menhir_env _menhir_stack _menhir_s _v) : 'freshtv558)

and _menhir_run455 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | IDENT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv553) = Obj.magic _menhir_stack in
        let (_v : (
# 62 "parser/parser.mly"
       (string)
# 14712 "parser/parser.ml"
        )) = _v in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv551) = Obj.magic _menhir_stack in
        let ((id : (
# 62 "parser/parser.mly"
       (string)
# 14720 "parser/parser.ml"
        )) : (
# 62 "parser/parser.mly"
       (string)
# 14724 "parser/parser.ml"
        )) = _v in
        ((let _v : 'tv_bin_loc_with_id = 
# 624 "parser/parser.mly"
             ( Loader_utils.Binary_loc.name id )
# 14729 "parser/parser.ml"
         in
        _menhir_goto_bin_loc_with_id _menhir_env _menhir_stack _v) : 'freshtv552)) : 'freshtv554)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv555 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv556)

and _menhir_run460 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 63 "parser/parser.mly"
       (string)
# 14743 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv549) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((n : (
# 63 "parser/parser.mly"
       (string)
# 14753 "parser/parser.ml"
    )) : (
# 63 "parser/parser.mly"
       (string)
# 14757 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_integer = 
# 572 "parser/parser.mly"
             ( int_of_string n )
# 14762 "parser/parser.ml"
     in
    _menhir_goto_integer _menhir_env _menhir_stack _menhir_s _v) : 'freshtv550)

and _menhir_run461 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 64 "parser/parser.mly"
       (string)
# 14769 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv547) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((n : (
# 64 "parser/parser.mly"
       (string)
# 14779 "parser/parser.ml"
    )) : (
# 64 "parser/parser.mly"
       (string)
# 14783 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_integer = 
# 572 "parser/parser.mly"
             ( int_of_string n )
# 14788 "parser/parser.ml"
     in
    _menhir_goto_integer _menhir_env _menhir_stack _menhir_s _v) : 'freshtv548)

and _menhir_run22 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState22 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState22 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState22 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState22 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState22
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState22
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
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState23 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState23 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState23 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState23 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState23

and _menhir_run24 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState24
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState24 _v
    | CONSTANT ->
        _menhir_run120 _menhir_env (Obj.magic _menhir_stack) MenhirState24
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState24
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState24
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState24 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState24 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState24
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState24 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState24
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState24
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState24
    | STACK ->
        _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState24
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState24

and _menhir_run2 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 61 "parser/parser.mly"
       (string)
# 14898 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INFER ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState2
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState2

and _menhir_run29 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState29 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState29 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState29 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState29 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState29
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState29

and _menhir_run30 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 62 "parser/parser.mly"
       (string)
# 14948 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INFER ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState30
    | AND | ANNOT | COMMA | CONCAT | DOTDOT | ELSE | EOF | EQUAL | GES | GEU | GOTO | GTS | GTU | HEXA _ | INT _ | LBRACE | LES | LEU | LPAR | LROTATE | LSHIFT | LTS | LTU | MINUS | MODS | MODU | NEQ | OR | PLUS | RBRACE | RBRACKETS | RBRACKETU | RPAR | RROTATE | RSHIFTS | RSHIFTU | SEMICOLON | SLASH_S | SLASH_U | STAR | STAR_S | STAR_U | THEN | XOR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv545 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 14962 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (id : (
# 62 "parser/parser.mly"
       (string)
# 14967 "parser/parser.ml"
        ))) = _menhir_stack in
        let _v : 'tv_expr = let sz_opt = 
# 124 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 14972 "parser/parser.ml"
         in
        
# 483 "parser/parser.mly"
  ( let sz = Utils.get_opt_or_default 1 sz_opt in
    Dba.Expr.var id sz )
# 14978 "parser/parser.ml"
         in
        _menhir_goto_expr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv546)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState30

and _menhir_run7 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 63 "parser/parser.mly"
       (string)
# 14989 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv543) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((value : (
# 63 "parser/parser.mly"
       (string)
# 14999 "parser/parser.ml"
    )) : (
# 63 "parser/parser.mly"
       (string)
# 15003 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_constant = 
# 465 "parser/parser.mly"
  ( Bitvector.of_string value )
# 15008 "parser/parser.ml"
     in
    _menhir_goto_constant _menhir_env _menhir_stack _menhir_s _v) : 'freshtv544)

and _menhir_run32 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState32
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState32 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState32
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState32
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState32 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState32 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState32
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState32 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState32
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState32
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState32
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState32

and _menhir_run33 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState33 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState33 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState33 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState33 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState33
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState33

and _menhir_run8 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 64 "parser/parser.mly"
       (string)
# 15081 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv541) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((value : (
# 64 "parser/parser.mly"
       (string)
# 15091 "parser/parser.ml"
    )) : (
# 64 "parser/parser.mly"
       (string)
# 15095 "parser/parser.ml"
    )) = _v in
    ((let _v : 'tv_constant = 
# 465 "parser/parser.mly"
  ( Bitvector.of_string value )
# 15100 "parser/parser.ml"
     in
    _menhir_goto_constant _menhir_env _menhir_stack _menhir_s _v) : 'freshtv542)

and _menhir_run34 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACKET ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv537 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState35
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState35 _v
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState35
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState35
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState35 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState35 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState35
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState35 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState35
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState35
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState35
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState35) : 'freshtv538)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv539 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv540)

and _menhir_reduce38 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_boption_UNCONTROLLED_ = 
# 133 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( false )
# 15155 "parser/parser.ml"
     in
    _menhir_goto_boption_UNCONTROLLED_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_reduce202 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_terminated_initialization_directive_SEMICOLON__ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 15164 "parser/parser.ml"
     in
    _menhir_goto_list_terminated_initialization_directive_SEMICOLON__ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run505 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv535) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_boption_UNCONTROLLED_ = 
# 135 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( true )
# 15178 "parser/parser.ml"
     in
    _menhir_goto_boption_UNCONTROLLED_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv536)

and _menhir_run16 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | SKO ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv525) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv523) = Obj.magic _menhir_stack in
        ((let _1 = () in
        let _v : 'tv_option_stop_annot_ = let x = 
# 312 "parser/parser.mly"
      ( Dba.KO )
# 15198 "parser/parser.ml"
         in
        
# 116 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 15203 "parser/parser.ml"
         in
        _menhir_goto_option_stop_annot_ _menhir_env _menhir_stack _v) : 'freshtv524)) : 'freshtv526)
    | SOK ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv529) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv527) = Obj.magic _menhir_stack in
        ((let _1 = () in
        let _v : 'tv_option_stop_annot_ = let x = 
# 313 "parser/parser.mly"
      ( Dba.OK )
# 15216 "parser/parser.ml"
         in
        
# 116 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 15221 "parser/parser.ml"
         in
        _menhir_goto_option_stop_annot_ _menhir_env _menhir_stack _v) : 'freshtv528)) : 'freshtv530)
    | EOF | HEXA _ | INT _ | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv531) = Obj.magic _menhir_stack in
        ((let _v : 'tv_option_stop_annot_ = 
# 114 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 15230 "parser/parser.ml"
         in
        _menhir_goto_option_stop_annot_ _menhir_env _menhir_stack _v) : 'freshtv532)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv533 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv534)

and _menhir_run20 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState20
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState20 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState20
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState20
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState20 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState20 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState20
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState20 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState20
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState20
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState20
    | STRING _v ->
        _menhir_run21 _menhir_env (Obj.magic _menhir_stack) MenhirState20 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState20

and _menhir_run133 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv519 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LBRACE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv515) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | AT ->
                _menhir_run145 _menhir_env (Obj.magic _menhir_stack) MenhirState135
            | IDENT _v ->
                _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState135 _v
            | RBRACE ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv513) = Obj.magic _menhir_stack in
                let (_menhir_s : _menhir_state) = MenhirState135 in
                ((let _v : 'tv_loption_separated_nonempty_list_COMMA_lvalue__ = 
# 142 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 15305 "parser/parser.ml"
                 in
                _menhir_goto_loption_separated_nonempty_list_COMMA_lvalue__ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv514)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState135) : 'freshtv516)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv517 * _menhir_state)) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv518)) : 'freshtv520)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv521 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv522)

and _menhir_run175 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState175
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState175 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState175
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState175
    | FALSE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv505 * _menhir_state) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState175 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | GOTO ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv501 * _menhir_state) * _menhir_state) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                _menhir_run178 _menhir_env (Obj.magic _menhir_stack) MenhirState185 _v
            | LPAR ->
                _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState185
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState185) : 'freshtv502)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv503 * _menhir_state) * _menhir_state) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv504)) : 'freshtv506)
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState175 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState175 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState175
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState175 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState175
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState175
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState175
    | TRUE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv511 * _menhir_state) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState175 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | GOTO ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv507 * _menhir_state) * _menhir_state) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | INT _v ->
                _menhir_run178 _menhir_env (Obj.magic _menhir_stack) MenhirState177 _v
            | LPAR ->
                _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState177
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState177) : 'freshtv508)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv509 * _menhir_state) * _menhir_state) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv510)) : 'freshtv512)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState175

and _menhir_run136 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 62 "parser/parser.mly"
       (string)
# 15421 "parser/parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INFER ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState136
    | ASSIGN | COMMA | LBRACE | RBRACE | SEMICOLON ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv499) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState136 in
        ((let _v : 'tv_option_size_annot_ = 
# 114 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 15437 "parser/parser.ml"
         in
        _menhir_goto_option_size_annot_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv500)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState136

and _menhir_run196 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState196
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState196 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState196
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState196
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState196 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState196 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState196
    | INT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv495) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState196 in
        let (_v : (
# 61 "parser/parser.mly"
       (string)
# 15472 "parser/parser.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | INFER ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState199
        | ANNOT | EOF | HEXA _ | INT _ | LPAR ->
            _menhir_reduce263 _menhir_env (Obj.magic _menhir_stack)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState199) : 'freshtv496)
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv497) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState196 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState197
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState197 _v
        | CONSTANT ->
            _menhir_run120 _menhir_env (Obj.magic _menhir_stack) MenhirState197
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState197
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState197
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState197 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState197 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState197
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState197 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState197
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState197
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState197
        | STACK ->
            _menhir_run25 _menhir_env (Obj.magic _menhir_stack) MenhirState197
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState197) : 'freshtv498)
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState196
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState196
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState196

and _menhir_run211 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
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
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState212
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState212 _v
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState212
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState212
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState212 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState212 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState212
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState212 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState212
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState212
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState212
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState212) : 'freshtv492)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv493 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv494)

and _menhir_run145 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LBRACKET ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv487 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState146
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState146 _v
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState146
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState146
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState146 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState146 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState146
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState146 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState146
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState146
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState146
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState146) : 'freshtv488)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv489 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv490)

and _menhir_run215 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv483 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState216
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState216 _v
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState216
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState216
        | FALSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv473 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState216 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv469 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv467 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                let _3 = () in
                let _1_inlined2 = () in
                let _1_inlined1 = () in
                let _1 = () in
                let _v : 'tv_annotable_instruction = let condition =
                  let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                  let x =
                    let _1 = _1_inlined1 in
                    
# 545 "parser/parser.mly"
            ( Dba.Expr.zero )
# 15671 "parser/parser.ml"
                    
                  in
                  
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 15677 "parser/parser.ml"
                  
                in
                
# 381 "parser/parser.mly"
  ( Dba.Instr.assume condition unknown_successor )
# 15683 "parser/parser.ml"
                 in
                _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv468)) : 'freshtv470)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv471 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv472)) : 'freshtv474)
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState216 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState216 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState216
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState216 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState216
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState216
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState216
        | TRUE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv481 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState216 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv477 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv475 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                let _3 = () in
                let _1_inlined2 = () in
                let _1_inlined1 = () in
                let _1 = () in
                let _v : 'tv_annotable_instruction = let condition =
                  let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                  let x =
                    let _1 = _1_inlined1 in
                    
# 544 "parser/parser.mly"
            ( Dba.Expr.one )
# 15733 "parser/parser.ml"
                    
                  in
                  
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 15739 "parser/parser.ml"
                  
                in
                
# 381 "parser/parser.mly"
  ( Dba.Instr.assume condition unknown_successor )
# 15745 "parser/parser.ml"
                 in
                _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv476)) : 'freshtv478)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv479 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv480)) : 'freshtv482)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState216) : 'freshtv484)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv485 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv486)

and _menhir_run223 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv463 * _menhir_state) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AT ->
            _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState224
        | BIN _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState224 _v
        | EXTS ->
            _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState224
        | EXTU ->
            _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState224
        | FALSE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv453 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState224 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv449 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv447 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                let _3 = () in
                let _1_inlined2 = () in
                let _1_inlined1 = () in
                let _1 = () in
                let _v : 'tv_annotable_instruction = let condition =
                  let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                  let x =
                    let _1 = _1_inlined1 in
                    
# 545 "parser/parser.mly"
            ( Dba.Expr.zero )
# 15813 "parser/parser.ml"
                    
                  in
                  
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 15819 "parser/parser.ml"
                  
                in
                
# 379 "parser/parser.mly"
  ( Dba.Instr._assert condition unknown_successor )
# 15825 "parser/parser.ml"
                 in
                _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv448)) : 'freshtv450)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv451 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv452)) : 'freshtv454)
        | HEXA _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState224 _v
        | IDENT _v ->
            _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState224 _v
        | IF ->
            _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState224
        | INT _v ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState224 _v
        | LPAR ->
            _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState224
        | MINUS ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState224
        | NOT ->
            _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState224
        | TRUE ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv461 * _menhir_state)) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState224 in
            ((let _menhir_stack = (_menhir_stack, _menhir_s) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAR ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv457 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv455 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
                let _3 = () in
                let _1_inlined2 = () in
                let _1_inlined1 = () in
                let _1 = () in
                let _v : 'tv_annotable_instruction = let condition =
                  let (_1_inlined1, _1) = (_1_inlined2, _1_inlined1) in
                  let x =
                    let _1 = _1_inlined1 in
                    
# 544 "parser/parser.mly"
            ( Dba.Expr.one )
# 15875 "parser/parser.ml"
                    
                  in
                  
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 15881 "parser/parser.ml"
                  
                in
                
# 379 "parser/parser.mly"
  ( Dba.Instr._assert condition unknown_successor )
# 15887 "parser/parser.ml"
                 in
                _menhir_goto_annotable_instruction _menhir_env _menhir_stack _menhir_s _v) : 'freshtv456)) : 'freshtv458)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv459 * _menhir_state)) * _menhir_state) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv460)) : 'freshtv462)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState224) : 'freshtv464)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv465 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv466)

and _menhir_errorcase : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    match _menhir_s with
    | MenhirState555 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv31 * _menhir_state * 'tv_patch) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv32)
    | MenhirState550 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv33 * _menhir_state * 'tv_integer) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv34)
    | MenhirState547 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv35) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv36)
    | MenhirState544 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv37 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv38)
    | MenhirState543 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv39) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv40)
    | MenhirState539 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv41) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv42)
    | MenhirState526 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv43 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv44)
    | MenhirState525 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv45 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv46)
    | MenhirState522 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv47 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv48)
    | MenhirState519 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv49 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_either_DOTDOT_COMMA_) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv50)
    | MenhirState518 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv51 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state * 'tv_either_DOTDOT_COMMA_) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv52)
    | MenhirState515 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv53 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv54)
    | MenhirState514 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv55 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv56)
    | MenhirState513 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv57 * _menhir_state * 'tv_lvalue)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv58)
    | MenhirState511 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv59 * _menhir_state * 'tv_boption_UNCONTROLLED_) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv60)
    | MenhirState509 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv61 * _menhir_state * 'tv_initialization_directive)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv62)
    | MenhirState504 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv63) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv64)
    | MenhirState502 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv65 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv66)
    | MenhirState500 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv67) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv68)
    | MenhirState496 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv69) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv70)
    | MenhirState495 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv71) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv72)
    | MenhirState494 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv73) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv74)
    | MenhirState492 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv75) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv76)
    | MenhirState489 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv77) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv78)
    | MenhirState486 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv79) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv80)
    | MenhirState485 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv81) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv82)
    | MenhirState484 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv83) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv84)
    | MenhirState482 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv85) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv86)
    | MenhirState480 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv87) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv88)
    | MenhirState477 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv89)) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv90)
    | MenhirState472 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv91) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv92)
    | MenhirState468 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv93 * _menhir_state * 'tv_located_directive)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv94)
    | MenhirState463 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv95 * 'tv_bin_loc_with_id)) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv96)
    | MenhirState459 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv97 * 'tv_bin_loc_with_id)) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv98)
    | MenhirState453 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv99) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv100)
    | MenhirState451 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv101 * _menhir_state * 'tv_dhunk_substitution) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv102)
    | MenhirState446 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv103 * _menhir_state * (
# 63 "parser/parser.mly"
       (string)
# 16080 "parser/parser.ml"
        ))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv104)
    | MenhirState444 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv105) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv106)
    | MenhirState439 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv107 * _menhir_state * 'tv_labelled_instruction) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv108)
    | MenhirState436 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv109 * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 16098 "parser/parser.ml"
        ))) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv110)
    | MenhirState434 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv111) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv112)
    | MenhirState432 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv113 * _menhir_state * (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 16111 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv114)
    | MenhirState429 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv115 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv116)
    | MenhirState428 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv117 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv118)
    | MenhirState427 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv119 * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv120)
    | MenhirState426 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv121) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv122)
    | MenhirState421 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv123 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 16139 "parser/parser.ml"
        ))) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv124)
    | MenhirState418 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv125 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv126)
    | MenhirState417 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv127 * _menhir_state * 'tv_kv) * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv128)
    | MenhirState416 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv129 * _menhir_state * 'tv_kv) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv130)
    | MenhirState409 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv131 * _menhir_state) * (
# 62 "parser/parser.mly"
       (string)
# 16163 "parser/parser.ml"
        ))) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv132)
    | MenhirState406 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv133) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv134)
    | MenhirState404 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv135 * _menhir_state * 'tv_declaration)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv136)
    | MenhirState401 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv137 * _menhir_state * 'tv_assignment)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv138)
    | MenhirState398 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv139 * 'tv_config) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) * 'tv_option_permission_block_) * _menhir_state * 'tv_list_terminated_assignment_SEMICOLON__) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv140)
    | MenhirState397 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv141 * 'tv_config) * _menhir_state * 'tv_list_terminated_declaration_SEMICOLON__) * 'tv_option_permission_block_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv142)
    | MenhirState391 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv143 * _menhir_state * 'tv_permission) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv144)
    | MenhirState389 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv145 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv146)
    | MenhirState386 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv147 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv148)
    | MenhirState370 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv149 * _menhir_state * 'tv_predicate) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv150)
    | MenhirState360 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv151 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv152)
    | MenhirState358 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv153 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv154)
    | MenhirState356 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv155 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv156)
    | MenhirState354 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv157 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv158)
    | MenhirState352 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv159 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv160)
    | MenhirState343 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv161 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv162)
    | MenhirState341 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv163 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv164)
    | MenhirState339 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv165 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv166)
    | MenhirState337 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv167 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv168)
    | MenhirState335 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv169 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv170)
    | MenhirState333 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv171 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv172)
    | MenhirState331 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv173 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv174)
    | MenhirState329 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv175 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv176)
    | MenhirState327 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv177 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv178)
    | MenhirState325 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv179 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv180)
    | MenhirState323 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv181 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv182)
    | MenhirState321 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv183 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv184)
    | MenhirState319 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv185 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv186)
    | MenhirState317 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv187 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv188)
    | MenhirState315 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv189 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv190)
    | MenhirState313 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv191 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv192)
    | MenhirState311 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv193 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv194)
    | MenhirState309 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv195 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv196)
    | MenhirState307 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv197 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv198)
    | MenhirState305 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv199 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv200)
    | MenhirState303 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv201 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv202)
    | MenhirState300 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv203 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv204)
    | MenhirState298 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv205 * _menhir_state * 'tv_constant_expr)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv206)
    | MenhirState296 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv207 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv208)
    | MenhirState295 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv209 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv210)
    | MenhirState294 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv211 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv212)
    | MenhirState293 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv213 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv214)
    | MenhirState292 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv215 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv216)
    | MenhirState289 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv217 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv218)
    | MenhirState287 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv219)) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv220)
    | MenhirState265 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv221 * 'tv_config) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv222)
    | MenhirState261 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv223)) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv224)
    | MenhirState248 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv225 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv226)
    | MenhirState245 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv227 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv228)
    | MenhirState244 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv229 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv230)
    | MenhirState232 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv231 * _menhir_state * 'tv_lvalue)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv232)
    | MenhirState229 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv233 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv234)
    | MenhirState224 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv235 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv236)
    | MenhirState221 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv237 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv238)
    | MenhirState216 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv239 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv240)
    | MenhirState213 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv241 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv242)
    | MenhirState212 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv243 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv244)
    | MenhirState208 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv245 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv246)
    | MenhirState203 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv247 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv248)
    | MenhirState200 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv249 * _menhir_state * 'tv_static_target) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv250)
    | MenhirState199 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv251 * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 16463 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv252)
    | MenhirState197 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv253 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv254)
    | MenhirState196 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv255 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv256)
    | MenhirState191 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv257 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv258)
    | MenhirState190 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv259 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv260)
    | MenhirState185 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv261 * _menhir_state) * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv262)
    | MenhirState177 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv263 * _menhir_state) * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv264)
    | MenhirState175 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv265 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv266)
    | MenhirState172 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv267 * _menhir_state)) * 'tv_set_of_lvalue_)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv268)
    | MenhirState167 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv269 * _menhir_state)) * 'tv_set_of_lvalue_)) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv270)
    | MenhirState161 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv271 * _menhir_state * 'tv_lvalue)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv272)
    | MenhirState147 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv273 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv274)
    | MenhirState146 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv275 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv276)
    | MenhirState136 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv277 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 16532 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv278)
    | MenhirState135 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv279) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv280)
    | MenhirState132 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv281 * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv282)
    | MenhirState130 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv283 * _menhir_state * 'tv_printarg)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv284)
    | MenhirState126 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv285 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv286)
    | MenhirState124 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv287 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv288)
    | MenhirState121 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv289 * _menhir_state) * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv290)
    | MenhirState119 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv291 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv292)
    | MenhirState118 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv293 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv294)
    | MenhirState117 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv295 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv296)
    | MenhirState116 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv297 * _menhir_state) * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv298)
    | MenhirState115 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv299 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv300)
    | MenhirState113 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv301 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv302)
    | MenhirState111 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv303 * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv304)
    | MenhirState99 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv305 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv306)
    | MenhirState98 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv307 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv308)
    | MenhirState97 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv309 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv310)
    | MenhirState96 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv311 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv312)
    | MenhirState95 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv313 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv314)
    | MenhirState94 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv315 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv316)
    | MenhirState93 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv317 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv318)
    | MenhirState92 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv319 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv320)
    | MenhirState91 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv321 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv322)
    | MenhirState90 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv323 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv324)
    | MenhirState83 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv325 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv326)
    | MenhirState82 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv327 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv328)
    | MenhirState81 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv329 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv330)
    | MenhirState80 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv331 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv332)
    | MenhirState79 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv333 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv334)
    | MenhirState78 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv335 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv336)
    | MenhirState77 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv337 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv338)
    | MenhirState76 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv339 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv340)
    | MenhirState75 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv341 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv342)
    | MenhirState74 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv343 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv344)
    | MenhirState73 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv345 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv346)
    | MenhirState72 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv347 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv348)
    | MenhirState71 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv349 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv350)
    | MenhirState70 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv351 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv352)
    | MenhirState69 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv353 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv354)
    | MenhirState68 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv355 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv356)
    | MenhirState67 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv357 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv358)
    | MenhirState66 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv359 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv360)
    | MenhirState65 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv361 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv362)
    | MenhirState64 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv363 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv364)
    | MenhirState63 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv365 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv366)
    | MenhirState62 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv367 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv368)
    | MenhirState61 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv369 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv370)
    | MenhirState60 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv371 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv372)
    | MenhirState59 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv373 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv374)
    | MenhirState58 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv375 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv376)
    | MenhirState57 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv377 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv378)
    | MenhirState56 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv379 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv380)
    | MenhirState55 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv381 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv382)
    | MenhirState54 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv383 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv384)
    | MenhirState53 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv385 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv386)
    | MenhirState52 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv387 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv388)
    | MenhirState51 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv389 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv390)
    | MenhirState50 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv391 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv392)
    | MenhirState49 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv393 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv394)
    | MenhirState48 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv395 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv396)
    | MenhirState47 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv397 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv398)
    | MenhirState46 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv399 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv400)
    | MenhirState45 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv401 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv402)
    | MenhirState44 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv403 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv404)
    | MenhirState43 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv405 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv406)
    | MenhirState42 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv407 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv408)
    | MenhirState39 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv409 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv410)
    | MenhirState38 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv411 * _menhir_state * 'tv_expr) * _menhir_state) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv412)
    | MenhirState37 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv413 * _menhir_state * 'tv_expr) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv414)
    | MenhirState36 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv415 * _menhir_state)) * _menhir_state * 'tv_expr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv416)
    | MenhirState35 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv417 * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv418)
    | MenhirState33 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv419 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv420)
    | MenhirState32 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv421 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv422)
    | MenhirState30 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv423 * _menhir_state * (
# 62 "parser/parser.mly"
       (string)
# 16900 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv424)
    | MenhirState29 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv425 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv426)
    | MenhirState26 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv427 * _menhir_state) * _menhir_state)) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv428)
    | MenhirState24 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv429 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv430)
    | MenhirState23 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv431 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv432)
    | MenhirState22 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv433 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv434)
    | MenhirState20 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv435 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv436)
    | MenhirState15 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv437 * _menhir_state * 'tv_address) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv438)
    | MenhirState13 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv439 * _menhir_state * 'tv_localized_instruction) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv440)
    | MenhirState2 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv441 * _menhir_state * (
# 61 "parser/parser.mly"
       (string)
# 16949 "parser/parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv442)
    | MenhirState1 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv443 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv444)
    | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv445) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv446)

and _menhir_reduce194 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_patch_ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 16968 "parser/parser.ml"
     in
    _menhir_goto_list_patch_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run544 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run461 _menhir_env (Obj.magic _menhir_stack) MenhirState544 _v
    | HEXA _v ->
        _menhir_run460 _menhir_env (Obj.magic _menhir_stack) MenhirState544 _v
    | INT _v ->
        _menhir_run454 _menhir_env (Obj.magic _menhir_stack) MenhirState544 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState544

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

and _menhir_init : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> _menhir_env =
  fun lexer lexbuf ->
    let _tok = Obj.magic () in
    {
      _menhir_lexer = lexer;
      _menhir_lexbuf = lexbuf;
      _menhir_token = _tok;
      _menhir_error = false;
    }

and body : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 97 "parser/parser.mly"
      ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 17014 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv29) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | EOF ->
        _menhir_reduce192 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState0) : 'freshtv30))

and dba : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 85 "parser/parser.mly"
      ('a Dba_types.program)
# 17035 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv27) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | ENTRYPOINT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv23) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | COLON ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv19) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAR ->
                _menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState261
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState261) : 'freshtv20)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv21) = Obj.magic _menhir_stack in
            (raise _eRR : 'freshtv22)) : 'freshtv24)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv25) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv26)) : 'freshtv28))

and decoder_base : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 103 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list)
# 17078 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv17) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        _menhir_run407 _menhir_env (Obj.magic _menhir_stack) MenhirState406
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState406) : 'freshtv18))

and decoder_msg : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 100 "parser/parser.mly"
      ((string * Parse_helpers.Message.Value.t)  list * (Dba_types.Caddress.Map.key * Dba.Instr.t) list)
# 17097 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv15) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        _menhir_run407 _menhir_env (Obj.magic _menhir_stack) MenhirState426
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState426) : 'freshtv16))

and dhunk_eof : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 88 "parser/parser.mly"
      (Dhunk.t)
# 17116 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv13) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | INT _v ->
        _menhir_run435 _menhir_env (Obj.magic _menhir_stack) MenhirState434 _v
    | EOF ->
        _menhir_reduce190 _menhir_env (Obj.magic _menhir_stack) MenhirState434
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState434) : 'freshtv14))

and dhunk_substitutions_eof : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 94 "parser/parser.mly"
      ((Virtual_address.t * Dhunk.t) list)
# 17137 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv11) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | HEXA _v ->
        _menhir_run445 _menhir_env (Obj.magic _menhir_stack) MenhirState444 _v
    | EOF ->
        _menhir_reduce188 _menhir_env (Obj.magic _menhir_stack) MenhirState444
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState444) : 'freshtv12))

and directives : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 112 "parser/parser.mly"
      (Directive.t list)
# 17158 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv9) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BIN _v ->
        _menhir_run461 _menhir_env (Obj.magic _menhir_stack) MenhirState453 _v
    | HEXA _v ->
        _menhir_run460 _menhir_env (Obj.magic _menhir_stack) MenhirState453 _v
    | INFER ->
        _menhir_run455 _menhir_env (Obj.magic _menhir_stack) MenhirState453
    | INT _v ->
        _menhir_run454 _menhir_env (Obj.magic _menhir_stack) MenhirState453 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState453) : 'freshtv10))

and expr_eof : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 82 "parser/parser.mly"
      (Dba.Expr.t)
# 17183 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv7) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AT ->
        _menhir_run34 _menhir_env (Obj.magic _menhir_stack) MenhirState500
    | BIN _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) MenhirState500 _v
    | EXTS ->
        _menhir_run33 _menhir_env (Obj.magic _menhir_stack) MenhirState500
    | EXTU ->
        _menhir_run32 _menhir_env (Obj.magic _menhir_stack) MenhirState500
    | HEXA _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) MenhirState500 _v
    | IDENT _v ->
        _menhir_run30 _menhir_env (Obj.magic _menhir_stack) MenhirState500 _v
    | IF ->
        _menhir_run29 _menhir_env (Obj.magic _menhir_stack) MenhirState500
    | INT _v ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState500 _v
    | LPAR ->
        _menhir_run24 _menhir_env (Obj.magic _menhir_stack) MenhirState500
    | MINUS ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState500
    | NOT ->
        _menhir_run22 _menhir_env (Obj.magic _menhir_stack) MenhirState500
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState500) : 'freshtv8))

and initialization : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 109 "parser/parser.mly"
      (Parse_helpers.Initialization.t list)
# 17222 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv5) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | UNCONTROLLED ->
        _menhir_run505 _menhir_env (Obj.magic _menhir_stack) MenhirState504
    | EOF ->
        _menhir_reduce202 _menhir_env (Obj.magic _menhir_stack) MenhirState504
    | AT | IDENT _ ->
        _menhir_reduce38 _menhir_env (Obj.magic _menhir_stack) MenhirState504
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState504) : 'freshtv6))

and instruction_eof : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 91 "parser/parser.mly"
      (Dba.Instr.t)
# 17245 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv3) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | ASSERT ->
        _menhir_run223 _menhir_env (Obj.magic _menhir_stack) MenhirState539
    | ASSUME ->
        _menhir_run215 _menhir_env (Obj.magic _menhir_stack) MenhirState539
    | AT ->
        _menhir_run145 _menhir_env (Obj.magic _menhir_stack) MenhirState539
    | FREE ->
        _menhir_run211 _menhir_env (Obj.magic _menhir_stack) MenhirState539
    | GOTO ->
        _menhir_run196 _menhir_env (Obj.magic _menhir_stack) MenhirState539
    | IDENT _v ->
        _menhir_run136 _menhir_env (Obj.magic _menhir_stack) MenhirState539 _v
    | IF ->
        _menhir_run175 _menhir_env (Obj.magic _menhir_stack) MenhirState539
    | NONDETASSUME ->
        _menhir_run133 _menhir_env (Obj.magic _menhir_stack) MenhirState539
    | PRINT ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) MenhirState539
    | STOP ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) MenhirState539
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState539) : 'freshtv4))

and patchmap : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 106 "parser/parser.mly"
      (Binstream.t Virtual_address.Map.t)
# 17282 "parser/parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv1) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAR ->
        _menhir_run544 _menhir_env (Obj.magic _menhir_stack) MenhirState543
    | EOF ->
        _menhir_reduce194 _menhir_env (Obj.magic _menhir_stack) MenhirState543
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState543) : 'freshtv2))

# 269 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
  

# 17303 "parser/parser.ml"
