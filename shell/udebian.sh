#!/bin/bash

#umount /mnt/iso/iso1
#umount /mnt/iso/iso2
#umount /mnt/iso/iso3
#umount /mnt/iso/iso4
for ((i=1;i<9;i++))
do
	umount /mnt/iso/iso$i
done
