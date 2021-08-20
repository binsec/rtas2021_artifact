#!/bin/bash
function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory> <unisim_tools directory> <unisim_lib directory> <unisim_simulators directory> <unisim_docs directory>"
}

if test "x$1" = x || test "x$2" = x || test "x$3" = x || test "x$4" = x || test "x$5" = x; then
	Usage
	exit
fi

HERE=`pwd`
DEST_DIR=$1
UNISIM_TOOLS_DIR=$2
UNISIM_LIB_DIR=$3
UNISIM_SIMULATORS_DIR=$4/cxx/tesla
UNISIM_DOCS_DIR=$5

BARRA_VERSION='0.4'

UNISIM_TOOLS_GENISSLIB_HEADER_FILES="\
action.hh \
conststr.hh \
operation.hh \
scanner.hh \
vect.hh \
bitfield.hh \
fwd.hh \
parser.hh \
sourcecode.hh \
cli.hh \
isa.hh \
product.hh \
strtools.hh \
comment.hh \
main.hh \
referencecounting.hh \
variable.hh \
generator.hh \
riscgenerator.hh \
ciscgenerator.hh \
subdecoder.hh \
specialization.hh \
errtools.hh"

UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES="\
scanner.cc \
parser.cc \
parser.h"

UNISIM_TOOLS_GENISSLIB_SOURCE_FILES="\
parser.yy scanner.ll action.cc bitfield.cc cli.cc \
comment.cc conststr.cc isa.cc main.cc operation.cc product.cc \
referencecounting.cc sourcecode.cc strtools.cc variable.cc \
generator.cc riscgenerator.cc ciscgenerator.cc subdecoder.cc \
specialization.cc errtools.cc"

UNISIM_TOOLS_GENISSLIB_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

UNISIM_TOOLS_GENISSLIB_M4_FILES="\
m4/lexer.m4 \
m4/parser_gen.m4"

GENISSLIB_EXTERNAL_HEADERS="\
cassert \
cctype \
cerrno \
cstdarg \
cstdio \
cstdlib \
cstring \
fstream \
inttypes.h \
iosfwd \
iostream \
limits \
map \
memory \
ostream \
unistd.h \
vector"

UNISIM_LIB_BARRA_SOURCE_FILES="\
unisim/kernel/kernel.cc \
unisim/kernel/config/xml_config_file_helper.cc \
unisim/kernel/config/ini_config_file_helper.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/profile_64.cc \
unisim/kernel/variable/endian/endian.cc \
unisim/util/queue/queue.cc \
unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.cc \
unisim/component/cxx/processor/tesla/cpu.cc \
unisim/component/cxx/processor/tesla/g80_debug.cc \
unisim/component/cxx/processor/tesla/flags.cc \
unisim/component/cxx/processor/tesla/disasm.cc \
unisim/component/cxx/memory/ram/memory_32.cc"

UNISIM_LIB_BARRA_ISA_FILES="\
unisim/component/cxx/processor/tesla/isa/opcode/tesla.isa \
unisim/component/cxx/processor/tesla/isa/src1/src1.isa \
unisim/component/cxx/processor/tesla/isa/src2/src2.isa \
unisim/component/cxx/processor/tesla/isa/src3/src3.isa \
unisim/component/cxx/processor/tesla/isa/dest/dest.isa \
unisim/component/cxx/processor/tesla/isa/control/control.isa \
unisim/component/cxx/processor/tesla/isa/opcode/mov.isa \
unisim/component/cxx/processor/tesla/isa/opcode/integer.isa \
unisim/component/cxx/processor/tesla/isa/opcode/fp32.isa \
unisim/component/cxx/processor/tesla/isa/opcode/fp32.isa \
unisim/component/cxx/processor/tesla/isa/opcode/branch.isa"

UNISIM_LIB_BARRA_HEADER_FILES="${UNISIM_LIB_BARRA_ISA_FILES} \
unisim/kernel/kernel.hh \
unisim/kernel/config/xml_config_file_helper.hh \
unisim/kernel/config/ini_config_file_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/util/xml/xml.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/register.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/profile.hh \
unisim/util/endian/endian.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/hash_table/hash_table.hh \
unisim/util/queue/queue.hh \
unisim/util/simfloat/integer.hh \
unisim/util/simfloat/floating.hh \
unisim/util/simfloat/host_floating.hh \
unisim/util/hypapp/hypapp.hh \
unisim/service/interfaces/debug_yielding.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/ti_c_io.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/http_server.hh \
unisim/service/interfaces/field.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/resetable.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/interfaces/synchronizable.hh \
unisim/service/interfaces/instruction_stats.hh \
unisim/service/interfaces/typed_registers.hh \
unisim/service/interfaces/runnable.hh \
unisim/service/interfaces/scheduler.hh \
unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.hh \
unisim/component/cxx/processor/tesla/cpu.hh \
unisim/component/cxx/processor/tesla/config.hh \
unisim/component/cxx/processor/tesla/disasm.hh \
unisim/component/cxx/processor/tesla/exec.hh \
unisim/component/cxx/processor/tesla/flags.hh \
unisim/component/cxx/processor/tesla/instruction.hh \
unisim/component/cxx/processor/tesla/register.hh \
unisim/component/cxx/processor/tesla/exception.hh \
unisim/component/cxx/processor/tesla/simfloat.hh \
unisim/component/cxx/processor/tesla/maskstack.hh \
unisim/component/cxx/processor/tesla/operation.hh \
unisim/component/cxx/processor/tesla/implicit_flow.hh \
unisim/component/cxx/processor/tesla/warp.hh \
unisim/component/cxx/processor/tesla/tesla_flow.hh \
unisim/component/cxx/processor/tesla/stats.hh \
unisim/component/cxx/processor/tesla/vectorfp32.hh \
unisim/component/cxx/processor/tesla/vectorfp32sse.hh \
unisim/component/cxx/processor/tesla/forward.hh \
unisim/component/cxx/processor/tesla/enums.hh \
unisim/component/cxx/processor/tesla/strided_register.hh \
unisim/component/cxx/processor/tesla/sampler.hh \
unisim/component/cxx/processor/tesla/hostfloat/hostfloat.hh \
unisim/component/cxx/processor/tesla/hostfloat/rounding.hh \
unisim/component/cxx/processor/tesla/hostfloat/denormals.hh \
unisim/component/cxx/processor/tesla/hostfloat/c99_rounding.hh \
unisim/component/cxx/processor/tesla/hostfloat/msvc_rounding.hh \
unisim/component/cxx/processor/tesla/hostfloat/traits.hh \
unisim/component/cxx/processor/tesla/interfaces.hh \
unisim/component/cxx/memory/ram/memory.hh"


	

UNISIM_LIB_BARRA_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/profile.tcc \
unisim/util/queue/queue.tcc \
unisim/util/simfloat/integer.tcc \
unisim/util/simfloat/floating.tcc \
unisim/util/simfloat/host_floating.tcc \
unisim/component/cxx/scheduler/cuda_scheduler/cuda_scheduler.tcc \
unisim/component/cxx/processor/tesla/cpu.tcc \
unisim/component/cxx/processor/tesla/exec.tcc \
unisim/component/cxx/processor/tesla/flags.tcc \
unisim/component/cxx/processor/tesla/instruction.tcc \
unisim/component/cxx/processor/tesla/register.tcc \
unisim/component/cxx/processor/tesla/exception.tcc \
unisim/component/cxx/processor/tesla/simfloat.tcc \
unisim/component/cxx/processor/tesla/memory.tcc \
unisim/component/cxx/processor/tesla/maskstack.tcc \
unisim/component/cxx/processor/tesla/operation.tcc \
unisim/component/cxx/processor/tesla/implicit_flow.tcc \
unisim/component/cxx/processor/tesla/warp.tcc \
unisim/component/cxx/processor/tesla/tesla_flow.tcc \
unisim/component/cxx/processor/tesla/stats.tcc \
unisim/component/cxx/processor/tesla/vectorfp32.tcc \
unisim/component/cxx/processor/tesla/vectorfp32sse.tcc \
unisim/component/cxx/processor/tesla/strided_register.tcc \
unisim/component/cxx/processor/tesla/sampler.tcc \
unisim/component/cxx/processor/tesla/hostfloat/hostfloat.tcc \
unisim/component/cxx/processor/tesla/hostfloat/rounding.tcc \
unisim/component/cxx/memory/ram/memory.tcc"

UNISIM_LIB_BARRA_M4_FILES="\
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/with_boost.m4 \
m4/boost_thread.m4 \
m4/boost_integer.m4"

BARRA_EXTERNAL_HEADERS="\
assert.h \
endian.h \
errno.h \
fcntl.h \
fstream \
getopt.h \
inttypes.h \
iosfwd \                                                                                                                                                        
iostream \
list \
map \
ostream \
queue \
signal.h \
sstream \
stdarg.h \
stdexcept \
stdio.h \
stdlib.h \
string \
string.h \
sys/stat.h \
sys/times.h \
sys/types.h \
time.h \
unistd.h \
vector \
"
# TODO: find the many other headers used and declare them here...


UNISIM_SIMULATORS_BARRA_SOURCE_FILES="cuda.cc driver_objects.cc exception.cc fatformat.cc system.cc"
UNISIM_SIMULATORS_BARRA_HEADER_FILES="device.hh exception.hh cuda.h driver.hh driver_objects.hh module.hh \
	config.hh kernel.hh fatformat.hh event.hh system.hh"
UNISIM_SIMULATORS_BARRA_TEMPLATE_FILES="device.tcc module.tcc driver.tcc kernel.tcc event.tcc"
UNISIM_SIMULATORS_BARRA_DATA_FILES="COPYING INSTALL NEWS README AUTHORS ChangeLog"

UNISIM_DOCS_FILES=""

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_barra_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/barra

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
	mkdir -p "${DEST_DIR}/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/genisslib/${file}" ]; then
		if [ "${UNISIM_TOOLS_DIR}/genisslib/${file}" -nt "${DEST_DIR}/genisslib/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_TOOLS_DIR}/genisslib/${file} ==> ${DEST_DIR}/genisslib/${file}"
		cp -f "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}" || exit
	fi
done

UNISIM_LIB_BARRA_FILES="${UNISIM_LIB_BARRA_SOURCE_FILES} ${UNISIM_LIB_BARRA_HEADER_FILES} ${UNISIM_LIB_BARRA_TEMPLATE_FILES}"

for file in ${UNISIM_LIB_BARRA_FILES}; do
	mkdir -p "${DEST_DIR}/barra/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/barra/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/barra/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/barra/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/barra/${file}" || exit
	fi
done

UNISIM_SIMULATORS_BARRA_FILES="${UNISIM_SIMULATORS_BARRA_SOURCE_FILES} ${UNISIM_SIMULATORS_BARRA_HEADER_FILES} ${UNISIM_SIMULATORS_BARRA_TEMPLATE_FILES} ${UNISIM_SIMULATORS_BARRA_DATA_FILES}"

for file in ${UNISIM_SIMULATORS_BARRA_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/barra/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/barra/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/barra/${file}"
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/barra/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_BARRA_DATA_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/${file}"
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/${file}" || exit
	fi
done

for file in ${UNISIM_DOCS_FILES}; do
	mkdir -p "${DEST_DIR}/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/${file}" ]; then
		if [ "${UNISIM_DOCS_DIR}/${file}" -nt "${DEST_DIR}/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_DOCS_DIR}/${file} ==> ${DEST_DIR}/${file}"
		cp -f "${UNISIM_DOCS_DIR}/${file}" "${DEST_DIR}/${file}" || exit
	fi
done


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/barra/config
mkdir -p ${DEST_DIR}/barra/m4
mkdir -p ${DEST_DIR}/genisslib/config
mkdir -p ${DEST_DIR}/genisslib/m4

for file in ${UNISIM_TOOLS_GENISSLIB_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/genisslib/${file}" ]; then
		if [ "${UNISIM_TOOLS_DIR}/${file}" -nt  "${DEST_DIR}/genisslib/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_TOOLS_DIR}/${file} ==> ${DEST_DIR}/genisslib/${file}"
		cp -f "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}" || exit
		has_to_build_genisslib_configure=yes
	fi
done

for file in ${UNISIM_LIB_BARRA_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/barra/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/barra/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/barra/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/barra/${file}" || exit
		has_to_build_barra_configure=yes
	fi
done

# Top level

echo "This package contains GenISSLib, an instruction set simulator generator, and Barra, a G80 instruction set simulator." > "${DEST_DIR}/README"
echo "See INSTALL for installation instructions." >> "${DEST_DIR}/README"
echo "See COPYING for licensing." >> "${DEST_DIR}/README"

echo "INSTALLATION" > "${DEST_DIR}/INSTALL"
echo "------------" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Requirements:" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bash (bash-3.0-19.3 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU make (make-3.80-6.EL4 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU autoconf (autoconf-2.59-5 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU automake (automake-1.9.2-3 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU flex (flex-2.5.4a-33 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bison (bison-1.875c-2 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - boost development package (boost-devel-1.32.0-1.rhel4 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - libxml2 development package (libxml2-devel-2.6.16-6 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "  - zlib development package (zlib-devel-1.2.1.2-1.2 on RHEL4)" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Building instructions:" >> "${DEST_DIR}/INSTALL"
echo "  $ ./configure" >> "${DEST_DIR}/INSTALL"
echo "  $ make" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Installing (optional):" >> "${DEST_DIR}/INSTALL"
echo "  $ make install" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"

CONFIGURE_AC="${DEST_DIR}/configure.ac"
MAKEFILE_AM="${DEST_DIR}/Makefile.am"


if [ ! -e "${CONFIGURE_AC}" ]; then
	has_to_build_configure=yes
fi

if [ ! -e "${MAKEFILE_AM}" ]; then
	has_to_build_configure=yes
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM Barra Standalone simulator], [$BARRA_VERSION], [Sylvain Collange <sylvain.collange@univ-perp.fr>], [barra])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([barra])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib barra" > "${MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_DOCS_FILES}" >> "${MAKEFILE_AM}"

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

# GENISSLIB

GENISSLIB_CONFIGURE_AC="${DEST_DIR}/genisslib/configure.ac"
GENISSLIB_MAKEFILE_AM="${DEST_DIR}/genisslib/Makefile.am"


if [ ! -e "${GENISSLIB_CONFIGURE_AC}" ]; then
	has_to_build_genisslib_configure=yes
fi

if [ ! -e "${GENISSLIB_MAKEFILE_AM}" ]; then
	has_to_build_genisslib_configure=yes
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
	echo "Generating GENISSLIB configure.ac"
	echo "AC_INIT([UNISIM GENISSLIB], [0.1], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])" > "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${GENISSLIB_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LEXER_GENERATOR" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "UNISIM_CHECK_PARSER_GENERATOR" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${GENISSLIB_CONFIGURE_AC}"

	echo "Generating GENISSLIB Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I m4" > "${GENISSLIB_MAKEFILE_AM}"
	echo "BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_YFLAGS = -d -p yy" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_LFLAGS = -l" >> "${GENISSLIB_MAKEFILE_AM}"
#	echo "AM_CXXFLAGS = -O1 # workaround for gcc-4.1" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "bin_PROGRAMS = genisslib" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"

	echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi


# Barra

BARRA_CONFIGURE_AC="${DEST_DIR}/barra/configure.ac"
BARRA_MAKEFILE_AM="${DEST_DIR}/barra/Makefile.am"


if [ ! -e "${BARRA_CONFIGURE_AC}" ]; then
	has_to_build_barra_configure=yes
fi

if [ ! -e "${BARRA_MAKEFILE_AM}" ]; then
	has_to_build_barra_configure=yes
fi

if [ "${has_to_build_barra_configure}" = "yes" ]; then
	echo "Generating Barra configure.ac"
	echo "AC_INIT([UNISIM Barra G80 simulator], [$BARRA_VERSION], [Sylvain Collange <sylvain.collange@univ-perp.fr>], [barra_cxx])" > "${BARRA_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${BARRA_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_ENABLE_SHARED" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_DISABLE_STATIC" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_PROG_LIBTOOL" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_SUBST(LIBTOOL_DEPS)" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${BARRA_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_THREAD" >> "${BARRA_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_INTEGER" >> "${BARRA_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${BARRA_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${BARRA_CONFIGURE_AC}"

	echo "Generating Barra Makefile.am"
#	echo "ACLOCAL_AMFLAGS=-I m4" > "${BARRA_MAKEFILE_AM}"
	echo "ACLOCAL_AMFLAGS=-I m4" > "${BARRA_MAKEFILE_AM}"
	echo "barra_la_INCLUDES=-I\$(top_srcdir) -I\$(top_builddir)" >> "${BARRA_MAKEFILE_AM}"
	echo "lib_LTLIBRARIES = libbarra.la" >> "${BARRA_MAKEFILE_AM}"
	echo "libbarra_la_SOURCES = ${UNISIM_LIB_BARRA_SOURCE_FILES} ${UNISIM_SIMULATORS_BARRA_SOURCE_FILES}" >> "${BARRA_MAKEFILE_AM}"
	echo "noinst_HEADERS = ${UNISIM_TOOLS_BARRA_HEADER_FILES} ${UNISIM_LIB_BARRA_HEADER_FILES} ${UNISIM_LIB_BARRA_TEMPLATE_FILES} ${UNISIM_SIMULATORS_BARRA_HEADER_FILES} ${UNISIM_SIMULATORS_BARRA_TEMPLATE_FILES}" >> "${BARRA_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_BARRA_M4_FILES}" >> "${BARRA_MAKEFILE_AM}"
	echo 'BUILT_SOURCES = $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_opcode.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_opcode.tcc \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src1.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src1.tcc \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src2.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src2.tcc \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src3.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src3.tcc \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_dest.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_dest.tcc \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_control.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_control.tcc' >> "${BARRA_MAKEFILE_AM}"
	echo '	' >> "${BARRA_MAKEFILE_AM}"
	echo 'CLEANFILES = $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_opcode.tcc $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_opcode.hh \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src1.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src1.tcc \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src2.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src2.tcc \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src3.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src3.tcc \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_dest.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_dest.tcc \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_control.hh $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_control.tcc' >> "${BARRA_MAKEFILE_AM}"
	echo '' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_opcode.tcc: $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_opcode.hh' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_opcode.hh: $(top_srcdir)/unisim/component/cxx/processor/tesla/isa/opcode/tesla.isa \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/isa/opcode/fp32.isa $(top_srcdir)/unisim/component/cxx/processor/tesla/isa/opcode/mov.isa \' >> "${BARRA_MAKEFILE_AM}"
	echo '	$(top_srcdir)/unisim/component/cxx/processor/tesla/isa/opcode/integer.isa $(top_srcdir)/unisim/component/cxx/processor/tesla/isa/opcode/branch.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '	cd $(top_srcdir)/unisim/component/cxx/processor/tesla; $(GENISSLIB_PATH) -I . -o tesla_opcode -w 32 isa/opcode/tesla.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src1.tcc: $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src1.hh' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src1.hh: $(top_srcdir)/unisim/component/cxx/processor/tesla/isa/src1/src1.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '	cd $(top_srcdir)/unisim/component/cxx/processor/tesla; $(GENISSLIB_PATH) -I . -o tesla_src1 -w 32 isa/src1/src1.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src2.tcc: $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src2.hh' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src2.hh: $(top_srcdir)/unisim/component/cxx/processor/tesla/isa/src2/src2.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '	cd $(top_srcdir)/unisim/component/cxx/processor/tesla; $(GENISSLIB_PATH) -I . -o tesla_src2 -w 32 isa/src2/src2.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src3.tcc: $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src3.hh' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_src3.hh: $(top_srcdir)/unisim/component/cxx/processor/tesla/isa/src3/src3.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '	cd $(top_srcdir)/unisim/component/cxx/processor/tesla; $(GENISSLIB_PATH) -I . -o tesla_src3 -w 32 isa/src3/src3.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_dest.tcc: $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_dest.hh' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_dest.hh: $(top_srcdir)/unisim/component/cxx/processor/tesla/isa/dest/dest.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '	cd $(top_srcdir)/unisim/component/cxx/processor/tesla; $(GENISSLIB_PATH) -I . -o tesla_dest -w 32 isa/dest/dest.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_control.tcc: $(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_control.hh' >> "${BARRA_MAKEFILE_AM}"
	echo '$(top_srcdir)/unisim/component/cxx/processor/tesla/tesla_control.hh: $(top_srcdir)/unisim/component/cxx/processor/tesla/isa/control/control.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '	cd $(top_srcdir)/unisim/component/cxx/processor/tesla; $(GENISSLIB_PATH) -I . -o tesla_control -w 32 isa/control/control.isa' >> "${BARRA_MAKEFILE_AM}"
	echo '' >> "${BARRA_MAKEFILE_AM}"
	echo 'install-exec-hook:' >> "${BARRA_MAKEFILE_AM}"
	echo '	cd $(DESTDIR)$(libdir); $(LN_S) -f libbarra.so libcuda.so' >> "${BARRA_MAKEFILE_AM}"
	echo '	cd $(DESTDIR)$(libdir); $(LN_S) -f libbarra.so libcuda.so.1' >> "${BARRA_MAKEFILE_AM}"

	echo "Building Barra configure"
	${SHELL} -c "cd ${DEST_DIR}/barra && libtoolize --force && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
