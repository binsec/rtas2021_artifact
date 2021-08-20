#!/bin/bash
HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

function usage
{
	echo "Usage: `basename $0` <genisslib version> <ppcemu version> <ppcemu-system version> <embedded-ppc-g4-board version> <systemc> <tlm2.0>"
}

#MAINTAINER="Gilles Mouchard <gilles.mouchard@cea.fr>"
ARCH=i386

function find_deb_dependencies
{
	local PKG_DEPS=""
	local TMP=""
	for FILE in $@; do
		if [ -f ${FILE} ] && [ -x ${FILE} ]; then
			local FILE_HEAD=`head -c 4 ${FILE}`
			local ELF_HEAD=`echo -e "\0177ELF"`
			if [ "${FILE_HEAD}" = "${ELF_HEAD}" ]; then
				local SO_DEPS=`ldd ${FILE} | sed -e 's/^.*=>//' -e 's/^.*not a dynamic executable//' | cut -f 1 -d "("`
				#echo "SO_DEPS=\"${SO_DEPS}\""
				for SO_DEP in ${SO_DEPS}; do
					local PKGS=`dpkg -S ${SO_DEP} 2> /dev/null | cut -f 1 -d ":"`
					#echo "PKGS=\"${PKGS}\""
					for PKG in ${PKGS}; do
						local PKG_VERSION=`dpkg -s ${PKG} 2> /dev/null | grep Version | cut -f 2- -d ":" | sed 's/^[ \t]*//'`
						local regex="([0-9][0-9]*:)*[0-9][0-9]*(\.[0-9][0-9]*)*"
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
	
	local PKG_DEPS=`echo -e "${TMP}" | sort -u | sed ':a;N;$!ba;s/\n/, /g'`
	echo ${PKG_DEPS}
}

function Package {
	local PACKAGE_NAME=$1
	local VERSION="$2"
	local SECTION="$3"
	local INSTALL_DIR="$4"
	local DESCRIPTION="$5"
	local MAINTAINER="$6"

	local CONTROL_FILE="${INSTALL_DIR}/DEBIAN/control"
	local PREINST_FILE="${INSTALL_DIR}/DEBIAN/preinst"
	local TEMPLATES_FILE="${INSTALL_DIR}/DEBIAN/templates"
	local MD5SUMS_FILE="${INSTALL_DIR}/DEBIAN/md5sums"

	echo "Packaging ${PACKAGE_NAME} version ${VERSION} located into ${INSTALL_DIR}"
	# compute the installed size
	local installed_size=`du ${INSTALL_DIR} -s -k | cut -f 1`

	echo "Installed size is ${installed_size} KB"
	
	# build list of installed files
	printf "Listing files..."
	cd ${INSTALL_DIR}
	file_list=`find .`
	printf "Done\n"
	
	# create DEBIAN dubdirectory
	mkdir -p ${INSTALL_DIR}/DEBIAN
	
	# Fill-in DEBIAN/md5sums
	printf "Computing md5 sums..."
	for file in ${file_list}
	do
		if [ -f "${file}" ]; then
			md5sum "${file}" | sed 's:./::' >> ${MD5SUMS_FILE}
		fi
	done
	printf "Done\n"

	# Finding package dependencies
	printf "Searching dependencies..."
	local DEPS=`find_deb_dependencies ${file_list}`
	printf "Done\n"
	
	# Fill-in DEBIAN/control
	echo "Package: ${PACKAGE_NAME}" > ${CONTROL_FILE}
	echo "Version: ${VERSION}" >> ${CONTROL_FILE}
	echo "Section: ${SECTION}" >> ${CONTROL_FILE}
	echo "Priority: optional" >> ${CONTROL_FILE}
	echo "Architecture: any" >> ${CONTROL_FILE}
	echo "Depends: ${DEPS}" >> ${CONTROL_FILE}
	echo "Installed-Size: ${installed_size}" >> ${CONTROL_FILE}
	echo "Maintainer: ${MAINTAINER}" >> ${CONTROL_FILE}
	echo "Description: ${DESCRIPTION}" >> ${CONTROL_FILE}
	echo "" >> ${CONTROL_FILE}

	echo "========================================="
	echo "=            DEBIAN/md5sums             ="
	echo "========================================="
	cat ${MD5SUMS_FILE}
	
	echo "========================================="
	echo "=            DEBIAN/control             ="
	echo "========================================="
	cat ${CONTROL_FILE}

	# Build the package
	echo "========================================="
	echo "=            Package build              ="
	echo "========================================="
	
	cd ${HERE}
	echo "Building ${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"
	fakeroot dpkg-deb --build ${INSTALL_DIR} &> /dev/null
	local GENERATED_DEB=`basename ${INSTALL_DIR}`.deb
	if [ -e ${GENERATED_DEB} ]; then
		mv -f ${GENERATED_DEB} ${PACKAGE_NAME}_${VERSION}_${ARCH}.deb
		rm -rf ${INSTALL_DIR}/DEBIAN
	else
		echo "Building failed"
		exit 255
	fi
}

Package "$1" "$2" "$3" "$4" "$5"

