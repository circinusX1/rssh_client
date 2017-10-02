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
#include <iostream>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include "md5.h"
#include "sysinfo.h"
#include "perulink.h"
#include "sshrev.h"
#include "secret.h"


perulink::perulink(sshrev* prev,
                   const char* addr,
                   int port,
                   const char* seed):_t(nullptr),
                                    _prev(prev),
                                    _seed(seed),
                                    _ok(false)
{
    if(!this->_from_string(addr, port))
    {
        LOG("INTERNET NOT REACHABLE.");
        return;
    }
    _ok=true;
}

perulink::~perulink()
{
    _alive = false;
    ::usleep(0xffff<<1);
    if (_t && _t->joinable())
    {
        _t->join();
    }
    delete _t;
}

void perulink::thread()
{
    for(int i=4000; i< 4100;i++)
    {
        if(_create(i) && _bind()!=-1)
        {
            _perport=i;
            break;
        }
        _destroy();
    }
    if(_thesock>0)
    {
        LOG( "UDP PORT:" << _perport);
        while(_alive)
        {
            ::usleep(0xffff);
            const std::string& push_udp_msessage = _push_udp_msessage();
            if(!push_udp_msessage.empty())
            {
                for(int i=0;i<1;i++)
                {
                    _sendsin((const uint8_t*)push_udp_msessage.c_str(),
                             (size_t)push_udp_msessage.length(),
                             _srv_sin);
                    ::usleep(0xffff);
                }
            }
            ::usleep(0xFFFF);
            _receive();
        }
    }
}

bool perulink::_receive()
{
    fd_set      rdSet;
    int         nfds = _thesock+1;
    timeval     tv   = {0,1024};

    FD_ZERO(&rdSet);
    FD_SET(_thesock, &rdSet);
    int sel = ::select(nfds, &rdSet, 0, 0, &tv);
    if(sel < 0)
    {
        LOG("ERR: NETWORK SUBSYSTEM FAILURE (select):" << errno);
        _destroy();
        exit(errno);
    }
    if(sel > 0 && FD_ISSET(_thesock, &rdSet))
    {
        return _rec_udp() > 0;
    }
    return false;
}

int perulink::_rec_udp()
{
    char local[512] = {0};
    socklen_t iRecvLen   = sizeof(_srv_sin);
    int rcv =  (int)::recvfrom (_thesock, (char*)local, 511,
                                0,
                                (struct sockaddr  *) &_srv_sin,
                                &iRecvLen);
    if(rcv>0)
    {
        LOG("UDP GOT:" << rcv << " BYTES");
        if(secret_udp_validated(local,rcv))
        {
            LOG("UDP SIGNALING");
            _prev->signal();
            LOG("UDP SIGNALLED");
        }
        return 1;
    }
    return 0;
}

int  perulink::_send(const uint8_t* buff, const int length)
{
    std::basic_string<uint8_t> enc = secret_udp_get_buffer(buff,length);
    int snd  = ::sendto(_thesock, (char*)enc.data(), enc.length(), 0,
                        (struct sockaddr  *) &_srv_sin,
                        sizeof(_srv_sin)) ;
    return snd;
}

int  perulink::_sendsin(const uint8_t* buff, size_t length, struct sockaddr_in& rsin)
{
    std::basic_string<uint8_t> enc = secret_udp_get_buffer(buff,length);
    int snd = ::sendto(_thesock, (char*)enc.data(), enc.length(), 0,
                       (struct sockaddr  *) &rsin,
                       sizeof(rsin)) ;
    return snd;
}

int perulink::_create(int port, int proto, const char* addr)
{
    assert(_thesock<=0);
    _thesock = ::socket(AF_INET, SOCK_DGRAM, proto);
    if((int)-1 == (int)_thesock)
        return -1;
    ::memset(&_local_sin, 0, sizeof(_local_sin));

    _local_sin.sin_family        = AF_INET;
    _local_sin.sin_addr.s_addr   = addr ? inet_addr(addr): htonl(INADDR_ANY);
    _local_sin.sin_port          = htons(port);
    return _thesock;
}

bool perulink::_destroy()
{
    bool b=false;
    if((int)_thesock > 0)
    {
        int k = 32;
        while(-1==::close(_thesock) && --k>0)
        {
            usleep(0xFF);
        }
        b=true;
    }
    _thesock = -1;
    return b;
}

bool perulink::_from_string( const char* r, uint16_t port)
{
    char l[128];

    ::strcpy(l,r);
    if(!strstr(l,"https://"))
    {
        LOG("INVALID HOST. eg: https://domain.com");
        std::cerr << "INVALID HOST. eg: https://domain.com\n";
        exit(1);
    }
    hostent *h;
    strtok(l,"://");
    char* dn = strtok(nullptr,"://");
    if(dn==nullptr)
    {
        LOG("INVALID HOST. eg: https://domain.com");
        std::cerr << "INVALID HOST. eg: https://domain.com\n";
        exit(1);
    }
    h = gethostbyname(dn);

    if(h && h->h_addr_list)
    {
        char *temp = h->h_addr_list[0];
        in_addr *addr = reinterpret_cast<in_addr*>(temp);

        _srv_sin.sin_addr.s_addr = addr->s_addr;
        _srv_sin.sin_port = htons(port);
        _srv_sin.sin_family=AF_INET;
        LOG("UDP TO: " << inet_ntoa(_srv_sin.sin_addr));
        return true;
    }
    return false;
}

int  perulink::_bind(const char* addr, int port)
{
    if(addr)
        _local_sin.sin_addr.s_addr = inet_addr(addr);
    if(port)
        _local_sin.sin_port = htons(port);
    assert(_local_sin.sin_port > 0);
    if(::bind(_thesock,(struct sockaddr *)&_local_sin, sizeof(_local_sin)))
    {
        return -1;
    }
    return _thesock;
}

