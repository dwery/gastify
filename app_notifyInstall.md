# Installation #

Download app\_notify [here](http://mezzo.net/asterisk/app_notify.html). Unpack.

**< asterisk 1.6**

Install it.
```
make
make install
```
**>= asterisk 1.6**

Open app\_notify.c in a text editor and add:
```
#include "asterisk.h"
```
right below this stanza:
```
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
```
Install it.
```
make
make install
```