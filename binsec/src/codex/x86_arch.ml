module Make
    (Domain : Codex.With_focusing.Base_with_types2)
    (EvalPred : Codex.Type_domain.EvalPred.Sig with module Domain := Domain)
= struct
  let bunknown ~size ctx =
    Domain.binary_unknown ~size ~level:(Domain.Context.level ctx) ctx

  module Registers = struct
    module Domain = Domain
    let registers = [("eax",32); ("ebx",32); ("ecx",32); ("edx",32); ("edi",32);
                     ("esp",32); ("ebp",32);
                     ("esi",32);
                     ("div",64); ("quo",64); ("rem",64);
                     (* Flags? *)
                     ("CF",1); ("PF",1); ("AF",1); ("ZF",1); ("SF",1); ("TF",1);
                     ("DF",1); ("OF",1); ("NT",1); ("RF",1); ("AC",1); ("ID",1);
                     ("IF",1); ("IOPL",2); ("VIF",1); ("VIP",1); ("VM",1); ("cpl",2);
                     (* Segment registers *)
                     ("cs",16); ("ds",16); ("ss",16); ("es",16); ("fs",16); ("gs",16);
                     (* Task register *)
                     ("tr",16);
                     (***** "Virtual" registers needed for the DBA translation *****)
                     ("res8",8); ("res16",16); ("res32",32); ("temp32",32);
                     ("temp32_0",32); ("temp32_1",32); ("temp32_2",32); ("temp32_3",32);
                     ("temp64",64);
                     ("res64",64);
                     (* GDTR (global descriptor table) base *)
                     ("gdt",32);
                     (* Segment bases (hidden registers) *)
                     ("ds_base",32); ("cs_base",32); ("ss_base",32); ("es_base",32);
                     ("fs_base",32); ("gs_base",32);
                     ("tr_base",32);
                     (* Segment descriptors (hidden registers) *)
                     ("ds_desc",64); ("cs_desc",64); ("ss_desc",64); ("es_desc",64);
                     ("fs_desc",64); ("gs_desc",64);
                     ("tr_desc",64);
                     (* IDTR (interrupt descriptor table) base *)
                     ("idt",32);
                     (* MM0 *)
                     ("mm0",64);
                    ]
    let initial_value ctx (name,size) =
      match name with
      | "esp" ->
          if Codex_options.AnalyzeKernel.get () then
            bunknown ~size ctx
          else
            (* XXX: specific to one example *)
            Domain.Binary_Forward.biconst ~size (Z.of_int 0x10000) ctx
      | "ds_base" | "cs_base" | "ss_base" | "ts_base" ->
          Domain.Binary_Forward.biconst ~size Z.zero ctx
      | "IF" | "TF" | "VM" | "RF" | "NT" ->
          Domain.Binary_Forward.biconst ~size Z.zero ctx
      | _ -> bunknown ctx ~size
  end
end
