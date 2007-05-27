/** 
 *
 * Copyright (C) 2006 by Jan Penschuck                     
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gtk/gtkstatusicon.h>
#include "main.h"
#include "gui.h"

/* watch the socket */
gboolean watcher(GIOChannel *source, GIOCondition condition, gpointer user_data) {

	gchar *buffer;
	gsize read;
	char *cidnum, *cidname, *called;
	g_io_channel_read_line(source, &buffer, &read, NULL, NULL);
	
	if (strchr(buffer,'|')) {
			cidnum = cidname = called = NULL;
		
			cidnum = strsep(&buffer, "|");
			cidname = strsep(&buffer, "|");
			called = strsep(&buffer, "\0");

			char notifyMessage[255];		
			strcpy(notifyMessage,_("call from:\n"));
			strcat(notifyMessage,cidnum);
			strcat(notifyMessage,", ");
			strcat(notifyMessage,cidname);
			strcat(notifyMessage,"\n");
			
			notifyPopup(notifyMessage, user_data);
			addToHistory(notifyMessage);
	} 
	else {
			fprintf(stderr, _("this is not a gastify message. waiting...\n"));
	}

	g_free(buffer);
	return TRUE;
}

/* set socket up to listen mode */
void watchSocket(int port, GtkStatusIcon *icon) {

  int sd, rc;
	struct sockaddr_in servAddr;

	if ((sd=socket(AF_INET, SOCK_DGRAM, 0))<0) {
		printf(_("cannot open socket\n"));
		exit(1);
	}
  
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port = htons(port);

  	if ((rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr)))<0 || servAddr.sin_port==0) {
	  printf(_("cannot bind port %d\n"), ntohs(servAddr.sin_port));
	  exit(1);
	}
	
	GIOChannel *gchannel = g_io_channel_unix_new(sd);
	g_io_add_watch(gchannel, G_IO_IN, watcher, icon);

	printf(_("waiting for data on port UDP %i\n"), port);
}
