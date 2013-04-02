#!/bin/bash

#mount -t iso9660 -o loop /mnt/hgfs/MacInstall/debian-6.0.6-amd64-DVD-1.iso /mnt/iso/iso1
#mount -t iso9660 -o loop /mnt/hgfs/MacInstall/debian-6.0.6-amd64-DVD-2.iso /mnt/iso/iso2
#mount -t iso9660 -o loop /mnt/hgfs/MacInstall/debian-6.0.6-amd64-DVD-3.iso /mnt/iso/iso3
#mount -t iso9660 -o loop /mnt/hgfs/MacInstall/debian-6.0.6-amd64-DVD-4.iso /mnt/iso/iso4

src_prefix=/mnt/hgfs/MacInstall/debian-6.0.6-amd64-DVD-
dst_prefix=/mnt/iso/iso
for ((i=1;i<9;i++))
do
	mount -t iso9660 -o loop $src_prefix$i.iso $dst_prefix$i
done
