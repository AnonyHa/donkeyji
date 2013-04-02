#!/bin/bash

target_dir=$1
arg_cnt=$#
if [ $arg_cnt -lt 1 ]
then
	echo 'not enough arguments'
	exit 1
fi

if [ ! -e $target_dir -o ! -d $target_dir ]
then
	echo 'wrong argument'
	exit 2
fi


tmp_list=/tmp/_tmp_list

cd $target_dir

ls | less > $tmp_list

while read line
do
	echo $line
	tar -cjvf "$line".tar.bz2 "$line"
done < $tmp_list

while read line
do
	echo $line
	rm -rf "$line"
done < $tmp_list

rm $tmp_list
