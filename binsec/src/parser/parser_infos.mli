
(* The type of tokens. *)

type token = 
  | XOR
  | UNDEF
  | TRUE
  | SUPER
  | STRING of (string)
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
  | INT of (string)
  | INFER
  | IFJUMP
  | IDENT of (string)
  | HIGHUNSIGNEDTHRESHOLDS
  | HIGHSIGNEDTHRESHOLDS
  | HEXADDRESS of (string * int)
  | HEXA of (string * int)
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

(* This exception is raised by the monolithic API functions. *)

exception Error

(* The monolithic API. *)

val configuration: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Infos.t)
