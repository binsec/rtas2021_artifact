This describes the artifact for the article “No Crash, No Exploit: Automated
Verification of Embedded Kernels”.

This artifact should reproduce the experiments related to the following research
questions (RQs) from the article:
- RQ0 (Soundness check): Does our analyzer fail to verify APE and ARTE when the
  kernel is vulnerable or buggy?
- RQ3 (Genericity): Can our method apply on different kernels, hardware
  architectures and toolchains?
- RQ4 (Automation): Is it possible to prove APE and ARTE in OS kernels fully
  automatically?
- RQ5 (Scalability): Can our method scale to large numbers of tasks?

Material to reproduce experiments for RQ1 and RQ2 is omitted because it depends
on proprietary executables.

# Installation

## Virtual machine

1. Download the [image](https://doi.org/10.5281/zenodo.4670548).
2. Import and start the image in a virtualization software (we tested it with
   VirtualBox).

User credentials for the VM:
login: demo
password: demo
You can use `sudo` to perform privileged operations if needed.

## Using Nix

If you have Nix installed on your system, you can follow the “Usage”
instructions below directly: commands like `nix-shell` should work. Please be
aware that installing the OCaml ecosystem and compiling all the sources can take
a while (up to several dozen minutes).

# Contents of the artifact

This artifact contains:
- A copy of the source code of EducRTOS (directory `educrtos`).
- A copy of the source code of the Codex abstract interpretation library
  (directory `libase`).
- A copy of the source code of BINSEC/Codex (directory `binsec`). Binsec is a
  general-purpose binary analysis tool. Our verification module is in
  `binsec/src/codex`.
- Scripts and other auxiliary files (directories `nix` and `scripts`).

# Usage

## Common to all experiments

IMPORTANT NOTICE: to launch any experiment, you need to navigate to the artifact
root directory (in the VM, `cd /home/demo/Desktop/rtas21_artifact`) and enter
the command `nix-shell`. This will yield a shell where our tool is available.
The tool is not installed outside of the shell yielded by `nix-shell`.

Each of the experiments (see below) implies to build various versions of EducRTOS
and attempt to verify them.  Every time it happens, the artifact will print a
line similar to:

```
/home/demo/Desktop/rtas21_artifact/scripts/verif_for.sh 10 param rr clang nodyn noprint -Os
```

You may execute this command yourself to re-launch the build-and-verify
procedure and inspect the analysis log in `/tmp/log`. The log contains the
emitted alarms. Note that due to the method we use (detailed in the paper),
alarms emitted during stage "Init 0" have no consequences for the verification
of the kernel, and thus are not reported outside of the logs.

The latest built version of EducRTOS is at `educrtos/system_${n}tasks.exe`,
where `${n}` is the number of user tasks included in the image. There is also an
objdump at `educrtos/system_${n}tasks.objdump`. The inferred control flow graphs
of the kernel are in the `rtas21_artifact/cfg` directory.

## RQ0: Soundness check

Inside `nix-shell`, execute `make rq0` from the artifact root. Seven versions of
EducRTOS with purposefully injected bugs will be compiled and analyzed. The
seven verifications should fail.

## RQ3: Genericity

Inside `nix-shell`, execute `make rq3` from the artifact root. This experiment
will build and attempt to verify 96 versions of EducRTOS with varying
parameters, as described in the paper. With our hardware setup, this takes about
90 minutes. All versions should pass the verifications without alarms.

You can run `make rq3_noprint` to exclude the 48 EducRTOS versions with debug
printing enabled, which are slower to verify. With our hardware setup, this
takes about 10 minutes.

## RQ4 and RQ5: Automation and Scalability

Inside `nix-shell`, you can run `make rq4and5_n`, with `n` replaced by a
specific number of tasks.  The artifact will build and verify an kernel image
bundled with n tasks, first in parameterized mode, then in in-context mode.

Only a limited set of values of n is supported, namely 1, 2, 5, 10, 50, 100,
500, 1000, 2000, 5000, 10000 and 100000. New values of n can be supported by
adding new `TASKS_${n}tasks` and `DEPS_${n}tasks` variables in
`educrtos/Makefile`.

Please be aware that for big task counts, memory consumption becomes very high
(this is an implementation issue that limits our scalability as the system swaps
for large number of tasks), as shown by Figure 8 in the paper.  For task counts
greater than 1000, only the parameterized verification will be performed.

Regarding the in-context verification: you may notice that it succeeds for 1, 2
and 5 tasks, but an alarm is emitted with greater task counts. This false alarm
appears because the in-context verification fails to verify a complex invariant
on x86 segment descriptors using only numeric information. Parameterized
verification, on the contrary, succeeds independently of the number of tasks.
This supports our statement that "Fully-automated in-context verification with no
annotation is achievable on very simple kernels, but is not robust enough for
more complex kernels."

# Modifying EducRTOS

In order to modify parts of EducRTOS, you need to navigate to the `educrtos`
directory, and run:
```
nix-shell --arg clang true
```
to compile with clang, and
```
nix-shell --arg clang false
```
to compile with GCC. Then you should run `make system_${n}tasks.exe` to build,
where `${n}` is the number of tasks to include in the executable (with the same
restrictions as in RQ4&5). It will build a version of EducRTOS with
earliest-deadline-first scheduling, dynamic task creation and printing
activated, using the `-O1` flag.

You can also test the OS in QEMU by running `make system_${n}tasks.qemu`.

Finally, you can attempt to verify the modified OS by running e.g.:
```
$ARTIFACT_ROOT/scripts/verif_for.sh 10 param rr gcc nodyn noprint -O1
```

# Modifying the analyzer

If you modify the sources of the analyzer, either in `binsec` or in `libase`, in
order to recompile it, you need to exit any `nix-shell` you were in, and then
run `nix-shell` again in the artifact root directory
(in the VM, `/home/demo/Desktop/rtas21_artifact`).

The definition of the "stack" of abstract domains used in the
analysis is defined in file `binsec/src/codex/dba2Codex.ml` and can be modified.
For instance, the following change:
```diff
diff --git a/src/codex/dba2Codex.ml b/src/codex/dba2Codex.ml
index 96abfc8a1..188ba2371 100644
--- a/src/codex/dba2Codex.ml
+++ b/src/codex/dba2Codex.ml
@@ -113,7 +113,7 @@ module Create () : Sig = struct
   module Propag_domain = Domains_constraints_constraint_propagation.Make
     (Constraints)(Ival_with_sentinel_basis)
   module Scalar0 = Constraint_domain2.Make(Constraints)(Propag_domain)
-  module Scalar = Bitwise_domain.Make (Scalar0)
+  module Scalar = (* Bitwise_domain.Make *) (Scalar0)
 
   module Region_numeric = Region_numeric_offset.Make(Scalar)
   module Numeric : Codex.Memory_sig.Operable_Value_Whole
```
leaves out the `Bitwise_domain` from the analysis, which will make the analyzer
emit false alarms due to imprecisions on the accessible range of the loaded x86
segment.

# Tests

BINSEC/Codex relies on two components that are tested independently.

We rely mainly on the ELF loader and instruction decoder of BINSEC, which have
been used in a large number of publications. The IR decoding of BINSEC is tested
by comparing the results of the BINSEC interpreter and symbolic execution engine
against real executions.

The Codex abstract interpretation library is mainly tested on C code, using a
collection of unit (C) tests, and comparison with other analyzers (such as
Frama-C's Eva) on large benchmarks. These C-based tests depend on Frama-C, that
we did not include in the artifact VM (as the purpose of the artifact is to
analyze binary code).

BINSEC/Codex itself is tested for soundness using our soundness check (RQ0) and
our collection of 96 EducRTOS versions (RQ4).

# (Experimental) Analyzing standalone executables

To analyze executables that are not kernels should be simpler, because it is not
required to separately analyze the boot code and runtime code (see paper for
details about that). To analyze in "non-kernel" mode, one simply needs to omit
the `-codex-analyze-kernel` option, e.g.:
```
binsec -codex <executable file> -entrypoint <start symbol>
```

# (Experimental) Analyzing ARMv7 executables

```
binsec -isa arm32 -decoder unisim-armsec -codex -entrypoint <start symbol> <executable file>
```

Or shorter, using the file `standalone_arm.ini`:
```
binsec -config standalone_arm.ini <executable file>
```
