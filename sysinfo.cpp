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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <assert.h>
#ifdef __FreeBSD__
#	include <if.h>
#else
#	include <linux/if.h>
#endif
#include <string>
#include "sysinfo.h"
#include "secret.h"
#include <fstream>

sysinfo::sysinfo()
{

#if 0
    _info = ::std::to_string(time(0));
    return;
#endif
    struct utsname u;
    if ( uname( &u ) == 0 )
    {
        _info += u.nodename;
    }

    _info += _cpuid();
#ifndef __FreeBSD__
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP );
    if ( sock > 0 ){
        struct ifconf conf;
        char ifconfbuf[ 128 * sizeof(struct ifreq)  ];
        memset( ifconfbuf, 0, sizeof( ifconfbuf ));
        conf.ifc_buf = ifconfbuf;
        conf.ifc_len = sizeof( ifconfbuf );
        if ( ioctl( sock, SIOCGIFCONF, &conf )==0)
        {
            struct ifreq* ifr;
            for ( ifr = conf.ifc_req; (char*)ifr < (char*)conf.ifc_req + conf.ifc_len; ifr++ )
            {
                if ( ifr->ifr_addr.sa_data == (ifr+1)->ifr_addr.sa_data )
                    continue;  // duplicate, skip it

                if ( ioctl( sock, SIOCGIFFLAGS, ifr ))
                    continue;  // failed to get flags, skip it
                if ( ioctl( sock, SIOCGIFHWADDR, ifr ) == 0 )
                {
                    _info += _smac( (unsigned char*)&(ifr->ifr_addr.sa_data));
                    _info += "-";
                }
            }
        }
        close( sock );
    }
#endif
}

std::string sysinfo::_cpuid()
{
    char u[256] = {0};
    if(::access("/etc/rssh_machine_id",0)==0)
    {
        std::ifstream pf ("/etc/rssh_machine_id",std::ifstream::binary);
        pf.read ((char*)u,sizeof(u)-1);
        pf.close();
    }
    else {
        printf("The program was not installed. /etc/rssh_machine_id missing \n ");
        exit (2);
    }
    std::string uu; uu.assign((const char*)u,16);
    return secret_base64_encode(uu);
}

std::string sysinfo::_smac( unsigned char* mac )
{
    std::string out;
    char chk[8];

    for ( unsigned int i = 0; i < 6; i++ )
    {
        sprintf(chk, "%02X", int(mac[i]&0xFF));
        out.append(chk);
    }
    return out;
}

