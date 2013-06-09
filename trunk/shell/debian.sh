#!/bin/bash

#mount -t iso9660 -o loop /mnt/hgfs/MacInstall/debian-6.0.6-amd64-DVD-1.iso /mnt/iso/iso1
#mount -t iso9660 -o loop /mnt/hgfs/MacInstall/debian-6.0.6-amd64-DVD-2.iso /mnt/iso/iso2
#mount -t iso9660 -o loop /mnt/hgfs/MacInstall/debian-6.0.6-amd64-DVD-3.iso /mnt/iso/iso3
#mount -t iso9660 -o loop /mnt/hgfs/MacInstall/debian-6.0.6-amd64-DVD-4.iso /mnt/iso/iso4

src_prefix=/mnt/hgfs/Core/debian-6.0.7-amd64-DVD-
dst_prefix=/mnt/iso/iso
total=9
for ((i=1;i<$total;i++))
do
	mount -t iso9660 -o loop $src_prefix$i.iso $dst_prefix$i
done


src_prefix_src=/mnt/hgfs/Core/debian-6.0.7-source-DVD-
dst_prefix_src=/mnt/iso/src
total_src=7
for ((i=1;i<$total_src;i++))
do
	mount -t iso9660 -o loop $src_prefix_src$i.iso $dst_prefix_src$i
done
