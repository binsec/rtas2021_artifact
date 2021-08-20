#!/bin/bash

VERSION=2.0
TMP_DIR=${HOME}/tmp
INSTALL_DIR=${TMP_DIR}/mingw32_install
HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

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
	echo "AppPublisherURL=http://www.unisim-vp.com" >> ${ISS_FILENAME}
	echo "AppSupportURL=http://www.unisim-vp.com/support.html" >> ${ISS_FILENAME}
	echo "AppUpdatesURL=http://www.unisim-vp.com/mingw.html" >> ${ISS_FILENAME}
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
			tar Yxf ${ARCHIVE} || exit
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
		lzma)
			tar Yxf ${ARCHIVE} || exit
			;;
	esac
}

rm -rf ${INSTALL_DIR}
mkdir -p ${INSTALL_DIR}

# Compile some missing libraries

# expat
if [ ! -e ${TMP_DIR}/expat-2.0.1-mingw32.tar.bz2 ]; then
	Download expat-2.0.1 expat-2.0.1.tar.gz http://ovh.dl.sourceforge.net/expat/expat-2.0.1.tar.gz
	Configure expat-2.0.1 --host=${HOST} --build=${BUILD}
	Compile expat-2.0.1
	Install expat-2.0.1
	cd ${INSTALL_DIR}
	tar jcvf ${TMP_DIR}/expat-2.0.1-mingw32.tar.bz2 *
	rm -rf ${INSTALL_DIR}/*
fi

# GDB
if [ ! -e ${TMP_DIR}/gdb-7.0-mingw32.tar.bz2 ]; then
	Download gdb-7.0 gdb-7.0.tar.bz2 ftp://ftp.gnu.org/pub/gnu/gdb/gdb-7.0.tar.bz2
	InstallBinArchive expat-2.0.1-mingw32.tar.bz2
	Configure gdb-7.0 --host=${HOST} --build=${BUILD} \
				--enable-targets=i686-pc-mingw32,m6811-elf,armeb-linux-gnu,powerpc-linux-gnu,mips-linux-gnu,sparc-linux-gnu \
				--disable-sim --disable-werror \
				--with-libexpat-prefix=${INSTALL_DIR}
	Compile gdb-7.0
	Install gdb-7.0
	cd ${INSTALL_DIR}
	tar jcvf ${TMP_DIR}/gdb-7.0-mingw32.tar.bz2 *
	rm -rf ${INSTALL_DIR}/*
fi

# SDL
if [ ! -e ${TMP_DIR}/SDL-1.2.14-mingw32.tar.bz2 ]; then
	Download SDL-1.2.14 SDL-1.2.14.tar.gz http://www.libsdl.org/release/SDL-1.2.14.tar.gz
	#sed -i -e 's/SDL_main(/main(/g' ${TMP_DIR}/SDL-1.2.14/src/main/win32/SDL_win32_main.c
	Configure SDL-1.2.14 --host=${HOST} --build=${BUILD}
	Compile SDL-1.2.14
	Install SDL-1.2.14
	#sed -i -e 's/-lSDLmain//g' -e 's/-Dmain=SDL_main//g' -e 's/-lmingw32//g' -e 's/-mwindows//g' ${INSTALL_DIR}/bin/sdl-config
	#sed -i -e 's/-lSDLmain//g' -e 's/-Dmain=SDL_main//g' -e 's/-lmingw32//g' ${INSTALL_DIR}/bin/sdl-config
	cd ${INSTALL_DIR}
	tar jcvf ${TMP_DIR}/SDL-1.2.14-mingw32.tar.bz2 *
	rm -rf ${INSTALL_DIR}/*
fi

# libxml2
if [ ! -e ${TMP_DIR}/libxml2-2.6.32-mingw32.tar.bz2 ]; then
	Download libxml2-2.6.32 libxml2-2.6.32.tar.gz ftp://xmlsoft.org/libxml2/libxml2-2.6.32.tar.gz
	cd ${TMP_DIR}/libxml2-2.6.32
	Configure libxml2-2.6.32 --host=${HOST} --build=${BUILD} --without-python
	Compile libxml2-2.6.32
	Install libxml2-2.6.32
	cd ${INSTALL_DIR}
	tar jcvf ${TMP_DIR}/libxml2-2.6.32-mingw32.tar.bz2 *
	rm -rf ${INSTALL_DIR}/*
fi

if [ ! -e ${TMP_DIR}/boost_1_40_0-mingw32.tar.bz2 ]; then
	Download boost_1_40_0 boost_1_40_0.tar.bz2 http://ovh.dl.sourceforge.net/boost/boost_1_40_0.tar.bz2
	cd ${TMP_DIR}/boost_1_40_0
	rm -f g++
	ln -s `which ${HOST}-g++` g++
	rm -f gcc
	ln -s `which ${HOST}-gcc` gcc
	rm -f ar
	ln -s `which ${HOST}-ar` ar
	rm -f ranlib
	ln -s `which ${HOST}-ranlib` ranlib
	rm -f as
	ln -s `which ${HOST}-as` as
	rm -f ld
	ln -s `which ${HOST}-ld` ld
	BJAM_OPTIONS="--prefix=${INSTALL_DIR} -j ${NUM_PROCESSORS} --without-mpi"
	PATH=./:${PATH} ./bootstrap.sh --without-icu
	PATH=./:${PATH} ./bjam ${BJAM_OPTIONS}
	PATH=./:${PATH} ./bjam ${BJAM_OPTIONS} install
	cd ${INSTALL_DIR}
	tar jcvf ${TMP_DIR}/boost_1_40_0-mingw32.tar.bz2 *
	rm -rf ${INSTALL_DIR}/*
fi

# Install cross-compiled packages
InstallBinArchive expat-2.0.1-mingw32.tar.bz2
InstallBinArchive gdb-7.0-mingw32.tar.bz2
InstallBinArchive SDL-1.2.14-mingw32.tar.bz2
InstallBinArchive libxml2-2.6.32-mingw32.tar.bz2
InstallBinArchive boost_1_40_0-mingw32.tar.bz2

# Install MSYS and MINGW
mingw_url="http://downloads.sourceforge.net/mingw"
mingw_file_list="binutils-2.19.1-mingw32-bin.tar.gz \
gcc-full-4.4.0-mingw32-bin-2.tar.lzma \
gmp-4.2.4-mingw32-dll.tar.gz
mpfr-2.4.1-mingw32-dll.tar.gz
pthreads-w32-2.8.0-mingw32-dll.tar.gz
w32api-3.13-mingw32-dev.tar.gz \
mingwrt-3.16-mingw32-dev.tar.gz \
mingwrt-3.16-mingw32-dll.tar.gz \
bash-3.1-MSYS-1.0.11-1.tar.bz2 \
bzip2-1.0.5-1-msys-1.0.11-bin.tar.gz \
libbz2-1.0.5-1-msys-1.0.11-dll-1.tar.gz \
libbz2-1.0.5-1-msys-1.0.11-dev.tar.gz \
bison-2.4.1-1-msys-1.0.11-bin.tar.lzma \
automake-1.11-1-msys-1.0.11-bin.tar.lzma \
autogen-5.9.2-2-msys-1.0.11-bin.tar.lzma \
autoconf-2.63-1-msys-1.0.11-bin.tar.lzma \
coreutils-5.97-MSYS-1.0.11-snapshot.tar.bz2 \
cygutils-1.3.4-3-msys-1.0.11-bin.tar.lzma \
csmake-3.81-MSYS-1.0.11-2.tar.bz2 \
diffutils-2.8.7.20071206cvs-2-msys-1.0.11-bin.tar.lzma \
findutils-4.4.2-1-msys-1.0.11-bin.tar.lzma \
file-5.03-1-msys-1.0.11-bin.tar.lzma \
gawk-3.1.7-1-msys-1.0.11-bin.tar.lzma \
lzma-4.43-MSYS-1.0.11-1-bin.tar.gz \
make-3.81-MSYS-1.0.11-2.tar.bz2 \
groff-1.20.1-1-msys-1.0.11-bin.tar.lzma \
msysCORE-1.0.11-bin.tar.gz \
tar-1.19.90-MSYS-1.0.11-1-bin.tar.gz \
texinfo-4.11-MSYS-1.0.11-1.tar.bz2 \
autoconf2.5-2.61-1-bin.tar.bz2 \
autoconf-4-1-bin.tar.bz2 \
autogen-5.9.2-MSYS-1.0.11-1-bin.tar.gz \
autogen-5.9.2-MSYS-1.0.11-1-dev.tar.gz \
autogen-5.9.2-MSYS-1.0.11-1-dll25.tar.gz \
automake1.10-1.10-1-bin.tar.bz2 \
automake-3-1-bin.tar.bz2 \
grep-2.5.4-1-msys-1.0.11-bin.tar.lzma \
bison-2.3-MSYS-1.0.11-1.tar.bz2 \
crypt-1.1-1-MSYS-1.0.11-1.tar.bz2 \
crypt-1.1_1-2-msys-1.0.11-bin.tar.lzma \
cvs-1.12.13-1-msys-1.0.11-bin.tar.lzma \
flex-2.5.35-1-msys-1.0.11-bin.tar.lzma \
flex-2.5.35-1-msys-1.0.11-doc.tar.lzma \
gdbm-1.8.3-2-msys-1.0.11-bin.tar.lzma \
gdbm-1.8.3-2-msys-1.0.11-doc.tar.lzma \
libgdbm-1.8.3-2-msys-1.0.11-dev.tar.lzma \
libgdbm-1.8.3-2-msys-1.0.11-dll-3.tar.lzma \
gettext-0.17-1-msys-1.0.11-bin.tar.lzma \
gettext-0.17-1-msys-1.0.11-dev.tar.lzma \
gettext-0.17-1-msys-1.0.11-doc.tar.lzma \
gettext-0.17-1-msys-1.0.11-ext.tar.lzma \
gmp-4.3.1-1-msys-1.0.11-doc.tar.lzma \
libgmp-4.3.1-1-msys-1.0.11-dev.tar.lzma \
libgmp-4.3.1-1-msys-1.0.11-dll-3.tar.lzma \
guile-1.8.7-1-msys-1.0.11-bin.tar.lzma \
libguile-1.8.7-1-msys-1.0.11-dev.tar.lzma \
libguile-1.8.7-1-msys-1.0.11-dll-17.tar.lzma \
inetutils-1.3.2-40-MSYS-1.0.11-2-bin.tar.gz \
libiconv-1.13.1-1-msys-1.0.11-bin.tar.lzma \
libiconv-1.13.1-1-msys-1.0.11-dev.tar.lzma \
libiconv-1.13.1-1-msys-1.0.11-doc.tar.lzma \
libtool1.5-1.5.25a-1-dll.tar.bz2 \
gawk-3.1.7-1-msys-1.0.11-bin.tar.lzma \
gawk-3.1.7-1-msys-1.0.11-doc.tar.lzma \
mingw-utils-0.4-1-mingw32-bin.tar.lzma \
mingw-utils-0.4-1-mingw32-doc.tar.lzma \
zlib-1.2.3-1-msys-1.0.11-dev.tar.gz \
zlib-1.2.3-1-msys-1.0.11-dll.tar.gz \
zlib-1.2.3-1-msys-1.0.11-doc.tar.gz \
liblzma-4.999.9beta-1-msys-1.0.11-dev.tar.gz \
liblzma-4.999.9beta-1-msys-1.0.11-dll-1.tar.gz \
xz-4.999.9beta-1-msys-1.0.11-bin.tar.gz \
xz-4.999.9beta-1-msys-1.0.11-doc.tar.gz \
vim-7.2-1-msys-1.0.11-bin.tar.lzma \
vim-7.2-1-msys-1.0.11-doc.tar.lzma \
vim-7.2-1-msys-1.0.11-lang.tar.lzma \
texinfo-4.13a-1-msys-1.0.11-bin.tar.lzma \
texinfo-4.13a-1-msys-1.0.11-doc.tar.lzma \
termcap-0.20050421_1-1-msys-1.0.11-bin.tar.lzma \
termcap-0.20050421_1-1-msys-1.0.11-dev.tar.lzma \
termcap-0.20050421_1-1-msys-1.0.11-doc.tar.lzma \
tar-1.22-1-msys-1.0.11-bin.tar.lzma \
tar-1.22-1-msys-1.0.11-doc.tar.lzma \
tar-1.22-1-msys-1.0.11-ext.tar.lzma \
sed-4.2.1-1-msys-1.0.11-bin.tar.lzma \
sed-4.2.1-1-msys-1.0.11-doc.tar.lzma \
rxvt-2.7.10.20050409-1-msys-1.0.11-bin.tar.lzma \
rxvt-2.7.10.20050409-1-msys-1.0.11-doc.tar.lzma \
libregex-1.20090805-1-msys-1.0.11-dev.tar.lzma \
libregex-1.20090805-1-msys-1.0.11-dll-1.tar.lzma \
regex-1.20090805-1-msys-1.0.11-doc.tar.lzma \
libpopt-1.15-1-msys-1.0.11-dev.tar.lzma \
libpopt-1.15-1-msys-1.0.11-dll-0.tar.lzma \
popt-1.15-1-msys-1.0.11-doc.tar.lzma \
perl-5.6.1_2-1-msys-1.0.11-bin.tar.lzma \
perl-5.6.1_2-1-msys-1.0.11-doc.tar.lzma \
perl-5.6.1_2-1-msys-1.0.11-html.tar.lzma \
perl-5.6.1_2-1-msys-1.0.11-man.tar.lzma \
patch-2.5.9-1-msys-1.0.11-bin.tar.lzma \
patch-2.5.9-1-msys-1.0.11-doc.tar.lzma \
libopenssl-0.9.8k-1-msys-1.0.11-dev.tar.lzma \
libopenssl-0.9.8k-1-msys-1.0.11-dll-098.tar.lzma \
openssl-0.9.8k-1-msys-1.0.11-bin.tar.lzma \
openssl-0.9.8k-1-msys-1.0.11-doc.tar.lzma \
openssh-4.7p1-2-msys-1.0.11-bin.tar.lzma \
openssh-4.7p1-2-msys-1.0.11-doc.tar.lzma \
mktemp-1.6-1-msys-1.0.11-bin.tar.lzma \
mktemp-1.6-1-msys-1.0.11-doc.tar.lzma \
libminires-1.02_1-1-msys-1.0.11-dev.tar.lzma \
libminires-1.02_1-1-msys-1.0.11-dll.tar.lzma \
minires-1.02_1-1-msys-1.0.11-bin.tar.lzma \
minires-1.02_1-1-msys-1.0.11-doc.tar.lzma \
man-1.6f-1-msys-1.0.11-bin.tar.lzma \
man-1.6f-1-msys-1.0.11-doc.tar.lzma \
man-1.6f-1-msys-1.0.11-lang.tar.lzma \
m4-1.4.13-1-msys-1.0.11-bin.tar.lzma \
m4-1.4.13-1-msys-1.0.11-doc.tar.lzma \
bsdcpio-2.7.1-1-msys-1.0.11-bin.tar.lzma \
bsdtar-2.7.1-1-msys-1.0.11-bin.tar.lzma \
lpr-enhanced-1.0.2-1-msys-1.0.11-bin.tar.lzma \
lpr-enhanced-1.0.2-1-msys-1.0.11-doc.tar.lzma \
lndir-1.0.1-1-msys-1.0.11-bin.tar.gz \
lndir-1.0.1-1-msys-1.0.11-doc.tar.gz \
libltdl-2.2.7a-1-msys-1.0.11-dev.tar.lzma \
libltdl-2.2.7a-1-msys-1.0.11-dll-7.tar.lzma \
libtool-2.2.7a-1-msys-1.0.11-bin.tar.lzma \
libtool-2.2.7a-1-msys-1.0.11-doc.tar.lzma \
libiconv-1.13.1-1-msys-1.0.11-bin.tar.lzma \
libiconv-1.13.1-1-msys-1.0.11-dev.tar.lzma \
libiconv-1.13.1-1-msys-1.0.11-doc.tar.lzma \
libarchive-2.7.1-1-msys-1.0.11-dev.tar.lzma \
libarchive-2.7.1-1-msys-1.0.11-dll-2.tar.lzma \
libarchive-2.7.1-1-msys-1.0.11-doc.tar.lzma \
less-436-1-msys-1.0.11-bin.tar.lzma \
less-436-1-msys-1.0.11-doc.tar.lzma \
inetutils-1.5-1-msys-1.0.11-bin.tar.lzma \
inetutils-1.5-1-msys-1.0.11-dev.tar.lzma \
inetutils-1.5-1-msys-1.0.11-doc.tar.lzma \
gzip-1.3.12-1-msys-1.0.11-bin.tar.lzma \
gzip-1.3.12-1-msys-1.0.11-doc.tar.lzma \
"

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
			zip)
				unzip -qo ${TMP_DIR}/${file}
				;;
			lzma)
				tar Yxf ${TMP_DIR}/${file} || exit
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

#InstallBinArchive file-bin.zip http://gnuwin32.sourceforge.net/downlinks/file-bin-zip.php
InstallBinArchive unrar-bin.zip http://gnuwin32.sourceforge.net/downlinks/unrar-bin-zip.php
InstallBinArchive unzip-bin.zip http://gnuwin32.sourceforge.net/downlinks/unzip-bin-zip.php
InstallBinArchive zip-bin.zip http://gnuwin32.sourceforge.net/downlinks/zip-bin-zip.php
InstallBinArchive libintl-bin.zip http://gnuwin32.sourceforge.net/downlinks/libintl-bin-zip.php
InstallBinArchive libiconv-bin.zip http://gnuwin32.sourceforge.net/downlinks/libiconv-bin-zip.php
InstallBinArchive wget-bin.zip http://gnuwin32.sourceforge.net/downlinks/wget-bin-zip.php
#InstallBinArchive regex-2.7-bin.zip http://gnuwin32.sourceforge.net/downlinks/regex-bin-zip.php
#InstallBinArchive zlib-1.2.3-bin.zip http://gnuwin32.sourceforge.net/downlinks/zlib-bin-zip.php

# Install subversion client
InstallBinArchive svn-win32-1.3.2.zip http://subversion.tigris.org/files/documents/15/32473/svn-win32-1.3.2.zip
cp -rf ${INSTALL_DIR}/svn-win32-1.3.2/* ${INSTALL_DIR}/.
rm -rf ${INSTALL_DIR}/svn-win32-1.3.2

# Install cmake
InstallBinArchive cmake-2.8.1-win32-x86.zip http://www.cmake.org/files/v2.8/cmake-2.8.1-win32-x86.zip
cp -rf ${INSTALL_DIR}/cmake-2.8.1-win32-x86/* ${INSTALL_DIR}/.
rm -rf ${INSTALL_DIR}/cmake-2.8.1-win32-x86

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


# Package everything into a single .EXE installer
Package mingw32-unisim-pack msys.bat m.ico "--norxvt"

