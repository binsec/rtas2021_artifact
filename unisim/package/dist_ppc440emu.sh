#!/bin/bash
function Usage
{
	echo "Usage:"
	echo "  $0 <destination directory>"
}

if [ -z "$1" ]; then
	Usage
	exit -1
fi

HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi
DEST_DIR=$1
UNISIM_TOOLS_DIR=${MY_DIR}/../unisim_tools
UNISIM_LIB_DIR=${MY_DIR}/../unisim_lib
UNISIM_SIMULATORS_DIR=${MY_DIR}/../unisim_simulators/tlm2/ppc440emu

PPC440EMU_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-ppc440emu-${PPC440EMU_VERSION}

UNISIM_TOOLS_GENISSLIB_HEADER_FILES="\
action.hh \
cli.hh \
errtools.hh \
isa.hh \
parser.hh \
riscgenerator.hh \
specialization.hh \
variable.hh \
bitfield.hh \
comment.hh \
fwd.hh \
main.hh \
product.hh \
scanner.hh \
strtools.hh \
vect.hh \
ciscgenerator.hh \
conststr.hh \
generator.hh \
operation.hh \
referencecounting.hh \
sourcecode.hh \
subdecoder.hh"

UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES="\
scanner.cc \
parser.cc \
parser.h"

UNISIM_TOOLS_GENISSLIB_SOURCE_FILES="\
parser.yy \
scanner.ll \
action.cc \
bitfield.cc \
cli.cc \
comment.cc \
conststr.cc \
isa.cc \
main.cc \
operation.cc \
product.cc \
referencecounting.cc \
sourcecode.cc \
strtools.cc \
variable.cc \
generator.cc \
riscgenerator.cc \
ciscgenerator.cc \
subdecoder.cc \
specialization.cc \
errtools.cc"

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

UNISIM_LIB_PPC440EMU_SOURCE_FILES="\
unisim/kernel/kernel.cc \
unisim/kernel/config/xml_config_file_helper.cc \
unisim/kernel/config/ini_config_file_helper.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/util/backtrace/backtrace.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/dwarf/abbrev.cc \
unisim/util/debug/dwarf/attr.cc \
unisim/util/debug/dwarf/class.cc \
unisim/util/debug/dwarf/dwarf32.cc \
unisim/util/debug/dwarf/dwarf64.cc \
unisim/util/debug/dwarf/encoding.cc \
unisim/util/debug/dwarf/filename.cc \
unisim/util/debug/dwarf/leb128.cc \
unisim/util/debug/dwarf/ml.cc \
unisim/util/blob/blob32.cc \
unisim/util/blob/section32.cc \
unisim/util/blob/segment32.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/elf_symtab/elf_symtab64.cc \
unisim/kernel/variable/endian/endian.cc \
unisim/util/queue/queue.cc \
unisim/util/garbage_collector/garbage_collector.cc \
unisim/util/random/random.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/service/power/cache_dynamic_energy.cc \
unisim/service/power/cache_dynamic_power.cc \
unisim/service/power/cache_leakage_power.cc \
unisim/service/power/cache_power_estimator.cc \
unisim/service/power/cache_profile.cc \
unisim/service/os/linux_os/linux_os_32.cc \
unisim/service/os/linux_os/linux_os_exception.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/linux_loader/linux32_loader.cc \
unisim/service/translator/memory_address/memory/translator.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_wommu_wfpu.cc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_debug_wommu_wfpu.cc \
unisim/component/cxx/processor/powerpc/floating.cc \
unisim/component/cxx/processor/powerpc/config.cc \
unisim/component/cxx/processor/powerpc/ppc440/config.cc \
unisim/component/cxx/processor/powerpc/ppc440/tb_debug_if.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu_wommu_wfpu.cc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu_debug_wommu_wfpu.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/tlm2/memory/ram/memory_debug.cc \
unisim/component/tlm2/interconnect/generic_router/variable_mapping.cc"

UNISIM_LIB_PPC440EMU_ISA_FILES="\
unisim/component/cxx/processor/powerpc/isa/altivec.isa \
unisim/component/cxx/processor/powerpc/isa/condition.isa \
unisim/component/cxx/processor/powerpc/isa/integer.isa \
unisim/component/cxx/processor/powerpc/isa/misc.isa \
unisim/component/cxx/processor/powerpc/isa/ppc.isa \
unisim/component/cxx/processor/powerpc/isa/branch.isa \
unisim/component/cxx/processor/powerpc/isa/floating.isa \
unisim/component/cxx/processor/powerpc/isa/optional_floating.isa \
unisim/component/cxx/processor/powerpc/isa/int_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/byte_reverse_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/string_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/multiple_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/fp_load_store.isa \
unisim/component/cxx/processor/powerpc/isa/external_control.isa \
unisim/component/cxx/processor/powerpc/isa/synchronization.isa \
unisim/component/cxx/processor/powerpc/isa/cache_management.isa \
unisim/component/cxx/processor/powerpc/isa/tlb_management.isa \
unisim/component/cxx/processor/powerpc/isa/specialization.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/ppc440.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/cache_management.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/tlb_management.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/synchronization.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/integer.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/misc.isa \
unisim/component/cxx/processor/powerpc/ppc440/isa/esr.isa"

UNISIM_LIB_PPC440EMU_HEADER_FILES="${UNISIM_LIB_PPC440EMU_ISA_FILES} \
unisim/kernel/kernel.hh \
unisim/kernel/config/xml_config_file_helper.hh \
unisim/kernel/config/ini_config_file_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/util/backtrace/backtrace.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/profile.hh \
unisim/util/debug/register.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/dwarf/abbrev.hh \
unisim/util/debug/dwarf/attr.hh \
unisim/util/debug/dwarf/call_frame_vm.hh \
unisim/util/debug/dwarf/class.hh \
unisim/util/debug/dwarf/die.hh \
unisim/util/debug/dwarf/encoding.hh \
unisim/util/debug/dwarf/fde.hh \
unisim/util/debug/dwarf/fmt.hh \
unisim/util/debug/dwarf/leb128.hh \
unisim/util/debug/dwarf/macinfo.hh \
unisim/util/debug/dwarf/pub.hh \
unisim/util/debug/dwarf/stmt_prog.hh \
unisim/util/debug/dwarf/addr_range.hh \
unisim/util/debug/dwarf/call_frame_prog.hh \
unisim/util/debug/dwarf/cie.hh \
unisim/util/debug/dwarf/cu.hh \
unisim/util/debug/dwarf/dwarf.hh \
unisim/util/debug/dwarf/expr_vm.hh \
unisim/util/debug/dwarf/filename.hh \
unisim/util/debug/dwarf/fwd.hh \
unisim/util/debug/dwarf/loc.hh \
unisim/util/debug/dwarf/ml.hh \
unisim/util/debug/dwarf/range.hh \
unisim/util/debug/dwarf/stmt_vm.hh \
unisim/util/blob/blob.hh \
unisim/util/blob/section.hh \
unisim/util/blob/segment.hh \
unisim/util/debug/elf_symtab/elf_symtab.hh \
unisim/util/endian/endian.hh \
unisim/util/garbage_collector/garbage_collector.hh \
unisim/util/hash_table/hash_table.hh \
unisim/util/xml/xml.hh \
unisim/util/queue/queue.hh \
unisim/util/simfloat/floating.hh \
unisim/util/simfloat/integer.hh \
unisim/util/simfloat/host_floating.hh \
unisim/util/device/register.hh \
unisim/util/random/random.hh \
unisim/util/allocator/allocator.hh \
unisim/util/loader/elf_loader/elf_common.h \
unisim/util/loader/elf_loader/elf_loader.hh \
unisim/util/loader/elf_loader/elf32.h \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/elf_loader/elf32_loader.hh \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/hypapp/hypapp.hh \
unisim/service/interfaces/debug_yielding.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/disassembly.hh \
unisim/service/interfaces/http_server.hh \
unisim/service/interfaces/field.hh \
unisim/service/interfaces/loader.hh \
unisim/service/interfaces/memory.hh \
unisim/service/interfaces/symbol_table_lookup.hh \
unisim/service/interfaces/stmt_lookup.hh \
unisim/service/interfaces/time.hh \
unisim/service/interfaces/memory_injection.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/linux_os.hh \
unisim/service/interfaces/os.hh \
unisim/service/interfaces/cache_power_estimator.hh \
unisim/service/interfaces/power_mode.hh \
unisim/service/interfaces/synchronizable.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/interfaces/blob.hh \
unisim/service/interfaces/backtrace.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/loader/elf_loader/elf_common.h \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32.h \
unisim/service/loader/elf_loader/elf64.h \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/linux_loader/linux_loader.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/service/power/cache_power_estimator.hh \
unisim/service/power/cache_profile.hh \
unisim/service/power/cache_dynamic_energy.hh \
unisim/service/power/cache_dynamic_power.hh \
unisim/service/power/cache_leakage_power.hh \
unisim/service/os/linux_os/linux_os.hh \
unisim/service/os/linux_os/linux_os_exception.hh \
unisim/service/translator/memory_address/memory/translator.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/powerpc/exception.hh \
unisim/component/cxx/processor/powerpc/floating.hh \
unisim/component/cxx/processor/powerpc/config.hh \
unisim/component/cxx/processor/powerpc/ppc440/cpu.hh \
unisim/component/cxx/processor/powerpc/ppc440/exception.hh \
unisim/component/cxx/processor/powerpc/ppc440/config.hh \
unisim/component/cxx/cache/cache.hh \
unisim/component/cxx/tlb/tlb.hh \
unisim/component/tlm2/interrupt/types.hh \
unisim/component/tlm2/processor/powerpc/ppc440/cpu.hh \
unisim/component/tlm2/memory/ram/memory.hh \
unisim/component/tlm2/interconnect/generic_router/router.hh \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.hh \
unisim/component/tlm2/interconnect/generic_router/config.hh"

UNISIM_LIB_PPC440EMU_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/profile.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/stmt.tcc \
unisim/util/debug/dwarf/addr_range.tcc \
unisim/util/debug/dwarf/call_frame_prog.tcc \
unisim/util/debug/dwarf/cie.tcc \
unisim/util/debug/dwarf/die.tcc \
unisim/util/debug/dwarf/expr_vm.tcc \
unisim/util/debug/dwarf/loc.tcc \
unisim/util/debug/dwarf/pub.tcc \
unisim/util/debug/dwarf/stmt_prog.tcc \
unisim/util/debug/dwarf/attr.tcc \
unisim/util/debug/dwarf/call_frame_vm.tcc \
unisim/util/debug/dwarf/cu.tcc \
unisim/util/debug/dwarf/dwarf.tcc \
unisim/util/debug/dwarf/fde.tcc \
unisim/util/debug/dwarf/macinfo.tcc \
unisim/util/debug/dwarf/range.tcc \
unisim/util/debug/dwarf/stmt_vm.tcc \
unisim/util/blob/blob.tcc \
unisim/util/blob/section.tcc \
unisim/util/blob/segment.tcc \
unisim/util/debug/elf_symtab/elf_symtab.tcc \
unisim/util/queue/queue.tcc \
unisim/util/simfloat/floating.tcc \
unisim/util/simfloat/integer.tcc \
unisim/util/simfloat/host_floating.tcc \
unisim/util/loader/elf_loader/elf_loader.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/os/linux_os/linux_os.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/linux_loader/linux_loader.tcc \
unisim/service/translator/memory_address/memory/translator.tcc \
unisim/component/cxx/processor/powerpc/exception.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu.tcc \
unisim/component/cxx/processor/powerpc/ppc440/exception.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_cache.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_debugging.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_exception_handling.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_fetch.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_load_store.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_mmu.tcc \
unisim/component/cxx/processor/powerpc/ppc440/cpu_dcr.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/cxx/cache/cache.tcc \
unisim/component/cxx/tlb/tlb.tcc \
unisim/component/tlm2/processor/powerpc/ppc440/cpu.tcc \
unisim/component/tlm2/memory/ram/memory.tcc \
unisim/component/tlm2/interconnect/generic_router/router.tcc \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.tcc"

UNISIM_LIB_PPC440EMU_M4_FILES="\
m4/times.m4 \
m4/endian.m4 \
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/systemc.m4 \
m4/with_boost.m4 \
m4/cacti.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4"

UNISIM_LIB_PPC440EMU_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_powerpc.xml"

PPC440EMU_EXTERNAL_HEADERS="\
assert.h \
ctype.h \
cxxabi.h \
errno.h \
fcntl.h \
fenv.h \
float.h \
fstream \
getopt.h \
inttypes.h \
limits.h \
math.h \
signal.h \
stdarg.h \
stdio.h \
stdlib.h \
string.h \
sys/types.h \
unistd.h \
cassert \
cerrno \
cstddef \
cstdio \
cstdlib \
cstring \
stdexcept \
deque \
list \
sstream \
iosfwd \
iostream \
stack \
map \
ostream \
queue \
vector \
string"

UNISIM_SIMULATORS_PPC440EMU_SOURCE_FILES="\
main.cc \
"

UNISIM_SIMULATORS_PPC440EMU_HEADER_FILES="\
"

UNISIM_SIMULATORS_PPC440EMU_TEMPLATE_FILES=
UNISIM_SIMULATORS_PPC440EMU_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
template_default_config.xml \
unisim.ico"

UNISIM_SIMULATORS_PPC440EMU_TESTBENCH_FILES=""

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_ppc440emu_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/ppc440emu

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

UNISIM_LIB_PPC440EMU_FILES="${UNISIM_LIB_PPC440EMU_SOURCE_FILES} ${UNISIM_LIB_PPC440EMU_HEADER_FILES} ${UNISIM_LIB_PPC440EMU_TEMPLATE_FILES} ${UNISIM_LIB_PPC440EMU_DATA_FILES}"

for file in ${UNISIM_LIB_PPC440EMU_FILES}; do
	mkdir -p "${DEST_DIR}/ppc440emu/`dirname ${file}`"
	has_to_copy=no
	if [ -e "${DEST_DIR}/ppc440emu/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/ppc440emu/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/ppc440emu/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/ppc440emu/${file}" || exit
	fi
done

UNISIM_SIMULATORS_PPC440EMU_FILES="${UNISIM_SIMULATORS_PPC440EMU_SOURCE_FILES} ${UNISIM_SIMULATORS_PPC440EMU_HEADER_FILES} ${UNISIM_SIMULATORS_PPC440EMU_TEMPLATE_FILES} ${UNISIM_SIMULATORS_PPC440EMU_DATA_FILES} ${UNISIM_SIMULATORS_PPC440EMU_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_PPC440EMU_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/ppc440emu/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/ppc440emu/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/ppc440emu/${file}"
		cp -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/ppc440emu/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_PPC440EMU_DATA_FILES}; do
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


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/ppc440emu/config
mkdir -p ${DEST_DIR}/ppc440emu/m4
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

for file in ${UNISIM_LIB_PPC440EMU_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/ppc440emu/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/ppc440emu/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/ppc440emu/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/ppc440emu/${file}" || exit
		has_to_build_ppc440emu_configure=yes
	fi
done

# Top level

echo "This package contains:" > "${DEST_DIR}/README"
echo "  - UNISIM GenISSLib: an instruction set simulator generator" >> "${DEST_DIR}/README"
echo "  - UNISIM ppc440emu: a user level PowerPC SystemC TLM simulator." >> "${DEST_DIR}/README"
echo "See INSTALL for installation instructions." >> "${DEST_DIR}/README"

echo "INSTALLATION" > "${DEST_DIR}/INSTALL"
echo "------------" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Requirements:" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bash" >> "${DEST_DIR}/INSTALL"
echo "  - GNU make" >> "${DEST_DIR}/INSTALL"
echo "  - GNU autoconf" >> "${DEST_DIR}/INSTALL"
echo "  - GNU automake" >> "${DEST_DIR}/INSTALL"
echo "  - GNU flex" >> "${DEST_DIR}/INSTALL"
echo "  - GNU bison" >> "${DEST_DIR}/INSTALL"
echo "  - boost (http://www.boost.org) development package (libboost-devel for Redhat/Mandriva, libboost-graph-dev for Debian/Ubuntu)" >> "${DEST_DIR}/INSTALL"
echo "  - libxml2 (http://xmlsoft.org/libxml2) development package (libxml2-devel for Redhat/Mandriva, libxml2-dev for Debian/Ubuntu)" >> "${DEST_DIR}/INSTALL"
echo "  - zlib (http://www.zlib.net) development package (zlib1g-devel for Redhat/Mandriva, zlib1g-devel for Debian/Ubuntu)" >> "${DEST_DIR}/INSTALL"
echo "  - libedit (http://www.thrysoee.dk/editline) development package (libedit-devel for Redhat/Mandriva, libedit-dev for Debian/Ubuntu)" >> "${DEST_DIR}/INSTALL"
echo "  - Core SystemC Language >= 2.3.0 (http://www.systemc.org)" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Building instructions:" >> "${DEST_DIR}/INSTALL"
echo "  $ ./configure --with-systemc=<path-to-systemc-install-dir>" >> "${DEST_DIR}/INSTALL"
echo "  $ make" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"
echo "Installing (optional):" >> "${DEST_DIR}/INSTALL"
echo "  $ make install" >> "${DEST_DIR}/INSTALL"
echo "" >> "${DEST_DIR}/INSTALL"

CONFIGURE_AC="${DEST_DIR}/configure.ac"
MAKEFILE_AM="${DEST_DIR}/Makefile.am"


if [ ! -e "${CONFIGURE_AC}" ]; then
	has_to_build_configure=yes
else
	if [ "$0" -nt "${CONFIGURE_AC}" ]; then
		has_to_build_configure=yes
	fi
fi

if [ ! -e "${MAKEFILE_AM}" ]; then
	has_to_build_configure=yes
else
	if [ "$0" -nt "${MAKEFILE_AM}" ]; then
		has_to_build_configure=yes
	fi
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM PowerMAC G4 PCI Standalone simulator], [${PPC440EMU_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-ppc440emu])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([ppc440emu])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib ppc440emu" > "${MAKEFILE_AM}"

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

# GENISSLIB

GENISSLIB_CONFIGURE_AC="${DEST_DIR}/genisslib/configure.ac"
GENISSLIB_MAKEFILE_AM="${DEST_DIR}/genisslib/Makefile.am"


if [ ! -e "${GENISSLIB_CONFIGURE_AC}" ]; then
	has_to_build_genisslib_configure=yes
else
	if [ "$0" -nt "${GENISSLIB_CONFIGURE_AC}" ]; then
		has_to_build_genisslib_configure=yes
	fi
fi

if [ ! -e "${GENISSLIB_MAKEFILE_AM}" ]; then
	has_to_build_genisslib_configure=yes
else
	if [ "$0" -nt "${GENISSLIB_MAKEFILE_AM}" ]; then
		has_to_build_genisslib_configure=yes
	fi
fi

if [ "${has_to_build_genisslib_configure}" = "yes" ]; then
	echo "Generating GENISSLIB configure.ac"
	echo "AC_INIT([UNISIM GENISSLIB], [${GENISSLIB_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Yves  Lhuillier <yves.lhuillier@cea.fr>], [genisslib])" > "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${GENISSLIB_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${GENISSLIB_CONFIGURE_AC}"
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

	AM_GENISSLIB_VERSION=`printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g'`
	echo "Generating GENISSLIB Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(abs_top_srcdir)/m4" > "${GENISSLIB_MAKEFILE_AM}"
	echo "BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_YFLAGS = -d -p yy" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_LFLAGS = -l" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "INCLUDES=-I\$(abs_top_srcdir) -I\$(abs_top_builddir)" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_PROGRAMS = genisslib" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\\\"${GENISSLIB_VERSION}\\\"" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"

	echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi


# ppc440emu

PPC440EMU_CONFIGURE_AC="${DEST_DIR}/ppc440emu/configure.ac"
PPC440EMU_MAKEFILE_AM="${DEST_DIR}/ppc440emu/Makefile.am"


if [ ! -e "${PPC440EMU_CONFIGURE_AC}" ]; then
	has_to_build_ppc440emu_configure=yes
else
	if [ "$0" -nt "${PPC440EMU_CONFIGURE_AC}" ]; then
		has_to_build_ppc440emu_configure=yes
	fi
fi

if [ ! -e "${PPC440EMU_MAKEFILE_AM}" ]; then
	has_to_build_ppc440emu_configure=yes
else
	if [ "$0" -nt "${PPC440EMU_MAKEFILE_AM}" ]; then
		has_to_build_ppc440emu_configure=yes
	fi
fi

if [ "${has_to_build_ppc440emu_configure}" = "yes" ]; then
	echo "Generating ppc440emu configure.ac"
	echo "AC_INIT([UNISIM PowerMac G4 PCI C++ simulator], [${PPC440EMU_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>, Reda Nouacer <reda.nouacer@cea.fr>], [unisim-ppc440emu-core])" > "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${PPC440EMU_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CACTI" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\`pwd\`/../genisslib/genisslib" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-ppc440emu-${PPC440EMU_VERSION}\"], [path of shared data relative to bin directory])" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${PPC440EMU_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${PPC440EMU_CONFIGURE_AC}"

	AM_PPC440EMU_VERSION=`printf ${PPC440EMU_VERSION} | sed -e 's/\./_/g'`
	echo "Generating ppc440emu Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I \$(abs_top_srcdir)/m4" > "${PPC440EMU_MAKEFILE_AM}"
	echo "INCLUDES=-I\$(abs_top_srcdir) -I\$(abs_top_builddir)" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libppc440emu-${PPC440EMU_VERSION}.a" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "libppc440emu_${AM_PPC440EMU_VERSION}_a_SOURCES = ${UNISIM_LIB_PPC440EMU_SOURCE_FILES}" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-ppc440emu-${PPC440EMU_VERSION} unisim-ppc440emu-debug-${PPC440EMU_VERSION}" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "unisim_ppc440emu_${AM_PPC440EMU_VERSION}_SOURCES = ${UNISIM_SIMULATORS_PPC440EMU_SOURCE_FILES}" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "unisim_ppc440emu_debug_${AM_PPC440EMU_VERSION}_SOURCES = ${UNISIM_SIMULATORS_PPC440EMU_SOURCE_FILES}" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "unisim_ppc440emu_debug_${AM_PPC440EMU_VERSION}_CPPFLAGS = -DDEBUG_PPC440EMU" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "unisim_ppc440emu_${AM_PPC440EMU_VERSION}_LDADD = libppc440emu-${PPC440EMU_VERSION}.a" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "unisim_ppc440emu_debug_${AM_PPC440EMU_VERSION}_LDADD = libppc440emu-${PPC440EMU_VERSION}.a" >> "${PPC440EMU_MAKEFILE_AM}"

	echo "noinst_HEADERS = ${UNISIM_LIB_PPC440EMU_HEADER_FILES} ${UNISIM_LIB_PPC440EMU_TEMPLATE_FILES} ${UNISIM_SIMULATORS_PPC440EMU_HEADER_FILES} ${UNISIM_SIMULATORS_PPC440EMU_TEMPLATE_FILES}" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_PPC440EMU_M4_FILES}" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-ppc440emu-${PPC440EMU_VERSION}" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_PPC440EMU_DATA_FILES} ${UNISIM_SIMULATORS_PPC440EMU_DATA_FILES}" >> "${PPC440EMU_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh \$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.tcc" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "CLEANFILES=\$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh \$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.tcc" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "\$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.tcc: \$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "\$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa.hh: ${UNISIM_LIB_PPC440EMU_ISA_FILES}" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\t" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(abs_top_builddir)/unisim/component/cxx/processor/powerpc/ppc440/isa -w 8 -I \$(abs_top_srcdir) \$(abs_top_srcdir)/unisim/component/cxx/processor/powerpc/ppc440/isa/ppc440.isa" >> "${PPC440EMU_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\trm -f \"\$(abs_top_builddir)/bin/\`basename \$\${PROGRAM}\`\"; \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(abs_top_builddir)/bin'; \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\t(cd '\$(abs_top_builddir)/bin' && cp -f \"\$(abs_top_builddir)/\$\${PROGRAM}\" \`basename \"\$\${PROGRAM}\"\`); \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\tdone\n" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\trm -rf '\$(abs_top_builddir)/bin'; \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\tfi\n" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\trm -f \"\$(abs_top_builddir)/share/unisim-ppc440emu-${PPC440EMU_VERSION}/\`basename \$\${SHARED_DATA}\`\"; \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(abs_top_builddir)/share/unisim-ppc440emu-${PPC440EMU_VERSION}'; \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\t(cd '\$(abs_top_builddir)/share/unisim-ppc440emu-${PPC440EMU_VERSION}' && cp -f \"\$(abs_top_srcdir)/\$\${SHARED_DATA}\" \`basename \"\$\${SHARED_DATA}\"\`); \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\tdone\n" >> "${PPC440EMU_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\trm -rf '\$(abs_top_builddir)/share'; \\\\\n" >> "${PPC440EMU_MAKEFILE_AM}"
	printf "\tfi\n" >> "${PPC440EMU_MAKEFILE_AM}"

	echo "Building powerpc configure"
	${SHELL} -c "cd ${DEST_DIR}/ppc440emu && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
