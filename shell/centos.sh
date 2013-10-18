#!/bin/bash

src_prefix=/mnt/hgfs/Core/CentOS-5.9-x86_64-bin-DVD-
dst_prefix=/mnt/iso/iso
for ((i=1;i<3;i++))
do
	mount -t iso9660 -o loop $src_prefix${i}of2.iso $dst_prefix$i
done
