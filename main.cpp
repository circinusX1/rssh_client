#include <iostream>
#include <assert.h>
#include "sysinfo.h"
#include "sshrev.h"
#include "main.h"
#include "per_id.h"

/**
 * @param argv
 * WEB TOKEN NAME <PASSW>
 */
int main(int argc, char *argv[])
{
    //    0         1      2
    //    1         2      3
    //    meiorrev  acces_spass
#ifndef DEBUG
    if (getuid()) {
        std::cout << "You are not root\n";
        LOG("You are not root");
        ::exit(1);
    }
#endif
    if(argc != 2){
        std::cout << "missing url: https://mylinuz.com\n";
        exit(-1);
    }
    if(::access(CONFIG_FILE,0)==0)
    {
        sshrev rev(argv[1]);
        return 0;
    }
    std::cout << "cannot find "<<CONFIG_FILE << "\n";
    return -1;
}
