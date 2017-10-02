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

#ifndef SHSESSION_H
#define SHSESSION_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include "secret.h"

#ifndef INADDR_NONE
#define INADDR_NONE (in_addr_t)-1
#endif
class sshrev;
class shsession
{
public:
    shsession(const char* username, const char* pass,
              const char* server, int sport,
              const char* client, int cport,
              const char* priv, const char* pub,
              sshrev* psess);

    ~shsession();
    bool    establish();
    void    maintain();
    void    stop();
private:

    bool    _connect_local();
    bool    _open_ssh();
    bool    _forward() ;
    void    _dip();

private:
    std::string _user;
    std::string _passw;
    std::string _srv;
    int         _sport;
    std::string _cli;
    int         _cport=0;
    std::string _priv_key;
    std::string _pub_key;
    bool        _alive = false;
    bool        _listening=false;
    sshrev*     _sshrev=nullptr;

private:
    int               _locasock = 0;
    LIBSSH2_LISTENER *_listener = nullptr;
    LIBSSH2_CHANNEL  *_channel = nullptr;
    LIBSSH2_SESSION  *_session = nullptr;
};

#endif // SHSESSION_H
