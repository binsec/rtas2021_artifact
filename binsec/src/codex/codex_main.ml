let () =
  Codex.Codex_config.set_assume_simple_asts false;
  Cli.Boot.enlist ~name:"codex analysis" ~f:Analyze.run_codex
