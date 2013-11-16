#!/bin/bash

cp -up /boot/vmlinuz-2.6.18-348.el5 /mnt/boot/vmlinuz
cp -up ./initrd.gz /mnt/boot/initrd.gz
cp -up ./grub.conf /mnt/boot/grub/grub.conf

#find . |cpio -o -H newc|gzip -9 > /root/virginix/initrd.gz
