# reverse ssh service (client).

### HTTPS/RSA/XTEA FULLLY SECURED
##### https://www.youtube.com/watch?v=X6GtgLtpYsk

### https://mylinuz.com

##### this is the source code for the client daemon for https://www.mylinuz.com service


### Web based shh remote terminal to any linux machine behind routers and firewalls.

Uses https and xtea encryption. 
THE SSH Session is open as long the terminal web page and the parent web page are not closed but not longer than 15 minutes. 


  * Build from sources 
      * Build for PI or x86 or BBB or iMx6 or NanoPI using appropriate bash make*.sh
      * The secret_lib.a just hides the salt & peper rules and XTEA keys. Don't reverse-engineer-it. 
      * Do not try to hack into the server or access different ports othrer than the ones assigned to your ssh session. 
            * Automated scripts will block any OOO activity IP for 200 hours. 
      * Install from binaries      
          * run install.sh. This installs meiotrev as a service. Write down the device ID or cat /etc/.mylinuz.com, 
            then go to  https://mylinuz.com
  
  * How secure.
     * The reverse shell tunel is open to the server as long you keep the web shell terminal window open.
         * If there is no activity in shell for about 4 minutes the remote Linux meeiotrev service closes the ssh session. 
     * Uses [XDEA] over HTTPS 
     * The UDP you see is used to speed things up, it punch-tru the router to inform the 
      client to pool now, not by pool interval, because the user just clicked connect onthe web page.
     * Choose a strong username and password for the WEB basic authentication.
     * Choose strong password for your linux device. Dont use id_rsa key for now when installing.

     * Always close you session from the CLOSE SESSION web button. 
          * If not you have to wait about 2 minutes to close itself

      * Multi step authentication
          * mylinuz.com credentials + username/password/unique device id.
          * Then session-id (generated at runtime before opennning the ssh -R) in the access web page
          * Then your Linux machine username and password.

##### Warning !
   * All web based RSSH services as thi one can track all your typing. 
            Everyting from web to ssh session is plain TEXT at some point in the pipeline.
       * mylinuz is blocking this by securing all transport layers even on the server between NGINX and the SHELL (see the sequence diagram [io])
   * Account password is hashed and then sent over HTTPS, but anyway don't pick a dumb web password which can be looked up by: https://www.md5online.org/md5-decrypt.html  or by https://md5decrypt.net/en/
      * As well for the second step authentication choose very strong password.
      * I have all my 5 devices on this service since begining. Is much secured than your banking account. 
             To get to the shell you would need to pass these steps
       * Signin to mylinuz.com Web
       * Session number generated before connection unique on your web session
       * Username/Password of your Linux
            
#### Details of installation
   *  Do not alter the pooling time under 60 seconds.
   *  Do not change the keys seeds salt and pepper.
   *  Do not attemp to log in manually. The ssh reverse account is jailed into a non funcitonal chroot.

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

# follow the steps


# check if the service is running
```bash

:~/rssh_client$ ps ax | grep meiotrev 
20196 ?        Ssl    0:00 /usr/local/bin/meiotrev https://mylinuz.com ** **
```

# the service logs are: /var/log/meiotrev.log
# Your mylinuz credentials to access the device are in 

```bash
cat /etc/.mylinuz.com   (user, password & device ID)
 
```

    
#### access your device at:
##### https://www.mylinuz.com
#### See a video on youtube
##### https://www.youtube.com/watch?v=X6GtgLtpYsk


Credits: https://github.com/tsl0922/ttyd  for the webterm



[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=L9RVWU5NUZ4YG)   [donations are taken care by meeiot domain]



