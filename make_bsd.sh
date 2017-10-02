#!/usr/local/bin/bash

#pkg install libssh2-1-dev
#pkg install libcurl4-openssl-dev

[[ -z $(locate libpthread.so | grep libpthread) ]] && echo  "libpthread not found" && exit 1
[[ -z $(locate libcurl.so) ]] && echo " libcurl not found" && exit 1
[[ -z $(locate libssh2.so) ]] && echo  "libssh2 not found" && exit 1

arch=$(uname -m)
oss=$(uname)

rm -rf  *.o
rm -rf *.a
rm -rf ./meiotrev-${oss}-${arch}

if [[ -d ../meiotrev/rssh_secret ]];then
    pushd ../meiotrev/rssh_secret
        ./make_bsd.sh
    popd
fi
[[ ! -f ./lib/lib_secret-${oss}-${arch}.a ]] && "cannot find ./lib/lib_secret-${oss}-${arch}.a" && exit 1
c++ *.cpp -std=c++14 -I/usr/src/sys/compat/linux \
-I/usr/include/net \
-I/usr/local/include -I./include -L/usr/local/lib \
-L./ -L./lib -l_secret-${oss}-${arch} -lpthread  -lcurl -lssh2 -o meiotrev-${oss}-${arch}
tar -czvf meiot-${oss}-${arch}.tar ./meiotrev-${oss}-${arch} install.sh
