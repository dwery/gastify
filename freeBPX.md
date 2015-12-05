## possible use with [freePBX](http://www.freepbx.org/) ##

  * add 999# to the extensionlist of a ringroup
  * add this to extensions\_custom.conf:

for asterisk <= 1.2
```
      [from-internal-custom]
      exten => 999,1,Notify(${CALLERIDNUM},${CALLERIDNAME},${EXTEN}/<hostWhereGastifyIsRunningOn>)
      exten => 999,2,Dial(SIP/${EXTEN},10)
```


for asterisk >= 1.2
```
      [from-internal-custom]
      exten => 999,1,Notify(${CALLERID(num)},${CALLERID(name)},${EXTEN}/<hostWhereGastifyIsRunningOn>)
      exten => 999,2,Dial(SIP/${EXTEN},10)
```

Hint: With [app\_ldap](http://www.mezzo.net/asterisk/app_ldap.html) you can get real names from your LDAP-Server.