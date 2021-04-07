(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2017                                               *)
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

(* Takes an Operable_Value and Memory describing operations inside a
   memory region, and create an Operable_Value and Memory describing
   operations between memory regions. *)
module Make(OV:Memory_sig.Operable_Value_Region):sig

  module Operable_Value:Memory_sig.Operable_Value_Whole
    with module Scalar = OV.Scalar

  module Memory(Region:Memory_sig.Memory_Region
             with module Scalar = OV.Scalar
              and type address = OV.binary
              and module Value.Scalar = OV.Scalar
            ):Memory_sig.Whole_Memory
    with module Scalar = OV.Scalar
     and module Value = Region.Value
     and type address = Operable_Value.binary

end
