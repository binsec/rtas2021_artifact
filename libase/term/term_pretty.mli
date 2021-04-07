(**************************************************************************)
(*                                                                        *)
(*  This file is part of Frama-C.                                         *)
(*                                                                        *)
(*  Copyright (C) 2007-2014                                               *)
(*    CEA (Commissariat à l'énergie atomique et aux énergies              *)
(*         alternatives)                                                  *)
(*                                                                        *)
(*  you can redistribute it and/or modify it under the terms of the GNU   *)
(*  Lesser General Public License as published by the Free Software       *)
(*  Foundation, version 2.1.                                              *)
(*                                                                        *)
(*  It is distributed in the hope that it will be useful,                 *)
(*  but WITHOUT ANY WARRANTY; without even the implied warranty of        *)
(*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *)
(*  GNU Lesser General Public License for more details.                   *)
(*                                                                        *)
(*  See the GNU Lesser General Public License version 2.1                 *)
(*  for more details (enclosed in the file licenses/LGPLv2.1).            *)
(*                                                                        *)
(**************************************************************************)

open Term_type;;

(* Simple (exponential) pretty printer; guaranteed not to fail. *)
val pretty_simple: Format.formatter -> term -> unit

(* Default pretty printer. *)
val pretty: Format.formatter -> term -> unit

(* The same as above, but does not print the definitions. *)
val pretty_short: Format.formatter -> term -> unit

(* Customizable pretty-printer. 
   [print_defs] is a boolean telling whether to print the definitions
   for the names that are substituted in the term.
   [named] is a predicate returning true if the term 
   should be named with a let.
   [print_name] is a format function printing the variable name
   when the term is named.  
   [repl] is a function that allows to substituted printing of one
   term by another term. *)
val pretty_custom:
  ?print_defs:bool -> 
  ?named:(term -> bool) ->
  ?print_name:(Format.formatter -> term -> unit) ->
  ?repl:(term -> term) ->
  Format.formatter -> term -> unit

(* To print a term kind instead of a full term. *)
val pretty_kind:
  (Format.formatter -> term -> unit) ->
  Format.formatter -> kind -> unit

val pretty_type: Format.formatter -> Term_types.t -> unit
