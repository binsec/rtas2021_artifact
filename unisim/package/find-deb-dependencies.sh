#!/bin/bash
if [ "x$1" = x ]; then
	echo "`basename $0`: Find package dependencies of a dynamically linked ELF binary."
	echo "Usage: `basename $0` <filename>"
	exit 1
fi

FILE=$1
if [ -f ${FILE} ] && [ -x ${FILE} ]; then
	FILE_HEAD=`head -c 4 ${FILE}`
	ELF_HEAD=`echo -e "\0177ELF"`
	if [ "${FILE_HEAD}" = "${ELF_HEAD}" ]; then
		OUTPUT=
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
				OUTPUT+="${PKG} (>= ${PKG_VERSION}) provides ${SO_DEP} needed by `basename ${FILE}`\n"
			done
		done

		echo -e "${OUTPUT}" | sort -u
		exit 0
	fi
fi

echo "${FILE} is not an ELF binary"
exit 1
