##########################################################################
#                                                                        #
#  This file is part of the Codex plugin for Frama-C.                    #
#                                                                        #
#  Copyright (C) 2007-2017                                               #
#    CEA (Commissariat à l'énergie atomique et aux énergies              #
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

ifndef FRAMAC_SHARE
FRAMAC_SHARE  :=$(shell frama-c-config -print-share-path)
endif
ifndef FRAMAC_LIBDIR
FRAMAC_LIBDIR :=$(shell frama-c-config -print-libpath)
endif

PLUGIN_ENABLE:=yes
PLUGIN_DYNAMIC:=yes
PLUGIN_DISTRIBUTED:=$(PLUGIN_ENABLE)
PLUGIN_REQUIRES = cudd zarith #minisat

PLUGIN_NAME:= CodexPlugin
PLUGIN_CMO:= \
	codex_options region_analysis_stmt build_semantic_equations compilation_to_term \
	printhtml codex_register
PLUGIN_EXTRA_BYTE := $(PWD)/../Codex.cmo $(PWD)/../dllCodex.so
PLUGIN_EXTRA_OPT := $(PWD)/../Codex.cmxa $(PWD)/../libCodex.a

MY_WARNINGS := -w -40-27-32-42-34-8-4-9-60-8
#MY_INCLUDE_DIRS := -I ../ -I ../utils -I ../term -I ../domains -I ../basis -I ../ext -I ../domains/memory -I ../domains/constraints
MY_INCLUDE_DIRS := -I ..
PLUGIN_BFLAGS += $(MY_WARNINGS) $(MY_INCLUDE_DIRS) 
PLUGIN_OFLAGS += $(MY_WARNINGS) $(MY_INCLUDE_DIRS)
PLUGIN_TESTS_DIRS:=nonterm

$(PWD)/../Codex.cmo $(PWD)/../Codex.cmi:
	cd .. && make Codex.cmo

$(PWD)/../Codex.cmx:
	cd .. && make Codex.cmx

$(PWD)/../Codex.cma:
	cd .. && make Codex.cma

$(PWD)/../dllCodex.so:
	cd .. && make dllCodex.so

$(PWD)/../libCodex.a:
	cd .. && make libCodex.a

$(PWD)/../Codex.cmxa:
	cd .. && make Codex.cmxa

# Recompile the plugin if the main library has changed.
$(addsuffix .cmx,$(PLUGIN_CMO)): $(PWD)/../Codex.cmx
$(addsuffix .cmo,$(PLUGIN_CMO)): $(PWD)/../Codex.cmo
$(addsuffix .cmi,$(PLUGIN_CMO)): $(PWD)/../Codex.cmi

# .PHONY: ../Codex.cmx ../Codex.cmo

#Codex.cmo: ../Codex.cmo
#Codex.cmxs: ../Codex.cmxs

PLUGIN_DISTRIB_EXTERNAL:= # Makefile.in configure.ac configure test.c test.oracle README.org

include $(FRAMAC_SHARE)/Makefile.dynamic

#####################################
# Regenerating the Makefile on need #
#####################################

ifeq ("$(FRAMAC_INTERNAL)","yes")
CONFIG_STATUS_DIR=$(FRAMAC_SRC)
else
CONFIG_STATUS_DIR=.
endif

test: byte
	cd $(PWD) && frama-c.byte -kernel-verbose 5 -load-module ./top/CodexPlugin -codex ../test.c

test.opt: opt
	cd $(PWD) && frama-c -load-module ./top/CodexPlugin -codex ../test.c
