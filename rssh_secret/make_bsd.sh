#!/usr/local/bin/bash
rm -rf *.o
rm -rf *.a
rm -rf ./meiotrev

c++ -std=c++0x -I/usr/local/include  -I../../rssh_client/include  -c -fPIC secret.cpp -o secret.o
c++ -std=c++0x -I/usr/local/include  -I../../rssh_client/include -c -fPIC md5.cpp -o md5.o
ar rcs lib_secret.a secret.o md5.o
