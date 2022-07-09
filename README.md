# reverse ssh service (client).

### HTTPS/RSA/XTEA FULLLY SECURED
##### https://www.youtube.com/watch?v=X6GtgLtpYsk

### https://mylinuz.com

##### this is the source code for the client service for: https://www.mylinuz.com service


### Web based shh remote terminal to any linux machine behind routers and firewalls.

  * Build from sources 
      * Build for PI or x86 or BBB or iMx6 or NanoPI using appropriate bash make*.sh
      * The secret_lib.a just hides the salt & peper rules and XTEA keys.  
          * The XTEA was there during tests when I did not use https. Is useless now.

   * Install from binaries      
      * run install.sh. This installs mylinuz as a service. Write down the device ID or cat /etc/.mylinuz.com, 
        then go to  https://mylinuz.com and log on.
      * Check /var/log/meeiot*    or something similar for the log files.
         * BUG!!!  delete that file periodically. Fix in progress. 
      * If you have multiple devices use the same username and password.  
  
  * Sessions
     * The reverse shell tunel is open to the server as long you keep the web shell terminal window open.
         *  A connection stays alive maximum 10 minutes (if there is shell traffic).  
         *  If there is no activity the service mylinuz.com will dropp  the connection in 1 minute.
---

  * How secure.
     * Uses [XDEA] , useless, now uses https.
     * Over HTTPS, as secure as is the HTTPS.
          *  The UDP side socket you see is used to speed things up, it punch-tru the router to inform the 
             client to pool now, not by pool interval, due to; the user just clicked connect onthe web page.  (is disabled)
     * Choose a strong username and passwordwhen you install the serive on your Linux running the install script.
     * Choose strong password for your linux device. Linux/Device users as: pi, ubuntu, debian, user, root, admin, remote, are rejected.
     * Do not use id_rsa key authentication when installing.
     * Always close you session from the CLOSE SESSION web button after typing exit in the shell. 
          * If not you have to wait about 1 minutes to close itself
---

  * Multi step authentication
     * https: mylinuz.com credentials + username/password/unique device id.
     * Then session-id (generated at runtime before opennning the ssh -R) in the access web page
     * Then your Linux machine username and password.

##### How does it work
     * Preety much like this:
     * https://www.howtogeek.com/428413/what-is-reverse-ssh-tunneling-and-how-to-use-it/
           
#### Details of installation
   *  Do not alter the pooling time under 60 seconds.
   *  Do not change the keys seeds salt and pepper.
   *  Do not attemp to log in manually. The ssh reverse account is jailed into a non funcitonal chroot, and you risk IP banning.

## INSTALL 

```bash
sudo ./install.sh

```

### If ./install fails rebuild the binary on th machine as

```bash
sudo apt install libssh2-1-dev
sudo apt install libcurl4-openssl-dev
sudo apt install libuid-dev   (I dont know, this might be optional)
./make.sh or ./make_arm.sh  for nano pi r-pi and bbb, (iMX not tested)
sudo ./install.sh
```

##### check if the service is running
```bash

:~/rssh_client$ ps ax | grep mylinuz  # could by meeiot, I chnaged the names back and forward coule of times !!! 
20196 ?        Ssl    0:00 /usr/local/bin/meiotrev https://mylinuz.com ** **
```

##### the service logs are: /var/log/meiotrev.log
##### Your mylinuz credentials to access the device are in 

```bash
cat /etc/.mylinuz.com   (user, password & device ID)
 
```

##### Warning !
   * Don't pick a dumb device password which can be looked up by: https://www.md5online.org/md5-decrypt.html  or by https://md5decrypt.net/en/
   * At some point in the webterm-handler process at mylinuz.com server, what you type is in plain text. This is when data passed from https to rev-ssh. 
If the coder want to log that information it simply  dumps that with printf() in a file, therfore all your typing can be traced. I am just
saying that to be aware of any other similar services can track all your all your typing. In the case of mylinuz.con you just have to trust the
service. Anyway, you have logs on your device and you can track any other access out of your own, and if you find unauthorized access, delete your accounts. If you want your own server with this service installed, open an issue, and we can take it from there. Setting up a new server is not free.


##### See a video on youtube
##### https://www.youtube.com/watch?v=X6GtgLtpYsk


Credits: https://github.com/tsl0922/ttyd  for the webterm



[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=L9RVWU5NUZ4YG)   [donations are taken care by meeiot domain]


    
## If you want your private customised server please Open an issue, and we can take it from there.

