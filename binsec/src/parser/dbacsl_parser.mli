
(* The type of tokens. *)

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
  | INT of (string)
  | INFER
  | IFJUMP
  | IDENT of (string)
  | HEXA of (string * int)
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

(* This exception is raised by the monolithic API functions. *)

exception Error

(* The monolithic API. *)

val term: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Dba.Instr.t)
