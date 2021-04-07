(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2015                                               *)
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

open Codex

  (* module A0 = struct include Domains.Simple_memory.Bare  ;; include To_add end *)
  (* module A1 = struct include Domains.Simple_memory.With_Bottom  ;; include To_add end *)
    
  (* module M = Domains.Finite_memory_symbolic_binary.Make(Domains.Simple_binary) *)
  (* module A2 = struct include M.Bare;; include To_add end *)
  (* module A3 = struct include M.With_Bottom;; include To_add end *)

  (* module FMArg = struct *)
  (*   include Domains.Eager_evaluating_basis.Make_Binary_Finite(Simple_binary_basis) *)
  (* end *)
  (* module M2 = Domains.Finite_memory.Make(FMArg) *)
  (* module A4 = struct include M2;; include To_add end *)

  (* module A4_With_Bottom = struct *)
  (*   include With_Bottom.Make_Memory_Forward(A4) *)
  (*   include To_add *)
  (* end *)

(* module Symbolic_Binary = Domains.Evaluating.Make_Memory_Binary_Boolean(Domains.Disjunctive_binary) *)


module FramaC_Log:Codex_log.S = struct
  type category = Codex_options.category

  let performance = Codex_options.register_category "performance";;
  let imprecision = Codex_options.register_category "imprecision";;    
  let warning fmt = Codex_options.warning fmt
  let error fmt = Codex_options.error fmt    
  let performance_warning fmt = Codex_options.feedback fmt ~dkey:performance
  let imprecision_warning fmt = Codex_options.feedback fmt ~dkey:imprecision
  let debug ?category fmt = 
    let dkey = match category with
      | None -> None
      | Some c ->  Some(Codex_options.register_category c)
    in
    Codex_options.debug ?dkey fmt
  let fatal fmt = Codex_options.fatal fmt
  let feedback fmt = Codex_options.feedback fmt
  
end;;

(* module A6 = struct *)
(*   include With_Bottom.Make_Memory_Forward(Domains.Finite_memory.Make(Domains.Disjunctive_binary))       *)
(*     (\* include With_Bottom.Make_Memory_Forward(Domains.Finite_memory.Make(Domains.Simple_binary)) *\) *)
(*   include To_add *)
(*   end *)

(* module A7 = struct *)

(*   (\* module Mem = (Domains.Finite_memory_symbolic_binary.Make(Domains.Disjunctive_binary)) *\) *)
(*   (\* include With_Bottom.Make_Memory_Forward(Domains.Finite_memory.Make(Domains.Disjunctive_binary)) *\) *)
(*   include With_Bottom.Make_Memory_Forward(Mem.With_Empty) *)
(*   include To_add *)
(* end *)


(* module Make_From_Binary(DD:(\* Finite_memory_sig.Finite_Memory_Value *\)Domain_sig.Base *)
(*                       with type Query.Boolean.t = Lattices.Quadrivalent.t) = struct *)
(*   (\* module TermD = Term_domain.Make(DD.Ref_Addr) *\) *)
(*   module Binary_With_Term = Prod_with_term(DD) *)
(*   include With_Bottom.Make_Memory_Forward(Finite_memory.Make(Binary_With_Term)) *)
(*   include To_add *)
(* end *)
(* module A8 = Make_From_Binary(Domains.Disjunctive_binary) *)

let abstract_domain x:(module Domain_sig.Base) = match x with
  (* | "0" -> (module A0:Domains.Evaluating.Arg) *)
  (* | "1" -> (module A1:Domains.Evaluating.Arg) *)
  (* | "2" -> (module A2:Domains.Evaluating.Arg) *)
  (* | "3" -> (module A3:Domains.Evaluating.Arg) *)
  (* | "6" -> (module A6:Domains.Evaluating.Arg) *)
  (* | "4" -> (module A4_With_Bottom:Domains.Evaluating.Arg) *)
  | _ -> assert false

module Run_For(Abstract_Domain:Domain_sig.Base) = struct

  module Analysis = Domains.Evaluating.Make_Memory_Binary_Boolean(Abstract_Domain)

  let print_value_of_exp1 exp_to_term ctx fmt (indent,ki,exp) =
    try
      let size = Cil.bitsSizeOf @@ Cil.typeOf exp in
      let exptrie = exp_to_term (ki,exp) in
      let recover_basis term =
        (* Codex_log.feedback "expression %a; term %a"
         *   Cil_datatype.Exp.pretty exp Term.pretty term; *)
        Analysis.Query.binary ~size ctx term in
      let lattice = Compilation_to_term.Callstack_Trie.fold exptrie
          (fun (x,term) acc -> match x with
             | `Boolean -> assert false (* Inject singleton binary 0 or 1. *)
             | `Binary -> Analysis.Query.Binary_Lattice.join ~size acc @@ recover_basis term) @@ Analysis.Query.Binary_Lattice.bottom ~size in
      Analysis.Query.Binary_Lattice.pretty ~size fmt lattice
    with Not_found -> Format.fprintf fmt "<unknown>"

  let print_value_of_exp exp_to_term ctx fmt (indent,ki,exp) =
  
    (try
       let size =
         let typ = Cil.typeOf exp in
         try Cil.bitsSizeOf typ
         with Cil.SizeOfError _ ->
           assert(Cil.isFunctionType typ);
           Codex_config.function_size()
       in
       let exptrie = exp_to_term (ki,exp) in

       (* We collect the strings in a set, because 1. different
          versions of Ocaml print the string in different orders, and
          2. to avoid seeing twice the same output. *)
       let module StringSet = Set.Make(String) in

       let stringset = Compilation_to_term.Callstack_Trie.fold exptrie (fun term acc ->
            let bin = Analysis.eval_binary ctx term in
            let string = Format.asprintf  "%a" 
                           (Abstract_Domain.binary_pretty ~size (Analysis.domain_context ctx)) bin in
            StringSet.add string acc) StringSet.empty in

       (* Now print the strings; manually add the indentation when we detect newlines. *)
       let count = ref 0 in
       stringset |> StringSet.iter (fun string ->
           (String.split_on_char '\n' string) |> List.iter (fun line ->
               if !count > 0 then begin
                   Printf.fprintf fmt "\n";
                   for _i = 1 to indent do Printf.fprintf fmt " " done;
                 end;
               Printf.fprintf fmt "%s%!" line;
               incr count))

     with Not_found -> Printf.fprintf fmt "<unknown>")
  ;;
  
  (* let print_value_of_exp_compared_to_value exp_to_term ctx fmt (ki,exp) = *)
  (*   let value_state = Db.Value.get_state ki in *)
  (*   let value_cvalue = !Db.Value.eval_expr ~with_alarms:CilE.warn_none_mode value_state exp in *)
  (*   try *)
  (*     let exptrie = exp_to_term (ki,exp) in *)
  (*     let recover_basis term = *)
  (*       (assert false) *)
  (*       (\* Analysis.Query.convert_to_cvalue *\) @@ Analysis.Query.binary ctx term in *)
  (*     let cvalue = Compilation_to_term.Callstack_Trie.fold exptrie *)
  (*         (fun term acc -> Cvalue.V.join acc @@ recover_basis term) Cvalue.V.bottom in *)
  (*     (\* let value_cvalue = assert false in *\) *)
  (*     if Cvalue.V.equal cvalue value_cvalue then Cvalue.V.pretty fmt cvalue *)
  (*     else if Cvalue.V.is_included value_cvalue cvalue *)
  (*     then Format.fprintf fmt "value:%a codex:%a (value better)" *)
  (*         Cvalue.V.pretty value_cvalue Cvalue.V.pretty cvalue *)
  (*     else if Cvalue.V.is_included cvalue value_cvalue *)
  (*     then Format.fprintf fmt "value:%a codex:%a (codex better)" *)
  (*         Cvalue.V.pretty value_cvalue Cvalue.V.pretty cvalue *)
  (*     else Format.fprintf fmt "value:%a codex:%a (incomparable)" *)
  (*         Cvalue.V.pretty value_cvalue Cvalue.V.pretty cvalue *)
  (*   with Not_found -> *)
  (*     if Cvalue.V.is_bottom value_cvalue then *)
  (*       Format.fprintf fmt "value:bottom codex:<unknown>" *)
  (*     else *)
  (*       Format.fprintf fmt "value:%a codex:<unknown> (value better)" Cvalue.V.pretty value_cvalue *)
  (* ;; *)
  
  let html_dump exp_to_term ctx =
    match Codex_options.HtmlDump.get() with
    | "" -> ()
    | file -> 
       let print_expinfo fmt (ki,exp) =
         let string_exp = Format.asprintf "%a" Cil_datatype.Exp.pretty exp in
         let type_exp = Format.asprintf "%a" Cil_datatype.Typ.pretty (Cil.typeOf exp) in         
         Printf.fprintf fmt "Expr:%s\nType %s\n Value%a"
           string_exp type_exp (print_value_of_exp exp_to_term ctx) (0,ki,exp)
      in
      let print_lvalinfo fmt (ki,exp) =
        let lval_exp = Format.asprintf "%a" Cil_datatype.Lval.pretty exp in
        let type_exp = Format.asprintf "%a" Cil_datatype.Typ.pretty (Cil.typeOfLval exp) in         
        Printf.fprintf fmt "Lval:%s\nType %s" lval_exp type_exp
      in
      Printhtml.print file print_expinfo print_lvalinfo;
      ()
  ;;

  let exp_dump pp_exp_value outc =
    let module Local = struct

      (* The format of locations used by GCC and Clang. *)
      let pp_location outc (begin_,end_) =
        Printf.fprintf outc "%s:%d:%d"
          (Filepath.Normalized.to_pretty_string begin_.Filepath.pos_path)
          begin_.Filepath.pos_lnum
          (begin_.Filepath.pos_cnum - begin_.Filepath.pos_bol)
      ;;

      class my_visitor pp_exp_value fmt =
        object(self)
          inherit Visitor.frama_c_inplace

          val print_exp = fun ki -> fun outc exp ->
            let posa = pos_out outc in
            Printf.fprintf outc "%a: `%s' -> "
              pp_location exp.Cil_types.eloc
              (Format.asprintf "%a@?" Printer.pp_exp exp);
            let posb = pos_out outc in
            let len = posb - posa in

            (* The normal way to avoid line reaks should be to use
               pretty format with a large margin, but for some reason
               this does not work. However, this is a good
               work-around. *)

            let fmt = Format.formatter_of_out_channel outc in

            let open Format in
            let funs = Format.pp_get_formatter_out_functions fmt () in
            Format.pp_set_formatter_out_functions fmt
              {funs with 
               out_newline = (fun () ->
                   funs.out_string "\n" 0 1; funs.out_string (String.make len ' ') 0 len)
              };
            Printf.fprintf outc "%a\n%!" pp_exp_value (len,ki,exp);

          method! vexpr exp =
            let open Cil_types in
            let ki = match self#current_stmt with None -> Kglobal | Some s -> Kstmt s in
            if not (Cil.isConstant exp ||
                    match exp.enode with (* Do not print adresses of stack variables either. *)
                    | AddrOf (Var vi, off) | StartOf (Var vi, off)
                      when Cil.isConstantOffset off -> true
                    | _ -> false
                   ) then
              (print_exp ki fmt exp;
               Cil.DoChildren)
            else
              Cil.SkipChildren
        end
    end in

    (* We use Frama-C file so that locations are ordered. *)    
    (* Callgraph.Uses.iter_in_rev_order (fun kf -> *)
    (*     ignore(Visitor.visitFramacKf (new my_visitor pp_exp_value) kf) *)
    (* Format.fprintf fmt "-*- mode:compilation -*-\n@[<v>"; *)
    Visitor.visitFramacFile (new Local.my_visitor pp_exp_value outc) (Ast.get());
    (* Format.fprintf fmt "@]" *)
  ;;

  (* let compare_to_value exp_to_term ctx = *)
  (*   let pp_exp_value = print_value_of_exp_compared_to_value exp_to_term ctx in *)
  (*   match Codex_options.CompareWithValue.get() with *)
  (*   | "" -> () *)
  (*   | file -> *)
  (*     let outc = open_out file in *)
  (*     let fmt = Format.formatter_of_out_channel outc in *)
  (*     exp_dump pp_exp_value fmt; *)
  (*     Format.pp_print_flush fmt (); *)
  (*     close_out outc *)
  (* ;; *)

  
  let exp_dump pp_exp_value  =
    match Codex_options.ExpDump.get() with
    | "" -> ()
    | file ->
      let outc = open_out file in
      exp_dump pp_exp_value outc;
      close_out outc
  ;;

  exception Return of Domain_sig.sat

  let check_reachability ctx reachable_table =
    let pretty_reachable = function
      | Domain_sig.Unsat -> "TRUE"
      | Domain_sig.Unknown -> "UNKNOWN"
      | Domain_sig.Sat _ -> "FALSE"
    in

    (* WE check if the Verifier_error() function is reachable, in every callstack. *)
    if not @@ Compilation_to_term.Reachable_Table.is_empty reachable_table then begin
      let end_reachable = 
        try
          Compilation_to_term.Reachable_Table.iter reachable_table (fun t ->
              Kernel.feedback "Term to prove reachable: %a" Term.pretty t;
              let res = Analysis.Query.reachable ctx t in
              let abstr_interp_found_reachable = 
                (match res with
                 | Lattices.Quadrivalent.Bottom | Lattices.Quadrivalent.False -> true
                 | _ -> false) in
              if (Codex_config.try_hard_double_check()) || not abstr_interp_found_reachable
              then
                (* let term = Analysis.retrieve_reachable_memory_term To_SMT_Domain.unique_id ctx t in *)
                (* (\* let res2 = Smtbackend.Term_to_smt.translate_and_resolve term in *\) *)
                (* let res = Smtbackend.Term_to_muz.translate_and_resolve term in *)
                (* assert(let open Domain_sig in match res,res2 with Valid,Valid -> true | Counter_example _, Counter_example _ -> true | _ -> false); *)
                (* let res = Smtbackend.Term_to_smt.translate_and_resolve term in *)
                let res = Abstract_Domain.reachable
                    (Analysis.domain_context ctx) (Analysis.eval_memory ctx t) in
                Codex_options.feedback "SVComp Unreachability (SMT): %s" @@ pretty_reachable res;
                match res with
                | Domain_sig.Unsat -> ()
                | _ -> raise (Return res)
            );
          Domain_sig.Unsat
        with Return x -> x
      in
      Codex_options.feedback "SVComp unreachability: %s" @@ pretty_reachable end_reachable;
    end

  ;;

  (* Who proved the status. Maybe later we will refine with a better
     explanation of the tool that proved the goal. *)
  type stage = Absint | Goal_oriented
  
  let check_assertions filter_category do_try_hard emit_status ctx assertions =
    let count_total_assertions = ref 0 in
    let count_proved_by_absint = ref 0 in
    let count_by_goal_oriented = ref 0 in
  
    Compilation_to_term.Assertion_Table.iter assertions (fun t k ->
        match filter_category k with
        | None -> ()
        | Some x -> begin
            incr count_total_assertions;
            let _y = Analysis.eval_boolean ctx t in
            (* Kernel.feedback "assertion: term = %a domain %a " Term.pretty t (Abstract_Domain.boolean_pretty (Analysis.domain_context ctx)) y; *)

            let result = Analysis.Query.boolean ctx t in
            let status = match result with
              (* TODO: Emit "unreachable for bottom." *)
              | Lattices.Quadrivalent.False ->
                incr count_proved_by_absint; Property_status.False_if_reachable
              | Lattices.Quadrivalent.Top -> Property_status.Dont_know
              | Lattices.Quadrivalent.True | Lattices.Quadrivalent.Bottom ->
                incr count_proved_by_absint; Property_status.True
            in
            emit_status Absint x status;
            let should_prove = do_try_hard
                               && (result != Lattices.Quadrivalent.True)
                               && (result != Lattices.Quadrivalent.False)
            in
            if should_prove || (Codex_config.try_hard_double_check()) then begin
              let v = Abstract_Domain.satisfiable (Analysis.domain_context ctx) (Analysis.eval_boolean ctx @@ Term.SmartCons.Boolean.not t) in
              let status = 
                match v with
                | Domain_sig.Unsat ->
                  if should_prove then incr count_by_goal_oriented;
                  Property_status.True
                | Domain_sig.Unknown -> Property_status.Dont_know
                | Domain_sig.Sat _ ->
                  if should_prove then incr count_by_goal_oriented;
                  Property_status.False_and_reachable
              in
              Codex_options.feedback "After goal-orientation: status is %a" Property_status.Emitted_status.pretty status;
              emit_status Goal_oriented x status
            end
          end);
    (!count_total_assertions, !count_proved_by_absint, !count_by_goal_oriented)
  ;;

  let show_stage = function
    | Absint -> "abstract interpretation"
    | Goal_oriented -> "goal-oriented procedure";;

  let run exp_to_term term assertions reachable_table = 
    let ctx = Analysis.root_context_upcast (Analysis.root_context()) in
    (match term with
    | None -> ()
    | Some term -> Analysis.eval ctx term;
      if Codex_options.PrintValue.get() then
        Codex_options.feedback "At the end of main function: %a"
          (Abstract_Domain.memory_pretty (Analysis.domain_context ctx))
          (Analysis.eval_memory ctx term));

    (* Check SVComp Verifier_error properties. *)
    check_reachability ctx reachable_table;

    (* Check SVComp Verifier_assert properties; later will be used for ACSL assertions. *)
    ignore @@ check_assertions
      (function Compilation_to_term.Assertion_Table.SVComp loc-> Some loc | _ -> None)
      (Codex_config.try_hard_on_assertions())
      (fun stage loc status ->
         Codex_options.feedback "%a:svcomp: result is %a (using %s)"
           Cil_datatype.Location.pretty loc 
           Property_status.Emitted_status.pretty status
           (show_stage stage))
      ctx assertions;

    (* Check for runtime errors. *)
    let count_total_alarms,count_good_alarms_ai,count_good_alarms_smt = check_assertions
      (function Compilation_to_term.Assertion_Table.Alarm(al,loc) -> Some(al,loc) | _ -> None)
      (Codex_config.try_hard_on_alarms())
      (fun stage (al,loc) status ->
         if status != Property_status.True
         then Codex_options.feedback "%a:alarm %a: result is %a (using %s)"
             Cil_datatype.Location.pretty loc 
             Alarms.pretty al
             Property_status.Emitted_status.pretty status
             (show_stage stage);
         ignore @@ Alarms.register Codex_options.emitter Cil_types.Kglobal ~status ~loc al)
      ctx assertions in

    let count_good_alarms = count_good_alarms_ai + count_good_alarms_smt in
    Codex_options.feedback "Proved %d/%d alarms (remaining: %d; by SMT: %d)" count_good_alarms count_total_alarms (count_total_alarms - count_good_alarms) count_good_alarms_smt;
    let pp_exp_value = print_value_of_exp exp_to_term ctx in
    html_dump exp_to_term ctx;
    (* compare_to_value exp_to_term ctx;     *)
    exp_dump pp_exp_value
  ;;


  

end

let init_codex_library () =
  Codex_log.register (module FramaC_Log);
  Codex_config.set_max_valid_absolute_address @@
  Z.of_string @@ Integer.to_string @@ Base.max_valid_absolute_address ();
  Codex_config.set_ptr_size @@ Cil.bitsSizeOf Cil.voidPtrType;
;;


(* Note: depends on the values of initialization, which may be muted,
   so we delay its creation. But putting all this code in a top-level
   functor improves performance (probably because of better
   inlining). *)
module Main_Domain() = struct

  module Constraints =
    Constraints.Constraints.MakeConstraints(Constraints.Condition.ConditionCudd)


  module NumericBasis = Basis.Ival;;
  (* module Constraint_Domain' = Constraint_domain2_domains.Make(Constraints)(Basis.Ival)
   * module Constraint_Domain'' = Constraint_Domain'.Domain *)
  module NonRel_Domain = Domains_constraints_nonrelational.Make(Constraints)(NumericBasis)
  (* module Constraint_Domain = Constraint_Domain'''.Base_Domain *)

  module Propag_Domain = Domains_constraints_constraint_propagation.Make(Constraints)(NumericBasis)

  (* module Apron_Domain = Domains_constraints_apron.Make(Constraints) *)
  
  (* module Prod = Domains_constraints_product.Make(Constraints)(Propag_Domain)(NonRel_Domain) *)
  (* module Prod_Domain = Domains_constraints_product.Make(Constraints)(Propag_Domain)(Apron_Domain) *)      
  
  (* let module IntegerD = Constraint_domain.Make_Integer(NumericBasis) *)
  (* module IntegerD = Constraint_domain2.Make(Constraints)(NonRel_Domain) *)
  module IntegerD = Constraint_domain2.Make(Constraints)(Propag_Domain)
  (* module IntegerD = Constraint_domain2.Make(Constraints)(Apron_Domain) *)      
  (* module IntegerD = Constraint_domain2.Make(Constraints)(Prod_Domain) *)
  module Param =
    Lift_integer_domain_to_binary_domain.Make(IntegerD)
  module BinaryD2 = Constraint_domain2.Make(Constraints)(Domains_constraints_constraint_propagation.Make(Constraints)(NumericBasis))
 
  (* module Param = Constraint_domain.Make_Binary(NumericBasis) *)
  (* module Param = Constraint_domain.Make_Binary(Binary_to_integer_basis.Lift(NumericBasis)) *)

  module Scalar = BinaryD2
  module Region_numeric = Region_numeric_offset.Make(Scalar)
  module Region_suffix_tree = Region_suffix_tree.Make(Scalar)

  module Region_Operable_Value = Region_suffix_tree.Operable_Value
  (* module Region_Operable_Value = Region_numeric.Operable_Value *)
                                       
  module M = Region_separation.Make(Region_Operable_Value)
  module Value = M.Operable_Value


  module Region_numeric_Memory = Region_numeric.Memory(Region_Operable_Value)(Value)
  module Region_suffix_tree_memory = Region_suffix_tree.Memory(Value)()

  (* module Memory = M.Memory(Region_numeric_Memory) *)
  module Memory = M.Memory(Region_suffix_tree_memory)


  module Analyzed_Domain = struct
    include With_Bottom.Make(Memory_domain.Make(Scalar)(Value)(Memory))
      (* module Convert_Ref_Addr = Ref_addr_non_modular.Convert *)
  end

  
end

let run () =
  init_codex_library();

  if Codex_options.Enabled.get()
  then
    let kf = Globals.Functions.find_by_name (Kernel.MainFunction.get()) in
    let (term,assertions,reachable_table,exp_to_term) = Compilation_to_term.compile kf in
    Codex_options.feedback "compilation done";
    Gc.full_major();
    (if Codex_options.Print.get() then
      match term with
      | None ->
        Codex_options.feedback "Term at the end of %s function: bottom" (Kernel.MainFunction.get())
      | Some term ->
        Codex_options.feedback "Term at the end of %s function: %a"
          (Kernel.MainFunction.get()) Term.pretty term);

    let module Tmp = Main_Domain() in
    (* let module Tmp = Domain_for_c.Make() in *)
    let module Analyzed_Domain = Tmp.Analyzed_Domain in
    (* let module Analyzed_Domain2 = (val Codex.Domain_for_c.v:Domain_sig.Base) in *)
    (* let module Analyzed_Domain = Codex.Domain_for_c.Analyzed_Domain in *)
    
    (* let module To_SMT_Domain = A8.Binary_With_Term.TermD in *)
    (* let module To_SMT_Domain = A8.Binary_With_Term.TermD in     *)
    
    (* Instead of taking to parameters, To_SMT_Domain should be a
       parameter to a functor inside Run_For(Analyzed_Domain). *)
    let module Instance = Run_For(Analyzed_Domain) in
    Instance.run exp_to_term term assertions reachable_table;

    Pervasives.flush_all()
    
;;


Db.Main.extend run;; 
