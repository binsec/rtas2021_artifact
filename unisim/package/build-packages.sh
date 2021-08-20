#!/bin/bash

function Usage
{
	echo "Usage:"
	echo "  $0 i386-deb <version> <SystemC 2.2.0 install dir> <TLM 2.0 install dir>"
	echo "  $0 i586-rpm <version> <SystemC 2.2.0 install dir> <TLM 2.0 install dir>"
	echo "  $0 mingw32 <version> <SystemC 2.2.0 tarball> <TLM 2.0 tarball>"
	echo "  $0 powerpc-darwin <version> <SystemC 2.2.0 tarball> <TLM 2.0 tarball>"
	echo "  $0 i386-darwin <version> <SystemC 2.2.0 tarball> <TLM 2.0 tarball>"
}

if test "x$1" = x || test "x$2" = x || test "x$3" = x || test "x$4" = x; then
	Usage
	exit
fi

TARGET=$1
BUILD=i686-pc-linux-gnu
VERSION=$2

HERE=`pwd`
MAINTAINER="Gilles Mouchard <gilles.mouchard@cea.fr>"

case ${TARGET} in
	*mingw32*)
		ARCH=i386
		#UNISIM_PREFIX=
		UNISIM_PREFIX=/usr
		;;
	*86*deb*)
		ARCH=i386
		UNISIM_PREFIX=/usr
		;;
	*86*rpm*)
		ARCH=i586
		UNISIM_PREFIX=/usr
		;;
	*powerpc*darwin*)
		ARCH=POWERPC
		UNISIM_PREFIX=/usr
		;;
	*86*darwin*)
		ARCH=i386
		UNISIM_PREFIX=/usr
		;;
esac

UNISIM_TOOLS_SHORT_NAME=unisim-tools
UNISIM_TOOLS_LONG_NAME=${UNISIM_TOOLS_SHORT_NAME}-${VERSION}
UNISIM_TOOLS_SOURCE_PACKAGE_FILENAME=${UNISIM_TOOLS_LONG_NAME}.tar.gz
UNISIM_TOOLS_WINDOWS_PACKAGE_FILENAME=${UNISIM_TOOLS_LONG_NAME}.exe
UNISIM_TOOLS_DEBIAN_PACKAGE_FILENAME=${UNISIM_TOOLS_SHORT_NAME}_${VERSION}_${ARCH}.deb
UNISIM_TOOLS_REDHAT_PACKAGE_FILENAME=${UNISIM_TOOLS_SHORT_NAME}-${VERSION}.${ARCH}.rpm
UNISIM_TOOLS_TEMPORARY_SOURCE_DIR=${HOME}/tmp/${UNISIM_TOOLS_LONG_NAME}
UNISIM_TOOLS_TEMPORARY_INSTALL_DIR=${HOME}/tmp/${UNISIM_TOOLS_SHORT_NAME}_${VERSION}_${ARCH}
UNISIM_TOOLS_TEMPORARY_CONFIG_DIR=${HOME}/tmp/${UNISIM_TOOLS_LONG_NAME}_config
UNISIM_TOOLS_LICENSE_FILE="share/unisim_tools/COPYING"
UNISIM_TOOLS_DESCRIPTION="UNISIM Tools"

UNISIM_LIB_SHORT_NAME=unisim-lib
UNISIM_LIB_LONG_NAME=${UNISIM_LIB_SHORT_NAME}-${VERSION}
UNISIM_LIB_SOURCE_PACKAGE_FILENAME=${UNISIM_LIB_LONG_NAME}.tar.gz
UNISIM_LIB_WINDOWS_PACKAGE_FILENAME=${UNISIM_LIB_LONG_NAME}.exe
UNISIM_LIB_DEBIAN_PACKAGE_FILENAME=${UNISIM_LIB_SHORT_NAME}_${VERSION}_${ARCH}.deb
UNISIM_LIB_REDHAT_PACKAGE_FILENAME=${UNISIM_LIB_SHORT_NAME}-${VERSION}.${ARCH}.rpm
UNISIM_LIB_TEMPORARY_SOURCE_DIR=${HOME}/tmp/${UNISIM_LIB_LONG_NAME}
UNISIM_LIB_TEMPORARY_INSTALL_DIR=${HOME}/tmp/${UNISIM_LIB_SHORT_NAME}_${VERSION}_${ARCH}
UNISIM_LIB_TEMPORARY_CONFIG_DIR=${HOME}/tmp/${UNISIM_LIB_LONG_NAME}_config
UNISIM_LIB_LICENSE_FILE="share/unisim_lib/COPYING"
UNISIM_LIB_DESCRIPTION="UNISIM Library"

UNISIM_SIMULATORS_SHORT_NAME=unisim-simulators
UNISIM_SIMULATORS_LONG_NAME=${UNISIM_SIMULATORS_SHORT_NAME}-${VERSION}
UNISIM_SIMULATORS_SOURCE_PACKAGE_FILENAME=${UNISIM_SIMULATORS_LONG_NAME}.tar.gz
UNISIM_SIMULATORS_WINDOWS_PACKAGE_FILENAME=${UNISIM_SIMULATORS_LONG_NAME}.exe
UNISIM_SIMULATORS_DEBIAN_PACKAGE_FILENAME=${UNISIM_SIMULATORS_SHORT_NAME}_${VERSION}_${ARCH}.deb
UNISIM_SIMULATORS_REDHAT_PACKAGE_FILENAME=${UNISIM_SIMULATORS_SHORT_NAME}-${VERSION}.${ARCH}.rpm
UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR=${HOME}/tmp/${UNISIM_SIMULATORS_LONG_NAME}
UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR=${HOME}/tmp/${UNISIM_SIMULATORS_SHORT_NAME}_${VERSION}_${ARCH}
UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR=${HOME}/tmp/${UNISIM_SIMULATORS_LONG_NAME}_config
UNISIM_SIMULATORS_LICENSE_FILE="share/unisim_simulators/COPYING"
UNISIM_SIMULATORS_DESCRIPTION="UNISIM Simulators"

if test ! -f ${HERE}/${UNISIM_TOOLS_SOURCE_PACKAGE_FILENAME}; then
	Usage
	echo "File ${HERE}/${UNISIM_TOOLS_SOURCE_PACKAGE_FILENAME} is needed. Use \"make dist\" to build it."
	exit
fi

if test ! -f ${HERE}/${UNISIM_LIB_SOURCE_PACKAGE_FILENAME}; then
	Usage
	echo "File ${HERE}/${UNISIM_LIB_LONG_NAME}.tar.gz is needed. Use \"make dist\" to build it."
	exit
fi

if test ! -f ${HERE}/${UNISIM_SIMULATORS_SOURCE_PACKAGE_FILENAME}; then
	Usage
	echo "File ${HERE}/${UNISIM_SIMULATORS_SOURCE_PACKAGE_FILENAME} is needed. Use \"make dist\" to build it."
	exit
fi

case ${TARGET} in
	*86*deb* | *86*rpm* )
		if test "x$3" = x; then
			Usage
			echo "SystemC 2.2.0 install directory is needed."
			exit
		fi
		SYSTEMC_INSTALL_DIR=$3

		if test "x$4" = x; then
			Usage
			echo "TLM 2.0 install directory is needed."
			exit
		fi

		TLM2_INSTALL_DIR=$4
		;;
	*mingw32* | powerpc*darwin* | *86*darwin*)
		if test "x$3" = x; then
			Usage
			echo "SystemC 2.2.0 tarball is needed."
			exit
		fi
		SYSTEMC_TARBALL=$3

		if test "x$4" = x; then
			Usage
			echo "TLM 2.0 tarball is needed."
			exit
		fi

		TLM2_TARBALL=$4
		;;
esac

case ${TARGET} in
	*mingw32*)
		UNISIM_BOOTSTRAP_MINGW32_DIR=${HOME}/tmp/unisim-bootstrap-mingw32
		if test -d ${UNISIM_BOOTSTRAP_MINGW32_DIR}; then
			echo "${UNISIM_BOOTSTRAP_MINGW32_DIR} already exists. Do you want to reuse it ? (Y/n)"
			read REUSE_UNISIM_BOOTSTRAP_MINGW32
		else
			REUSE_UNISIM_BOOTSTRAP_MINGW32="n"
		fi

		if test ! ${REUSE_UNISIM_BOOTSTRAP_MINGW32} = "y" && test ! ${REUSE_UNISIM_BOOTSTRAP_MINGW32} = "Y"; then
			rm -rf ${UNISIM_BOOTSTRAP_MINGW32_DIR}
			mkdir -p ${UNISIM_BOOTSTRAP_MINGW32_DIR}
			cd ${UNISIM_BOOTSTRAP_MINGW32_DIR}
			mkdir -p archives
			mkdir -p source
			mkdir -p patches
			mkdir -p install
			cp `dirname $0`/patch-systemc-2.2.0 patches/.
			cp `dirname $0`/COPYING .
			cp ${SYSTEMC_TARBALL} archives/.
			cp ${TLM2_TARBALL} archives/.
			`dirname $0`/compile-unisim-bootstrap-mingw32.sh || exit
			`dirname $0`/compile-unisim-bootstrap-mingw32.sh clean || exit
			rm -rf source
		fi
		SYSTEMC_INSTALL_DIR=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/systemc
		TLM2_INSTALL_DIR=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/TLM-2008-06-09
		;;
	*86*darwin*)
		UNISIM_BOOTSTRAP_I686_DARWIN_DIR=${HOME}/tmp/unisim-bootstrap-i686-darwin
		if test -d ${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}; then
			echo "${UNISIM_BOOTSTRAP_I686_DARWIN_DIR} already exists. Do you want to reuse it ? (Y/n)"
			read REUSE_UNISIM_BOOTSTRAP_I686_DARWIN
		else
			REUSE_UNISIM_BOOTSTRAP_I686_DARWIN="n"
		fi

		if test ! ${REUSE_UNISIM_BOOTSTRAP_I686_DARWIN} = "y" && test ! ${REUSE_UNISIM_BOOTSTRAP_I686_DARWIN} = "Y"; then
			rm -rf ${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}
			mkdir -p ${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}
			cd ${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}
			mkdir -p archives
			mkdir -p source
			mkdir -p patches
			mkdir -p install
			cp `dirname $0`/patch-systemc-2.2.0 patches/.
			cp `dirname $0`/COPYING .
			cp ${SYSTEMC_TARBALL} archives/.
			cp ${TLM2_TARBALL} archives/.
			`dirname $0`/compile-unisim-bootstrap-i686-darwin.sh || exit
			`dirname $0`/compile-unisim-bootstrap-i686-darwin.sh clean || exit
			rm -rf source
		fi
		SYSTEMC_INSTALL_DIR=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/systemc
		TLM2_INSTALL_DIR=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/TLM-2008-06-09
		;;
	*powerpc*darwin*)
		UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR=${HOME}/tmp/unisim-bootstrap-powerpc-darwin
		if test -d ${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}; then
			echo "${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR} already exists. Do you want to reuse it ? (Y/n)"
			read REUSE_UNISIM_BOOTSTRAP_POWERPC_DARWIN
		else
			REUSE_UNISIM_BOOTSTRAP_POWERPC_DARWIN="n"
		fi

		if test ! ${REUSE_UNISIM_BOOTSTRAP_POWERPC_DARWIN} = "y" && test ! ${REUSE_UNISIM_BOOTSTRAP_POWERPC_DARWIN} = "Y"; then
			rm -rf ${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}
			mkdir -p ${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}
			cd ${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}
			mkdir -p archives
			mkdir -p source
			mkdir -p patches
			mkdir -p install
			cp `dirname $0`/patch-systemc-2.2.0 patches/.
			cp `dirname $0`/COPYING .
			cp ${SYSTEMC_TARBALL} archives/.
			cp ${TLM2_TARBALL} archives/.
			`dirname $0`/compile-unisim-bootstrap-powerpc-darwin.sh || exit
			`dirname $0`/compile-unisim-bootstrap-powerpc-darwin.sh clean || exit
			rm -rf source
		fi
		SYSTEMC_INSTALL_DIR=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/systemc
		TLM2_INSTALL_DIR=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/TLM-2008-06-09
		;;
	*86*deb* | *86*rpm*)
		if test "x$3" = x; then
			Usage
			exit
		fi
		SYSTEMC_INSTALL_DIR=$3
		;;
esac

NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`

function fill_rpm_files_section ()
{
	BASE=$1
	local PARENT=$2
	local FILE_LIST=`cd ${BASE}/${PARENT}; ls`
	OUTPUT_FILE=$3
	for FILE in ${FILE_LIST}; do
		if [ -d "${BASE}/${PARENT}/${FILE}" ]; then
			echo "%dir \"${PARENT}/${FILE}\"" >> ${OUTPUT_FILE}
			fill_rpm_files_section "${BASE}" "${PARENT}/${FILE}" "${OUTPUT_FILE}"
		else
			echo "\"${PARENT}/${FILE}\"" >> ${OUTPUT_FILE}
		fi
	done
}

function find_deb_dependencies
{
	PKG_DEPS=""
	TMP=""
	for FILE in $@; do
		if [ -f ${FILE} ] && [ -x ${FILE} ]; then
			FILE_HEAD=`head -c 4 ${FILE}`
			ELF_HEAD=`echo -e "\0177ELF"`
			if [ "${FILE_HEAD}" = "${ELF_HEAD}" ]; then
				SO_DEPS=`ldd ${FILE} | sed 's/^.*=>//' | cut -f 1 -d "("`
				#echo "SO_DEPS=\"${SO_DEPS}\""
				for SO_DEP in ${SO_DEPS}; do
					PKGS=`dpkg -S ${SO_DEP} 2> /dev/null | cut -f 1 -d ":"`
					#echo "PKGS=\"${PKGS}\""
					for PKG in ${PKGS}; do
						PKG_VERSION=`dpkg -s ${PKG} 2> /dev/null | grep Version | cut -f 2- -d ":" | sed 's/^[ \t]*//'`
						regex="([0-9][0-9]*:)*[0-9][0-9]*(\.[0-9][0-9]*)*"
						if [[ ${PKG_VERSION} =~ ${regex} ]]; then
							PKG_VERSION=$BASH_REMATCH
						fi
						if [ -n "${TMP}" ]; then
							TMP+="\n"
						fi
						TMP+="${PKG} (>= ${PKG_VERSION})"
					done
				done
			fi
		fi
	done
	
	PKG_DEPS=`echo -e "${TMP}" | sort -u | sed ':a;N;$!ba;s/\n/, /g'`
	echo ${PKG_DEPS}
}

function Package {
	PACKAGE_NAME=$1
	INSTALL_DIR=$2
	LICENSE_FILE=$3
	DESCRIPTION=$4

	case ${TARGET} in
		*mingw32*)
			cd ${INSTALL_DIR}
			rm -rf ${INSTALL_DIR}/dist
			rm -f ${ISS_FILENAME}
			
			# build list of installed files
			file_list=`find .`

			ISS_FILENAME=${PACKAGE_NAME}-${VERSION}.iss
			
			# Fill-in dist.iss
			echo "[Setup]" > ${ISS_FILENAME}
			echo "AppName=${PACKAGE_NAME}" >> ${ISS_FILENAME}
			echo "AppVerName=${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
			echo "AppPublisher=CEA" >> ${ISS_FILENAME}
			echo "AppPublisherURL=http://www.unisim.org" >> ${ISS_FILENAME}
			echo "AppSupportURL=http://www.unisim.org" >> ${ISS_FILENAME}
			echo "AppUpdatesURL=http://www.unisim.org" >> ${ISS_FILENAME}
			echo "DefaultDirName={pf}\\${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
			echo "DefaultGroupName=${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
			echo "LicenseFile=.${UNISIM_PREFIX}/${LICENSE_FILE}" >> ${ISS_FILENAME}
			echo "OutputDir=dist" >> ${ISS_FILENAME}
			echo "OutputBaseFilename=${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
			echo "Compression=lzma" >> ${ISS_FILENAME}
			echo "SolidCompression=yes" >> ${ISS_FILENAME}
			echo "" >> ${ISS_FILENAME}
			echo "[Languages]" >> ${ISS_FILENAME}
			echo "Name: \"english\"; MessagesFile: \"compiler:Default.isl\"" >> ${ISS_FILENAME}
			echo "" >> ${ISS_FILENAME}
			echo "[Tasks]" >> ${ISS_FILENAME}
#			echo "Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked" >> ${ISS_FILENAME}
			echo "" >> ${ISS_FILENAME}
			echo "[Files]" >> ${ISS_FILENAME}
			for file in ${file_list}
			do
				if test -f ${file}; then
					stripped_filename=`echo ${file} | sed 's:./::'`
					echo "Source: \"${stripped_filename}\"; DestDir: \"{app}/`dirname ${stripped_filename}`\"; Flags: ignoreversion" >> ${ISS_FILENAME}
				fi
			done
			
			echo "========================================="
			echo "=            Inno Setup Script          ="
			echo "========================================="
			cat ${ISS_FILENAME}
			
			# Build the package
			echo "========================================="
			echo "=            Package build              ="
			echo "========================================="
		
			wine ~/.wine/drive_c/Program\ Files/Inno\ Setup\ 5/ISCC.exe ${ISS_FILENAME} || exit
			cp -f ${INSTALL_DIR}/dist/${PACKAGE_NAME}-${VERSION}.exe ${HERE}
			rm -rf ${INSTALL_DIR}/dist
			rm -f ${ISS_FILENAME}
			;;
		*86*darwin*)
			cd ${INSTALL_DIR}
			tar zcvf ${HERE}/${PACKAGE_NAME}-${VERSION}-i686-darwin.tar.gz *
			;;
		*powerpc*darwin*)
			cd ${INSTALL_DIR}
			tar zcvf ${HERE}/${PACKAGE_NAME}-${VERSION}-powerpc-darwin.tar.gz *
			;;
		*86*deb*)
			CONTROL_FILE=${INSTALL_DIR}/DEBIAN/control
			PREINST_FILE=${INSTALL_DIR}/DEBIAN/preinst
			TEMPLATES_FILE=${INSTALL_DIR}/DEBIAN/templates
			MD5SUMS_FILE=${INSTALL_DIR}/DEBIAN/md5sums
		
			# compute the installed size
			installed_size=`du ${INSTALL_DIR} -s -k | cut -f 1`
			
			# build list of installed files
			cd ${INSTALL_DIR}
			file_list=`find .`
			
			# create DEBIAN dubdirectory
			mkdir -p ${INSTALL_DIR}/DEBIAN
			
			# Fill-in DEBIAN/md5sums
			for file in ${file_list}
			do
				if test -f $file; then
					md5sum $file | sed 's:./::' >> ${MD5SUMS_FILE}
				fi
			done

			# Finding package dependencies
			DEPS=`find_deb_dependencies ${file_list}`
			
			# Fill-in DEBIAN/control
			echo "Package: ${PACKAGE_NAME}" > ${CONTROL_FILE}
			echo "Version: ${VERSION}" >> ${CONTROL_FILE}
			echo "Section: devel" >> ${CONTROL_FILE}
			echo "Priority: optional" >> ${CONTROL_FILE}
			echo "Architecture: ${ARCH}" >> ${CONTROL_FILE}
			echo "Pre-Depends: debconf (>= 0.5) | debconf-2.0" >> ${CONTROL_FILE}
			echo "Depends: ${DEPS}" >> ${CONTROL_FILE}
			echo "Installed-Size: ${installed_size}" >> ${CONTROL_FILE}
			echo "Maintainer: ${MAINTAINER}" >> ${CONTROL_FILE}
			echo "Description: ${DESCRIPTION}" >> ${CONTROL_FILE}
			echo "" >> ${CONTROL_FILE}

			# Fill-in DEBIAN/preinst
			echo "#!/bin/sh -e" > ${PREINST_FILE}
			echo "" >> ${PREINST_FILE}
			echo "# Source debconf library." >> ${PREINST_FILE}
			echo ". /usr/share/debconf/confmodule" >> ${PREINST_FILE}
			echo "db_version 2.0" >> ${PREINST_FILE}
			echo "# This conf script is capable of backing up" >> ${PREINST_FILE}
			echo "db_capb backup" >> ${PREINST_FILE}
			echo "" >> ${PREINST_FILE}
			echo "STATE=1" >> ${PREINST_FILE}
			echo "while true; do" >> ${PREINST_FILE}
			echo "    case \"\${STATE}\" in" >> ${PREINST_FILE}
			echo "        1) # present license" >> ${PREINST_FILE}
			echo "            db_fset ${PACKAGE_NAME}/present_license seen false" >> ${PREINST_FILE}
			echo "            if ! db_input critical ${PACKAGE_NAME}/present_license ; then" >> ${PREINST_FILE}
			echo "                errmsg \"${PACKAGE_NAME} license could not be presented\"" >> ${PREINST_FILE}
			echo "                exit 2" >> ${PREINST_FILE}
			echo "            fi" >> ${PREINST_FILE}
			echo "            db_fset ${PACKAGE_NAME}/accept_license seen false" >> ${PREINST_FILE}
			echo "            if ! db_input critical ${PACKAGE_NAME}/accept_license ; then" >> ${PREINST_FILE}
			echo "                errmsg \"${PACKAGE_NAME} license agree question could not be asked\"" >> ${PREINST_FILE}
			echo "                exit 2" >> ${PREINST_FILE}
			echo "            fi" >> ${PREINST_FILE}
			echo "            ;;" >> ${PREINST_FILE}
			echo "" >> ${PREINST_FILE}
			echo "        2)  # determine users' choice" >> ${PREINST_FILE}
			echo "            db_get ${PACKAGE_NAME}/accept_license" >> ${PREINST_FILE}
			echo "            if [ \"\${RET}\" = \"true\" ]; then" >> ${PREINST_FILE}
			echo "                # license accepted" >> ${PREINST_FILE}
			echo "                exit 0" >> ${PREINST_FILE}
			echo "            fi" >> ${PREINST_FILE}
			echo "            # error on decline license (give user chance to back up)" >> ${PREINST_FILE}
			echo "            db_input critical ${PACKAGE_NAME}/error_license" >> ${PREINST_FILE}
			echo "            ;;" >> ${PREINST_FILE}
			echo "        3)  # user has confirmed declining license" >> ${PREINST_FILE}
			echo "            echo \"user did not accept the ${PACKAGE_NAME} license\" >&2" >> ${PREINST_FILE}
			echo "            exit 1" >> ${PREINST_FILE}
			echo "            ;;" >> ${PREINST_FILE}
			echo "        *)  # unknown state" >> ${PREINST_FILE}
			echo "            echo \"${PACKAGE_NAME} license state unknown: \${STATE}\" >&2" >> ${PREINST_FILE}
			echo "            exit 2" >> ${PREINST_FILE}
			echo "            ;;" >> ${PREINST_FILE}
			echo "    esac" >> ${PREINST_FILE}
			echo "" >> ${PREINST_FILE}
			echo "    if db_go; then" >> ${PREINST_FILE}
			echo "        STATE=\$((\${STATE} + 1))" >> ${PREINST_FILE}
			echo "    else" >> ${PREINST_FILE}
			echo "        STATE=\$((\${STATE} - 1))" >> ${PREINST_FILE}
			echo "    fi" >> ${PREINST_FILE}
			echo "" >> ${PREINST_FILE}
			echo "done" >> ${PREINST_FILE}
			chmod 0555 ${PREINST_FILE}
			
			# Fill-in DEBIAN/templates
			echo "Template: ${PACKAGE_NAME}/accept_license" > ${TEMPLATES_FILE}
			echo "Type: boolean" >> ${TEMPLATES_FILE}
			echo "Default: false" >> ${TEMPLATES_FILE}
			echo "Description: Do you agree with the terms of ${PACKAGE_NAME} license?" >> ${TEMPLATES_FILE}
			echo " In order to install this package, you must agree to its license terms. Not accepting will cancel the installation." >> ${TEMPLATES_FILE}
			echo "" >> ${TEMPLATES_FILE}
			echo "Template: ${PACKAGE_NAME}/error_license" >> ${TEMPLATES_FILE}
			echo "Type: error" >> ${TEMPLATES_FILE}
			echo "Description: Declined ${PACKAGE_NAME} License" >> ${TEMPLATES_FILE}
			echo " If you do not agree to the ${PACKAGE_NAME} license terms you cannot install this software." >> ${TEMPLATES_FILE}
			echo " The installation of this package will be canceled." >> ${TEMPLATES_FILE}
			echo "" >> ${TEMPLATES_FILE}
			echo "Template: ${PACKAGE_NAME}/present_license" >> ${TEMPLATES_FILE}
			echo "Type: note" >> ${TEMPLATES_FILE}
			echo "Description: ${PACKAGE_NAME} License" >> ${TEMPLATES_FILE}
			echo "  PLEASE READ CAREFULLY THE FOLLOWING LICENSE" >> ${TEMPLATES_FILE}
			echo "  " >> ${TEMPLATES_FILE}
			cat ${INSTALL_DIR}${UNISIM_PREFIX}/${LICENSE_FILE} | sed 's/^/  /' >> ${TEMPLATES_FILE}

			echo "========================================="
			echo "=            DEBIAN/control             ="
			echo "========================================="
			cat ${CONTROL_FILE}

			echo "========================================="
			echo "=            DEBIAN/preinst             ="
			echo "========================================="
			cat ${PREINST_FILE}

			echo "========================================="
			echo "=            DEBIAN/templates           ="
			echo "========================================="
			cat ${TEMPLATES_FILE}

			echo "========================================="
			echo "=            DEBIAN/md5sums             ="
			echo "========================================="
			cat ${MD5SUMS_FILE}
			
			# Build the package
			echo "========================================="
			echo "=            Package build              ="
			echo "========================================="
			fakeroot dpkg-deb --build ${INSTALL_DIR}
			mv ${INSTALL_DIR}/../${PACKAGE_NAME}_${VERSION}_${ARCH}.deb ${HERE}/.
			;;

		*86*rpm*)
			RPM_MACROS_FILE=${HOME}/.rpmmacros
			echo "%_rpmdir ${HOME}/tmp" > ${RPM_MACROS_FILE}

			SPEC_FILE="${HOME}/tmp/${PACKAGE_NAME}.spec"
			echo "Summary: ${DESCRIPTION}" > ${SPEC_FILE}
			echo "Name: ${PACKAGE_NAME}" >> ${SPEC_FILE}
			echo "Version: `echo ${VERSION} | cut -f 1 -d -`" >> ${SPEC_FILE}
			echo "Release: `echo ${VERSION} | cut -f 2 -d -`" >> ${SPEC_FILE}
			echo "License: BSD" >> ${SPEC_FILE}
			echo "Packager: https://unisim.org/svn/devel/package/build-packages.sh" >> ${SPEC_FILE}
			echo "Group: Development" >> ${SPEC_FILE}
			echo "BuildRoot: ${INSTALL_DIR}" >> ${SPEC_FILE}
			echo "Provides: ${PACKAGE_NAME}" >> ${SPEC_FILE}
			# Let the packager find the dependencies
			#echo "Requires: ${DEPS}" >> ${SPEC_FILE}
			echo "" >> ${SPEC_FILE}
			echo "%description"  >> ${SPEC_FILE}
			echo "${DESCRIPTION}"  >> ${SPEC_FILE}
			echo ""  >> ${SPEC_FILE}
			echo "%files" >> ${SPEC_FILE}
			fill_rpm_files_section ${INSTALL_DIR} "" ${SPEC_FILE}

			echo "========================================="
			echo "=               RPM Spec                ="
			echo "========================================="
			cat ${SPEC_FILE}

			# no way to do license control with RPM, as no interaction with the user is allowed

			# Build the package
			echo "========================================="
			echo "=            Package build              ="
			echo "========================================="
			rpmbuild -bb ${SPEC_FILE} --buildroot "${INSTALL_DIR}" --macros=${RPM_MACROS_FILE} --target=${ARCH}-pc-linux || exit
			# uncomment this line to dump informations about the package
			# rpm -qpl --info --requires --provides ${HOME}/tmp/${ARCH}/${PACKAGE_NAME}-${VERSION}.${ARCH}.rpm
			mv ${HOME}/tmp/${ARCH}/${PACKAGE_NAME}-${VERSION}.${ARCH}.rpm ${HERE}/.
			rmdir ${HOME}/tmp/${ARCH} || true
			rm -f ${RPM_MACROS_FILE}
			;;
	esac
}

function Configure
{
	SOURCE_DIR=$1
	CONFIG_DIR=$2
	INSTALL_DIR=$3
	NUM_FIXED_ARGS=3
	
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

	echo "========================================="
	echo "=              Configuring              ="
	echo "========================================="
	rm -rf ${CONFIG_DIR}
	mkdir -p ${CONFIG_DIR}
	cd ${CONFIG_DIR}
	#${SOURCE_DIR}/configure -C --prefix=${INSTALL_DIR}${UNISIM_PREFIX} "${args[@]}" || exit
	${SOURCE_DIR}/configure -C --prefix=${UNISIM_PREFIX} "${args[@]}" || exit
}

function Compile
{
	CONFIG_DIR=$1
	INSTALL_DIR=$2
	echo "========================================="
	echo "=              Compiling                ="
	echo "========================================="
	rm -rf ${INSTALL_DIR}
	cd ${CONFIG_DIR}
	make -j ${NUM_PROCESSORS} all || exit
}

function Install
{
	CONFIG_DIR=$1
	INSTALL_DIR=$2
	echo "========================================="
	echo "=              Installing               ="
	echo "========================================="
	rm -rf ${INSTALL_DIR}
	cd ${CONFIG_DIR}
	fakeroot make install prefix=${INSTALL_DIR}${UNISIM_PREFIX} || exit
}

function Uninstall
{
	CONFIG_DIR=$1
	INSTALL_DIR=$2
	echo "========================================="
	echo "=             Uninstalling              ="
	echo "========================================="
	rm -rf ${INSTALL_DIR}
	cd ${CONFIG_DIR}
	fakeroot make uninstall || exit
}

function Clean
{
	CONFIG_DIR=$1
	echo "========================================="
	echo "=              Cleaning                 ="
	echo "========================================="
	cd ${CONFIG_DIR}
	make clean || exit
}

case ${TARGET} in
	*mingw32*)
		if test ! -f ${HERE}/${PACKAGE_NAME}-${VERSION}.exe; then
			echo "Do you want to package unisim-bootstrap-mingw32 ? (Y/n)"
			read YES_NO
			if test ${YES_NO} = "y" || test ${YES_NO} = "Y"; then
				Package unisim-bootstrap-mingw32 ${UNISIM_BOOTSTRAP_MINGW32_DIR} COPYING "" "${UNISIM_BOOTSTRAP_MINGW32_DESCRIPTION}"
			fi
		fi
		;;
	*86*darwin*)
		if test ! -f ${HERE}/${PACKAGE_NAME}-${VERSION}.exe; then
			echo "Do you want to package unisim-bootstrap-i686-darwin ? (Y/n)"
			read YES_NO
			if test ${YES_NO} = "y" || test ${YES_NO} = "Y"; then
				Package unisim-bootstrap-i686-darwin ${UNISIM_BOOTSTRAP_I686_DARWIN_DIR} COPYING "" "${UNISIM_BOOTSTRAP_I686_DARWIN_DESCRIPTION}"
			fi
		fi
		;;
	*powerpc*darwin*)
		if test ! -f ${HERE}/${PACKAGE_NAME}-${VERSION}.exe; then
			echo "Do you want to package unisim-bootstrap-powerpc-darwin ? (Y/n)"
			read YES_NO
			if test ${YES_NO} = "y" || test ${YES_NO} = "Y"; then
				Package unisim-bootstrap-powerpc-darwin ${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR} COPYING "" "${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DESCRIPTION}"
			fi
		fi
		;;
	*86*deb* | *86*rpm*)
		;;
esac

echo "========================================="
echo "=        Uncompressing tarballs         ="
echo "========================================="
rm -rf ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR}
cd ${HOME}/tmp
tar zxvf ${HERE}/${UNISIM_TOOLS_LONG_NAME}.tar.gz || exit

rm -rf ${UNISIM_LIB_TEMPORARY_SOURCE_DIR}
cd ${HOME}/tmp
tar zxvf ${HERE}/${UNISIM_LIB_LONG_NAME}.tar.gz || exit

rm -rf ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR}
cd ${HOME}/tmp
tar zxvf ${HERE}/${UNISIM_SIMULATORS_LONG_NAME}.tar.gz || exit

Configure ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR} ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} CXXFLAGS="-m32 -O3"
Compile ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
Install ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}

case ${TARGET} in
	*mingw32*)
		Configure ${UNISIM_LIB_TEMPORARY_SOURCE_DIR} ${UNISIM_LIB_TEMPORARY_CONFIG_DIR} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=i586-mingw32msvc \
          --with-unisim-tools=${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          --with-tlm20=${TLM2_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -DDEBUG_NETSTUB" \
          --with-sdl=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/SDL \
          --with-boost=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/boost \
          --with-zlib=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/zlib \
          --with-libxml2=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/libxml2 \
          --enable-release \
          --disable-shared
		;;
	*86*darwin*)
		Configure ${UNISIM_LIB_TEMPORARY_SOURCE_DIR} ${UNISIM_LIB_TEMPORARY_CONFIG_DIR} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=i686-apple-darwin8 \
          --with-unisim-tools=${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          --with-tlm20=${TLM2_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -DDEBUG_NETSTUB" \
          --with-sdl=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/SDL \
          --with-boost=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/boost \
          --with-zlib=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/zlib \
          --with-libedit=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/libedit \
          --with-libxml2=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/libxml2 \
          --enable-release
		;;
	*powerpc*darwin*)
		Configure ${UNISIM_LIB_TEMPORARY_SOURCE_DIR} ${UNISIM_LIB_TEMPORARY_CONFIG_DIR} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=powerpc-apple-darwin8 \
          --with-unisim-tools=${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          --with-tlm20=${TLM2_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -DDEBUG_NETSTUB" \
          --with-sdl=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/SDL \
          --with-boost=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/boost \
          --with-zlib=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/zlib \
          --with-libedit=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/libedit \
          --with-libxml2=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/libxml2 \
          --enable-release
		;;
	*86*deb* | *86*rpm*)
		Configure ${UNISIM_LIB_TEMPORARY_SOURCE_DIR} ${UNISIM_LIB_TEMPORARY_CONFIG_DIR} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=i686-pc-linux-gnu \
          --with-unisim-tools=${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          --with-tlm20=${TLM2_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -DDEBUG_NETSTUB" \
          --enable-release
		;;
esac

Compile ${UNISIM_LIB_TEMPORARY_CONFIG_DIR} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR}
Install ${UNISIM_LIB_TEMPORARY_CONFIG_DIR} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR}
mkdir -p ${UNISIM_LIB_TEMPORARY_INSTALL_DIR}/${UNISIM_PREFIX}/src
cp ${HERE}/${UNISIM_LIB_LONG_NAME}.tar.gz ${UNISIM_LIB_TEMPORARY_INSTALL_DIR}/${UNISIM_PREFIX}/src/.
Package ${UNISIM_LIB_SHORT_NAME} ${UNISIM_LIB_TEMPORARY_INSTALL_DIR} ${UNISIM_LIB_LICENSE_FILE} "${UNISIM_LIB_DESCRIPTION}"

case ${TARGET} in
	*mingw32*)
		Configure ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR} ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=i586-mingw32msvc \
          --with-unisim-lib=${UNISIM_LIB_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          --with-tlm20=${TLM2_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -DDEBUG_NETSTUB" \
          --with-sdl=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/SDL \
          --with-boost=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/boost \
          --with-zlib=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/zlib \
          --with-libxml2=${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/libxml2 \
          --enable-release
		;;
	*86*darwin*)
		Configure ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR} ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=i686-apple-darwin8 \
          --with-unisim-lib=${UNISIM_LIB_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          --with-tlm20=${TLM2_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -DDEBUG_NETSTUB" \
          --with-sdl=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/SDL \
          --with-boost=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/boost \
          --with-zlib=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/zlib \
          --with-libedit=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/libedit \
          --with-libxml2=${UNISIM_BOOTSTRAP_I686_DARWIN_DIR}/install/libxml2 \
          --enable-release
		;;
	*powerpc*darwin*)
		Configure ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR} ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=powerpc-apple-darwin8 \
          --with-unisim-lib=${UNISIM_LIB_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          --with-tlm20=${TLM2_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -DDEBUG_NETSTUB" \
          --with-sdl=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/SDL \
          --with-boost=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/boost \
          --with-zlib=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/zlib \
          --with-libedit=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/libedit \
          --with-libxml2=${UNISIM_BOOTSTRAP_POWERPC_DARWIN_DIR}/install/libxml2 \
          --enable-release
		;;
	*86*deb* | *86*rpm*)
		Configure ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR} ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=i686-pc-linux-gnu \
          --with-unisim-lib=${UNISIM_LIB_TEMPORARY_INSTALL_DIR}${UNISIM_PREFIX} \
          --with-systemc=${SYSTEMC_INSTALL_DIR} \
          --with-tlm20=${TLM2_INSTALL_DIR} \
          CXXFLAGS="-m32 -O3 -DDEBUG_NETSTUB" \
          --enable-release
		;;
esac

Compile ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}
Install ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}
cp -f `dirname $0`/unisim_simulators_package_COPYING ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}/${UNISIM_PREFIX}/${UNISIM_SIMULATORS_LICENSE_FILE}

case ${TARGET} in
	*mingw32*)
		cp ${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/libxml2/bin/libxml2-2.dll ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}/bin
		cp ${UNISIM_BOOTSTRAP_MINGW32_DIR}/install/SDL/bin/SDL.dll ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}/bin
		;;
esac

mkdir -p ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}/${UNISIM_PREFIX}/src
cp ${HERE}/${UNISIM_SIMULATORS_LONG_NAME}.tar.gz ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}/${UNISIM_PREFIX}/src/.
Package ${UNISIM_SIMULATORS_SHORT_NAME} ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR} ${UNISIM_SIMULATORS_LICENSE_FILE} "${UNISIM_SIMULATORS_DESCRIPTION}"

case ${TARGET} in
	*mingw32*)
		Uninstall ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
		Clean ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
		Configure ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR} ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=i586-mingw32msvc
		Compile ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
		Install ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
		;;
	*86*darwin*)
		Uninstall ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
		Clean ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
		Configure ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR} ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=i686-apple-darwin8
		Compile ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
		Install ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
		;;
	*powerpc*darwin*)
		Uninstall ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
		Clean ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
		Configure ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR} ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} \
          --build=${BUILD} \
          --host=powerpc-apple-darwin8
		Compile ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
		Install ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
		;;
	*86*deb* | *86*rpm*)
		;;
esac

mkdir -p ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}/${UNISIM_PREFIX}/src
cp ${HERE}/${UNISIM_TOOLS_LONG_NAME}.tar.gz ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}/${UNISIM_PREFIX}/src/.
Package ${UNISIM_TOOLS_SHORT_NAME} ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR} ${UNISIM_TOOLS_LICENSE_FILE} "${UNISIM_TOOLS_DESCRIPTION}"

echo "========================================="
echo "=       Clean temporary directories     ="
echo "========================================="
# cleanup every temporary directories
rm -rf ${UNISIM_TOOLS_TEMPORARY_INSTALL_DIR}
rm -rf ${UNISIM_TOOLS_TEMPORARY_CONFIG_DIR}
rm -rf ${UNISIM_TOOLS_TEMPORARY_SOURCE_DIR}

rm -rf ${UNISIM_LIB_TEMPORARY_INSTALL_DIR}
rm -rf ${UNISIM_LIB_TEMPORARY_CONFIG_DIR}
rm -rf ${UNISIM_LIB_TEMPORARY_SOURCE_DIR}

rm -rf ${UNISIM_SIMULATORS_TEMPORARY_INSTALL_DIR}
rm -rf ${UNISIM_SIMULATORS_TEMPORARY_CONFIG_DIR}
rm -rf ${UNISIM_SIMULATORS_TEMPORARY_SOURCE_DIR}
