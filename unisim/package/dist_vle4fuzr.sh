#!/bin/bash

SIMPKG=vle4fuzr
SIMPKG_SRCDIR=cxx/vle4fuzr
SIMPKG_DSTDIR=vle4fuzr
source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

#import dist_vle4fuzr || exit

# PPC VLE
import unisim/component/cxx/processor/powerpc/isa/book_i/fixed_point || exit
import unisim/component/cxx/processor/powerpc/isa/book_i/efp/efs || exit
import unisim/component/cxx/processor/powerpc/isa/book_ii || exit
import unisim/component/cxx/processor/powerpc/isa/book_iii_e || exit
import unisim/component/cxx/processor/powerpc/isa/book_e || exit
import unisim/component/cxx/processor/powerpc/isa/book_vle || exit
import unisim/component/cxx/processor/powerpc/isa/lsp || exit
import unisim/component/cxx/processor/powerpc/isa/mpu || exit
import unisim/component/cxx/processor/powerpc || exit
# ARMv7
import unisim/component/cxx/processor/arm/isa/thumb || exit
import unisim/component/cxx/processor/arm/isa/thumb2 || exit
import unisim/component/cxx/processor/arm/isa/arm32 || exit
import unisim/component/cxx/processor/arm || exit
import unisim/util/symbolic || exit

import libc/inttypes || exit
import std/cstdlib || exit
import std/iostream || exit
import std/list || exit
import std/map || exit
import std/sstream || exit
import std/stdexcept || exit
import std/string || exit
import std/vector || exit

import m4/ax_cflags_warn_all || exit

copy source isa isa_vle isa_thumb isa_arm32 header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_THUMB_FILES="$(files isa_thumb)"

UNISIM_LIB_SIMULATOR_ISA_ARM32_FILES="$(files isa_arm32)"

UNISIM_LIB_SIMULATOR_ISA_PPC_FILES="\
$(files isa) \
$(files isa_vle) \
"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_THUMB_FILES} \
${UNISIM_LIB_SIMULATOR_ISA_ARM32_FILES} \
${UNISIM_LIB_SIMULATOR_ISA_PPC_FILES} \
$(files header) \
$(files template) \
"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
"

UNISIM_BINDINGS_SOURCE_FILES="\
bindings.cc \
emu.cc \
vle.cc \
arm.cc \
"

UNISIM_BINDINGS_HEADER_FILES="\
top_arm32.isa \
top_thumb.isa \
top_vle.isa \
top_vle_concrete.isa \
top_vle_branch.isa \
xvalue.hh \
emu.hh \
vle.hh \
arm.hh \
"

UNISIM_TOP_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
AUTHORS \
README \
INSTALL \
"

UNISIM_SIMULATOR_SAMPLE_FILES=" \
samples/arm_chpc.py \
samples/gcd_arm.py \
samples/gcd.c \
samples/gcd_vle.py \
samples/guess.c \
samples/guess_vle.py \
samples/memhook.py \
samples/page_info.py \
samples/sample_arm.py \
samples/unipy.py \
"

UNISIM_SIMULATOR_DATA_FILES="${UNISIM_TOP_DATA_FILES}"

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_BINDINGS_SOURCE_FILES} \
${UNISIM_BINDINGS_HEADER_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG_DSTDIR}/${file}"
done

for file in ${UNISIM_TOP_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

for file in ${UNISIM_SIMULATOR_SAMPLE_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Top level

output_top_configure_ac <(cat << EOF
AC_INIT([UniSIM VLE4FUZR package], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CONFIG_SUBDIRS([genisslib]) 
AC_CONFIG_SUBDIRS([${SIMPKG_DSTDIR}]) 
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
)

output_top_makefile_am <(cat << EOF
SUBDIRS=genisslib ${SIMPKG_DSTDIR}
EXTRA_DIST = configure.cross
dist_noinst_DATA = ${UNISIM_SIMULATOR_SAMPLE_FILES}
EOF
)

build_top_configure
build_top_configure_cross

# Simulator

output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM VLE4FUZR C++ simulator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}-core])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS([config.h])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_CXX
AC_PROG_INSTALL
LT_INIT
AC_SUBST(LIBTOOL_DEPS)
AC_PROG_LN_S
AC_LANG([C++])
AM_PROG_CC_C_O
case "\${host}" in
	*mingw*)
		CPPFLAGS="-U__STRICT_ANSI__ \${CPPFLAGS}"
		;;
	*)
		;;
esac
$(lines ac)
GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib
AC_SUBST(GENISSLIB_PATH)
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-${SIMPKG}-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
EOF
)

output_simulator_makefile_am <(cat << EOF
ACLOCAL_AMFLAGS=-I m4
AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: \$(LIBTOOL_DEPS)
	\$(SHELL) ./config.status libtool

# Program
bin_PROGRAMS = unisim-${SIMPKG}-${SIMULATOR_VERSION}
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_SOURCE_FILES}
#unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static-libtool-libs
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# Dynamic Plugin
lib_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES} ${UNISIM_BINDINGS_SOURCE_FILES}
nodist_libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = top_vle_branch.cc top_vle_concrete.cc
#libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -shared -no-undefined
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -no-undefined

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_BINDINGS_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_TOP_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.tcc\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.tcc\
	\$(top_builddir)/top_vle_concrete.hh\
	\$(top_builddir)/top_vle_concrete.cc\
	\$(top_builddir)/top_vle_branch.hh\
	\$(top_builddir)/top_vle_branch.cc\

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.tcc\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.tcc\
	\$(top_builddir)/top_vle_concrete.hh\
	\$(top_builddir)/top_vle_concrete.cc\
	\$(top_builddir)/top_vle_branch.hh\
	\$(top_builddir)/top_vle_branch.cc\

\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.tcc: \$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.hh
\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.hh: top_arm32.isa ${UNISIM_LIB_SIMULATOR_ISA_ARM32_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32 -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_arm32.isa

\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.tcc: \$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.hh
\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.hh: top_thumb.isa ${UNISIM_LIB_SIMULATOR_ISA_THUMB_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_thumb.isa

\$(top_builddir)/top_vle_concrete.cc: \$(top_builddir)/top_vle_concrete.hh
\$(top_builddir)/top_vle_concrete.hh: top_vle_concrete.isa top_vle.isa ${UNISIM_SIMULATOR_ISA_PPC_FILES}
	\$(GENISSLIB_PATH) \$(GILFLAGS) -o \$(top_builddir)/top_vle_concrete -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_vle_concrete.isa

\$(top_builddir)/top_vle_branch.cc: \$(top_builddir)/top_vle_branch.hh
\$(top_builddir)/top_vle_branch.hh: top_vle_branch.isa top_vle.isa ${UNISIM_SIMULATOR_ISA_PPC_FILES}
	\$(GENISSLIB_PATH) \$(GILFLAGS) -o \$(top_builddir)/top_vle_branch -w 8 -I \$(top_srcdir) \$(top_srcdir)/top_vle_branch.isa

EOF
)

build_simulator_configure

echo "Distribution is up-to-date"

