#!/bin/sh

java -Djava.library.path=$(pwd)/../dll/lib  NativeShmidcat /usr/bin/gtkwave sample.vcd



