#!/bin/bash

PKG_DEPS=""
TMP=""
for FILE in $@; do
	if [ -f ${FILE} ] && [ -x ${FILE} ]; then
		FILE_HEAD=`head -c 4 ${FILE}`
		ELF_HEAD=`echo -e "\0177ELF"`
		if [ "${FILE_HEAD}" = "${ELF_HEAD}" ]; then
			SO_DEPS=`ldd ${FILE} | sed -e 's/^.*=>//' -e 's/^.*not a dynamic executable//' | cut -f 1 -d "("`
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

