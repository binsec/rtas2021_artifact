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

module type Operable_Value_Enumerable = sig
  include Operable_Value_Region
  val fold_crop:
    size:int -> Scalar.Context.t ->
    binary -> inf:Z.t -> sup:Z.t -> (Z.t -> 'a -> 'a) -> 'a -> 'a
  val is_precise:
    size:int -> Scalar.Context.t -> binary -> Z.t Memory_sig.precision             
end

module type S = sig

  module Scalar : Domain_sig.Base

  (* A simple operable value that directly uses the Scalar.binary to
     represent memory locations. *)
  module Operable_Value:sig

    type binary
    
    module Binary_Forward:Transfer_functions.Binary_Forward
      with module Arity := Domain_sig.Context_Arity_Forward(Scalar.Context)
       and type boolean := Scalar.boolean
       and type binary := binary  
         
    include Operable_Value_Enumerable
      with module Scalar = Scalar
       and type binary := binary
       and module Binary_Forward := Binary_Forward
         
  end
  

  (* Can be used with any address, as long as we can enumerate on their value. *)
  module Memory
      (Address:Operable_Value_Enumerable with module Scalar = Scalar)
      (Value:Value with module Scalar = Scalar):sig
    include Memory_Region
      with module Scalar = Scalar
       and module Value = Value
       and type address = Address.binary
  end

end

module Make(Scalar:Domain_sig.Base) : S with module Scalar = Scalar
