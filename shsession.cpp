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
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include "shsession.h"
#include "sshrev.h"
#include "sysinfo.h"


shsession::shsession(const char* username,
                     const char* pass,
                     const char* server,
                     int sport,
                     const char* client,
                     int cport,
                     const char* priv,
                     const char* pub,
                     sshrev* prev):
    _user(username),
    _passw(pass),
    _srv(server),
    _sport(sport),
    _cli(client),
    _cport(cport),
    _priv_key(priv),
    _pub_key(pub),
    _sshrev(prev)
{
    LOG( _user << "@" << _srv << ":"<<_sport<<","<<_cport);
}

shsession::~shsession()
{
    if (_channel)
        libssh2_channel_free(_channel);
    if (_listener)
        libssh2_channel_forward_cancel(_listener);
    _listener=nullptr;
    if(_session){
        libssh2_session_disconnect(_session, "CLIENT DISCONNECTING");
        libssh2_session_free(_session);
    }
    if(_locasock>0)
        ::close(_locasock);
    libssh2_exit();
    LOG( "~shsession");
}

void    shsession::maintain()
{
    assert(_session);

    LOG("WAITING FOR REMOTE CONNECTION");
    _listening = true;
    _alive   = true;
    _channel = libssh2_channel_forward_accept(_listener);
    _listening = false;
    LOG("LISTENER EXITS");
    if(_alive)
    {
        LOG( " alive ");
        if (_channel==0)
        {
            LOG( "ERR: COULD NOT ACCEPT CONNECTION!. SERVER PROTOCOL MISMATCH");
        }
        else
        {
            _forward();
        }
    }else{
        LOG( "! ALIVE ");
    }
    if(_channel)
        libssh2_channel_free(_channel);
    _channel=nullptr;
}

void    shsession::stop()
{
    _alive = false;
    if(_listening){

        int srvsock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (srvsock > 0) {
            struct sockaddr_in sin;
            ::memset(&sin,0,sizeof(sin));
            sin.sin_family = AF_INET;
            sin.sin_addr.s_addr = inet_addr(_srv.c_str());
            sin.sin_port = htons(_sport);
            if (0 == ::connect(srvsock, (struct sockaddr*)(&sin),
                               sizeof(struct sockaddr_in)))
            {
                LOG("SELF CONNECTING");
                ::send(srvsock,"\r\n\r\n",4,0);
                ::usleep(0xFFFF);
                ::close(srvsock);
            }
        }
        _listening=false;
    }
}

bool shsession::establish()
{
    int rc = libssh2_init (0);
    if(rc)
    {
        LOG( "ERROR: libssh2_init: " << strerror(errno) );
        return false;
    }
    if (_connect_local())
        return _open_ssh();
    return false;
}

bool shsession::_connect_local()
{
    _locasock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_locasock <= 0) {
        LOG( "socket " << strerror(errno) );
        return false;
    }
    struct sockaddr_in sin;
    ::memset(&sin,0,sizeof(sin));
    sin.sin_family = AF_INET;
    _dip();
    sin.sin_addr.s_addr = inet_addr(_srv.c_str());
    if (INADDR_NONE == sin.sin_addr.s_addr)
    {
        LOG( "ERROR: inedaddr " << strerror(errno) );
        return false;
    }
    sin.sin_port = htons(_cport);   /* SSH port */
    if (connect(_locasock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        LOG( "socket " << strerror(errno) );
        return false;
    }
    return true;
}


void _func(LIBSSH2_SESSION*,
           void* ctx,
           const char *data,
           size_t l)
{
    (void)ctx;
    (void)l;
    std::cout << "CALLBACK: " << data << "\n";
}

bool shsession::_open_ssh()
{
    _session = libssh2_session_init();
    if(!_session) {
        LOG( "ERROR: libssh2_session_init " << strerror(errno) );
        return false;
    }

    libssh2_trace(_session, LIBSSH2_TRACE_AUTH |LIBSSH2_TRACE_CONN |LIBSSH2_TRACE_ERROR |LIBSSH2_TRACE_PUBLICKEY |LIBSSH2_TRACE_KEX |LIBSSH2_TRACE_SOCKET );
    libssh2_trace_sethandler(_session, this, &_func);
    libssh2_session_set_blocking(_session,1);
    int rc = libssh2_session_handshake(_session, _locasock);
    if(rc) {
        LOG( "ERROR: libssh2_session_handshake " << strerror(errno) );
        return false;
    }
/*
    const char *fingerprint = libssh2_hostkey_hash(_session, LIBSSH2_HOSTKEY_HASH_SHA1);
    fprintf(stdout, "Fingerprint: ");
    for(int i = 0; i < 20; i++)
        fprintf(stdout, "%02X ", (unsigned char)fingerprint[i]);
    fprintf(stdout, "\n");
*/
    int auth = 0;
    char *userauthlist = libssh2_userauth_list(_session,
                                               _user.c_str(),
                                               _user.length());
    if (strstr(userauthlist, "password"))
        auth |= 1;
    if (strstr(userauthlist, "publickey"))
        auth |= 2;

    secret_userauth_t autht = {
        _session,
        auth,
        _user.c_str(),
        nullptr,
        _pub_key.c_str(),
        _priv_key.c_str(),
        &libssh2_userauth_password_ex,
        &libssh2_userauth_publickey_fromfile_ex,
        &libssh2_userauth_publickey
    };
    int err = secret_rssh_authenticate(&autht);
    if(err!=0)
    {
        LOG("ERR: secret_rssh_authenticate " << strerror(err));
        return false;
    }

    int remote_listenport = 0;
    _listener = libssh2_channel_forward_listen_ex(_session,
                                                  _srv.c_str(),
                                                  _sport,
                                                  &remote_listenport,
                                                  1);
    if (nullptr==_listener)
    {
        LOG( "COULD NOT START THE TCPIP-FORWARD LISTENER. SERVER PROBLEM");
        return false;
    }
    return true;
}

bool shsession::_forward()
{
    int             len, i, rc = 0;
    struct sockaddr_in  sin;
    fd_set          fds;
    struct timeval  tv;
    ssize_t         wr;
    char            buf[8912];

    ::memset(&sin,0,sizeof(sin));
    LOG( "LOCAL %s:%d " <<  _srv << _sport);
    int iosock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (iosock == -1)
    {
        LOG( "ERROR OPENING SOCKET " << strerror(errno) );
        return false;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(_cport);
    sin.sin_addr.s_addr = inet_addr(_cli.c_str());
    if (INADDR_NONE == sin.sin_addr.s_addr)
    {
        LOG( "INVALID LOCAL IP ADDRESS");
        ::close(iosock);
        goto DONE;
    }

    if (-1 == ::connect(iosock,
                        (struct sockaddr *)&sin,
                        sizeof(struct sockaddr_in)))
    {
        LOG( "CONNECT FAILED");
        ::close(iosock);
        goto DONE;    }

    LOG( "FORWARDING FROM:"<< _srv << _sport << " TO " <<_cli << _cport);

    ::libssh2_session_set_blocking(_session, 0);

    _alive=true;
    while (_alive)
    {
        FD_ZERO(&fds);
        FD_SET(iosock, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0xFFFF;
        rc = ::select(iosock + 1, &fds, NULL, NULL, &tv);
        if (-1 == rc) {
            LOG( "SOCKET FATAL SELECT " << strerror(errno) );
            _alive = false;
            goto DONE;
        }
        if (rc && FD_ISSET(iosock, &fds))
        {
            len = ::recv(iosock, buf, sizeof(buf)-1, 0);
            if (len < 0)
            {
                LOG( "SOCKET FATAL RECV " << strerror(errno) );
                _alive = false;
                goto DONE;
            }
            else if (0 == len)
            {
                LOG( "CLOSED " << strerror(errno) );
                _alive = false;
                goto DONE;
            }
            wr = 0;
            while(len-wr)
            {
                i = ::libssh2_channel_write(_channel, buf+wr, len-wr);
                if (i < 0) {
                    if(i==LIBSSH2_ERROR_EAGAIN)
                    {
                        ::usleep(0xFFFF);
                        continue;
                    }
                    LOG( "LIBSSH2_CHANNEL_WRITE " <<":"<<errno<<":"<< strerror(errno) );
                    _alive = false;
                    goto DONE;
                }
                wr += i;
            }
            _sshrev->alive();
        }
        while (_alive)
        {
            len = ::libssh2_channel_read(_channel, buf, sizeof(buf)-1);
            if (LIBSSH2_ERROR_EAGAIN == len)
            {
                ::usleep(0xFF);
                break;
            }
            else if (len < 0)
            {
                //LIBSSH2_ERROR_CHANNEL_CLOSED
                LOG( "LIBSSH2_CHANNEL_READ " << strerror(errno) );
                _alive = false;
                goto DONE;
            }
            wr = 0;
            // std::cout << "\n[" << buf << "]\n";
            // char dest[8912]={0};
            // unsigned int dl=0;
            // libssh2_base64_decode(_session, (char**)&dest, &dl, buf, len);
            // std::cout << "\n[" << dest << "]\n";

            while (wr < len)
            {
                i = ::send(iosock, buf + wr, len - wr, 0);
                if (i <= 0)
                {
                    LOG( "SEND " << strerror(errno) );
                    _alive = false;
                    goto DONE;
                }
                wr += i;
            }
            if (0!=(i=::libssh2_channel_eof(_channel)))
            {
                if(i<0)
                {
                    LOG( i <<" LIBSSH2_CHANNEL_EOF " << strerror(errno) );
                    _alive = false;
                    goto DONE;
                }
            }
            _sshrev->alive();
        }
        ::usleep(0xFF);
    }

DONE:
    LOG( "SSH SESSION ENDED ");
    ::close(iosock);
    libssh2_session_set_blocking(_session, 1);
    return _alive;
}

void shsession::_dip()
{
	struct hostent	*he;
	int				a,b,c,d;

	if(4 == ::sscanf(_srv.c_str(), "%d.%d.%d.%d", &a, &b ,&c, &d))
	{
		return;
	}
	std::string srv = _srv;
	size_t dots =  srv.find(":");
	if(dots != std::string::npos)
	{
		srv = _srv.substr(dots+3);
	}
	if ( (he = gethostbyname(srv.c_str())) == 0)
	{
		return;
	}
	_srv =  inet_ntoa(*((struct in_addr*) he->h_addr_list[0]));
}
