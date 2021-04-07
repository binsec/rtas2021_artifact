(**************************************************************************)
(*                                                                        *)
(*  This file is part of Codex.                                           *)
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

module Make
    (Scalar:Domain_sig.Base)
    (Operable_Value:Memory_sig.Operable_Value_Whole with module Scalar = Scalar)
    (Whole_Memory:Memory_sig.Whole_Memory
     with module Scalar = Scalar
      and type Value.binary = Operable_Value.binary
      and type address = Operable_Value.binary)
  :sig
    include Domain_sig.Base
      with type binary = Operable_Value.binary
      and type boolean = Scalar.boolean
      and module Context = Operable_Value.Scalar.Context
      and module Binary_Forward = Operable_Value.Binary_Forward
end

