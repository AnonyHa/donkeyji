#!/bin/bash

src_prefix=/mnt/hgfs/MacInstall/rhel-server-6.4-x86_64-dvd.iso
dst_prefix=/mnt/iso/iso
for ((i=1;i<2;i++))
do
	mount -t iso9660 -o loop $src_prefix $dst_prefix$i
done
