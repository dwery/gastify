/*
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
#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libnotify/notify.h>
#include <libnotify/notification.h>
#include <gtk/gtkstatusicon.h>
#include "main.h"
#include "socket.h"
#include "gui.h"

static GtkWidget *menu;
static GtkWidget *historyDialog;
static GtkWidget *textView;
static GtkTextBuffer *buffer;
static GtkToggleAction *toggle;

GtkStatusIcon* initializeGui() {

	GtkStatusIcon *icon;
	GtkBuilder *builder;
	gchar *uifile;
	int x, y;
	gboolean push_in;

	builder = gtk_builder_new();
	uifile = g_strconcat(PACKAGE_DATA_DIR, "/gastify/gastify.ui", NULL);
	gtk_builder_add_from_file (builder, uifile, NULL);
	gtk_builder_connect_signals(builder, NULL);

	/* intialize statusicon */	
	icon = gtk_status_icon_new_from_icon_name("gastify");
	gtk_status_icon_set_tooltip(icon, _("Gastify Call Notification"));

	/* load menu */
	menu = (GtkWidget*)gtk_builder_get_object(builder, "menu1");
	toggle = (GtkToggleAction*)gtk_builder_get_object(builder, "notification-toggle");
	gtk_status_icon_position_menu((GtkMenu*)menu, &x, &y, &push_in, icon);
	g_signal_connect(icon, "popup-menu", GTK_SIGNAL_FUNC(activateMenu), NULL);
	g_signal_connect(icon, "activate", GTK_SIGNAL_FUNC(onShowHistory), icon);

	/* load history-window */
	historyDialog = (GtkWidget*)gtk_builder_get_object(builder, "window1");
	textView = (GtkWidget*)gtk_builder_get_object(builder, "textview1");
	buffer = gtk_text_buffer_new(NULL);
	g_signal_connect(historyDialog, "delete-event", GTK_SIGNAL_FUNC(gtk_widget_hide_on_delete), NULL);

	return icon;
}

/* show libnotify popup */
void notifyPopup(gchar *notifyMessage, GtkStatusIcon *icon) {
	
	NotifyNotification *notify;
	gchar *cmdoutput = NULL; 
	
	notify_init("gastify");
	notify = notify_notification_new("gastify", notifyMessage, NULL, NULL);
	notify_notification_set_timeout(notify, popuptime*1000);
	notify_notification_attach_to_status_icon(notify, icon);
	
	if ( gtk_toggle_action_get_active((GtkToggleAction*)(toggle))) {
		notify_notification_show(notify, NULL);
	}
	
	if ( execcmd ) {
		g_spawn_command_line_sync(execcmd, &cmdoutput, NULL, NULL, NULL);
		g_free(cmdoutput);
	}
	
	gtk_status_icon_set_from_icon_name(icon, "gastify_new_call");
	g_object_unref(G_OBJECT(notify));
}

/* log call to buffer */
void addToHistory(gchar *call) {

	GtkTextIter iter;
	gchar timestamp[255];
	time_t timet;
	gchar *line;

	/* get timestamp and assemble line*/
	time(&timet);
	strftime(timestamp, 64, "%a %H:%M \t", localtime(&timet));
	line = g_strconcat(timestamp, " ", call, NULL);

	/* write to GtkTextView */
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);
	gtk_text_buffer_insert(buffer, &iter, line, -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(textView), buffer);
	
	g_free(line);

}

/* open menu */
void activateMenu() {

	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());

}

/* show about */
void onShowAbout() {

	gchar *license = g_strconcat("\nGastify is free software: you can redistribute it and/or modify ", 
					"it under the terms of the GNU Lesser General Public License as published by ",
					"the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\r",
					"Gastify is distributed in the hope that it will be useful, ",
					"but WITHOUT ANY WARRANTY; without even the implied warranty of ",
					"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the ",
					"GNU General Public License for more details.\n\r",
					"You should have received a copy of the GNU Lesser General Public License ",
					"along with gastify.  If not, see <http://www.gnu.org/licenses/>.",
					NULL);

	gchar *authors[] = {"Jan Penschuck","penschuck@gmail.com", NULL};
	
	gtk_show_about_dialog(NULL,
							"name", PACKAGE,
							"version", VERSION,
							"license", license,
							"wrap-license", TRUE,
							"copyright", "\xc2\xa9 2006-2009 Jan Penschuck",
							"comments", _("a client for app-notify"),
							"logo-icon-name", "gastify",
							"website", "http://code.google.com/p/gastify/",
							"website-label", "gastify website",
							"authors", authors,
							NULL);
					
	g_free(license);

}

/* show history */
void onShowHistory(gpointer *data) {

	gtk_status_icon_set_from_icon_name(GTK_STATUS_ICON(data), "gastify");
	
	if ( !GTK_WIDGET_VISIBLE(historyDialog) ) {
		gtk_widget_show(historyDialog);
	} else {
		gtk_widget_hide(historyDialog);
	}

}

/* clear buffer */
void onClearHistory() {

	buffer = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(textView), buffer);

}

