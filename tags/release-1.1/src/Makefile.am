INCLUDES = \
  $(PACKAGE_CFLAGS)

AM_CFLAGS = \
  -Wall -g -export-dynamic `pkg-config --cflags gtk+-2.0 libglade-2.0`

gastify_LDFLAGS = \
  `pkg-config --libs gtk+-2.0 glib-2.0 libglade-2.0 libnotify`

bin_PROGRAMS = gastify

gastify_SOURCES = \
  gui.h\
  socket.c\
  socket.h\
  main.c\
  gui.c\
  main.h

gastify_LDADD = \
  $(PACKAGE_LIBS)

