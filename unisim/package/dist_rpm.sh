#!/bin/bash
HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

#NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`

function usage
{
	echo "Usage: `basename $0` <genisslib version> <ppcemu version> <ppcemu-system version> <embedded-ppc-g4-board version> <systemc> <tlm2.0>"
}

function BuildRPM
{
	NAME="$1"
	VERSION="$2"
	RELEASE="$3"
	URL="$4"
	LICENSE="$5"
	SUMMARY="$6"
	DESCRIPTION="$7"
	GROUP="$8"
	FILES="$9"
	NUM_FIXED_ARGS=9
	
	declare -a args
	i=0
	j=0
	for arg in "$@"
	do
			if test ${i} -ge ${NUM_FIXED_ARGS}; then
					args[${j}]=${arg}
					j=$((${j}+1))
			fi
			i=$((${i}+1))
	done

	TOP_DIR="${HOME}/tmp/rpm"

	if [ -d "${TOP_DIR}" ]; then
		echo "ERROR! Directory ${TOP_DIR} already exists. Please remove ${TOP_DIR} before."
		exit -1
	fi

	SPEC="${TOP_DIR}/SPECS/${NAME}.spec"

	mkdir -p ${TOP_DIR}/{BUILD,RPMS/{i586,noarch,x86_64},SOURCES,SRPMS,SPECS,tmp} 
	cp -f "${NAME}-${VERSION}.tar.gz" "${TOP_DIR}/SOURCES"

	echo "%define _topdir     ${HOME}/tmp/rpm" > "${SPEC}"
	echo "%define name        ${NAME}" >> "${SPEC}"
	echo "%define release     ${RELEASE}" >> "${SPEC}"
	echo "%define version     ${VERSION}" >> "${SPEC}"
	echo "%define buildroot %{_topdir}/%{name}-%{version}-root" >> "${SPEC}"

	echo "BuildRoot: %{buildroot}" >> "${SPEC}"
	echo "Summary:   ${SUMMARY}" >> "${SPEC}"
	echo "License:   ${LICENSE}" >> "${SPEC}"
	echo "Name:      %{name}" >> "${SPEC}"
	echo "Version:   %{version}" >> "${SPEC}"
	echo "Release:   %{release}" >> "${SPEC}"
	echo "Source:    %{name}-%{version}.tar.gz" >> "${SPEC}"
	echo "Prefix:    /usr" >> "${SPEC}"
	echo "Group:     ${GROUP}" >> "${SPEC}"
	echo "URL:       ${URL}" >> "${SPEC}"

	echo "%description" >> "${SPEC}"
	echo "${DESCRIPTION}" >> "${SPEC}"

	echo "%prep" >> "${SPEC}"
	echo "%setup -q" >> "${SPEC}"


	echo "%build" >> "${SPEC}"
	echo "./configure ""${args[@]}" >> "${SPEC}"
	echo "%make" >> "${SPEC}"

	echo "%install" >> "${SPEC}"
	echo "make install prefix=\$RPM_BUILD_ROOT/usr" >> "${SPEC}"

	echo "%files" >> "${SPEC}"
	echo "%defattr(-,root,root)" >> "${SPEC}"
	printf "${FILES}" | sed -e 's/ /\n/g' >> "${SPEC}"

	rpmbuild -v -bb --clean "${SPEC}" || exit -1

	mv -f ${TOP_DIR}/RPMS/*/${NAME}-${VERSION}.*.rpm "${HERE}"

	rm -rf "${TOP_DIR}"
}

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ] || [ -z "$6" ]; then
	usage
	exit -1
fi

GENISSLIB_VERSION="$1"
PPCEMU_VERSION="$2"
PPCEMU_SYSTEM_VERSION="$3"
EMBEDDED_PPC_G4_BOARD_VERSION="$4"
SYSTEMC="$5"
TLM20="$6"

BuildRPM \
	"genisslib" \
	"${GENISSLIB_VERSION}" \
	"1" \
	"http://www.unisim-vp.com" \
	"GPL" \
	"UNISIM genisslib" \
	"UNISIM Instruction set simulator generator" \
	"Development/Tools" \
	"/usr/bin/genisslib" \
	"'CXXFLAGS=-O3 -g'"

BuildRPM \
	"ppcemu" \
	"${PPCEMU_VERSION}" \
	"1" \
	"http://www.unisim-vp.com" \
	"BSD" \
	"UNISIM ppcemu" \
	"UNISIM ppcemu is user level PowerPC simulator with support of ELF32 binaries and Linux system call translation." \
	"Development/Tools" \
	"/usr/bin/ppcemu \
	/usr/bin/ppcemu-debug \
	/usr/share/ppcemu/AUTHORS \
	/usr/share/ppcemu/COPYING \
	/usr/share/ppcemu/ChangeLog \
	/usr/share/ppcemu/INSTALL \
	/usr/share/ppcemu/NEWS \
	/usr/share/ppcemu/README \
	/usr/share/ppcemu/gdb_powerpc.xml" \
	"--with-systemc=${SYSTEMC}" \
	"'CXXFLAGS=-O3 -g'"

BuildRPM \
	"ppcemu_system" \
	"${PPCEMU_SYSTEM_VERSION}" \
	"1" \
	"http://www.unisim-vp.com" \
	"BSD" \
	"UNISIM ppcemu-system" \
	"UNISIM ppcemu-system is a full system simulator of a \"PowerMac G4 PCI\" like machine (MPC7447A/MPC107) with Linux boot support." \
	"Development/Tools" \
	"/usr/bin/ppcemu-system \
	/usr/bin/ppcemu-system-debug \
	/usr/share/ppcemu-system/AUTHORS \
	/usr/share/ppcemu-system/COPYING \
	/usr/share/ppcemu-system/ChangeLog \
	/usr/share/ppcemu-system/INSTALL \
	/usr/share/ppcemu-system/NEWS \
	/usr/share/ppcemu-system/README \
	/usr/share/ppcemu-system/gdb_powerpc.xml \
	/usr/share/ppcemu-system/device_tree_pmac_g4.xml \
	/usr/share/ppcemu-system/pc_linux_fr_keymap.xml \
	/usr/share/ppcemu-system/pc_linux_us_keymap.xml \
	/usr/share/ppcemu-system/pc_windows_keymap.xml" \
	"--with-systemc=${SYSTEMC}" \
	"'CXXFLAGS=-O3 -g'"

BuildRPM \
	"embedded_ppc_g4_board" \
	"${EMBEDDED_PPC_G4_BOARD_VERSION}" \
	"1" \
	"http://www.unisim-vp.com" \
	"BSD" \
	"UNISIM embedded-ppc-g4-board" \
	"UNISIM embedded-ppc-g4-board is a full system simulator of an MPC7447A/MPC107 board with support of ELF32 binaries and targeted for industrial applications." \
	"Development/Tools" \
	"/usr/bin/embedded-ppc-g4-board \
	/usr/bin/embedded-ppc-g4-board-debug \
	/usr/share/embedded-ppc-g4-board/AUTHORS \
	/usr/share/embedded-ppc-g4-board/COPYING \
	/usr/share/embedded-ppc-g4-board/ChangeLog \
	/usr/share/embedded-ppc-g4-board/INSTALL \
	/usr/share/embedded-ppc-g4-board/NEWS \
	/usr/share/embedded-ppc-g4-board/README \
	/usr/share/embedded-ppc-g4-board/gdb_powerpc.xml" \
	"--with-systemc=${SYSTEMC}" \
	"'CXXFLAGS=-O3 -g'"
