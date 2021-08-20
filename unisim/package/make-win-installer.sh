#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ] || [ -z "$5" ] || [ -z "$6" ] || [ -z "$7" ] || [ -z "$7" ]; then
	echo "Usage: $0 <package name> <version> <directory> <start program> <start program icon> <start program params> <license file> <description>"
	exit 0
fi

HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

function Package {
	local PACKAGE_NAME=$1
	local VERSION="$2"
	local INSTALL_DIR="$3"
	local START="$4"
	local START_ICON="$5"
	local START_PARAMS="$6"
	local LICENSE_FILE="$7"
	local DESCRIPTION="$8"

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
	echo "OutputBaseFilename=setup-${PACKAGE_NAME}-${VERSION}" >> ${ISS_FILENAME}
	echo "Compression=lzma" >> ${ISS_FILENAME}
	echo "SolidCompression=yes" >> ${ISS_FILENAME}
	echo "LicenseFile=${LICENSE_FILE}" >> ${ISS_FILENAME}
# 	if test "x${START_ICON}" != x; then
# 		echo "SetupIconFile=${START_ICON}" >> ${ISS_FILENAME}
# 	fi
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
		echo "Name: \"{group}\\${PACKAGE_NAME}\"; Filename: \"{app}\\${START}\"; IconFilename: \"{app}\\${START_ICON}\"; Parameters: \"${START_PARAMS}\"; WorkingDir: \"{app}\"" >> ${ISS_FILENAME}
		echo "Name: \"{commondesktop}\\${PACKAGE_NAME}\"; Filename: \"{app}\\${START}\"; IconFilename: \"{app}\\${START_ICON}\"; Parameters: \"${START_PARAMS}\"; Tasks: desktopicon; WorkingDir: \"{app}\"" >> ${ISS_FILENAME}
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

Package "$1" "$2" "$3" "$4" "$5" "$6" "$7" "$8"
