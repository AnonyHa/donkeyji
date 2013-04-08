#!/bin/bash

arg_cnt=$#
if [ $arg_cnt -lt 1 ]
then
	echo 'not enough arguments'
	exit 1
fi

target_dir=$1
tmp_list=/tmp/_tmp_list
new_tmp_list=/tmp/_new_tmp_list
if [ ! -e $target_dir -o ! -d $target_dir ]
then
	echo 'wrong argument'
	exit 2
fi

shift

cd $target_dir

ls|less > $tmp_list

while [ $# -gt 0 ]
do
	grep -v "^$1$" $tmp_list > $new_tmp_list
	mv $new_tmp_list  $tmp_list
	shift
done
cat $tmp_list

while read line
do
	#echo $line
	tar -cjvf "$line".tar.bz2 "$line"
done < $tmp_list

while read line
do
	#echo $line
	rm -rf "$line"
done < $tmp_list

#rm -f $tmp_list
