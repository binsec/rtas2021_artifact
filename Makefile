.PHONY: all checknix rq0 rq3

TARGETS = "rq0 rq3 rq3_noprint rq4and5_<n>"
analysis_log = /tmp/log

all: checknix
	@echo "Please specify a target to run, i.e. one of: " $(TARGETS)

checknix:
	@if [ -z $(NIX_ARTIFACT_ENV) ]; then \
	  echo "Not in the suitable environment to run the artifact. Please open the required environment by typing 'nix-shell'."; \
	  exit 1; \
	fi

rq0: checknix rq0_0 rq0_1 rq0_2 rq0_3 rq0_4 rq0_5 rq0_6

rq0_0:
	@echo "Analyzing kernel with arbitrary user-controlled jump..."
	@export analysis_log=$(analysis_log) ; \
	  ./scripts/analyze_with_bug.sh 0

rq0_1:
	@echo "Analyzing kernel with code descriptor corruption..."
	@export analysis_log=$(analysis_log) ; \
	  ./scripts/analyze_with_bug.sh 1

rq0_2:
	@echo "Analyzing kernel with data segment corruption..."
	@export analysis_log=$(analysis_log) ; \
	  ./scripts/analyze_with_bug.sh 2

rq0_3:
	@echo "Analyzing kernel with arbitrary user-controlled write..."
	@export analysis_log=$(analysis_log) ; \
	  ./scripts/analyze_with_bug.sh 3

rq0_4:
	@echo "Analyzing kernel with arbitrary user-controlled read..."
	@export analysis_log=$(analysis_log) ; \
	  ./scripts/analyze_with_bug.sh 4

rq0_5:
	@echo "Analyzing kernel with illegal opcode..."
	@export analysis_log=$(analysis_log) ; \
	  ./scripts/analyze_with_bug.sh 5

rq0_6:
	@echo "Analyzing kernel with division by zero..."
	@export analysis_log=$(analysis_log) ; \
	  ./scripts/analyze_with_bug.sh 6

rq3: checknix
	@export ntasks_args=10 \
	  param_or_fixed=param \
	  opt_flags='-O1 -O2 -O3 -Os' \
	  compilers='gcc clang' \
	  scheduling_algs='rr fp edf' \
	  dyn_threads_args='nodyn dyn' \
	  printing_args='noprint print' ; \
	  ./scripts/all_verif.sh

rq3_noprint: checknix
	@export ntasks_args=10 \
	  param_or_fixed=param \
	  opt_flags='-O1 -O2 -O3 -Os' \
	  compilers='gcc clang' \
	  scheduling_algs='rr fp edf' \
	  dyn_threads_args='nodyn dyn' \
	  printing_args='noprint' ; \
	  ./scripts/all_verif.sh

rq4and5_%: checknix
	@./scripts/rq4and5.sh $*
