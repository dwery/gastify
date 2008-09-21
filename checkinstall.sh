#!/bin/sh
## Freitag, September 19 2008

if [ "`which checkinstall`" = "" ]
then
	echo "You need checkinstall."
        exit
fi

VERSION=`fgrep "AM_INIT_AUTOMAKE" configure.in |gawk '{ print $2 }'`

## whoooOOOSH
./configure --prefix=/usr

checkinstall -y -D \
	--pkgversion=$VERSION \
	--pkglicense=GPL2 \
	--arch=i386 \
	--reset-uids \
	--requires=libgtk2.0-0,libglib2.0-0,libnotify1 \
	--maintainer=penschuck@gmail.com \
	--provides=gastify \
	--pkgsource=http://code.google.com/p/gastify/source/checkout \
	--pakdir=`pwd` \
	--backup=no \
	--install=no \
	--deldesc \
	--deldoc \
	--delspec

checkinstall -y -R \
        --pkgversion=$VERSION \
        --pkglicense=GPL2 \
        --arch=i386 \
	--reset-uids \
        --requires=libnotify,gtk2,glib2  \
        --maintainer=penschuck@gmail.com \
        --provides=gastify \
        --pkgsource=http://code.google.com/p/gastify/source/checkout \
        --pakdir=`pwd` \
        --backup=no \
        --install=no \
        --deldesc \
        --deldoc \
        --delspec  

make maintainer-clean
