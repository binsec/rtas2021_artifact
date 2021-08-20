#!/bin/bash
HERE=$(pwd)
MY_DIR=$(cd $(dirname $0); pwd)
cd "${MY_DIR}/../unisim_simulators"
for FILE in */*/VERSION; do
	echo "${FILE}: $(cat ${FILE})"
done
cd "${HERE}"
