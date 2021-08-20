#!/bin/bash
HERE=$(pwd)
MY_DIR=$(cd $(dirname $0); pwd)

echo "Starting building web site..."
${MY_DIR}/build.sh all || exit -1
echo "Congratulation. Bye !"

firefox ${HERE}/site/index.html
#opera ${HERE}/site/index.html
