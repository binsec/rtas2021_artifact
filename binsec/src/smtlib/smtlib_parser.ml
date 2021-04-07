
module MenhirBasics = struct
  
  exception Error = Parsing.Parse_error
  
  type token = 
    | UNDERSCORE
    | SYMBOL of (
# 140 "smtlib/smtlib_parser.mly"
       (string)
# 12 "smtlib/smtlib_parser.ml"
  )
    | STRING of (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 17 "smtlib/smtlib_parser.ml"
  )
    | SETOPTION
    | SETLOGIC
    | SETINFO
    | RPAREN
    | RESETASSERTIONS
    | RESET
    | QUOTEDSYMBOL of (
# 141 "smtlib/smtlib_parser.mly"
       (string)
# 28 "smtlib/smtlib_parser.ml"
  )
    | PUSH
    | POP
    | PAR
    | NUMERAL of (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 36 "smtlib/smtlib_parser.ml"
  )
    | MODEL
    | METAINFO
    | LPAREN
    | LET
    | LAMBDA
    | KEYWORD of (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 46 "smtlib/smtlib_parser.ml"
  )
    | HEXADECIMAL of (
# 134 "smtlib/smtlib_parser.mly"
       (string)
# 51 "smtlib/smtlib_parser.ml"
  )
    | GETVALUE
    | GETUNSATCORE
    | GETUNSATASSUMPTIONS
    | GETPROOF
    | GETOPTION
    | GETMODEL
    | GETINFO
    | GETASSIGNMENT
    | GETASSERTIONS
    | FORALL
    | EXIT
    | EXISTS
    | EOF
    | ECHO
    | DEFINESORT
    | DEFINEFUNREC
    | DEFINEFUN
    | DECLARESORT
    | DECLAREFUN
    | DECLARECONST
    | DECIMAL of (
# 133 "smtlib/smtlib_parser.mly"
       (string)
# 76 "smtlib/smtlib_parser.ml"
  )
    | CHECKSAT
    | BV_NUMERAL of (
# 135 "smtlib/smtlib_parser.mly"
       (string)
# 82 "smtlib/smtlib_parser.ml"
  )
    | BOOL of (
# 138 "smtlib/smtlib_parser.mly"
       (bool)
# 87 "smtlib/smtlib_parser.ml"
  )
    | BINARY of (
# 136 "smtlib/smtlib_parser.mly"
       (string)
# 92 "smtlib/smtlib_parser.ml"
  )
    | BANG
    | ASSERT
    | AS
  
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
  | MenhirState257
  | MenhirState256
  | MenhirState255
  | MenhirState249
  | MenhirState243
  | MenhirState241
  | MenhirState239
  | MenhirState236
  | MenhirState234
  | MenhirState233
  | MenhirState231
  | MenhirState229
  | MenhirState226
  | MenhirState225
  | MenhirState223
  | MenhirState222
  | MenhirState220
  | MenhirState217
  | MenhirState214
  | MenhirState212
  | MenhirState210
  | MenhirState208
  | MenhirState202
  | MenhirState201
  | MenhirState198
  | MenhirState197
  | MenhirState192
  | MenhirState190
  | MenhirState189
  | MenhirState188
  | MenhirState185
  | MenhirState183
  | MenhirState180
  | MenhirState179
  | MenhirState177
  | MenhirState175
  | MenhirState171
  | MenhirState168
  | MenhirState165
  | MenhirState162
  | MenhirState157
  | MenhirState154
  | MenhirState150
  | MenhirState147
  | MenhirState144
  | MenhirState141
  | MenhirState135
  | MenhirState134
  | MenhirState132
  | MenhirState129
  | MenhirState128
  | MenhirState125
  | MenhirState123
  | MenhirState119
  | MenhirState117
  | MenhirState113
  | MenhirState110
  | MenhirState107
  | MenhirState106
  | MenhirState105
  | MenhirState101
  | MenhirState98
  | MenhirState92
  | MenhirState91
  | MenhirState90
  | MenhirState82
  | MenhirState81
  | MenhirState80
  | MenhirState79
  | MenhirState77
  | MenhirState76
  | MenhirState75
  | MenhirState74
  | MenhirState72
  | MenhirState67
  | MenhirState66
  | MenhirState65
  | MenhirState64
  | MenhirState61
  | MenhirState59
  | MenhirState57
  | MenhirState55
  | MenhirState53
  | MenhirState50
  | MenhirState48
  | MenhirState45
  | MenhirState42
  | MenhirState40
  | MenhirState38
  | MenhirState36
  | MenhirState33
  | MenhirState23
  | MenhirState15
  | MenhirState11
  | MenhirState10
  | MenhirState5
  | MenhirState4
  | MenhirState2

# 22 "smtlib/smtlib_parser.mly"
  
    open Smtlib
    open Location ;;

    (* Helper construction functions.
       File location is handled in production rules.
       *)
    let mk_sexpr sexpr_desc sexpr_loc = { sexpr_desc; sexpr_loc; } ;;
    let mk_identifier id_desc id_loc = { id_desc; id_loc; } ;;

    let mk_sort sort_desc sort_loc = { sort_desc; sort_loc; } ;;

    let mk_command command_desc command_loc =
      { command_desc; command_loc; }
    ;;

    let mk_fun_def fun_def_desc fun_def_loc =
      { fun_def_desc; fun_def_loc; }
    ;;

    let mk_fun_rec_def fun_rec_def_desc fun_rec_def_loc =
      { fun_rec_def_desc; fun_rec_def_loc; }
    ;;

    let mk_sorted_var sorted_var_desc sorted_var_loc =
      { sorted_var_desc; sorted_var_loc; }
    ;;

    let mk_qual_identifier qual_identifier_desc qual_identifier_loc =
      { qual_identifier_desc; qual_identifier_loc; }
    ;;

    let mk_var_binding var_binding_desc var_binding_loc =
      { var_binding_desc; var_binding_loc; }
    ;;

    let mk_term term_desc term_loc = { term_desc; term_loc; } ;;

    let mk_smt_option smt_option_desc smt_option_loc = {
        smt_option_desc; smt_option_loc ;
      }
    ;;

    let mk_script script_commands script_loc =
      { script_commands; script_loc; }
    ;;

    let mk_model model_commands model_loc = { model_commands; model_loc; }
    let mk_attribute attribute_desc attribute_loc =
      { attribute_desc; attribute_loc; }
    ;;

    let mk_attr_value attr_value_desc attr_value_loc =
      { attr_value_desc; attr_value_loc; }
    ;;

    let mk_info_flag info_flag_desc info_flag_loc =
      { info_flag_desc; info_flag_loc; }
    ;;

    let mk_symbol symbol_desc symbol_loc = { symbol_desc; symbol_loc; } ;;

    let mk_loc _ _ = dummy_loc

# 277 "smtlib/smtlib_parser.ml"

let rec _menhir_goto_list_sort_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_sort_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState226 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1033 * Lexing.position * _menhir_state * 'tv_sort) * _menhir_state * 'tv_list_sort_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1031 * Lexing.position * _menhir_state * 'tv_sort) * _menhir_state * 'tv_list_sort_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _endpos_x_, _menhir_s, (x : 'tv_sort)), _, (xs : 'tv_list_sort_)) = _menhir_stack in
        let _v : 'tv_list_sort_ = 
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 292 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_sort_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1032)) : 'freshtv1034)
    | MenhirState225 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv1039 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv1035 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState229 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState229 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState229 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState229) : 'freshtv1036)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv1037 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1038)) : 'freshtv1040)
    | _ ->
        _menhir_fail ()

and _menhir_goto_list_sorted_var_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_sorted_var_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState198 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1023 * _menhir_state * 'tv_sorted_var) * _menhir_state * 'tv_list_sorted_var_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1021 * _menhir_state * 'tv_sorted_var) * _menhir_state * 'tv_list_sorted_var_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_sorted_var)), _, (xs : 'tv_list_sorted_var_)) = _menhir_stack in
        let _v : 'tv_list_sorted_var_ = 
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 342 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_sorted_var_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1022)) : 'freshtv1024)
    | MenhirState197 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1029 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sorted_var_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1025 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sorted_var_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState201 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState201 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState201 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState201) : 'freshtv1026)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1027 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sorted_var_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1028)) : 'freshtv1030)
    | _ ->
        _menhir_fail ()

and _menhir_reduce46 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_sort_ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 384 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_list_sort_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_reduce48 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_sorted_var_ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 393 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_list_sorted_var_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_goto_nonempty_list_sorted_var_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_sorted_var_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState110 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv1001 * _menhir_state * 'tv_sorted_var) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv999 * _menhir_state * 'tv_sorted_var) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_sorted_var)), _, (xs : 'tv_nonempty_list_sorted_var_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_sorted_var_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 410 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_nonempty_list_sorted_var_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv1000)) : 'freshtv1002)
    | MenhirState105 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1007 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1003 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BINARY _v ->
                _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState113 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | BOOL _v ->
                _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState113 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | DECIMAL _v ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState113 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | HEXADECIMAL _v ->
                _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState113 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | LPAREN ->
                _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState113 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | NUMERAL _v ->
                _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState113 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState113 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | STRING _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState113 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState113 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState113) : 'freshtv1004)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1005 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1006)) : 'freshtv1008)
    | MenhirState117 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1013 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1009 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BINARY _v ->
                _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState119 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | BOOL _v ->
                _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState119 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | DECIMAL _v ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState119 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | HEXADECIMAL _v ->
                _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState119 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | LPAREN ->
                _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState119 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | NUMERAL _v ->
                _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState119 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState119 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | STRING _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState119 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState119 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState119) : 'freshtv1010)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1011 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1012)) : 'freshtv1014)
    | MenhirState123 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv1019 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1015 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BINARY _v ->
                _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState125 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | BOOL _v ->
                _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState125 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | DECIMAL _v ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState125 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | HEXADECIMAL _v ->
                _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState125 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | LPAREN ->
                _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState125 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | NUMERAL _v ->
                _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState125 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState125 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | STRING _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState125 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState125 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState125) : 'freshtv1016)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv1017 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv1018)) : 'freshtv1020)
    | _ ->
        _menhir_fail ()

and _menhir_goto_nonempty_list_sort_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_sort_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState82 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv975 * Lexing.position * _menhir_state * 'tv_sort) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv973 * Lexing.position * _menhir_state * 'tv_sort) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _endpos_x_, _menhir_s, (x : 'tv_sort)), _, (xs : 'tv_nonempty_list_sort_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_sort_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 558 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_nonempty_list_sort_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv974)) : 'freshtv976)
    | MenhirState81 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv983 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv979 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv977 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
            let (_endpos__4_ : Lexing.position) = _endpos in
            ((let (((_menhir_stack, _menhir_s, _startpos__1_), _endpos_id_, _, (id : 'tv_identifier), _startpos_id_), _, (sorts : 'tv_nonempty_list_sort_)) = _menhir_stack in
            let _4 = () in
            let _1 = () in
            let _endpos = _endpos__4_ in
            let _v : 'tv_sort = let _endpos = _endpos__4_ in
            let _startpos = _startpos__1_ in
            
# 291 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in mk_sort (SortFun (id, sorts)) loc )
# 584 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_sort _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv978)) : 'freshtv980)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv981 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv982)) : 'freshtv984)
    | MenhirState192 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv997 * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv993 * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv991 * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
            let (_endpos__4_ : Lexing.position) = _endpos in
            ((let (((_menhir_stack, _menhir_s), _startpos__2_), _, (sorts : 'tv_nonempty_list_sort_)) = _menhir_stack in
            let _4 = () in
            let _2 = () in
            let _1 = () in
            let _v : 'tv_poly_parameters = 
# 278 "smtlib/smtlib_parser.mly"
                                 ( sorts )
# 615 "smtlib/smtlib_parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv989) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_poly_parameters) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv987) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_poly_parameters) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv985) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let ((x : 'tv_poly_parameters) : 'tv_poly_parameters) = _v in
            ((let _v : 'tv_option_poly_parameters_ = 
# 116 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 632 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_option_poly_parameters_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv986)) : 'freshtv988)) : 'freshtv990)) : 'freshtv992)) : 'freshtv994)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv995 * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sort_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv996)) : 'freshtv998)
    | _ ->
        _menhir_fail ()

and _menhir_goto_nonempty_list_index_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_index_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState67 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv967 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_nonempty_list_index_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv963 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_nonempty_list_index_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv961 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_nonempty_list_index_) = Obj.magic _menhir_stack in
            let (_endpos__5_ : Lexing.position) = _endpos in
            ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _), _endpos_symb_, _, (symb : 'tv_symbol), _startpos_symb_), _, (indexes : 'tv_nonempty_list_index_)) = _menhir_stack in
            let _5 = () in
            let _2 = () in
            let _1 = () in
            let _startpos = _startpos__1_ in
            let _endpos = _endpos__5_ in
            let _v : 'tv_identifier = let _endpos = _endpos__5_ in
            let _startpos = _startpos__1_ in
            
# 305 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_identifier (IdUnderscore (symb, indexes)) loc )
# 675 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_identifier _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv962)) : 'freshtv964)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv965 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_nonempty_list_index_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv966)) : 'freshtv968)
    | MenhirState72 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv971 * _menhir_state * 'tv_index) * _menhir_state * 'tv_nonempty_list_index_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv969 * _menhir_state * 'tv_index) * _menhir_state * 'tv_nonempty_list_index_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_index)), _, (xs : 'tv_nonempty_list_index_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_index_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 694 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_nonempty_list_index_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv970)) : 'freshtv972)
    | _ ->
        _menhir_fail ()

and _menhir_goto_nonempty_list_attribute_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_attribute_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState129 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv955 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_term) * _menhir_state * 'tv_nonempty_list_attribute_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv951 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_term) * _menhir_state * 'tv_nonempty_list_attribute_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv949 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_term) * _menhir_state * 'tv_nonempty_list_attribute_) = Obj.magic _menhir_stack in
            let (_endpos__5_ : Lexing.position) = _endpos in
            ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _), _endpos_t_, _, (t : 'tv_term)), _, (attrs : 'tv_nonempty_list_attribute_)) = _menhir_stack in
            let _5 = () in
            let _2 = () in
            let _1 = () in
            let _endpos = _endpos__5_ in
            let _v : 'tv_term = let _endpos = _endpos__5_ in
            let _startpos = _startpos__1_ in
            
# 336 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_term (TermAnnotatedTerm(t, attrs)) loc )
# 729 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_term _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv950)) : 'freshtv952)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv953 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_term) * _menhir_state * 'tv_nonempty_list_attribute_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv954)) : 'freshtv956)
    | MenhirState132 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv959 * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) * _menhir_state * 'tv_nonempty_list_attribute_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv957 * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) * _menhir_state * 'tv_nonempty_list_attribute_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _endpos_x_, _menhir_s, (x : 'tv_attribute), _startpos_x_), _, (xs : 'tv_nonempty_list_attribute_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_attribute_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 748 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_nonempty_list_attribute_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv958)) : 'freshtv960)
    | _ ->
        _menhir_fail ()

and _menhir_goto_list_sexpr_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_sexpr_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState23 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv931 * _menhir_state * 'tv_sexpr) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv929 * _menhir_state * 'tv_sexpr) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_sexpr)), _, (xs : 'tv_list_sexpr_)) = _menhir_stack in
        let _v : 'tv_list_sexpr_ = 
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 767 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_sexpr_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv930)) : 'freshtv932)
    | MenhirState15 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv939 * _menhir_state * Lexing.position) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv935 * _menhir_state * Lexing.position) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv933 * _menhir_state * Lexing.position) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
            let (_endpos__3_ : Lexing.position) = _endpos in
            ((let ((_menhir_stack, _menhir_s, _startpos__1_), _, (sexprs : 'tv_list_sexpr_)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _v : 'tv_sexpr = let _endpos = _endpos__3_ in
            let _startpos = _startpos__1_ in
            
# 393 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_sexpr (SexprParens sexprs) loc )
# 793 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_sexpr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv934)) : 'freshtv936)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv937 * _menhir_state * Lexing.position) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv938)) : 'freshtv940)
    | MenhirState10 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv947 * _menhir_state * Lexing.position) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv943 * _menhir_state * Lexing.position) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv941 * _menhir_state * Lexing.position) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
            let (_endpos__3_ : Lexing.position) = _endpos in
            ((let ((_menhir_stack, _menhir_s, _startpos__1_), _, (sexprs : 'tv_list_sexpr_)) = _menhir_stack in
            let _3 = () in
            let _1 = () in
            let _endpos = _endpos__3_ in
            let _v : 'tv_attribute_value = let _endpos = _endpos__3_ in
            let _startpos = _startpos__1_ in
            
# 378 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_attr_value (AttrValSexpr sexprs) loc )
# 827 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_attribute_value _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv942)) : 'freshtv944)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv945 * _menhir_state * Lexing.position) * _menhir_state * 'tv_list_sexpr_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv946)) : 'freshtv948)
    | _ ->
        _menhir_fail ()

and _menhir_goto_option_poly_parameters_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_option_poly_parameters_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState190 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv921 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv917 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run106 _menhir_env (Obj.magic _menhir_stack) MenhirState197 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | RPAREN ->
                _menhir_reduce48 _menhir_env (Obj.magic _menhir_stack) MenhirState197
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState197) : 'freshtv918)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv919 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv920)) : 'freshtv922)
    | MenhirState223 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv927 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv923 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState225 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState225 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState225 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | RPAREN ->
                _menhir_reduce46 _menhir_env (Obj.magic _menhir_stack) MenhirState225
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState225) : 'freshtv924)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv925 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv926)) : 'freshtv928)
    | _ ->
        _menhir_fail ()

and _menhir_goto_qual_identifier : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> 'tv_qual_identifier -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v, _startpos) in
    match _menhir_s with
    | MenhirState255 | MenhirState241 | MenhirState202 | MenhirState64 | MenhirState134 | MenhirState135 | MenhirState128 | MenhirState125 | MenhirState119 | MenhirState113 | MenhirState101 | MenhirState92 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv913 * Lexing.position * _menhir_state * 'tv_qual_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv911 * Lexing.position * _menhir_state * 'tv_qual_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos__1_, _menhir_s, (_1 : 'tv_qual_identifier), _startpos__1_) = _menhir_stack in
        let _endpos = _endpos__1_ in
        let _v : 'tv_term = let _endpos = _endpos__1_ in
        let _startpos = _startpos__1_ in
        
# 321 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_term (TermQualIdentifier _1) loc )
# 926 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_term _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv912)) : 'freshtv914)
    | MenhirState65 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv915 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_qual_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BINARY _v ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState134 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | BOOL _v ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState134 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | DECIMAL _v ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState134 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | HEXADECIMAL _v ->
            _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState134 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | LPAREN ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState134 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | NUMERAL _v ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState134 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState134 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | STRING _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState134 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState134 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState134) : 'freshtv916)
    | _ ->
        _menhir_fail ()

and _menhir_goto_sort : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> 'tv_sort -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState192 | MenhirState82 | MenhirState81 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv863 * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState82 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState82 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState82 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv861 * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _endpos_x_, _menhir_s, (x : 'tv_sort)) = _menhir_stack in
            let _v : 'tv_nonempty_list_sort_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 983 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_nonempty_list_sort_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv862)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState82) : 'freshtv864)
    | MenhirState79 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv871 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv867 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv865 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            let (_endpos__5_ : Lexing.position) = _endpos in
            ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _), _endpos_id_, _, (id : 'tv_identifier), _startpos_id_), _endpos_so_, _, (so : 'tv_sort)) = _menhir_stack in
            let _5 = () in
            let _2 = () in
            let _1 = () in
            let _startpos = _startpos__1_ in
            let _endpos = _endpos__5_ in
            let _v : 'tv_qual_identifier = let _endpos = _endpos__5_ in
            let _startpos = _startpos__1_ in
            
# 348 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_qual_identifier (QualIdentifierAs (id, so)) loc )
# 1016 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_qual_identifier _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv866)) : 'freshtv868)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv869 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv870)) : 'freshtv872)
    | MenhirState107 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv887 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv883 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv881 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            let (_endpos__4_ : Lexing.position) = _endpos in
            ((let (((_menhir_stack, _menhir_s, _startpos__1_), _endpos_symb_, _, (symb : 'tv_symbol), _startpos_symb_), _endpos_so_, _, (so : 'tv_sort)) = _menhir_stack in
            let _4 = () in
            let _1 = () in
            let _v : 'tv_sorted_var = let _endpos = _endpos__4_ in
            let _startpos = _startpos__1_ in
            
# 283 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_sorted_var (SortedVar (symb, so)) loc )
# 1049 "smtlib/smtlib_parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv879) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_sorted_var) = _v in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
            match _menhir_s with
            | MenhirState123 | MenhirState117 | MenhirState110 | MenhirState105 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv875 * _menhir_state * 'tv_sorted_var) = Obj.magic _menhir_stack in
                ((assert (not _menhir_env._menhir_error);
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run106 _menhir_env (Obj.magic _menhir_stack) MenhirState110 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | RPAREN ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv873 * _menhir_state * 'tv_sorted_var) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, (x : 'tv_sorted_var)) = _menhir_stack in
                    let _v : 'tv_nonempty_list_sorted_var_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 1072 "smtlib/smtlib_parser.ml"
                     in
                    _menhir_goto_nonempty_list_sorted_var_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv874)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState110) : 'freshtv876)
            | MenhirState198 | MenhirState197 ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv877 * _menhir_state * 'tv_sorted_var) = Obj.magic _menhir_stack in
                ((assert (not _menhir_env._menhir_error);
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run106 _menhir_env (Obj.magic _menhir_stack) MenhirState198 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | RPAREN ->
                    _menhir_reduce48 _menhir_env (Obj.magic _menhir_stack) MenhirState198
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState198) : 'freshtv878)
            | _ ->
                _menhir_fail ()) : 'freshtv880)) : 'freshtv882)) : 'freshtv884)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv885 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv886)) : 'freshtv888)
    | MenhirState183 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv893 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((((('freshtv889 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState185 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState185
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState185) : 'freshtv890)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((((('freshtv891 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv892)) : 'freshtv894)
    | MenhirState201 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv895 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BINARY _v ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState202 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | BOOL _v ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState202 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | DECIMAL _v ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState202 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | HEXADECIMAL _v ->
            _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState202 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | LPAREN ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState202 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | NUMERAL _v ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState202 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState202 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | STRING _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState202 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState202 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState202) : 'freshtv896)
    | MenhirState226 | MenhirState225 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv897 * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState226 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState226 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState226 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | RPAREN ->
            _menhir_reduce46 _menhir_env (Obj.magic _menhir_stack) MenhirState226
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState226) : 'freshtv898)
    | MenhirState229 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((((('freshtv903 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((((('freshtv899 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState231 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState231
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState231) : 'freshtv900)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((((('freshtv901 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv902)) : 'freshtv904)
    | MenhirState234 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv909 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv905 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState236 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState236
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState236) : 'freshtv906)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv907 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv908)) : 'freshtv910)
    | _ ->
        _menhir_fail ()

and _menhir_goto_index : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_index -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv859 * _menhir_state * 'tv_index) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | NUMERAL _v ->
        _menhir_run69 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState72 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run68 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState72 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | RPAREN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv857 * _menhir_state * 'tv_index) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, (x : 'tv_index)) = _menhir_stack in
        let _v : 'tv_nonempty_list_index_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 1257 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_nonempty_list_index_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv858)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState72) : 'freshtv860)

and _menhir_goto_nonempty_list_fun_rec_def_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_fun_rec_def_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState188 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv851 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv847 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv843 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let _menhir_stack = (_menhir_stack, _endpos) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState208 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | EOF | RPAREN ->
                    _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState208
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState208) : 'freshtv844)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv845 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) * Lexing.position) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv846)) : 'freshtv848)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv849 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv850)) : 'freshtv852)
    | MenhirState210 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv855 * _menhir_state * 'tv_fun_rec_def) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv853 * _menhir_state * 'tv_fun_rec_def) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_fun_rec_def)), _, (xs : 'tv_nonempty_list_fun_rec_def_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_fun_rec_def_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 1322 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_nonempty_list_fun_rec_def_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv854)) : 'freshtv856)
    | _ ->
        _menhir_fail ()

and _menhir_goto_nonempty_list_term_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_term_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState135 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv823 * Lexing.position * _menhir_state * 'tv_term) * _menhir_state * 'tv_nonempty_list_term_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv821 * Lexing.position * _menhir_state * 'tv_term) * _menhir_state * 'tv_nonempty_list_term_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _endpos_x_, _menhir_s, (x : 'tv_term)), _, (xs : 'tv_nonempty_list_term_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_term_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 1341 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_nonempty_list_term_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv822)) : 'freshtv824)
    | MenhirState134 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv831 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_qual_identifier * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv827 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_qual_identifier * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv825 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_qual_identifier * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) = Obj.magic _menhir_stack in
            let (_endpos__4_ : Lexing.position) = _endpos in
            ((let (((_menhir_stack, _menhir_s, _startpos__1_), _endpos_qualid_, _, (qualid : 'tv_qual_identifier), _startpos_qualid_), _, (ts : 'tv_nonempty_list_term_)) = _menhir_stack in
            let _4 = () in
            let _1 = () in
            let _endpos = _endpos__4_ in
            let _v : 'tv_term = let _endpos = _endpos__4_ in
            let _startpos = _startpos__1_ in
            
# 324 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_term (TermQualIdentifierTerms(qualid, ts)) loc )
# 1368 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_term _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv826)) : 'freshtv828)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv829 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_qual_identifier * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv830)) : 'freshtv832)
    | MenhirState64 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv841 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv837 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv833 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let _menhir_stack = (_menhir_stack, _endpos) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState141 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | EOF | RPAREN ->
                    _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState141
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState141) : 'freshtv834)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv835 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) * Lexing.position) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv836)) : 'freshtv838)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv839 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv840)) : 'freshtv842)
    | _ ->
        _menhir_fail ()

and _menhir_goto_nonempty_list_var_binding_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_var_binding_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState98 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv813 * _menhir_state * 'tv_var_binding) * _menhir_state * 'tv_nonempty_list_var_binding_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv811 * _menhir_state * 'tv_var_binding) * _menhir_state * 'tv_nonempty_list_var_binding_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, (x : 'tv_var_binding)), _, (xs : 'tv_nonempty_list_var_binding_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_var_binding_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 1438 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_nonempty_list_var_binding_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv812)) : 'freshtv814)
    | MenhirState90 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv819 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_var_binding_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv815 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_var_binding_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BINARY _v ->
                _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState101 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | BOOL _v ->
                _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState101 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | DECIMAL _v ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState101 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | HEXADECIMAL _v ->
                _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState101 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | LPAREN ->
                _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState101 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | NUMERAL _v ->
                _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState101 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState101 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | STRING _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState101 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState101 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState101) : 'freshtv816)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv817 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_var_binding_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv818)) : 'freshtv820)
    | _ ->
        _menhir_fail ()

and _menhir_goto_option_NUMERAL_ : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> 'tv_option_NUMERAL_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState50 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv803 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv799 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState53 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState53
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState53) : 'freshtv800)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv801 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv802)) : 'freshtv804)
    | MenhirState55 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv809 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv805 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState57 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState57
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState57) : 'freshtv806)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv807 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv808)) : 'freshtv810)
    | _ ->
        _menhir_fail ()

and _menhir_goto_attribute : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> 'tv_attribute -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v, _startpos) in
    match _menhir_s with
    | MenhirState4 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv781 * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv779 * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos_attr_, _menhir_s, (attr : 'tv_attribute), _startpos_attr_) = _menhir_stack in
        let _endpos = _endpos_attr_ in
        let _v : 'tv_smt_option = let _endpos = _endpos_attr_ in
        let _startpos = _startpos_attr_ in
        
# 408 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_smt_option (OptionAttribute attr) loc )
# 1569 "smtlib/smtlib_parser.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv777) = _menhir_stack in
        let (_endpos : Lexing.position) = _endpos in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_smt_option) = _v in
        ((let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v) in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv775 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_smt_option) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv771 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_smt_option) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState33 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState33
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState33) : 'freshtv772)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv773 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_smt_option) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv774)) : 'freshtv776)) : 'freshtv778)) : 'freshtv780)) : 'freshtv782)
    | MenhirState40 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv787 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv783 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState42 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState42
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState42) : 'freshtv784)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv785 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv786)) : 'freshtv788)
    | MenhirState59 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv793 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv789 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState61 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState61
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState61) : 'freshtv790)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv791 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv792)) : 'freshtv794)
    | MenhirState132 | MenhirState129 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv797 * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | KEYWORD _v ->
            _menhir_run5 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState132 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv795 * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _endpos_x_, _menhir_s, (x : 'tv_attribute), _startpos_x_) = _menhir_stack in
            let _v : 'tv_nonempty_list_attribute_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 1678 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_nonempty_list_attribute_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv796)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState132) : 'freshtv798)
    | _ ->
        _menhir_fail ()

and _menhir_reduce44 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_list_sexpr_ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 1693 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_list_sexpr_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run11 : _menhir_env -> 'ttv_tail * _menhir_state * Lexing.position -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BV_NUMERAL _v ->
        _menhir_run12 _menhir_env (Obj.magic _menhir_stack) MenhirState11 _v
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState11

and _menhir_run15 : _menhir_env -> 'ttv_tail -> _menhir_state -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _startpos ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BINARY _v ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState15 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | BOOL _v ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState15 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | DECIMAL _v ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState15 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | HEXADECIMAL _v ->
        _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState15 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | KEYWORD _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState15 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | LPAREN ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState15 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | NUMERAL _v ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState15 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState15 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | STRING _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState15 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState15 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | UNDERSCORE ->
        _menhir_run11 _menhir_env (Obj.magic _menhir_stack) MenhirState15
    | RPAREN ->
        _menhir_reduce44 _menhir_env (Obj.magic _menhir_stack) MenhirState15
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState15

and _menhir_run16 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 1748 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv769) = Obj.magic _menhir_stack in
    let (_endpos_kwd_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((kwd : (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 1759 "smtlib/smtlib_parser.ml"
    )) : (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 1763 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos_kwd_ : Lexing.position) = _startpos in
    ((let _v : 'tv_sexpr = let _endpos = _endpos_kwd_ in
    let _startpos = _startpos_kwd_ in
    
# 390 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_sexpr (SexprKeyword kwd) loc )
# 1772 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_sexpr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv770)

and _menhir_reduce69 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _v : 'tv_option_poly_parameters_ = 
# 114 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 1781 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_option_poly_parameters_ _menhir_env _menhir_stack _menhir_s _v

and _menhir_run191 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAREN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv765 * _menhir_state) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState192 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState192 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState192 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState192) : 'freshtv766)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv767 * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv768)

and _menhir_goto_nonempty_list_symbol_ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_nonempty_list_symbol_ -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState180 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv757 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv755 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _endpos_x_, _menhir_s, (x : 'tv_symbol), _startpos_x_), _, (xs : 'tv_nonempty_list_symbol_)) = _menhir_stack in
        let _v : 'tv_nonempty_list_symbol_ = 
# 223 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 1830 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_nonempty_list_symbol_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv756)) : 'freshtv758)
    | MenhirState179 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv763 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv759 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState183 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState183 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState183 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState183) : 'freshtv760)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (((('freshtv761 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv762)) : 'freshtv764)
    | _ ->
        _menhir_fail ()

and _menhir_run80 : _menhir_env -> 'ttv_tail -> _menhir_state -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _startpos ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAREN ->
        _menhir_run77 _menhir_env (Obj.magic _menhir_stack) MenhirState80 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState80 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState80 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | UNDERSCORE ->
        _menhir_run75 _menhir_env (Obj.magic _menhir_stack) MenhirState80
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState80

and _menhir_goto_identifier : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> 'tv_identifier -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v, _startpos) in
    match _menhir_s with
    | MenhirState76 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv743 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState79 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState79 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState79 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState79) : 'freshtv744)
    | MenhirState80 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv745 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState81 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState81 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState81 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState81) : 'freshtv746)
    | MenhirState234 | MenhirState225 | MenhirState229 | MenhirState226 | MenhirState201 | MenhirState192 | MenhirState183 | MenhirState107 | MenhirState79 | MenhirState81 | MenhirState82 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv749 * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv747 * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos_id_, _menhir_s, (id : 'tv_identifier), _startpos_id_) = _menhir_stack in
        let _endpos = _endpos_id_ in
        let _v : 'tv_sort = let _endpos = _endpos_id_ in
        let _startpos = _startpos_id_ in
        
# 289 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in mk_sort (SortIdentifier id) loc )
# 1934 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_sort _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv748)) : 'freshtv750)
    | MenhirState255 | MenhirState241 | MenhirState202 | MenhirState64 | MenhirState65 | MenhirState134 | MenhirState135 | MenhirState128 | MenhirState125 | MenhirState119 | MenhirState113 | MenhirState101 | MenhirState92 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv753 * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv751 * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos_id_, _menhir_s, (id : 'tv_identifier), _startpos_id_) = _menhir_stack in
        let _startpos = _startpos_id_ in
        let _endpos = _endpos_id_ in
        let _v : 'tv_qual_identifier = let _endpos = _endpos_id_ in
        let _startpos = _startpos_id_ in
        
# 345 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_qual_identifier (QualIdentifierIdentifier id) loc )
# 1951 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_qual_identifier _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv752)) : 'freshtv754)
    | _ ->
        _menhir_fail ()

and _menhir_run68 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 140 "smtlib/smtlib_parser.mly"
       (string)
# 1960 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv741) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 140 "smtlib/smtlib_parser.mly"
       (string)
# 1971 "smtlib/smtlib_parser.ml"
    )) : (
# 140 "smtlib/smtlib_parser.mly"
       (string)
# 1975 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _v : 'tv_index = let _endpos = _endpos__1_ in
    let _startpos = _startpos__1_ in
    
# 297 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    IdxSymbol (mk_symbol (SimpleSymbol _1) loc) )
# 1984 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_index _menhir_env _menhir_stack _menhir_s _v) : 'freshtv742)

and _menhir_run69 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 1991 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv739) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 2002 "smtlib/smtlib_parser.ml"
    )) : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 2006 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _v : 'tv_index = 
# 295 "smtlib/smtlib_parser.mly"
          ( IdxNum _1 )
# 2012 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_index _menhir_env _menhir_stack _menhir_s _v) : 'freshtv740)

and _menhir_run77 : _menhir_env -> 'ttv_tail -> _menhir_state -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _startpos ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | UNDERSCORE ->
        _menhir_run75 _menhir_env (Obj.magic _menhir_stack) MenhirState77
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState77

and _menhir_fail : unit -> 'a =
  fun () ->
    Printf.fprintf stderr "Internal failure -- please contact the parser generator's developers.\n%!";
    assert false

and _menhir_goto_term : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> 'tv_term -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v) in
    match _menhir_s with
    | MenhirState92 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv657 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv653 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv651 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos__4_ : Lexing.position) = _endpos in
            ((let (((_menhir_stack, _menhir_s, _startpos__1_), _endpos_symb_, _, (symb : 'tv_symbol), _startpos_symb_), _endpos_t_, _, (t : 'tv_term)) = _menhir_stack in
            let _4 = () in
            let _1 = () in
            let _v : 'tv_var_binding = let _endpos = _endpos__4_ in
            let _startpos = _startpos__1_ in
            
# 354 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_var_binding (VarBinding (symb, t)) loc )
# 2061 "smtlib/smtlib_parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv649) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_var_binding) = _v in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv647 * _menhir_state * 'tv_var_binding) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run91 _menhir_env (Obj.magic _menhir_stack) MenhirState98 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv645 * _menhir_state * 'tv_var_binding) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _menhir_s, (x : 'tv_var_binding)) = _menhir_stack in
                let _v : 'tv_nonempty_list_var_binding_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 2082 "smtlib/smtlib_parser.ml"
                 in
                _menhir_goto_nonempty_list_var_binding_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv646)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState98) : 'freshtv648)) : 'freshtv650)) : 'freshtv652)) : 'freshtv654)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv655 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv656)) : 'freshtv658)
    | MenhirState101 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv665 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_var_binding_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv661 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_var_binding_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv659 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_var_binding_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos__7_ : Lexing.position) = _endpos in
            ((let ((((((_menhir_stack, _menhir_s, _startpos__1_), _), _startpos__3_), _, (vbindings : 'tv_nonempty_list_var_binding_)), _endpos__5_), _endpos_t_, _, (t : 'tv_term)) = _menhir_stack in
            let _7 = () in
            let _5 = () in
            let _3 = () in
            let _2 = () in
            let _1 = () in
            let _endpos = _endpos__7_ in
            let _v : 'tv_term = let _endpos = _endpos__7_ in
            let _startpos = _startpos__1_ in
            
# 327 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_term (TermLetTerm (vbindings, t)) loc )
# 2123 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_term _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv660)) : 'freshtv662)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv663 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_var_binding_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv664)) : 'freshtv666)
    | MenhirState113 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv673 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv669 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv667 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos__7_ : Lexing.position) = _endpos in
            ((let ((((((_menhir_stack, _menhir_s, _startpos__1_), _), _startpos__3_), _, (svars : 'tv_nonempty_list_sorted_var_)), _endpos__5_), _endpos_t_, _, (t : 'tv_term)) = _menhir_stack in
            let _7 = () in
            let _5 = () in
            let _3 = () in
            let _2 = () in
            let _1 = () in
            let _endpos = _endpos__7_ in
            let _v : 'tv_term = let _endpos = _endpos__7_ in
            let _startpos = _startpos__1_ in
            
# 339 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_term (TermLambdaTerm(svars, t)) loc )
# 2160 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_term _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv668)) : 'freshtv670)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv671 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv672)) : 'freshtv674)
    | MenhirState119 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv681 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv677 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv675 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos__7_ : Lexing.position) = _endpos in
            ((let ((((((_menhir_stack, _menhir_s, _startpos__1_), _), _startpos__3_), _, (svars : 'tv_nonempty_list_sorted_var_)), _endpos__5_), _endpos_t_, _, (t : 'tv_term)) = _menhir_stack in
            let _7 = () in
            let _5 = () in
            let _3 = () in
            let _2 = () in
            let _1 = () in
            let _endpos = _endpos__7_ in
            let _v : 'tv_term = let _endpos = _endpos__7_ in
            let _startpos = _startpos__1_ in
            
# 330 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_term (TermForallTerm (svars, t)) loc)
# 2197 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_term _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv676)) : 'freshtv678)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv679 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv680)) : 'freshtv682)
    | MenhirState125 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv689 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv685 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv683 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos__7_ : Lexing.position) = _endpos in
            ((let ((((((_menhir_stack, _menhir_s, _startpos__1_), _), _startpos__3_), _, (svars : 'tv_nonempty_list_sorted_var_)), _endpos__5_), _endpos_t_, _, (t : 'tv_term)) = _menhir_stack in
            let _7 = () in
            let _5 = () in
            let _3 = () in
            let _2 = () in
            let _1 = () in
            let _endpos = _endpos__7_ in
            let _v : 'tv_term = let _endpos = _endpos__7_ in
            let _startpos = _startpos__1_ in
            
# 333 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_term (TermExistsTerm (svars, t)) loc )
# 2234 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_term _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv684)) : 'freshtv686)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((((('freshtv687 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv688)) : 'freshtv690)
    | MenhirState128 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv691 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | KEYWORD _v ->
            _menhir_run5 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState129 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState129) : 'freshtv692)
    | MenhirState64 | MenhirState135 | MenhirState134 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv695 * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BINARY _v ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState135 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | BOOL _v ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState135 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | DECIMAL _v ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState135 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | HEXADECIMAL _v ->
            _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState135 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | LPAREN ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState135 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | NUMERAL _v ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState135 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState135 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | STRING _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState135 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState135 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv693 * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _endpos_x_, _menhir_s, (x : 'tv_term)) = _menhir_stack in
            let _v : 'tv_nonempty_list_term_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 2287 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_nonempty_list_term_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv694)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState135) : 'freshtv696)
    | MenhirState202 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv727 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv725 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((let (((((((_menhir_stack, _endpos_symb_, _menhir_s, (symb : 'tv_symbol), _startpos_symb_), _, (polys : 'tv_option_poly_parameters_)), _startpos__3_), _, (svars : 'tv_list_sorted_var_)), _endpos__5_), _endpos_so_, _, (so : 'tv_sort)), _endpos_t_, _, (t : 'tv_term)) = _menhir_stack in
        let _5 = () in
        let _3 = () in
        let _startpos = _startpos_symb_ in
        let _endpos = _endpos_t_ in
        let _v : 'tv_fun_def = 
# 260 "smtlib/smtlib_parser.mly"
  ( (symb, polys, svars, so, t) )
# 2307 "smtlib/smtlib_parser.ml"
         in
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv723) = _menhir_stack in
        let (_endpos : Lexing.position) = _endpos in
        let (_menhir_s : _menhir_state) = _menhir_s in
        let (_v : 'tv_fun_def) = _v in
        let (_startpos : Lexing.position) = _startpos in
        ((let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v, _startpos) in
        match _menhir_s with
        | MenhirState189 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv709 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_def * Lexing.position) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv705 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_def * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let _menhir_env = _menhir_discard _menhir_env in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv703 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_def * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos__3_ : Lexing.position) = _endpos in
                ((let ((_menhir_stack, _menhir_s, _startpos__1_), _endpos_fd_, _, (fd : 'tv_fun_def), _startpos_fd_) = _menhir_stack in
                let _3 = () in
                let _1 = () in
                let _v : 'tv_fun_rec_def = let _endpos = _endpos__3_ in
                let _startpos = _startpos__1_ in
                
# 272 "smtlib/smtlib_parser.mly"
 ( let s, ps, svs, so, t = fd in
   let loc = mk_loc _startpos _endpos in
   mk_fun_rec_def (FunRecDef (s, ps, svs, so, t)) loc )
# 2341 "smtlib/smtlib_parser.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv701) = _menhir_stack in
                let (_menhir_s : _menhir_state) = _menhir_s in
                let (_v : 'tv_fun_rec_def) = _v in
                ((let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv699 * _menhir_state * 'tv_fun_rec_def) = Obj.magic _menhir_stack in
                ((assert (not _menhir_env._menhir_error);
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run189 _menhir_env (Obj.magic _menhir_stack) MenhirState210 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | RPAREN ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv697 * _menhir_state * 'tv_fun_rec_def) = Obj.magic _menhir_stack in
                    ((let (_menhir_stack, _menhir_s, (x : 'tv_fun_rec_def)) = _menhir_stack in
                    let _v : 'tv_nonempty_list_fun_rec_def_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 2362 "smtlib/smtlib_parser.ml"
                     in
                    _menhir_goto_nonempty_list_fun_rec_def_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv698)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState210) : 'freshtv700)) : 'freshtv702)) : 'freshtv704)) : 'freshtv706)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ('freshtv707 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_def * Lexing.position) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv708)) : 'freshtv710)
        | MenhirState212 ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv721 * Lexing.position * _menhir_state * 'tv_fun_def * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv719 * Lexing.position * _menhir_state * 'tv_fun_def * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _endpos_fd_, _menhir_s, (fd : 'tv_fun_def), _startpos_fd_) = _menhir_stack in
            let _endpos = _endpos_fd_ in
            let _v : 'tv_fun_nonrec_def = let _endpos = _endpos_fd_ in
            let _startpos = _startpos_fd_ in
            
# 265 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    let s, ps, svs, so, t = fd in
    mk_fun_def (FunDef (s, ps, svs, so, t)) loc )
# 2390 "smtlib/smtlib_parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv717) = _menhir_stack in
            let (_endpos : Lexing.position) = _endpos in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : 'tv_fun_nonrec_def) = _v in
            ((let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v) in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv715 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_nonrec_def) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv711 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_nonrec_def) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let _menhir_stack = (_menhir_stack, _endpos) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState214 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | EOF | RPAREN ->
                    _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState214
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState214) : 'freshtv712)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv713 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_nonrec_def) = Obj.magic _menhir_stack in
                ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv714)) : 'freshtv716)) : 'freshtv718)) : 'freshtv720)) : 'freshtv722)
        | _ ->
            _menhir_fail ()) : 'freshtv724)) : 'freshtv726)) : 'freshtv728)
    | MenhirState241 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv733 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv729 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState243 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState243
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState243) : 'freshtv730)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv731 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv732)) : 'freshtv734)
    | MenhirState255 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv737 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BINARY _v ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState256 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | BOOL _v ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState256 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | DECIMAL _v ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState256 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | HEXADECIMAL _v ->
            _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState256 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv735) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = MenhirState256 in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | UNDERSCORE ->
                _menhir_run11 _menhir_env (Obj.magic _menhir_stack) MenhirState257
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState257) : 'freshtv736)
        | NUMERAL _v ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState256 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | STRING _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState256 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState256) : 'freshtv738)
    | _ ->
        _menhir_fail ()

and _menhir_goto_attribute_value : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> 'tv_attribute_value -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v ->
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv643 * Lexing.position * _menhir_state * (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 2503 "smtlib/smtlib_parser.ml"
    ) * Lexing.position) = Obj.magic _menhir_stack in
    let (_endpos : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let (_v : 'tv_attribute_value) = _v in
    ((let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv641 * Lexing.position * _menhir_state * (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 2512 "smtlib/smtlib_parser.ml"
    ) * Lexing.position) = Obj.magic _menhir_stack in
    let (_endpos_attrval_ : Lexing.position) = _endpos in
    let (_ : _menhir_state) = _menhir_s in
    let ((attrval : 'tv_attribute_value) : 'tv_attribute_value) = _v in
    ((let (_menhir_stack, _endpos_kwd_, _menhir_s, (kwd : (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 2520 "smtlib/smtlib_parser.ml"
    )), _startpos_kwd_) = _menhir_stack in
    let _startpos = _startpos_kwd_ in
    let _endpos = _endpos_attrval_ in
    let _v : 'tv_attribute = let _endpos = _endpos_attrval_ in
    let _startpos = _startpos_kwd_ in
    
# 402 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_attribute (AttrKeywordValue (kwd, attrval)) loc )
# 2530 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_attribute _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv642)) : 'freshtv644)

and _menhir_goto_sexpr : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_sexpr -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv639 * _menhir_state * 'tv_sexpr) = Obj.magic _menhir_stack in
    ((assert (not _menhir_env._menhir_error);
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BINARY _v ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState23 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | BOOL _v ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState23 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | DECIMAL _v ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState23 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | HEXADECIMAL _v ->
        _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState23 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | KEYWORD _v ->
        _menhir_run16 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState23 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | LPAREN ->
        _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState23 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | NUMERAL _v ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState23 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState23 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | STRING _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState23 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState23 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | RPAREN ->
        _menhir_reduce44 _menhir_env (Obj.magic _menhir_stack) MenhirState23
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState23) : 'freshtv640)

and _menhir_goto_list_delimited_LPAREN_command_RPAREN__ : _menhir_env -> 'ttv_tail -> _menhir_state -> 'tv_list_delimited_LPAREN_command_RPAREN__ -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v _startpos ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v, _startpos) in
    match _menhir_s with
    | MenhirState33 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv501 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_smt_option) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv499 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_smt_option) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_sopt_, _, (sopt : 'tv_smt_option)), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_sopt_ in
            let _startpos = _startpos__1_ in
            
# 253 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdSetOption sopt) loc )
# 2592 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2598 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2604 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv500)) : 'freshtv502)
    | MenhirState38 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv505 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv503 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_symb_, _, (symb : 'tv_symbol), _startpos_symb_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_symb_ in
            let _startpos = _startpos__1_ in
            
# 250 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdSetLogic symb) loc )
# 2626 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2632 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2638 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv504)) : 'freshtv506)
    | MenhirState42 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv509 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv507 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_attr_, _, (attr : 'tv_attribute), _startpos_attr_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_attr_ in
            let _startpos = _startpos__1_ in
            
# 247 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdSetInfo attr) loc )
# 2660 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2666 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2672 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv508)) : 'freshtv510)
    | MenhirState45 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv513 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv511 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 223 "smtlib/smtlib_parser.mly"
    ( let loc = mk_loc _startpos _endpos in
      mk_command CmdResetAssertions loc )
# 2694 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2700 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2706 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv512)) : 'freshtv514)
    | MenhirState48 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv517 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv515 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 220 "smtlib/smtlib_parser.mly"
    ( let loc = mk_loc _startpos _endpos in
      mk_command CmdReset loc )
# 2728 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2734 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2740 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv516)) : 'freshtv518)
    | MenhirState53 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv521 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv519 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos_num_, _, (num : 'tv_option_NUMERAL_)), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_num_ in
            let _startpos = _startpos__1_ in
            
# 244 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdPush num) loc )
# 2762 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2768 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2774 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv520)) : 'freshtv522)
    | MenhirState57 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv525 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv523 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos_num_, _, (num : 'tv_option_NUMERAL_)), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_num_ in
            let _startpos = _startpos__1_ in
            
# 241 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdPop num) loc )
# 2796 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2802 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2808 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv524)) : 'freshtv526)
    | MenhirState61 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv529 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv527 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_attr_, _, (attr : 'tv_attribute), _startpos_attr_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_attr_ in
            let _startpos = _startpos__1_ in
            
# 238 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdMetaInfo attr) loc )
# 2830 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2836 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2842 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv528)) : 'freshtv530)
    | MenhirState141 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv533 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv531 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _startpos__2_), _, (ts : 'tv_nonempty_list_term_)), _endpos__4_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _4 = () in
        let _2 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__4_ in
            let _startpos = _startpos__1_ in
            
# 235 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdGetValue ts) loc )
# 2866 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2872 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2878 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv532)) : 'freshtv534)
    | MenhirState144 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv537 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv535 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 229 "smtlib/smtlib_parser.mly"
    ( let loc = mk_loc _startpos _endpos in
      mk_command CmdGetUnsatCore loc )
# 2900 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2906 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2912 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv536)) : 'freshtv538)
    | MenhirState147 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv541 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv539 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 232 "smtlib/smtlib_parser.mly"
    ( let loc = mk_loc _startpos _endpos in
      mk_command CmdGetUnsatAssumptions loc )
# 2934 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2940 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2946 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv540)) : 'freshtv542)
    | MenhirState150 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv545 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv543 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 226 "smtlib/smtlib_parser.mly"
    ( let loc = mk_loc _startpos _endpos in
      mk_command CmdGetProof loc )
# 2968 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 2974 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 2980 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv544)) : 'freshtv546)
    | MenhirState154 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv549 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 2988 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv547 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 2994 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_kwd_, (kwd : (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 2999 "smtlib/smtlib_parser.ml"
        )), _startpos_kwd_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_kwd_ in
            let _startpos = _startpos__1_ in
            
# 217 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdGetOption kwd) loc )
# 3014 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3020 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3026 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv548)) : 'freshtv550)
    | MenhirState157 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv553 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv551 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 214 "smtlib/smtlib_parser.mly"
    ( let loc = mk_loc _startpos _endpos in
      mk_command CmdGetModel loc )
# 3048 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3054 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3060 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv552)) : 'freshtv554)
    | MenhirState162 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv557 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * 'tv_info_flag) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv555 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * 'tv_info_flag) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_iflag_, (iflag : 'tv_info_flag)), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_iflag_ in
            let _startpos = _startpos__1_ in
            
# 211 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdGetInfo iflag) loc )
# 3082 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3088 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3094 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv556)) : 'freshtv558)
    | MenhirState165 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv561 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv559 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 208 "smtlib/smtlib_parser.mly"
    ( let loc = mk_loc _startpos _endpos in
      mk_command CmdGetAssignment loc )
# 3116 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3122 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3128 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv560)) : 'freshtv562)
    | MenhirState168 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv565 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv563 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 205 "smtlib/smtlib_parser.mly"
    ( let loc = mk_loc _startpos _endpos in
      mk_command CmdGetAssertions loc )
# 3150 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3156 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3162 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv564)) : 'freshtv566)
    | MenhirState171 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv569 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv567 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 202 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command CmdExit loc )
# 3184 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3190 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3196 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv568)) : 'freshtv570)
    | MenhirState175 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv573 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 3204 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv571 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 3210 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_s_, (s : (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 3215 "smtlib/smtlib_parser.ml"
        )), _startpos_s_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_s_ in
            let _startpos = _startpos__1_ in
            
# 199 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdEcho s) loc )
# 3230 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3236 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3242 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv572)) : 'freshtv574)
    | MenhirState185 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((((('freshtv577 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((((('freshtv575 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_symb_, _, (symb : 'tv_symbol), _startpos_symb_), _startpos__3_inlined1_), _, (symbs : 'tv_nonempty_list_symbol_)), _endpos__5_), _endpos_so_, _, (so : 'tv_sort)), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _5 = () in
        let _3_inlined1 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _3, _1) = (_startpos__1_inlined1_, _3_inlined1, _1_inlined1) in
            let _endpos = _endpos_so_ in
            let _startpos = _startpos__1_ in
            
# 196 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdDefineSort (symb, symbs, so)) loc )
# 3266 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3272 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3278 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv576)) : 'freshtv578)
    | MenhirState208 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv581 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv579 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _startpos__2_), _, (frdefs : 'tv_nonempty_list_fun_rec_def_)), _endpos__4_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _4 = () in
        let _2 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__4_ in
            let _startpos = _startpos__1_ in
            
# 193 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_command (CmdDefineFunRec frdefs) loc )
# 3302 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3308 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3314 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv580)) : 'freshtv582)
    | MenhirState214 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv585 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_nonrec_def) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv583 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_nonrec_def) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_fdef_, _, (fdef : 'tv_fun_nonrec_def)), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_fdef_ in
            let _startpos = _startpos__1_ in
            
# 190 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_command (CmdDefineFun fdef) loc )
# 3336 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3342 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3348 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv584)) : 'freshtv586)
    | MenhirState220 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv589 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 3356 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv587 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 3362 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_symb_, _, (symb : 'tv_symbol), _startpos_symb_), _endpos_num_, (num : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 3367 "smtlib/smtlib_parser.ml"
        )), _startpos_num_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_num_ in
            let _startpos = _startpos__1_ in
            
# 187 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdDeclareSort(symb, num)) loc )
# 3382 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3388 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3394 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv588)) : 'freshtv590)
    | MenhirState231 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((((((('freshtv593 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((((((('freshtv591 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((((((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_symb_, _, (symb : 'tv_symbol), _startpos_symb_), _, (polys : 'tv_option_poly_parameters_)), _startpos__4_), _, (sorts : 'tv_list_sort_)), _endpos__6_), _endpos_so_, _, (so : 'tv_sort)), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _6 = () in
        let _4 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_so_ in
            let _startpos = _startpos__1_ in
            
# 184 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdDeclareFun (symb, polys, sorts, so)) loc )
# 3418 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3424 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3430 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv592)) : 'freshtv594)
    | MenhirState236 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv597 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv595 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_symb_, _, (symb : 'tv_symbol), _startpos_symb_), _endpos_so_, _, (so : 'tv_sort)), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_so_ in
            let _startpos = _startpos__1_ in
            
# 180 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_command (CmdDeclareConst(symb, so)) loc )
# 3452 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3458 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3464 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv596)) : 'freshtv598)
    | MenhirState239 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv601 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv599 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _endpos__1_inlined1_, _startpos__1_inlined1_), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_endpos__1_, _startpos__1_, _1) = (_endpos__1_inlined1_, _startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos__1_ in
            let _startpos = _startpos__1_ in
            
# 178 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in mk_command CmdCheckSat loc )
# 3485 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3491 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3497 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv600)) : 'freshtv602)
    | MenhirState243 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv605 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv603 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((((_menhir_stack, _menhir_s, _startpos__1_), _startpos__1_inlined1_), _endpos_t_, _, (t : 'tv_term)), _endpos__3_), _, (xs : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_xs_) = _menhir_stack in
        let _3 = () in
        let _1_inlined1 = () in
        let _1 = () in
        let _startpos = _startpos__1_ in
        let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = let x =
          let x =
            let (_startpos__1_, _1) = (_startpos__1_inlined1_, _1_inlined1) in
            let _endpos = _endpos_t_ in
            let _startpos = _startpos__1_ in
            
# 176 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in mk_command (CmdAssert t) loc )
# 3518 "smtlib/smtlib_parser.ml"
            
          in
          
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3524 "smtlib/smtlib_parser.ml"
          
        in
        
# 213 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x :: xs )
# 3530 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos) : 'freshtv604)) : 'freshtv606)
    | MenhirState2 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv623 * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv619 * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | EOF ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv615 * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv613 * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos__2_ : Lexing.position) = _endpos in
                ((let ((((_menhir_stack, _startpos__1_), _endpos__1_inlined1_), _, (x : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_x_), _endpos__3_) = _menhir_stack in
                let _2 = () in
                let _3 = () in
                let _1_inlined1 = () in
                let _1 = () in
                let _v : (
# 145 "smtlib/smtlib_parser.mly"
       (Smtlib.model)
# 3562 "smtlib/smtlib_parser.ml"
                ) = let commands =
                  let x =
                    let _1 = _1_inlined1 in
                    
# 183 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3569 "smtlib/smtlib_parser.ml"
                    
                  in
                  
# 200 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( x )
# 3575 "smtlib/smtlib_parser.ml"
                  
                in
                let _startpos_commands_ = _startpos__1_ in
                let _endpos = _endpos__2_ in
                let _startpos = _startpos_commands_ in
                
# 164 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_model commands loc
   )
# 3586 "smtlib/smtlib_parser.ml"
                 in
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv611) = _menhir_stack in
                let (_v : (
# 145 "smtlib/smtlib_parser.mly"
       (Smtlib.model)
# 3593 "smtlib/smtlib_parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv609) = Obj.magic _menhir_stack in
                let (_v : (
# 145 "smtlib/smtlib_parser.mly"
       (Smtlib.model)
# 3600 "smtlib/smtlib_parser.ml"
                )) = _v in
                ((let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : 'freshtv607) = Obj.magic _menhir_stack in
                let ((_1 : (
# 145 "smtlib/smtlib_parser.mly"
       (Smtlib.model)
# 3607 "smtlib/smtlib_parser.ml"
                )) : (
# 145 "smtlib/smtlib_parser.mly"
       (Smtlib.model)
# 3611 "smtlib/smtlib_parser.ml"
                )) = _v in
                (Obj.magic _1 : 'freshtv608)) : 'freshtv610)) : 'freshtv612)) : 'freshtv614)) : 'freshtv616)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv617 * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _menhir_s, _, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv618)) : 'freshtv620)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv621 * Lexing.position) * Lexing.position) * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv622)) : 'freshtv624)
    | MenhirState249 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv637 * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | EOF ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv633 * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv631 * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos__2_ : Lexing.position) = _endpos in
            ((let (_menhir_stack, _menhir_s, (commands : 'tv_list_delimited_LPAREN_command_RPAREN__), _startpos_commands_) = _menhir_stack in
            let _2 = () in
            let _v : (
# 143 "smtlib/smtlib_parser.mly"
       (Smtlib.script)
# 3646 "smtlib/smtlib_parser.ml"
            ) = let _endpos = _endpos__2_ in
            let _startpos = _startpos_commands_ in
            
# 156 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_script commands loc )
# 3653 "smtlib/smtlib_parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv629) = _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 143 "smtlib/smtlib_parser.mly"
       (Smtlib.script)
# 3661 "smtlib/smtlib_parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv627) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let (_v : (
# 143 "smtlib/smtlib_parser.mly"
       (Smtlib.script)
# 3669 "smtlib/smtlib_parser.ml"
            )) = _v in
            ((let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv625) = Obj.magic _menhir_stack in
            let (_menhir_s : _menhir_state) = _menhir_s in
            let ((_1 : (
# 143 "smtlib/smtlib_parser.mly"
       (Smtlib.script)
# 3677 "smtlib/smtlib_parser.ml"
            )) : (
# 143 "smtlib/smtlib_parser.mly"
       (Smtlib.script)
# 3681 "smtlib/smtlib_parser.ml"
            )) = _v in
            (Obj.magic _1 : 'freshtv626)) : 'freshtv628)) : 'freshtv630)) : 'freshtv632)) : 'freshtv634)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv635 * _menhir_state * 'tv_list_delimited_LPAREN_command_RPAREN__ * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s, _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv636)) : 'freshtv638)
    | _ ->
        _menhir_fail ()

and _menhir_reduce67 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let (_, _endpos) = Obj.magic _menhir_stack in
    let _v : 'tv_option_NUMERAL_ = 
# 114 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( None )
# 3700 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_option_NUMERAL_ _menhir_env _menhir_stack _endpos _menhir_s _v

and _menhir_run51 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 3707 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv497) = Obj.magic _menhir_stack in
    let (_endpos_x_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((x : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 3718 "smtlib/smtlib_parser.ml"
    )) : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 3722 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos_x_ : Lexing.position) = _startpos in
    ((let _endpos = _endpos_x_ in
    let _v : 'tv_option_NUMERAL_ = 
# 116 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( Some x )
# 3729 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_option_NUMERAL_ _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv498)

and _menhir_run5 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 3736 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v, _startpos) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | BINARY _v ->
        _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState5 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | BOOL _v ->
        _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState5 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | DECIMAL _v ->
        _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState5 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | HEXADECIMAL _v ->
        _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState5 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | LPAREN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv493) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState5 in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BINARY _v ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState10 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | BOOL _v ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState10 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | DECIMAL _v ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState10 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | HEXADECIMAL _v ->
            _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState10 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | KEYWORD _v ->
            _menhir_run16 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState10 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | LPAREN ->
            _menhir_run15 _menhir_env (Obj.magic _menhir_stack) MenhirState10 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | NUMERAL _v ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState10 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState10 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | STRING _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState10 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState10 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | UNDERSCORE ->
            _menhir_run11 _menhir_env (Obj.magic _menhir_stack) MenhirState10
        | RPAREN ->
            _menhir_reduce44 _menhir_env (Obj.magic _menhir_stack) MenhirState10
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState10) : 'freshtv494)
    | NUMERAL _v ->
        _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState5 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState5 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | STRING _v ->
        _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState5 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState5 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | KEYWORD _ | RPAREN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv495 * Lexing.position * _menhir_state * (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 3801 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos_kwd_, _menhir_s, (kwd : (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 3806 "smtlib/smtlib_parser.ml"
        )), _startpos_kwd_) = _menhir_stack in
        let _startpos = _startpos_kwd_ in
        let _endpos = _endpos_kwd_ in
        let _v : 'tv_attribute = let _endpos = _endpos_kwd_ in
        let _startpos = _startpos_kwd_ in
        
# 399 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_attribute (AttrKeyword kwd) loc )
# 3816 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_attribute _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv496)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState5

and _menhir_run189 : _menhir_env -> 'ttv_tail -> _menhir_state -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _startpos ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState189 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState189 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState189

and _menhir_goto_symbol : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> 'tv_symbol -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v, _startpos) in
    match _menhir_s with
    | MenhirState10 | MenhirState23 | MenhirState15 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv445 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv443 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos_symb_, _menhir_s, (symb : 'tv_symbol), _startpos_symb_) = _menhir_stack in
        let _v : 'tv_sexpr = let _endpos = _endpos_symb_ in
        let _startpos = _startpos_symb_ in
        
# 387 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_sexpr (SexprSymbol symb) loc )
# 3855 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_sexpr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv444)) : 'freshtv446)
    | MenhirState5 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv449 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv447 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos_symb_, _menhir_s, (symb : 'tv_symbol), _startpos_symb_) = _menhir_stack in
        let _endpos = _endpos_symb_ in
        let _v : 'tv_attribute_value = let _endpos = _endpos_symb_ in
        let _startpos = _startpos_symb_ in
        
# 375 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_attr_value (AttrValSymbol symb) loc )
# 3871 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_attribute_value _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv448)) : 'freshtv450)
    | MenhirState36 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv455 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv451 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState38 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState38
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState38) : 'freshtv452)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv453 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv454)) : 'freshtv456)
    | MenhirState75 | MenhirState66 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv457 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | NUMERAL _v ->
            _menhir_run69 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState67 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run68 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState67 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState67) : 'freshtv458)
    | MenhirState255 | MenhirState241 | MenhirState234 | MenhirState229 | MenhirState226 | MenhirState225 | MenhirState202 | MenhirState201 | MenhirState192 | MenhirState183 | MenhirState64 | MenhirState134 | MenhirState135 | MenhirState65 | MenhirState128 | MenhirState125 | MenhirState119 | MenhirState113 | MenhirState107 | MenhirState101 | MenhirState92 | MenhirState79 | MenhirState82 | MenhirState81 | MenhirState80 | MenhirState76 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv461 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv459 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos_symb_, _menhir_s, (symb : 'tv_symbol), _startpos_symb_) = _menhir_stack in
        let _startpos = _startpos_symb_ in
        let _endpos = _endpos_symb_ in
        let _v : 'tv_identifier = let _endpos = _endpos_symb_ in
        let _startpos = _startpos_symb_ in
        
# 303 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in mk_identifier (IdSymbol symb) loc )
# 3930 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_identifier _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv460)) : 'freshtv462)
    | MenhirState91 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv463 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BINARY _v ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState92 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | BOOL _v ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState92 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | DECIMAL _v ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState92 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | HEXADECIMAL _v ->
            _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState92 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | LPAREN ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState92 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | NUMERAL _v ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState92 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState92 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | STRING _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState92 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState92 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState92) : 'freshtv464)
    | MenhirState106 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv465 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState107 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState107 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState107 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState107) : 'freshtv466)
    | MenhirState177 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv471 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv467 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState179 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState179 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState179) : 'freshtv468)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv469 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv470)) : 'freshtv472)
    | MenhirState180 | MenhirState179 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv475 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState180 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState180 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv473 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _endpos_x_, _menhir_s, (x : 'tv_symbol), _startpos_x_) = _menhir_stack in
            let _v : 'tv_nonempty_list_symbol_ = 
# 221 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [ x ] )
# 4023 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_nonempty_list_symbol_ _menhir_env _menhir_stack _menhir_s _v) : 'freshtv474)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState180) : 'freshtv476)
    | MenhirState212 | MenhirState189 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv477 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | PAR ->
            _menhir_run191 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | LPAREN ->
            _menhir_reduce69 _menhir_env (Obj.magic _menhir_stack) MenhirState190
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState190) : 'freshtv478)
    | MenhirState217 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv487 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | NUMERAL _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv483 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            let (_v : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 4057 "smtlib/smtlib_parser.ml"
            )) = _v in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _endpos, _v, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv479 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 4069 "smtlib/smtlib_parser.ml"
                ) * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let _menhir_stack = (_menhir_stack, _endpos) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState220 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | EOF | RPAREN ->
                    _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState220
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState220) : 'freshtv480)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : ((('freshtv481 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 4091 "smtlib/smtlib_parser.ml"
                ) * Lexing.position) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _, _menhir_s, _, _), _, _, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv482)) : 'freshtv484)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv485 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv486)) : 'freshtv488)
    | MenhirState222 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv489 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | PAR ->
            _menhir_run191 _menhir_env (Obj.magic _menhir_stack) MenhirState223
        | LPAREN ->
            _menhir_reduce69 _menhir_env (Obj.magic _menhir_stack) MenhirState223
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState223) : 'freshtv490)
    | MenhirState233 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv491 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            _menhir_run80 _menhir_env (Obj.magic _menhir_stack) MenhirState234 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState234 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState234 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState234) : 'freshtv492)
    | _ ->
        _menhir_fail ()

and _menhir_run12 : _menhir_env -> ('ttv_tail * _menhir_state * Lexing.position) * _menhir_state -> _menhir_state -> (
# 135 "smtlib/smtlib_parser.mly"
       (string)
# 4138 "smtlib/smtlib_parser.ml"
) -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _v ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _v) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | NUMERAL _v ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv439 * _menhir_state * Lexing.position) * _menhir_state) * _menhir_state * (
# 135 "smtlib/smtlib_parser.mly"
       (string)
# 4150 "smtlib/smtlib_parser.ml"
        )) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_v : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 4156 "smtlib/smtlib_parser.ml"
        )) = _v in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _v, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv435 * _menhir_state * Lexing.position) * _menhir_state) * _menhir_state * (
# 135 "smtlib/smtlib_parser.mly"
       (string)
# 4168 "smtlib/smtlib_parser.ml"
            )) * Lexing.position * (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 4172 "smtlib/smtlib_parser.ml"
            ) * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv433 * _menhir_state * Lexing.position) * _menhir_state) * _menhir_state * (
# 135 "smtlib/smtlib_parser.mly"
       (string)
# 4180 "smtlib/smtlib_parser.ml"
            )) * Lexing.position * (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 4184 "smtlib/smtlib_parser.ml"
            ) * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos__5_ : Lexing.position) = _endpos in
            ((let ((((_menhir_stack, _menhir_s, _startpos__1_), _), _, (value : (
# 135 "smtlib/smtlib_parser.mly"
       (string)
# 4190 "smtlib/smtlib_parser.ml"
            ))), _endpos_size_, (size : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 4194 "smtlib/smtlib_parser.ml"
            )), _startpos_size_) = _menhir_stack in
            let _5 = () in
            let _2 = () in
            let _1 = () in
            let _startpos = _startpos__1_ in
            let _endpos = _endpos__5_ in
            let _v : 'tv_spec_constant = 
# 367 "smtlib/smtlib_parser.mly"
  ( CstDecimalSize(value, size) )
# 4204 "smtlib/smtlib_parser.ml"
             in
            _menhir_goto_spec_constant _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv434)) : 'freshtv436)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv437 * _menhir_state * Lexing.position) * _menhir_state) * _menhir_state * (
# 135 "smtlib/smtlib_parser.mly"
       (string)
# 4214 "smtlib/smtlib_parser.ml"
            )) * Lexing.position * (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 4218 "smtlib/smtlib_parser.ml"
            ) * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv438)) : 'freshtv440)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv441 * _menhir_state * Lexing.position) * _menhir_state) * _menhir_state * (
# 135 "smtlib/smtlib_parser.mly"
       (string)
# 4229 "smtlib/smtlib_parser.ml"
        )) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv442)

and _menhir_run75 : _menhir_env -> 'ttv_tail * _menhir_state * Lexing.position -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState75 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState75 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState75

and _menhir_run91 : _menhir_env -> 'ttv_tail -> _menhir_state -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _startpos ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState91 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState91 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState91

and _menhir_run106 : _menhir_env -> 'ttv_tail -> _menhir_state -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _startpos ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState106 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState106 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState106

and _menhir_run76 : _menhir_env -> 'ttv_tail * _menhir_state * Lexing.position -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let _menhir_stack = (_menhir_stack, _menhir_s) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAREN ->
        _menhir_run77 _menhir_env (Obj.magic _menhir_stack) MenhirState76 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState76 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState76 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState76

and _menhir_goto_spec_constant : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> 'tv_spec_constant -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_stack = (_menhir_stack, _endpos, _menhir_s, _v, _startpos) in
    match _menhir_s with
    | MenhirState10 | MenhirState23 | MenhirState15 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv401 * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv399 * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos_sc_, _menhir_s, (sc : 'tv_spec_constant), _startpos_sc_) = _menhir_stack in
        let _v : 'tv_sexpr = let _endpos = _endpos_sc_ in
        let _startpos = _startpos_sc_ in
        
# 384 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_sexpr (SexprConstant sc) loc )
# 4312 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_sexpr _menhir_env _menhir_stack _menhir_s _v) : 'freshtv400)) : 'freshtv402)
    | MenhirState5 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv405 * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv403 * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos_sc_, _menhir_s, (sc : 'tv_spec_constant), _startpos_sc_) = _menhir_stack in
        let _endpos = _endpos_sc_ in
        let _v : 'tv_attribute_value = let _endpos = _endpos_sc_ in
        let _startpos = _startpos_sc_ in
        
# 372 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_attr_value (AttrValSpecConstant sc) loc )
# 4328 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_attribute_value _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv404)) : 'freshtv406)
    | MenhirState255 | MenhirState241 | MenhirState202 | MenhirState64 | MenhirState134 | MenhirState135 | MenhirState128 | MenhirState125 | MenhirState119 | MenhirState113 | MenhirState101 | MenhirState92 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv409 * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv407 * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _endpos__1_, _menhir_s, (_1 : 'tv_spec_constant), _startpos__1_) = _menhir_stack in
        let _endpos = _endpos__1_ in
        let _v : 'tv_term = let _endpos = _endpos__1_ in
        let _startpos = _startpos__1_ in
        
# 318 "smtlib/smtlib_parser.mly"
 ( let loc = mk_loc _startpos _endpos in
   mk_term (TermSpecConstant _1) loc )
# 4344 "smtlib/smtlib_parser.ml"
         in
        _menhir_goto_term _menhir_env _menhir_stack _endpos _menhir_s _v) : 'freshtv408)) : 'freshtv410)
    | MenhirState256 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv431 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) = Obj.magic _menhir_stack in
        ((assert (not _menhir_env._menhir_error);
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv427 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv423 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let _menhir_stack = (_menhir_stack, _endpos) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | EOF ->
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv419 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
                    let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                    ((let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv417 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
                    let (_endpos__7_ : Lexing.position) = _endpos in
                    ((let ((((((_menhir_stack, _startpos__1_), _startpos__2_), _endpos_t_, _, (t : 'tv_term)), _endpos_v_, _, (v : 'tv_spec_constant), _startpos_v_), _endpos__5_), _endpos__6_) = _menhir_stack in
                    let _7 = () in
                    let _6 = () in
                    let _5 = () in
                    let _2 = () in
                    let _1 = () in
                    let _v : (
# 147 "smtlib/smtlib_parser.mly"
       (Smtlib.term * Smtlib.constant)
# 4385 "smtlib/smtlib_parser.ml"
                    ) = 
# 171 "smtlib/smtlib_parser.mly"
  ( t, v )
# 4389 "smtlib/smtlib_parser.ml"
                     in
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv415) = _menhir_stack in
                    let (_v : (
# 147 "smtlib/smtlib_parser.mly"
       (Smtlib.term * Smtlib.constant)
# 4396 "smtlib/smtlib_parser.ml"
                    )) = _v in
                    ((let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv413) = Obj.magic _menhir_stack in
                    let (_v : (
# 147 "smtlib/smtlib_parser.mly"
       (Smtlib.term * Smtlib.constant)
# 4403 "smtlib/smtlib_parser.ml"
                    )) = _v in
                    ((let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : 'freshtv411) = Obj.magic _menhir_stack in
                    let ((_1 : (
# 147 "smtlib/smtlib_parser.mly"
       (Smtlib.term * Smtlib.constant)
# 4410 "smtlib/smtlib_parser.ml"
                    )) : (
# 147 "smtlib/smtlib_parser.mly"
       (Smtlib.term * Smtlib.constant)
# 4414 "smtlib/smtlib_parser.ml"
                    )) = _v in
                    (Obj.magic _1 : 'freshtv412)) : 'freshtv414)) : 'freshtv416)) : 'freshtv418)) : 'freshtv420)
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    let (_menhir_env : _menhir_env) = _menhir_env in
                    let (_menhir_stack : ((((('freshtv421 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
                    ((let (((_menhir_stack, _, _menhir_s, _, _), _), _) = _menhir_stack in
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv422)) : 'freshtv424)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (((('freshtv425 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
                ((let ((_menhir_stack, _, _menhir_s, _, _), _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv426)) : 'freshtv428)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ((('freshtv429 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position * _menhir_state * 'tv_spec_constant * Lexing.position) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv430)) : 'freshtv432)
    | _ ->
        _menhir_fail ()

and _menhir_reduce16 : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    let (_, _startpos) = Obj.magic _menhir_stack in
    let _v : 'tv_list_delimited_LPAREN_command_RPAREN__ = 
# 211 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
    ( [] )
# 4447 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_list_delimited_LPAREN_command_RPAREN__ _menhir_env _menhir_stack _menhir_s _v _startpos

and _menhir_run3 : _menhir_env -> 'ttv_tail -> _menhir_state -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _startpos ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | ASSERT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv265 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BINARY _v ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState241 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | BOOL _v ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState241 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | DECIMAL _v ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState241 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | HEXADECIMAL _v ->
            _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState241 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | LPAREN ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState241 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | NUMERAL _v ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState241 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState241 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | STRING _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState241 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState241 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState241) : 'freshtv266)
    | CHECKSAT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv271 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv267 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState239 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState239
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState239) : 'freshtv268)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv269 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv270)) : 'freshtv272)
    | DECLARECONST ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv273 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState233 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState233 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState233) : 'freshtv274)
    | DECLAREFUN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv275 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState222 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState222 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState222) : 'freshtv276)
    | DECLARESORT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv277 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState217 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState217 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState217) : 'freshtv278)
    | DEFINEFUN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv279 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState212 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState212 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState212) : 'freshtv280)
    | DEFINEFUNREC ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv285 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv281 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run189 _menhir_env (Obj.magic _menhir_stack) MenhirState188 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState188) : 'freshtv282)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv283 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv284)) : 'freshtv286)
    | DEFINESORT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv287 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState177 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState177 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState177) : 'freshtv288)
    | ECHO ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv297 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | STRING _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv293 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            let (_v : (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 4643 "smtlib/smtlib_parser.ml"
            )) = _v in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _endpos, _v, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv289 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 4655 "smtlib/smtlib_parser.ml"
                ) * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let _menhir_stack = (_menhir_stack, _endpos) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState175 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | EOF | RPAREN ->
                    _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState175
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState175) : 'freshtv290)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv291 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 4677 "smtlib/smtlib_parser.ml"
                ) * Lexing.position) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s, _), _), _, _, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv292)) : 'freshtv294)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv295 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv296)) : 'freshtv298)
    | EXIT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv303 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv299 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState171 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState171
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState171) : 'freshtv300)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv301 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv302)) : 'freshtv304)
    | GETASSERTIONS ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv309 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv305 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState168 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState168
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState168) : 'freshtv306)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv307 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv308)) : 'freshtv310)
    | GETASSIGNMENT ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv315 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv311 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState165 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState165
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState165) : 'freshtv312)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv313 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv314)) : 'freshtv316)
    | GETINFO ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv331 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | KEYWORD _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv327) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            let (_v : (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 4799 "smtlib/smtlib_parser.ml"
            )) = _v in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_env = _menhir_discard _menhir_env in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv325) = Obj.magic _menhir_stack in
            let (_endpos_kwd_ : Lexing.position) = _endpos in
            let ((kwd : (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 4809 "smtlib/smtlib_parser.ml"
            )) : (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 4813 "smtlib/smtlib_parser.ml"
            )) = _v in
            let (_startpos_kwd_ : Lexing.position) = _startpos in
            ((let _endpos = _endpos_kwd_ in
            let _v : 'tv_info_flag = let _endpos = _endpos_kwd_ in
            let _startpos = _startpos_kwd_ in
            
# 414 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in
    mk_info_flag (InfoFlagKeyword kwd) loc )
# 4823 "smtlib/smtlib_parser.ml"
             in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv323) = _menhir_stack in
            let (_endpos : Lexing.position) = _endpos in
            let (_v : 'tv_info_flag) = _v in
            ((let _menhir_stack = (_menhir_stack, _endpos, _v) in
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : (('freshtv321 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * 'tv_info_flag) = Obj.magic _menhir_stack in
            ((assert (not _menhir_env._menhir_error);
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv317 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * 'tv_info_flag) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let _menhir_stack = (_menhir_stack, _endpos) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState162 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | EOF | RPAREN ->
                    _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState162
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState162) : 'freshtv318)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv319 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * 'tv_info_flag) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s, _), _), _, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv320)) : 'freshtv322)) : 'freshtv324)) : 'freshtv326)) : 'freshtv328)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv329 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv330)) : 'freshtv332)
    | GETMODEL ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv337 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv333 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState157 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState157
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState157) : 'freshtv334)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv335 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv336)) : 'freshtv338)
    | GETOPTION ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv347 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | KEYWORD _v ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv343 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            let (_v : (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 4912 "smtlib/smtlib_parser.ml"
            )) = _v in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _endpos, _v, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | RPAREN ->
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv339 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 4924 "smtlib/smtlib_parser.ml"
                ) * Lexing.position) = Obj.magic _menhir_stack in
                let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
                ((let _menhir_stack = (_menhir_stack, _endpos) in
                let _menhir_env = _menhir_discard _menhir_env in
                let _tok = _menhir_env._menhir_token in
                match _tok with
                | LPAREN ->
                    _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState154 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
                | EOF | RPAREN ->
                    _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState154
                | _ ->
                    assert (not _menhir_env._menhir_error);
                    _menhir_env._menhir_error <- true;
                    _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState154) : 'freshtv340)
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                let (_menhir_env : _menhir_env) = _menhir_env in
                let (_menhir_stack : (('freshtv341 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 4946 "smtlib/smtlib_parser.ml"
                ) * Lexing.position) = Obj.magic _menhir_stack in
                ((let (((_menhir_stack, _menhir_s, _), _), _, _, _) = _menhir_stack in
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv342)) : 'freshtv344)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv345 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv346)) : 'freshtv348)
    | GETPROOF ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv353 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv349 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState150 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState150
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState150) : 'freshtv350)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv351 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv352)) : 'freshtv354)
    | GETUNSATASSUMPTIONS ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv359 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv355 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState147 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState147
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState147) : 'freshtv356)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv357 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv358)) : 'freshtv360)
    | GETUNSATCORE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv365 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv361 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState144 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState144
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState144) : 'freshtv362)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv363 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv364)) : 'freshtv366)
    | GETVALUE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv371 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv367 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BINARY _v ->
                _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState64 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | BOOL _v ->
                _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState64 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | DECIMAL _v ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState64 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | HEXADECIMAL _v ->
                _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState64 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | LPAREN ->
                _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState64 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | NUMERAL _v ->
                _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState64 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState64 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | STRING _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState64 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState64 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState64) : 'freshtv368)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv369 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv370)) : 'freshtv372)
    | METAINFO ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv373 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | KEYWORD _v ->
            _menhir_run5 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState59 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState59) : 'freshtv374)
    | POP ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv375 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | NUMERAL _v ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState55 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | RPAREN ->
            _menhir_reduce67 _menhir_env (Obj.magic _menhir_stack) MenhirState55
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState55) : 'freshtv376)
    | PUSH ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv377 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | NUMERAL _v ->
            _menhir_run51 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState50 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | RPAREN ->
            _menhir_reduce67 _menhir_env (Obj.magic _menhir_stack) MenhirState50
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState50) : 'freshtv378)
    | RESET ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv383 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv379 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState48 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState48
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState48) : 'freshtv380)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv381 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv382)) : 'freshtv384)
    | RESETASSERTIONS ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv389 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _endpos, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | RPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv385 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState45 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | EOF | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState45
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState45) : 'freshtv386)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv387 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
            ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv388)) : 'freshtv390)
    | SETINFO ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv391 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | KEYWORD _v ->
            _menhir_run5 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState40 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState40) : 'freshtv392)
    | SETLOGIC ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv393 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState36 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState36 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState36) : 'freshtv394)
    | SETOPTION ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv395 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | KEYWORD _v ->
            _menhir_run5 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState4 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState4) : 'freshtv396)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv397 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv398)

and _menhir_errorcase : _menhir_env -> 'ttv_tail -> _menhir_state -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s ->
    match _menhir_s with
    | MenhirState257 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv69 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv70)
    | MenhirState256 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv71 * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv72)
    | MenhirState255 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv73 * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv74)
    | MenhirState249 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv75) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv76)
    | MenhirState243 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv77 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_term) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv78)
    | MenhirState241 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv79 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv80)
    | MenhirState239 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv81 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv82)
    | MenhirState236 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv83 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv84)
    | MenhirState234 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv85 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv86)
    | MenhirState233 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv87 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv88)
    | MenhirState231 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((((('freshtv89 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv90)
    | MenhirState229 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((((('freshtv91 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sort_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv92)
    | MenhirState226 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv93 * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv94)
    | MenhirState225 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv95 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv96)
    | MenhirState223 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv97 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv98)
    | MenhirState222 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv99 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv100)
    | MenhirState220 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv101 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position * (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 5348 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _, _menhir_s, _, _), _, _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv102)
    | MenhirState217 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv103 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv104)
    | MenhirState214 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv105 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_fun_nonrec_def) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv106)
    | MenhirState212 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv107 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv108)
    | MenhirState210 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv109 * _menhir_state * 'tv_fun_rec_def) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv110)
    | MenhirState208 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv111 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_fun_rec_def_) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv112)
    | MenhirState202 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv113 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sorted_var_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv114)
    | MenhirState201 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv115 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) * _menhir_state * 'tv_list_sorted_var_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv116)
    | MenhirState198 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv117 * _menhir_state * 'tv_sorted_var) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv118)
    | MenhirState197 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv119 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * _menhir_state * 'tv_option_poly_parameters_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv120)
    | MenhirState192 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv121 * _menhir_state) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv122)
    | MenhirState190 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv123 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv124)
    | MenhirState189 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv125 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv126)
    | MenhirState188 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv127 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv128)
    | MenhirState185 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((((('freshtv129 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) * Lexing.position) * Lexing.position * _menhir_state * 'tv_sort) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv130)
    | MenhirState183 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv131 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_symbol_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv132)
    | MenhirState180 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv133 * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv134)
    | MenhirState179 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv135 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv136)
    | MenhirState177 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv137 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv138)
    | MenhirState175 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv139 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 5447 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _), _), _, _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv140)
    | MenhirState171 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv141 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv142)
    | MenhirState168 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv143 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv144)
    | MenhirState165 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv145 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv146)
    | MenhirState162 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv147 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * 'tv_info_flag) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _), _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv148)
    | MenhirState157 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv149 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv150)
    | MenhirState154 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv151 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 5481 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((((_menhir_stack, _menhir_s, _), _), _, _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv152)
    | MenhirState150 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv153 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv154)
    | MenhirState147 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv155 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv156)
    | MenhirState144 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv157 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv158)
    | MenhirState141 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((((('freshtv159 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) * _menhir_state * 'tv_nonempty_list_term_) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv160)
    | MenhirState135 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv161 * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv162)
    | MenhirState134 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv163 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_qual_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv164)
    | MenhirState132 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv165 * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv166)
    | MenhirState129 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv167 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_term) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv168)
    | MenhirState128 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv169 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv170)
    | MenhirState125 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv171 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv172)
    | MenhirState123 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv173 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv174)
    | MenhirState119 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv175 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv176)
    | MenhirState117 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv177 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv178)
    | MenhirState113 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv179 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_sorted_var_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv180)
    | MenhirState110 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv181 * _menhir_state * 'tv_sorted_var) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv182)
    | MenhirState107 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv183 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv184)
    | MenhirState106 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv185 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv186)
    | MenhirState105 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv187 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv188)
    | MenhirState101 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (((('freshtv189 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) * _menhir_state * 'tv_nonempty_list_var_binding_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv190)
    | MenhirState98 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv191 * _menhir_state * 'tv_var_binding) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv192)
    | MenhirState92 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv193 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv194)
    | MenhirState91 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv195 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv196)
    | MenhirState90 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv197 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv198)
    | MenhirState82 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv199 * Lexing.position * _menhir_state * 'tv_sort) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv200)
    | MenhirState81 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv201 * _menhir_state * Lexing.position) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv202)
    | MenhirState80 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv203 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv204)
    | MenhirState79 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv205 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_identifier * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv206)
    | MenhirState77 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv207 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv208)
    | MenhirState76 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv209 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv210)
    | MenhirState75 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv211 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv212)
    | MenhirState74 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv213 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv214)
    | MenhirState72 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv215 * _menhir_state * 'tv_index) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv216)
    | MenhirState67 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv217 * _menhir_state * Lexing.position) * _menhir_state) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv218)
    | MenhirState66 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv219 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv220)
    | MenhirState65 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv221 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv222)
    | MenhirState64 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv223 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv224)
    | MenhirState61 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv225 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv226)
    | MenhirState59 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv227 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv228)
    | MenhirState57 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv229 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv230)
    | MenhirState55 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv231 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv232)
    | MenhirState53 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv233 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position * _menhir_state * 'tv_option_NUMERAL_) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv234)
    | MenhirState50 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv235 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv236)
    | MenhirState48 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv237 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv238)
    | MenhirState45 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : (('freshtv239 * _menhir_state * Lexing.position) * Lexing.position * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (((_menhir_stack, _menhir_s, _), _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv240)
    | MenhirState42 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv241 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_attribute * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv242)
    | MenhirState40 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv243 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv244)
    | MenhirState38 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv245 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_symbol * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv246)
    | MenhirState36 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv247 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv248)
    | MenhirState33 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ((('freshtv249 * _menhir_state * Lexing.position) * Lexing.position) * Lexing.position * _menhir_state * 'tv_smt_option) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv250)
    | MenhirState23 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv251 * _menhir_state * 'tv_sexpr) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv252)
    | MenhirState15 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv253 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv254)
    | MenhirState11 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv255 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv256)
    | MenhirState10 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv257 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _menhir_s, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv258)
    | MenhirState5 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv259 * Lexing.position * _menhir_state * (
# 139 "smtlib/smtlib_parser.mly"
       (string)
# 5755 "smtlib/smtlib_parser.ml"
        ) * Lexing.position) = Obj.magic _menhir_stack in
        ((let (_menhir_stack, _, _menhir_s, _, _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv260)
    | MenhirState4 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv261 * _menhir_state * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        ((let ((_menhir_stack, _menhir_s, _), _) = _menhir_stack in
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv262)
    | MenhirState2 ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : ('freshtv263 * Lexing.position) * Lexing.position) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv264)

and _menhir_run6 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 140 "smtlib/smtlib_parser.mly"
       (string)
# 5772 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv67) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 140 "smtlib/smtlib_parser.mly"
       (string)
# 5783 "smtlib/smtlib_parser.ml"
    )) : (
# 140 "smtlib/smtlib_parser.mly"
       (string)
# 5787 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _startpos = _startpos__1_ in
    let _endpos = _endpos__1_ in
    let _v : 'tv_symbol = let _endpos = _endpos__1_ in
    let _startpos = _startpos__1_ in
    
# 311 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in mk_symbol (SimpleSymbol _1) loc )
# 5797 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_symbol _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv68)

and _menhir_run7 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 5804 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv65) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 5815 "smtlib/smtlib_parser.ml"
    )) : (
# 137 "smtlib/smtlib_parser.mly"
       (string)
# 5819 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _startpos = _startpos__1_ in
    let _endpos = _endpos__1_ in
    let _v : 'tv_spec_constant = 
# 362 "smtlib/smtlib_parser.mly"
          ( CstString _1 )
# 5827 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_spec_constant _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv66)

and _menhir_run8 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 141 "smtlib/smtlib_parser.mly"
       (string)
# 5834 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv63) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 141 "smtlib/smtlib_parser.mly"
       (string)
# 5845 "smtlib/smtlib_parser.ml"
    )) : (
# 141 "smtlib/smtlib_parser.mly"
       (string)
# 5849 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _startpos = _startpos__1_ in
    let _endpos = _endpos__1_ in
    let _v : 'tv_symbol = let _endpos = _endpos__1_ in
    let _startpos = _startpos__1_ in
    
# 313 "smtlib/smtlib_parser.mly"
  ( let loc = mk_loc _startpos _endpos in mk_symbol (QuotedSymbol _1) loc )
# 5859 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_symbol _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv64)

and _menhir_run9 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 5866 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv61) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 5877 "smtlib/smtlib_parser.ml"
    )) : (
# 132 "smtlib/smtlib_parser.mly"
       (Smtlib.numeral)
# 5881 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _startpos = _startpos__1_ in
    let _endpos = _endpos__1_ in
    let _v : 'tv_spec_constant = 
# 361 "smtlib/smtlib_parser.mly"
          ( CstNumeral _1 )
# 5889 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_spec_constant _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv62)

and _menhir_run65 : _menhir_env -> 'ttv_tail -> _menhir_state -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _menhir_s _startpos ->
    let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
    let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | AS ->
        _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState65
    | BANG ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv31 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState65 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BINARY _v ->
            _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState128 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | BOOL _v ->
            _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState128 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | DECIMAL _v ->
            _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState128 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | HEXADECIMAL _v ->
            _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState128 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | LPAREN ->
            _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState128 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | NUMERAL _v ->
            _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState128 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState128 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | STRING _v ->
            _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState128 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState128 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState128) : 'freshtv32)
    | EXISTS ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv37 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState65 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv33 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run106 _menhir_env (Obj.magic _menhir_stack) MenhirState123 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState123) : 'freshtv34)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv35 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv36)) : 'freshtv38)
    | FORALL ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv43 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState65 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv39 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run106 _menhir_env (Obj.magic _menhir_stack) MenhirState117 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState117) : 'freshtv40)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv41 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv42)) : 'freshtv44)
    | LAMBDA ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv49 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState65 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv45 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run106 _menhir_env (Obj.magic _menhir_stack) MenhirState105 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState105) : 'freshtv46)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv47 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv48)) : 'freshtv50)
    | LET ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv55 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState65 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv51 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run91 _menhir_env (Obj.magic _menhir_stack) MenhirState90 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState90) : 'freshtv52)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : ('freshtv53 * _menhir_state * Lexing.position) * _menhir_state) = Obj.magic _menhir_stack in
            ((let (_menhir_stack, _menhir_s) = _menhir_stack in
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) _menhir_s) : 'freshtv54)) : 'freshtv56)
    | LPAREN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv57) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState65 in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _menhir_s, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | AS ->
            _menhir_run76 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | UNDERSCORE ->
            _menhir_run75 _menhir_env (Obj.magic _menhir_stack) MenhirState74
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState74) : 'freshtv58)
    | QUOTEDSYMBOL _v ->
        _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState65 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | SYMBOL _v ->
        _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState65 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | UNDERSCORE ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv59 * _menhir_state * Lexing.position) = Obj.magic _menhir_stack in
        let (_menhir_s : _menhir_state) = MenhirState65 in
        ((let _menhir_stack = (_menhir_stack, _menhir_s) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | BV_NUMERAL _v ->
            _menhir_run12 _menhir_env (Obj.magic _menhir_stack) MenhirState66 _v
        | QUOTEDSYMBOL _v ->
            _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState66 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | SYMBOL _v ->
            _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState66 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState66) : 'freshtv60)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState65

and _menhir_run17 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 134 "smtlib/smtlib_parser.mly"
       (string)
# 6094 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv29) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 134 "smtlib/smtlib_parser.mly"
       (string)
# 6105 "smtlib/smtlib_parser.ml"
    )) : (
# 134 "smtlib/smtlib_parser.mly"
       (string)
# 6109 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _startpos = _startpos__1_ in
    let _endpos = _endpos__1_ in
    let _v : 'tv_spec_constant = 
# 364 "smtlib/smtlib_parser.mly"
              ( CstHexadecimal _1 )
# 6117 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_spec_constant _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv30)

and _menhir_run18 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 133 "smtlib/smtlib_parser.mly"
       (string)
# 6124 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv27) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 133 "smtlib/smtlib_parser.mly"
       (string)
# 6135 "smtlib/smtlib_parser.ml"
    )) : (
# 133 "smtlib/smtlib_parser.mly"
       (string)
# 6139 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _startpos = _startpos__1_ in
    let _endpos = _endpos__1_ in
    let _v : 'tv_spec_constant = 
# 360 "smtlib/smtlib_parser.mly"
          ( CstDecimal _1 )
# 6147 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_spec_constant _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv28)

and _menhir_run19 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 138 "smtlib/smtlib_parser.mly"
       (bool)
# 6154 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv25) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 138 "smtlib/smtlib_parser.mly"
       (bool)
# 6165 "smtlib/smtlib_parser.ml"
    )) : (
# 138 "smtlib/smtlib_parser.mly"
       (bool)
# 6169 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _startpos = _startpos__1_ in
    let _endpos = _endpos__1_ in
    let _v : 'tv_spec_constant = 
# 363 "smtlib/smtlib_parser.mly"
          ( CstBool _1 )
# 6177 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_spec_constant _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv26)

and _menhir_run20 : _menhir_env -> 'ttv_tail -> Lexing.position -> _menhir_state -> (
# 136 "smtlib/smtlib_parser.mly"
       (string)
# 6184 "smtlib/smtlib_parser.ml"
) -> Lexing.position -> 'ttv_return =
  fun _menhir_env _menhir_stack _endpos _menhir_s _v _startpos ->
    let _menhir_env = _menhir_discard _menhir_env in
    let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv23) = Obj.magic _menhir_stack in
    let (_endpos__1_ : Lexing.position) = _endpos in
    let (_menhir_s : _menhir_state) = _menhir_s in
    let ((_1 : (
# 136 "smtlib/smtlib_parser.mly"
       (string)
# 6195 "smtlib/smtlib_parser.ml"
    )) : (
# 136 "smtlib/smtlib_parser.mly"
       (string)
# 6199 "smtlib/smtlib_parser.ml"
    )) = _v in
    let (_startpos__1_ : Lexing.position) = _startpos in
    ((let _startpos = _startpos__1_ in
    let _endpos = _endpos__1_ in
    let _v : 'tv_spec_constant = 
# 359 "smtlib/smtlib_parser.mly"
          ( CstBinary _1 )
# 6207 "smtlib/smtlib_parser.ml"
     in
    _menhir_goto_spec_constant _menhir_env _menhir_stack _endpos _menhir_s _v _startpos) : 'freshtv24)

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

and model : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 145 "smtlib/smtlib_parser.mly"
       (Smtlib.model)
# 6236 "smtlib/smtlib_parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv21) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAREN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv17) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | MODEL ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv13 * Lexing.position) = Obj.magic _menhir_stack in
            let (_endpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_curr_p in
            ((let _menhir_stack = (_menhir_stack, _endpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | LPAREN ->
                _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState2 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | RPAREN ->
                _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState2
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState2) : 'freshtv14)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv15 * Lexing.position) = Obj.magic _menhir_stack in
            (raise _eRR : 'freshtv16)) : 'freshtv18)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv19) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv20)) : 'freshtv22))

and script : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 143 "smtlib/smtlib_parser.mly"
       (Smtlib.script)
# 6285 "smtlib/smtlib_parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv11) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAREN ->
        _menhir_run3 _menhir_env (Obj.magic _menhir_stack) MenhirState249 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
    | EOF ->
        _menhir_reduce16 _menhir_env (Obj.magic _menhir_stack) MenhirState249
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState249) : 'freshtv12))

and value : (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (
# 147 "smtlib/smtlib_parser.mly"
       (Smtlib.term * Smtlib.constant)
# 6306 "smtlib/smtlib_parser.ml"
) =
  fun lexer lexbuf ->
    let _menhir_env = _menhir_init lexer lexbuf in
    Obj.magic (let (_menhir_env : _menhir_env) = _menhir_env in
    let (_menhir_stack : 'freshtv9) = ((), _menhir_env._menhir_lexbuf.Lexing.lex_curr_p) in
    ((let _menhir_env = _menhir_discard _menhir_env in
    let _tok = _menhir_env._menhir_token in
    match _tok with
    | LPAREN ->
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv5) = Obj.magic _menhir_stack in
        let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
        ((let _menhir_stack = (_menhir_stack, _startpos) in
        let _menhir_env = _menhir_discard _menhir_env in
        let _tok = _menhir_env._menhir_token in
        match _tok with
        | LPAREN ->
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv1 * Lexing.position) = Obj.magic _menhir_stack in
            let (_startpos : Lexing.position) = _menhir_env._menhir_lexbuf.Lexing.lex_start_p in
            ((let _menhir_stack = (_menhir_stack, _startpos) in
            let _menhir_env = _menhir_discard _menhir_env in
            let _tok = _menhir_env._menhir_token in
            match _tok with
            | BINARY _v ->
                _menhir_run20 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState255 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | BOOL _v ->
                _menhir_run19 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState255 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | DECIMAL _v ->
                _menhir_run18 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState255 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | HEXADECIMAL _v ->
                _menhir_run17 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState255 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | LPAREN ->
                _menhir_run65 _menhir_env (Obj.magic _menhir_stack) MenhirState255 _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | NUMERAL _v ->
                _menhir_run9 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState255 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | QUOTEDSYMBOL _v ->
                _menhir_run8 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState255 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | STRING _v ->
                _menhir_run7 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState255 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | SYMBOL _v ->
                _menhir_run6 _menhir_env (Obj.magic _menhir_stack) _menhir_env._menhir_lexbuf.Lexing.lex_curr_p MenhirState255 _v _menhir_env._menhir_lexbuf.Lexing.lex_start_p
            | _ ->
                assert (not _menhir_env._menhir_error);
                _menhir_env._menhir_error <- true;
                _menhir_errorcase _menhir_env (Obj.magic _menhir_stack) MenhirState255) : 'freshtv2)
        | _ ->
            assert (not _menhir_env._menhir_error);
            _menhir_env._menhir_error <- true;
            let (_menhir_env : _menhir_env) = _menhir_env in
            let (_menhir_stack : 'freshtv3 * Lexing.position) = Obj.magic _menhir_stack in
            (raise _eRR : 'freshtv4)) : 'freshtv6)
    | _ ->
        assert (not _menhir_env._menhir_error);
        _menhir_env._menhir_error <- true;
        let (_menhir_env : _menhir_env) = _menhir_env in
        let (_menhir_stack : 'freshtv7) = Obj.magic _menhir_stack in
        (raise _eRR : 'freshtv8)) : 'freshtv10))

# 269 "/nix/store/1c8vhb9i13ra7qzds164gkxhyw4qzk0i-menhir-20190626/share/menhir/standard.mly"
  

# 6369 "smtlib/smtlib_parser.ml"
