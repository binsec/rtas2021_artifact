#!/bin/bash
SIMPKG=armv7_gt
SIMPKG_SRCDIR=tlm2/armv7_gt
SIMPKG_DSTDIR=armv7_gt
source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

import unisim/component/tlm2/processor/arm/cortex_a9 || exit
import unisim/component/tlm2/memory/ram || exit
import unisim/component/tlm2/interconnect/generic_router || exit
import unisim/service/time/sc_time || exit
import unisim/service/time/host_time || exit
import unisim/service/loader/multiformat_loader || exit
import unisim/service/trap_handler || exit
import unisim/service/debug/gdb_server || exit
import unisim/service/debug/inline_debugger || exit
import unisim/service/debug/debugger || exit
import unisim/service/profiling/addr_profiler || exit
import unisim/kernel || exit
import unisim/util/likely || exit

import m4/scml2
import m4/ax_cflags_warn_all || exit

import libc/stdio || exit
import libc/stdlib || exit
import libc/inttypes || exit
import std/cstdlib || exit
import std/iostream || exit
import std/list || exit
import std/sstream || exit
import std/stdexcept || exit
import std/string || exit

copy source isa_thumb isa_arm32 header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_THUMB_FILES="$(files isa_thumb)"

UNISIM_LIB_SIMULATOR_ISA_ARM32_FILES="$(files isa_arm32)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="\
${UNISIM_LIB_SIMULATOR_ISA_THUMB_FILES} \
${UNISIM_LIB_SIMULATOR_ISA_ARM32_FILES} \
$(files header) \
$(files template)"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_SOURCE_FILES="\
main.cc \
simulator.cc \
GenericTimer.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
simulator.hh \
GenericTimer.hh \
"

UNISIM_SIMULATOR_PKG_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
README \
INSTALL \
AUTHORS \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_FILES="${UNISIM_SIMULATOR_SOURCE_FILES} ${UNISIM_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_DATA_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG_DSTDIR}/${file}"
done

for file in ${UNISIM_SIMULATOR_PKG_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Yves Lhuillier <yves.lhuillier@cea.fr>
Gilles Mouchard <gilles.mouchard@cea.fr>
Reda Nouacer <reda.nouacer@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - ARMv7-GT: an ARM V7 modest SoC full system simulator with a generic timer
  - UniSIM GenISSLib (will not be installed): an instruction set simulator generator

See INSTALL for installation instructions.
EOF

cat << EOF > "${DEST_DIR}/INSTALL"
INSTALLATION
------------

Requirements:
  - GNU C++ compiler
  - GNU C++ standard library
  - GNU bash
  - GNU make
  - GNU autoconf
  - GNU automake
  - GNU flex
  - GNU bison
  - boost (http://www.boost.org) development package (libboost-devel for Redhat/Mandriva, libboost-graph-dev for Debian/Ubuntu)
  - libxml2 (http://xmlsoft.org/libxml2) development package (libxml2-devel for Redhat/Mandriva, libxml2-dev for Debian/Ubuntu)
  - zlib (http://www.zlib.net) development package (zlib1g-devel for Redhat/Mandriva, zlib1g-devel for Debian/Ubuntu)
  - libedit (http://www.thrysoee.dk/editline) development package (libedit-devel for Redhat/Mandriva, libedit-dev for Debian/Ubuntu)
  - Core SystemC Language >= 2.3 (http://www.systemc.org)


Building instructions:
  $ ./configure --with-systemc=<path-to-systemc-install-dir> --with-scml2=<path-to-scml2-install-dir>
  $ make

Installing (optional):
  $ make install
EOF

output_top_configure_ac <(cat << EOF
AC_INIT([UniSIM ARMv7-GT Standalone simulator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}])
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

SIM_VERSION_MAJOR=$(printf "${SIMULATOR_VERSION}" | cut -f 1 -d .)
SIM_VERSION_MINOR=$(printf "${SIMULATOR_VERSION}" | cut -f 2 -d .)
SIM_VERSION_PATCH=$(printf "${SIMULATOR_VERSION}" | cut -f 3 -d .)
SIM_VERSION="${SIMULATOR_VERSION}"
SIM_VERSION_CODENAME="Triumphalis Tarraco"
SIM_AUTHOR="Yves Lhuillier (yves.lhuillier@cea.fr)"
SIM_PROGRAM_NAME="UNISIM Arm_Simtest"
SIM_LICENSE="BSD (See file COPYING)"
SIM_COPYRIGHT="Copyright (C) 2007-2017, Commissariat a l'Energie Atomique"
SIM_DESCRIPTION="UNISIM ARM SELF SIMULATOR TEST GENERATION"
SIM_SCHEMATIC="arm_simtest/fig_schematic.pdf"
output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM ARMv7-GT C++ simulator], [${SIMULATOR_VERSION}], [Yves Lhuillier <yves.lhuillier@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}-core])
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
AC_DEFINE([SIM_VERSION_MAJOR], [${SIM_VERSION_MAJOR}], [Version major number])
AC_DEFINE([SIM_VERSION_MINOR], [${SIM_VERSION_MINOR}], [Version minor number])
AC_DEFINE([SIM_VERSION_PATCH], [${SIM_VERSION_PATCH}], [Version patch number])
AC_DEFINE([SIM_VERSION], ["${SIM_VERSION}"], [Version])
AC_DEFINE([SIM_VERSION_CODENAME], ["${SIM_VERSION_CODENAME}"], [Version code name])
AC_DEFINE([SIM_AUTHOR], ["${SIM_AUTHOR}"], [Author])
AC_DEFINE([SIM_PROGRAM_NAME], ["${SIM_PROGRAM_NAME}"], [Program name])
AC_DEFINE([SIM_COPYRIGHT], ["${SIM_COPYRIGHT}"], [Copyright])
AC_DEFINE([SIM_LICENSE], ["${SIM_LICENSE}"], [License])
AC_DEFINE([SIM_DESCRIPTION], ["${SIM_DESCRIPTION}"], [Description])
AC_DEFINE([SIM_SCHEMATIC], ["${SIM_SCHEMATIC}"], [Schematic])
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
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static-libtool-libs
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# Static Library
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -static

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.tcc\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.tcc\

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.tcc\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.hh\
	\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.tcc

\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.tcc: \$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.hh
\$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32.hh: ${UNISIM_LIB_SIMULATOR_ISA_ARM32_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/arm/isa_arm32 -w 8 -I \$(top_srcdir) -I \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/arm32 \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/arm32/arm32.isa

\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.tcc: \$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.hh
\$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb.hh: ${UNISIM_LIB_SIMULATOR_ISA_THUMB_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/arm/isa_thumb -w 8 -I \$(top_srcdir) -I \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/thumb2 \$(top_srcdir)/unisim/component/cxx/processor/arm/isa/thumb2/thumb.isa
EOF
)

build_simulator_configure

echo "Distribution is up-to-date"

