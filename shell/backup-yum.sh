#!/bin/bash

src_dir=/mnt/hgfs/VMwareShared/yum/
if ! [ -e $src_dir ]; then
	echo "no src dir"
	exit 1
fi

dst_dir=/mnt/hgfs/VMwareShared/rpms/el5/
if ! [ -e $dst_dir ]; then
	mkdir -p $dst_dir
fi

all_dist="base epel extras rpmforge updates"

for dist in $all_dist; do
	src_rpm=$src_dir$dist/packages
	if ! [ -e $src_rpm ]; then
		echo "no this dist: $dist"
		continue
	fi

	dst_rpm=$dst_dir$dist/
	if ! [ -e $dst_rpm ]; then
		mkdir -p $dst_rpm
	fi
	cp -ru $src_rpm $dst_rpm
done

echo "back completed"
