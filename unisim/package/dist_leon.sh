#!/bin/bash

SIMPKG=leon
SIMPKG_SRCDIR=cxx/leon
SIMPKG_DSTDIR=leon
source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

import unisim/component/cxx/processor/sparc/isa/sv8

import libc/inttypes
import std/iostream
import std/sstream
import std/string
import std/map
import std/vector

import m4/ax_cflags_warn_all || exit

copy source header template data isa
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_SV8_FILES="$(files isa)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_SV8_FILES} \
$(files header) \
$(files template) \
"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_SOURCE_FILES="\
options.cc \
main.cc \
arch.cc \
sys/elfloader.cc \
hw/console.cc \
hw/pnp.cc \
utils/options.cc \
utils/trace.cc \
utils/pfxchan.cc \
utils/cfmt.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
fwd.hh \
options.hh \
arch.hh \
sys/gaislersystem.hh \
sys/elfloader.hh \
hw/peripheral.hh \
hw/controller.hh \
hw/pnp.hh \
hw/console.hh \
fpu.hh \
utils/trace.hh \
utils/cfmt.hh \
utils/beaccess.hh \
utils/options.hh \
utils/pfxchan.hh \
sys/gaislersystem.tcc \
hw/controller.tcc \
"

UNISIM_SIMULATOR_PKG_DATA_FILES="\
COPYING \
README \
INSTALL \
AUTHORS \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_DATA_FILES="\
${UNISIM_SIMULATOR_PKG_DATA_FILES} \
"

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG_DSTDIR}/${file}"
done

for file in ${UNISIM_SIMULATOR_PKG_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Top level

output_top_configure_ac <(cat << EOF
AC_INIT([UniSIM Leon package], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>], [unisim-${SIMPKG}])
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
EOF
)

build_top_configure
build_top_configure_cross

# Simulator

output_simulator_configure_ac <(cat <<EOF
AC_INIT([UNISIM Leon C++ simulator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>], [unisim-${SIMPKG}-core])
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

# Static Library
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -static

# Dynamic Plugin
lib_LTLIBRARIES = libunisim-${SIMPKG}-plugin-${SIMULATOR_VERSION}.la
libunisim_${AM_SIMPKG}_plugin_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES} ${UNISIM_SIMULATOR_SOURCE_FILES}
libunisim_${AM_SIMPKG}_plugin_${AM_SIMULATOR_VERSION}_la_CPPFLAGS = -DSIM_PLUGIN
libunisim_${AM_SIMPKG}_plugin_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -shared -no-undefined

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/sparc/isa_sv8.hh\
	\$(top_builddir)/unisim/component/cxx/processor/sparc/isa_sv8.tcc\

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/sparc/isa_sv8.hh\
	\$(top_builddir)/unisim/component/cxx/processor/sparc/isa_sv8.tcc\

\$(top_builddir)/unisim/component/cxx/processor/sparc/isa_sv8.tcc: \$(top_builddir)/unisim/component/cxx/processor/sparc/isa_sv8.hh
\$(top_builddir)/unisim/component/cxx/processor/sparc/isa_sv8.hh: ${UNISIM_LIB_SIMULATOR_ISA_SV8_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/sparc/isa_sv8 -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/sparc/isa/sv8/isa.isa

EOF
)

build_simulator_configure

echo "Distribution is up-to-date"
