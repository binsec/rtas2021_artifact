open Codex
open Ctypes

let rec contiki_list =
  { descr = Structure
    { st_byte_size = Some 4;
      st_name = Some "list";
      st_members =
      [ (0, "next", {descr = Ptr {pointed=contiki_list;index=Zero}; const=false; pred=Pred.True});
      ]
    };
    const = false; pred = Pred.True;
  }

module M = Datatype_sig.StringMap

let types =
  let t = M.empty in
  M.add "list" contiki_list t
