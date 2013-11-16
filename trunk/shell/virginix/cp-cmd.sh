#!/bin/bash

all_cmds="
less
grep
vim
fuser
lsof
mingetty 
shutdown 
redis-server 
halt 
ftp 
ls 
ping 
mount 
route 
init 
ifconfig 
insmod 
lsmod 
ftp 
bash 
touch
"

CP='/bin/cp --parents -upv'

target_dir=/mnt/sysroot/

for cmd in $all_cmds; do
	cmd_path=`which $cmd`
	echo $cmd_path
	if [ $? -eq 0 ]; then
		$CP $cmd_path $target_dir

		all_libs=`ldd $cmd_path|grep '/'|awk -F '=>' '{print $NF}'|awk '{print $1}'`
		for lib in $all_libs; do
			#echo $lib
			$CP $lib $target_dir
		done
	else
		echo "no this cmd: $cmd"
	fi
done
