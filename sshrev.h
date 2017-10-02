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

#ifndef SSHREV_H
#define SSHREV_H

#include <string>
#include <vector>
#include <curl/curl.h> // sudo apt update && sudo apt install libcurl4-openssl-dev
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <stdio.h>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "perulink.h"
#include "secret.h"

#define SOME_LOOPS 4
#define CONFIG_FILE "/etc/.mylinuz.com"
enum ECMD_C{eNOTHING, eSTART, eHOLD, eSTOP};

class shsession;
class sshrev
{
public:
    sshrev(const char* url);
    ~sshrev();
    void alive(){_age = time(0);}
    void signal(){
        std::lock_guard<std::mutex> lk(_m);
        _signalled= SOME_LOOPS;
    }
    void thread();
    bool stopping(){
        return _stopping;
    }
private:
    void        _run();
    bool        _register();
    ECMD_C      _get_cmd();
    void        _udp_delay();
    void        _tstart();
    void        _tstop();

private:
    std::thread*        _t = nullptr;
    size_t              _signalled;
    std::atomic<bool>   _stopping;
    bool        _alive = true;
    std::string _url;
    std::string _priv_key;
    std::string _pub_key;
    std::string _device;
    std::string _ba_user;
    std::string _ba_pass;
    std::string _ba_devname;
    std::string _rsshuser;
    std::string _server;
    std::string _client;
    int         _cli_session = 0;
    int         _srv_port = 0;
    int         _local_port = 0;
    time_t      _age = 0;
    time_t      _sleep = 5;
    shsession*  _ssh_tunel = nullptr;
    perulink*   _udper = nullptr;

    std::mutex              _m;
    std::condition_variable _cv;
    bool                    _readyt = false;
    bool                    _dirty  = true;
    bool                    _online = false;
    bool                    _autenticated = false;
};


#define TRY_IT(X_) try{X_;}catch(std::runtime_error& e){\
    std::cout << e.what();\
}



#endif // SSHREV_H
