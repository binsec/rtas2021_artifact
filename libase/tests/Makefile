##########################################################################
#                                                                        #
#  This file is part of the Codex plug-in of Frama-C.                    #
#                                                                        #
#  Copyright (C) 2007-2015                                               #
#    CEA (Commissariat a l'energie atomique et aux energies              #
#         alternatives)                                                  #
#                                                                        #
#  you can redistribute it and/or modify it under the terms of the GNU   #
#  Lesser General Public License as published by the Free Software       #
#  Foundation, version 2.1.                                              #
#                                                                        #
#  It is distributed in the hope that it will be useful,                 #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#  GNU Lesser General Public License for more details.                   #
#                                                                        #
#  See the GNU Lesser General Public License version 2.1                 #
#  for more details (enclosed in the file licenses/LGPLv2.1).            #
#                                                                        #
##########################################################################

c-files = $(wildcard *.c big/*.c horn/*.c) # verasco1.3-test/*.c)
#c-files = $(wildcard horn/*.c)
results = $(c-files:%.c=%.result)
diffs = $(c-files:%.c=%.diff)
frama-c = $(shell which frama-c)

#byte = .byte

codex = $(if $(byte),../frama-c/top/CodexPlugin.cmo,../frama-c/top/CodexPlugin.cmxs)

codex-options := -codex
# codex-options := $(codex-options) -codex-print-force-lazyness
#codex-options := $(codex-options) -codex-domains 0,1,2,3,4
codex-options := $(codex-options) -codex-domains 6

# Remove temporary debug messages inserted with Kernel.feedback
codex-options := -kernel-verbose 0 $(codex-options)

%.sav: %.c $(frama-c)
	-frama-c$(byte) $< -save $@

%.diff-with-value-precise: %.sav $(frama-c) $(codex)
	frama-c$(byte) -load $< -val -no-warn-signed-overflow -no-warn-unsigned-overflow \
	-no-undefined-pointer-comparison-propagate-all -slevel 1000000 \
	-then -load-module ../frama-c/top/CodexPlugin $(codex-options) \
	-codex-compare-with-value $*.diff-with-value-precise


# Display result and exit on first error.
# TODO: Also compare with value with huge slevels, which provide the most precise correct results
# on tests that can be completely unrolled.
%.result %.exp_dump: %.c $(frama-c) $(codex)
#	@echo "frama-c$(byte) -load-module ../Codex -load $< $(codex-options)"
	rm -f $*.exp_dump $*.result
	timeout 20s frama-c$(byte) -load-module ../frama-c/top/CodexPlugin $< $(codex-options) \
	-codex-no-print -codex-no-print-value -codex-exp-dump $*.exp_dump > $*.result \
	|| (cat $*.result; exit 1)

# Use Value exp-dump. The options are those that correspond most to Codex defaults.
#%.result %.exp-dump: %.sav $(frama-c) 
#	@echo "frama-c$(byte) -load-module ../Codex -load $< $(codex-options)"
#	frama-c -load $< -val -no-warn-signed-overflow -no-warn-unsigned-overflow \
#	-no-undefined-pointer-comparison-propagate-all \
#	-val-exp-dump $*.exp_dump


# -val -val-exp-dump $*.exp_dump 
%.diff: %.result
	@test -s $*.oracle || (echo "$*.oracle does not exists. Output is:"; cat $*.result)
	@echo "diff -u $*.oracle $*.result"
	-@diff -u $*.oracle $*.result > $*.diff || (cat $*.diff; exit 2)

all: $(results)

# TODO: Only have the .result; we don't care about the oracle (we have version control for that)
#all: $(diffs)
#	for i in $(basename $(wildcard *.result big/*.result)); do mv $$i.result $$i.oracle; done

#update:
#	for i in $(basename $(wildcard *.result big/*.result)); do mv $$i.result $$i.oracle; done

clean:
	rm -f $(results) $(diffs)

.PRECIOUS: $(results) $(diffs)
