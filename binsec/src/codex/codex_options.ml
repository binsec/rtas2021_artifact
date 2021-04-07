include Cli.Make (struct
  let name = "codex"
  let shortname = "codex"
end)

module ApplicationFile = struct
  include Builder.String(struct
    let name = "app-file"
    let default = ""
    let doc = "Path to an application ELF"
  end)
end

module X86Types = struct
  include Builder.String (struct
    let name = "x86-types"
    let default = "rr"
    let doc = "Version of the C types for x86 OS"
  end)
end

module UseShape = struct
  include Builder.True (struct
    let name = "use-shape"
    let doc = "Set to not use shape"
  end)
end

module NbTasks = struct
  include Builder.Integer (struct
    let name = "nb-tasks"
    let doc = "Number of tasks (required)"
    let default = 0
  end)
end

module AnalyzeKernel = struct
  include Builder.False (struct
    let name = "analyze-kernel"
    let doc = "Whether to analyze a kernel (tailored, more complex analysis)"
  end)
end

module DynThreads = struct
  include Builder.True (struct
    let name = "dyn-threads"
    let doc = "Analyzed kernel features dynamic thread creation"
  end)
end
