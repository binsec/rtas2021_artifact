
(* The type of tokens. *)

type token = 
  | UNSATT
  | SATT
  | RPAREN
  | NUM of (string)
  | LPAREN
  | IDENT of (string)
  | ERROR_MSG
  | ERROR
  | EOF

(* This exception is raised by the monolithic API functions. *)

exception Error

(* The monolithic API. *)

val main: (Lexing.lexbuf -> token) -> Lexing.lexbuf -> (Formula.status option * (string * string) list list)
