#!/bin/bash

num=`ls -l /root/sysroot.d|wc -l`
let num++
echo $num
find /mnt/sysroot|cpio -o|gzip > /root/sysroot.d/sysroot${num}.gz
