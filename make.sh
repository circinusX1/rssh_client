#!/bin/bash
# apt install (--reinstall) libssh2-1-dev
# apt install libcurl4-openssl-dev
echo "If this build fails, try to reinstall or install"
echo "		apt-get install -y --reinstall libssh2-1-dev  libcurl4-openssl-dev"
echo 

ldc=$(which ldconfig)
[[ -z ${ldc} ]] && echo "ldconfig not found" && exit 1
[[ -z $(${ldc} -p | grep libpthread) ]] && echo  "libpthread not found" && exit 1
[[ -z $(${ldc} -p | grep libcurl) ]] && echo " libcurl not found" && exit 1
[[ -z $(${ldc} -p | grep libssh2) ]] && echo  "libssh2 not found" && exit 1

arch=$(uname -m)
oss=$(uname)
bitss=""
echo "BUILD FOR ${arch} ${oss}"
sleep 3
if [[ $1 == "-m32" ]];then
    echo -e "if the build fails consider to install \n \ 
            linux-libc-dev:i386,  \n  \
            libssh2-1-dev:i386,  \n  \
            libcurl4-openssl-dev:i386"
    bitss="-m32"
else
    [[ ${arch} == "x86_64" ]] && echo "You can pass -m32 to build for a 32 LINUX OS"
    sleep 3
    bitss=""
fi

rm -rf *.o
rm -rf *.a
rm -rf ./meiotrev-${oss}-${arch}${bitss}

if [[ -d ../meiotrev/rssh_secret ]];then
    echo ".... building lib too ..."
    pushd ../meiotrev/rssh_secret
        echo ">>> ./make.sh ${bitss}"
        ./make.sh ${bitss}
    popd
else
    echo ".... using prebuild lib "
fi

[[ ! -f ./lib/lib_secret-${oss}-${arch}${bitss}.a ]] && "cannot find ./lib/lib_secret-${oss}-${arch}${bitss}.a" && exit 1

echo ">>> g++ *.cpp ${bitss} -std=c++14 -I./include -L./ -L./lib -l_secret-${oss}-${arch}${bitss} -lpthread  -lcurl -lssh2 -o meiotrev-${oss}-${arch}${bitss}"

stdcpp="-std=c++14"
[[ $arch == "i686" ]] && stdcpp="-std=c++11"

g++ *.cpp ${bitss} ${stdcpp} -I./include -L./ -L./lib -l_secret-${oss}-${arch}${bitss} -lpthread  -lcurl -lssh2 -o meiotrev-${oss}-${arch}${bitss}

rm -rf meiot-${oss}-${arch}${bitss}.tar
tar -czvf meiot-${oss}-${arch}${bitss}.tar meiotrev-${oss}-${arch}${bitss} install.sh
echo "DONE"


