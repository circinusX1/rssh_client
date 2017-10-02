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

#ifndef __SECRET_H
#define __SECRET_H

#include <string>
#include <stdio.h>
#include <stdint.h>
#include <libssh2.h>
#include <libssh2_publickey.h>
#include <libssh2_sftp.h>

#include "md5.h"


typedef int (*pfn_libssh2_userauth_password_ex)(LIBSSH2_SESSION *session,
                                             const char *username,
                                             unsigned int username_len,
                                             const char *password,
                                             unsigned int password_len,
                                             LIBSSH2_PASSWD_CHANGEREQ_FUNC((*passwd_change_cb)));
typedef int  (*pfn_libssh2_userauth_publickey_fromfile_ex)(LIBSSH2_SESSION *session,
                                       const char *username,
                                       unsigned int username_len,
                                       const char *publickey,
                                       const char *privatekey,
                                       const char *passphrase);
typedef int (*pfn_libssh2_userauth_publickey)(LIBSSH2_SESSION *session,
                           const char *username,
                           const unsigned char *pubkeydata,
                           size_t pubkeydata_len,
                           LIBSSH2_USERAUTH_PUBLICKEY_SIGN_FUNC((*sign_callback)),
                           void **abstract);

typedef struct secret_userauth_t{
    LIBSSH2_SESSION* session;
    int         login;
    const char* user;
    const char* passwd;
    const char* pub;
    const char* priv;
    pfn_libssh2_userauth_password_ex  p_libssh2_userauth_password_ex;
    pfn_libssh2_userauth_publickey_fromfile_ex p_libssh2_userauth_publickey_fromfile_ex;
    pfn_libssh2_userauth_publickey p_libssh2_userauth_publickey;
}secret_userauth_t;


std::string                 secret_reg_request(const std::string& url,
                                         const char* systeminfo,
                                         const std::string& bauser,
                                         const std::string& bapassw,
                                         std::string& dp, const std::string& pkf);
const char*                 secret_auto_string();
std::basic_string<uint8_t>  secret_ed(const uint8_t *in, int len, const uint32_t __key[4], bool encrypt);
std::basic_string<uint8_t>  secret_base64_decode(const std::string& in);
std::string                 secret_base64_encode(const std::string &in);
std::string                 secret_hex_encode(const std::basic_string<uint8_t>& in);
std::basic_string<uint8_t>  secret_hex_decode(const std::string& in);
bool                        secret_udp_validated(const char* buf, int len);
int                         secret_rssh_authenticate(const secret_userauth_t* p);
std::basic_string<uint8_t>  secret_udp_get_buffer(const uint8_t* loco, int length);
void                        secret_get_pr(const std::string& pkpk, const std::string& device, int pkl,int k4, std::string& rpk, std::string& rPK);
std::string                 secret_salt1(int rnd);


class ClientCert
{
public:
    ClientCert();
    ~ClientCert();
};

#endif //__SECRET_H
