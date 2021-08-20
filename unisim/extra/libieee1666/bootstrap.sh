#!/bin/sh
#
# This script is intended for building configure
#

mkdir -p config
libtoolize -c -f
aclocal -Im4
autoheader
autoconf -f -Wall
automake -ac
