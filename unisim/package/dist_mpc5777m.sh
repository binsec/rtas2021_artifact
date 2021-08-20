#!/bin/bash
SIMPKG=mpc5777m
SIMPKG_SRCDIR=tlm2/mpc5777m
SIMPKG_DSTDIR=mpc5777m
source "$(dirname $0)/dist_common.sh"

import_genisslib || exit

import unisim/component/tlm2/processor/powerpc/e200/mpc57xx/e200z710n3 || exit
import unisim/component/tlm2/processor/powerpc/e200/mpc57xx/e200z425bn3 || exit
import unisim/component/tlm2/memory/ram || exit
import unisim/component/tlm2/interconnect/generic_router || exit
import unisim/component/tlm2/interconnect/freescale/mpc57xx/xbar || exit
import unisim/component/tlm2/interconnect/freescale/mpc57xx/pbridge || exit
import unisim/component/tlm2/interconnect/freescale/mpc57xx/ebi || exit
import unisim/component/tlm2/interrupt/freescale/mpc57xx/intc || exit
import unisim/component/tlm2/timer/freescale/mpc57xx/stm || exit
import unisim/component/tlm2/watchdog/freescale/mpc57xx/swt || exit
import unisim/component/tlm2/timer/freescale/mpc57xx/pit || exit
import unisim/component/tlm2/com/freescale/mpc57xx/linflexd || exit
import unisim/component/tlm2/com/serial_terminal || exit
import unisim/component/tlm2/dma/freescale/mpc57xx/dmamux || exit
import unisim/component/tlm2/dma/freescale/mpc57xx/edma || exit
import unisim/component/tlm2/operators || exit
import unisim/component/tlm2/com/freescale/mpc57xx/dspi || exit
import unisim/component/tlm2/com/freescale/mpc57xx/siul2 || exit
import unisim/component/tlm2/com/bosch/m_can || exit
import unisim/component/tlm2/memory/semaphore/freescale/mpc57xx/sema42 || exit
import unisim/kernel/tlm2 || exit
import unisim/kernel/config/xml || exit
import unisim/kernel/config/ini || exit
import unisim/kernel/config/json || exit
import unisim/kernel/logger/console || exit
import unisim/kernel/logger/text_file || exit
import unisim/kernel/logger/http || exit
import unisim/kernel/logger/xml_file || exit
import unisim/kernel/logger/netstream || exit
import unisim/util/backtrace || exit
import unisim/service/debug/debugger || exit
import unisim/service/debug/gdb_server || exit
import unisim/service/debug/inline_debugger || exit
import unisim/service/debug/profiler || exit
import unisim/service/loader/multiformat_loader || exit
import unisim/service/time/sc_time || exit
import unisim/service/time/host_time || exit
import unisim/service/netstreamer || exit
import unisim/service/http_server || exit
import unisim/service/instrumenter || exit
import unisim/service/tee/char_io || exit
import unisim/service/web_terminal || exit
import unisim/kernel/logger || exit

import std/fstream || exit
import std/sstream || exit
import std/map || exit
import std/string || exit
import std/stdexcept || exit

import m4/ax_cflags_warn_all || exit

copy source isa isa_vle header template data
copy m4 && has_to_build_simulator_configure=yes # Some imported files (m4 macros) impact configure generation

UNISIM_LIB_SIMULATOR_SOURCE_FILES="$(files source)"

UNISIM_LIB_SIMULATOR_ISA_FILES="$(files isa) $(files isa_vle)"

UNISIM_LIB_SIMULATOR_HEADER_FILES="${UNISIM_LIB_SIMULATOR_ISA_FILES} $(files header) $(files template)"

UNISIM_LIB_SIMULATOR_M4_FILES="$(files m4)"

UNISIM_LIB_SIMULATOR_DATA_FILES="$(files data)"

UNISIM_SIMULATOR_SOURCE_FILES="\
simulator.cc \
config.cc \
debugger.cc \
xbar_0.cc \
xbar_1.cc \
pbridge_a.cc \
pbridge_b.cc \
ebi.cc \
xbar_1_m1_concentrator.cc \
iahbg_0.cc \
iahbg_1.cc \
intc_0.cc \
stm_0.cc \
stm_1.cc \
stm_2.cc \
swt_0.cc \
swt_1.cc \
swt_2.cc \
swt_3.cc \
pit_0.cc \
pit_1.cc \
linflexd_0.cc \
linflexd_1.cc \
linflexd_2.cc \
linflexd_14.cc \
linflexd_15.cc \
linflexd_16.cc \
dmamux_0.cc \
dmamux_1.cc \
dmamux_2.cc \
dmamux_3.cc \
dmamux_4.cc \
dmamux_5.cc \
dmamux_6.cc \
dmamux_7.cc \
dmamux_8.cc \
dmamux_9.cc \
edma_0.cc \
edma_1.cc \
dspi_0.cc \
dspi_1.cc \
dspi_2.cc \
dspi_3.cc \
dspi_4.cc \
dspi_5.cc \
dspi_6.cc \
dspi_12.cc \
siul2.cc \
m_can_1.cc \
m_can_2.cc \
m_can_3.cc \
m_can_4.cc \
shared_can_message_ram_router.cc \
sema4.cc \
"

UNISIM_SIMULATOR_HEADER_FILES="\
simulator.hh \
config.hh \
"

UNISIM_SIMULATOR_EXTRA_FILES="\
trace32-core0.cmm.in \
trace32-core1.cmm.in \
trace32-core2.cmm.in \
trace32-multi.cmm.in \
sim_gtkwave.sh.in \
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
COPYING \
README \
INSTALL \
AUTHORS \
NEWS \
ChangeLog \
unisim.ico \
soft/Makefile \
soft/app/Makefile \
soft/app/Z4_2/main.c \
soft/app/Z7_0/main.c \
soft/app/Z7_1/main.c \
soft/app/Z4_2/boot.elf \
soft/app/Z7_0/boot.elf \
soft/app/Z7_1/boot.elf \
soft/libsys/Makefile \
soft/libsys/bin/common/gcc-wrapper \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-ranlib \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-gcc \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-ld \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-strip \
soft/libsys/bin/Z4_2/bin/powerpc-eabivle-ar \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-ranlib \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-gcc \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-ld \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-strip \
soft/libsys/bin/Z7_0/bin/powerpc-eabivle-ar \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-ranlib \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-gcc \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-ld \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-strip \
soft/libsys/bin/Z7_1/bin/powerpc-eabivle-ar \
soft/libsys/lib/libsys.a \
soft/libsys/lib/ldscripts/common/mpc57xx.ld \
soft/libsys/lib/ldscripts/Z4_2/mpc57xx.ld \
soft/libsys/lib/ldscripts/Z7_0/mpc57xx.ld \
soft/libsys/lib/ldscripts/Z7_1/mpc57xx.ld \
soft/libsys/include/typedefs.h \
soft/libsys/include/MPC5777M.h \
soft/libsys/include/derivative.h \
soft/libsys/include/compiler_api.h \
soft/libsys/include/intc.h \
soft/libsys/include/stm.h \
soft/libsys/include/swt.h \
soft/libsys/include/pit.h \
soft/libsys/include/linflexd.h \
soft/libsys/include/dmamux.h \
soft/libsys/include/edma.h \
soft/libsys/include/dspi.h \
soft/libsys/include/siul2.h \
soft/libsys/include/ebi.h \
soft/libsys/include/pbridge.h \
soft/libsys/include/xbar.h \
soft/libsys/include/smpu.h \
soft/libsys/include/m_can.h \
soft/libsys/include/sema4.h \
soft/libsys/include/console.h \
soft/libsys/include/ramdisk.h \
soft/libsys/include/lfs.h \
soft/libsys/include/lfs_util.h \
soft/libsys/src/core/intc_sw_handlers.S \
soft/libsys/src/core/startup.S \
soft/libsys/src/core/vector.S \
soft/libsys/src/core/cache.c \
soft/libsys/src/core/mpu.c \
soft/libsys/src/sys/sys.c \
soft/libsys/src/drv/intc.c \
soft/libsys/src/drv/stm.c \
soft/libsys/src/drv/swt.c \
soft/libsys/src/drv/pit.c \
soft/libsys/src/drv/linflexd.c \
soft/libsys/src/drv/dmamux.c \
soft/libsys/src/drv/edma.c \
soft/libsys/src/drv/dspi.c \
soft/libsys/src/drv/siul2.c \
soft/libsys/src/drv/ebi.c \
soft/libsys/src/drv/pbridge.c \
soft/libsys/src/drv/xbar.c \
soft/libsys/src/drv/smpu.c \
soft/libsys/src/drv/m_can.c \
soft/libsys/src/drv/sema4.c \
soft/libsys/src/drv/console.c \
soft/libsys/src/drv/ramdisk.c \
soft/libsys/src/fs/lfs.c \
soft/libsys/src/fs/lfs_util.c \
sim_config.xml \
gtkwave_init_script.tcl \
bandwidth_gtkwave_init_script.tcl \
.gtkwaverc \
gtkwave.ini \
config.t32 \
baf.bin \
"

UNISIM_SIMULATOR_TESTBENCH_FILES="\
main.cc \
"

UNISIM_SIMULATOR_FILES="\
${UNISIM_SIMULATOR_SOURCE_FILES} \
${UNISIM_SIMULATOR_HEADER_FILES} \
${UNISIM_SIMULATOR_EXTRA_FILES} \
${UNISIM_SIMULATOR_DATA_FILES} \
${UNISIM_SIMULATOR_TESTBENCH_FILES} \
"

for file in ${UNISIM_SIMULATOR_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${SIMPKG_DSTDIR}/${file}"
done

for file in ${UNISIM_SIMULATOR_PKG_DATA_FILES}; do
	dist_copy "${UNISIM_SIMULATOR_DIR}/${file}" "${DEST_DIR}/${file}"
done

# Top level

output_top_configure_ac <(cat << EOF
AC_INIT([UNISIM MPC5777M Simulator Package], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves Lhuillier <yves.lhuillier@cea.fr>, Franck Vedrine <franck.vedrine@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>], [unisim-${SIMPKG}])
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

output_simulator_configure_ac <(cat << EOF
AC_INIT([UNISIM MPC5777M Standalone simulator], [${SIMULATOR_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves Lhuillier <yves.lhuillier@cea.fr>, Franck Vedrine <franck.vedrine@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>, Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>], [unisim-${SIMPKG}-core])
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
		CXXFLAGS="-Wa,-mbig-obj \${CXXFLAGS}"
		;;
	*)
		;;
esac
$(lines ac)
GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib
AC_SUBST(GENISSLIB_PATH)
AC_DEFINE([BIN_TO_SHARED_DATA_PATH], ["../share/unisim-${SIMPKG}-${SIMULATOR_VERSION}"], [path of shared data relative to bin directory])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([trace32-core0.cmm])
AC_CONFIG_FILES([trace32-core1.cmm])
AC_CONFIG_FILES([trace32-core2.cmm])
AC_CONFIG_FILES([trace32-multi.cmm])
AC_CONFIG_FILES([sim_gtkwave.sh], [chmod +x sim_gtkwave.sh])
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
nodist_libunisim_${AM_SIMPKG}_${AM_SIMULATOR_VERSION}_la_SOURCES = unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc

noinst_HEADERS = ${UNISIM_LIB_SIMULATOR_HEADER_FILES} ${UNISIM_SIMULATOR_HEADER_FILES}
EXTRA_DIST = ${UNISIM_LIB_SIMULATOR_M4_FILES}
sharedir = \$(prefix)/share/unisim-${SIMPKG}-${SIMULATOR_VERSION}
dist_share_DATA = ${UNISIM_SIMULATOR_PKG_DATA_FILES}
nobase_dist_share_DATA = ${UNISIM_LIB_SIMULATOR_DATA_FILES} ${UNISIM_SIMULATOR_DATA_FILES} trace32-core0.cmm trace32-core1.cmm trace32-core2.cmm trace32-multi.cmm sim_gtkwave.sh

BUILT_SOURCES=\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc

CLEANFILES=\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh\
	\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc

\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.cc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh
\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3 -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z710n3/isa/vle/e200z710n3.isa

\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.cc: \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh
\$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.hh: ${UNISIM_LIB_SIMULATOR_ISA_FILES}
	\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3 -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/powerpc/e200/mpc57xx/e200z425bn3/isa/vle/e200z425bn3.isa
EOF
)

build_simulator_configure

echo "Distribution is up-to-date"
