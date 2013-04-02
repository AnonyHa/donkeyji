#!/bin/bash
#Fri Mar 29 02:24:35 EDT 2013

err_no_orig=1
err_no_cur=2
err_arg_cnt=3
err_arg_type=4

data_dir=/home/huge/g-code/shell/diff-pkg/data
orig_deb_list=$data_dir/orig.deblist
cur_deb_list=$data_dir/cur.deblist
diff_deb_list=$data_dir/diff.deblist


function gen_orig() {
	if [ ! -d $data_dir ]
	then
		mkdir -p $data_dir
	fi
	dpkg -l| grep ii| awk {'print $2'} > $orig_deb_list
}

function gen_cur() {
	if [ ! -d $data_dir ]
	then
		mkdir -p $data_dir
	fi
	dpkg -l| grep ii| awk {'print $2'} > $cur_deb_list
}

function gen_diff() {
	if [ ! -f $orig_deb_list ]
	then
		echo 'no orig list file'
		exit $err_no_orig
	fi

	if [ ! -f $cur_deb_list ]
	then
		echo 'no cur list file'
		exit $err_no_cur
	fi

	have_diff=0
	for pkg in `cat $cur_deb_list`
	do
		rtn=`grep ^$pkg$ $orig_deb_list`
		#echo $rtn
		if [ -z "$rtn" ]
		then
			have_diff=1
			echo "new pkg: $pkg"
			echo $pkg >> $diff_deb_list
		fi
	done

	if [ $have_diff -eq 0 ]
	then
		echo 'no any new pkg found'
	fi
}

arg_cnt=$#
if [ $arg_cnt -ne 1 ]
then
	echo 'only one argument is needed'
	exit $err_arg_cnt
fi

arg=$1
case $arg in
	orig)
		gen_orig
		;;
	cur)
		gen_cur
		;;
	diff)
		gen_cur
		gen_diff
		;;
	*)
		echo 'valid argument: orig, cur, diff'
		exit $err_arg_type
		;;
esac
