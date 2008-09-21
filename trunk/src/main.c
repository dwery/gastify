/* Mi Jul 26 17:50:21 UTC 2006
 *
 * Copyright (C) 2006-2008 by Jan Penschuck                                                    
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

#include <config.h>
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
	
static gint port = 40000;
static gboolean daemonize = FALSE;
static gboolean version = FALSE;
gint   popuptime = 10;
gchar  *execcmd = NULL;

static GOptionEntry entries[] = 
{
	{ "port", 'p', 0, G_OPTION_ARG_INT, &port, "listen on given port", "N" },
	{ "time", 't', 0, G_OPTION_ARG_INT, &popuptime, "display popup given time", "SECONDS" },
	{ "demonize", 'd', 0, G_OPTION_ARG_NONE, &daemonize, "forks in the background", NULL },
	{ "execute", 'e', 0, G_OPTION_ARG_STRING, &execcmd, "executes command on new call", "COMMAND TO EXECUTE" },
	{ "version", 'V', 0, G_OPTION_ARG_NONE, &version, "print version info", NULL },	
	{ NULL }
};

int main(int argc, char *argv[]) {
	
	/* commandline options */
	GOptionContext *context;

	context = g_option_context_new ("");
 	g_option_context_add_main_entries(context, entries, PACKAGE);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	g_option_context_parse (context, &argc, &argv, NULL);
	
	/* print version info */		
	if (version) {
		printf("%s %s\n", PACKAGE, VERSION);
		exit(0);		
	}

	/* demonize or not */
	if (daemonize) {				
		pid_t pid, sid;
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
	
	g_option_context_free(context);
	
	/* get up and running */
	g_set_prgname(PACKAGE);
	gtk_window_set_default_icon_name(PACKAGE);
  	gtk_set_locale();
	gtk_init(&argc, &argv);
	
	/* gettext */
  	setlocale(LC_ALL, "");
  	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain(GETTEXT_PACKAGE);
	
	/* open socket and initialize statusicon */
	watchSocket(port, initializeGui());
	gtk_main();
	
	return 0;
}
