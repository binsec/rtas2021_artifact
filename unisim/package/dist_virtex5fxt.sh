#!/bin/bash
SIMPKG=virtex5fxt
SIMPKG_SRCDIR=tlm2/virtex5fxt
SIMPKG_DSTDIR=virtex5fxt
source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

import unisim/component/tlm2/processor/powerpc/book_e/ppc440 || exit
import unisim/component/tlm2/memory/ram || exit
import unisim/component/tlm2/interrupt/xilinx/xps_intc || exit
import unisim/component/tlm2/interrupt/xilinx/xps_intc || exit
import unisim/component/tlm2/timer/xilinx/xps_timer || exit
import unisim/component/tlm2/timer/xilinx/xps_timer || exit
import unisim/component/tlm2/interconnect/generic_router || exit
import unisim/component/tlm2/memory/flash/am29 || exit
import unisim/component/tlm2/interconnect/xilinx/dcr_controller || exit
import unisim/component/tlm2/interconnect/xilinx/crossbar || exit
import unisim/component/tlm2/interconnect/xilinx/mci || exit
import unisim/component/tlm2/com/xilinx/xps_uart_lite || exit
import unisim/component/tlm2/com/xilinx/xps_gpio || exit
import unisim/component/tlm2/com/xilinx/xps_gpio || exit
import unisim/component/tlm2/com/xilinx/xps_gpio || exit
import unisim/kernel || exit
import unisim/kernel/config/xml || exit
import unisim/kernel/config/ini || exit
import unisim/kernel/config/json || exit
import unisim/kernel/logger/console || exit
import unisim/kernel/logger/text_file || exit
import unisim/kernel/logger/http || exit
import unisim/kernel/logger/xml_file || exit
import unisim/kernel/logger/netstream || exit
import unisim/service/debug/debugger || exit
import unisim/service/debug/gdb_server || exit
import unisim/service/debug/inline_debugger || exit
import unisim/service/loader/multiformat_loader || exit
import unisim/service/time/sc_time || exit
import unisim/service/time/host_time || exit
import unisim/service/translator/memory_address/memory || exit
import unisim/service/netstreamer || exit
import unisim/service/debug/profiler || exit
import unisim/service/http_server || exit
import unisim/service/instrumenter || exit
import unisim/service/tee/char_io || exit
import unisim/service/web_terminal || exit
import unisim/service/os/linux_os || exit
import unisim/service/debug/user_interface || exit
import unisim/kernel/variable/endian || exit
import unisim/kernel/logger || exit
import unisim/kernel/tlm2 || exit

import m4/ax_cflags_warn_all || exit

copy source isa header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_FILES="$(files isa)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="${UNISIM_LIB_SIMULATOR_ISA_FILES} $(files header) $(files template)"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_SOURCE_FILES="\
config.cc \
mplb.cc \
intc.cc \
timer.cc \
dcr_controller.cc \
crossbar.cc \
uart_lite.cc \
gpio_dip_switches_8bit.cc \
gpio_leds_8bit.cc \
gpio_5_leds_positions.cc \
gpio_push_buttons_5bit.cc \
dip_switches_8bit.cc \
leds_8bit.cc \
5_leds_positions.cc \
push_buttons_5bit.cc \
memory_router.cc \
simulator.cc \
debugger.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
simulator.hh \
config.hh \
"

UNISIM_SIMULATOR_PKG_DATA_FILES="\
COPYING \
NEWS \
ChangeLog \
"

UNISIM_SIMULATOR_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
template_default_config.xml \
unisim.ico"

UNISIM_SIMULATOR_TESTBENCH_FILES="\
main.cc \
"

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_EXTRA_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
${UNISIM_SIMULATOR_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG_DSTDIR}/${file}"
done

for file in ${UNISIM_SIMULATOR_PKG_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Gilles Mouchard <gilles.mouchard@cea.fr>
Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>
Réda Nouacer <reda.nouacer@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - UNISIM GenISSLib: an instruction set simulator generator
  - UNISIM Virtex 5 FXT Simulator: a full system Virtex-5-FXT-like simulator including a PPC440x5 and some Xilinx Virtex 5 FXT IPs.
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
  $ ./configure --with-systemc=<path-to-systemc-install-dir>
  $ make

Installing (optional):
  $ make install
EOF

output_top_configure_ac <(cat << EOF
AC_INIT([UNISIM Virtex 5 FXT Simulator Package], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}])
AC_CONFIG_AUX_DIR(config)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([subdir-objects tar-pax])
AC_PATH_PROGS(SH, sh)
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CONFIG_SUBDIRS([genisslib])
AC_CONFIG_SUBDIRS([virtex5fxt])
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

output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM Virtex 5 FXT Standalone simulator], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-${SIMPKG}-core])
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
PKG_PROG_PKG_CONFIG([0.26])
AC_LANG([C++])
AM_PROG_CC_C_O
CPPFLAGS="\${CPPFLAGS} -D_LARGEFILE64_SOURCE"
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
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_SOURCES = ${UNISIM_SIMULATOR_TESTBENCH_FILES} ${UNISIM_SIMULATOR_SOURCE_FILES}
#unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDFLAGS = -static-libtool-libs
unisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_LDADD = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la

# Static Library
noinst_LTLIBRARIES = libunisim-${SIMPKG}-${SIMULATOR_VERSION}.la
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = ${UNISIM_LIB_SIMULATOR_SOURCE_FILES}
libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_LDFLAGS = -static
nodist_libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.cc

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_PKG_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES}

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.cc

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.cc

\$(top_builddir)/unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.cc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.hh
\$(top_builddir)/unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440 -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/book_e/ppc440/isa/ppc440.isa
EOF
)

build_simulator_configure

echo "Distribution is up-to-date"
