#!/bin/bash

ARGS=1
E_BADARGS=65

#arg1=$1

if [ $# -eq "1" ]; then

if [ "$1" = "xcode" ]; then
        echo "$arg1 ok"
elif [ "$1" = "eclipse" ]; then
        echo "$arg1 ok"
else
        echo "Usage: `basename $0` <IDE [unixmakefiles|xcode|eclipse]>"
        exit $E_BADARGS
fi

elif [ $# -eq "0" ]; then
  echo "unixmakefiles ok"
else
        echo "Usage: `basename $0` <IDE [unixmakefiles|xcode|eclipse]>"
        exit $E_BADARGS
fi


find . -name "*.xcodeproj" | xargs rm -rf
find . -name cmake_install.cmake | xargs rm -f
find . -name "CMakeFiles" | xargs rm -rf
find . -name "CMakeCache.txt" | xargs rm -f

rm -rf CMakeFiles/
rm -rf CMakeCache.txt 
rm -rf RouterScheduler.xcodeproj/

if [ "$1" = "xcode" ]; then
        cmake -G"Xcode" .
elif [ "$1" = "eclipse" ]; then
        cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug .
else 
        cmake -G"Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug .
fi

