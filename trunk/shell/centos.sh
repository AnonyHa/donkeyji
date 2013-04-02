#!/bin/bash

src_prefix=/mnt/hgfs/MacInstall/CentOS-6.4-x86_64-bin-DVD
dst_prefix=/mnt/iso/iso
for ((i=1;i<3;i++))
do
	mount -t iso9660 -o loop $src_prefix$i.iso $dst_prefix$i
done
