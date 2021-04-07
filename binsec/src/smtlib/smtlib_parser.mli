
(* The type of tokens. *)

type token = 
  | UNDERSCORE
  | SYMBOL of (string)
  | STRING of (string)
  | SETOPTION
  | SETLOGIC
  | SETINFO
  | RPAREN
  | RESETASSERTIONS
  | RESET
  | QUOTEDSYMBOL of (string)
  | PUSH
  | POP
  | PAR
  | NUMERAL of (Smtlib.numeral)
  | MODEL
  | METAINFO
  | LPAREN
  | LET
  | LAMBDA
  | KEYWORD of (string)
  | HEXADECIMAL of (string)
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
  | DECIMAL of (string)
  | CHECKSAT
  | BV_NUMERAL of (string)
  | BOOL of (bool)
  | BINARY of (string)
  | BANG
  | ASSERT
  | AS

(* This exception is raised by the monolithic API functions. *)

exception Error

(* The monolithic API. *)

val value: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Smtlib.term * Smtlib.constant)

val script: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Smtlib.script)

val model: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Smtlib.model)
