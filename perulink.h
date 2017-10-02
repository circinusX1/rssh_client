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

#ifndef PERULINK_H
#define PERULINK_H

#ifdef __FreeBSD__
#	include <netinet/in.h>
#endif
#include <sys/socket.h>
#include <resolv.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netdb.h>
#include <thread>
#include <mutex>


class sshrev;
class perulink
{
public:
    perulink(sshrev* prev, const char* addr, int port, const char* seed);
    ~perulink();
    void thread();
    void push_udp_msessage(const std::string& msg){
        std::lock_guard<std::mutex> guard(_m);
        _msg = msg;
    }
    int port()const{return _perport;}
    bool ok(){
        _t = new std::thread (&perulink::thread, this);
        return _ok;
    }
private:
    int _create(int port, int proto=0, const char* addr=nullptr);
    int _listen();
    bool _destroy();
    std::string _push_udp_msessage(){
        std::string ret;
        std::lock_guard<std::mutex> guard(_m);
        ret = _msg;
        _msg.clear();
        return ret;
    }
    bool _from_string( const char* r, uint16_t port=0);
    int  _send(const uint8_t* buff, const int length);
    int  _sendsin(const uint8_t* buff, size_t length, struct sockaddr_in& rsin);
    bool _receive();
    int _rec_udp();
    int _bind(const char* ip=nullptr, int port=0);
private:

    bool               _alive = true;
    std::thread*       _t;
    std::mutex         _m;
    int                _thesock = 0;
    int                _listener = 0;
    int                _perport=-1;
    std::string        _msg;
    struct sockaddr_in _local_sin;
    struct sockaddr_in _srv_sin;
    int32_t            _mask=0;
    sshrev*            _prev=nullptr;
    std::string        _seed;
    bool               _ok=false;
};

#endif
