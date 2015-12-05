# An Example for the usage of the --call-cmd switch #

[Here](http://code.google.com/p/gastify/source/browse/trunk/extras/placecall.sh) is a bash script which uses the asterisk manager to make a call.
Add a user with the permissions to make calls in the manager.conf.
```
; a asterisk 1.6x example
[caller]
secret = secret
deny=0.0.0.0/0.0.0.0
permit=127.0.0.1/255.255.255.0
permit=192.168.1.1/255.255.255.0
read = call,originate
write = call,originate
```
Fill out the variables on top of the script, place it maybe in /usr/local/bin and use it like this:

` gastify -dc /usr/local/bin/placecall.sh `