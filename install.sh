#!/bin/bash

# apt install libssh2-1-dev
# apt install libcurl4-openssl-dev
# apt install libuid-dev
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

bitss=""
[[ ! -z ${1} ]] && bitss="-m32"

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

SRVE="/lib/systemd/system/meiotrev.service"
pkill  meiotrev-${oss}-${arch}${bitss} > /dev/zero
systemctl stop  meiotrev > /dev/zero
systemctl disable  meiotrev > /dev/zero
sudo rm /etc/.mylinux.mine.nu
sleep 2
clear
usename="xxxxxxxx"
password="yyyyyyyyyy"
echo "ENTER YOUR mylinux.mine.nu LOGIN CREDENTIALS (These are to credentials to login to the website)"
echo -n ">mylinux.mine.nu USERNAME (minim 8 characters long and underscore): "
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

echo ${md5_user} > /etc/.mylinux.mine.nu
echo ${md5_pass} >> /etc/.mylinux.mine.nu
echo ${devname} >> /etc/.mylinux.mine.nu

echo "REGISTERING TO mylinux.mine.nu"
echo "USER: ${md5_user}"
echo "PASS: ${md5_pass}, WILL BE MD5-ed"
echo "DEV:  ${devname}"


cp -rf ./meiotrev-${oss}-${arch}${bitss} /usr/local/bin
[[ ! -f /usr/local/bin/meiotrev-${oss}-${arch}${bitss} ]] && echo "CANNOT CREATE /usr/local/bin/meiotrev-${oss}-${arch}${bitss}" && exit 1
chmod +x /usr/local/bin/meiotrev-${oss}-${arch}${bitss}

LOG="/var/log/meiotrev.log"
touch ${LOG}
echo "# meeiotrev.service, by: marrius9876@gmail.com" > ${SRVE}
echo "[Unit]" >> ${SRVE}
echo "Description=meiotrev" >> ${SRVE}
echo "AssertPathExists=/usr/local/bin" >> ${SRVE}
echo "After=network-online.target" >> ${SRVE}
echo "Wants=network-online.target" >> ${SRVE}
echo "" >> ${SRVE}
echo "[Service]" >> ${SRVE}
echo "Type=simple" >> ${SRVE}
echo "ExecStart=/usr/local/bin/meiotrev-${oss}-${arch}${bitss} https://mylinux.mine.nu" >> ${SRVE}
echo "StandardOutput=file:${LOG}" >> ${SRVE}
echo "StandardError=file:${LOG}" >> ${SRVE}
echo "" >> ${SRVE}
echo "[Install]" >> ${SRVE}
echo "WantedBy=multi-user.target" >> ${SRVE}
#echo "WantedBy=network-online.service" >> ${SRVE}

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

systemctl daemon-reload

echo "Enabling service"
systemctl enable meiotrev
echo "Starting service"
systemctl start meiotrev
systemctl daemon-reload
echo "Please wait..."
sleep 8
proc=$(systemctl status meiotrev | grep running)
echo "---------------------------------------------------------------------------"
echo ">> Your: /etc/.mylinux.mine.nu"
cat /etc/.mylinux.mine.nu
if [[ -f ${LOG} ]];then
	cnt=$(tail -20 ${LOG} | grep "DEVICE ID" | awk '{print $9}')
	echo ""
	if [[ ! -z ${proc} && ! -z ${cnt} ]];then
	    echo "Installation complete. Here are  https://mylinux.mine.nu credentials:"
	    echo "Username:  ${username}"
	    echo "Password:  ${passwd}"
	    echo "${devname} DEVICE ID: '${cnt}'. Please write it down"
	    echo "Username & Password and device name are saved in /etc/.mylinux.mine.nu"
	    echo "mylinux.mine.nu client service logs are  /var/log/ folder under meeiotrev* files"
	else
	    systemctl status meiotrev | grep running
	    systemctl stop  meiotrev > /dev/zero
	    systemctl disable  meiotrev > /dev/zero
	    sudo rm /etc/.mylinux.mine.nu
	    echo "INSTALL FAILED. NO PROC!"
	fi
else
	    systemctl status meiotrev | grep running
	    systemctl stop  meiotrev > /dev/zero
	    systemctl disable  meiotrev > /dev/zero
	    rm /etc/.mylinux.mine.nu
	    echo "INSTALL FAILED. NO LOG!"
fi
