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

#include <string.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glade/glade-xml.h>
#include <libnotify/notify.h>
#include <libnotify/notification.h>
#include <gtk/gtkstatusicon.h>
#include "main.h"
#include "socket.h"
#include "gui.h"

#define MAX_MSG 1024
#define VERSION "1.1"
#define APPNAME "gastify"

static GtkWidget *menu;
static GtkWidget *historyDialog;
static GtkWidget *textView;
static GtkTextBuffer *buffer;

GtkStatusIcon* initializeGui() {

	/* initialize statusicon */
	GtkStatusIcon *icon;
	icon = gtk_status_icon_new_from_icon_name("gastify");
	gtk_status_icon_set_tooltip(icon, _("gastify call notification"));
	
	/* load glade stuff */	
	GladeXML *xml = glade_xml_new("/usr/share/gastify/gastify.glade", "menu1", "gastify");
	glade_xml_signal_autoconnect(xml);
	menu = glade_xml_get_widget(xml, "menu1");
	
	xml = glade_xml_new("/usr/share/gastify/gastify.glade", "window1", "gastify");
	glade_xml_signal_autoconnect(xml);
	historyDialog = glade_xml_get_widget(xml, "window1");
	g_signal_connect(historyDialog, "delete-event", GTK_SIGNAL_FUNC(gtk_widget_hide_on_delete), NULL);
	textView = glade_xml_get_widget(xml, "textview1");
	
	buffer = gtk_text_buffer_new(NULL);
	
	int x, y;
	gboolean push_in;
	x=y=0;

	gtk_status_icon_position_menu((GtkMenu*)menu, &x, &y, &push_in, icon);
	g_signal_connect(icon, "popup-menu", GTK_SIGNAL_FUNC(activateMenu), NULL);
	g_signal_connect(icon, "activate", GTK_SIGNAL_FUNC(onShowHistory), NULL);
	
	return icon;
}

void notifyPopup(char *notifyMessage, GtkStatusIcon *icon) {
	NotifyNotification *notify;
	notify_init("gastify");
	notify = notify_notification_new("gastify", notifyMessage, NULL, NULL);
	notify_notification_set_timeout(notify, popuptime*1000);
	notify_notification_attach_to_status_icon(notify, icon);		
	notify_notification_show(notify, NULL);
	g_object_unref(G_OBJECT(notify));
}

/* log call to buffer */
void addToHistory(gchar *call) {
	GtkTextIter iter;
	char timestamp[255];
	time_t timet;
	char line[255];

	/* get timestamp */
	time( &timet );
	strftime(timestamp, 64, "%X", localtime(&timet));
	
	/* assemble line */
	strcpy(line, timestamp);
	strcat(line," - ");
	char *ptr;
	ptr = strchr(call, '\n');
	*ptr = ' ';
	strcat(line, call);

	gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1);
	gtk_text_buffer_insert(buffer, &iter, line, -1);
	gtk_text_view_set_buffer((GtkTextView*)textView, buffer);
}

/* open menu */
void activateMenu() {
	gtk_menu_popup((GtkMenu*)menu, NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
}

/* show about */
void onShowAbout(GtkWidget *widget, gpointer user_data) {
	static const char *authors[] = {"Jan Penschuck","penschuck@gmail.com", NULL};
	gtk_show_about_dialog(NULL,
							"name", APPNAME,
							"version", VERSION,
							"license", "GNU General Public License 2",
							"copyright", "\xc2\xa9 2006 Jan Penschuck",
							"comments", _("a client for app-notify"),
							"logo-icon-name", "gastify",
							"website", "http://gastify.googlepages.com",
							"authors", authors,
							NULL);
}

/* show history */
void onShowHistory() {	
	if ( !GTK_WIDGET_VISIBLE(historyDialog) ) {
		gtk_widget_show(historyDialog);		
	} else {
		gtk_widget_hide(historyDialog);
	}
}

/* clear buffer */
void onClearHistory() {
	buffer = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer((GtkTextView*)textView, buffer);
}
