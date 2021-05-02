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
          * run install.sh. This installs meiotrev as a service. Write down the device ID or cat /etc/.meiot_rev_cred, then got to https://rssh.mine.nu  or https://mylinuz.com
  
  * How secure.
     * The reverse shell tunel is open to the server as long you keep the web shell terminal window open.
         * If there is no activity in shell for about 4 minutes the remote Linux meeiotrev service closes the ssh session. 
     * Uses HTTPS + [XDEA]
     * UDP side traffic uses XDEA. UDP is used to speed things up therefore is not important.
     * Choose a strong username and password for the WEB basic authentication.
     * Choose strong password for your linux device. Dont use id_rsa key for now when installing.
     * Always close you session from the CLOSE SESSION web button. 
          * If not you have to wait about 2 minutes to close itself
      * Multi step authentication
          * mylinuz.com credentials + username/password/unique device id.
          * Then basic authentication (username/passwod) + session-id (generated at runtime before opennning the ssh -R)
          * Then of course your Linux machine username and password.

##### Custom Server

             * If I setup a custom server for you I can get rid of username/password and use rsa keys both ways.   


##### Warning !
   * All services like mylinuz.com can track all your typing. Everyting from web to ssh session is plain text 
at some point in the source code where the shell terminal handler is piped.
       * I blocked this by securing all transport layers even on the server between NGINX and the SHELL (see the sequence diagram [io])
   * Account password is hashed in your browser ande sent over HTTPS, I cannot see that but please don't pick a dumb web password which can be looked up by: https://www.md5online.org/md5-decrypt.html  or by https://md5decrypt.net/en/
      * As well for the second step authentication choose very strong password.
      * I have all my 5 devices on this service since begining. Is much secured than your banking account. To get to the shell you would need to pass these steps: (HTTPS)
       * Signin to mylinuz.com Web + captcha
       * Device id (40 characters long) is generated in your device only + captcha
       * Random generated session number (30 chacracters long)
       * Username of your Linux
       * Password of your Linux
            


#### Details of installation

### The code is presented for information only. 
   *  Do not alter the pooling time under 60 seconds.
   *  Do not change the keys seeds salt and pepper.
   *  Do not attemp to log in manually. The ssh reverse account is jailed into a non funcitonal chroot.


```bash
sudo apt install libssh2-1-dev
sudo apt install libcurl4-openssl-dev
sudo apt install libuid-dev   (I dont know, this might be optional)

./make.sh or ./make_arm.sh  for nano pi r-pi and bbb, (iMX not tested)
sudo ./install.sh
# follow the steps
cat /etc/.meiot_rev_cred  # to  see your device id login and web authorizarion (this are not mylinuz.com credentials).

#check if the service is running
:~/rssh_client$ ps ax | grep meiotrev 
20196 ?        Ssl    0:00 /usr/local/bin/meiotrev https://rssh.mine.nu ** **

# the service log is in: /var/log/meiotrev.log
  
```

    
#### access your device at:

##### https://www.mylinuz.com

#### See a video on youtube
##### https://www.youtube.com/watch?v=X6GtgLtpYsk


Credits: https://github.com/tsl0922/ttyd  for the webterm



[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=L9RVWU5NUZ4YG)   [donations are taken care by meeiot domain]



