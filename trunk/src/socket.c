/*
 * Copyright (C) 2006-2010 by Jan Penschuck
 *
 * This file is part of gastify.
 *
 * gastify is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * gastify is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with gastify.  If not, see <http://www.gnu.org/licenses/>.
 *
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
gboolean watcher(GIOChannel *source, GIOCondition condition,
        gpointer __attribute__((__unused__)) user_data)
{
	gchar *buffer;
	gsize readed;

	g_io_channel_read_line(source, &buffer, &readed, NULL, NULL);

	if (strchr(buffer,'|') && condition == G_IO_IN) {

		gchar *cidnum, *cidname, *called;

		cidnum = strsep(&buffer, "|");
		cidname = strsep(&buffer, "|");
		called = strsep(&buffer, "\0");

		/* remove trailing \n */
		called[strlen(called) - 1] = '\0';

		if (strip_leading)
		        cidnum++;
			
		addToHistory(cidnum, called, cidname);

		g_free(buffer);
	}
	else {
		fprintf(stderr, _("this is not a gastify message. waiting...\n"));
	}
	
	return TRUE;
}

/* set socket up to listen mode */
void watchSocket(int port, GtkStatusIcon *icon)
{
	int sd, rc;
	struct sockaddr_in servAddr;
	GIOChannel *gchannel;

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf(_("cannot open socket\n"));
		exit(1);
	}

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port = htons(port);

 	if ((rc = bind(sd, (struct sockaddr *) &servAddr,sizeof(servAddr))) < 0 || servAddr.sin_port == 0) {
 		printf(_("cannot bind port %d\n"), ntohs(servAddr.sin_port));
		exit(1);
	}

	gchannel = g_io_channel_unix_new(sd);
	g_io_add_watch(gchannel, G_IO_IN, watcher, (gpointer) icon);

	printf(_("waiting for data on port UDP %i\n"), port);
}

