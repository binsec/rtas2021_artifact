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

HERE=$(pwd)
MY_DIR=$(cd $(dirname $0); pwd)
DEST_DIR=$1
UNISIM_TOOLS_DIR=${MY_DIR}/../unisim_tools
UNISIM_LIB_DIR=${MY_DIR}/../unisim_lib
UNISIM_SIMULATORS_DIR=${MY_DIR}/../unisim_simulators/tlm2/at32uc3c

AT32UC3C_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-at32uc3c-${AT32UC3C_VERSION}

if test -z "${DISTCOPY}"; then
    DISTCOPY=cp
fi

UNISIM_TOOLS_GENISSLIB_HEADER_FILES="\
action.hh \
cli.hh \
errtools.hh \
isa.hh \
parser_defs.hh \
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
parser_tokens.hh"

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

UNISIM_LIB_AT32UC3C_SOURCE_FILES="\
unisim/kernel/kernel.cc \
unisim/kernel/config/xml_config_file_helper.cc \
unisim/kernel/config/ini_config_file_helper.cc \
unisim/kernel/tlm2/tlm.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/util/backtrace/backtrace.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/profile_64.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/debug/data_object.cc \
unisim/util/debug/stmt_32.cc \
unisim/util/debug/stmt_64.cc \
unisim/util/debug/dwarf/abbrev.cc \
unisim/util/debug/dwarf/attr.cc \
unisim/util/debug/dwarf/class.cc \
unisim/util/debug/dwarf/dwarf32.cc \
unisim/util/debug/dwarf/dwarf64.cc \
unisim/util/debug/dwarf/encoding.cc \
unisim/util/debug/dwarf/filename.cc \
unisim/util/debug/dwarf/leb128.cc \
unisim/util/debug/dwarf/ml.cc \
unisim/util/debug/dwarf/register_number_mapping.cc \
unisim/util/debug/dwarf/data_object.cc \
unisim/util/debug/dwarf/c_loc_expr_parser.cc \
unisim/util/blob/blob32.cc \
unisim/util/blob/blob64.cc \
unisim/util/blob/section32.cc \
unisim/util/blob/section64.cc \
unisim/util/blob/segment32.cc \
unisim/util/blob/segment64.cc \
unisim/util/debug/elf_symtab/elf_symtab32.cc \
unisim/util/debug/elf_symtab/elf_symtab64.cc \
unisim/util/debug/coff_symtab/coff_symtab32.cc \
unisim/kernel/variable/endian/endian.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/loader/coff_loader/coff_loader32.cc \
unisim/util/lexer/lexer.cc \
unisim/util/ieee754/ieee754.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/debug/debugger/debugger32.cc \
unisim/service/profiling/addr_profiler/profiler32.cc \
unisim/service/time/host_time/time.cc \
unisim/service/time/sc_time/time.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/elf_loader/elf64_loader.cc \
unisim/service/loader/raw_loader/raw_loader32.cc \
unisim/service/loader/s19_loader/s19_loader.cc \
unisim/service/loader/coff_loader/coff_loader32.cc \
unisim/service/loader/multiformat_loader/multiformat_loader.cc \
unisim/service/loader/multiformat_loader/multiformat_loader32.cc \
unisim/service/os/avr32_t2h_syscalls/avr32_t2h_syscalls_32.cc \
unisim/service/tee/loader/tee.cc \
unisim/service/tee/symbol_table_lookup/tee_32.cc \
unisim/service/tee/blob/tee_32.cc \
unisim/service/tee/stmt_lookup/tee_32.cc \
unisim/service/tee/backtrace/tee_32.cc \
unisim/component/cxx/processor/avr32/config.cc \
unisim/component/cxx/processor/avr32/avr32a/config.cc \
unisim/component/cxx/processor/avr32/avr32a/avr32uc/cpu.cc \
unisim/component/cxx/processor/avr32/avr32a/avr32uc/cpu_debug.cc \
unisim/component/cxx/processor/avr32/avr32a/avr32uc/config.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc \
unisim/component/tlm2/processor/avr32/avr32uc/cpu.cc \
unisim/component/tlm2/processor/avr32/avr32uc/cpu_debug.cc \
unisim/component/tlm2/memory/ram/memory.cc \
unisim/component/tlm2/memory/ram/memory_debug.cc \
unisim/component/tlm2/interconnect/generic_router/variable_mapping.cc \
"

UNISIM_LIB_AT32UC3C_ISA_FILES="\
unisim/component/cxx/processor/avr32/isa/avr32.isa \
unisim/component/cxx/processor/avr32/isa/arithmetic_operations.isa \
unisim/component/cxx/processor/avr32/isa/bit_shift_operations.isa \
unisim/component/cxx/processor/avr32/isa/coprocessor_interface.isa \
unisim/component/cxx/processor/avr32/isa/data_transfer_operations.isa \
unisim/component/cxx/processor/avr32/isa/dsp_operations.isa \
unisim/component/cxx/processor/avr32/isa/instruction_flow.isa \
unisim/component/cxx/processor/avr32/isa/logic_operations.isa \
unisim/component/cxx/processor/avr32/isa/multiplication_operations.isa \
unisim/component/cxx/processor/avr32/isa/simd_operations.isa \
unisim/component/cxx/processor/avr32/isa/system_control_operations.isa \
unisim/component/cxx/processor/avr32/avr32a/isa/avr32a.isa \
unisim/component/cxx/processor/avr32/avr32a/isa/instruction_flow.isa \
unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa/avr32uc.isa \
"

UNISIM_LIB_AT32UC3C_HEADER_FILES="${UNISIM_LIB_AT32UC3C_ISA_FILES} \
unisim/kernel/kernel.hh \
unisim/kernel/config/xml_config_file_helper.hh \
unisim/kernel/config/ini_config_file_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/kernel/tlm2/tlm.hh \
unisim/util/backtrace/backtrace.hh \
unisim/util/likely/likely.hh \
unisim/util/inlining/inlining.hh \
unisim/util/nat_sort/nat_sort.hh \
unisim/util/arithmetic/arithmetic.hh \
unisim/util/debug/memory_access_type.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/event.hh \
unisim/util/debug/profile.hh \
unisim/util/debug/register.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/data_object.hh \
unisim/util/debug/dwarf/abbrev.hh \
unisim/util/debug/dwarf/attr.hh \
unisim/util/debug/dwarf/call_frame_vm.hh \
unisim/util/debug/dwarf/class.hh \
unisim/util/debug/dwarf/die.hh \
unisim/util/debug/dwarf/encoding.hh \
unisim/util/debug/dwarf/fde.hh \
unisim/util/debug/dwarf/fmt.hh \
unisim/util/debug/dwarf/version.hh \
unisim/util/debug/dwarf/option.hh \
unisim/util/debug/dwarf/cfa.hh \
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
unisim/util/debug/dwarf/register_number_mapping.hh \
unisim/util/debug/dwarf/frame.hh \
unisim/util/debug/dwarf/util.hh \
unisim/util/debug/dwarf/data_object.hh \
unisim/util/debug/dwarf/c_loc_expr_parser.hh \
unisim/util/blob/blob.hh \
unisim/util/blob/section.hh \
unisim/util/blob/segment.hh \
unisim/util/debug/elf_symtab/elf_symtab.hh \
unisim/util/debug/coff_symtab/coff_symtab.hh \
unisim/util/endian/endian.hh \
unisim/util/hash_table/hash_table.hh \
unisim/util/xml/xml.hh \
unisim/util/loader/elf_loader/elf_common.h \
unisim/util/loader/elf_loader/elf_loader.hh \
unisim/util/loader/elf_loader/elf32.h \
unisim/util/loader/elf_loader/elf64.h \
unisim/util/loader/elf_loader/elf32_loader.hh \
unisim/util/loader/elf_loader/elf64_loader.hh \
unisim/util/loader/coff_loader/coff_loader.hh \
unisim/util/loader/coff_loader/ti/ti.hh \
unisim/util/dictionary/dictionary.hh \
unisim/util/lexer/lexer.hh \
unisim/util/parser/parser.hh \
unisim/util/simfloat/floating.hh \
unisim/util/simfloat/integer.hh \
unisim/util/simfloat/host_floating.hh \
unisim/util/ieee754/ieee754.hh \
unisim/util/hypapp/hypapp.hh \
unisim/service/interfaces/debug_yielding.hh \
unisim/service/interfaces/debug_event.hh \
unisim/service/interfaces/debug_info_loading.hh \
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
unisim/service/interfaces/profiling.hh \
unisim/service/interfaces/registers.hh \
unisim/service/interfaces/synchronizable.hh \
unisim/service/interfaces/trap_reporting.hh \
unisim/service/interfaces/blob.hh \
unisim/service/interfaces/backtrace.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/interfaces/avr32_t2h_syscalls.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/profiling/addr_profiler/profiler.hh \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/elf_loader/elf64_loader.hh \
unisim/service/loader/raw_loader/raw_loader.hh \
unisim/service/loader/s19_loader/s19_loader.hh \
unisim/service/loader/coff_loader/coff_loader.hh \
unisim/service/loader/multiformat_loader/multiformat_loader.hh \
unisim/service/os/avr32_t2h_syscalls/avr32_t2h_syscalls.hh \
unisim/service/time/host_time/time.hh \
unisim/service/time/sc_time/time.hh \
unisim/service/tee/loader/tee.hh \
unisim/service/tee/symbol_table_lookup/tee.hh \
unisim/service/tee/blob/tee.hh \
unisim/service/tee/stmt_lookup/tee.hh \
unisim/service/tee/backtrace/tee.hh \
unisim/component/cxx/memory/ram/memory.hh \
unisim/component/cxx/processor/avr32/config.hh \
unisim/component/cxx/processor/avr32/avr32a/config.hh \
unisim/component/cxx/processor/avr32/avr32a/avr32uc/config.hh \
unisim/component/cxx/processor/avr32/avr32a/avr32uc/cpu.hh \
unisim/component/tlm2/interrupt/types.hh \
unisim/component/tlm2/processor/avr32/avr32uc/cpu.hh \
unisim/component/tlm2/memory/ram/memory.hh \
unisim/component/tlm2/interconnect/generic_router/router.hh \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.hh \
unisim/component/tlm2/interconnect/generic_router/config.hh \
"

UNISIM_LIB_AT32UC3C_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/profile.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/stmt.tcc \
unisim/util/debug/data_object.tcc \
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
unisim/util/debug/dwarf/frame.tcc \
unisim/util/debug/dwarf/data_object.tcc \
unisim/util/blob/blob.tcc \
unisim/util/blob/section.tcc \
unisim/util/blob/segment.tcc \
unisim/util/debug/elf_symtab/elf_symtab.tcc \
unisim/util/debug/coff_symtab/coff_symtab.tcc \
unisim/util/loader/elf_loader/elf_loader.tcc \
unisim/util/loader/coff_loader/coff_loader.tcc \
unisim/util/loader/coff_loader/ti/ti.tcc \
unisim/util/dictionary/dictionary.tcc \
unisim/util/lexer/lexer.tcc \
unisim/util/parser/parser.tcc \
unisim/util/simfloat/floating.tcc \
unisim/util/simfloat/integer.tcc \
unisim/util/simfloat/host_floating.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/debug/debugger/debugger.tcc \
unisim/service/profiling/addr_profiler/profiler.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/elf_loader/elf32_loader.tcc \
unisim/service/loader/elf_loader/elf64_loader.tcc \
unisim/service/loader/raw_loader/raw_loader.tcc \
unisim/service/loader/s19_loader/s19_loader.tcc \
unisim/service/loader/coff_loader/coff_loader.tcc \
unisim/service/loader/multiformat_loader/multiformat_loader.tcc \
unisim/service/os/avr32_t2h_syscalls/avr32_t2h_syscalls.tcc \
unisim/service/translator/memory_address/memory/translator.tcc \
unisim/service/tee/loader/tee.tcc \
unisim/service/tee/symbol_table_lookup/tee.tcc \
unisim/service/tee/blob/tee.tcc \
unisim/service/tee/stmt_lookup/tee.tcc \
unisim/service/tee/backtrace/tee.tcc \
unisim/component/cxx/processor/avr32/avr32a/avr32uc/cpu.tcc \
unisim/component/cxx/memory/ram/memory.tcc \
unisim/component/tlm2/processor/avr32/avr32uc/cpu.tcc \
unisim/component/tlm2/memory/ram/memory.tcc \
unisim/component/tlm2/interconnect/generic_router/router.tcc \
unisim/component/tlm2/interconnect/generic_router/router_dispatcher.tcc \
"

UNISIM_LIB_AT32UC3C_M4_FILES="\
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
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4 \
m4/pthread.m4"

UNISIM_LIB_AT32UC3C_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_avr32.xml \
unisim/util/debug/dwarf/avr32_dwarf_register_number_mapping.xml \
"

AT32UC3C_EXTERNAL_HEADERS="\
assert.h \
ctype.h \
cxxabi.h \
errno.h \
fcntl.h \
fenv.h \
float.h \
getopt.h \
inttypes.h \
limits.h \
math.h \
signal.h \
stdarg.h \
stdio.h \
stdlib.h \
string.h \
ctype.h \
sys/types.h \
unistd.h \
fstream \
cassert \
cmath \
cerrno \
cstddef \
cstdio \
cstdlib \
cstring \
iomanip \
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
string \
set"

UNISIM_SIMULATORS_AT32UC3C_SOURCE_FILES="\
config.cc \
memory_router.cc \
memory_router_debug.cc \
"

UNISIM_SIMULATORS_AT32UC3C_HEADER_FILES="\
simulator.hh \
config.hh \
"

UNISIM_SIMULATORS_AT32UC3C_TEMPLATE_FILES="\
simulator.tcc \
"

UNISIM_SIMULATORS_AT32UC3C_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
unisim.ico"

UNISIM_SIMULATORS_AT32UC3C_TESTBENCH_FILES="\
main.cc \
main_debug.cc \
"

has_to_build_configure=no
has_to_build_configure_cross=no
has_to_build_genisslib_configure=no
has_to_build_at32uc3c_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/at32uc3c

UNISIM_TOOLS_GENISSLIB_FILES="${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES} ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES} ${UNISIM_TOOLS_GENISSLIB_DATA_FILES}"

for file in ${UNISIM_TOOLS_GENISSLIB_FILES}; do
	mkdir -p "${DEST_DIR}/$(dirname ${file})"
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
		${DISTCOPY} -f "${UNISIM_TOOLS_DIR}/genisslib/${file}" "${DEST_DIR}/genisslib/${file}" || exit
	fi
done

UNISIM_LIB_AT32UC3C_FILES="${UNISIM_LIB_AT32UC3C_SOURCE_FILES} ${UNISIM_LIB_AT32UC3C_HEADER_FILES} ${UNISIM_LIB_AT32UC3C_TEMPLATE_FILES} ${UNISIM_LIB_AT32UC3C_DATA_FILES}"

for file in ${UNISIM_LIB_AT32UC3C_FILES}; do
	mkdir -p "${DEST_DIR}/at32uc3c/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/at32uc3c/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/at32uc3c/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/at32uc3c/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/at32uc3c/${file}" || exit
	fi
done

UNISIM_SIMULATORS_AT32UC3C_FILES="${UNISIM_SIMULATORS_AT32UC3C_SOURCE_FILES} ${UNISIM_SIMULATORS_AT32UC3C_HEADER_FILES} ${UNISIM_SIMULATORS_AT32UC3C_TEMPLATE_FILES} ${UNISIM_SIMULATORS_AT32UC3C_DATA_FILES} ${UNISIM_SIMULATORS_AT32UC3C_TESTBENCH_FILES}"

for file in ${UNISIM_SIMULATORS_AT32UC3C_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/at32uc3c/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/at32uc3c/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/at32uc3c/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/at32uc3c/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_AT32UC3C_DATA_FILES}; do
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
		${DISTCOPY} -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/${file}" || exit
	fi
done


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/at32uc3c/config
mkdir -p ${DEST_DIR}/at32uc3c/m4
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
		${DISTCOPY} -f "${UNISIM_TOOLS_DIR}/${file}" "${DEST_DIR}/genisslib/${file}" || exit
		has_to_build_genisslib_configure=yes
	fi
done

for file in ${UNISIM_LIB_AT32UC3C_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/at32uc3c/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/at32uc3c/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/at32uc3c/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/at32uc3c/${file}" || exit
		has_to_build_at32uc3c_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/AUTHORS"
Julien Lisita <julien.lisita@cea.fr>
Gilles Mouchard <gilles.mouchard@cea.fr>
EOF

cat << EOF > "${DEST_DIR}/README"
This package contains:
  - UNISIM GenISSLib: an instruction set simulator generator
  - UNISIM AT32UC3C Simulator: an AVR32UC simulator.
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
  - Core SystemC Language >= 2.3.0 (http://www.systemc.org)


Building instructions:
  $ ./configure --with-systemc=<path-to-systemc-install-dir> --with-tlm20=<path-to-TLM-library-install-dir>
  $ make

Note: Configure option '--with-tlm20' is no longer needed with SystemC >= 2.3

Installing (optional):
  $ make install
EOF

CONFIGURE_AC="${DEST_DIR}/configure.ac"
MAKEFILE_AM="${DEST_DIR}/Makefile.am"
CONFIGURE_CROSS="${DEST_DIR}/configure.cross"

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

if [ ! -e "${CONFIGURE_CROSS}" ]; then
	has_to_build_configure_cross=yes
else
	if [ "$0" -nt "${CONFIGURE_CROSS}" ]; then
		has_to_build_configure_cross=yes
	fi
fi

if [ "${has_to_build_configure}" = "yes" ]; then
	echo "Generating configure.ac"
	echo "AC_INIT([UNISIM AT32UC3C Simulator Package], [${AT32UC3C_VERSION}], [Julien Lisita <julien.lisita@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>], [unisim-at32uc3c])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([at32uc3c])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
	echo "SUBDIRS=genisslib at32uc3c" > "${MAKEFILE_AM}"
	echo "EXTRA_DIST = configure.cross" >> "${MAKEFILE_AM}"

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

if [ "${has_to_build_configure_cross}" = "yes" ]; then
	echo "Building configure.cross"
	cat << EOF_CONFIGURE_CROSS > "${CONFIGURE_CROSS}"
#!/bin/bash
HERE=\$(pwd)
MY_DIR=\$(cd \$(dirname \$0); pwd)

# remove --host, --with-systemc, --with-tlm20, --with-zlib, --with-libxml2, --with-boost, --with-ncurses, --with-libedit from command line arguments
host=""
help=""
i=0
j=0
for arg in "\$@"
do
	case "\${arg}" in
		--host=*)
			host=\$(printf "%s" "\${arg}" | cut -f 2- -d '=')
			;;
		--with-systemc=* | --with-tlm20=* | --with-zlib=* | --with-libxml2=* | --with-boost=* | --with-ncurses=* | --with-libedit=*)
			;;
		--help=* | --help)
			help="yes"
			args[\${j}]=\${arg}
			j=\$((\${j}+1))
			;;
		*)
			args[\${j}]=\${arg}
			j=\$((\${j}+1))
			;;
	esac
	i=\$((\${i}+1))
done

if test "\${help}" != "yes"; then
	if test -z "\${host}"; then
		echo "ERROR: No canonical name for the host system type was specified. Use --host=<canonical name> to specify a host system type (e.g. --host=i586-pc-mingw32)"
		exit -1
	fi
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for genisslib"
else
	echo "=== configuring in genisslib (\${HERE}/genisslib)"
	echo "\$(basename \$0): running \${MY_DIR}/genisslib/configure \${args[@]}"
fi
if test ! -d \${HERE}/genisslib; then
	mkdir "\${HERE}/genisslib"
fi
cd "\${HERE}/genisslib"
\${MY_DIR}/genisslib/configure "\${args[@]}"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for at32uc3c"
else
	echo "=== configuring in at32uc3c (\${HERE}/at32uc3c) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/at32uc3c/configure \$@"
fi

if test ! -d \${HERE}/at32uc3c; then
	mkdir \${HERE}/at32uc3c
fi
cd \${HERE}/at32uc3c
\${MY_DIR}/at32uc3c/configure "\$@"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	exit 0
fi

echo "\$(basename \$0): creating Makefile.cross"
cat << EOF_MAKEFILE_CROSS > Makefile.cross
#!/usr/bin/make -f
all: at32uc3c-all
clean: genisslib-clean at32uc3c-clean
distclean: genisslib-distclean at32uc3c-distclean
	rm -f \${HERE}/Makefile.cross
install: at32uc3c-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
at32uc3c-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/at32uc3c all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
at32uc3c-clean:
	@\\\$(MAKE) -C \${HERE}/at32uc3c clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
at32uc3c-distclean:
	@\\\$(MAKE) -C \${HERE}/at32uc3c distclean
at32uc3c-install:
	@\\\$(MAKE) -C \${HERE}/at32uc3c install
EOF_MAKEFILE_CROSS

chmod +x Makefile.cross

echo "\$(basename \$0): run 'make -f \${HERE}/Makefile.cross' or '\${HERE}/Makefile.cross' to build for \${host} host system type"
EOF_CONFIGURE_CROSS
	chmod +x "${CONFIGURE_CROSS}"
fi  # has_to_build_configure_cross = "yes"

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

	AM_GENISSLIB_VERSION=$(printf ${GENISSLIB_VERSION} | sed -e 's/\./_/g')
	echo "Generating GENISSLIB Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I m4" > "${GENISSLIB_MAKEFILE_AM}"
	echo "BUILT_SOURCES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "CLEANFILES = ${UNISIM_TOOLS_GENISSLIB_BUILT_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_YFLAGS = -d -p yy" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_LFLAGS = -l" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_PROGRAMS = genisslib" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_SOURCES = ${UNISIM_TOOLS_GENISSLIB_SOURCE_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "genisslib_CPPFLAGS = -DGENISSLIB_VERSION=\\\"${GENISSLIB_VERSION}\\\"" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "noinst_HEADERS= ${UNISIM_TOOLS_GENISSLIB_HEADER_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_TOOLS_GENISSLIB_M4_FILES}" >> "${GENISSLIB_MAKEFILE_AM}"
# The following lines are a workaround caused by a bugFix in AUTOMAKE 1.12
# Note that parser_tokens.hh has been added to BUILT_SOURCES above
# assumption: parser.cc and either parser.h or parser.hh are generated at the same time
    echo "\$(top_builddir)/parser_tokens.hh: \$(top_builddir)/parser.cc" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\tif test -f \"\$(top_builddir)/parser.h\"; then \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\t\tcp -f \"\$(top_builddir)/parser.h\" \"\$(top_builddir)/parser_tokens.hh\"; \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\telif test -f \"\$(top_builddir)/parser.hh\"; then \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\t\tcp -f \"\$(top_builddir)/parser.hh\" \"\$(top_builddir)/parser_tokens.hh\"; \\\\\n" >> "${GENISSLIB_MAKEFILE_AM}"
    printf "\tfi\n" >> "${GENISSLIB_MAKEFILE_AM}"

    echo "Building GENISSLIB configure"
	${SHELL} -c "cd ${DEST_DIR}/genisslib && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi


# at32uc3c

AT32UC3C_CONFIGURE_AC="${DEST_DIR}/at32uc3c/configure.ac"
AT32UC3C_MAKEFILE_AM="${DEST_DIR}/at32uc3c/Makefile.am"


if [ ! -e "${AT32UC3C_CONFIGURE_AC}" ]; then
	has_to_build_at32uc3c_configure=yes
else
	if [ "$0" -nt "${AT32UC3C_CONFIGURE_AC}" ]; then
		has_to_build_at32uc3c_configure=yes
	fi
fi

if [ ! -e "${AT32UC3C_MAKEFILE_AM}" ]; then
	has_to_build_at32uc3c_configure=yes
else
	if [ "$0" -nt "${AT32UC3C_MAKEFILE_AM}" ]; then
		has_to_build_at32uc3c_configure=yes
	fi
fi

if [ "${has_to_build_at32uc3c_configure}" = "yes" ]; then
	echo "Generating at32uc3c configure.ac"
	echo "AC_INIT([UNISIM AT32UC3C Standalone simulator], [${AT32UC3C_VERSION}], [Julien Lisita <julien.lisita@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>], [unisim-at32uc3c-core])" > "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AM_PROG_CC_C_O" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "CPPFLAGS=\"${CPPFLAGS} -D_LARGEFILE64_SOURCE\"" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${AT32UC3C_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "case \"\${host}\" in" >> "${AT32UC3C_CONFIGURE_AC}"
	printf "\t*mingw*)\n" >> "${AT32UC3C_CONFIGURE_AC}"
	printf "\t;;\n" >> "${AT32UC3C_CONFIGURE_AC}"
	printf "\t*)\n" >> "${AT32UC3C_CONFIGURE_AC}"
	printf "\tUNISIM_CHECK_PTHREAD(main)\n" >> "${AT32UC3C_CONFIGURE_AC}"
	printf "\t;;\n" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "esac" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH(main)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "UNISIM_CHECK_SYSTEMC" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-at32uc3c-${AT32UC3C_VERSION}\"], [path of shared data relative to bin directory])" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${AT32UC3C_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${AT32UC3C_CONFIGURE_AC}"

	AM_AT32UC3C_VERSION=$(printf ${AT32UC3C_VERSION} | sed -e 's/\./_/g')
	echo "Generating at32uc3c Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I m4" > "${AT32UC3C_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libat32uc3c-${AT32UC3C_VERSION}.a" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "libat32uc3c_${AM_AT32UC3C_VERSION}_a_SOURCES = ${UNISIM_LIB_AT32UC3C_SOURCE_FILES}" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-at32uc3c-${AT32UC3C_VERSION} unisim-at32uc3c-debug-${AT32UC3C_VERSION}" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "unisim_at32uc3c_${AM_AT32UC3C_VERSION}_SOURCES = main.cc ${UNISIM_SIMULATORS_AT32UC3C_SOURCE_FILES}" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "unisim_at32uc3c_${AM_AT32UC3C_VERSION}_LDADD = libat32uc3c-${AT32UC3C_VERSION}.a" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "unisim_at32uc3c_debug_${AM_AT32UC3C_VERSION}_SOURCES = main_debug.cc ${UNISIM_SIMULATORS_AT32UC3C_SOURCE_FILES}" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "unisim_at32uc3c_debug_${AM_AT32UC3C_VERSION}_LDADD = libat32uc3c-${AT32UC3C_VERSION}.a" >> "${AT32UC3C_MAKEFILE_AM}"

	echo "noinst_HEADERS = ${UNISIM_LIB_AT32UC3C_HEADER_FILES} ${UNISIM_LIB_AT32UC3C_TEMPLATE_FILES} ${UNISIM_SIMULATORS_AT32UC3C_HEADER_FILES} ${UNISIM_SIMULATORS_AT32UC3C_TEMPLATE_FILES}" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_AT32UC3C_M4_FILES}" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-at32uc3c-${AT32UC3C_VERSION}" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_AT32UC3C_DATA_FILES} ${UNISIM_SIMULATORS_AT32UC3C_DATA_FILES}" >> "${AT32UC3C_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh \$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.tcc" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh \$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.tcc" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.tcc: \$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "\$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa.hh: ${UNISIM_LIB_AT32UC3C_ISA_FILES}" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\t" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "\$(GENISSLIB_PATH) -o \$(top_builddir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa -w 8 -I \$(top_srcdir) \$(top_srcdir)/unisim/component/cxx/processor/avr32/avr32a/avr32uc/isa/avr32uc.isa" >> "${AT32UC3C_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tdone\n" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tfi\n" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-at32uc3c-${AT32UC3C_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-at32uc3c-${AT32UC3C_VERSION}'; \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-at32uc3c-${AT32UC3C_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tdone\n" >> "${AT32UC3C_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${AT32UC3C_MAKEFILE_AM}"
	printf "\tfi\n" >> "${AT32UC3C_MAKEFILE_AM}"

	echo "Building at32uc3c configure"
	${SHELL} -c "cd ${DEST_DIR}/at32uc3c && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
