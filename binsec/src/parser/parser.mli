
(* The type of tokens. *)

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
  | STRING of (string)
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
  | INT of (string)
  | INFER
  | IF
  | IDENT of (string)
  | HEXA of (string)
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
  | BIN of (string)
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

(* This exception is raised by the monolithic API functions. *)

exception Error

(* The monolithic API. *)

val patchmap: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Binstream.t Virtual_address.Map.t)

val instruction_eof: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Dba.Instr.t)

val initialization: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Parse_helpers.Initialization.t list)

val expr_eof: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Dba.Expr.t)

val directives: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Directive.t list)

val dhunk_substitutions_eof: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> ((Virtual_address.t * Dhunk.t) list)

val dhunk_eof: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Dhunk.t)

val decoder_msg: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> ((string * Parse_helpers.Message.Value.t)  list * (Dba_types.Caddress.Map.key * Dba.Instr.t) list)

val decoder_base: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> ((string * Parse_helpers.Message.Value.t)  list)

val dba: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> ('a Dba_types.program)

val body: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> ((Dba_types.Caddress.Map.key * Dba.Instr.t) list)
