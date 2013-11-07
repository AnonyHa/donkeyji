#/bin/bash
DIRECTOR_IP=$1
R1_IP=$2
R2_IP=$3
if [ -z $DIRECTOR_IP ]
then
	echo "direct ip should be specified"
	exit 1
fi
if [ -z $R1_IP ]
then
	echo "R1 ip should be specified"
	exit 1
fi
if [ -z $R2_IP ]
then
	echo "R2 ip should be specified"
	exit 1
fi


ifconfig eth0:0 192.168.100.1 netmask 255.255.255.255 broadcast 192.168.100.1 up

route add -host 192.168.100.1 dev eth0:0

ifconfig eth0 $DIRECTOR_IP netmask 255.255.255.0 broadcast 192.168.100.255 up

ipvsadm -C
ipvsadm -A -t 192.168.100.1:80 -s rr -p
ipvsadm -a -t 192.168.100.1:80 -r $R1_IP -g
ipvsadm -a -t 192.168.100.1:80 -r $R2_IP -g
ipvsadm -L -n
