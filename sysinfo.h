/*
Copyright (c) 2016 github.com/circinusX1 marrius9876@g---l.com.
All rights reserved.

Redistribution and use in source and binary forms are
permitted provided that the above copyright notice and
this paragraph are duplicated in all such forms and that
any documentation, advertising materials, and other materials
related to such distribution and use acknowledge that the
software was developed by the github.com/circinusX1. The name of the
circinusX1 may not be used to endorse or promote products
derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE.
 * */

#ifndef SYSINFO_H
#define SYSINFO_H

#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <mutex>

#define _S(X_) std::string(X_)

class sysinfo
{
public:
    sysinfo();
    const char* info()const {return _info.c_str();}

private:
    std::string _smac( unsigned char* mac );
    std::string _cpuid();
    int _getVolumeHash();
    std::string _info;
};

class Fileit
{
    std::mutex __guard;
    char       __date[32];
public:
    Fileit(){}

    const char* ct()
    {
        time_t  curtime = time(0);

        ::strcpy(__date, ctime(&curtime));
        char *pe = strchr(__date, '\r');
        if(pe) *pe = 0;
        pe = strchr(__date, '\n');
        if(pe) * pe = 0;
        return __date;
    }

    void log (std::stringstream& ss)
    {
        std::lock_guard<std::mutex> guard(__guard);

        FILE *log = ::fopen("/var/log/meiotrev.log", "ab");
        if (!log){log = fopen("/var/log/meiotrev.log", "wb");}
        if (!log)
        {
            std::cout << "can not open /var/log/meiotrev.log for writing.\n";
            std::cout.flush();
            std::cout << "\r\n";
            return;
        }
        ::fwrite(ss.str().c_str(),sizeof(char),ss.str().length(),log);
        ::fclose(log);
    #ifdef DEBUG
        std::cout << ss.str();
    #endif
    }
};

#define LOG(x) \
do{\
    Fileit f; \
    std::stringstream ss; ss  << f.ct() << " " << x << "\n";\
    f.log(ss); \
}while(0);

#endif // SYSINFO_H
