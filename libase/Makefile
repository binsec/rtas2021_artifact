##########################################################################
#                                                                        #
#  This file is part of Codex.                                           #
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

everything: Codex.cma Codex.cmxa libCodex.a dllCodex.so


# XXX: Je manque peut-etre des optimisations parce que j'ai pas tous les cmx dans les include path
# Essayer de voir si rajouter tous les include ptahs par defaut ameliore mes perfs

################################################################
# Common rules

.SUFFIXES: .ml .mli .mll .mly .cmo .cmi .cmx

%.cmi: %.mli
	$(OCAMLC) $(FLAGS) $(OPTML) $(OPTCMI) -c $(INCLUDES) $<

# Note that this may also builds a cmi; but they will not appear in
# the dependencies produced by ocamldep anyway. So we must not write
# them.
%.cmo: %.ml
	$(OCAMLC) $(FLAGS) $(OPTML) $(OPTCMO) -c $(INCLUDES) $<

# Note: including everythin everytime is better for optimization.
# Indeed ocamlopt must find the cmx files to optimize.
.ml.cmx:
	$(OCAMLOPT) $(FLAGS) $(OPTML) $(OPTCMX) -c $(INCLUDES) $(includes) $<

%.cmxa:
	$(OCAMLOPT) -a -o $@ $+

%.cma:
	$(OCAMLC) -a -o $@ $+


################################################################

PACKAGES = -package ocamlgraph -package zarith  -package cudd # -package apron.boxMPQ -package apron # -package elina # -package minisat # -package apron

OPTC?= # .opt
OCAMLC?=ocamlfind $(OCAMLROOT)ocamlc$(OPTC) $(PACKAGES)
OCAMLOPT?=ocamlfind $(OCAMLROOT)ocamlopt$(OPTC) $(PACKAGES)
# -inline 0 helps getting a complete backtrace.
OCAMLDEP?=$(OCAMLROOT)ocamldep
OCAMLMKLIB?=$(OCAMLROOT)ocamlmklib

# For some reason I had to comment this out to be able to compile with -j 8.
# Not anymore.
PACK := -for-pack Codex
WARNINGS := -w +a-3-4-6-9-40-41-42-44-45-48-34
WARNINGS := $(WARNINGS)-27 #-32-8-37 # Should probably be on on a clean version.
FLAGS := -annot -bin-annot -g $(WARNINGS) $(PACK) -no-alias-deps # -unboxed-types

################################################################
# Templates

define PATH_template
$(1)_CMX := $$(addsuffix .cmx, $$($(1)_ML))
$(1)_CMO := $$(addsuffix .cmo, $$($(1)_ML))
$(1)_CMI := $$(addsuffix .cmi, $$($(1)_MLI) $$($(1)_ML))
$$($(1)_CMX) $$($(1)_CMO) $$($(1)_CMI): INCLUDES:=$$($(1)_INCLUDES)
# We do not want to inheritate from the FLAGS targets that depend on these.
$$($(1)_CMX) $$($(1)_CMO) $$($(1)_CMI): FLAGS := $(FLAGS)
endef

# Sub-folders organisation:
# - Each file is prefixed by the name of the folder
# - Each folder contains a file foldername_aliases, which is opened by everyfile.
# - Outside of the folder, there is generally an ml/mli file corresponding to the folder,
#   that exports the useful things. If folder.mli exists, the .cmi file compiled can then be used,
#   and further users of the module do not need to have a -I folder flag.
# - Without folder.mli, we have still avoided name clashes are thus avoided, and we can use -I folder everywhere.

define PATH_template_module_aliases
$(1)_CMX := $$(addsuffix .cmx, $$($(1)_ML))
$(1)_CMO := $$(addsuffix .cmo, $$($(1)_ML))
$(1)_CMI := $$(addsuffix .cmi, $$($(1)_MLI) $$($(1)_ML))
$$($(1)_CMX) $$($(1)_CMO) $$($(1)_CMI): INCLUDES:=$$($(1)_INCLUDES)

# Compiles, adds as a pre-requesite, and opens the $(2)_aliases files
$$($(1)_CMX) $$($(1)_CMO) $$($(1)_CMI): FLAGS += -open $(3)_aliases
$$($(1)_CMX) $$($(1)_CMO) $$($(1)_CMI): $(2)/$(2)_aliases.cmi
$(2)/$(2)_aliases.cmo $(2)/$(2)_aliases.cmx $(2)/$(2)_aliases.cmi:
	$$(OCAMLC) $$(PACK) -bin-annot -no-alias-deps -w -49 $(2)/$(2)_aliases.ml
endef


dirs := . ext transfer_functions basis lattices domains term utils smtbackend constraints single_value_abstraction \
  types
sources := $(addsuffix /*.ml, $(dirs)) $(addsuffix /*.mli, $(dirs))
includes := $(addprefix -I , $(dirs)) -I domains/memory -I domains/constraints

# FLAGS += -I smtbackend


################################################################

# ML files, in link order.

# TODO: Create a pack with these two.
#lattices_modules := set_Lattice map_Pointwise_Lattice bitvector_Lattice \
	            flat_Lattice add_top_bottom

utils_modules := int_builtins smallmap extstdlib utils not_implemented datatype_sig \
		 bdd condition_map immutable_array immutable_dynamic_array dynamic_array \
		 mutable_radix_trie interval_map okasakimap alarm_logger

types_modules := ctypes

lattices_modules := set_Lattice prod_Lattice dummy_Lattice assert_False_Lattice

# TODO: bitvector_Lattice is really a basis.
basis_modules := basis_Assert_False_Transfer_Functions basis_noop_transfer_functions \
		quadrivalent_basis ival_basis basis_prod \
		binary_to_integer_basis dummy_basis \
		sentinel_basis reduced_prod_basis ival_with_sentinel_basis

#		  finite_memory_sig finite_memory finite_memory_symbolic_binary disjunctive_binary
domains_modules := \
		  assert_False_Transfer_Functions with_Noop \
		  with_Bottom \
		  evaluating  \
	          term_domain \
		  constraints/domains_constraints_assert_false \
		  constraints/domains_constraints_product \
		  constraints/domains_constraints_nonrelational \
		  constraints/domains_constraints_constraint_propagation \
		  constraint_domain2 prod_domain  \
		  lift_integer_domain_to_binary_domain \
		  memory/memory_sig memory/region_numeric_offset \
	          memory/region_suffix_tree memory/region_separation \
		  memory/memory_domain \
		  memory/type_domain memory/region_binary memory/with_focusing \
		  bitwise_domain
# apron_domain
#		  constraints/domains_constraints_apron \

term_modules := term_type term_utils \
		term_dag term_type_inference \
		term_pretty \
		term_hashcons term_hashtbl_stack \
		term_smart_constructors_sig \
		term_smart_constructors_no_rewrite \
		term_smart_constructors_administrative_reduction \
		term_smart_constructors_bdd_rewrite \
		term_smart_constructors
#		term_direct_interpreter_sig term_direct_interpreter

constraints_modules := constraints_condition constraints_constraints_sig \
			constraints_constraints constraints_const_eval constraints_slicing constraints_smt 

smtbackend_modules := smtlib_sig smtlib

transfer_functions_modules := builtin transfer_functions_ids transfer_functions_flags transfer_functions_sig transfer_functions_concrete transfer_functions_conversions transfer_functions

single_value_abstraction_modules := single_value_abstraction_standard single_value_abstraction

#	  $(addprefix lattices/, $(lattices_modules)) 
#  simple_memory_lattice

root_modules_1 = \
	  region_analysis_sig region_analysis \
	  quadrivalent_Lattice term_types term_constructors \
	  lattice_sig \
	  $(addprefix transfer_functions/, $(transfer_functions_modules)) \
	  $(addprefix single_value_abstraction/, $(single_value_abstraction_modules)) \
          basis_sig domain_sig \
	  term_constructors_gadt

SRC_ML := codex_log codex_config \
	  ext/framac_ival \
	  $(addprefix utils/, $(utils_modules)) \
	  $(addprefix types/, $(types_modules)) \
          $(addprefix ext/, WTO binary_heap ) \
	  $(root_modules_1) \
	  $(addprefix term/, $(term_modules)) term \
	  $(addprefix smtbackend/smtbackend_, $(smtbackend_modules)) smtbackend \
	  $(addprefix lattices/, $(lattices_modules)) lattices \
	  $(addprefix basis/, $(basis_modules)) basis \
	  $(addprefix constraints/, $(constraints_modules)) constraints \
	  $(addprefix domains/, $(domains_modules)) domains
#	  $(addprefix frama-c/, $(root_modules_3))

SRC_MLI := $(basename $(wildcard $(addsuffix /*.mli, $(dirs) domains/memory domains/constraints)))

################ Get the list of files not compiled.

given := $(addsuffix .ml, $(SRC_ML))
given += $(addsuffix .mli, $(SRC_ML))
given += $(addsuffix .mli, $(SRC_MLI))
given := $(realpath $(given))
not_compiled := $(filter-out $(given), $(realpath $(wildcard $(sources))))
not_given := $(filter-out $(realpath $(wildcard $(sources))), $(given))

ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
pretty_not_compiled := $(sort $(patsubst $(ROOT_DIR)/%,%,$(not_compiled)))

ifdef not_compiled

$(info Some files in the repository are not compiled: $(pretty_not_compiled))
endif

# TODO: Using the dependencies given by ocamldep, we could find if
# some file was compiled but not used.
# maybe using my deps.pdf file? Which could be updated with subgraphs.

# J'ai fait un truc, mais c'est pas encore genial. Mais apperemment les fichiers suivants
# ne servent a rien:
# implication_hash, ref_addr_base (c'est pour cvalue je crois), world.ml

check:
# 	@echo $(given)
# 	@echo "================"
# 	@echo $(realpath $(wildcard $(sources)))
# 	@echo "================"
# 	@echo $(not_given)
# 	@echo "================"
# 	@echo $(not_compiled)
# 	@echo "================"
#	$(error "Some files are not compiled:")
# else
# check:
# endif

.PHONY: check

################################################################
# Compilation of the ext/frama_c_ival directory into a pack (.cmo/.cmx).

EXT_IVAL_FILES := $(addprefix ext/framac_ival/, pretty_utils integer abstract_interp bottom floating_point float_interval fc_float  fval ival ival_noinf)
EXT_IVAL_CMX = $(addsuffix .cmx, $(EXT_IVAL_FILES))
EXT_IVAL_CMO = $(addsuffix .cmo, $(EXT_IVAL_FILES))
EXT_IVAL_CMI = $(addsuffix .cmi, ext/framac_ival/float_interval_sig ext/framac_ival/float_sig)

#$(EXT_IVAL_CMX) $(EXT_IVAL_CMO) $(EXT_IVAL_CMI) : ext/framac_ival/framac_ival_aliases.cmi
$(EXT_IVAL_CMX) $(EXT_IVAL_CMO) $(EXT_IVAL_CMI) : FLAGS := -I ext/framac_ival -I . -for-pack Codex.Framac_ival 

include ext/framac_ival/.depend

ext/framac_ival.cmo: utils/datatype_sig.cmi lattice_sig.cmo $(EXT_IVAL_CMO)
	ocamlc -bin-annot -custom -pack -o ext/framac_ival.cmo $(EXT_IVAL_CMO) $(EXT_IVAL_CMI) # ext/framac_ival/float_sig.cmi ext/framac_ival/float_interval_sig.cmi
ext/framac_ival.cmi: ext/framac_ival.cmo
ext/framac_ival.cmx: utils/datatype_sig.cmi lattice_sig.cmx $(EXT_IVAL_CMX) $(EXT_IVAL_CMI)
	ocamlopt -bin-annot -for-pack Codex -pack -o ext/framac_ival.cmx $(EXT_IVAL_CMX) $(EXT_IVAL_CMI)

ext/framac_ival/c_bindings.o: # static link for native compilation
	cd ext/framac_ival/ && ocamlopt -ccopt -fPIC -c c_bindings.c

################################################################
# Compilation of the ext/ directory into ext.cmxa

EXT_ML := $(filter ext/%, $(SRC_ML))
EXT_MLI := $(filter ext/%, $(SRC_MLI))
EXT_INCLUDES := -I ext -I utils
$(eval $(call PATH_template,EXT,ext))

################################################################
# Compilation of the utils/ directory into utils.cmxa

UTILS_ML := $(filter utils/%, $(SRC_ML))
UTILS_MLI := $(filter utils/%, $(SRC_MLI))
UTILS_INCLUDES := -I utils

utils/int_builtins_c.o: # static link for native compilation
	cd utils && ocamlopt -ccopt -fPIC -c int_builtins_c.c

$(eval $(call PATH_template,UTILS,utils))

################################################################
# Compilation of the types/ directory into types.cmxa

TYPES_ML := $(filter types/%, $(SRC_ML))
TYPES_MLI := $(filter types/%, $(SRC_MLI))
TYPES_INCLUDES := -I types -I utils

$(eval $(call PATH_template,TYPES,types))

################################################################
# Compilation of the lattices/ directory into lattices.cmxa

LATTICES_ML := $(filter lattices/%, $(SRC_ML))
LATTICES_MLI := $(filter lattices/%, $(SRC_MLI))
LATTICES_INCLUDES :=  -I utils -I lattices

$(eval $(call PATH_template,LATTICES,lattices))

################################################################
# Compilation of the transfer_functions directory.

TRANSFER_FUNCTIONS_ML := $(filter transfer_functions/%, $(SRC_ML))
TRANSFER_FUNCTIONS_MLI := $(filter transfer_functions/%, $(SRC_MLI))
TRANSFER_FUNCTIONS_INCLUDES :=  -I utils -I transfer_functions

$(eval $(call PATH_template,TRANSFER_FUNCTIONS,transfer_functions))

################################################################
# Compilation of the single_value_abstraction directory.

SINGLE_VALUE_ABSTRACTION_ML := $(filter single_value_abstraction/%, $(SRC_ML))
SINGLE_VALUE_ABSTRACTION_MLI := $(filter single_value_abstraction/%, $(SRC_MLI))
SINGLE_VALUE_ABSTRACTION_INCLUDES :=  -I utils -I single_value_abstraction

$(eval $(call PATH_template,SINGLE_VALUE_ABSTRACTION,single_value_abstraction))


################################################################
# Compilation of the basis/ directory into basis.cmxa

BASIS_ML := $(filter basis/%, $(SRC_ML))
BASIS_MLI := $(filter basis/%, $(SRC_MLI))

BASIS_INCLUDES := -I ext -I utils -I basis -I lattices -I transfer_functions -I single_value_abstraction

$(eval $(call PATH_template,BASIS,basis))

################################################################
# Compilation of the domains/ directory into domains.cmxa

DOMAINS_ML := $(filter domains/%, $(SRC_ML))
DOMAINS_MLI := $(filter domains/%, $(SRC_MLI))

DOMAINS_INCLUDES := -I utils -I domains -I ext -I lattices -I domains/memory -I domains/constraints -I transfer_functions -I types -I single_value_abstraction

$(eval $(call PATH_template,DOMAINS,domains))

################################################################
# Compilation of the term/ directory into term.cmxa

TERM_ML := $(filter term/%, $(SRC_ML))
TERM_MLI := $(filter term/%, $(SRC_MLI))
TERM_INCLUDES := -I utils -I term -I . -I transfer_functions

$(eval $(call PATH_template,TERM,term))

# Fine-grained description of cross-directory dependencies.

term/term_dag.cmo term/term_dag.cmx: INCLUDES+= -I ext
term/term_dag.cmo term/term_dag.cmx: ext/binary_heap.cmi

################################################################
# Compilation of the smtbackend/ directory into smtbackend.cmxa

SMTBACKEND_ML := $(filter smtbackend/%, $(SRC_ML))
SMTBACKEND_MLI := $(filter smtbackend/%, $(SRC_MLI))
SMTBACKEND_INCLUDES := -I utils -I smtbackend -I .

$(eval $(call PATH_template_module_aliases,SMTBACKEND,smtbackend,Smtbackend))

#$(SMTBACKEND_CMO) $(SMTBACKEND_CMI): smtbackend/smtbackend_aliases.cmi
#$(SMTBACKEND_CMO) $(SMTBACKEND_CMI): FLAGS += -open Smtbackend_aliases

smtbackend/smtbackend_term_to_smt.cmo smtbackend/smtbackend_term_to_smt.cmx: INCLUDES += -I term
#smtbackend/smtbackend_aliases.cmo smtbackend/smtbackend_aliases.cmx smtbackend/smtbackend_aliases.cmi:
#	$(OCAMLC) -no-alias-deps -w -49 smtbackend/smtbackend_aliases.ml


################################################################
# Compilation of the constraints/ directory into constraints.cmxa

CONSTRAINTS_ML := $(filter constraints/%, $(SRC_ML))
CONSTRAINTS_MLI := $(filter constraints/%, $(SRC_MLI))
CONSTRAINTS_INCLUDES := -I utils -I constraints -I . -I smtbackend -I transfer_functions

$(eval $(call PATH_template_module_aliases,CONSTRAINTS,constraints,Constraints))

################################################################
# Compilation of the root directory into term.cmxa

ROOT_ML :=  $(filter-out $(addsuffix /%, $(dirs)), $(SRC_ML))
ROOT_MLI := $(filter-out $(addsuffix /%, $(dirs)), $(SRC_MLI))
# term_constructors transfer_functions lattices_sig # term

ROOT_INCLUDES :=  -I term -I utils -I transfer_functions

$(eval $(call PATH_template,ROOT,.))

# Fine-grained description of cross-directory dependencies.



basis_sig.cmo basis_sig.cmx basis_sig.cmi: INCLUDES += -I ext -I single_value_abstraction
basis_sig.cmo basis_sig.cmx basis_sig.cmi: ext/framac_ival.cmi

basis/ival_basis.cmo basis/ival_basis.cmx basis/ival_basis.cmi: INCLUDES += -I ext
basis/ival_basis.cmo basis/ival_basis.cmx basis/ival_basis.cmi: ext/framac_ival.cmi

smtbackend/smtbackend_term_to_muz.cmx smtbackend/smtbackend_term_to_muz.cmo: INCLUDES += -I term
smtbackend/smtbackend_common.cmx smtbackend/smtbackend_common.cmo: INCLUDES += -I term

domains/constraint_domain.cmx domains/constraint_domain.cmo: INCLUDES += -I smtbackend
domains/constraint_domain2.cmx domains/constraint_domain2.cmo: INCLUDES += -I smtbackend -I constraints

basis.cmo basis.cmx: INCLUDES := -I basis

domains/assert_False_Transfer_Functions.cmo domains/assert_False_Transfer_Functions.cmx: INCLUDES += -I lattices
domains/evaluating.cmo domains/evaluating.cmx: INCLUDES += -I term
domains/constraint_domains_sig.cmi : INCLUDES += -I constraints

domains/constraint_domain.cmi domains/constraint_domain.cmo domains/constraint_domain.cmx: INCLUDES += -I basis -I constraints
domains/constraints/domains_constraints_constraint_propagation.cmi domains/constraints/domains_constraints_constraint_propagation.cmo domains/constraints/domains_constraints_constraint_propagation.cmx: INCLUDES += -I basis -I constraints
domains/constraint_domain2.cmi domains/constraint_domain2.cmo domains/constraint_domain2.cmx: INCLUDES += -I basis -I constraints
domains/constraint_domain2_domains.cmi domains/constraint_domain2_domains.cmo domains/constraint_domain2_domains.cmx: INCLUDES += -I basis -I constraints

domains/constraints/domains_constraints_nonrelational.cmi domains/constraints/domains_constraints_nonrelational.cmo domains/constraints/domains_constraints_nonrelational.cmx: INCLUDES += -I basis -I constraints
domains/constraints/domains_constraints_product.cmi domains/constraints/domains_constraints_product.cmo domains/constraints/domains_constraints_product.cmx: INCLUDES += -I basis -I constraints -I single_value_abstraction
domains/constraints/domains_constraints_apron.cmi domains/constraints/domains_constraints_apron.cmo domains/constraints/domains_constraints_apron.cmx: INCLUDES += -I basis -I constraints

domains/constraints/domains_constraints_assert_false.cmi domains/constraints/domains_constraints_assert_false.cmo domains/constraints/domains_constraints_assert_false.cmx: INCLUDES += -I basis -I constraints

domains/prod_domain.cmi domains/prod_domain.cmo domains/prod_domain.cmx: INCLUDES += -I single_value_abstraction

constraints/constraints_condition.cmx: INCLUDES += -I term



domains/term_domain.cmi domains/term_domain.cmo domains/term_domain.cmx: INCLUDES += -I term
domains/memory/region_numeric_offset.cmi domains/memory/region_numeric_offset.cmo domains/memory/region_numeric_offset.cmx: INCLUDES += -I basis
domains/memory/region_suffix_tree.cmi domains/memory/region_suffix_tree.cmo domains/memory/region_suffix_tree.cmx: INCLUDES += -I basis

domains.cmi domains.cmo domains.cmx: INCLUDES += -I domains

lattices.cmi lattices.cmo lattices.cmx: INCLUDES += -I lattices
basis.cmi basis.cmo basis.cmx: INCLUDES += -I basis
smtbackend.cmi smtbackend.cmo smtbackend.cmx: INCLUDES += -I smtbackend

frama-c/codex_register.cmo frama-c/codex_register.cmx: INCLUDES += -I domains -I basis -I lattices

# domains.cmi domains.cmo domains.cmx: domains/with_Noop.cmo domains/quadrivalent.cmo domains/finite_memory_symbolic_binary.cmo

# We compile the library into packs, that are easier to reuse.



pack_additional_cmi = domains/constraint_domains_sig.cmi

Codex.cmo: $(addsuffix .cmo, $(SRC_ML)) $(pack_additional_cmi) # ext/framac_ival/dllc_bindings.so utils/dllint_builtins_c.so
	$(OCAMLC) -bin-annot -pack -o $@ $+

Codex.cmx: $(addsuffix .cmx, $(SRC_ML)) $(pack_additional_cmi) ext/framac_ival/c_bindings.o utils/int_builtins_c.o
	$(OCAMLOPT) -bin-annot -pack -o $@ $+

Codex.cma: Codex.cmo
	ocamlmklib -verbose -o Codex $+

Codex.cmxa: Codex.cmx
	ocamlmklib -verbose -o Codex $+

dllCodex.so libCodex.a: ext/framac_ival/c_bindings.o utils/int_builtins_c.o
	ocamlmklib -verbose -o Codex $+ 

#endif

# ################################################################
# # Frama-C plugin: now has a separate Makefile.
# 
# frama-c-modules := $(addprefix frama-c/, \
# 		codex_options region_analysis_stmt \
# 		build_semantic_equations compilation_to_term \
# 		printhtml codex_register)
# 
# FRAMAC_ML := $(frama-c-modules)
# FRAMAC_MLI := $(basename $(wildcard frama-c/*.mli))
# FRAMAC_INCLUDES := -I $(FRAMAC_LIBDIR) -I $(FRAMAC_PLUGINDIR) -I frama-c -open Codex # -I term -I utils -I ext -I domains -I basis 
# $(eval $(call PATH_template,FRAMAC,frama-c))
# 
# #CodexPlugin.cmo: $(addsuffix .cmo, $(SRC_ML) $(frama-c-modules))
# #	$(OCAMLC) -pack -o $@ $+
# 
# CodexPlugin.cmo: Codex.cmo $(addsuffix .cmo, $(frama-c-modules))
# 	$(OCAMLC) -pack -o $@ $+
# 
# 
# #CodexPlugin.cmxs: $(addsuffix .cmx, $(SRC_ML) $(frama-c-modules))
# #	$(OCAMLOPT) -shared -o $@ $+
# 
# CodexPlugin.cmxs: Codex.cmx $(addsuffix .cmx, $(frama-c-modules))
# 	$(OCAMLOPT) -shared -o $@ $+
# 


################################################################
# Dependencies.

# We all the dependencies on all directories at once, so we do not
# miss any.

.depend: Makefile
	 @echo "Generating .depend"
	 @rm -f .depend && touch .depend
	 @for file in $(addsuffix .ml, $(SRC_ML)) $(addsuffix .mli, $(SRC_MLI)); \
	 do \
	 $(OCAMLDEP) $(includes) $$file >> .depend; \
	 done
	 @for file in $(addsuffix .ml, $(SMTBACKEND_ML)) $(addsuffix .mli, $(SMTBACKEND_MLI)); do \
	 $(OCAMLDEP) $(includes) -open Smtbackend_aliases -map smtbackend/smtbackend_aliases.ml $$file >> .depend; \
	 done

sinclude .depend


################################################################
# Tests

#msg-keys = -codex-msg-key domains,propagate,implication_hash,new_propagate #,hashcons
#msg-keys = -codex-msg-key domains,eager_evaluation -codex-debug 2
#msg-keys = -codex-msg-key eager_evaluation
#msg-keys = -codex-msg-key domains,eager_evaluation,hashcons -codex-debug 2
#msg-keys = -codex-msg-key domains -codex-debug 2
msg-keys = -codex-msg-key evaluating
# options = -codex-print-force-lazyness
# Note: domain 2 creates a lot of terms that are slow to display.
options := $(msg-keys) $(options) -codex-verbose-terms 0 -codex-domains 6 # -codex-domains 0,1,3
#options := $(options) -codex-unknown-uninitialized # For svcomp
#options := $(options) -no-allow-duplication # Else, some assertions are duplicated.

frama-c/top/CodexPlugin.cmo: Codex.cmo
	cd frama-c && make byte

frama-c/top/CodexPlugin.cmxs: Codex.cmx
	cd frama-c && make opt

test.byte: Codex.cmo frama-c/top/CodexPlugin.cmo check
	OCAMLRUNPARAM="b" time frama-c.byte -kernel-debug 0 -codex-debug 2 $(msg-keys) -load-module ./frama-c/top/CodexPlugin -codex -codex-print -codex-no-print-value $(options) test.c -codex-exp-dump test.dump -no-allow-duplication  && cat test.dump # -codex-unknown-uninitialized -absolute-valid-range 0-0xff
# -ulevel -1
#test.byte: Codex.cmo check
#	OCAMLRUNPARAM="b" time frama-c.byte -kernel-debug 0 $(msg-keys) -load-module ./Codex -codex -codex-print $(options) test.c # -codex-unknown-uninitialized 


test.opt: Codex.cmx frama-c/top/CodexPlugin.cmxs check
	time frama-c -codex-debug 2 -kernel-debug 2 $(msg-keys) -load-module ./frama-c/top/CodexPlugin -codex -codex-print  -codex-no-print-value $(options) test.c -ulevel -1 -codex-exp-dump test.dump -no-allow-duplication  && cat test.dump

test: test.byte

bench: Codex.cmo
	OCAMLRUNPARAM="b" time frama-c.byte -ulevel -1 -kernel-debug 0 $(msg-keys) -load-module ./Codex -codex -codex-print -codex-no-print-value $(options) -codex-exp-dump bench.dump test/adpcm.c && diff bench.dump.tocompare bench.dump


tests::  frama-c/top/CodexPlugin.cmo
	-cd tests && byte=".byte" make -k -j4 clean all # ; make update

tests.opt: frama-c/top/CodexPlugin.cmxs
	-cd tests && byte="" make -k -j8  clean all #; make update

.PHONY: clean test test.byte test.opt tests tests.opt frama-c/top/CodexPlugin.cmo frama-c/top/CodexPlugin.cmxs

CSMITH_PATH='/usr/local/include/csmith-2.2.0'

csmith:
	csmith > csmith.c && frama-c -cpp-extra-args='-I$(CSMITH_PATH)' -load-module ./Codex -codex -codex-domains 1,3 csmith.c


################################################################
# Codex library.

# LIB_SRC_ML := $(addprefix utils/, $(utils_modules)) \
#              $(addprefix ext/, WTO binary_heap) \
# 	 $(root_modules_1) \
# 	  $(addprefix term/, $(term_modules)) term \
# 	  $(addprefix smtbackend/smtbackend_, $(smtbackend_modules)) smtbackend \
# 	  $(addprefix lattices/, $(lattices_modules)) lattices \
# 	  $(addprefix basis/, $(basis_modules)) basis \
# 	  $(root_modules_2) \
# 	  $(addprefix constraints/, $(constraints_modules)) constraints \
# 	  $(addprefix domains/, $(domains_modules)) domains  \
# 	  $(addprefix frama-c/, $(root_modules_3))
# 
# #LIB_SRC_MLI := $(basename $(wildcard $(addsuffix /*.mli, $(dirs))))
# 
# 
# libcodex.cma: $(addsuffix .cmo, $(LIB_SRC_ML))
# 	$(OCAMLC) -pack -o $@ $+




################################################################
# Misc

cleaned_files := *~ *.cmi *.cmo *.cma *.cmx *.o *.a *.so *.cmxa \
		 *.cmt *.cmti *.annot *.ppx *.cmxs # .depend # .depend useful for parallel compilation.
clean::
	rm -f ext/framac_ival/*.cmo ext/framac_ival/*.cmi ext/framac_ival/*.cmx ext/framac_ival/*.o ext/framac_ival/*.annot
	rm -f $(cleaned_files) \
	      $(foreach dir,$(dirs) domains/memory frama-c,$(addprefix $(dir)/, $(cleaned_files))) 

# Show compilation dependencies in the .depend file.
deps.pdf: .depend
	make --no-print-directory -B -s -f Makefile.depend_to_dot > deps.dot && tred deps.dot > depsred.dot && dot -Tpdf depsred.dot > deps.pdf && evince deps.pdf

backup: 
	git gc && make clean && sh -c 'cd .. && tar --exclude="test/unused" --exclude="benchmarks" --exclude="test/svcomp15" --exclude="test/stb" -czf codex_`date "+%y-%m-%d-%H-%M"`_`uname -n`.tgz codex'

test-compile-fresh:
	cd `mktemp -d` && pwd && git clone $(PWD) && cd codex && make .depend && make test



# Probably not everything is needed
install: Codex.cma Codex.cmxa dllCodex.so libCodex.a Codex.o Codex.cmi Codex.cmo Codex.cmx Codex.a
	ocamlfind remove libase
	mkdir -p $$(ocamlfind printconf destdir)
	ocamlfind install libase $+ META

# Copy everything to a temporary directory, and change reduce.sh to refer to that directory.
# The copy allows to run several creduce  in parallel (or to change the code during a creduce test)
# Changing reduce.sh is nececessary because creduce now make copies itself
creduce: frama-c/top/CodexPlugin.cmxs
	cd `mktemp -d` && pwd && cp $(PWD)/frama-c/top/CodexPlugin.cmxs $(PWD)/reduce.sh $(PWD)/reduce.c . && sed -i "s%__PWD__%`pwd`%g" reduce.sh && creduce ./reduce.sh reduce.c

#pwd && cp /home/matthieu/i/src/lsl/codex/Codex.cmxs /home/matthieu/i/src/lsl/codex/reduce.sh /home/matthieu/i/src/lsl/codex/reduce.c . && creduce ./reduce.sh reduce.c

