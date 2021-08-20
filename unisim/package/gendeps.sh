#!/bin/bash
# setting INCLUDE_UNTRACKED_FILES=yes make that script generate dependencies for untracked files too

# if [ -z "$1" ]; then
# 	echo "Usage: $0 <directory>"
# 	echo "Example: $0 unisim/kernel"
# 	exit
# fi

shopt -s nullglob

UNISIM_DIR=$(cd $(dirname $(cd $(dirname $0); pwd)); pwd)
PACKAGE_DIR="${UNISIM_DIR}/package"
UNISIM_LIB_DIR="${UNISIM_DIR}/unisim_lib"

function discover_file_deps
{
	local BASE="$1"
	local FILENAME="$2"
	
	local DIRPATH
	if [ -z "${BASE}" ]; then
		DIRPATH="${UNISIM_LIB_DIR}"
	else
		DIRPATH="${UNISIM_LIB_DIR}/${BASE}"
	fi
	
	local FILEPATH="${DIRPATH}/${FILENAME}"
	
	if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${DIRPATH}/${FILENAME}" &> /dev/null; then
		mkdir -p "${PACKAGE_DIR}/${BASE}"
		local cc_regex='\.cc$|\.cpp$'
		local hh_regex='\.hh$|\.h$\.hpp$'
		local tcc_regex='\.tcc$|\.tpp$'
		local isa_regex='\.isa$'
		local interfaces_regex='^unisim/service/interfaces'
		
		local PKG_DEPS_TXT="${PACKAGE_DIR}/${BASE}/pkg_deps.txt"
		
		if [[ "${FILEPATH}" =~ ${cc_regex} ]] || [[ "${FILEPATH}" =~ ${hh_regex} ]] || [[ "${FILEPATH}" =~ ${tcc_regex} ]] || [[ "${FILEPATH}" =~ ${isa_regex} ]]; then
			local INCLUDE_DEP
			for INCLUDE_DEP in $(grep -E '^#[[:blank:]]*include[[:blank:]]*[<"]unisim/' "${FILEPATH}" | sed -e 's/^#[[:blank:]]*include[[:blank:]]*"//g' -e 's/^#[[:blank:]]*include[[:blank:]]*<//g' -e 's/".*$//g' -e 's/>.*$//g'); do
				local PKG_DEP
				if [[ "${INCLUDE_DEP}" =~ ${interfaces_regex} ]]; then
					PKG_DEP=$(echo "${INCLUDE_DEP}" | sed -e 's/\.hh$//')
				else
					PKG_DEP=$(dirname "${INCLUDE_DEP}")
				fi
				
				if [ "${BASE}" != "${PKG_DEP}" ]; then
					echo "${PKG_DEP}" >> "${PKG_DEPS_TXT}"
				fi
			done
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<systemc>' "${FILEPATH}"; then
				echo "m4/systemc" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<sys/socket\.h>' "${FILEPATH}"; then
				echo "m4/bsd_sockets" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<sys/times\.h>' "${FILEPATH}"; then
				echo "m4/times" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cxxabi\.h>' "${FILEPATH}"; then
				echo "m4/cxxabi" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<libxml/.*\.h>' "${FILEPATH}"; then
				echo "m4/libxml2" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<dlfcn\.h>' "${FILEPATH}"; then
				echo "m4/get_exec_path" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<tvs/.*\.h>' "${FILEPATH}"; then
				echo "m4/tvs" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<editline/.*\.h>' "${FILEPATH}"; then
				echo "m4/libedit" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<pthread\.h>' "${FILEPATH}"; then
				echo "m4/pthread" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<Python\.h>' "${FILEPATH}"; then
				echo "m4/python_dev" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<boost/.*\.hpp>' "${FILEPATH}"; then
				echo "m4/boost" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cacti.*\.hh>' "${FILEPATH}"; then
				echo "m4/cacti" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<SDL\.h>' "${FILEPATH}"; then
				echo "m4/sdl" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<artimon\.h>' "${FILEPATH}"; then
				echo "m4/artimon" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<artimon\.h>' "${FILEPATH}"; then
				echo "m4/artimon" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<lua\.h>' "${FILEPATH}"; then
				echo "m4/lua" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<assert\.h>' "${FILEPATH}"; then
				echo "libc/assert" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<ctype\.h>' "${FILEPATH}"; then
				echo "libc/ctype" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<errno\.h>' "${FILEPATH}"; then
				echo "libc/errno" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<fcntl\.h>' "${FILEPATH}"; then
				echo "libc/fcntl" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<fenv\.h>' "${FILEPATH}"; then
				echo "libc/fenv" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<float\.h>' "${FILEPATH}"; then
				echo "libc/float" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<getopt\.h>' "${FILEPATH}"; then
				echo "libc/getopt" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<inttypes\.h>' "${FILEPATH}"; then
				echo "libc/inttypes" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<limits\.h>' "${FILEPATH}"; then
				echo "libc/limits" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<math\.h>' "${FILEPATH}"; then
				echo "libc/math" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<signal\.h>' "${FILEPATH}"; then
				echo "libc/signal" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<stdarg\.h>' "${FILEPATH}"; then
				echo "libc/stdarg" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<stdio\.h>' "${FILEPATH}"; then
				echo "libc/stdio" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<stdlib\.h>' "${FILEPATH}"; then
				echo "libc/stdlib" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<string\.h>' "${FILEPATH}"; then
				echo "libc/string" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<sys/types\.h>' "${FILEPATH}"; then
				echo "sys/types" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<unistd\.h>' "${FILEPATH}"; then
				echo "libc/unistd" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<stdint\.h>' "${FILEPATH}"; then
				echo "libc/stdint" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<time\.h>' "${FILEPATH}"; then
				echo "libc/time" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<dirent\.h>' "${FILEPATH}"; then
				echo "libc/dirent" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<fstream>' "${FILEPATH}"; then
				echo "std/fstream" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cassert>' "${FILEPATH}"; then
				echo "std/cassert" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cmath>' "${FILEPATH}"; then
				echo "std/cmath" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cerrno>' "${FILEPATH}"; then
				echo "std/cerrno" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cstddef>' "${FILEPATH}"; then
				echo "std/cstddef" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cstdio>' "${FILEPATH}"; then
				echo "std/cstdio" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cstdlib>' "${FILEPATH}"; then
				echo "std/cstdlib" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cstring>' "${FILEPATH}"; then
				echo "std/cstring" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<iomanip>' "${FILEPATH}"; then
				echo "std/iomanip" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<stdexcept>' "${FILEPATH}"; then
				echo "std/stdexcept" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<deque>' "${FILEPATH}"; then
				echo "std/deque" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<list>' "${FILEPATH}"; then
				echo "std/list" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<sstream>' "${FILEPATH}"; then
				echo "std/sstream" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<iosfwd>' "${FILEPATH}"; then
				echo "std/iosfwd" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<iostream>' "${FILEPATH}"; then
				echo "std/iostream" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<stack>' "${FILEPATH}"; then
				echo "std/stack" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<map>' "${FILEPATH}"; then
				echo "std/map" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<ostream>' "${FILEPATH}"; then
				echo "std/ostream" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<queue>' "${FILEPATH}"; then
				echo "std/queue" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<vector>' "${FILEPATH}"; then
				echo "std/vector" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<string>' "${FILEPATH}"; then
				echo "std/string" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<set>' "${FILEPATH}"; then
				echo "std/set" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<algorithm>' "${FILEPATH}"; then
				echo "std/algorithm" >> "${PKG_DEPS_TXT}"
			fi
			if grep -qs -E '^#[[:blank:]]*include[[:blank:]]*<cctype>' "${FILEPATH}"; then
				echo "std/cctype" >> "${PKG_DEPS_TXT}"
			fi
		fi
		
		if [[ "${FILEPATH}" =~ ${isa_regex} ]]; then
			local INCLUDE_DEP
			for INCLUDE_DEP in $(grep -E '^[[:blank:]]*include[[:blank:]]*"unisim/' "${FILEPATH}" | sed -e 's/^[[:blank:]]*include[[:blank:]]*"//g' -e 's/".*$//g'); do
				local PKG_DEP
				if [[ "${INCLUDE_DEP}" =~ ${interfaces_regex} ]]; then
					PKG_DEP=$(echo "${INCLUDE_DEP}" | sed -e 's/\.hh$//')
				else
					PKG_DEP=$(dirname "${INCLUDE_DEP}")
				fi
				
				if [ "${BASE}" != "${PKG_DEP}" ]; then
					echo "${PKG_DEP}" >> "${PKG_DEPS_TXT}"
				fi
			done
		fi
		if [ -e "${PKG_DEPS_TXT}" ]; then
			sort -u -o "${PKG_DEPS_TXT}" "${PKG_DEPS_TXT}"
		fi
	fi
}

function list_files
{
	local BASE="$1"
	shift
	local LIST_NAME="$1"
	shift
	local FILENAME_LIST=("$@")
	
	mkdir -p "${PACKAGE_DIR}/${BASE}"
	
	if [ ${#FILENAME_LIST[@]} -ne 0 ]; then
		printf "" > "${PACKAGE_DIR}/${BASE}/${LIST_NAME}_list.txt"
		local FILENAME
		for FILENAME in "${FILENAME_LIST[@]}"; do
			if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${UNISIM_LIB_DIR}/${BASE}/${FILENAME}" &> /dev/null; then
				echo "${BASE}/${FILENAME}" >> "${PACKAGE_DIR}/${BASE}/${LIST_NAME}_list.txt"
			fi
		done
	fi
}

function list_interfaces_files()
{
	cd "${UNISIM_LIB_DIR}/unisim/service/interfaces"
	local HEADER_FILENAME
	for HEADER_FILENAME in *.hh; do
		if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${UNISIM_LIB_DIR}/unisim/service/interfaces/${HEADER_FILENAME}" &> /dev/null; then
			local PKG_NAME=$(echo "${HEADER_FILENAME}" | sed -e 's/\.hh$//')
			mkdir -p "${PACKAGE_DIR}/unisim/service/interfaces/${PKG_NAME}"
			echo "unisim/service/interfaces/${HEADER_FILENAME}" > "${PACKAGE_DIR}/unisim/service/interfaces/${PKG_NAME}/header_list.txt"
		fi
	done
}

function discover_interfaces_file_deps()
{
	cd "${UNISIM_LIB_DIR}/unisim/service/interfaces"
	local interfaces_regex='^unisim/service/interfaces'
	local HEADER_FILENAME
	for HEADER_FILENAME in *.hh *.h *.hpp; do
		if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${UNISIM_LIB_DIR}/unisim/service/interfaces/${HEADER_FILENAME}" &> /dev/null; then
			local PKG_NAME=$(echo "${HEADER_FILENAME}" | sed -e 's/\.hh$//')
			local PKG_DEPS_TXT="${PACKAGE_DIR}/unisim/service/interfaces/${PKG_NAME}/pkg_deps.txt"
			printf "" > "${PKG_DEPS_TXT}"
			local INCLUDE_DEP
			for INCLUDE_DEP in $(grep -E '^#[[:blank:]]*include' "${HEADER_FILENAME}" | grep "unisim/" | sed -e 's/^#[[:blank:]]*include[[:blank:]]*"//g' -e 's/^#[[:blank:]]*include[[:blank:]]*<//g' -e 's/".*$//g' -e 's/>.*$//g'); do
				local PKG_DEP
				if [[ "${INCLUDE_DEP}" =~ ${interfaces_regex} ]]; then
					PKG_DEP=$(echo "${INCLUDE_DEP}" | sed -e 's/\.hh$//')
				else
					PKG_DEP=$(dirname "${INCLUDE_DEP}")
				fi
				
				if [ "${BASE}" != "${PKG_DEP}" ]; then
					echo "${PKG_DEP}" >> "${PKG_DEPS_TXT}"
				fi
			done
		fi
	done
}

function crawl_directory
{
	local SAVE_PWD=${PWD}
	local BASE="$1"
	
	if [ -n "${BASE}" ]; then
		local PKG_DEPS_TXT="${PACKAGE_DIR}/${BASE}/pkg_deps.txt"
		if [ -e "${PKG_DEPS_TXT}" ]; then
			rm -f "${PKG_DEPS_TXT}"
		fi
	fi
	
	local DIRPATH
	if [ -z "${BASE}" ]; then
		DIRPATH="${UNISIM_LIB_DIR}"
	else
		DIRPATH="${UNISIM_LIB_DIR}/${BASE}"
	fi
	
	cd "${DIRPATH}"
	
	if [ "${BASE}" = "unisim/service/interfaces" ]; then
		list_interfaces_files
		discover_interfaces_file_deps
	else
		local SOURCE_LIST=(*.cc *.cpp)
		list_files "${BASE}" source "${SOURCE_LIST[@]}"
		local HEADER_LIST=(*.hh *.h *.hpp)
		list_files "${BASE}" header "${HEADER_LIST[@]}"
		local TEMPLATE_LIST=(*.tcc *.tpp)
		list_files "${BASE}" template "${TEMPLATE_LIST[@]}"
		local ISA_LIST=(*.isa)
		list_files "${BASE}" isa "${ISA_LIST[@]}"
		local DATA_LIST=(*.xml *.svg *.css *.js *.json *.ini *.ico *.png)
		list_files "${BASE}" data "${DATA_LIST[@]}"

		local FILENAME
		for FILENAME in *; do
			local FILEPATH="${DIRPATH}/${FILENAME}"
			if [ -d "${FILEPATH}" ]; then
				if [ "${FILENAME}" = "attic" ]; then
					continue
				fi
				if [ "${INCLUDE_UNTRACKED_FILES}" = "yes" ] || git ls-files --error-unmatch "${FILENAME}" &> /dev/null; then
					if [ -z "${BASE}" ]; then
						crawl_directory "${FILENAME}" &
					else
						crawl_directory "${BASE}/${FILENAME}" &
					fi
				fi
			elif [ -f "${FILEPATH}" ]; then
				discover_file_deps "${BASE}" "${FILENAME}"
			fi
		done
		
		wait
	fi
	cd "${SAVE_PWD}"
}

mkdir -p "${PACKAGE_DIR}/m4"

mkdir -p "${PACKAGE_DIR}/m4/times"
echo "m4/times.m4" > "${PACKAGE_DIR}/m4/times/m4_list.txt"
echo "UNISIM_CHECK_TIMES" > "${PACKAGE_DIR}/m4/times/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/endian"
echo "m4/endian.m4" > "${PACKAGE_DIR}/m4/endian/m4_list.txt"
echo "UNISIM_CHECK_ENDIAN" > "${PACKAGE_DIR}/m4/endian/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/cxxabi"
echo "m4/cxxabi.m4" > "${PACKAGE_DIR}/m4/cxxabi/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/cxxabi/pkg_deps.txt"
echo "UNISIM_CHECK_CXXABI" > "${PACKAGE_DIR}/m4/cxxabi/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/libxml2"
echo "m4/libxml2.m4" > "${PACKAGE_DIR}/m4/libxml2/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/libxml2/pkg_deps.txt"
echo "m4/zlib" > "${PACKAGE_DIR}/m4/libxml2/pkg_deps.txt"
echo "UNISIM_CHECK_LIBXML2" > "${PACKAGE_DIR}/m4/libxml2/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/zlib"
echo "m4/zlib.m4" > "${PACKAGE_DIR}/m4/zlib/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/zlib/pkg_deps.txt"
echo "UNISIM_CHECK_ZLIB" > "${PACKAGE_DIR}/m4/zlib/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/bsd_sockets"
echo "m4/bsd_sockets.m4" > "${PACKAGE_DIR}/m4/bsd_sockets/m4_list.txt"
echo "UNISIM_CHECK_BSD_SOCKETS" > "${PACKAGE_DIR}/m4/bsd_sockets/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/curses"
echo "m4/curses.m4" > "${PACKAGE_DIR}/m4/curses/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/curses/pkg_deps.txt"

mkdir -p "${PACKAGE_DIR}/m4/libedit"
echo "m4/libedit.m4" > "${PACKAGE_DIR}/m4/libedit/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/libedit/pkg_deps.txt"
echo "UNISIM_CHECK_LIBEDIT" > "${PACKAGE_DIR}/m4/libedit/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/systemc"
echo "m4/systemc.m4" > "${PACKAGE_DIR}/m4/systemc/m4_list.txt"
echo "UNISIM_CHECK_SYSTEMC" > "${PACKAGE_DIR}/m4/systemc/ac_list.txt"
echo "m4/pthread" > "${PACKAGE_DIR}/m4/systemc/pkg_deps.txt"

mkdir -p "${PACKAGE_DIR}/m4/check_lib"
echo "m4/check_lib.m4" > "${PACKAGE_DIR}/m4/check_lib/m4_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/get_exec_path"
echo "m4/get_exec_path.m4" > "${PACKAGE_DIR}/m4/get_exec_path/m4_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/get_exec_path/pkg_deps.txt"
echo "UNISIM_CHECK_GET_EXECUTABLE_PATH" > "${PACKAGE_DIR}/m4/get_exec_path/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/real_path"
echo "m4/real_path.m4" > "${PACKAGE_DIR}/m4/real_path/m4_list.txt"
echo "UNISIM_CHECK_REAL_PATH" > "${PACKAGE_DIR}/m4/real_path/ac_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/real_path/pkg_deps.txt"

mkdir -p "${PACKAGE_DIR}/m4/pthread"
echo "m4/pthread.m4" > "${PACKAGE_DIR}/m4/pthread/m4_list.txt"
echo "UNISIM_CHECK_PTHREAD" > "${PACKAGE_DIR}/m4/pthread/ac_list.txt"
echo "m4/check_lib" > "${PACKAGE_DIR}/m4/pthread/pkg_deps.txt"

mkdir -p "${PACKAGE_DIR}/m4/tvs"
echo "m4/tvs.m4" > "${PACKAGE_DIR}/m4/tvs/m4_list.txt"
echo "UNISIM_CHECK_TVS" > "${PACKAGE_DIR}/m4/tvs/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/boost"
echo "m4/boost.m4" > "${PACKAGE_DIR}/m4/boost/m4_list.txt"
echo "UNISIM_CHECK_BOOST(1.53.0)" > "${PACKAGE_DIR}/m4/boost/ac_list.txt"

mkdir -p "${PACKAGE_DIR}/m4/cacti"
echo "m4/cacti.m4" > "${PACKAGE_DIR}/m4/cacti/m4_list.txt"
echo "UNISIM_CHECK_CACTI" > "${PACKAGE_DIR}/m4/cacti/ac_list.txt"

rm -rf "${PACKAGE_DIR}/unisim"
crawl_directory "unisim"

echo "unisim/component/cxx/processor/arm/isa/arm32" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb2" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav7/pkg_deps.txt"

echo "unisim/component/cxx/processor/arm/isa/arm32" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pmsav7/pkg_deps.txt"
echo "unisim/component/cxx/processor/arm/isa/thumb2" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pmsav7/pkg_deps.txt"

echo "unisim/component/cxx/processor/arm/isa/arm64" >> "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/vmsav8/pkg_deps.txt"

echo "m4/endian" >> "${PACKAGE_DIR}/unisim/util/endian/pkg_deps.txt"
echo "m4/real_path" >> "${PACKAGE_DIR}/unisim/kernel/pkg_deps.txt"

sed -i '/unisim\/component\/cxx\/processor\/arm\/cache/d' "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/pkg_deps.txt"

mv "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/arm32/isa_arm32_list.txt"
mv "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb/isa_thumb_list.txt"
mv "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb2/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/arm/isa/thumb2/isa_thumb_list.txt"

mv "${PACKAGE_DIR}/unisim/component/cxx/processor/powerpc/isa/book_vle/isa_list.txt" "${PACKAGE_DIR}/unisim/component/cxx/processor/powerpc/isa/book_vle/isa_vle_list.txt"

sed -i '/unisim\/component\/cxx\/processor\/hcs12x\/xb\.isa/d' "${PACKAGE_DIR}/unisim/component/cxx/processor/hcs12x/isa_list.txt"
sed -i '/unisim\/component\/cxx\/processor\/hcs12x\/s12xgate\.isa/d' "${PACKAGE_DIR}/unisim/component/cxx/processor/hcs12x/isa_list.txt"

echo "unisim/component/cxx/processor/hcs12x/xb.isa" >  "${PACKAGE_DIR}/unisim/component/cxx/processor/hcs12x/isa_xb_list.txt"
echo "unisim/component/cxx/processor/hcs12x/s12xgate.isa" >  "${PACKAGE_DIR}/unisim/component/cxx/processor/hcs12x/isa_s12xgate_list.txt"
