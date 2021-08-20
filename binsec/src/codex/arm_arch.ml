module Ctypes = Codex.Ctypes

module Make
    (Domain : Codex.With_focusing.Base_with_types)
    (EvalPred : Codex.Type_domain.EvalPred.Sig with module Domain := Domain)
= struct

  let bunknown ~size ctx =
    Domain.binary_unknown ~size ~level:(Domain.Context.level ctx) ctx

  module Registers = struct
    module Domain = Domain
    let registers = [("r0",32); ("nxt_r0",32);("r1",32); ("nxt_r1",32);
                     ("r2",32); ("nxt_r2",32);("r3",32); ("nxt_r3",32);
                     ("r4",32); ("nxt_r4",32);("r5",32); ("nxt_r5",32);
                     ("r6",32); ("nxt_r6",32);("r7",32); ("nxt_r7",32);
                     ("r8",32); ("nxt_r8",32);("r9",32); ("nxt_r9",32);
                     ("ip",32); ("nxt_ip",32);
                     ("sp",32); ("nxt_sp",32);
                     ("sp_usr",32); ("nxt_sp_usr",32);
                     ("pc",32); ("nxt_pc",32);
                     ("lr",32); ("nxt_lr",32);
                     ("lr_usr",32); ("nxt_lr_usr",32);
                     ("sl",32); ("nxt_sl",32);
                     ("fp",32); ("nxt_fp",32);
                     ("n",1); ("nxt_n",1);
                     ("z",1); ("nxt_z",1);
                     ("c",1); ("nxt_c",1);
                     ("v",1); ("nxt_v",1);
                     ("cpsr",32); ("nxt_cpsr",32);
                     ("tmp32_0",32); ("tmp32_1",32);
                     ("tmp32_2",32); ("tmp32_3",32);
                     ("tmp32_4",32);
                     ("itstate",8); ("nxt_itstate",8);
                     ("diagcr",32); ("nxt_diagcr",32);
                     ("mpidr",32); ("nxt_mpidr",32);
                     ("sctlr",32); ("nxt_sctlr",32);
                     ("tlbiall",32); ("nxt_tlbiall",32);
                     ("tlbiasid",32); ("nxt_tlbiasid",32);
                     ("ttbr0",32); ("nxt_ttbr0",32);
                     ("bpiall",32); ("nxt_bpiall",32);
                     ("iciallu",32); ("nxt_iciallu",32);
                     ("contextidr",32); ("nxt_contextidr",32);
                     ("dcisw",32); ("nxt_dcisw",32);
                     ("octlr",32); ("nxt_actlr",32);
                     ("dacr",32); ("nxt_dacr",32);
                     ("clidr",32);
                     ("cfgbar",32);
                     ("ccsidr",32);
                     ("vbar",32); ("nxt_vbar",32);
                     ("dfsr",32); ("nxt_dfsr",32);
                     ("dfar",32); ("nxt_dfar",32);
                     ("ifsr",32); ("nxt_ifsr",32);
                     ("ifar",32); ("nxt_ifar",32);
                     ("csselr",32); ("nxt_csselr",32);
                     ("bsr_in",32); ("bsr_out",32);
                     ("actlr",32);
                     ("t",1); ("nxt_t",1); (* Thumb mode flag. Should always remain 0 for us *)
                    ]

    let initial_value ctx (name,size) =
      match name with
      | "mpidr" ->
          let unknown = bunknown ~size:32 ctx in
          let v = Domain.Binary_Forward.bshl ~size:32 ~nsw:false ~nuw:false ctx unknown (Domain.Binary_Forward.biconst ~size (Z.of_int 2) ctx) in
          v
      | _ -> bunknown ctx ~size
  end
end
