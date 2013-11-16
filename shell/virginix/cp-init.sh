#!/bin/bash

cp -puv ./inittab /mnt/sysroot/etc/
if ! [ -d /mnt/sysroot/etc/rc.d ]; then
	mkdir -pv /mnt/sysroot/etc/rc.d
fi
cp -puv ./rc.sysinit /mnt/sysroot/etc/rc.d
