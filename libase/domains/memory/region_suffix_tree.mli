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

open Memory_sig

module Make(Scalar:Domain_sig.Base):sig

  module Operable_Value:sig
    (* This operable value can also be used with the "naive" Region_numeric memory.  *)
    include Region_numeric_offset.Operable_Value_Enumerable 
      with module Scalar = Scalar
             
  end
  
  module Memory(Value:Value with module Scalar = Scalar)():sig
    include Memory_Region
      with module Scalar = Scalar
       and module Value = Value
       and type address = Operable_Value.binary
  end
end
