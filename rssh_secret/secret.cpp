
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <string>
#include <fstream>
#include "md5.h"
#include "secret.h"


#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
using namespace std;

#define _S(X_) std::string(X_)
#define BLOCK_SIZE 8

static const char sauth[] = "Marius Chincisan";

// not used in curl
static const char __curl_cli_key[] = "\n"
                                     "Bag Attributes\n"
                                     "    localKeyID: FC 48 E7 6E EE 06 87 51 4C EE A6 0E 74 30 51 79 B9 94 E3 AB \n"
                                     "subject=C = CA, ST = ONTARIO, L = RICHMOND HILL, O = CANADA-1002852LTD, CN = rssht.mine.nu\n"
                                     "\n"
                                     "issuer=C = CA, ST = ONTARIO, L = RICHMOND HILL, O = CANADA-1002852LTD, CN = CANADA-1002852LTD\n"
                                     "\n"
                                     "-----BEGIN CERTIFICATE-----\n"
                                     "MIIDZjCCAk6gAwIBAgIUXJ6lO4TxPXjxUUrwMfSPE4JSFI0wDQYJKoZIhvcNAQEL\n"
                                     "--removed--\n"   
                                     "Tc+5/7GbxWUo+w==\n"
                                     "-----END CERTIFICATE-----\n"
                                     "Bag Attributes: <No Attributes>\n"
                                     "subject=C = CA, ST = ONTARIO, L = RICHMOND HILL, O = CANADA-1002852LTD, CN = CANADA-1002852LTD\n"
                                     "\n"
                                     "issuer=C = CA, ST = ONTARIO, L = RICHMOND HILL, O = CANADA-1002852LTD, CN = CANADA-1002852LTD\n"
                                     "\n"
                                     "-----BEGIN CERTIFICATE-----\n"
                                     "MIIDvzCCAqegAwIBAgIUOelJO019n7kpBzPR85EemLkyEjowDQYJKoZIhvcNAQEL\n"
                                     "=removed=\n"
                                     "4DDMkSM57J0hMEllpllhbJt4Jz++mYXO5u/rnChmX0yTbSVFpS7Te0ihqgW2vPEe\n"
                                     "sd0M\n"
                                     "-----END CERTIFICATE-----\n"
                                     "Bag Attributes\n"
                                     "    localKeyID: FC 48 E7 6E EE 06 87 51 4C EE A6 0E 74 30 51 79 B9 94 E3 AB \n"
                                     "Key Attributes: <No Attributes>\n"
                                     "-----BEGIN PRIVATE KEY-----\n"
                                     "=removed=\n"
                                     "ZcBE+ENM6Wm9jCALd8vLd0S+\n"
                                     "-----END PRIVATE KEY-----\n"
                                     "\n";


void xtea_decipher(unsigned int num_rounds,
                   uint32_t v[2],
uint32_t const __key[4]);
void xtea_encipher(unsigned int num_rounds,
                   uint32_t v[2],
uint32_t const __key[4]);

std::basic_string<uint8_t> secret_base64_decode(const std::string& in);
std::string secret_base64_encode(const std::string &in);
std::string secret_hex_encode(const std::string &in);

char auth[]  = "------------------------------------ tweaked";

const char* secret_auto_string()
{
    char* p = auth;
    *(p+5)=1; /*tweaked*/
    *(p+5)=2; /*tweaked*/
    *(p+5)=3; /*tweaked*/
    *(p+5)=4; /*tweaked*/
    return auth;
}


std::string secret_reg_request(const std::string& url,
                               const char* systeminfo,
                               const std::string& bauser,
                               const std::string& bapassw,
                               std::string& dp,
                               const std::string& pkf)
{
    std::string request = url + _S("/__page_tweaked.php?k=") + _S(auth) + _S("&did=");
    dp = md5(_S(systeminfo)+bapassw);
    dp+=dp.at(13);    /*tweaked*/
    dp+=dp.at(9);    /*tweaked*/
    dp+=dp.at(0);   /*tweaked*/
    dp+=dp.at(17);   /*tweaked*/
    dp+=dp.at(3);    /*tweaked*/
    dp+=dp.at(11);    /*tweaked*/
    dp+=dp.at(2);   /*tweaked*/
    dp+=dp.at(30);   /*tweaked*/
    request += dp;

    std::basic_string<uint8_t> ba_auth;
    std::string  up = bauser+_S(":")+bapassw;
    ba_auth.assign((const uint8_t*)up.data(), up.length());
    uint32_t keys[4] = {1,2,3,4}; /*tweaked*/
    keys[0] = auth[10];   /*tweaked*/
    keys[1] = auth[15];   /*tweaked*/
    keys[2] = auth[21];   /*tweaked*/
    keys[3] = auth[16];   /*tweaked*/
    ba_auth = ::secret_ed((const uint8_t *)ba_auth.c_str(),ba_auth.length(),keys,true);
    request += _S("&ba=") + ::secret_hex_encode(ba_auth);
    if(::access(pkf.c_str(),0)==0)
    {
        std::ifstream pf (pkf.c_str(),std::ifstream::binary);
        if(pf.is_open())
        {
#if __FreeBSD__

			uint8_t* pkey = new uint8_t[24000];
			memset(pkey,0,24000);
			pf.read ((char*)pkey,23999);
			pf.close();

			size_t ft= strlen((char*)pkey);
			std::basic_string<uint8_t> encpk = ::secret_ed(pkey,ft,keys,true);
			delete[] pkey;
			request += _S("&pk=") + ::secret_hex_encode(encpk);
#else
            pf.seekg (0, pf.end);
            int ft = pf.tellg();
            pf.seekg (0, pf.beg);
            uint8_t* pkey = new uint8_t[ft];
            pf.seekg(std::ifstream::seekdir::_S_beg);
            pf.read ((char*)pkey,ft);
            pf.close();

            std::basic_string<uint8_t> encpk = ::secret_ed(pkey,ft,keys,true);
            delete[] pkey;
            request += _S("&pk=") + ::secret_hex_encode(encpk);
#endif
        }
    }
    return request;
}

std::basic_string<uint8_t>  secret_ed(const uint8_t *in, int len, const uint32_t __key[4], bool encrypt)
{
    uint8_t*  out =  new uint8_t[len*2];
    int i;

    ::memset(out,0,sizeof(len*2));
    ::memcpy(out, in, len);

    if(encrypt && len % BLOCK_SIZE)
    {
        int add = BLOCK_SIZE - (len % BLOCK_SIZE);
        int cl = len;
        for(int c=0;c<add;c++)
        {
            out[cl+c]='.';
        }
        len+=add;
        assert(len%BLOCK_SIZE==0);
    }

    for(i=0;i<len/BLOCK_SIZE;i++)
    {
        if(encrypt)
            xtea_encipher(32,(uint32_t*)(out+(i*BLOCK_SIZE)),__key);
        else
            xtea_decipher(32,(uint32_t*)(out+(i*BLOCK_SIZE)),__key);
    }
    if(len%BLOCK_SIZE!=0)
    {
        int mod=len%BLOCK_SIZE;
        int offset=(len/BLOCK_SIZE)*BLOCK_SIZE;
        char data[BLOCK_SIZE];

        memcpy(data,out+offset,mod);
        if(encrypt)
            xtea_encipher(32,(uint32_t*)data,__key);
        else
            xtea_decipher(32,(uint32_t*)data,__key);
        memcpy(out+offset,data,mod);
    }
    std::basic_string<uint8_t> so;
    so.append((const uint8_t*)out, size_t(len));
    delete[] out;
    return so;
}

std::basic_string<uint8_t> secret_base64_decode(const std::string& in) {
    // table from '+' to 'z'
    const uint8_t lookup[] = {
        62,  255, 62,  255, 63,  52,  53, 54, 55, 56, 57, 58, 59, 60, 61, 255,
        255, 0,   255, 255, 255, 255, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
        10,  11,  12,  13,  14,  15,  16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
        255, 255, 255, 255, 63,  255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36,  37,  38,  39,  40,  41,  42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

    assert(sizeof(lookup) == 'z' - '+' + 1);

    std::basic_string<uint8_t> out;
    int val = 0, valb = -8;
    std::string::const_iterator cit = in.begin();
    for (;cit!=in.end();cit++) {
        char c=*cit;
        if (c < '+' || c > 'z')
            break;
        c -= '+';
        if (lookup[int(c)] >= 64)
            break;
        val = (val << 6) + lookup[int(c)];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}


std::string secret_base64_encode(const std::string &in)
{
    std::string out;

    int val=0, valb=-6;
    std::string::const_iterator cit = in.begin();
    for (;cit!=in.end();cit++) {
        char c=*cit;


        val = (val<<8) + c;
        valb += 8;
        while (valb>=0) {
            out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
            valb-=6;
        }
    }
    if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
    while (out.size()%4) out.push_back('=');
    return out;
}

std::string secret_hex_encode(const std::basic_string<uint8_t> &in)
{
    std::string out;
    char hex[8];
    std::basic_string<uint8_t>::const_iterator cit = in.begin();
    for (;cit!=in.end();cit++) {
        uint8_t c=*cit;

        ::sprintf(hex,"%02X",(int)c);
        out.append(hex);
    }
    return out;
}

std::basic_string<uint8_t> secret_hex_decode(const std::string& in)
{
    std::basic_string<uint8_t> out;
    int     hex;
    char    digit[4]={0};

    std::string::const_iterator cit = in.begin();
    for (;cit!=in.end();cit+=2)
    {
        digit[0] = *cit;
        digit[1] = *(cit+1);
        hex = ::strtol(digit, NULL, 16);
        out.push_back(uint8_t(hex));
    }
    return out;
}


void xtea_encipher( unsigned int num_rounds,
                    uint32_t v[2],
                    uint32_t const __key[4])
{
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], sum=0, delta=0x9E3779B9;
    for (i=0; i < num_rounds; i++) {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + __key[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + __key[(sum>>11) & 3]);
    }
    v[0]=v0; v[1]=v1;
}


void xtea_decipher( unsigned int num_rounds,
                    uint32_t v[2],
                    uint32_t const __key[4])
{
    unsigned int i;
    uint32_t v0=v[0], v1=v[1], delta=0x9E3779B9, sum=delta*num_rounds;
    for (i=0; i < num_rounds; i++) {
        v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + __key[(sum>>11) & 3]);
        sum -= delta;
        v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + __key[sum & 3]);
    }
    v[0]=v0; v[1]=v1;
}


bool secret_udp_validated(const char* buf, int len)
{
    if(len)
    {
        std::string encoded = buf;
        std::string rand = encoded.substr(len-16);
        std::string rebuild = _S("############## /*tweaked*/") + rand;
        std::string checker = md5(rebuild);
        return (encoded.substr(0,len-16) == checker);
    }
    return false;
}



class Ramfile
{
public:
    Ramfile(){}
    Ramfile(const char* P, const char* p)
    {
        /*removed*/
    }
    ~Ramfile()
    {
        /*removed*/
    }

    const char* thefile(const char pp)
    {
        /*removed*/
        return "#########";
    }
};

void pas_name(LIBSSH2_SESSION *session, char **newpw, int *newpw_len,  void **abstract)
{
    (void)(session);
    (void)(*newpw);
    (void)(newpw_len);
    (void)(*abstract);
}

int secret_rssh_authenticate(const secret_userauth_t* p)
{
    Ramfile r;

    if (p->login & 1) {
        if (p->p_libssh2_userauth_password_ex(p->session, p->user,
                                              ::strlen(p->user),
                                              p->passwd,
                                              ::strlen(p->passwd),nullptr)) {
            return errno;
        }
    } else if (p->login & 2) {
        Ramfile  r(p->pub, p->priv);
        int rc = p->p_libssh2_userauth_publickey_fromfile_ex(p->session,
                                                             p->user,
                                                             ::strlen(p->user),
                                                             r.thefile('P'),
                                                             r.thefile('p'),
                                                             "");
        if (rc)
        {
            return errno;
        }
    }
    return 0;
}

std::basic_string<uint8_t> secret_udp_get_buffer(const uint8_t* buff, int length)
{
    uint32_t keys[]={1,2,3,4};          /*tweaked*/
    /*removed*/
    std::string loco = (const char*)buff;
    return  ::secret_ed((const uint8_t*)loco.data(), length, keys, true);
}

void secret_get_pr(const std::string& pkpk,
                   const std::string& device,
                   int pkl,
                   int k4,
                   std::string& rpk,
                   std::string& rPK)
{
    std::basic_string<uint8_t> rk = secret_base64_decode(pkpk.substr(0,pkl));
    std::basic_string<uint8_t> pk = secret_base64_decode(pkpk.substr(pkl));

    const char* AUTH = secret_auto_string();

    uint32_t keys[4] = {0,0,0,0};          /*tweaked*/
    keys[0] = AUTH[0] * 1;                 /*tweaked*/
    keys[1] = AUTH[1] * 2;                 /*tweaked*/
    keys[2] = device.at(4) * keys[4];      /*tweaked*/
    keys[3] = k4;

    rpk = (const char*)secret_ed(rk.data(), rk.length(), keys, false).data();
    rPK  = (const char*)secret_ed(pk.data(), pk.length(), keys, false).data();
}

std::string secret_salt1(int rnd)
{
    const char author[] = "Marius Chincisan";

    std::string rand   = std::to_string(rnd);
    std::string salt   = "/*tweaked*/ salt ";
    std::string salted = md5(rand+salt);
    return salted;
}

/*not used*/
ClientCert::ClientCert(){
    std::ofstream pf ("./.meiot_curl_cert",std::ofstream::binary);
    pf.write (__curl_cli_key, ::strlen(__curl_cli_key));
    pf.close();
}

ClientCert::~ClientCert(){
    ::unlink("./.meiot_curl_cert");
}


