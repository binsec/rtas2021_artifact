#!/bin/bash
case $(lsb_release -s -i) in
	Ubuntu* | Debian*)
		HOST=i586-mingw32msvc
		;;
	Mandriva*)
		HOST=i586-pc-mingw32
		;;
	Mageia*)
		case $(lsb_release -r) in
			1 | 2 | 3)
				HOST=i586-pc-mingw32
				;;
			*)
				HOST=i686-w64-mingw32
				;;
		esac
		;;
	RedHat* | Fedora* | SUSE*)
		HOST=i686-pc-mingw32
		;;
	*)
		echo "FATAL! Unknown Linux distribution. Aborting..."
		exit -1
esac

#BUILD=i686-pc-linux-gnu
SDL_VERSION=1.2.15
LIBXML2_VERSION=2.7.8
EXPAT_VERSION=2.1.0-1 #2.0.1
CROSS_GDB_VERSION=7.6.2
CROSS_GDB_ARCHITECTURES="powerpc-440fp-linux-gnu powerpc-7450-linux-gnu armel-linux-gnu m6811-elf"
ZLIB_VERSION=1.2.7
BOOST_VERSION=1_60_0 # 1_47_0
TCLTK_VERSION=8.5.11
GPERF_VERSION=3.0.4

HERE=`pwd`
MY_DIR=$(cd $(dirname $0); pwd)

TMP_DIR=${HOME}/tmp

DOWNLOADS_DIR=${HERE}/downloads
BUILD_DIR=${TMP_DIR}/mingw32-bootstrap-build
INSTALL_DIR=${TMP_DIR}/mingw32-bootstrap-install
PACKAGES_DIR=${HERE}/mingw32-packages

mkdir -p ${DOWNLOADS_DIR}
mkdir -p ${BUILD_DIR}
rm -rf ${INSTALL_DIR}
mkdir -p ${INSTALL_DIR}
mkdir -p ${PACKAGES_DIR}

NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`

function Patch
{
	NAME=$1
	PATCH=$2
	echo "========================================="
	echo "=              Patching                 ="
	echo "========================================="
	cd ${BUILD_DIR}/${NAME}
	patch -p1 < ${PATCH} || exit 
}

function Configure
{
	NAME=$1
	NUM_FIXED_ARGS=1
	
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
	cd ${BUILD_DIR}/${NAME}
	./configure --prefix=/usr "${args[@]}" || exit
}

function Compile
{
	NAME=$1
	echo "========================================="
	echo "=              Compiling                ="
	echo "========================================="
	cd ${BUILD_DIR}/${NAME}
	make -j ${NUM_PROCESSORS} all || exit
}

function CompileSingleProcess
{
	NAME=$1
	echo "========================================="
	echo "=              Compiling                ="
	echo "========================================="
	cd ${BUILD_DIR}/${NAME}
	make all || exit
}

function Install
{
	NAME=$1
	NUM_FIXED_ARGS=1
	
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
	echo "=              Installing               ="
	echo "========================================="
	cd ${BUILD_DIR}/${NAME}
	echo "make install-strip prefix=${INSTALL_DIR}/${NAME}" "${args[@]}"
	make install prefix=${INSTALL_DIR}/${NAME} "${args[@]}" || exit
}

function Download
{
	NAME=$1
	ARCHIVE_NAME=$2
	ARCHIVE_URL=$3

	ARCHIVE=${DOWNLOADS_DIR}/${ARCHIVE_NAME}
	if test ! -f ${ARCHIVE}; then
		cd ${DOWNLOADS_DIR}
		printf "Downloading ${ARCHIVE_URL}"
		success=no
		until [ $success = yes ]; do
			if wget --timeout=60 -t 1 -q  -O ${ARCHIVE_NAME} ${ARCHIVE_URL}; then
				success=yes
			else
				printf "."
			fi
		done
		printf "\n"
	fi

	cd ${BUILD_DIR} || exit
	rm -rf ${NAME}

	echo "Unpacking ${ARCHIVE_NAME}"
	ext=`echo "${ARCHIVE}" | awk -F . '{print $NF}'`
	case ${ext} in
		bz2)
			tar jxf ${ARCHIVE} || exit
			;;
		gz)
			tar zxf ${ARCHIVE} || exit
			;;
		zip)
			unzip -qo ${ARCHIVE}
			;;
		lzma)
			tar Yxf ${ARCHIVE} || exit
			;;
	esac
}

function InstallBinArchive
{
	ARCHIVE_NAME=$1
	ARCHIVE_URL=$2
	if [ -z "$3" ]; then
		SUBDIR=.
	else
		SUBDIR=$3
	fi

	ARCHIVE=${PACKAGES_DIR}/${ARCHIVE_NAME}
	if test ! -f ${ARCHIVE}; then
		cd ${BUILD_DIR}
		printf "Downloading ${ARCHIVE_URL}"
		success=no
		until [ $success = yes ]; do
			if wget --timeout=60 -t 1 -q -O ${ARCHIVE} ${ARCHIVE_URL}; then
				success=yes
			else
				printf "."
			fi
		done
		printf "\n"
	fi

	echo "Unpacking ${ARCHIVE_NAME}"
	mkdir -p ${INSTALL_DIR}/${SUBDIR}
	cd ${INSTALL_DIR}/${SUBDIR}
	ext=`echo "${ARCHIVE}" | awk -F . '{print $NF}'`
	case ${ext} in
		bz2)
			tar jxf ${ARCHIVE} || exit
			;;
		gz)
			tar zxf ${ARCHIVE} || exit
			;;
		zip)
			unzip -qo ${ARCHIVE}
			;;
		lzma)
			tar Yxf ${ARCHIVE} || exit
			;;
	esac
}

function Package
{
	NAME=$1
	if [ -n "$2" ]; then
		DIRNAME=$2
	else
		DIRNAME="${NAME}"
	fi

	cd ${INSTALL_DIR}/${DIRNAME}
	tar jcvf ${PACKAGES_DIR}/${NAME}-mingw32.tar.bz2 * || exit -1
}

function Clean
{
	rm -rf ${INSTALL_DIR}/*
	rm -rf ${BUILD_DIR}/*
}

CMD=$1

case ${CMD} in
	all)
		# expat
# 		if [ ! -e ${PACKAGES_DIR}/expat-${EXPAT_VERSION}-mingw32.tar.bz2 ]; then
# 			Clean
# 			Download expat-${EXPAT_VERSION} expat-${EXPAT_VERSION}.tar.gz http://ovh.dl.sourceforge.net/expat/expat-${EXPAT_VERSION}.tar.gz
# 			Configure expat-${EXPAT_VERSION} --host=${HOST}
# 			Compile expat-${EXPAT_VERSION}
# 			Install expat-${EXPAT_VERSION}
# 			Package expat-${EXPAT_VERSION}
# 			Clean
# 		fi

		# zlib
		if [ ! -e ${PACKAGES_DIR}/zlib-${ZLIB_VERSION}-mingw32.tar.bz2 ]; then
			Clean
#			Download zlib-${ZLIB_VERSION} zlib-${ZLIB_VERSION}.tar.gz http://zlib.net/zlib-${ZLIB_VERSION}.tar.gz
			Download zlib-${ZLIB_VERSION} zlib-${ZLIB_VERSION}.tar.gz http://prdownloads.sourceforge.net/libpng/zlib-${ZLIB_VERSION}.tar.gz\?download
			make -C ${BUILD_DIR}/zlib-${ZLIB_VERSION} -f win32/Makefile.gcc PREFIX=${HOST}- BINARY_PATH=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/bin INCLUDE_PATH=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/include LIBRARY_PATH=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/lib SHARED_MODE=1 install
			if [ -e ${INSTALL_DIR}/zlib-${ZLIB_VERSION}/lib/libzdll.a ]; then
				mv ${INSTALL_DIR}/zlib-${ZLIB_VERSION}/lib/libzdll.a ${INSTALL_DIR}/zlib-${ZLIB_VERSION}/lib/libz.dll.a
			fi
			Package zlib-${ZLIB_VERSION}
			Clean
		fi

		# cross-GDB
		for CROSS_GDB_ARCH in ${CROSS_GDB_ARCHITECTURES}
		do
			if [ ! -e ${PACKAGES_DIR}/cross-gdb-${CROSS_GDB_ARCH}-${CROSS_GDB_VERSION}-mingw32.tar.bz2 ]; then
				Clean
				CROSS_GDB_STRIPPED_VERSION=$(printf "${CROSS_GDB_VERSION}" | sed -e 's/^\(.*\)a$/\1/g')   # remove 'a' at the end of version number
				Download gdb-${CROSS_GDB_STRIPPED_VERSION} gdb-${CROSS_GDB_VERSION}.tar.bz2 ftp://ftp.gnu.org/pub/gnu/gdb/gdb-${CROSS_GDB_VERSION}.tar.bz2
				#InstallBinArchive expat-${EXPAT_VERSION}-mingw32.tar.bz2 '' expat-${EXPAT_VERSION}
				InstallBinArchive expat-${EXPAT_VERSION}-mingw32-dll.tar.lzma '' expat-${EXPAT_VERSION}
				InstallBinArchive expat-${EXPAT_VERSION}-mingw32-dev.tar.lzma '' expat-${EXPAT_VERSION}
				mv ${BUILD_DIR}/gdb-${CROSS_GDB_STRIPPED_VERSION} ${BUILD_DIR}/cross-gdb-${CROSS_GDB_ARCH}-${CROSS_GDB_VERSION}
				Configure cross-gdb-${CROSS_GDB_ARCH}-${CROSS_GDB_VERSION} --host=${HOST} --target=${CROSS_GDB_ARCH} \
							--disable-sim --disable-werror \
							--with-libexpat-prefix=${INSTALL_DIR}/expat-${EXPAT_VERSION}
				Compile cross-gdb-${CROSS_GDB_ARCH}-${CROSS_GDB_VERSION}
				Install cross-gdb-${CROSS_GDB_ARCH}-${CROSS_GDB_VERSION}
				Package cross-gdb-${CROSS_GDB_ARCH}-${CROSS_GDB_VERSION}
				Clean
			fi
		done

		# SDL
		if [ ! -e ${PACKAGES_DIR}/SDL-${SDL_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download SDL-${SDL_VERSION} SDL-${SDL_VERSION}.tar.gz http://www.libsdl.org/release/SDL-${SDL_VERSION}.tar.gz
			Configure SDL-${SDL_VERSION} --host=${HOST}
			Compile SDL-${SDL_VERSION}
			Install SDL-${SDL_VERSION}
			Package SDL-${SDL_VERSION}
			Clean
		fi

		# libxml2
		if [ ! -e ${PACKAGES_DIR}/libxml2-${LIBXML2_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download libxml2-${LIBXML2_VERSION} libxml2-${LIBXML2_VERSION}.tar.gz ftp://xmlsoft.org/libxml2/libxml2-${LIBXML2_VERSION}.tar.gz
			InstallBinArchive zlib-${ZLIB_VERSION}-mingw32.tar.bz2 '' zlib-${ZLIB_VERSION}
			Configure libxml2-${LIBXML2_VERSION} --host=${HOST} --without-python --with-zlib=${INSTALL_DIR}/zlib-${ZLIB_VERSION} CPPFLAGS=-DLIBXML_STATIC
			Compile libxml2-${LIBXML2_VERSION}
			Install libxml2-${LIBXML2_VERSION}
			Package libxml2-${LIBXML2_VERSION}
			Clean
		fi

		# boost
		if [ ! -e ${PACKAGES_DIR}/boost_${BOOST_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download boost_${BOOST_VERSION} boost_${BOOST_VERSION}.tar.bz2 http://ovh.dl.sourceforge.net/boost/boost_${BOOST_VERSION}.tar.bz2
			Patch boost_${BOOST_VERSION} ${HERE}/boost-${BOOST_VERSION}-mingw-context.patch
			InstallBinArchive zlib-${ZLIB_VERSION}-mingw32.tar.bz2 '' zlib-${ZLIB_VERSION}
			cd ${BUILD_DIR}/boost_${BOOST_VERSION}
			mkdir -p ${INSTALL_DIR}/boost_${BOOST_VERSION}
			./bootstrap.sh --without-icu || exit -1
			echo "using gcc : $(${HOST}-gcc -v 2>&1 | tail -1 | awk '{print $3}' | cut -f 1 -d '-') : ${HOST}-g++
				:
				<rc>${HOST}-windres
				<archiver>${HOST}-ar
		;" > user-config.jam
			./bjam -j ${NUM_PROCESSORS} toolset=gcc target-os=windows variant=release threading=multi threadapi=win32\
			link=shared runtime-link=shared --prefix=${INSTALL_DIR}/boost_${BOOST_VERSION} --user-config=user-config.jam \
			--without-mpi --without-python -sNO_BZIP2=1 -sZLIB_BINARY=z.dll \
			-sZLIB_INCLUDE=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/include -sZLIB_LIBPATH=${INSTALL_DIR}/zlib-${ZLIB_VERSION}/lib \
			--layout=tagged install || exit -1
			Package boost_${BOOST_VERSION}
			Clean
		fi

		# TCLTK
		if [ ! -e ${PACKAGES_DIR}/tcltk-${TCLTK_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download tcl${TCLTK_VERSION} tcl${TCLTK_VERSION}-src.tar.gz http://prdownloads.sourceforge.net/tcl/tcl${TCLTK_VERSION}-src.tar.gz
			Download tk${TCLTK_VERSION} tk${TCLTK_VERSION}-src.tar.gz http://prdownloads.sourceforge.net/tcl/tk${TCLTK_VERSION}-src.tar.gz
			cd ${BUILD_DIR}/tcl${TCLTK_VERSION}/win
			./configure --host=${HOST} --prefix=/
			make
			make install prefix=${INSTALL_DIR}/tcltk${TCLTK_VERSION} exec_prefix=${INSTALL_DIR}/tcltk${TCLTK_VERSION} libdir=${INSTALL_DIR}/tcltk${TCLTK_VERSION}/lib
			cd ${BUILD_DIR}/tk${TCLTK_VERSION}/win
			./configure --host=${HOST} --with-tcl=${BUILD_DIR}/tcl${TCLTK_VERSION}/win --prefix=/
			make
			make install prefix=${INSTALL_DIR}/tcltk${TCLTK_VERSION} exec_prefix=${INSTALL_DIR}/tcltk${TCLTK_VERSION} libdir=${INSTALL_DIR}/tcltk${TCLTK_VERSION}/lib
			Package tcltk-${TCLTK_VERSION} tcltk${TCLTK_VERSION}
			Clean
		fi
		# gperf
		if [ ! -e ${PACKAGES_DIR}/gperf-${GPERF_VERSION}-mingw32.tar.bz2 ]; then
			Clean
			Download gperf-${GPERF_VERSION} gperf-${GPERF_VERSION}.tar.gz http://ftp.gnu.org/pub/gnu/gperf/gperf-${GPERF_VERSION}.tar.gz
			Configure gperf-${GPERF_VERSION} --host=${HOST}
			Compile gperf-${GPERF_VERSION}
			Install gperf-${GPERF_VERSION}
			Package gperf-${GPERF_VERSION}
			Clean
		fi
		# netcat
		if [ ! -e ${PACKAGES_DIR}/nc-1.11-mingw32.tar.bz2 ]; then
			Download nc111nt_rodneybeede nc111nt_rodneybeede.zip http://www.rodneybeede.com/downloads/nc111nt_rodneybeede.zip
			mkdir -p ${INSTALL_DIR}/nc-1.11/opt/bin
			cp ${BUILD_DIR}/nc111nt_rodneybeede/nc.exe ${INSTALL_DIR}/nc-1.11/opt/bin/nc.exe
			Package nc-1.11
		fi
		;;
	clean)
		Clean
		rm -rf ${DOWNLOADS_DIR}/*
		rm -rf ${BUILD_DIR}/*
		rm -rf ${INSTALL_DIR}/*
		rm -rf ${PACKAGES_DIR}/*
		;;
	*)
		echo "Usage: $(basename $0) <all | clean>"
		exit -1;
esac

exit 0
