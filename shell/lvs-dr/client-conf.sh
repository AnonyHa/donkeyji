#!/bin/bash

C_IP=$1
if [ -z $C_IP ]
then
	echo "client ip should be specified"
	exit 1
fi

ifconfig eth0 $C_IP netmask 255.255.255.0 broadcast 192.168.100.255 up
