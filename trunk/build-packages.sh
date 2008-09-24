#!/bin/sh
## Freitag, September 19 2008

if [ "`which checkinstall`" = "" ]
then
	echo "You need checkinstall."
        exit
fi

VERSION=`fgrep "AM_INIT_AUTOMAKE" configure.in |gawk '{ print $2 }'`
CURRENT_DIR=`pwd`

## whoooOOOSH
## a .tar.gz
make maintainer-clean
cd ..
tar --exclude=.svn -zcf gastify-$VERSION.tar.gz gastify/
mv gastify-$VERSION.tar.gz $CURRENT_DIR
cd $CURRENT_DIR

./configure --prefix=/usr
## a .deb
checkinstall -y -D \
	--pkgname=gastify \
	--pkgversion=$VERSION \
	--pkglicense=LGPL3 \
	--arch=i386 \
	--reset-uids \
	--requires=libgtk2.0-0,libglib2.0-0,libnotify1 \
	--maintainer=penschuck@gmail.com \
	--provides=gastify \
	--pkgsource=http://code.google.com/p/gastify/source/checkout \
	--pakdir=$CURRENT_DIR \
	--backup=no \
	--install=no \
	--fstrans \
	--deldesc \
	--deldoc \
	--delspec

## a .rpm
checkinstall -y -R \
	--pkgname=gastify \
        --pkgversion=$VERSION \
        --pkglicense=LGPL3 \
        --arch=i386 \
	--reset-uids \
        --requires=libnotify,gtk2,glib2  \
        --maintainer=penschuck@gmail.com \
        --provides=gastify \
        --pkgsource=http://code.google.com/p/gastify/source/checkout \
        --pakdir=$CURRENT_DIR \
        --backup=no \
        --install=no \
        --fstrans \
        --deldesc \
        --deldoc \
        --delspec  

make maintainer-clean