#!/bin/bash

#umount /mnt/iso/iso1
#umount /mnt/iso/iso2
#umount /mnt/iso/iso3
#umount /mnt/iso/iso4
total=9
for ((i=1;i<$total;i++))
do
	umount /mnt/iso/iso$i
done


total=7
for ((i=1;i<$total;i++))
do
	umount /mnt/iso/src$i
done
