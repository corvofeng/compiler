#!/bin/bash

pwd=$PWD
echo $pwd

if [ -f $pwd/../CMakeLists.txt ]; then
	echo "In project directory, error.."
	exit 0
fi

echo "continue ..."


flag=0

if [ -f $pwd/../../compiler/CMakeLists.txt ]; then
	src=$pwd/../../compiler
else
    exit 0
fi

cd $pwd/.. && cmake $src

cd $pwd/.. && make -j4

cd $pwd/../bin && ./comp

cd $pwd  && make -f MyMake


