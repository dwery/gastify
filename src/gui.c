/*
 * Copyright (C) 2006-2008 by Jan Penschuck
 * Copyright (C) 2009 by Alessandro Zummo
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

static GtkMenu *menu, *contextmenu;
static GtkWindow *historyDialog;
static GtkTreeView *treeView;
static GtkCheckMenuItem *toggle;
static GtkListStore *store;
static GtkStatusIcon *statusIcon;

enum {
	COL_DATE,
	COL_EXTENSION,
	COL_CID,
	COL_NAME,
	NUM_COLS
};

static void on_treeview_row_activated(GtkTreeView *treeview, GtkTreePath *path,
		GtkTreeViewColumn *col, gpointer user_data);

static void on_call_clicked(GtkButton *button,
			gpointer user_data);

/* open menu */
void on_icon_popup_menu(void)
{
	gtk_menu_popup(menu, NULL, NULL, NULL,
		NULL, 0, gtk_get_current_event_time());
}

/* show about */
void onShowAbout(void)
{
	gchar *license = g_strconcat(
		"\nGastify is free software: you can redistribute it and/or modify ", 
		"it under the terms of the GNU Lesser General Public License as published by ",
		"the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\n\r",
		"Gastify is distributed in the hope that it will be useful, ",
		"but WITHOUT ANY WARRANTY; without even the implied warranty of ",
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the ",
		"GNU General Public License for more details.\n\r",
		"You should have received a copy of the GNU Lesser General Public License ",
		"along with gastify.  If not, see <http://www.gnu.org/licenses/>.",
		NULL);

	gchar *authors[] = {
		"Jan Penschuck <penschuck@gmail.com>",
		"Alessandro Zummo <a.zummo@towertech.it>",
		NULL
	};
	
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
void on_icon_activate(void)
{	
	gchar *iconfile;
	
	if (!GTK_WIDGET_VISIBLE(historyDialog)) {
		gtk_widget_show(GTK_WIDGET(historyDialog));
		gtk_window_deiconify(historyDialog);
		iconfile = g_strconcat(PACKAGE_DATA_DIR, "/icons/hicolor/48x48/apps/gastify.png", NULL);
		gtk_status_icon_set_from_file(GTK_STATUS_ICON(statusIcon), iconfile);
	} else {
		gtk_widget_hide(GTK_WIDGET(historyDialog));
	}
}

GtkStatusIcon * initializeGui(void)
{
	GtkBuilder *builder;
	gchar *uifile;
	gchar *iconfile;	
	int x, y;
	gboolean push_in;

	builder = gtk_builder_new();
	uifile = g_strconcat(PACKAGE_DATA_DIR, "/gastify/gastify.ui", NULL);
	gtk_builder_add_from_file(builder, uifile, NULL);
	gtk_builder_connect_signals(builder, NULL);

	/* intialize statusIcon */	
	statusIcon = (GtkStatusIcon *) gtk_builder_get_object(builder, "icon");
	iconfile = g_strconcat(PACKAGE_DATA_DIR, "/icons/hicolor/48x48/apps/gastify.png", NULL);
	gtk_status_icon_set_from_file(statusIcon, iconfile);	

	/* load menus */
	menu = (GtkMenu *) gtk_builder_get_object(builder, "menu1");
	contextmenu = (GtkMenu *) gtk_builder_get_object(builder, "row-context-menu");

	toggle = (GtkCheckMenuItem *) gtk_builder_get_object(builder, "notification-toggle");

	gtk_status_icon_position_menu(menu, &x, &y, &push_in, statusIcon);

	/* load history-window */
	historyDialog = (GtkWindow *) gtk_builder_get_object(builder, "window1");
	treeView = (GtkTreeView *) gtk_builder_get_object(builder, "treeview");
	store = (GtkListStore *) gtk_builder_get_object(builder, "liststore1");
	
	if (callcmd) {
		gtk_widget_set_sensitive((GtkWidget *)
			gtk_builder_get_object(builder, "call_number"), TRUE);
			
		gtk_widget_set_sensitive((GtkWidget *)
			gtk_builder_get_object(builder, "call"), TRUE);

		g_signal_connect((GtkWidget *) gtk_builder_get_object(builder, "call"),
			"clicked", GTK_SIGNAL_FUNC(on_call_clicked),
			gtk_builder_get_object(builder, "call_number"));
	
		g_signal_connect(treeView, "row-activated", (GCallback) on_treeview_row_activated,
			gtk_builder_get_object(builder, "call_number"));
			
		gtk_widget_set_visible((GtkWidget *)gtk_builder_get_object(builder, "hbox1"), TRUE);
	}
	
	notify_init("gastify");

	return statusIcon;
}

static void on_notify_close(NotifyNotification *notification, gpointer user_data)
{
	gint reason = notify_notification_get_closed_reason(notification);
	gchar *iconfile;

	/* We get NOTIFYD_CLOSED_EXPIRED if the user clicks on the X */
	if (0 && reason == 2) { /* NOTIFYD_CLOSED_USER */
		/* reset status icon */ 
		iconfile = g_strconcat(PACKAGE_DATA_DIR, "/icons/hicolor/48x48/apps/gastify.png", NULL);
		gtk_status_icon_set_from_file(GTK_STATUS_ICON(user_data), iconfile);

	}

	g_object_unref(G_OBJECT(notification));
}

/* show libnotify popup */
static void notifyPopup(gchar *title, gchar *message, GtkStatusIcon *icon)
{
	NotifyNotification *notify;
	gchar *iconfile;

	iconfile = g_strconcat(PACKAGE_DATA_DIR, "/icons/hicolor/48x48/apps/gastify_new_call.png", NULL);
	gtk_status_icon_set_from_file(icon, iconfile);
	
	if (gtk_check_menu_item_get_active(toggle)) {

		notify = notify_notification_new(title, message, "stock_landline-phone", NULL);
		notify_notification_set_timeout(notify, popuptime * 1000);
		notify_notification_attach_to_status_icon(notify, icon);

		g_signal_connect(notify, "closed", G_CALLBACK(on_notify_close), icon);

		notify_notification_show(notify, NULL);
	}
	
	if (execcmd) {
		GError *err;
		g_spawn_command_line_async(execcmd, &err);
	}
}

static gchar *
get_selection_column(GtkTreeView *view, gint column)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(view);

	if (gtk_tree_selection_get_selected(selection, &model, &iter)) {

		gchar *data;
			    
		gtk_tree_model_get(model, &iter, column, &data, -1);

		return data;
	}

	return NULL;
}

/* log call to buffer */
void addToHistory(gchar *cid, gchar *extension, gchar *name)
{
	gchar *message;
	GtkTreeIter iter;
	gchar timestamp[20];
	time_t timet;

	/* get timestamp and assemble line*/
	time(&timet);
	strftime(timestamp, 64, "%a %H:%M:%S", localtime(&timet));

	gtk_list_store_insert(store, &iter, 0);
	gtk_list_store_set(store, &iter,
		COL_DATE, timestamp,
		COL_CID, cid,
		COL_EXTENSION, extension,
		COL_NAME, name,
		-1);

	gtk_tree_view_scroll_to_cell(treeView,
		gtk_tree_model_get_path(gtk_tree_view_get_model(treeView), &iter),
		NULL, FALSE, 0, 0);

	message = g_strconcat(timestamp, " - ", cid, "\n", _(" for "), extension, "\n", NULL);
	   
	notifyPopup(name, message, statusIcon);

	g_free(message);
}

static void do_call(const char *number)
{
	if (callcmd && strlen(number)) {

		gchar *cmd;
		GError *err;

		cmd = g_strconcat(callcmd, " ", number, NULL);

		g_spawn_command_line_async(cmd, &err);

		g_free(cmd);
	}
}

/* clear buffer */
void onClearHistory(void)
{
	gtk_list_store_clear(store);
}

static void on_call_clicked(GtkButton __attribute__((__unused__)) *button,
			gpointer user_data)
{
	do_call(gtk_entry_get_text((GtkEntry *)user_data));
}

void on_treeview_row_activated(GtkTreeView *treeview,
		GtkTreePath __attribute__((__unused__)) *path,
		GtkTreeViewColumn __attribute__((__unused__)) *col,
		gpointer user_data)
{
	gchar *data = get_selection_column(treeview, COL_CID);
	if (data) {

		gtk_entry_set_text(GTK_ENTRY(user_data), data);

		do_call(data);
					  
		g_free(data);
	}
}

void on_menu_copy_num_activate(void)
{
	gchar *data = get_selection_column(treeView, COL_CID);
	if (data) {

		gtk_clipboard_set_text(gtk_clipboard_get(GDK_NONE), data, -1);

		g_free(data);
	}
}

void on_menu_copy_name_activate(void)
{
	gchar *data = get_selection_column(treeView, COL_NAME);
	if (data) {

		gtk_clipboard_set_text(gtk_clipboard_get(GDK_NONE), data, -1);

		g_free(data);
	}
}

void on_menu_lookup_gsearch_activate(void)
{
	gchar *data, *uri;

	data = get_selection_column(treeView, COL_CID);
	if (data) {

		uri = g_strconcat("http://www.google.com/search?q=",
			data, "&client=gastify", NULL);

		gtk_show_uri(NULL, uri, GDK_CURRENT_TIME, NULL);
					  
		g_free(data);
		g_free(uri);
	}
}

void on_menu_lookup_gphone_activate(void)
{
	gchar *data, *uri;

	data = get_selection_column(treeView, COL_CID);
	if (data) {

		uri = g_strconcat("http://www.google.com/search?pb=r&q=",
			data, "&client=gastify", NULL);
			
		gtk_show_uri(NULL, uri, GDK_CURRENT_TIME, NULL);
					  
		g_free(data);
		g_free(uri);
	}
}

void on_menu_remove_activate(void)
{
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;

	selection = gtk_tree_view_get_selection(treeView);

	if (gtk_tree_selection_get_selected(selection, &model, &iter))
		gtk_list_store_remove(store, &iter);
}

static void show_context_menu(GtkTreeView *treeview, GdkEventButton *event)
{
	GtkTreeSelection *s = gtk_tree_view_get_selection(treeview);

	if (gtk_tree_selection_count_selected_rows(s) != 1)
		return;

	gtk_menu_popup(contextmenu, NULL, NULL, NULL, NULL,
		(event != NULL) ? event->button : 0,
		(event != NULL) ? gdk_event_get_time((GdkEvent * )event)
		: gtk_get_current_event_time());
}

gboolean on_treeview_popup_menu(GtkTreeView *treeview)
{
	show_context_menu(treeview, NULL);
	return TRUE;
}

gboolean on_treeview_button_press_event(GtkTreeView *treeview, GdkEventButton *event)
{
	if (event->type == GDK_BUTTON_PRESS && event->button == 3)
	{
		GtkTreeSelection *selection;
		GtkTreePath *path;

		selection = gtk_tree_view_get_selection(treeview);

		gtk_tree_selection_unselect_all(selection);

		if (gtk_tree_view_get_path_at_pos(treeview,
				(gint) event->x, 
				(gint) event->y,
				&path, NULL, NULL, NULL))
		{
			gtk_tree_selection_select_path(selection, path);
			gtk_tree_path_free(path);
		}

		show_context_menu(treeview, event);

		return TRUE;
	}
	
	return FALSE;
}

static gboolean stop_blinking(gpointer user_data)
{
	gtk_status_icon_set_blinking(GTK_STATUS_ICON(user_data), FALSE);
	return FALSE;
}

/* blink a couple of times when the user selects the
 * window's destroy button
 */
gboolean on_window1_delete_event(GtkWidget *window)
{
	gtk_widget_hide(window);
	gtk_status_icon_set_blinking(statusIcon, TRUE);
	g_timeout_add_seconds(2, &stop_blinking, statusIcon);

	return TRUE;
}
