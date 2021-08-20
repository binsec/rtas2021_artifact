#!/bin/bash
HERE=`pwd`
MY_DIR=`dirname $0`
if test ${MY_DIR} = "."; then
	MY_DIR=${HERE}
elif test ${MY_DIR} = ".."; then
	MY_DIR=${HERE}/..
fi

UNISIM_REPO="$1"
shift
SYSTEMC="$1"
shift
TLM20="$1"
shift

if [ -z "${UNISIM_REPO}" ] || [ -z "${SYSTEMC}" ] || [ -z "${TLM20}" ] || [ -z "$1" ]; then
	echo "Usage: `basename $0` <unisim repo> <systemc> <tlm2.0> [<pkg name>...]"
	echo "pkg name: genisslib | ppcemu | ppcemu-system | embedded-ppc-g4-board | tms320c3x | armemu | star12x"
	exit -1
fi

WORKING_DIR=${HOME}/tmp/dist_source
rm -rf ${WORKING_DIR}
mkdir -p ${WORKING_DIR}

for PKG_NAME in "$@"; do
	case ${PKG_NAME} in
		ppcemu)
			UNISIM_PPCEMU_SOURCE=${WORKING_DIR}/ppcemu_source
			${UNISIM_REPO}/package/dist_ppcemu.sh ${UNISIM_PPCEMU_SOURCE} ${UNISIM_REPO} || exit -1
			cd ${UNISIM_PPCEMU_SOURCE}
			./configure --with-systemc=${SYSTEMC} --with-tlm20=${TLM20} || exit -1
			make dist-gzip || exit -1
			make dist-bzip2 || exit -1
			make dist-zip || exit -1
			mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
			cd ${HERE}
			rm -rf ${UNISIM_PPCEMU_SOURCE}
			;;
		ppcemu-system)
			UNISIM_PPCEMU_SYSTEM_SOURCE=${WORKING_DIR}/ppcemu_system_source
			${UNISIM_REPO}/package/dist_ppcemu_system.sh ${UNISIM_PPCEMU_SYSTEM_SOURCE} ${UNISIM_REPO} || exit -1
			cd ${UNISIM_PPCEMU_SYSTEM_SOURCE}
			./configure --with-systemc=${SYSTEMC} --with-tlm20=${TLM20}|| exit -1
			make dist-gzip || exit -1
			make dist-bzip2 || exit -1
			make dist-zip || exit -1
			mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
			cd ${HERE}
			rm -rf ${UNISIM_PPCEMU_SYSTEM_SOURCE}
			;;
		embedded-ppc-g4-board)
			UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE=${WORKING_DIR}/embedded_ppc_g4_board_source
			${UNISIM_REPO}/package/dist_embedded_ppc_g4_board.sh ${UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE} ${UNISIM_REPO} || exit -1
			cd ${UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE}
			./configure --with-systemc=${SYSTEMC} --with-tlm20=${TLM20} || exit -1
			make dist-gzip || exit -1
			make dist-bzip2 || exit -1
			make dist-zip || exit -1
			mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
			cd ${HERE}
			rm -rf ${UNISIM_EMBEDDED_PPC_G4_BOARD_SOURCE}
			;;
		tms320c3x)
			UNISIM_TMS320C3X_SOURCE=${WORKING_DIR}/tms320c3x
			${UNISIM_REPO}/package/dist_tms320c3x.sh ${UNISIM_TMS320C3X_SOURCE} ${UNISIM_REPO} || exit -1
			cd ${UNISIM_TMS320C3X_SOURCE}
			./configure || exit -1
			make dist-gzip || exit -1
			make dist-bzip2 || exit -1
			make dist-zip || exit -1
			mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
			cd ${HERE}
			rm -rf ${UNISIM_TMS320C3X_SOURCE}
			;;
		genisslib)
			UNISIM_GENISSLIB_SOURCE=${WORKING_DIR}/genisslib
			${UNISIM_REPO}/package/dist_genisslib.sh ${UNISIM_GENISSLIB_SOURCE} ${UNISIM_REPO} || exit -1
			cd ${UNISIM_GENISSLIB_SOURCE}
			./configure || exit -1
			make dist-gzip || exit -1
			make dist-bzip2 || exit -1
			make dist-zip || exit -1
			mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
			cd ${HERE}
			rm -rf ${UNISIM_GENISSLIB_SOURCE}
			;;
		armemu)
			UNISIM_ARMEMU_SOURCE=${WORKING_DIR}/armemu_source
			UNISIM_ARMEMU_BUILD=${WORKING_DIR}/armemu_build
			${UNISIM_REPO}/package/dist_armemu.sh ${UNISIM_ARMEMU_SOURCE} ${UNISIM_REPO} || exit -1
			mkdir -p ${UNISIM_ARMEMU_BUILD}
			cd ${UNISIM_ARMEMU_BUILD}
			cmake ${UNISIM_ARMEMU_SOURCE} -Dwith_osci_systemc=${SYSTEMC} -Dwith_osci_tlm2=${TLM20}
			make package_source
			mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
			cd ${HERE}
			rm -rf ${UNISIM_ARMEMU_BUILD}
			rm -rf ${UNISIM_ARMEMU_SOURCE}
			;;
		star12x)
			UNISIM_STAR12X_SOURCE=${WORKING_DIR}/star12x
			${UNISIM_REPO}/package/dist_star12x.sh ${UNISIM_STAR12X_SOURCE} ${UNISIM_REPO} || exit -1
			cd ${UNISIM_STAR12X_SOURCE}
			./configure --with-systemc=${SYSTEMC} --with-tlm20=${TLM20} || exit -1
			make dist-gzip || exit -1
			make dist-bzip2 || exit -1
			make dist-zip || exit -1
			mv -f *.tar.gz *.tar.bz2 *.zip ${HERE}
			cd ${HERE}
			rm -rf ${UNISIM_STAR12X_SOURCE}
			;;
		*)
			echo "Unknown package \"${PKG_NAME}\""
			;;
	esac
done

rm -rf ${WORKING_DIR}
