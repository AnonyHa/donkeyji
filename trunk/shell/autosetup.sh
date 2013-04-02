#!/bin/bash
#author: huge
#Sat Mar  9 00:32:35 EST 2013

ERR_ARG=1

arg_cnt=$#
if [ arg_cnt -ne 1 ]
then
	echo 'only one argument is needed'
	exit $ERR_ARG
fi

install_file=$1
failed_file=./failed.log
succeed_file=./succeed.log
cmd='apt-get'
opt='-y --force-yes'
action='install'

function auto_install () {
	pack=$1
	$cmd $opt $action $pack
}

if [ -e $failed_file ]
then
	rm -f $failed_file
fi

if [ -e $succeed_file ]
then
	rm -f $succeed_file
fi

tmp_list=./_to_install
grep -v ^# $install_file|grep -v ^# > $tmp_list

while read package
do
	echo ==============
	echo $package
	echo ==============
	#apt-get -y --force-yes install $package
	#$cmd $opt $action $package
	auto_install $package
	if [ $? -ne 0 ]
	then
		echo $package >> $failed_file
		#exit $?
	else
		echo $package >> $succeed_file
	fi
	echo 
	echo 
done < $tmp_list

$cmd $opt --purge autoremove
$cmd $opt clean
