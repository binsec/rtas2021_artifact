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
UNISIM_SIMULATORS_DIR=${MY_DIR}/../unisim_simulators/cxx/tms320c3x
# UNISIM_DOCS_DIR=${MY_DIR}/../unisim_docs

TMS320C3X_VERSION=$(cat ${UNISIM_SIMULATORS_DIR}/VERSION)
GENISSLIB_VERSION=$(cat ${UNISIM_TOOLS_DIR}/genisslib/VERSION)-tms320c3x-${TMS320C3X_VERSION}

# DOCS_VERSION=lastest-tms320c3x-${TMS320C3X_VERSION}
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

UNISIM_LIB_TMS320C3X_SOURCE_FILES="\
unisim/kernel/kernel.cc \
unisim/kernel/config/xml_config_file_helper.cc \
unisim/kernel/config/ini_config_file_helper.cc \
unisim/kernel/logger/logger.cc \
unisim/kernel/logger/logger_server.cc \
unisim/util/backtrace/backtrace.cc \
unisim/util/xml/xml.cc \
unisim/util/debug/symbol_32.cc \
unisim/util/debug/symbol_64.cc \
unisim/util/debug/symbol_table_32.cc \
unisim/util/debug/symbol_table_64.cc \
unisim/util/debug/watchpoint_registry_32.cc \
unisim/util/debug/watchpoint_registry_64.cc \
unisim/util/debug/breakpoint_registry_32.cc \
unisim/util/debug/breakpoint_registry_64.cc \
unisim/util/debug/data_object.cc \
unisim/util/debug/profile_32.cc \
unisim/util/debug/profile_64.cc \
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
unisim/util/debug/coff_symtab/coff_symtab64.cc \
unisim/kernel/variable/endian/endian.cc \
unisim/util/loader/elf_loader/elf32_loader.cc \
unisim/util/loader/elf_loader/elf64_loader.cc \
unisim/util/loader/coff_loader/coff_loader32.cc \
unisim/util/loader/coff_loader/coff_loader64.cc \
unisim/util/lexer/lexer.cc \
unisim/service/loader/coff_loader/coff_loader32.cc \
unisim/service/loader/coff_loader/coff_loader64.cc \
unisim/service/loader/elf_loader/elf32_loader.cc \
unisim/service/loader/elf_loader/elf64_loader.cc \
unisim/service/loader/raw_loader/raw_loader32.cc \
unisim/service/loader/raw_loader/raw_loader64.cc \
unisim/service/loader/s19_loader/s19_loader.cc \
unisim/service/loader/multiformat_loader/multiformat_loader32.cc \
unisim/service/loader/multiformat_loader/multiformat_loader64.cc \
unisim/service/debug/inline_debugger/inline_debugger.cc \
unisim/service/debug/inline_debugger/inline_debugger_32.cc \
unisim/service/debug/inline_debugger/inline_debugger_64.cc \
unisim/service/debug/gdb_server/gdb_server_32.cc \
unisim/service/debug/gdb_server/gdb_server_64.cc \
unisim/service/debug/gdb_server/gdb_server.cc \
unisim/service/debug/debugger/debugger32.cc \
unisim/service/debug/debugger/debugger64.cc \
unisim/service/profiling/addr_profiler/profiler32.cc \
unisim/service/profiling/addr_profiler/profiler64.cc \
unisim/service/os/ti_c_io/ti_c_io_32.cc \
unisim/service/os/ti_c_io/ti_c_io_64.cc \
unisim/service/time/host_time/time.cc \
unisim/service/tee/loader/tee.cc \
unisim/service/tee/symbol_table_lookup/tee_32.cc \
unisim/service/tee/symbol_table_lookup/tee_64.cc \
unisim/service/tee/blob/tee_32.cc \
unisim/service/tee/blob/tee_64.cc \
unisim/service/tee/stmt_lookup/tee_32.cc \
unisim/service/tee/stmt_lookup/tee_64.cc \
unisim/service/tee/backtrace/tee_32.cc \
unisim/service/tee/backtrace/tee_64.cc \
unisim/component/cxx/processor/tms320c3x/cpu_tms320vc33.cc \
unisim/component/cxx/processor/tms320c3x/cpu_tms320vc33_debug.cc \
unisim/component/cxx/processor/tms320c3x/register.cc \
unisim/component/cxx/memory/ram/memory_32.cc \
unisim/component/cxx/memory/ram/memory_64.cc"

UNISIM_LIB_TMS320C3X_ISA_FILES="\
unisim/component/cxx/processor/tms320c3x/isa/2op.isa \
unisim/component/cxx/processor/tms320c3x/isa/3op.isa \
unisim/component/cxx/processor/tms320c3x/isa/actions_dec.isa \
unisim/component/cxx/processor/tms320c3x/isa/control.isa \
unisim/component/cxx/processor/tms320c3x/isa/interlock.isa \
unisim/component/cxx/processor/tms320c3x/isa/load_store.isa \
unisim/component/cxx/processor/tms320c3x/isa/parallel.isa \
unisim/component/cxx/processor/tms320c3x/isa/power.isa \
unisim/component/cxx/processor/tms320c3x/isa/tms320c3x.isa"

UNISIM_LIB_TMS320C3X_HEADER_FILES="${UNISIM_LIB_TMS320C3X_ISA_FILES} \
unisim/kernel/kernel.hh \
unisim/kernel/config/xml_config_file_helper.hh \
unisim/kernel/config/ini_config_file_helper.hh \
unisim/kernel/logger/logger.hh \
unisim/kernel/logger/logger_server.hh \
unisim/util/backtrace/backtrace.hh \
unisim/util/likely/likely.hh \
unisim/util/xml/xml.hh \
unisim/util/debug/memory_access_type.hh \
unisim/util/debug/breakpoint.hh \
unisim/util/debug/event.hh \
unisim/util/debug/register.hh \
unisim/util/debug/symbol.hh \
unisim/util/debug/stmt.hh \
unisim/util/debug/simple_register.hh \
unisim/util/debug/watchpoint_registry.hh \
unisim/util/debug/watchpoint.hh \
unisim/util/debug/breakpoint_registry.hh \
unisim/util/debug/symbol_table.hh \
unisim/util/debug/data_object.hh \
unisim/util/debug/profile.hh \
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
unisim/util/arithmetic/arithmetic.hh \
unisim/util/hash_table/hash_table.hh \
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
unisim/util/inlining/inlining.hh \
unisim/util/nat_sort/nat_sort.hh \
unisim/util/hypapp/hypapp.hh \
unisim/service/interfaces/debug_yielding.hh \
unisim/service/interfaces/debug_event.hh \
unisim/service/interfaces/debug_info_loading.hh \
unisim/service/interfaces/memory_access_reporting.hh \
unisim/service/interfaces/ti_c_io.hh \
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
unisim/service/interfaces/trap_reporting.hh \
unisim/service/interfaces/blob.hh \
unisim/service/interfaces/backtrace.hh \
unisim/service/interfaces/data_object_lookup.hh \
unisim/service/loader/elf_loader/elf_loader.hh \
unisim/service/loader/elf_loader/elf32_loader.hh \
unisim/service/loader/elf_loader/elf64_loader.hh \
unisim/service/loader/raw_loader/raw_loader.hh \
unisim/service/loader/s19_loader/s19_loader.hh \
unisim/service/loader/coff_loader/coff_loader.hh \
unisim/service/loader/multiformat_loader/multiformat_loader.hh \
unisim/service/debug/inline_debugger/inline_debugger.hh \
unisim/service/debug/gdb_server/gdb_server.hh \
unisim/service/debug/debugger/debugger.hh \
unisim/service/profiling/addr_profiler/profiler.hh \
unisim/service/os/ti_c_io/ti_c_io.hh \
unisim/service/time/host_time/time.hh \
unisim/service/tee/loader/tee.hh \
unisim/service/tee/blob/tee.hh \
unisim/service/tee/symbol_table_lookup/tee.hh \
unisim/service/tee/stmt_lookup/tee.hh \
unisim/service/tee/backtrace/tee.hh \
unisim/component/cxx/processor/tms320c3x/config.hh \
unisim/component/cxx/processor/tms320c3x/cpu.hh \
unisim/component/cxx/processor/tms320c3x/exception.hh \
unisim/component/cxx/processor/tms320c3x/register.hh \
unisim/component/cxx/memory/ram/memory.hh"


UNISIM_LIB_TMS320C3X_TEMPLATE_FILES="\
unisim/util/debug/breakpoint_registry.tcc \
unisim/util/debug/watchpoint_registry.tcc \
unisim/util/debug/symbol_table.tcc \
unisim/util/debug/symbol.tcc \
unisim/util/debug/profile.tcc \
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
unisim/service/loader/coff_loader/coff_loader.tcc \
unisim/service/loader/elf_loader/elf_loader.tcc \
unisim/service/loader/elf_loader/elf32_loader.tcc \
unisim/service/loader/elf_loader/elf64_loader.tcc \
unisim/service/loader/raw_loader/raw_loader.tcc \
unisim/service/loader/s19_loader/s19_loader.tcc \
unisim/service/loader/multiformat_loader/multiformat_loader.tcc \
unisim/service/debug/inline_debugger/inline_debugger.tcc \
unisim/service/debug/gdb_server/gdb_server.tcc \
unisim/service/debug/debugger/debugger.tcc \
unisim/service/profiling/addr_profiler/profiler.tcc \
unisim/service/os/ti_c_io/ti_c_io.tcc \
unisim/service/tee/loader/tee.tcc \
unisim/service/tee/symbol_table_lookup/tee.tcc \
unisim/service/tee/blob/tee.tcc \
unisim/service/tee/stmt_lookup/tee.tcc \
unisim/service/tee/backtrace/tee.tcc \
unisim/component/cxx/processor/tms320c3x/cpu.tcc \
unisim/component/cxx/processor/tms320c3x/exception.tcc \
unisim/component/cxx/memory/ram/memory.tcc"

UNISIM_LIB_TMS320C3X_M4_FILES="\
m4/endian.m4 \
m4/times.m4 \
m4/cxxabi.m4 \
m4/libxml2.m4 \
m4/zlib.m4 \
m4/boost_graph.m4 \
m4/bsd_sockets.m4 \
m4/curses.m4 \
m4/libedit.m4 \
m4/with_boost.m4 \
m4/check_lib.m4 \
m4/get_exec_path.m4 \
m4/real_path.m4"

UNISIM_LIB_TMS320C3X_DATA_FILES="\
unisim/service/debug/gdb_server/gdb_tms320c3x.xml \
"

TMS320C3X_EXTERNAL_HEADERS="\
assert.h \
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
sys/types.h \
time.h \
unistd.h \
vector \
"

UNISIM_SIMULATORS_TMS320C3X_SOURCE_FILES="\
main.cc \
"

UNISIM_SIMULATORS_TMS320C3X_HEADER_FILES="\
"

UNISIM_SIMULATORS_TMS320C3X_TEMPLATE_FILES="\
"

UNISIM_SIMULATORS_TMS320C3X_DATA_FILES="\
COPYING \
INSTALL \
NEWS \
README \
AUTHORS \
ChangeLog \
unisim.ico \
template_default_config.xml \
c31boot.out \
fibo.out \
"

# UNISIM_DOCS_FILES="\
# genisslib_manual.pdf \
# tms320c3x_manual.pdf \
# genisslib_manual.tex \
# genisslib/genisslib.tex \
# tms320c3x_manual.tex \
# tms320c3x/tms320c3x.tex \
# tms320c3x/service_instanciation.tex \
# tms320c3x/service_connection.tex \
# tms320c3x/simple_service.tex \
# tms320c3x/simple_client.tex \
# tms320c3x/run_time_parameter.tex \
# tms320c3x/debug_control_interface.tex \
# tms320c3x/disassembly_interface.tex \
# tms320c3x/loader_interface.tex \
# tms320c3x/memory_access_reporting_control_interface.tex \
# tms320c3x/memory_access_reporting_interface.tex \
# tms320c3x/memory_injection_interface.tex \
# tms320c3x/memory_interface.tex \
# tms320c3x/registers_interface.tex \
# tms320c3x/symbol_table_lookup_interface.tex \
# tms320c3x/ti_c_io_interface.tex \
# tms320c3x/time_interface.tex \
# tms320c3x/trap_reporting_interface.tex \
# tms320c3x/including_logger.tex \
# tms320c3x/deriving_from_object.tex \
# tms320c3x/binding_logger.tex \
# tms320c3x/using_logger.tex \
# tms320c3x/register_interface.tex \
# tms320c3x/assembly_pattern.tex \
# tms320c3x/fig_tms320c3x.pdf \
# tms320c3x/fig_object_hierarchy.pdf \
# tms320c3x/fig_code_composer.jpg \
# tms320c3x/fig_tms320c3x_board.jpg \
# tms320c3x/fig_tms320c3x_dev_kit.jpg \
# tms320c3x/fig_unit_test_generator.pdf \
# tms320c3x/fig_unit_test.pdf \
# tms320c3x/fig_driver_error.jpg \
# tms320c3x/fig_fft512_output0.pdf \
# tms320c3x/fig_iir_output.pdf \
# tms320c3x/fig_bp_output.pdf \
# tms320c3x/fig_lp_output.pdf"

has_to_build_configure=no
has_to_build_genisslib_configure=no
has_to_build_tms320c3x_configure=no

mkdir -p ${DEST_DIR}/genisslib
mkdir -p ${DEST_DIR}/tms320c3x
# mkdir -p ${DEST_DIR}/docs

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

UNISIM_LIB_TMS320C3X_FILES="${UNISIM_LIB_TMS320C3X_SOURCE_FILES} ${UNISIM_LIB_TMS320C3X_HEADER_FILES} ${UNISIM_LIB_TMS320C3X_TEMPLATE_FILES} ${UNISIM_LIB_TMS320C3X_DATA_FILES}"

for file in ${UNISIM_LIB_TMS320C3X_FILES}; do
	mkdir -p "${DEST_DIR}/tms320c3x/$(dirname ${file})"
	has_to_copy=no
	if [ -e "${DEST_DIR}/tms320c3x/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt "${DEST_DIR}/tms320c3x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/tms320c3x/${file}"
		cp -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/tms320c3x/${file}" || exit
	fi
done

UNISIM_SIMULATORS_TMS320C3X_FILES="${UNISIM_SIMULATORS_TMS320C3X_SOURCE_FILES} ${UNISIM_SIMULATORS_TMS320C3X_HEADER_FILES} ${UNISIM_SIMULATORS_TMS320C3X_TEMPLATE_FILES} ${UNISIM_SIMULATORS_TMS320C3X_DATA_FILES}"

for file in ${UNISIM_SIMULATORS_TMS320C3X_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/tms320c3x/${file}" ]; then
		if [ "${UNISIM_SIMULATORS_DIR}/${file}" -nt "${DEST_DIR}/tms320c3x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_SIMULATORS_DIR}/${file} ==> ${DEST_DIR}/tms320c3x/${file}"
		${DISTCOPY} -f "${UNISIM_SIMULATORS_DIR}/${file}" "${DEST_DIR}/tms320c3x/${file}" || exit
	fi
done

for file in ${UNISIM_SIMULATORS_TMS320C3X_DATA_FILES}; do
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

# for file in ${UNISIM_DOCS_FILES}; do
# 	mkdir -p "${DEST_DIR}/docs/$(dirname ${file})"
# 	has_to_copy=no
# 	if [ -e "${DEST_DIR}/docs/${file}" ]; then
# 		if [ "${UNISIM_DOCS_DIR}/${file}" -nt "${DEST_DIR}/docs/${file}" ]; then
# 			has_to_copy=yes
# 		fi
# 	else
# 		has_to_copy=yes
# 	fi
# 	if [ "${has_to_copy}" = "yes" ]; then
# 		echo "${UNISIM_DOCS_DIR}/${file} ==> ${DEST_DIR}/docs/${file}"
# 		${DISTCOPY} -f "${UNISIM_DOCS_DIR}/${file}" "${DEST_DIR}/docs/${file}" || exit
# 	fi
# done


mkdir -p ${DEST_DIR}/config
mkdir -p ${DEST_DIR}/tms320c3x/config
mkdir -p ${DEST_DIR}/tms320c3x/m4
mkdir -p ${DEST_DIR}/genisslib/config
mkdir -p ${DEST_DIR}/genisslib/m4
# mkdir -p ${DEST_DIR}/docs/config
# mkdir -p ${DEST_DIR}/docs/m4

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

for file in ${UNISIM_LIB_TMS320C3X_M4_FILES}; do
	has_to_copy=no
	if [ -e "${DEST_DIR}/tms320c3x/${file}" ]; then
		if [ "${UNISIM_LIB_DIR}/${file}" -nt  "${DEST_DIR}/tms320c3x/${file}" ]; then
			has_to_copy=yes
		fi
	else
		has_to_copy=yes
	fi
	if [ "${has_to_copy}" = "yes" ]; then
		echo "${UNISIM_LIB_DIR}/${file} ==> ${DEST_DIR}/tms320c3x/${file}"
		${DISTCOPY} -f "${UNISIM_LIB_DIR}/${file}" "${DEST_DIR}/tms320c3x/${file}" || exit
		has_to_build_tms320c3x_configure=yes
	fi
done

# Top level

cat << EOF > "${DEST_DIR}/README"
This package contains GenISSLib, an instruction set simulator generator, and a TMS320C3X instruction set simulator.
See INSTALL for installation instructions.
See COPYING for licensing.
EOF

cat << EOF > "${DEST_DIR}/INSTALL"
INSTALLATION
------------

Requirements:
  - GNU bash (bash-3.0-19.3 on RHEL4)
  - GNU make (make-3.80-6.EL4 on RHEL4)
  - GNU autoconf (autoconf-2.59-5 on RHEL4)
  - GNU automake (automake-1.9.2-3 on RHEL4)
  - GNU flex (flex-2.5.4a-33 on RHEL4)
  - GNU bison (bison-1.875c-2 on RHEL4)
  - boost development package (boost-devel-1.32.0-1.rhel4 on RHEL4)
  - libxml2 development package (libxml2-devel-2.6.16-6 on RHEL4)
  - zlib development package (zlib-devel-1.2.1.2-1.2 on RHEL4)

Building instructions:
  $ ./configure
  $ make

Installing (optional):
  $ make install
EOF

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
	echo "AC_INIT([UNISIM TMS320C3X Standalone simulator], [${TMS320C3X_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>], [unisim-tms320c3x])" > "${DEST_DIR}/configure.ac"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE" >> "${CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${CONFIGURE_AC}"
	echo "AC_CONFIG_SUBDIRS([genisslib])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_SUBDIRS([tms320c3x])"  >> "${CONFIGURE_AC}" 
# 	echo "AC_CONFIG_SUBDIRS([docs])"  >> "${CONFIGURE_AC}" 
	echo "AC_CONFIG_FILES([Makefile])" >> "${CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${CONFIGURE_AC}"

	echo "Generating Makefile.am"
# 	echo "SUBDIRS=genisslib tms320c3x docs" > "${MAKEFILE_AM}"
	echo "SUBDIRS=genisslib tms320c3x" > "${MAKEFILE_AM}"

	echo "Building configure"
	${SHELL} -c "cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
fi

# Documentation

# DOCS_CONFIGURE_AC="${DEST_DIR}/docs/configure.ac"
# DOCS_MAKEFILE_AM="${DEST_DIR}/docs/Makefile.am"
# 
# 
# if [ ! -e "${DOCS_CONFIGURE_AC}" ]; then
# 	has_to_build_docs_configure=yes
# else
# 	if [ "$0" -nt "${DOCS_CONFIGURE_AC}" ]; then
# 		has_to_build_docs_configure=yes
# 	fi
# fi
# 
# if [ ! -e "${DOCS_MAKEFILE_AM}" ]; then
# 	has_to_build_docs_configure=yes
# else
# 	if [ "$0" -nt "${DOCS_MAKEFILE_AM}" ]; then
# 		has_to_build_docs_configure=yes
# 	fi
# fi
# 
# if [ "${has_to_build_docs_configure}" = "yes" ]; then
# 	echo "Generating docs configure.ac"
# 	echo "AC_INIT([UNISIM Documentation], [${DOCS_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>], [unisim-docs])" > "${DOCS_CONFIGURE_AC}"
# 	echo "AC_CONFIG_AUX_DIR(config)" >> "${DOCS_CONFIGURE_AC}"
# 	echo "AC_CANONICAL_BUILD" >> "${DOCS_CONFIGURE_AC}"
# 	echo "AC_CANONICAL_HOST" >> "${DOCS_CONFIGURE_AC}"
# 	echo "AC_CANONICAL_TARGET" >> "${DOCS_CONFIGURE_AC}"
# 	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${DOCS_CONFIGURE_AC}"
# 	echo "AC_PATH_PROGS(SH, sh)" >> "${DOCS_CONFIGURE_AC}"
# 	echo "AC_PROG_INSTALL" >> "${DOCS_CONFIGURE_AC}"
# 	echo "AC_PROG_LN_S" >> "${DOCS_CONFIGURE_AC}"
# 	echo "AC_CONFIG_FILES([Makefile])" >> "${DOCS_CONFIGURE_AC}"
# 	echo "AC_OUTPUT" >> "${DOCS_CONFIGURE_AC}"
# 
# 	echo "Generating docs Makefile.am"
# 	echo "ACLOCAL_AMFLAGS=-I m4" > "${DOCS_MAKEFILE_AM}"
# 	echo "EXTRA_DIST = ${UNISIM_DOCS_FILES}" >> "${DOCS_MAKEFILE_AM}"
# 	echo "sharedir = \$(prefix)/share/unisim-tms320c3x-${TMS320C3X_VERSION}" >> "${DOCS_MAKEFILE_AM}"
# 	echo "share_DATA = tms320c3x_manual.pdf" >> "${DOCS_MAKEFILE_AM}"
# 
# 	cat << EOF > "${DEST_DIR}/docs/README"
# This is the documentation of UNISIM GenISSLib, an instruction set simulator generator, and the UNISIM TMS320C3X instruction set simulator.
# See INSTALL for installation instructions.
# See COPYING for licensing.
# See tms320c3x_manual.pdf and genisslib_manual.pdf for a printable documentation of these tools.
# EOF
# 
# 	echo "" > "${DEST_DIR}/docs/NEWS"
# 	echo "" > "${DEST_DIR}/docs/ChangeLog"
# 	
# 	cat << EOF > "${DEST_DIR}/docs/AUTHORS"
# Gilles Mouchard <gilles.mouchard@cea.fr>
# Daniel Gracia Pérez <daniel.gracia-perez@cea.fr>
# EOF
# 
# 	cat << EOF > "${DEST_DIR}/docs/INSTALL"
# Requirements:
#  - Latex
#  - rubber
# 
# Build the documentation:
#  $ rubber -d genisslib_manual
#  $ rubber -d tms320c3x_manual
# EOF
# 
# cat << EOF > "${DEST_DIR}/docs/COPYING"
# Copyright (c) 2009,
# Commissariat a l'Energie Atomique (CEA)
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
#  - Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 
#  - Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 
#  - Neither the name of CEA nor the names of its contributors may be used to
#    endorse or promote products derived from this software without specific prior
#    written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# EOF
# 
# 	echo "Building configure"
# 	${SHELL} -c "cd ${DEST_DIR}/docs && aclocal && autoconf --force && automake -ac"
# fi

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

# TMS320C3X

TMS320C3X_CONFIGURE_AC="${DEST_DIR}/tms320c3x/configure.ac"
TMS320C3X_MAKEFILE_AM="${DEST_DIR}/tms320c3x/Makefile.am"


if [ ! -e "${TMS320C3X_CONFIGURE_AC}" ]; then
	has_to_build_tms320c3x_configure=yes
else
	if [ "$0" -nt "${TMS320C3X_CONFIGURE_AC}" ]; then
		has_to_build_tms320c3x_configure=yes
	fi
fi

if [ ! -e "${TMS320C3X_MAKEFILE_AM}" ]; then
	has_to_build_tms320c3x_configure=yes
else
	if [ "$0" -nt "${TMS320C3X_MAKEFILE_AM}" ]; then
		has_to_build_tms320c3x_configure=yes
	fi
fi

if [ "${has_to_build_tms320c3x_configure}" = "yes" ]; then
	echo "Generating TMS320C3X configure.ac"
	echo "AC_INIT([UNISIM TMS320C3X C++ simulator], [${TMS320C3X_VERSION}], [Gilles Mouchard <gilles.mouchard@cea.fr>, Daniel Gracia Perez <daniel.gracia-perez@cea.fr>], [unisim-tms320c3x-core])" > "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CONFIG_MACRO_DIR([m4])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CONFIG_AUX_DIR(config)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CONFIG_HEADERS([config.h])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CANONICAL_BUILD" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CANONICAL_HOST" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CANONICAL_TARGET" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AM_INIT_AUTOMAKE([subdir-objects tar-pax])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_PATH_PROGS(SH, sh)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_PROG_CXX" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_PROG_RANLIB" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_PROG_INSTALL" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_PROG_LN_S" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_LANG([C++])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CHECK_HEADERS([${TMS320C3X_EXTERNAL_HEADERS}],, AC_MSG_ERROR([Some external headers are missing.]))" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ENDIAN(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_TIMES(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_WITH_BOOST(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CURSES(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBEDIT(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BSD_SOCKETS(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_ZLIB(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_LIBXML2(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_BOOST_GRAPH(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_CXXABI(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_GET_EXECUTABLE_PATH(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "UNISIM_CHECK_REAL_PATH(main)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "GENISSLIB_PATH=\$(pwd)/../genisslib/genisslib" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_SUBST(GENISSLIB_PATH)" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_DEFINE([BIN_TO_SHARED_DATA_PATH], [\"../share/unisim-tms320c3x-${TMS320C3X_VERSION}\"], [path of shared data relative to bin directory])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_CONFIG_FILES([Makefile])" >> "${TMS320C3X_CONFIGURE_AC}"
	echo "AC_OUTPUT" >> "${TMS320C3X_CONFIGURE_AC}"

	AM_TMS320C3X_VERSION=$(printf ${TMS320C3X_VERSION} | sed -e 's/\./_/g')
	echo "Generating TMS320C3X Makefile.am"
	echo "ACLOCAL_AMFLAGS=-I m4" > "${TMS320C3X_MAKEFILE_AM}"
	echo "AM_CPPFLAGS=-I\$(top_srcdir) -I\$(top_builddir)" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "noinst_LIBRARIES = libtms320c3x-${TMS320C3X_VERSION}.a" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "libtms320c3x_${AM_TMS320C3X_VERSION}_a_SOURCES = ${UNISIM_LIB_TMS320C3X_SOURCE_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "bin_PROGRAMS = unisim-tms320c3x-${TMS320C3X_VERSION} unisim-tms320c3x-debug-${TMS320C3X_VERSION}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "unisim_tms320c3x_${AM_TMS320C3X_VERSION}_SOURCES = ${UNISIM_SIMULATORS_TMS320C3X_SOURCE_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "unisim_tms320c3x_debug_${AM_TMS320C3X_VERSION}_SOURCES = ${UNISIM_SIMULATORS_TMS320C3X_SOURCE_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "unisim_tms320c3x_debug_${AM_TMS320C3X_VERSION}_CPPFLAGS = -DDEBUG_TMS320C3X" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "unisim_tms320c3x_${AM_TMS320C3X_VERSION}_LDADD = libtms320c3x-${TMS320C3X_VERSION}.a" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "unisim_tms320c3x_debug_${AM_TMS320C3X_VERSION}_LDADD = libtms320c3x-${TMS320C3X_VERSION}.a" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "noinst_HEADERS = ${UNISIM_LIB_TMS320C3X_HEADER_FILES} ${UNISIM_LIB_TMS320C3X_TEMPLATE_FILES} ${UNISIM_SIMULATORS_TMS320C3X_HEADER_FILES} ${UNISIM_SIMULATORS_TMS320C3X_TEMPLATE_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "EXTRA_DIST = ${UNISIM_LIB_TMS320C3X_M4_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "sharedir = \$(prefix)/share/unisim-tms320c3x-${TMS320C3X_VERSION}" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "dist_share_DATA = ${UNISIM_LIB_TMS320C3X_DATA_FILES} ${UNISIM_SIMULATORS_TMS320C3X_DATA_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"

	echo "BUILT_SOURCES=\$(top_srcdir)/unisim/component/cxx/processor/tms320c3x/isa_tms320c3x.hh \$(top_srcdir)/unisim/component/cxx/processor/tms320c3x/isa_tms320c3x.tcc" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "CLEANFILES=\$(top_srcdir)/unisim/component/cxx/processor/tms320c3x/isa_tms320c3x.hh \$(top_srcdir)/unisim/component/cxx/processor/tms320c3x/isa_tms320c3x.tcc" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/tms320c3x/isa_tms320c3x.tcc: \$(top_srcdir)/unisim/component/cxx/processor/tms320c3x/isa_tms320c3x.hh" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "\$(top_srcdir)/unisim/component/cxx/processor/tms320c3x/isa_tms320c3x.hh: ${UNISIM_LIB_TMS320C3X_ISA_FILES}" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\t" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "cd \$(top_srcdir)/unisim/component/cxx/processor/tms320c3x; \$(GENISSLIB_PATH) -o isa_tms320c3x -w 32 -I isa isa/tms320c3x.isa" >> "${TMS320C3X_MAKEFILE_AM}"

	echo "all-local: all-local-bin all-local-share" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "clean-local: clean-local-bin clean-local-share" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "all-local-bin: \$(bin_PROGRAMS)" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\t@PROGRAMS='\$(bin_PROGRAMS)'; \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tfor PROGRAM in \$\${PROGRAMS}; do \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/bin/\$\$(basename \$\${PROGRAM})\"; \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/bin'; \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_builddir)/\$\${PROGRAM}\" \$(top_builddir)/bin/\$\$(basename \"\$\${PROGRAM}\"); \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tdone\n" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "clean-local-bin:" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(bin_PROGRAMS)' ]; then \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/bin'; \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tfi\n" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "all-local-share: \$(dist_share_DATA)" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\t@SHARED_DATAS='\$(dist_share_DATA)'; \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tfor SHARED_DATA in \$\${SHARED_DATAS}; do \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\trm -f \"\$(top_builddir)/share/unisim-tms320c3x-${TMS320C3X_VERSION}/\$\$(basename \$\${SHARED_DATA})\"; \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tmkdir -p '\$(top_builddir)/share/unisim-tms320c3x-${TMS320C3X_VERSION}'; \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tcp -f \"\$(top_srcdir)/\$\${SHARED_DATA}\" \$(top_builddir)/share/unisim-tms320c3x-${TMS320C3X_VERSION}/\$\$(basename \"\$\${SHARED_DATA}\"); \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tdone\n" >> "${TMS320C3X_MAKEFILE_AM}"
	echo "clean-local-share:" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\t@if [ ! -z '\$(dist_share_DATA)' ]; then \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\trm -rf '\$(top_builddir)/share'; \\\\\n" >> "${TMS320C3X_MAKEFILE_AM}"
	printf "\tfi\n" >> "${TMS320C3X_MAKEFILE_AM}"


	echo "Building TMS320C3X configure"
	${SHELL} -c "cd ${DEST_DIR}/tms320c3x && aclocal -I m4 && autoconf --force && autoheader && automake -ac"
fi

echo "Distribution is up-to-date"
