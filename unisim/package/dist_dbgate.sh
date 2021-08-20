#!/bin/bash

SIMPKG=dbgate
SIMPKG_SRCDIR=cxx/dbgate
source "$(dirname $0)/dist_common.sh"

import unisim/util/loader/elf_loader
import unisim/util/identifier
import unisim/util/dbgate/py
import unisim/util/hypapp

import libc/inttypes
import std/iostream
import std/sstream
import std/string
import std/map
import std/vector

import m4/pthread

copy source header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_PLUGIN_SOURCE_FILES="$(files source)"

UNISIM_LIB_PLUGIN_HEADER_FILES="$(files header) $(files template)"

UNISIM_LIB_PLUGIN_M4_FILES="$(files m4)"

UNISIM_PROGRAM_SOURCE_FILES="\
tracee.cc \
tracer.cc \
"

UNISIM_PROGRAM_HEADER_FILES="\
tracee.hh \
"

UNISIM_PROGRAM_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
AUTHORS \
README \
INSTALL \
dbgate.py \
dbgate.gdb \
"

UNISIM_LIB_PLUGIN_FILES="\
${UNISIM_LIB_PLUGIN_SOURCE_FILES} \
${UNISIM_LIB_PLUGIN_HEADER_FILES} \
"

UNISIM_PROGRAM_FILES="\
${UNISIM_PROGRAM_SOURCE_FILES} \
${UNISIM_PROGRAM_HEADER_FILES} \
${UNISIM_PROGRAM_DATA_FILES} \
"

for file in ${UNISIM_PROGRAM_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Simulator

output_simulator_configure_ac <(cat <<EOF
AC_INIT([UNISIM DBGate C++ Server], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>], [unisim-${SIMPKG}])
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
AX_CXXFLAGS_WARN_ALL
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
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_PROGRAM_SOURCE_FILES}
unisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# Dynamic Plugin
lib_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_PLUGIN_SOURCE_FILES}
#libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -shared -no-undefined
libunisim_${SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -no-undefined

noinst_HEADERS = ${UNISIM_LIB_PLUGIN_HEADER_FILES} ${UNISIM_PROGRAM_HEADER_FILES} dbgate.gdb dbgate.py
EXTRA_DIST = ${UNISIM_LIB_PLUGIN_M4_FILES}

install-exec-hook:
	cd \$(DESTDIR)\$(libdir) && \$(MKDIR_P) \$(PYTHON_LIB)/site-packages/${SIMPKG}
	\$(INSTALL) \$(abs_srcdir)/dbgate.py \$(DESTDIR)\$(libdir)/\$(PYTHON_LIB)/site-packages/${SIMPKG}/__init__.py
	\$(INSTALL) \$(abs_srcdir)/dbgate.gdb \$(DESTDIR)\$(libdir)/\$(PYTHON_LIB)/site-packages/${SIMPKG}/dbgate.gdb
	\$(PYTHON_BIN) \$(DESTDIR)\$(libdir)/\$(PYTHON_LIB)/site-packages/${SIMPKG}/__init__.py \$(DESTDIR)\$(libdir)/libunisim-${SIMPKG}-${SIMULATOR_VERSION}.so

EOF
)

build_simulator_configure

echo "Distribution is up-to-date"
