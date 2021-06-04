#!/bin/bash

# apt install libssh2-1-dev
# apt install libcurl4-openssl-dev
# apt install libuid-dev
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

bitss=""
ldc=$(which ldconfig)
[[ -z ${ldc} ]] && echo "ldconfig not found" && exit 1
[[ -z $(${ldc} -p | grep libpthread) ]] && echo  "libpthread not found" && exit 1
[[ -z $(${ldc} -p | grep libcurl) ]] && echo " libcurl not found" && exit 1
[[ -z $(${ldc} -p | grep libssh2) ]] && echo  "libssh2 not found" && exit 1
[[ ! -f /etc/rssh_machine_id ]] && cat /proc/sys/kernel/random/uuid > /etc/rssh_machine_id
[[ ! -f /etc/rssh_machine_id ]] && echo  "/etc/rssh_machine_id not found" && exit 1
[[ ! -d /lib/systemd/system ]] && echo  "systemd /lib/systemd/system path not found" && exit 1
arch=$(uname -m)
oss=$(uname)

[[ ! -f ./meiotrev-${oss}-${arch}${bitss} ]] && echo "./meiotrev-${oss}-${arch}${bitss} not found" && exit 1

SRVE="/etc/init.d/meiotrev"

service meiotrev stop > /dev/zero
update-rc.d  meiotrev disable > /dev/zero
update-rc.d  meiotrev remove > /dev/zero

pkill  meiotrev-${oss}-${arch}${bitss} > /dev/zero
pkill  meiotrev-* > /dev/zero
pkill  meiotrev-${oss}-${arch}${bitss} > /dev/zero
pkill  meiotrev-* > /dev/zero

rm ${SRVE}
sudo rm /etc/.mylinuz.com
sleep 2
clear
usename="xxxxxxxx"
password="yyyyyyyyyy"
echo "ENTER YOUR mylinuz.com LOGIN CREDENTIALS (These are to credentials to login to the website)"
echo -n ">mylinuz.com USERNAME (minim 8 characters long and underscore): "
read username
[[ ${#username} -le 8 ]] && echo "username over 8 characters and underscore" && exit 1
[[ -z $(echo $username | grep "_") ]] && echo "you have to use at least one underscore" && exit 1

echo -n ">mylynuz.com PASSWORD (min 10 characters, no . and at least one underscore): "
read passwd
[[ ${#passwd} -le 10 ]] && echo "password has to be over 10 characters long" && exit 1
[[ ${passwd} =~ "." ]] && echo "dont use . or : in password" && exit 1
[[ -z $(echo $passwd | grep "_") ]] && echo "you have to use at least one underscore" && exit 1

devname=$(cat /etc/hostname)
echo "WE FOUND MACHINE NAME: ${devname}"
echo -n ">ENTER MACHINE NAME (OR USE A DIFFERENT NAME): "
read dev_name

if [[ ! -z $dev_name ]];then
    devname=${dev_name}
fi

md5_user=${username}
md5_pass=${passwd}

echo ${md5_user} > /etc/.mylinuz.com
echo ${md5_pass} >> /etc/.mylinuz.com
echo ${devname} >> /etc/.mylinuz.com

echo "REGISTERING TO MYLINUZ.COM"
echo "USER: ${md5_user}"
echo "PASS: ${md5_pass}, WILL BE MD5-ed"
echo "DEV:  ${devname}"


cp -rf ./meiotrev-${oss}-${arch}${bitss} /usr/local/bin
[[ ! -f /usr/local/bin/meiotrev-${oss}-${arch}${bitss} ]] && echo "CANNOT CREATE /usr/local/bin/meiotrev-${oss}-${arch}${bitss}" && exit 1
chmod +x /usr/local/bin/meiotrev-${oss}-${arch}${bitss}

LOG="/var/log/meiotrev.log"
touch ${LOG}
echo "#! /bin/bash">> ${SRVE}
echo "### BEGIN INIT INFO">> ${SRVE}
echo "# Provides:          skeleton">> ${SRVE}
echo "# Required-Start:    $remote_fs $syslog">> ${SRVE}
echo "# Required-Stop:     $remote_fs $syslog">> ${SRVE}
echo "# Default-Start:     2 3 4 5">> ${SRVE}
echo "# Default-Stop:      0 1 6">> ${SRVE}
echo "# Short-Description: Example initscript">> ${SRVE}
echo "# Description:       This file should be used to construct scripts to be">> ${SRVE}
echo "#                    placed in /etc/init.d.">> ${SRVE}
echo "### END INIT INFO">> ${SRVE}
echo "/usr/local/bin/meiotrev-${oss}-${arch}${bitss} https://mylinuz.com &">> ${SRVE}
echo "">> ${SRVE}
chmod +x ${SRVE}


[[ ! -f ${SRVE} ]] && echo "CANNOT CREATE ${SRVE}" && exit 1

# log rotate
if [[ -d /etc/logrotate.d/ ]];then
    LR=/etc/logrotate.d/meiotrev
    echo "    ${LOG} {" > ${LR}
    echo "        daily" >> ${LR}
    echo "        rotate 8" >> ${LR}
    echo "        missingok" >> ${LR}
    echo "        notifempty" >> ${LR}
    echo "        compress" >> ${LR}
    echo "        nocreate" >> ${LR}
    echo "    }" >> ${LR}
fi


echo "Enabling service"
update-rc.d meiotrev defaults
echo "Starting service"
service meiotrev start

echo "Please wait..."
sleep 8
proc=$(ps ax | grep  meiotrev )
echo "---------------------------------------------------------------------------"
echo ">> Your: /etc/.mylinuz.com"
cat /etc/.mylinuz.com
if [[ -f ${LOG} ]];then
	cnt=$(tail -20 ${LOG} | grep "DEVICE ID" | awk '{print $9}')
	echo ""
	if [[ ! -z ${proc} && ! -z ${cnt} ]];then
	    echo "Installation complete. Here are  https://mylinuz.com credentials:"
	    echo "Username:  ${username}"
	    echo "Password:  ${passwd}"
	    echo "${devname} DEVICE ID: '${cnt}'. Please write it down"
	    echo "Username & Password and device name are saved in /etc/.mylinuz.com"
	    echo "mylinuz.vom client service logs are  /var/log/ folder under meeiotrev* files"
	else
	    sudo rm /etc/.mylinuz.com
	    echo "INSTALL FAILED. NO PROC!"
	fi
else
	    rm /etc/.mylinuz.com
	    echo "INSTALL FAILED. NO LOG!"
fi
