
(* The type of tokens. *)

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

(* This exception is raised by the monolithic API functions. *)

exception Error

(* The monolithic API. *)

val policy: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Policy_type.policy)
