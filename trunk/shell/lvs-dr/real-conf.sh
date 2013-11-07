#/bin/bash
REAL_IP=$1
if [ -z $REAL_IP ]
then
	echo "real ip should be specified"
	exit 1
fi

echo 1 > /proc/sys/net/ipv4/conf/lo/arp_ignore
echo 1 > /proc/sys/net/ipv4/conf/all/arp_ignore
echo 2 > /proc/sys/net/ipv4/conf/lo/arp_announce
echo 2 > /proc/sys/net/ipv4/conf/all/arp_announce

ifconfig lo:0 192.168.196.1 netmask 255.255.255.255 broadcast 192.168.196.1 up

route add -host 192.168.196.1 dev lo:0

ifconfig eth0 $REAL_IP netmask 255.255.255.0 broadcast 192.168.196.255 gateway 192.168.196.2 up
