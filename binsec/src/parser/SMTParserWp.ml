
module MenhirBasics = struct
  
  exception Error = Parsing.Parse_error
  
  type token = 
    | UNSATT
    | SATT
    | RPAREN
    | NUM of (
# 24 "parser/SMTParserWp.mly"
       (string)
# 14 "parser/SMTParserWp.ml"
  )
    | LPAREN
    | IDENT of (
# 23 "parser/SMTParserWp.mly"
       (string)
# 20 "parser/SMTParserWp.ml"
  )
    | ERROR_MSG
    | ERROR
    | EOF
  
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
  | MenhirState24
  | MenhirState22
  | MenhirState18
  | MenhirState5
  | MenhirState4
  | MenhirState0

let rec _menhir_goto_list_value_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_value_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState18 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv93 * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_list_value_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv91 * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((xs : 'tv_list_value_) : 'tv_list_value_) = _v in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_value)) = _menhir_stack in
        let _v : 'tv_list_value_ = 
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 64 "parser/SMTParserWp.ml"
         in
        _menhir_goto_list_value_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv92)) : 'freshtv94)
    | MenhirState4 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv97) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_list_value_) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv95) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((values : 'tv_list_value_) : 'tv_list_value_) = _v in
        ((let _v : 'tv_args = 
# 48 "parser/SMTParserWp.mly"
                         ( [values] )
# 79 "parser/SMTParserWp.ml"
         in
        _menhir_goto_args _menhir_env _menhir_stack _menhir_s _v) : 'freshtv96)) : 'freshtv98)
    | _ ->
        _menhir_fail ()

and _menhir_goto_value : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_value -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState5 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv87 * _menhir_state) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv83 * _menhir_state) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv81 * _menhir_state) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s), _, (v : 'tv_value)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_value = 
# 59 "parser/SMTParserWp.mly"
                                  ( v )
# 107 "parser/SMTParserWp.ml"
             in
            _menhir_goto_value _menhir_env _menhir_stack _menhir_s _v) : 'freshtv82)) : 'freshtv84)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv85 * _menhir_state) * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv86)) : 'freshtv88)
    | MenhirState18 | MenhirState4 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv89 * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | IDENT _v ->
            _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState18 _v
        | LPAREN ->
            _menhir_run5 _menhir_env (Obj.magic _menhir_stack) MenhirState18
        | RPAREN ->
            _menhir_reduce3 _menhir_env (Obj.magic _menhir_stack) MenhirState18
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState18) : 'freshtv90)
    | _ ->
        _menhir_fail ()

and _menhir_run7 : _menhir_env -> 'ttv_tail * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 139 "parser/SMTParserWp.ml"
) -> (
# 24 "parser/SMTParserWp.mly"
       (string)
# 143 "parser/SMTParserWp.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _v ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv79 * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 151 "parser/SMTParserWp.ml"
    )) = Obj.magic _menhir_stack in
    let ((n : (
# 24 "parser/SMTParserWp.mly"
       (string)
# 156 "parser/SMTParserWp.ml"
    )) : (
# 24 "parser/SMTParserWp.mly"
       (string)
# 160 "parser/SMTParserWp.ml"
    )) = _v in
    ((let (_menhir_stack, _menhir_s, (id : (
# 23 "parser/SMTParserWp.mly"
       (string)
# 165 "parser/SMTParserWp.ml"
    ))) = _menhir_stack in
    let _v : 'tv_value = 
# 57 "parser/SMTParserWp.mly"
                                  ( id, n )
# 170 "parser/SMTParserWp.ml"
     in
    _menhir_goto_value _menhir_env _menhir_stack _menhir_s _v) : 'freshtv80)

and _menhir_fail : unit -> 'a =
  fun () ->
    Printf.fprintf stderr "Internal failure -- please contact the parser generator's developers.\n%!";
    assert false

and _menhir_run1 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv77) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 188 "parser/SMTParserWp.ml"
    ) = 
# 41 "parser/SMTParserWp.mly"
        (
      Kernel_options.Logger.error "SMTParserWp: Unexpected token in main";
      None, [] )
# 194 "parser/SMTParserWp.ml"
     in
    _menhir_goto_main _menhir_env _menhir_stack _menhir_s _v) : 'freshtv78)

and _menhir_goto_verdict : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_verdict -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv75 * _menhir_state * 'tv_verdict) = Obj.magic _menhir_stack in
    ((if _menhir_env._menhir_error then
      _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState24
    else
      let _tok = _menhir_env._menhir_token in
      match _tok with
      | EOF ->
          _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState24
      | LPAREN ->
          _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState24
      | SATT ->
          _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState24
      | UNSATT ->
          _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState24
      | _ ->
          (assert (not _menhir_env._menhir_error);
          _menhir_env._menhir_error <- true;
          _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState24)) : 'freshtv76)

and _menhir_reduce3 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_value_ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 226 "parser/SMTParserWp.ml"
     in
    _menhir_goto_list_value_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run5 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | IDENT _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv73 * _menhir_state) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState5 in
        let (_v : (
# 23 "parser/SMTParserWp.mly"
       (string)
# 243 "parser/SMTParserWp.ml"
        )) = _v in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | IDENT _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv69 * _menhir_state) * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 254 "parser/SMTParserWp.ml"
            )) = Obj.magic _menhir_stack in
            let (_v : (
# 23 "parser/SMTParserWp.mly"
       (string)
# 259 "parser/SMTParserWp.ml"
            )) = _v in
            ((let _menhir_stack = (_menhir_stack, _v) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | NUM _v ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv65 * _menhir_state) * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 270 "parser/SMTParserWp.ml"
                )) * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 274 "parser/SMTParserWp.ml"
                )) = Obj.magic _menhir_stack in
                let (_v : (
# 24 "parser/SMTParserWp.mly"
       (string)
# 279 "parser/SMTParserWp.ml"
                )) = _v in
                ((let _menhir_stack = (_menhir_stack, _v) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | RPAREN ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv61 * _menhir_state) * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 290 "parser/SMTParserWp.ml"
                    )) * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 294 "parser/SMTParserWp.ml"
                    )) * (
# 24 "parser/SMTParserWp.mly"
       (string)
# 298 "parser/SMTParserWp.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let _menhir_env = _menhir_discard _menhir_env in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv59 * _menhir_state) * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 305 "parser/SMTParserWp.ml"
                    )) * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 309 "parser/SMTParserWp.ml"
                    )) * (
# 24 "parser/SMTParserWp.mly"
       (string)
# 313 "parser/SMTParserWp.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let ((((_menhir_stack, _menhir_s), _, (id1 : (
# 23 "parser/SMTParserWp.mly"
       (string)
# 318 "parser/SMTParserWp.ml"
                    ))), (id2 : (
# 23 "parser/SMTParserWp.mly"
       (string)
# 322 "parser/SMTParserWp.ml"
                    ))), (n : (
# 24 "parser/SMTParserWp.mly"
       (string)
# 326 "parser/SMTParserWp.ml"
                    ))) = _menhir_stack in
                    let _5 = () in
                    let _1 = () in
                    let _v : 'tv_memory_ident = 
# 53 "parser/SMTParserWp.mly"
  ( Format.sprintf "%s %s %s" id1 id2 n )
# 333 "parser/SMTParserWp.ml"
                     in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv57) = _menhir_stack in
                    let (_menhir_s : _menhir_state) = _menhir_s in
                    let (_v : 'tv_memory_ident) = _v in
                    ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv55 * _menhir_state * 'tv_memory_ident) = Obj.magic _menhir_stack in
                    ((assert (not _menhir_env._menhir_error);
                    let _tok = _menhir_env._menhir_token in
                    match _tok with
                    | NUM _v ->
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : 'freshtv51 * _menhir_state * 'tv_memory_ident) = Obj.magic _menhir_stack in
                        let (_v : (
# 24 "parser/SMTParserWp.mly"
       (string)
# 351 "parser/SMTParserWp.ml"
                        )) = _v in
                        ((let _menhir_env = _menhir_discard _menhir_env in
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : 'freshtv49 * _menhir_state * 'tv_memory_ident) = Obj.magic _menhir_stack in
                        let ((n : (
# 24 "parser/SMTParserWp.mly"
       (string)
# 359 "parser/SMTParserWp.ml"
                        )) : (
# 24 "parser/SMTParserWp.mly"
       (string)
# 363 "parser/SMTParserWp.ml"
                        )) = _v in
                        ((let (_menhir_stack, _menhir_s, (id : 'tv_memory_ident)) = _menhir_stack in
                        let _v : 'tv_value = 
# 58 "parser/SMTParserWp.mly"
                                  ( id, n )
# 369 "parser/SMTParserWp.ml"
                         in
                        _menhir_goto_value _menhir_env _menhir_stack _menhir_s _v) : 'freshtv50)) : 'freshtv52)
                    | _ ->
                        assert (not _menhir_env._menhir_error);
                        _menhir_env._menhir_error <- true;
                        let (_menhir_env : _menhir_env) = _menhir_env in
                        let (_menhir_stack : 'freshtv53 * _menhir_state * 'tv_memory_ident) = Obj.magic _menhir_stack in
                        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
                        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv54)) : 'freshtv56)) : 'freshtv58)) : 'freshtv60)) : 'freshtv62)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((('freshtv63 * _menhir_state) * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 386 "parser/SMTParserWp.ml"
                    )) * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 390 "parser/SMTParserWp.ml"
                    )) * (
# 24 "parser/SMTParserWp.mly"
       (string)
# 394 "parser/SMTParserWp.ml"
                    )) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv64)) : 'freshtv66)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv67 * _menhir_state) * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 405 "parser/SMTParserWp.ml"
                )) * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 409 "parser/SMTParserWp.ml"
                )) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv68)) : 'freshtv70)
        | NUM _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _v
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv71 * _menhir_state) * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 422 "parser/SMTParserWp.ml"
            )) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv72)) : 'freshtv74)
    | LPAREN ->
        _menhir_run5 _menhir_env (Obj.magic _menhir_stack) MenhirState5
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState5

and _menhir_run15 : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 23 "parser/SMTParserWp.mly"
       (string)
# 436 "parser/SMTParserWp.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | NUM _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv47 * _menhir_state * (
# 23 "parser/SMTParserWp.mly"
       (string)
# 452 "parser/SMTParserWp.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv48)

and _menhir_goto_args : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_args -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : ('freshtv45 * _menhir_state) * _menhir_state * 'tv_args) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | RPAREN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv41 * _menhir_state) * _menhir_state * 'tv_args) = Obj.magic _menhir_stack in
        ((let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState22
        | LPAREN ->
            _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState22
        | SATT ->
            _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState22
        | UNSATT ->
            _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState22
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState22) : 'freshtv42)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv43 * _menhir_state) * _menhir_state * 'tv_args) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv44)) : 'freshtv46)

and _menhir_goto_main : _menhir_env -> 'ttv_tail -> _menhir_state -> (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 494 "parser/SMTParserWp.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    match _menhir_s with
    | MenhirState24 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv31 * _menhir_state * 'tv_verdict) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 505 "parser/SMTParserWp.ml"
        )) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv29 * _menhir_state * 'tv_verdict) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((m : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 513 "parser/SMTParserWp.ml"
        )) : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 517 "parser/SMTParserWp.ml"
        )) = _v in
        ((let (_menhir_stack, _menhir_s, (v : 'tv_verdict)) = _menhir_stack in
        let _v : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 523 "parser/SMTParserWp.ml"
        ) = 
# 37 "parser/SMTParserWp.mly"
                     ( v, snd m )
# 527 "parser/SMTParserWp.ml"
         in
        _menhir_goto_main _menhir_env _menhir_stack _menhir_s _v) : 'freshtv30)) : 'freshtv32)
    | MenhirState22 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv35 * _menhir_state) * _menhir_state * 'tv_args)) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 537 "parser/SMTParserWp.ml"
        )) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv33 * _menhir_state) * _menhir_state * 'tv_args)) = Obj.magic _menhir_stack in
        let (_ : _menhir_state) = _menhir_s in
        let ((m : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 545 "parser/SMTParserWp.ml"
        )) : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 549 "parser/SMTParserWp.ml"
        )) = _v in
        ((let ((_menhir_stack, _menhir_s), _, (x : 'tv_args)) = _menhir_stack in
        let _3 = () in
        let _1 = () in
        let _v : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 557 "parser/SMTParserWp.ml"
        ) = let args = 
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 561 "parser/SMTParserWp.ml"
         in
        
# 39 "parser/SMTParserWp.mly"
  ( None, args @ (snd m))
# 566 "parser/SMTParserWp.ml"
         in
        _menhir_goto_main _menhir_env _menhir_stack _menhir_s _v) : 'freshtv34)) : 'freshtv36)
    | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv39) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 576 "parser/SMTParserWp.ml"
        )) = _v in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv37) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let ((_1 : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 584 "parser/SMTParserWp.ml"
        )) : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 588 "parser/SMTParserWp.ml"
        )) = _v in
        (Obj.magic _1 : 'freshtv38)) : 'freshtv40)
    | _ ->
        _menhir_fail ()

and _menhir_errorcase : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    match _menhir_s with
    | MenhirState24 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv17 * _menhir_state * 'tv_verdict) = Obj.magic _menhir_stack in
        (_menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState24 : 'freshtv18)
    | MenhirState22 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv19 * _menhir_state) * _menhir_state * 'tv_args)) = Obj.magic _menhir_stack in
        (_menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState22 : 'freshtv20)
    | MenhirState18 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv21 * _menhir_state * 'tv_value) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv22)
    | MenhirState5 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv23 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv24)
    | MenhirState4 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv25 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv26)
    | MenhirState0 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv27) = Obj.magic _menhir_stack in
        (_menhir_run1 _menhir_env (Obj.magic _menhir_stack) MenhirState0 : 'freshtv28)

and _menhir_run2 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv15) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_verdict = 
# 33 "parser/SMTParserWp.mly"
         ( Some Formula.UNSAT )
# 635 "parser/SMTParserWp.ml"
     in
    _menhir_goto_verdict _menhir_env _menhir_stack _menhir_s _v) : 'freshtv16)

and _menhir_run3 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv13) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : 'tv_verdict = 
# 32 "parser/SMTParserWp.mly"
         ( Some Formula.SAT   )
# 649 "parser/SMTParserWp.ml"
     in
    _menhir_goto_verdict _menhir_env _menhir_stack _menhir_s _v) : 'freshtv14)

and _menhir_run4 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | ERROR ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv11) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState4 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | ERROR_MSG ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv7 * _menhir_state) = Obj.magic _menhir_stack in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv5 * _menhir_state) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_args = 
# 47 "parser/SMTParserWp.mly"
                         ( [] )
# 679 "parser/SMTParserWp.ml"
             in
            _menhir_goto_args _menhir_env _menhir_stack _menhir_s _v) : 'freshtv6)) : 'freshtv8)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv9 * _menhir_state) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv10)) : 'freshtv12)
    | IDENT _v ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState4 _v
    | LPAREN ->
        _menhir_run5 _menhir_env (Obj.magic _menhir_stack) MenhirState4
    | RPAREN ->
        _menhir_reduce3 _menhir_env (Obj.magic _menhir_stack) MenhirState4
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState4

and _menhir_run23 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv3) = Obj.magic _menhir_stack in
    let (_menhir_s : _menhir_state) = _menhir_s in
    ((let _1 = () in
    let _v : (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 709 "parser/SMTParserWp.ml"
    ) = 
# 40 "parser/SMTParserWp.mly"
        ( (None, []) )
# 713 "parser/SMTParserWp.ml"
     in
    _menhir_goto_main _menhir_env _menhir_stack _menhir_s _v) : 'freshtv4)

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

and main : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 28 "parser/SMTParserWp.mly"
      (Formula.status option * (string * string) list list)
# 732 "parser/SMTParserWp.ml"
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
    | EOF ->
        _menhir_run23 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | LPAREN ->
        _menhir_run4 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | SATT ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | UNSATT ->
        _menhir_run2 _menhir_env (Obj.magic _menhir_stack) MenhirState0
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState0) : 'freshtv2))

# 269 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
  

# 767 "parser/SMTParserWp.ml"
