#!/bin/bash
if [ -z "$1" ]; then
	echo "Usage:"
	echo "  $0 <destination directory>"
	exit -1
fi

if [ -z "${SIMPKG}" ]; then
	echo "SIMPKG is not set"
	exit -1
fi

if [ -z "${SIMPKG_SRCDIR}" ]; then
	echo "SIMPKG_SRCDIR is not set"
	exit -1
fi

has_two_levels_dirs=no
genisslib_imported=no
PACKAGE_DIR=$(cd $(dirname $0); pwd)
UNISIM_DIR=$(cd $(dirname $(dirname $0)); pwd)
UNISIM_TOOLS_DIR="${UNISIM_DIR}/unisim_tools"
UNISIM_LIB_DIR="${UNISIM_DIR}/unisim_lib"
UNISIM_SIMULATOR_DIR="${UNISIM_DIR}/unisim_simulators/${SIMPKG_SRCDIR}"
SIMULATOR_VERSION=$(cat "${UNISIM_SIMULATOR_DIR}/VERSION")
AM_SIMULATOR_VERSION=$(printf '%s' "${SIMULATOR_VERSION}" | sed -e 's/\./_/g')
AM_SIMPKG=$(printf '%s' "${SIMPKG}" | sed -e 's/-/_/g')
has_to_build_simulator_configure=no
has_to_build_configure=no

declare -a UNISIM_LIB_PACKAGES
declare -A UNISIM_LIB_PACKAGE_IMPORTED

mkdir -p "$1"
DEST_DIR=$(cd "$1"; pwd)

if [ -z "${DISTCOPY}" ]; then
	DISTCOPY=cp
fi

function import()
{
	local PKG="$1"
	if [ -d "${PACKAGE_DIR}/${PKG}" ]; then
		if [ -z ${UNISIM_LIB_PACKAGE_IMPORTED["${PKG}"]} ]; then
			UNISIM_LIB_PACKAGE_IMPORTED["${PKG}"]=1
			
			local PKG_DEPS_TXT="${PACKAGE_DIR}/${PKG}/pkg_deps.txt"
			
			if [ -e "${PKG_DEPS_TXT}" ]; then
				local DEP_PKG
				while IFS= read -r DEP_PKG; do
					echo "Package \"${PKG}\" requires Packages \"${DEP_PKG}\""
					if ! import "${DEP_PKG}"; then
						echo -e "\033[31mERROR! Missing dependency: Package \"${PKG}\" requires Packages \"${DEP_PKG}\"\033[0m" >&2
						unset UNISIM_LIB_PACKAGE_IMPORTED["${PKG}"]
						return 1
					fi
				done < "${PKG_DEPS_TXT}"
			fi
			echo -e "Importing Package \"${PKG}\"" >&2
			UNISIM_LIB_PACKAGES+=("${PKG}")
		fi
		return 0
	fi

	echo -e "\033[31mERROR! Package \"${PKG}\" is missing\033[0m" >&2
	return 1
}

function files()
{
	while [ $# -ne 0 ]; do
		local LIST_NAME="$1"
		local FOUND='no'
		for PACKAGE in ${UNISIM_LIB_PACKAGES[@]}; do
			LIST_FILENAME="${PACKAGE_DIR}/${PACKAGE}/${LIST_NAME}_list.txt"
			if [ -e "${LIST_FILENAME}" ]; then
				local FILENAME
				while IFS= read -r FILENAME; do
					FOUND='yes'
					echo -n " ${FILENAME}"
				done < "${LIST_FILENAME}"
			fi
		done
	
		if [ "${FOUND}" != 'yes' ]; then
			echo -e "\033[33mWARNING! There are no files in List \"${LIST_NAME}\"\033[0m" >&2
		fi
		
		shift
	done
}

function lines()
{
	while [ $# -ne 0 ]; do
		local LIST_NAME="$1"
		local FOUND='no'
		for PACKAGE in ${UNISIM_LIB_PACKAGES[@]}; do
			LIST_FILENAME="${PACKAGE_DIR}/${PACKAGE}/${LIST_NAME}_list.txt"
			if [ -e "${LIST_FILENAME}" ]; then
				local LINE
				while IFS= read -r LINE; do
					FOUND='yes'
					echo "${LINE}"
				done < "${LIST_FILENAME}"
			fi
		done
		
		if [ "${FOUND}" != 'yes' ]; then
			echo -e "\033[33mWARNING! There are no lines in List \"${LIST_NAME}\"\033[0m" >&2
		fi
		shift
	done
}

function pkg_deps_changed()
{
	for PACKAGE in ${UNISIM_LIB_PACKAGES[@]}; do
		for FILE in ${PACKAGE_DIR}/${PACKAGE}/*; do
			if [ ! -e "$1" -o "${FILE}" -nt "$1" ]; then
				return 0;
			fi
		done
	done
	return 1;
}

function has_to_build() {
	if [ ! -e "$1" -o "$2" -nt "$1" ]; then
		return 0;
	else
		return 1;
	fi
}

function dist_copy() {
	if has_to_build "$2" "$1"; then
		echo "$1 ==> $2"
		mkdir -p "$(dirname $2)"
		${DISTCOPY} -f "$1" "$2" || exit
		return 0
	fi
	return 1
}

function enable_two_levels_dirs()
{
	if [ -z "${SIMPKG_DSTDIR}" ]; then
		echo "SIMPKG_DSTDIR is not set"
		exit -1
	fi
	
	has_two_levels_dirs=yes;
}

function make_pkg_configure_cross()
{
	local CONFIGURE_CROSS="$1"
	cat << EOF_CONFIGURE_CROSS > "${CONFIGURE_CROSS}"
#!/bin/bash
HERE=\$(pwd)
MY_DIR=\$(cd \$(dirname \$0); pwd)

# remove --host from command line arguments
host=""
help=""
i=0
j=0
for arg in "\$@"
do
	case "\${arg}" in
		--host=*)
			host=\$(printf '%s' "\${arg}" | cut -f 2- -d '=')
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
\${MY_DIR}/genisslib/configure --disable-option-checking "\${args[@]}"
STATUS="\$?"
cd "\${HERE}"
if test \${STATUS} -ne 0; then
	exit \${STATUS}
fi

if test "\${help}" = "yes"; then
	echo "=== configure help for ${SIMPKG}"
else
	echo "=== configuring in ${SIMPKG} (\${HERE}/${SIMPKG_DSTDIR}) for \${host} host system type"
	echo "\$(basename \$0): running \${MY_DIR}/${SIMPKG_DSTDIR}/configure \$@"
fi

if test ! -d \${HERE}/${SIMPKG_DSTDIR}; then
	mkdir \${HERE}/${SIMPKG_DSTDIR}
fi
cd \${HERE}/${SIMPKG_DSTDIR}
\${MY_DIR}/${SIMPKG_DSTDIR}/configure "\$@"
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
all: ${SIMPKG}-all
clean: genisslib-clean ${SIMPKG}-clean
distclean: genisslib-distclean ${SIMPKG}-distclean
	rm -f \${HERE}/Makefile.cross
install: ${SIMPKG}-install

genisslib-all:
	@\\\$(MAKE) -C \${HERE}/genisslib all
${SIMPKG}-all: genisslib-all
	@\\\$(MAKE) -C \${HERE}/${SIMPKG_DSTDIR} all
genisslib-clean:
	@\\\$(MAKE) -C \${HERE}/genisslib clean
${SIMPKG}-clean:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG_DSTDIR} clean
genisslib-distclean:
	@\\\$(MAKE) -C \${HERE}/genisslib distclean
${SIMPKG}-distclean:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG_DSTDIR} distclean
${SIMPKG}-install:
	@\\\$(MAKE) -C \${HERE}/${SIMPKG_DSTDIR} install
EOF_MAKEFILE_CROSS

chmod +x Makefile.cross

echo "\$(basename \$0): run 'make -f \${HERE}/Makefile.cross' or '\${HERE}/Makefile.cross' to build for \${host} host system type"
EOF_CONFIGURE_CROSS
	chmod +x "${CONFIGURE_CROSS}"
}

function copy()
{
	local status=0
	for LIST_NAME in "$@"; do
		for FILE in $(files ${LIST_NAME}); do
			if [ "${has_two_levels_dirs}" = "yes" ]; then
				if ! dist_copy "${UNISIM_LIB_DIR}/${FILE}" "${DEST_DIR}/${SIMPKG_DSTDIR}/${FILE}"; then
					status=1
				fi
			else
				if ! dist_copy "${UNISIM_LIB_DIR}/${FILE}" "${DEST_DIR}/${FILE}"; then
					status=1
				fi
			fi
		done
	done
	return ${status}
}

function import_genisslib()
{
	enable_two_levels_dirs

	if ! GILINSTALL=noinst ${UNISIM_DIR}/package/dist_genisslib.sh ${DEST_DIR}/genisslib; then
		return 1
	fi
	genisslib_imported=yes

	return 0
}

function output_top_configure_ac()
{
	if [ "${has_two_levels_dirs}" = "no" ]; then
		echo "Please remove useless call to output_top_configure_ac"
		exit
	fi

	local CONFIGURE_AC="${DEST_DIR}/configure.ac"
	if has_to_build "${CONFIGURE_AC}" "$0" || \
	   has_to_build "${CONFIGURE_AC}" "${UNISIM_SIMULATOR_DIR}/VERSION" || \
	   has_to_build "${CONFIGURE_AC}" "${PACKAGE_DIR}/dist_common.sh"; then
		echo "Generating configure.ac"
		cat "$1" > "${CONFIGURE_AC}"
		has_to_build_configure=yes
	fi
}

function output_top_makefile_am()
{
	if [ "${has_two_levels_dirs}" = "no" ]; then
		echo "Please remove useless call to output_top_makefile_am"
		exit
	fi

	local MAKEFILE_AM="${DEST_DIR}/Makefile.am"
	if has_to_build "${MAKEFILE_AM}" "$0" || \
	   has_to_build "${MAKEFILE_AM}" "${UNISIM_SIMULATOR_DIR}/VERSION" || \
	   has_to_build "${MAKEFILE_AM}" "${PACKAGE_DIR}/dist_common.sh"; then
		echo "Generating Makefile.am"
		cat "$1" > "${MAKEFILE_AM}"
		has_to_build_configure=yes
	fi
}

function build_top_configure()
{
	if [ "${has_two_levels_dirs}" = "no" ]; then
		echo "Please remove useless call to build_top_configure"
		exit
	fi

	mkdir -p "${DEST_DIR}/config"
	
	local CONFIGURE="${DEST_DIR}/configure"
	if has_to_build "${CONFIGURE}" "${PACKAGE_DIR}/dist_common.sh"; then
		has_to_build_configure=yes
	fi
	
	if [ "${has_to_build_configure}" = "yes" ]; then
		echo "Building configure"
		local CMD="cd ${DEST_DIR} && aclocal && autoconf --force && automake -ac"
		echo "Running: ${CMD}"
		${SHELL} -c "${CMD}"
	fi
}

function build_top_configure_cross()
{
	if [ "${has_two_levels_dirs}" = "no" ]; then
		echo "Please remove useless call to build_top_configure_cross"
		exit
	fi

	local CONFIGURE_CROSS="${DEST_DIR}/configure.cross"
	if has_to_build "${CONFIGURE_CROSS}" "$0" || \
	   has_to_build "${CONFIGURE_CROSS}" "${CONFIGURE_CROSS}/dist_common.sh"; then
		echo "Building configure.cross"
		make_pkg_configure_cross "${CONFIGURE_CROSS}"
	fi  # has to build configure cross
}

function output_simulator_configure_ac()
{
	local SIMULATOR_CONFIGURE_AC
	if [ "${has_two_levels_dirs}" = "yes" ]; then
		SIMULATOR_CONFIGURE_AC="${DEST_DIR}/${SIMPKG_DSTDIR}/configure.ac"
	else
		SIMULATOR_CONFIGURE_AC="${DEST_DIR}/configure.ac"
	fi
	if has_to_build "${SIMULATOR_CONFIGURE_AC}" "$0" || \
	   pkg_deps_changed "${SIMULATOR_CONFIGURE_AC}" || \
	   has_to_build "${SIMULATOR_CONFIGURE_AC}" "${UNISIM_SIMULATOR_DIR}/VERSION" || \
	   has_to_build "${SIMULATOR_CONFIGURE_AC}" "${PACKAGE_DIR}/dist_common.sh"; then
		echo "Generating ${SIMPKG} configure.ac"
		cat "$1" > "${SIMULATOR_CONFIGURE_AC}"
		has_to_build_simulator_configure=yes
	fi
}

function output_simulator_makefile_am()
{
	local SIMULATOR_MAKEFILE_AM
	if [ "${has_two_levels_dirs}" = "yes" ]; then
		SIMULATOR_MAKEFILE_AM="${DEST_DIR}/${SIMPKG_DSTDIR}/Makefile.am"
	else
		SIMULATOR_MAKEFILE_AM="${DEST_DIR}/Makefile.am"
	fi
	
	if has_to_build "${SIMULATOR_MAKEFILE_AM}" "$0" || \
	   pkg_deps_changed "${SIMULATOR_MAKEFILE_AM}" || \
	   has_to_build "${SIMULATOR_MAKEFILE_AM}" "${UNISIM_SIMULATOR_DIR}/VERSION" || \
	   has_to_build "${SIMULATOR_MAKEFILE_AM}" "${PACKAGE_DIR}/dist_common.sh"; then
		echo "Generating ${SIMPKG} Makefile.am"
		cat "$1" > "${SIMULATOR_MAKEFILE_AM}"
		has_to_build_simulator_configure=yes
	fi
}

function build_simulator_configure()
{
	local SIMULATOR_CONFIG
	local SIMULATOR_CONFIGURE_AC
	local SIMULATOR_CONFIGURE
	local SIMULATOR_DIR
	if [ "${has_two_levels_dirs}" = "yes" ]; then
		SIMULATOR_CONFIG="${DEST_DIR}/${SIMPKG_DSTDIR}/config"
		SIMULATOR_CONFIGURE_AC="${DEST_DIR}/${SIMPKG_DSTDIR}/configure.ac"
		SIMULATOR_CONFIGURE="${DEST_DIR}/${SIMPKG_DSTDIR}/configure"
		SIMULATOR_DIR="${DEST_DIR}/${SIMPKG_DSTDIR}"
	else
		SIMULATOR_CONFIG="${DEST_DIR}/config"
		SIMULATOR_CONFIGURE_AC="${DEST_DIR}/configure.ac"
		SIMULATOR_CONFIGURE="${DEST_DIR}/configure"
		SIMULATOR_DIR="${DEST_DIR}"
	fi
	
	mkdir -p "${SIMULATOR_CONFIG}"
	
	if has_to_build "${SIMULATOR_CONFIGURE}" "${PACKAGE_DIR}/dist_common.sh"; then
		has_to_build_simulator_configure=yes
	fi
	
	if [ "${has_to_build_simulator_configure}" = "yes" ]; then
		echo "Building ${SIMPKG} configure"
		local CMD="cd '${SIMULATOR_DIR}' && aclocal -I m4"
		# optionally run libtoolize only if LT_INIT is present in configure.ac
		if grep -qs "LT_INIT" "${SIMULATOR_CONFIGURE_AC}"; then
			CMD+=" && libtoolize --force"
		fi
		CMD+=" && autoconf --force && autoheader && automake -ac"
		echo "Running: ${CMD}"
		${SHELL} -c "${CMD}"
	fi
}
