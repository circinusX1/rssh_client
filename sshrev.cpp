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
#include <fstream>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include "main.h"
#include "sshrev.h"
#include "picojson.h"
#include "shsession.h"
#include "per_id.h"
#include "md5.h"
#include "sysinfo.h"
#include "secret.h"

using namespace picojson;
#define T_OUT           600
#ifdef DEBUG
#   define STOP_DELAY       10
#   define OFF_LINE_DELAY   5
#else
#   define STOP_DELAY      60
#   define OFF_LINE_DELAY  30
#endif
#define RUN_DELAY       10
#define UDP_PORT        29630
#define RHHH_USER       "rssh"
#define UDP_SEED        "rssh__1963_12_05"

typedef CURLcode *p_curl(CURL *curl, CURLoption option, ...);

sshrev::sshrev(const char* url):
    _url(url),
    _sleep(STOP_DELAY)
{
    std::ifstream file(CONFIG_FILE);
    if (file.is_open()) {
        std::getline(file, _ba_user);
        std::getline(file, _ba_pass);
    	std::getline(file, _ba_devname);
        file.close();
    }
    _signalled = 0;
    _alive = true;
    _stopping = false;
    _run();
}

sshrev::~sshrev()
{
    delete _udper;
    _tstop();
}

static size_t __writefunc(void *ptr, size_t size, size_t nmemb, std::string* s)
{
    char en[8912];
    if((size*nmemb) < sizeof(en)-1)
    {
        memcpy(en, ptr, size * nmemb);
        en[size*nmemb]=0;
        s->append(en);
        LOG( en );
        return size*nmemb;
    }
    return 0; // to big
}

/*
controlling json {"access":"granted"}
*/
bool sshrev::_register()
{
    bool        rv = false;
    sysinfo     sinfo;
    std::string ret;
    std::string req = ::secret_reg_request(_url,
                                           sinfo.info(),
                                           _ba_user,
                                           _ba_pass,
                                           _device,
                                           ret);
    req+="&dn="; req+=_ba_devname;
    LOG("-------------------------------");
    LOG("DEVICE ID        : " << _device);
    LOG("DEVICE NAME      : " << _ba_devname);
    LOG("MYLINUX USER        : " << _ba_user);
    LOG("MYLINUX PASSWORD    : " << _ba_pass);
    std::cout << "DEVICE ID        : " << _device << "\n";
    std::cout << "DEVICE NAME      : " << _ba_devname << "\n";
    std::cout << "MYLINUX USER        : " << _ba_user << "\n";
    std::cout << "MYLINUX PASSWORD    : " << _ba_pass << "\n";

    ::curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL*  curl = ::curl_easy_init();
    if(curl) {
        LOG(req);
        // static const char   _Cert[]="XXX.pem";
        curl_easy_setopt(curl, CURLOPT_URL, req.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&ret);
        // curl_easy_setopt(curl, CURLOPT_SSLCERT, "./.meiot_curl_cert");         // client side cert
        //curl_easy_setopt(curl, CURLOPT_SSLKEY, _Key);         // not used
        curl_easy_setopt(curl, CURLOPT_KEYPASSWD, "mariusc");

        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            _online = false;
            _autenticated = false;
            LOG("ERR: CURL_EASY_PERFORM: " <<
                curl_easy_strerror(res) );
        }
        curl_easy_cleanup(curl);
    }
    ::curl_global_cleanup();
    if(!ret.empty())
    {
        try{
            picojson::value pout;
            picojson::parse(pout, ret);
            std::string access;
            std::string error;
            std::string pkpk;
            int         k4=0, pkl=0;


            TRY_IT(error      = pout.get(_S("E")).get<std::string>()); //error
            if(!error.empty())
            {
                std::cout << error << "\r\n";
                LOG(error);
                exit(1);
            }
            TRY_IT(access     = pout.get(_S("A")).get<std::string>()); //acess
            TRY_IT(pkpk       = pout.get(_S("PP")).get<std::string>());
            TRY_IT(_rsshuser  = pout.get(_S("U")).get<std::string>());
            TRY_IT(k4         = pout.get(_S("KK"),true));
            TRY_IT(pkl        = pout.get(_S("KL"),true));
            if(!pkpk.empty())
            {
                ::secret_get_pr(pkpk, _device, pkl, k4, _priv_key, _pub_key);
                if(access != "granted")
                {
                    LOG("ERR: https://mylinux.mine.nu" << _device );
                    return rv;
                }
                if(_priv_key.empty() || _pub_key.empty())
                {
                    LOG("ERR: KEYS: ARE EMPTY" );
                    return rv;
                }
                rv=true;
            }else
            {
                LOG("ERR: NOT A JSON" );
                return rv;
            }
        }catch(std::runtime_error& e)
        {
            LOG("ERR: BAD JSON");
        }
    }
    return rv;
}

/**
 * @brief sshrev::_get_cmd
 * @return
 * queries a web with remote_web_cmd()
 */
ECMD_C sshrev::_get_cmd()
{
    std::string ret;
    int         rnd    = (int)time(0);
    std::string rand   = std::to_string(rnd);
    std::string salted = secret_salt1(rnd);

    std::string req = _url + _S("/__get.php");
    std::string args= _S("?k=") +
            _S(secret_auto_string()) +
            _S("&slt=") + salted +
            _S("&pep=") + rand +
            _S("&did=") + _device +
            _S("&uuu=") + std::to_string(_udper->port());

    ::curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL*  curl = ::curl_easy_init();
    if(curl) {
        std::string all=req+args;
        LOG(all);
        curl_easy_setopt(curl, CURLOPT_URL, all.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&ret);

        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            _online = false;
            _autenticated = false;
            LOG("ERR: curl_easy_perform: " <<
                ::curl_easy_strerror(res) );
        }
        ::curl_easy_cleanup(curl);
    }
    ::curl_global_cleanup();
    if(ret.size())
    {
        picojson::value pout;
        int             session = -1;
        std::string     cmd;

        if(_udper)
        {
            _udper->push_udp_msessage(args);
        }
        try{
            picojson::parse(pout, ret);
            _server   = pout.contains(_S("URL")) ? pout.get(_S("URL")).get<std::string>() : "";
            cmd       = pout.contains(_S("CMD")) ? pout.get(_S("CMD")).get<std::string>() : "";
            session   = pout.contains(_S("SES")) ? pout.get(_S("SES"),true) : 0;

            if(cmd=="start")
            {
                if(_cli_session && _cli_session != session)
                {
                    _tstop();
                }
                _cli_session = session;
                TRY_IT(_srv_port    = pout.get(_S("RP"),true));
                TRY_IT(_local_port  = pout.get(_S("LP"),true));
                int isleep = 0;
                TRY_IT(isleep  = pout.get(_S("DELAY"),true));
                if(isleep>5 && isleep<121)
                    _sleep = isleep;
                else
                    _sleep = RUN_DELAY;
                return eSTART;
            }
            if(_cli_session != session || cmd=="stop")
            {
                _sleep=STOP_DELAY;
                return eSTOP;
            }
            if(cmd=="hold")
            {
                return eHOLD;
            }
        }
        catch(std::runtime_error& e)
        {
            LOG( e.what() );
        }
    }
    return eSTOP;
}

void sshrev::_udp_delay()
{
    time_t fut = time(0) + _sleep;
    while(time(0) < fut)
    {
        do{
            if(_signalled > 0 && _signalled > 0)
            {
                --_signalled;
                goto DONE;
            }
        }while(0);
        ::usleep(0xFFFF);
    }
DONE:
    ::usleep(0xFFFF);
}

void sshrev::_run()
{
    while(_alive)
    {
        if(_online==false)
        {
            delete _udper;
            _udper = nullptr;
            _udper = new perulink(this, _url.c_str(), UDP_PORT, UDP_SEED);

            if(_udper->ok()==false)
            {
                ::sleep(OFF_LINE_DELAY);
                LOG("NO INTERNET. TRYING AGAIN IN 30");
                continue;
            }
            _online=true;
        }
        if(!_autenticated)
        {
            _autenticated = _register();
            LOG(" REGISTERRING");
            if(!_autenticated){
                ::sleep(OFF_LINE_DELAY);
                continue;
            }else {
                LOG("SUCCESSFULLY REGISTERRED");
                std::ofstream file(CONFIG_FILE);
                if (file.is_open()) {
                    file << _ba_user << "\n";
                    file << _ba_pass << "\n";
                    file << _ba_devname << "\n";
                    file << _device << "\n";
                    file.close();
                }
            }
        }

        if(_stopping)
        {
            LOG("OVERLAPPED STOPPING");
            sleep(1);
            continue;
        }
        ECMD_C c = _get_cmd();
        LOG("COMMAND = " << c);
        switch(c)
        {
        case eSTART:
            if(_t==nullptr)
            {
                LOG("STARTING SSH THREAD");
                _age = 0;
                do{
                    std::unique_lock<std::mutex> lk(_m);
                    _readyt = false;
                }while(0);
                _tstart();
                do{
                    std::unique_lock<std::mutex> lk(_m);
                    _cv.wait(lk, [this]{return _readyt;});
                }while(0);
            }
            else {
                if(_cli_session == 0)
                {
                    _tstop();
                    LOG("ALREADY STOPPED");
                }
                else{
                    LOG("RUNNING SSHTHREAD");
                }
            }
            break;
        case eSTOP:
        default:
            if(_t)
            {
                _stopping=true;
                LOG("STOPPING SSHTHREAD");
                _tstop();
                LOG("STOPPED");
                _stopping=false;
            }
            break;
        }

        if(_t && _age && time(0)-_age > T_OUT)
        {
            _age = time(0);
            LOG(" TO OLD, STOPPING SSH THREAD");
            int sess = _cli_session;
            _tstop();
            _cli_session = sess;
        }
        _udp_delay();
    }
}

void sshrev::thread()
{
    _client = "127.0.0.1";
    _age = time(0);

    LOG("STARTING SSHSESSION THREAD");
    _ssh_tunel = new shsession(RHHH_USER,
                               RHHH_USER,
                               _server.c_str(),
                               _srv_port,
                               _client.c_str(),
                               _local_port,
                               _priv_key.c_str(),
                               _pub_key.c_str(), this);
    if(_ssh_tunel->establish())
    {
        if(_readyt==false)
        {
            std::lock_guard<std::mutex> lk(_m);
            _readyt = true;
            _cv.notify_one();
        }
        _ssh_tunel->maintain();
    }

    if(_ssh_tunel){
        std::lock_guard<std::mutex> lk(_m);
        delete _ssh_tunel;
        _ssh_tunel = nullptr;
    }

    if(_readyt==false)
    {
        std::lock_guard<std::mutex> lk(_m);
        _readyt = true;
        _cv.notify_one();
    }
    _cli_session = 0;
    LOG("STOPPED  SSHSESSION  THREAD");
}


void sshrev::_tstart()
{
    _tstop();
    if(_t==nullptr)
        _t = new std::thread (&sshrev::thread, this);
}

void sshrev::_tstop()
{
    if(_ssh_tunel)
    {
        _ssh_tunel->stop();
    }
    if(_t){
        _t->join();
    }
    if(_t){
        std::lock_guard<std::mutex> lk(_m);
        delete _t;
        _t = nullptr;
    }
}
