The notification will be sent from [app\_notify](http://code.google.com/p/app-notify), an [asterisk](http://www.asterisk.org/) extension. Gastify sits in the notification-area and displays a popup with the help of libnotify when a call arrives. Besides it logs all calls.

![http://gastify.googlecode.com/svn/www/gastify.gif](http://gastify.googlecode.com/svn/www/gastify.gif)

Latest release is v1.3.0. See changes [here](http://gastify.googlecode.com/svn/tags/release-1.3.0/ChangeLog).

### Features ###
  * notification for incoming calls - can be disabled
  * a click on the status-icon opens a call-log
  * can execute a command if a new call arrives
  * ability to initiate a recall or a call per script
  * lookup telephonenumbers in google and google phonebook

### Dependencies ###
  * [app\_notify](http://mezzo.net/asterisk/app_notify.html) running and properly configured on the asterisk server
  * gtk+, glade, libnotify

### Install ###
from source:
```
./configure --prefix=/usr
make
make install
```
... or use the .deb or .rpm package

### Usage ###
just run gastify in a terminal
```
gastify --help
```
displays the help
<br /><br /><br /><br />
<img src='https://stats.it-penschuck.de/piwik.php?idsite=3&rec=1' alt='' />