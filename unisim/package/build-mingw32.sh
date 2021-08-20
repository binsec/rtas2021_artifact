#!/bin/bash

VERSION=1.0
TMP_DIR=${HOME}/tmp
INSTALL_DIR=${TMP_DIR}/mingw32_install
HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

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
	echo "AppPublisherURL=http://www.unisim.org" >> ${ISS_FILENAME}
	echo "AppSupportURL=http://www.unisim.org" >> ${ISS_FILENAME}
	echo "AppUpdatesURL=http://www.unisim.org" >> ${ISS_FILENAME}
	echo "DefaultDirName={sd}\\${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "DefaultGroupName=${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "AllowNoIcons=yes" >> ${ISS_FILENAME}
	echo "OutputDir=dist" >> ${ISS_FILENAME}
	echo "OutputBaseFilename=${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
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
	cp -f ${INSTALL_DIR}/dist/${PACKAGE_NAME}-${VERSION}.exe ${HERE}
	rm -rf ${INSTALL_DIR}/dist
	rm -f ${ISS_FILENAME}
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
	esac
}

function InstallBinArchive
{
	ARCHIVE_NAME=$1
	ARCHIVE_URL=$2

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

	echo "Unpacking ${ARCHIVE_NAME}"
	cd ${INSTALL_DIR}
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
	esac
}

rm -rf ${INSTALL_DIR}
mkdir -p ${INSTALL_DIR}

# Compile some missing libraries

# expat
Download expat-2.0.1 expat-2.0.1.tar.gz http://ovh.dl.sourceforge.net/expat/expat-2.0.1.tar.gz
Configure expat-2.0.1 --host=i586-mingw32msvc --build=i686-pc-linux-gnu
Compile expat-2.0.1
Install expat-2.0.1

# GDB
Download gdb-6.8 gdb-6.8.tar.bz2 ftp://ftp.gnu.org/pub/gnu/gdb/gdb-6.8.tar.bz2
Configure gdb-6.8 --host=i586-mingw32msvc --build=i686-pc-linux-gnu \
            --enable-targets=i686-pc-mingw32,m6811-elf,armeb-linux-gnu,powerpc-linux-gnu,mips-linux-gnu,sparc-linux-gnu \
            --disable-sim --disable-werror \
            --with-libexpat-prefix=${INSTALL_DIR}
Compile gdb-6.8
Install gdb-6.8

# SDL
Download SDL-1.2.13 SDL-1.2.13.tar.gz http://www.libsdl.org/release/SDL-1.2.13.tar.gz
Configure SDL-1.2.13 --host=i586-mingw32msvc --build=i686-pc-linux-gnu
Compile SDL-1.2.13
Install SDL-1.2.13

# libxml2
Download libxml2-2.6.31 libxml2-2.6.31.tar.gz ftp://xmlsoft.org/libxml2/libxml2-2.6.31.tar.gz
cd ${TMP_DIR}/libxml2-2.6.31
Configure libxml2-2.6.31 --host=i586-mingw32msvc --build=i686-pc-linux-gnu --without-python
Compile libxml2-2.6.31
Install libxml2-2.6.31

# boost
Download boost_1_34_1 boost_1_34_1.tar.bz2 http://ovh.dl.sourceforge.net/boost/boost_1_34_1.tar.bz2
cd ${TMP_DIR}/boost_1_34_1
rm -f g++
ln -s `which i586-mingw32msvc-g++` g++
rm -f gcc
ln -s `which i586-mingw32msvc-gcc` gcc
rm -f ar
ln -s `which i586-mingw32msvc-ar` ar
rm -f ranlib
ln -s `which i586-mingw32msvc-ranlib` ranlib
rm -f as
ln -s `which i586-mingw32msvc-as` as
rm -f ld
ln -s `which i586-mingw32msvc-ld` ld
PATH=./:${PATH} ./configure --with-toolset=gcc --prefix=${INSTALL_DIR} --without-icu --with-libraries=graph,thread
PATH=./:${PATH} make BJAM_CONFIG="-j ${NUM_PROCESSORS}"
PATH=./:${PATH} make install
cd ${INSTALL_DIR}/include
mv boost-1_34_1/boost boost
rmdir boost-1_34_1
cd ${INSTALL_DIR}/lib
ln -s libboost_thread-mgw34-mt-1_34_1.a libboost_thread.a

# SystemC
# Note: for a licence issue, SystemC should not be included in that package

# Download systemc-2.2.0 systemc-2.2.0.tar.gz http://panoramis.free.fr/search.systemc.org/download/sc220/systemc-2.2.0.tgz
# cd ${TMP_DIR}/systemc-2.2.0
# cat ${MY_DIR}/patch-systemc-2.2.0-mingw32 | patch -p1
# mkdir -p objdir
# cd objdir
# ../configure --prefix=${INSTALL_DIR} --host=i586-mingw32msvc CXX=i586-mingw32msvc-g++ AS=i586-mingw32msvc-as
# make -j ${NUM_PROCESSORS} all
# make install
# mv ${INSTALL_DIR}/lib-mingw32/* ${INSTALL_DIR}/lib/.
# rm -rf ${INSTALL_DIR}/lib-linux
# mkdir -p ${INSTALL_DIR}/share
# mkdir -p ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/AUTHORS ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/ChangeLog ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/COPYING ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/docs ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/examples ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/INSTALL ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/LICENSE ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/NEWS ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/README ${INSTALL_DIR}/share/systemc-2.2.0
# mv ${INSTALL_DIR}/RELEASENOTES ${INSTALL_DIR}/share/systemc-2.2.0

# Install MSYS and MINGW
mingw_url="http://downloads.sourceforge.net/mingw"
mingw_file_list="binutils-2.18.50-20080109-2.tar.gz \
gcc-core-3.4.5-20060117-2.tar.gz \
gcc-g++-3.4.5-20060117-2.tar.gz \
w32api-3.11.tar.gz \
mingw-runtime-3.14.tar.gz \
bash-3.1-MSYS-1.0.11-1.tar.bz2 \
bzip2-1.0.3-MSYS-1.0.11-1.tar.bz2 \
coreutils-5.97-MSYS-1.0.11-snapshot.tar.bz2 \
csmake-3.81-MSYS-1.0.11-2.tar.bz2 \
diffutils-2.8.7-MSYS-1.0.11-1.tar.bz2 \
findutils-4.3-MSYS-1.0.11-1.tar.bz2 \
gawk-3.1.5-MSYS-1.0.11-1.tar.bz2 \
lzma-4.43-MSYS-1.0.11-1-bin.tar.gz \
make-3.81-MSYS-1.0.11-2.tar.bz2 \
MSYS-1.0.11-20071204.tar.bz2 \
msysCORE-1.0.11-2007.01.19-1.tar.bz2 \
tar-1.19.90-MSYS-1.0.11-1-bin.tar.gz \
texinfo-4.11-MSYS-1.0.11-1.tar.bz2 \
autoconf2.5-2.61-1-bin.tar.bz2 \
autoconf-4-1-bin.tar.bz2 \
autogen-5.9.2-MSYS-1.0.11-1-bin.tar.gz \
autogen-5.9.2-MSYS-1.0.11-1-dev.tar.gz \
autogen-5.9.2-MSYS-1.0.11-1-dll25.tar.gz \
automake1.10-1.10-1-bin.tar.bz2 \
automake-3-1-bin.tar.bz2 \
bison-2.3-MSYS-1.0.11-1.tar.bz2 \
crypt-1.1-1-MSYS-1.0.11-1.tar.bz2 \
cvs-1.11.22-MSYS-1.0.11-1-bin.tar.gz \
flex-2.5.33-MSYS-1.0.11-1.tar.bz2 \
gdbm-1.8.3-MSYS-1.0.11-1.tar.bz2 \
gettext-0.16.1-1-bin.tar.bz2 \
gettext-0.16.1-1-dll.tar.bz2 \
gettext-0.16.1-MSYS-1.0.11-1.tar.bz2 \
gettext-devel-0.16.1-MSYS-1.0.11-1.tar.bz2 \
gmp-4.2.2-MSYS-1.0.11-1-dev.tar.gz \
gmp-4.2.2-MSYS-1.0.11-1-dll3.tar.gz \
guile-1.8.4-MSYS-1.0.11-1-bin.tar.gz \
guile-1.8.4-MSYS-1.0.11-1-dev.tar.gz \
guile-1.8.4-MSYS-1.0.11-1-dll17.tar.gz \
guile-1.8.4-MSYS-1.0.11-1-doc.tar.gz \
inetutils-1.3.2-40-MSYS-1.0.11-2-bin.tar.gz \
libiconv-1.11-1-bin.tar.bz2 \
libiconv-1.11-1-dll.tar.bz2 \
libiconv-1.11-MSYS-1.0.11-1.tar.bz2 \
libtool1.5-1.5.25a-1-bin.tar.bz2 \
libtool1.5-1.5.25a-1-dll.tar.bz2 \
lndir-6.8.1.0-MSYS-1.0.11-1.tar.bz2 \
lpr-1.0.1-MSYS.tar.gz \
minires-1.01-1-MSYS-1.0.11-1.tar.bz2 \
openssh-4.7p1-MSYS-1.0.11-1-bin.tar.gz \
openssl-0.9.8g-1-MSYS-1.0.11-2-bin.tar.gz \
openssl-0.9.8g-1-MSYS-1.0.11-2-dev.tar.gz \
openssl-0.9.8g-1-MSYS-1.0.11-2-dll098.tar.gz \
perl-5.6.1-MSYS-1.0.11-1.tar.bz2 \
perl-man-5.6.1-MSYS-1.0.11-1.tar.bz2 \
regex-0.12-MSYS-1.0.11-1.tar.bz2 \
vim-7.1-MSYS-1.0.11-1-bin.tar.gz \
zlib-1.2.3-MSYS-1.0.11-1.tar.bz2 \
gawk-3.1.5-MSYS-1.0.11-1.tar.bz2 \
m4-1.4.7-MSYS.tar.bz2"

for file in ${mingw_file_list}
do
	if test ! -f ${TMP_DIR}/${file}; then
		cd ${TMP_DIR}
		printf "Downloading ${mingw_url}/${file}"
		success=no
		until [ $success = yes ]; do
			if wget --timeout=60 -t 1 -q ${mingw_url}/${file}; then
				success=yes
			else
				printf "."
			fi
		done
		printf "\n"
	fi
	if test -f ${TMP_DIR}/${file}; then
		echo "Unpacking ${file}"
		cd ${INSTALL_DIR}
		ext=`echo "${TMP_DIR}/${file}" | awk -F . '{print $NF}'`
		case ${ext} in
			bz2)
				tar jxf ${TMP_DIR}/${file} || exit
				;;
			gz)
				tar zxf ${TMP_DIR}/${file} || exit
				;;
		esac
	fi
done

# cp -rf ${INSTALL_DIR}/usr/local ${INSTALL_DIR}/.
# cp -rf ${INSTALL_DIR}/usr/spool ${INSTALL_DIR}/.
# cp -rf ${INSTALL_DIR}/coreutils-5.97/* ${INSTALL_DIR}/.
# rm -rf ${INSTALL_DIR}/usr
# rm -rf ${INSTALL_DIR}/coreutils-5.97
cp -rf ${INSTALL_DIR}/usr/* ${INSTALL_DIR}/.
cp -rf ${INSTALL_DIR}/coreutils-5.97/* ${INSTALL_DIR}/.
rm -rf ${INSTALL_DIR}/usr
rm -rf ${INSTALL_DIR}/coreutils-5.97

# Install some addtional utilities from gnuwin32

InstallBinArchive file-bin.zip http://gnuwin32.sourceforge.net/downlinks/file-bin-zip.php
InstallBinArchive unrar-bin.zip http://gnuwin32.sourceforge.net/downlinks/unrar-bin-zip.php
InstallBinArchive unzip-bin.zip http://gnuwin32.sourceforge.net/downlinks/unzip-bin-zip.php
InstallBinArchive zip-bin.zip http://gnuwin32.sourceforge.net/downlinks/zip-bin-zip.php
InstallBinArchive libintl-bin.zip http://gnuwin32.sourceforge.net/downlinks/libintl-bin-zip.php
InstallBinArchive libiconv-bin.zip http://gnuwin32.sourceforge.net/downlinks/libiconv-bin-zip.php
InstallBinArchive wget-bin.zip http://gnuwin32.sourceforge.net/downlinks/wget-bin-zip.php
InstallBinArchive regex-2.7-bin.zip http://gnuwin32.sourceforge.net/downlinks/regex-bin-zip.php
InstallBinArchive zlib-1.2.3-bin.zip http://gnuwin32.sourceforge.net/downlinks/zlib-bin-zip.php

# Install subversion client
InstallBinArchive svn-win32-1.3.2.zip http://subversion.tigris.org/files/documents/15/32473/svn-win32-1.3.2.zip
cp -rf ${INSTALL_DIR}/svn-win32-1.3.2/* ${INSTALL_DIR}/.
rm -rf ${INSTALL_DIR}/svn-win32-1.3.2

# Package everything into a single .EXE installer
Package mingw32-unisim-pack msys.bat m.ico "--norxvt"

