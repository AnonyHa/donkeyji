#!/bin/sh 

WHOAMI=`whoami`
if [ $WHOAMI != root ];then
	echo "You must be root."
	exit 1
fi

PATH=/sbin:/bin:/usr/sbin:/usr/bin:usr/local/sbin:/usr/local/bin
export PATH
OS=`uname`

linuxadd () {
echo "Ready to wget the keyfile..."
#wget -O /tmp/.key_all http://218.107.55.88:16380/key/key_all > /dev/null 2>&1
keyfile=./key_all
keyfile_tmp=./key_all_tmp
UserName=${1:?"User missing!"}
usercheck=`cat $keyfile |awk '{print $1}'|grep "^${UserName}$"`
if [ -z $usercheck ];then
	echo "user [$UserName] is not exist in the keyfile."
	exit 1
else
	cat $keyfile |grep ${UserName} >${keyfile_tmp}
	while read user type key
	do
	if [ $UserName = $user ];then
		/usr/sbin/useradd $UserName -G root -c $UserName -s /bin/bash -m
		su $UserName -c "mkdir -p /home/${user}/.ssh"
		su $UserName -c "echo $type $key >/home/${user}/.ssh/authorized_keys"
		su $UserName -c "chmod 755 /home/${user}/.ssh"
		su $UserName -c "chmod 755 /home/${user}/.ssh/authorized_keys"
		/usr/sbin/usermod -p '*' $UserName
		/usr/sbin/usermod -U $UserName
	fi
	done < ${keyfile_tmp}
fi
}

bsdadd () {
echo "Ready to fetch the key file..."
fetch -o /tmp/.key_all http://218.107.55.88:16380/key/key_all > /dev/null 2>&1
keyfile=/tmp/.key_all
keyfile_tmp=/tmp/.key_all_tmp
UserName=${1:?"User missing!"}
usercheck=`cat $keyfile |awk '{print $1}'|grep "^${UserName}$"`
if [ -z $usercheck ];then
	echo "user [$UserName] is not exist in the keyfile."
	exit 1
else
	cat $keyfile |grep ${UserName} >${keyfile_tmp}
	while read user type key
	do
        id $UserName
        if [ $? != 0 ]; then
                /usr/sbin/pw useradd $UserName -m -G wheel -s /bin/csh
                /bin/mkdir -p /home/${UserName}/.ssh
                echo $type $key >/home/${UserName}/.ssh/authorized_keys
		chown ${UserName}:${UserName} /home/${UserName}
                echo "adding $UserName successfully"
        else
                echo "user $UserName exist"
                /usr/sbin/pw useradd $UserName -m -G wheel -s /bin/csh
                if [ ! -d /home/${UserName}/.ssh ];then
                        su $UserName -c "mkdir -p /home/${UserName}/.ssh"
                fi
                echo $type $key >/home/${UserName}/.ssh/authorized_keys
		chown ${UserName}:${UserName} /home/${UserName}
        fi
done < ${keyfile_tmp}
fi
}

if [ $OS = 'Linux' ];then
 	linuxadd $1
elif [ $OS = 'FreeBSD' ];then
	bsdadd $1
else 
	echo "This script is not for $OS."
fi
rm -f /tmp/.key_all
rm -f /tmp/.key_all_tmp
