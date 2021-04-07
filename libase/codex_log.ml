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

type category = string
let register_category x = x

module type S = sig
  val warning:  ('a, Format.formatter, unit) format -> 'a
  val error:  ('a, Format.formatter, unit) format -> 'a
  val feedback:  ('a, Format.formatter, unit) format -> 'a  
  val performance_warning:  ('a, Format.formatter, unit) format -> 'a
  val imprecision_warning:  ('a, Format.formatter, unit) format -> 'a    
  val debug: ?category:string -> ('a, Format.formatter, unit) format -> 'a
  val fatal: ('a, Format.formatter, unit, 'b) format4 -> 'a

end


module Default:S = struct
  let logwith x = Format.kfprintf (fun fmt ->
      Format.pp_print_newline fmt ();
      Format.pp_print_flush fmt ()
    ) Format.err_formatter x
  ;;

  let warning = logwith
  let error = logwith
  let feedback = logwith
  let performance_warning = logwith
  let imprecision_warning = logwith
  let debug ?category = logwith
  let fatal str = Format.kfprintf (fun fmt ->
      Format.pp_print_newline fmt ();
      Format.pp_print_flush fmt ();
      assert false)
      Format.err_formatter str ;;
end

let r = ref (module Default:S);;
let register (module X:S) = r:= (module X);;

(* Does not print anything (except fatal errors). *)
module Null:S = struct
  let logwith x = Format.ikfprintf  (fun fmt ->
      Format.pp_print_newline fmt ();
      Format.pp_print_flush fmt ()
    ) Format.err_formatter x
  ;;

  let warning = logwith
  let error = logwith
  let feedback = logwith
  let performance_warning = logwith
  let imprecision_warning = logwith
  let debug ?category = logwith
  let fatal str = Format.kfprintf (fun fmt ->
      Format.pp_print_newline fmt ();
      Format.pp_print_flush fmt ();
      assert false)
      Format.err_formatter str ;;
end


module Dynamic:S = struct
  (* let register_category = let module X:S = (val !r) in X.register_category;; *)
  let warning fmt = let module X:S = (val !r) in X.warning fmt;;
  let error fmt = let module X:S = (val !r) in X.error fmt;;
  let performance_warning fmt = let module X:S = (val !r) in X.performance_warning fmt;;
  let imprecision_warning fmt = let module X:S = (val !r) in X.imprecision_warning fmt;;
  let debug ?category fmt = let module X:S = (val !r) in X.debug ?category fmt;;
  let fatal fmt = let module X:S = (val !r) in X.fatal fmt;;
  let feedback fmt = let module X:S = (val !r) in X.feedback fmt;;  
end

module Used = Dynamic
(* module Used = Default *)

include Used

(* Make cleaner printers, tied to a category. *)
module Make(P:sig
    val category:string         (* Default category. *)
    val debug:bool              (* If true, print debug information for this category. *)
  end) = struct
  include Used
  let debug ?category str =
    let category = match category with
      | Some category -> category
      | None -> P.category
    in
    if P.debug then debug ~category str else Format.ifprintf Format.err_formatter str;;
end
