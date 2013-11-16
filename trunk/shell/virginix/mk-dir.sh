#!/bin/bash

all_dir=`ls /`
for d in $all_dir; do
	mkdir -pv /mnt/sysroot/$d
done
