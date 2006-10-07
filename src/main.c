/* Mi Jul 26 17:50:21 UTC 2006 */

/***************************************************************************
 * Copyright (C) 2006 by Jan Penschuck                                                    
 *
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "socket.h"
#include "gui.h"

#define MAX_MSG 1024
#define VERSION "1.1"
#define APPNAME "gastify"
	
static gint port = 40000;
static gboolean daemonize = FALSE;
static gboolean version = FALSE;
gint popuptime = 10;

static GOptionEntry entries[] = 
{
	{ "port", 'p', 0, G_OPTION_ARG_INT, &port, "listen on given port", "N" },
	{ "time", 't', 0, G_OPTION_ARG_INT, &popuptime, "display popup given time", "SECONDS" },
	{ "demonize", 'd', 0, G_OPTION_ARG_NONE, &daemonize, "forks in the background", NULL },
	{ "version", 'V', 0, G_OPTION_ARG_NONE, &version, "print version info", NULL },
	{ NULL }
};

int main(int argc, char *argv[]) {
	
	/* commandline options */
	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new ("");
 	g_option_context_add_main_entries(context, entries, "gastify");
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	g_option_context_parse (context, &argc, &argv, &error);
	
	/* print version info */		
	if (version) {
		printf("%s %s\n", APPNAME, VERSION);
		exit(0);		
	}

	/* demonize or not */
	if (daemonize) {				
		pid_t   pid, sid;
		pid = fork();
	
		if (pid < 0) {
			exit(EXIT_FAILURE);
		} 
		else if (pid > 0) {
			exit(EXIT_SUCCESS);
		}

		umask(0);
		sid = setsid();
		if (sid < 0) {
			exit(EXIT_FAILURE);
		}						
	}	
	
	/* get up and running */
	g_set_prgname("gastify");
	gtk_window_set_default_icon_name("gastify");
    gtk_set_locale();
	gtk_init(&argc, &argv);
	
	/* gettext */
    setlocale (LC_ALL, "");
    bindtextdomain("gastify", "/usr/share/locale");
	textdomain("gastify");
	
	/* open socket and initialize statusicon */
	watchSocket(port, initializeGui());
	gtk_main ();
	
	return 0;
}
