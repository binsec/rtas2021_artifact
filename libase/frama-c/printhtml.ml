(**************************************************************************)
(*                                                                        *)
(*  This file is part of the Codex plugin of Frama-C.                     *)
(*                                                                        *)
(*  Copyright (C) 2007-2016                                               *)
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

(* As the printer reconstructs some expressions (e.g. ++), the
   displayed expressions may not match the true expressions of the
   program (there are expressions that are created, and the nest level
   do not mach). Moreover the eid are not always reliable.

   So, we first hook the printer to construct a reliable hash of all
   the printed expressions of the program (and their level of nest);
   we fill this hash afterwards. *)

(* Note that comparison is done by ids; as ids follow the level of
   nest, this allows expinfos to be displayed in the correct order
   (which is why we use a map here) *)
type exp_or_lval =
  | Exp of Cil_types.kinstr * Cil_types.exp
  | Lval of Cil_types.kinstr * Cil_types.lval


module ExpOrLvalWithId = struct

  module Type = struct
    type t = (exp_or_lval * int)
    let compare (_,a) (_,b) = Datatype.Int.compare a b
    let hash (_,a) = Datatype.Int.hash a
    let equal (_,a) (_,b) = Datatype.Int.equal a b
    let pretty (a,_) = assert false
  end

  module Map = Map.Make(Type)

end

module PrintHtmlCode = struct


  (* These references are really ugly; however this module should only
     be used through pp_file below, which makes sure that they are
     correctly used. *)
  let exp_unique_id = ref 0;;
  let current_level = ref 0;;
  let cur_stmt = ref Cil.invalidStmt;;
  
  let printed_exp_hash = ref (Obj.magic 0)

  let add_exp exp id nesting_level =
    let expmap =
      try Cil_datatype.Stmt.Hashtbl.find !printed_exp_hash !cur_stmt
      with Not_found -> ExpOrLvalWithId.Map.empty
    in
    let expmap = ExpOrLvalWithId.Map.add (exp,id) nesting_level expmap in
    Cil_datatype.Stmt.Hashtbl.replace !printed_exp_hash !cur_stmt expmap;
  ;;

 let add_lval exp id nesting_level =
    let expmap =
      try Cil_datatype.Stmt.Hashtbl.find !printed_exp_hash !cur_stmt
      with Not_found -> ExpOrLvalWithId.Map.empty
    in
    let expmap = ExpOrLvalWithId.Map.add (exp,id) nesting_level expmap in
    Cil_datatype.Stmt.Hashtbl.replace !printed_exp_hash !cur_stmt expmap;
  ;;

  
  
  (* HTML code printer; escape problematic character codes <,>,and &;
     insert HTML tags invisible to the pretty printer. *)
  module PrinterClassHtml (X: Printer.PrinterClass) = struct

    open Cil_types;;

    class printer : Printer.extensible_printer = object(self)
      inherit X.printer as super
      
      (* Note: We use pp_print_as so that the markup text is not seen by
         the pretty printer. *)

      method! exp fmt exp =
        let kinstr = match self#current_stmt with None -> Kglobal | Some s -> Kstmt s in
        Format.pp_print_as fmt 0 
          (Format.sprintf "<span data-eid=\"%d\" class=\"expnest expnest%d\">"
             !exp_unique_id !current_level);
        add_exp (Exp (kinstr,exp)) !exp_unique_id !current_level;
        incr exp_unique_id;
        incr current_level;
        (match exp.enode with
         | AddrOf lv | StartOf lv -> Format.pp_print_as fmt 1 "&amp;"; self#lval fmt lv
         | _ ->      super#exp fmt exp);
        decr current_level;
        Format.pp_print_as fmt 0 "</span>";
        
        (* MAYBE: do not increment level for Lval nodes, as this is already done by lval. *)

      method! lval fmt lval =
        let kinstr = match self#current_stmt with None -> Kglobal | Some s -> Kstmt s in
        Format.pp_print_as fmt 0 (Printf.sprintf "<span data-eid=\"%d\" class=\"expnest expnest%d\">" !exp_unique_id !current_level);
        add_exp (Lval(kinstr,lval)) !exp_unique_id !current_level;
        incr exp_unique_id;        
        incr current_level;
        super#lval fmt lval;
        decr current_level;
        Format.pp_print_as fmt 0 "</span>"

      (* Note: stmt does not work, we have to use annotated_stmt instead. *)
      method! annotated_stmt x fmt stmt =
        cur_stmt := stmt;
        super#annotated_stmt x fmt stmt
      
      method! instr fmt instr =
        Format.pp_print_as fmt 0 "<span class=\"statement\">";
        super#instr fmt instr;
        Format.pp_print_as fmt 0 "</span>";

      method! binop fmt = function
        | Shiftlt -> Format.pp_print_as fmt 2 "&lt;&lt;"
        | Shiftrt -> Format.pp_print_as fmt 2 "&gt;&gt;"
        | Lt -> Format.pp_print_as fmt 1 "&lt;"                    
        | Gt -> Format.pp_print_as fmt 1 "&gt;"                         
        | Le -> Format.pp_print_as fmt 2 "&lt;="                    
        | Ge -> Format.pp_print_as fmt 2 "&gt;="
        | LAnd -> Format.pp_print_as fmt 2 "&amp;&amp;"
        | x -> super#binop fmt x

      (* Just add a link to jump to. *)
      method! varinfo fmt vi =
        if vi.vglob then begin
          Format.pp_print_as fmt 0 (Printf.sprintf "<a href=\"#vi-global-%s\">" vi.vname);
          super#varinfo fmt vi;
          Format.pp_print_as fmt 0 "</a>"
        end
        else
          super#varinfo fmt vi

      method! vdecl fmt vi =
        if vi.vglob then begin
          Format.pp_print_as fmt 0 (Printf.sprintf "<a name=\"vi-global-%s\"></a>" vi.vname);
          super#vdecl fmt vi
        end
        else super#vdecl fmt vi
      
    end
  end

  let pp_file fmt file =
    
    printed_exp_hash := Cil_datatype.Stmt.Hashtbl.create 117;
    
    let printer = Printer.current_printer() in
    let () = Printer.update_printer
        (module PrinterClassHtml: Printer.PrinterExtension) in
    Printer.pp_file fmt (Ast.get());
    Printer.set_printer printer;
    !printed_exp_hash
    
end

(* https://material.google.com/style/color.html#color-color-palette *)
(* The jquery code is used only to display the corresponding eidinfo node. *)
let prefixe = 
  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\
<html lang=\"en\">\
  <head>\
    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script>\
    <script type='text/javascript'>\
      $(document).ready(function(){\
        $(\".expnest\").hover(function(){\
           $(\"#eidinfo\" + $(this).attr(\"data-eid\")).toggle();\
         })\
      });\
    </script>\
    <style>\
      a { text-decoration: none; }\
      .def { display: none; }\
//      .expnest:hover + .expinfo > .def { display: inline-block; }\
//      .statement:hover + .expinfo > .def { display: inline-block; }\
      .expinfo {\
         position:fixed;\
         top: 2em;\
         right: 2em;\
         width: 40em;\
         height: 60em;\
         border: 1px solid black;\
         border-radius: 12px;\
         padding: 10px;\
       }\
      .expinfo > .eidinfo { display: none; white-space: pre; }\
\
      .statement:hover { background-color:#B2DFDB; }\
      .expnest0:hover { background-color:#FFCDD2; }\
      .expnest1:hover { background-color:#D1C4E9; }\
      .expnest2:hover { background-color:#90CAF9; }\
      .expnest3:hover { background-color:#FA6900; }\
      .expnest4:hover { background-color:#FFD54F; }\
      .expnest5:hover { background-color:#81C784; }\
      .expnest6:hover { background-color:#BCAAA4; }\
      .expnest7:hover { background-color:#9FA8DA; }\
      .expnest8:hover { background-color:#4FC3F7; }\
      .expnest9:hover { background-color:#F38630; }\
      .expnest10:hover { background-color:#FFEE58; }\
      .expnest11:hover { background-color:#A7DBD8; }\
      .expnest12:hover { background-color:#E0E4CC; } \
      .expnest13:hover { background-color:#69D2E7; }\
\
      .statement:hover { background-color:#B2DFDB; }\
      .font-expnest0 { background-color:#FFCDD2; }\
      .font-expnest1 { background-color:#D1C4E9; }\
      .font-expnest2 { background-color:#90CAF9; }\
      .font-expnest3 { background-color:#FA6900; }\
      .font-expnest4 { background-color:#FFD54F; }\
      .font-expnest5 { background-color:#81C784; }\
      .font-expnest6 { background-color:#BCAAA4; }\
      .font-expnest7 { background-color:#9FA8DA; }\
      .font-expnest8 { background-color:#4FC3F7; }\
      .font-expnest9 { background-color:#F38630; }\
      .font-expnest10 { background-color:#FFEE58; }\
      .font-expnest11 { background-color:#A7DBD8; }\
      .font-expnest12 { background-color:#E0E4CC; } \
      .font-expnest13 { background-color:#69D2E7; }\
\
    </style>\
    <meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\
    <title>Analysis results</title>\
  </head>\
  <body>\
    <pre><code>\n";;

let suffixe1 = "\n</code></pre><div class=\"expinfo\">\n";;
let suffixe2 = "</div></body></html>";;

(* Note: si on veut  rajouter des liens, attention on ne peut pas les nester. *)
(*    <a href="tutu">nesting</a><a href="toto">not</a><a href="tutu">permitted</a> *)

let print filename print_expinfo print_lvalinfo =

  let print_exp_or_lval_info fmt = function
    | Exp (ki,e) -> print_expinfo fmt (ki,e)
    | Lval (ki,e) -> print_lvalinfo fmt (ki,e)
  in

  (* Open file and print prefixe. *)
  let outfile = open_out filename in
  let outfmt = Format.formatter_of_out_channel outfile in
  let file = Ast.get() in
  output_string outfile prefixe;
  flush outfile;

  (* Print the main file. *)
  let printed_exp_hash = PrintHtmlCode.pp_file outfmt file in
  Format.pp_print_flush outfmt ();
  output_string outfile suffixe1;
  flush outfile;

  (* Print the "informations" frame. *)
  printed_exp_hash |> Cil_datatype.Stmt.Hashtbl.iter (fun stmt expmap ->
    expmap |> ExpOrLvalWithId.Map.iter (fun (exp,id) nestlevel ->
                  Printf.fprintf outfile "<p class=\"font-expnest%d eidinfo\" id=\"eidinfo%d\">"
                    nestlevel id;
                  print_exp_or_lval_info outfile exp;
                  Printf.fprintf outfile "</p>\n"));

  (* Print suffixe. *)
  output_string outfile suffixe2;
  flush outfile;
  close_out outfile;
;;
