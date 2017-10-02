#!/bin/bash
rm -rf *.o
rm -rf *.a
rm -rf ./meiotrev

g++ -c -fPIC -I../../rssh_client/include secret.cpp -o secret.o
g++ -c -fPIC  -I../../rssh_client/include md5.cpp -o md5.o
ar rcs lib_secret.a secret.o md5.o

