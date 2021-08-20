#!/bin/bash -ex

# check required variables
USAGE="Usage: `basename $0` -s <systemc_path> -t <tlm2_path>"
NO_ARGS=0 
E_OPTERROR=85

if [ $# -eq "$NO_ARGS" ]    # Script invoked with no command-line args?
then
	echo $USAGE
	exit $E_OPTERROR          # Exit and explain usage.
	                          # Usage: scriptname -options
	                          # Note: dash (-) necessary
fi  

while getopts ":s:t:" Option
do
	case $Option in
		s ) SYSTEMC_PATH=$OPTARG;;
		t ) TLM2_PATH=$OPTARG;;
		* ) echo "ERROR: Unknown command \'$Option\'\n$USAGE"; exit $E_OPTERROR;;
	esac
done

if [ -z ${SYSTEMC_PATH} ]
then
	echo "ERROR: SystemC path not defined.\n$USAGE"; exit $E_OPTERROR
fi
if [ -z ${TLM2_PATH} ]
then
	echo "ERROR: TLM2 path not defined.\n$USAGE"; exit $E_OPTERROR
fi


# create src and build directories (remove if they already exist)
rm -rf src build install simulator.tar.bz2
mkdir src
mkdir build
mkdir install

# create config.xml
BUILD_SH_PATH=`dirname $0`
REPO_PATH=`dirname ${BUILD_SH_PATH}`
REPO_PATH=`dirname ${REPO_PATH}`
REPO_PATH=`dirname ${REPO_PATH}`
echo "${REPO_PATH}"
echo "<?xml version='1.0' encoding='UTF-8'?>
<repositories>
	<entry>
		<name>Unisim Tools</name>
		<location>${REPO_PATH}/unisim_tools</location>
	</entry>
	<entry>
		<name>Unisim Library</name>
		<location>${REPO_PATH}/unisim_lib</location>
	</entry>
	<entry>
		<name>Unisim Simulators</name>
		<location>${REPO_PATH}/unisim_simulators</location>
	</entry>
</repositories>" > config.xml

# copy files required into the src directory
python3.1 ${REPO_PATH}/package/puscomp/puscomp.py -c config.xml -o src unisim_simulator::tlm2::armemu

# remove previous builds
rm *.tar.bz2
# configure and build the simulator
cd build
cmake ../src -DCMAKE_BUILD_TYPE=RELEASE -Dwith_osci_systemc=$SYSTEMC_PATH -Dwith_osci_tlm2=$TLM2_PATH -DCMAKE_INSTALL_PREFIX=../install
make -j
make install
make package_source
cp *.tar.bz2 ..
cd ..
tar jcvf simulator.tar.bz2 install

