#CC ?= gcc
CC ?= clang

# Scheduler.
SCHEDULER ?= EDF_SCHEDULING
# SCHEDULER=FP_SCHEDULING
# SCHEDULER=ROUND_ROBIN_SCHEDULING

OPT_FLAGS ?= -O1

SINGLEFILE := no               # normal compilation
# SINGLEFILE := yes              # concatenante every kernel file
# SINGLEFILE := preprocessed    # ... and preprocess the result
SINGLEFILE := $(strip $(SINGLEFILE))

DYNAMIC_TASK_CREATION ?= yes

DEBUG_PRINTING ?= yes

INJECT_BUG ?= no
