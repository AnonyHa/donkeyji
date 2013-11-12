#!/bin/bash

cmd=$1
cmd_path=`which $cmd`
#echo $cmd_path
libs=`ldd $cmd_path |awk -F '=>' {'print $2'}|awk '{print $1}'|grep  '^/'`
echo $libs
echo
echo
echo
for f in $libs; do
	echo $f
	cp --parents -upv $f /mnt/sysroot/
done
