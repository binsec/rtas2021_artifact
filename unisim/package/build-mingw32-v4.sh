#!/bin/bash

VERSION=4.0
CROSS_GDB_ARCHITECTURES="powerpc-440fp-linux-gnu powerpc-7450-linux-gnu armel-linux-gnu m6811-elf"
TMP_DIR=${HOME}/tmp
INSTALL_DIR=${TMP_DIR}/mingw32_install
HERE=`pwd`
MY_DIR=$(cd $(dirname $0); pwd)

HOST=i586-pc-mingw32
BUILD=i686-pc-linux-gnu

NUM_PROCESSORS=`cat /proc/cpuinfo | cut -f 1 | grep vendor_id | wc -l`

function Package {
	PACKAGE_NAME=$1
	START=$2
	START_ICON=$3
	START_PARAMS=$4

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
	echo "AppPublisherURL=http://www.unisim-vp.org" >> ${ISS_FILENAME}
	echo "AppSupportURL=http://www.unisim-vp.org" >> ${ISS_FILENAME}
	echo "AppUpdatesURL=http://www.unisim-vp.org" >> ${ISS_FILENAME}
	echo "DefaultDirName={sd}\\${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "DefaultGroupName=${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "AllowNoIcons=yes" >> ${ISS_FILENAME}
	echo "OutputDir=dist" >> ${ISS_FILENAME}
	echo "OutputBaseFilename=setup-${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "Compression=lzma" >> ${ISS_FILENAME}
	echo "SolidCompression=yes" >> ${ISS_FILENAME}
	echo "" >> ${ISS_FILENAME}
	echo "[Languages]" >> ${ISS_FILENAME}
	echo "Name: \"english\"; MessagesFile: \"compiler:Default.isl\"" >> ${ISS_FILENAME}
	echo "" >> ${ISS_FILENAME}
	echo "[Tasks]" >> ${ISS_FILENAME}
	echo "Name: \"desktopicon\"; Description: \"{cm:CreateDesktopIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked" >> ${ISS_FILENAME}
#	echo "Name: \"quicklaunchicon\"; Description: \"{cm:CreateQuickLaunchIcon}\"; GroupDescription: \"{cm:AdditionalIcons}\"; Flags: unchecked" >> ${ISS_FILENAME}
	echo "" >> ${ISS_FILENAME}
	echo "[Files]" >> ${ISS_FILENAME}
	for file in ${file_list}
	do
		if test -f ${file}; then
			stripped_filename=`echo ${file} | sed 's:./::'`
			echo "Source: \"${stripped_filename}\"; DestDir: \"{app}/`dirname ${stripped_filename}`\"; Flags: ignoreversion" >> ${ISS_FILENAME}
		fi
	done

	if test "x${START}" != x && test "x${START_ICON}" != x; then
		echo "" >> ${ISS_FILENAME}
		echo "[Icons]" >> ${ISS_FILENAME}
		echo "Name: \"{group}\\${PACKAGE_NAME}\"; Filename: \"{app}\\${START}\"; IconFilename: \"{app}\\${START_ICON}\"; Parameters: \"${START_PARAMS}\"" >> ${ISS_FILENAME}
		echo "Name: \"{commondesktop}\\${PACKAGE_NAME}\"; Filename: \"{app}\\${START}\"; IconFilename: \"{app}\\${START_ICON}\"; Parameters: \"${START_PARAMS}\"; Tasks: desktopicon" >> ${ISS_FILENAME}
#		echo "Name: \"{userappdata}\\${PACKAGE_NAME}\"; Filename: \"{app}\\${START}\"; IconFilename: \"{app}\\${START_ICON}\"; Tasks: quicklaunchicon" >> ${ISS_FILENAME}
	fi
	
	echo "========================================="
	echo "=            Inno Setup Script          ="
	echo "========================================="
	cat ${ISS_FILENAME}
	
	# Build the package
	echo "========================================="
	echo "=            Package build              ="
	echo "========================================="

	wine ~/.wine/drive_c/Program\ Files/Inno\ Setup\ 5/ISCC.exe ${ISS_FILENAME} || exit
	cp -f ${INSTALL_DIR}/dist/setup-${PACKAGE_NAME}-${VERSION}.exe ${HERE}
	rm -rf ${INSTALL_DIR}/dist
	rm -f ${ISS_FILENAME}
}

function Patch
{
	NAME=$1
	PATCH=$2
	echo "========================================="
	echo "=              Patching                 ="
	echo "========================================="
	cd ${TMP_DIR}/${NAME}
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
	cd ${TMP_DIR}/${NAME}
#	./configure --prefix=${INSTALL_DIR} "${args[@]}" || exit
	./configure --prefix=/usr "${args[@]}" || exit
}

function Compile
{
	NAME=$1
	echo "========================================="
	echo "=              Compiling                ="
	echo "========================================="
	cd ${TMP_DIR}/${NAME}
	make -j ${NUM_PROCESSORS} all || exit
}

function Install
{
	NAME=$1
	echo "========================================="
	echo "=              Installing               ="
	echo "========================================="
	cd ${TMP_DIR}/${NAME}
	make install prefix=${INSTALL_DIR} || exit
}

function Download
{
	NAME=$1
	ARCHIVE_NAME=$2
	ARCHIVE_URL=$3

	ARCHIVE=${TMP_DIR}/${ARCHIVE_NAME}
	if test ! -f ${ARCHIVE}; then
		cd ${TMP_DIR}
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

	cd ${TMP_DIR} || exit
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
			tar xf ${ARCHIVE} || exit
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

	ARCHIVE=${TMP_DIR}/${ARCHIVE_NAME}
	if test ! -f ${ARCHIVE}; then
		cd ${TMP_DIR}
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

	echo "Unpacking ${ARCHIVE_NAME} into ${SUBDIR}"
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
			tar xf  ${ARCHIVE} || exit
			;;
	esac
}

rm -rf ${INSTALL_DIR}
mkdir -p ${INSTALL_DIR}

# Compile some missing libraries
${MY_DIR}/bootstrap-mingw32.sh all
cp -f ${HERE}/mingw32-packages/*.bz2 ${TMP_DIR}/.

# expat
# if [ ! -e ${TMP_DIR}/expat-2.0.1-mingw32.tar.bz2 ]; then
# 	Download expat-2.0.1 expat-2.0.1.tar.gz http://ovh.dl.sourceforge.net/expat/expat-2.0.1.tar.gz
# 	Configure expat-2.0.1 --host=${HOST} --build=${BUILD}
# 	Compile expat-2.0.1
# 	Install expat-2.0.1
# 	cd ${INSTALL_DIR}
# 	tar jcvf ${TMP_DIR}/expat-2.0.1-mingw32.tar.bz2 *
# 	rm -rf ${INSTALL_DIR}/*
# fi

# GDB
# if [ ! -e ${TMP_DIR}/gdb-7.2-mingw32.tar.bz2 ]; then
# 	Download gdb-7.2 gdb-7.2.tar.bz2 ftp://ftp.gnu.org/pub/gnu/gdb/gdb-7.2.tar.bz2
# 	#InstallBinArchive expat-2.0.1-mingw32.tar.bz2
# 	Configure gdb-7.2 --host=${HOST} --build=${BUILD} \
# 				--enable-targets=i686-pc-mingw32,m6811-elf,armeb-linux-gnu,powerpc-linux-gnu,mips-linux-gnu,sparc-linux-gnu \
# 				--disable-sim --disable-werror \
# 				--with-libexpat-prefix=${INSTALL_DIR}
# 	Compile gdb-7.2
# 	Install gdb-7.2
# 	cd ${INSTALL_DIR}
# 	tar jcvf ${TMP_DIR}/gdb-7.2-mingw32.tar.bz2 *
# 	rm -rf ${INSTALL_DIR}/*
# fi

# cross-GDB
# for CROSS_GDB_ARCH in ${CROSS_GDB_ARCHITECTURES}
# do
# 	if [ ! -e ${TMP_DIR}/cross-gdb-${CROSS_GDB_ARCH}-7.2-mingw32.tar.bz2 ]; then
# 		Download gdb-7.2 gdb-7.2.tar.bz2 ftp://ftp.gnu.org/pub/gnu/gdb/gdb-7.2.tar.bz2
# 		Configure gdb-7.2 --host=${HOST} --build=${BUILD} --target=${CROSS_GDB_ARCH} \
# 					--disable-sim --disable-werror
# 		Compile gdb-7.2
# 		Install gdb-7.2
# 		cd ${INSTALL_DIR}
# 		tar jcvf ${TMP_DIR}/cross-gdb-${CROSS_GDB_ARCH}-7.2-mingw32.tar.bz2 *
# 		rm -rf ${INSTALL_DIR}/*
# 	fi
# done

# SDL
# if [ ! -e ${TMP_DIR}/SDL-1.2.14-mingw32.tar.bz2 ]; then
# 	Download SDL-1.2.14 SDL-1.2.14.tar.gz http://www.libsdl.org/release/SDL-1.2.14.tar.gz
# 	#sed -i -e 's/SDL_main(/main(/g' ${TMP_DIR}/SDL-1.2.14/src/main/win32/SDL_win32_main.c
# 	Configure SDL-1.2.14 --host=${HOST} --build=${BUILD}
# 	Compile SDL-1.2.14
# 	Install SDL-1.2.14
# 	#sed -i -e 's/-lSDLmain//g' -e 's/-Dmain=SDL_main//g' -e 's/-lmingw32//g' -e 's/-mwindows//g' ${INSTALL_DIR}/bin/sdl-config
# 	#sed -i -e 's/-lSDLmain//g' -e 's/-Dmain=SDL_main//g' -e 's/-lmingw32//g' ${INSTALL_DIR}/bin/sdl-config
# 	cd ${INSTALL_DIR}
# 	tar jcvf ${TMP_DIR}/SDL-1.2.14-mingw32.tar.bz2 *
# 	rm -rf ${INSTALL_DIR}/*
# fi

# libxml2
# if [ ! -e ${TMP_DIR}/libxml2-2.7.8-mingw32.tar.bz2 ]; then
# 	Download libxml2-2.7.8 libxml2-2.7.8.tar.gz ftp://xmlsoft.org/libxml2/libxml2-2.7.8.tar.gz
# 	cd ${TMP_DIR}/libxml2-2.7.8
# 	Configure libxml2-2.7.8 --host=${HOST} --build=${BUILD} --without-python
# 	Compile libxml2-2.7.8
# 	Install libxml2-2.7.8
# 	cd ${INSTALL_DIR}
# 	tar jcvf ${TMP_DIR}/libxml2-2.7.8-mingw32.tar.bz2 *
# 	rm -rf ${INSTALL_DIR}/*
# fi

# if [ ! -e ${TMP_DIR}/boost_1_47_0-mingw32.tar.bz2 ]; then
# 	Download boost_1_47_0 boost_1_47_0.tar.bz2 http://ovh.dl.sourceforge.net/boost/boost_1_47_0.tar.bz2
# 	cd ${TMP_DIR}/boost_1_47_0
# 	rm -f g++
# 	ln -s `which ${HOST}-g++` g++
# 	rm -f gcc
# 	ln -s `which ${HOST}-gcc` gcc
# 	rm -f ar
# 	ln -s `which ${HOST}-ar` ar
# 	rm -f ranlib
# 	ln -s `which ${HOST}-ranlib` ranlib
# 	rm -f as
# 	ln -s `which ${HOST}-as` as
# 	rm -f ld
# 	ln -s `which ${HOST}-ld` ld
# 	BJAM_OPTIONS="--prefix=${INSTALL_DIR} -j ${NUM_PROCESSORS} --without-mpi"
# 	PATH=./:${PATH} ./bootstrap.sh --without-icu
# 	PATH=./:${PATH} ./bjam ${BJAM_OPTIONS}
# 	PATH=./:${PATH} ./bjam ${BJAM_OPTIONS} install
# 	cd ${INSTALL_DIR}
# 	tar jcvf ${TMP_DIR}/boost_1_47_0-mingw32.tar.bz2 *
# 	rm -rf ${INSTALL_DIR}/*
# fi

# Install MSYS and MINGW
mingw_url="http://downloads.sourceforge.net/mingw"
msys_file_list="\
msysCORE-1.0.17-1-msys-1.0.17-lic.tar.lzma \
msysCORE-1.0.17-1-msys-1.0.17-doc.tar.lzma \
libiconv-1.13.1-2-msys-1.0.13-dll-2.tar.lzma \
libopenssl-1.0.0-1-msys-1.0.13-dll-100.tar.lzma \
liblzma-4.999.9beta_20100401-1-msys-1.0.13-dll-1.tar.gz \
libltdl-2.4-1-msys-1.0.15-dll-7.tar.lzma \
libintl-0.17-2-msys-dll-8.tar.lzma \
make-3.81-3-msys-1.0.13-bin.tar.lzma \
less-436-2-msys-1.0.13-bin.tar.lzma \
texinfo-4.13a-2-msys-1.0.13-bin.tar.lzma \
tar-1.23-1-msys-1.0.13-bin.tar.lzma \
sed-4.2.1-2-msys-1.0.13-bin.tar.lzma \
xz-4.999.9beta_20100401-1-msys-1.0.13-bin.tar.gz \
gzip-1.3.12-2-msys-1.0.13-bin.tar.lzma \
grep-2.5.4-2-msys-1.0.13-bin.tar.lzma \
libmagic-5.04-1-msys-1.0.13-dll-1.tar.lzma \
file-5.04-1-msys-1.0.13-bin.tar.lzma \
findutils-4.4.2-2-msys-1.0.13-bin.tar.lzma \
diffutils-2.8.7.20071206cvs-3-msys-1.0.13-bin.tar.lzma \
gawk-3.1.7-2-msys-1.0.13-bin.tar.lzma \
libpopt-1.15-2-msys-1.0.13-dll-0.tar.lzma \
cygutils-dos2unix-1.3.4-4-msys-1.0.13-bin.tar.lzma \
vim-7.3-2-msys-1.0.16-bin.tar.lzma \
perl-5.8.8-1-msys-1.0.17-bin.tar.lzma \
libminires-1.02_1-2-msys-1.0.13-dll.tar.lzma \
patch-2.6.1-1-msys-1.0.13-bin.tar.lzma \
openssl-1.0.0-1-msys-1.0.13-bin.tar.lzma \
openssh-5.4p1-1-msys-1.0.13-bin.tar.lzma \
mktemp-1.6-2-msys-1.0.13-bin.tar.lzma \
m4-1.4.14-1-msys-1.0.13-bin.tar.lzma \
coreutils-5.97-3-msys-1.0.13-bin.tar.lzma \
libexpat-2.0.1-1-msys-1.0.13-dll-1.tar.lzma \
libgmp-5.0.1-1-msys-1.0.13-dll-10.tar.lzma \
libcrypt-1.1_1-3-msys-1.0.13-dll-0.tar.lzma \
libgdbm-1.8.3-3-msys-1.0.13-dll-3.tar.lzma \
libbz2-1.0.5-2-msys-1.0.13-dll-1.tar.lzma \
zlib-1.2.3-2-msys-1.0.13-dll.tar.lzma \
msysCORE-1.0.17-1-msys-1.0.17-bin.tar.lzma \
termcap-0.20050421_1-2-msys-1.0.13-bin.tar.lzma \
libguile-1.8.7-2-msys-1.0.15-rtm.tar.lzma \
libregex-1.20090805-2-msys-1.0.13-dll-1.tar.lzma \
libtermcap-0.20050421_1-2-msys-1.0.13-dll-0.tar.lzma \
msysCORE-1.0.17-1-msys-1.0.17-ext.tar.lzma \
bzip2-1.0.5-2-msys-1.0.13-bin.tar.lzma \
bash-3.1.17-4-msys-1.0.16-bin.tar.lzma \
libopts-5.10.1-1-msys-1.0.15-dll-25.tar.lzma \
libguile-1.8.7-2-msys-1.0.15-dll-17.tar.lzma \
libarchive-2.8.3-1-msys-1.0.13-dll-2.tar.lzma \
lndir-1.0.1-2-msys-1.0.13-bin.tar.lzma \
inetutils-1.7-1-msys-1.0.13-bin.tar.lzma \
guile-1.8.7-2-msys-1.0.15-bin.tar.lzma \
flex-2.5.35-2-msys-1.0.13-bin.tar.lzma \
cvs-1.12.13-2-msys-1.0.13-bin.tar.lzma \
coreutils-5.97-3-msys-1.0.13-ext.tar.lzma \
bsdcpio-2.8.3-1-msys-1.0.13-bin.tar.lzma \
bsdtar-2.8.3-1-msys-1.0.13-bin.tar.lzma \
bison-2.4.2-1-msys-1.0.13-bin.tar.lzma \
autogen-5.10.1-1-msys-1.0.15-bin.tar.lzma \
libregex-1.20090805-2-msys-1.0.13-dev.tar.lzma \
libiconv-1.13.1-2-msys-1.0.13-dev.tar.lzma \
libopts-5.10.1-1-msys-1.0.15-dev.tar.lzma \
libmagic-5.04-1-msys-1.0.13-dev.tar.lzma \
inetutils-1.7-1-msys-1.0.13-dev.tar.lzma \
libgdbm-1.8.3-3-msys-1.0.13-dev.tar.lzma \
gdbm-1.8.3-3-msys-1.0.13-bin.tar.lzma \
libcrypt-1.1_1-3-msys-1.0.13-dev.tar.lzma \
crypt-1.1_1-3-msys-1.0.13-bin.tar.lzma \
lpr-enhanced-1.0.2-1-msys-1.0.11-bin.tar.lzma \
libcharset-1.13.1-2-msys-1.0.13-dll-1.tar.lzma \
libtermcap-0.20050421_1-2-msys-1.0.13-dev.tar.lzma \
rxvt-2.7.2-3-msys-1.0.14-bin.tar.lzma \
libbz2-1.0.5-2-msys-1.0.13-dev.tar.lzma \
libbz2-1.0.5-2-msys-1.0.13-dll-1.tar.lzma \
libpopt-1.15-2-msys-1.0.13-dev.tar.lzma \
libguile-1.8.7-2-msys-1.0.15-dev.tar.lzma \
unzip-6.0-1-msys-1.0.13-bin.tar.lzma \
wget-1.12-1-msys-1.0.13-lang.tar.lzma \
wget-1.12-1-msys-1.0.13-bin.tar.lzma \
locate-4.4.2-2-msys-1.0.13-bin.tar.lzma \
findutils-4.4.2-2-msys-1.0.13-lang.tar.lzma \
texinfo-4.13a-2-msys-1.0.13-lang.tar.lzma \
libopenssl-1.0.0-1-msys-1.0.13-dev.tar.lzma \
bison-2.4.2-1-msys-1.0.13-lang.tar.lzma \
flex-2.5.35-2-msys-1.0.13-lang.tar.lzma \
libminires-1.02_1-2-msys-1.0.13-dev.tar.lzma \
minires-1.02_1-2-msys-1.0.13-bin.tar.lzma \
rebase-3.0.1_1-2-msys-1.0.15-bin.tar.lzma \
dash-0.5.5.1_2-1-msys-1.0.13-bin.tar.lzma \
zip-3.0-1-msys-1.0.14-bin.tar.lzma \
"

mingw_file_list="\
mingw-get-0.3-mingw32-alpha-1-bin.tar.gz \
mingw-get-0.3-mingw32-alpha-1-lic.tar.gz \
pkginfo-0.3-mingw32-alpha-1-bin.tar.gz \
libiconv-1.13.1-1-mingw32-dll-2.tar.lzma \
libintl-0.17-1-mingw32-dll-8.tar.lzma \
mingwrt-3.18-mingw32-dll.tar.gz \
libgmp-5.0.1-1-mingw32-dll-10.tar.lzma \
libmpfr-2.4.1-1-mingw32-dll-1.tar.lzma \
libpthread-2.8.0-3-mingw32-dll-2.tar.lzma \
pthreads-w32-2.8.0-3-mingw32-dev.tar.lzma \
libgomp-4.5.2-1-mingw32-dll-1.tar.lzma \
libmpc-0.8.1-1-mingw32-dll-2.tar.lzma \
libssp-4.5.2-1-mingw32-dll-0.tar.lzma \
libgettextpo-0.17-1-mingw32-dll-0.tar.lzma \
libgcc-4.5.2-1-mingw32-dll-1.tar.lzma \
w32api-3.17-2-mingw32-dev.tar.lzma \
mingwrt-3.18-mingw32-dev.tar.gz \
binutils-2.21-3-mingw32-bin.tar.lzma \
libexpat-2.0.1-1-mingw32-dll-1.tar.gz \
gdb-7.2-1-mingw32-bin.tar.lzma \
make-3.82-5-mingw32-bin.tar.lzma \
gcc-4.5.2-1-mingw32-lic.tar.lzma \
libstdc++-4.5.2-1-mingw32-dll-6.tar.lzma \
libgfortran-4.5.2-1-mingw32-dll-3.tar.lzma \
libobjc-4.5.2-1-mingw32-dll-2.tar.lzma \
gcc-core-4.5.2-1-mingw32-bin.tar.lzma \
gcc-c++-4.5.2-1-mingw32-bin.tar.lzma \
gcc-fortran-4.5.2-1-mingw32-bin.tar.lzma \
gcc-objc-4.5.2-1-mingw32-bin.tar.lzma \
gdb-7.2-1-mingw32-bin.tar.lzma \
libltdl-2.4-1-mingw32-dll-7.tar.lzma \
libiconv-1.13.1-1-mingw32-dev.tar.lzma \
libiconv-1.13.1-1-mingw32-bin.tar.lzma \
gettext-0.17-1-mingw32-dev.tar.lzma \
gettext-0.17-1-mingw32-bin.tar.lzma \
libltdl-2.4-1-mingw32-dev.tar.lzma \
libtool-2.4-1-mingw32-bin.tar.lzma \
automake1.11-1.11.1-1-mingw32-bin.tar.lzma \
automake1.10-1.10.2-1-mingw32-bin.tar.lzma \
automake1.9-1.9.6-3-mingw32-bin.tar.lzma \
automake1.8-1.8.5-1-mingw32-bin.tar.lzma \
automake1.7-1.7.9-1-mingw32-bin.tar.lzma \
automake1.6-1.6.3-1-mingw32-bin.tar.lzma \
automake1.5-1.5-1-mingw32-bin.tar.lzma \
automake1.4-1.4p6-1-mingw32-bin.tar.lzma \
automake-4-1-mingw32-bin.tar.lzma \
autoconf2.5-2.67-1-mingw32-bin.tar.lzma \
autoconf2.1-2.13-4-mingw32-bin.tar.lzma \
autoconf-9-1-mingw32-bin.tar.lzma \
libz-1.2.5-1-mingw32-dll-1.tar.lzma \
libz-1.2.5-1-mingw32-dev.tar.lzma \
libpdcurses-3.4-1-mingw32-dll.tar.lzma \
PDCurses-3.4-1-mingw32-bin.tar.lzma \
libpdcurses-3.4-1-mingw32-dev.tar.lzma \
mpc-0.8.1-1-mingw32-dev.tar.lzma \
bsdtar-2.8.3-1-mingw32-bin.tar.bz2 \
bsdcpio-2.8.3-1-mingw32-bin.tar.bz2 \
libarchive-2.8.3-1-mingw32-dll-2.tar.bz2 \
libarchive-2.8.3-1-mingw32-dev.tar.bz2 \
liblzma-4.999.9beta_20100401-1-mingw32-dll-1.tar.bz2 \
liblzma-4.999.9beta_20100401-1-mingw32-dev.tar.bz2 \
xz-4.999.9beta_20100401-1-mingw32-bin.tar.bz2 \
xz-4.999.9beta_20100401-1-mingw32-lang.tar.bz2 \
mpfr-2.4.1-1-mingw32-dev.tar.lzma \
gmp-5.0.1-1-mingw32-dev.tar.lzma \
libgmpxx-5.0.1-1-mingw32-dll-4.tar.lzma \
libexpat-2.0.1-1-mingw32-dev.tar.gz \
expat-2.0.1-1-mingw32-bin.tar.gz \
gendef-1.0.1346-1-mingw32-bin.tar.lzma \
pexports-0.44-1-mingw32-bin.tar.lzma \
cygutils-dos2unix-1.3.4-1-mingw32-bin.tar.lzma \
mingw-catgets-1.0-dev.tar.gz \
mingw-catgets-1.0-bin.tar.gz \
libcharset-1.13.1-1-mingw32-dll-1.tar.lzma \
libbz2-1.0.6-1-mingw32-dll-2.tar.lzma \
bzip2-1.0.6-1-mingw32-dev.tar.lzma \
bzip2-1.0.6-1-mingw32-bin.tar.lzma \
libz-1.2.5-1-mingw32-dll-1.tar.lzma \
libz-1.2.5-1-mingw32-dev.tar.lzma \
libltdl-2.4-1-mingw32-dll-7.tar.lzma \
libltdl-2.4-1-mingw32-dev.tar.lzma \
make-3.82-5-mingw32-bin.tar.lzma \
make-3.82-5-mingw32-lang.tar.lzma \
"

for file in ${msys_file_list}
do
	InstallBinArchive ${file} ${mingw_url}/${file}
done

mkdir -p ${INSTALL_DIR}/mingw

for file in ${mingw_file_list}
do
	InstallBinArchive ${file} ${mingw_url}/${file} mingw
done

# Install cross-compiled packages
SDL_VERSION=1.2.14
LIBXML2_VERSION=2.7.8
EXPAT_VERSION=2.0.1
CROSS_GDB_VERSION=7.2a
ZLIB_VERSION=1.2.5
BOOST_VERSION=1_47_0
TCLTK_VERSION=8.5.11
GPERF_VERSION=3.0.4
InstallBinArchive expat-${EXPAT_VERSION}-mingw32.tar.bz2 '.' mingw
InstallBinArchive zlib-${ZLIB_VERSION}-mingw32.tar.bz2 '.' mingw
InstallBinArchive libxml2-${LIBXML2_VERSION}-mingw32.tar.bz2 '.' mingw
sed -i "s#^\(prefix=\).*\$#\1/mingw#" ${INSTALL_DIR}/mingw/bin/xml2-config
sed -i "s# -L/.* -lz# -L/mingw/lib -lz#g" ${INSTALL_DIR}/mingw/bin/xml2-config
sed -i "s# -L/.* -lz# -L/mingw/lib -lz#g" ${INSTALL_DIR}/mingw/lib/libxml2.la
printf "#!/lib/sh\n" | cat - ${INSTALL_DIR}/mingw/lib/xml2Conf.sh > ${INSTALL_DIR}/mingw/lib/tmp.out && mv -f ${INSTALL_DIR}/mingw/lib/tmp.out ${INSTALL_DIR}/mingw/lib/xml2Conf.sh
sed -i "s#^\(XML2_LIBDIR=\).*\$#\1\"-L/mingw/lib\"#" ${INSTALL_DIR}/mingw/lib/xml2Conf.sh
sed -i "s#^\(XML2_LIBS=\).*\$#\1\"-lxml2 -L/mingw/lib -lz\"#" ${INSTALL_DIR}/mingw/lib/xml2Conf.sh
sed -i "s#^\(XML2_INCLUDEDIR=\).*\$#\1\"-I/mingw/include/libxml2\"#" ${INSTALL_DIR}/mingw/lib/xml2Conf.sh
chmod +x ${INSTALL_DIR}/mingw/lib/xml2Conf.sh
sed -i "s#^\(Libs\.private:\).*\$#\1 -L/mingw/lib -lz -lws2_32#" ${INSTALL_DIR}/mingw/lib/pkgconfig/libxml-2.0.pc
InstallBinArchive boost_${BOOST_VERSION}-mingw32.tar.bz2 '.' mingw
InstallBinArchive SDL-${SDL_VERSION}-mingw32.tar.bz2 '.' mingw
InstallBinArchive cross-gdb-powerpc-440fp-linux-gnu-${CROSS_GDB_VERSION}-mingw32.tar.bz2 '.' mingw
InstallBinArchive cross-gdb-powerpc-7450-linux-gnu-${CROSS_GDB_VERSION}-mingw32.tar.bz2 '.' mingw
InstallBinArchive cross-gdb-armel-linux-gnu-${CROSS_GDB_VERSION}-mingw32.tar.bz2 '.' mingw
InstallBinArchive cross-gdb-m6811-elf-${CROSS_GDB_VERSION}-mingw32.tar.bz2 '.' mingw
InstallBinArchive tcltk-${TCLTK_VERSION}-mingw32.tar.bz2 '.'
printf "#!/bin/sh\n" | cat - ${INSTALL_DIR}/lib/tclConfig.sh > ${INSTALL_DIR}/lib/tmp.out && mv ${INSTALL_DIR}/lib/tmp.out ${INSTALL_DIR}/lib/tclConfig.sh
sed -i "s#^\(TCL_BUILD_LIB_SPEC=\).*\$#\1'-L/lib -ltcl85'#" ${INSTALL_DIR}/lib/tclConfig.sh
sed -i "s#^\(TCL_BUILD_STUB_LIB_SPEC=\).*\$#\1'-L/lib -ltclstub85'#" ${INSTALL_DIR}/lib/tclConfig.sh
sed -i "s#^\(TCL_BUILD_STUB_LIB_PATH=\).*\$#\1'/lib/libtclstub85.a'#" ${INSTALL_DIR}/lib/tclConfig.sh
sed -i "s#^\(TCL_SRC_DIR=\).*\$#\1'/include'#" ${INSTALL_DIR}/lib/tclConfig.sh
printf "#!/bin/sh\n" | cat - ${INSTALL_DIR}/lib/tkConfig.sh > ${INSTALL_DIR}/lib/tmp.out && mv ${INSTALL_DIR}/lib/tmp.out ${INSTALL_DIR}/lib/tkConfig.sh
sed -i "s#^\(TK_BUILD_LIB_SPEC=\).*\$#\1'-L/lib -ltk85'#" ${INSTALL_DIR}/lib/tkConfig.sh
sed -i "s#^\(TK_BUILD_STUB_LIB_SPEC=\).*\$#\1'-L/lib -ltkstub85'#" ${INSTALL_DIR}/lib/tkConfig.sh
sed -i "s#^\(TK_BUILD_STUB_LIB_PATH=\).*\$#\1'/lib/libtkstub85.a'#" ${INSTALL_DIR}/lib/tkConfig.sh
sed -i "s#^\(TK_SRC_DIR=\).*\$#\1'/include'#" ${INSTALL_DIR}/lib/tkConfig.sh
chmod +x ${INSTALL_DIR}/lib/tclConfig.sh
chmod +x ${INSTALL_DIR}/lib/tkConfig.sh
cp -f ${INSTALL_DIR}/lib/tclConfig.sh ${INSTALL_DIR}/bin
cp -f ${INSTALL_DIR}/lib/tkConfig.sh ${INSTALL_DIR}/bin
InstallBinArchive gperf-${GPERF_VERSION}-mingw32.tar.bz2 '.' mingw
InstallBinArchive nc-1.11-mingw32.tar.bz2 '.'

# cp -rf ${INSTALL_DIR}/usr/local ${INSTALL_DIR}/.
# cp -rf ${INSTALL_DIR}/usr/spool ${INSTALL_DIR}/.
# cp -rf ${INSTALL_DIR}/coreutils-5.97/* ${INSTALL_DIR}/.
# rm -rf ${INSTALL_DIR}/usr
# rm -rf ${INSTALL_DIR}/coreutils-5.97
#cp -rf ${INSTALL_DIR}/usr/* ${INSTALL_DIR}/.
#cp -rf ${INSTALL_DIR}/coreutils-5.97/* ${INSTALL_DIR}/.
rm -rf ${INSTALL_DIR}/usr
rm -rf ${INSTALL_DIR}/coreutils-5.97

# Install some addtional utilities from gnuwin32

InstallBinArchive unrar-bin.zip http://gnuwin32.sourceforge.net/downlinks/unrar-bin-zip.php opt
#InstallBinArchive unzip-bin.zip http://gnuwin32.sourceforge.net/downlinks/unzip-bin-zip.php
#InstallBinArchive zip-bin.zip http://gnuwin32.sourceforge.net/downlinks/zip-bin-zip.php
#InstallBinArchive libintl-bin.zip http://gnuwin32.sourceforge.net/downlinks/libintl-bin-zip.php
#InstallBinArchive libiconv-bin.zip http://gnuwin32.sourceforge.net/downlinks/libiconv-bin-zip.php
#InstallBinArchive wget-bin.zip http://gnuwin32.sourceforge.net/downlinks/wget-bin-zip.php
InstallBinArchive libiconv-1.9.2-1-bin.zip http://gnuwin32.sourceforge.net/downlinks/libiconv-bin-zip.php opt
InstallBinArchive libintl-0.14.4-bin.zip http://gnuwin32.sourceforge.net/downlinks/libintl-bin-zip.php opt
InstallBinArchive util-linux-ng-2.14.1-bin.zip http://downloads.sourceforge.net/gnuwin32/util-linux-ng-2.14.1-bin.zip opt

# Install subversion client
InstallBinArchive svn-win32-1.3.2.zip http://subversion.tigris.org/files/documents/15/32473/svn-win32-1.3.2.zip
rm -f ${INSTALL_DIR}/svn-win32-1.3.2/README.txt
cp -rf ${INSTALL_DIR}/svn-win32-1.3.2/* ${INSTALL_DIR}/.
rm -rf ${INSTALL_DIR}/svn-win32-1.3.2

# Install cmake
InstallBinArchive cmake-2.8.1-win32-x86.zip http://www.cmake.org/files/v2.8/cmake-2.8.1-win32-x86.zip
cp -rf ${INSTALL_DIR}/cmake-2.8.1-win32-x86/* ${INSTALL_DIR}/.
rm -rf ${INSTALL_DIR}/cmake-2.8.1-win32-x86

# Install gtk+
InstallBinArchive gtk+-bundle_2.24.10-20120208_win32.zip http://ftp.gnome.org/pub/gnome/binaries/win32/gtk+/2.24/gtk+-bundle_2.24.10-20120208_win32.zip mingw
rm -f ${INSTALL_DIR}/mingw/gtk+-bundle_2.24.10-20120208_win32.README.txt

# Install python 3.1.2
# if [ ! -e ${TMP_DIR}/python-3.1.2-mingw32.tar.bz2 ]; then
# 	cd ${TMP_DIR}
# 	printf "Downloading python 3.1.2"
# 	wget http://www.python.org/ftp/python/3.1.2/python-3.1.2.msi
# 	wine msiexec.exe /qn /i ${TMP_DIR}/python-3.1.2.msi TARGETDIR=${INSTALL_DIR}/python3.1.2
# 	cd ~/.wine/drive_c
# 	tar jcvf ${TMP_DIR}/python-3.1.2-mingw32.tar.bz2 python-3.1.2
# 	wine msiexec.exe /qn /x ${TMP_DIR}/python-3.1.2.msi TARGETDIR=${INSTALL_DIR}/python3.1.2
# 	rm -rf ${INSTALL_DIR}/python3.1.2
# 	mv -f python-3.1.2-mingw32.tar.bz2 ${TMP_DIR}/python-3.1.2-mingw32.tar.bz2
# fi

# Fix msys.bat to include /opt/bin in the PATH
sed -i '1 i \@set PATH=/opt/bin;%PATH%' "${INSTALL_DIR}/msys.bat"

# Package everything into a single .EXE installer
Package mingw32-unisim-pack msys.bat m.ico "--norxvt"
