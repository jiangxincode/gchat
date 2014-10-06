#include "../common/common.h"
#include "support.h"
#include "callbacks.h"
#include "interface.h"

extern GtkWidget *gifmain;
extern GtkWidget *scrolledwindow1;
extern GtkWidget *scrolledwindow4;

extern GtkWidget *offline;
//the treeview widget used to display the contacts list for the client
extern GtkWidget *tree;
//the treeview widget used to display the offline messages
extern GtkWidget *offline_tree;
extern GtkWidget *chat_window[100];

extern GtkListStore *offline_temp_store;
extern GtkTreeIter offline_selected_iter;
extern GtkTreeModel *offline_selected_model;

extern contacts_chat_window_id_t *head;
extern struct sockaddr_in servAddr;
extern int sockfd;
extern int offline_messages_count;


void gif_receive_messages(void *server)
{
	int server_sockfd = *(int *)server;
	gifhdr_t *gifheader;
	int rcv_status;
	char *gifdata, *gifbuffer, *errormsg;

	pthread_t pthd = pthread_self();

	while(1)
	{
		gifbuffer = (char *) malloc(1024);
		rcv_status = recv(server_sockfd, gifbuffer, 1024, 0);

		if(rcv_status < 0)
		{
			errormsg = strerror(errno);
			gdk_threads_enter();
			message_dialog(GTK_MESSAGE_INFO, errormsg);
			gdk_threads_leave();
			pthread_cancel(pthd);
			return;
		}
		else if(rcv_status == 0)
		{
			contacts_chat_window_id_t *ptr;
			GtkWidget *widget;

			gdk_threads_enter();
			message_dialog(GTK_MESSAGE_INFO, "Server Disconnected");
			gdk_threads_leave();

			while(head != NULL)
			{
				ptr = head;
				head = head->next;
				free(ptr);
			}

			gdk_threads_enter();
			gtk_container_remove(GTK_CONTAINER(scrolledwindow1), tree);

			widget = lookup_widget(gifmain, "butConnect");
			gtk_widget_set_sensitive(widget, TRUE);
			widget = lookup_widget(gifmain, "mnuConnect");
			gtk_widget_set_sensitive(widget, TRUE);
			widget = lookup_widget(gifmain, "mnuDisconect");
			gtk_widget_set_sensitive(widget, FALSE);
			widget = lookup_widget(gifmain, "butAdd");
			gtk_widget_set_sensitive(widget, FALSE);
			widget = lookup_widget(gifmain, "mnuAdd");
			gtk_widget_set_sensitive(widget, FALSE);
			widget = lookup_widget(gifmain, "mnuDelete");
			gtk_widget_set_sensitive(widget, FALSE);
			widget = lookup_widget(gifmain, "butConf");
			gtk_widget_set_sensitive(widget, FALSE);
			widget = lookup_widget(gifmain, "butOffline");
			gtk_widget_set_sensitive(widget, FALSE);
			widget = lookup_widget(gifmain, "mnuOffline");
			gtk_widget_set_sensitive(widget, FALSE);
			gdk_threads_leave();

			pthread_cancel(pthd);
			return;
		}

		gifheader = (gifhdr_t *) malloc(sizeof(gifhdr_t));
		memcpy(gifheader, gifbuffer, 32);
		if((gifheader->length) > 0)
		{
			gifdata = (char *) malloc(gifheader->length);
			memcpy(gifdata, (gifbuffer + 32), gifheader->length);
		}

		switch(gifheader->type)
		{
		case 3:		// GIF_ADDRLIST_MSG
		{
			contacts_chat_window_id_t *ptr;
			user_status_t *usrs;
			int i, counter;

			gdk_threads_enter();
			GtkWidget *widget;
			GtkTreeStore *store;
			GtkTreeViewColumn *column;
			GtkCellRenderer *renderer;
			GtkTreeIter parent_iter, child_iter;
			GdkPixbuf *img;
			GtkTreeSelection *select;
			gdk_threads_leave();

			if(gifheader->reserved == 1)
			{
				// removing the tree from the container for refresing
				gdk_threads_enter();
				gtk_container_remove(GTK_CONTAINER(scrolledwindow1), tree);
				gdk_threads_leave();

				while(head != NULL)
				{
					ptr = head;
					head = head->next;
					free(ptr);
				}
			}

			gdk_threads_enter();
			widget = lookup_widget(gifmain, "butConnect");
			gtk_widget_set_sensitive(widget, FALSE);
			widget = lookup_widget(gifmain, "mnuConnect");
			gtk_widget_set_sensitive(widget, FALSE);
			widget = lookup_widget(gifmain, "mnuDisconect");
			gtk_widget_set_sensitive(widget, TRUE);
			widget = lookup_widget(gifmain, "butAdd");
			gtk_widget_set_sensitive(widget, TRUE);
			widget = lookup_widget(gifmain, "mnuAdd");
			gtk_widget_set_sensitive(widget, TRUE);
			widget = lookup_widget(gifmain, "mnuDelete");
			gtk_widget_set_sensitive(widget, TRUE);
			widget = lookup_widget(gifmain, "butConf");
			gtk_widget_set_sensitive(widget, TRUE);
			widget = lookup_widget(gifmain, "butOffline");
			gtk_widget_set_sensitive(widget, TRUE);
			widget = lookup_widget(gifmain, "mnuOffline");
			gtk_widget_set_sensitive(widget, TRUE);
			gdk_threads_leave();

			//creating a model
			gdk_threads_enter();
			store = gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
			gdk_threads_leave();

			//creating a view
			gdk_threads_enter();
			tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
			gdk_threads_leave();

			//adding the view to the scrolledwindow
			gdk_threads_enter();
			gtk_container_add(GTK_CONTAINER(scrolledwindow1), tree);
			gdk_threads_leave();

			//creating first column
			gdk_threads_enter();
			renderer = gtk_cell_renderer_pixbuf_new();
			column =
			        gtk_tree_view_column_new_with_attributes("Status",
			                        renderer,
			                        "pixbuf",
			                        STATUS_COLUMN_PIXMAP,
			                        NULL);
			gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
			gdk_threads_leave();

			//creating second column
			gdk_threads_enter();
			renderer = gtk_cell_renderer_text_new();
			column =
			        gtk_tree_view_column_new_with_attributes
			        ("Contacts", renderer, "text", CONTACTS_COLUMN_TEXT, NULL);
			gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
			gdk_threads_leave();

			//Acquire a top-level iterator
			gdk_threads_enter();
			gtk_tree_store_append(store, &parent_iter, NULL);
			if(gifheader->length != 0)
				gtk_tree_store_set(store, &parent_iter,
				                   CONTACTS_COLUMN_TEXT, "Available", -1);
			else
				gtk_tree_store_set(store, &parent_iter,
				                   CONTACTS_COLUMN_TEXT, "Not Available", -1);
			gdk_threads_leave();

			i = counter = 0;
			usrs = (user_status_t *) malloc(sizeof(user_status_t));
			while(counter < (gifheader->length))
			{
				contacts_chat_window_id_t *ptr;

				memcpy(usrs,
				       (gifdata + (i * sizeof(user_status_t))),
				       sizeof(user_status_t));

				// setting the window id for each contacts and constructing a linked list
				ptr =
				        (contacts_chat_window_id_t *)
				        malloc(sizeof(contacts_chat_window_id_t));
				strcpy(ptr->contacts_name, usrs->loginid);
				ptr->window_id = i;
				ptr->window_open = 0;
				if(head == NULL)
				{
					head = ptr;
					ptr->next = NULL;
				}
				else
				{
					ptr->next = head;
					head = ptr;
				}

				// setting the status image for online clients and offline clients
				gdk_threads_enter();
				if(usrs->status == 1)
					img = gdk_pixbuf_new_from_file("../pixmaps/ok.png", NULL);
				else
					img =
					        gdk_pixbuf_new_from_file("../pixmaps/kill.png", NULL);
				gdk_threads_leave();

				//Acquire a child iterator
				gdk_threads_enter();
				gtk_tree_store_append(store, &child_iter, &parent_iter);
				gtk_tree_store_set(store, &child_iter,
				                   CONTACTS_COLUMN_TEXT,
				                   usrs->loginid,
				                   STATUS_COLUMN_PIXMAP, img, -1);
				gdk_threads_leave();

				i++;
				counter = i * sizeof(user_status_t);
			}
			free(usrs);

			gdk_threads_enter();
			gtk_tree_view_expand_all(GTK_TREE_VIEW(tree));
			gtk_widget_show(tree);
			gdk_threads_leave();

			// signal handling for "changed" event
			gdk_threads_enter();
			select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
			gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
			g_signal_connect(G_OBJECT(select), "changed",
			                 G_CALLBACK(gif_call_client_for_chat), NULL);
			gdk_threads_leave();

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}

		case 6:		// GIF_CHAT_MSG
		{
			contacts_chat_window_id_t *ptr;
			GtkWidget *display_text;
			GtkTextBuffer *buff;
			GtkTextIter start, end;
			int line_no;

			for(ptr = head; ptr != NULL; ptr = ptr->next)
			{
				if((strcmp(ptr->contacts_name, gifheader->sender)) == 0)
					break;
				else
					continue;
			}

			if(ptr->window_open == 0)
			{
				GtkWidget *widget;
				GtkTextBuffer *buff;
				ptr->window_open = 1;	// since the window is gonna be opened

				gdk_threads_enter();
				chat_window[ptr->window_id] = create_Chat();
				widget =
				        lookup_widget(chat_window[ptr->window_id], "entInput");
				gtk_window_set_focus(GTK_WINDOW
				                     (chat_window[ptr->window_id]), widget);
				gtk_window_set_title(GTK_WINDOW
				                     (chat_window[ptr->window_id]),
				                     gifheader->sender);
				gtk_widget_show(chat_window[ptr->window_id]);
				gdk_threads_leave();

				// creating tags(foreground color) for the buffer
				gdk_threads_enter();
				widget =
				        lookup_widget(chat_window[ptr->window_id], "txtDisplay");
				buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
				gtk_text_buffer_create_tag(buff, "red_foreground",
				                           "foreground", "red", NULL);
				gtk_text_buffer_create_tag(buff, "blue_foreground",
				                           "foreground", "blue", NULL);
				gdk_threads_leave();
			}

			gdk_threads_enter();
			display_text =
			        lookup_widget(chat_window[ptr->window_id], "txtDisplay");
			buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(display_text));
			gtk_text_buffer_insert_at_cursor(buff, gifheader->sender, -1);
			gtk_text_buffer_insert_at_cursor(buff, "  :  ", -1);
			gtk_text_buffer_insert_at_cursor(buff, gifdata, -1);
			gdk_threads_leave();

			// setting the color(blue) for client's name in the display text box
			gdk_threads_enter();
			line_no = gtk_text_buffer_get_line_count(buff);
			gtk_text_buffer_get_iter_at_line(buff, &start, line_no);
			gtk_text_buffer_get_iter_at_line_offset(buff, &end,
			                                        line_no,
			                                        strlen
			                                        (gifheader->sender));
			gtk_text_buffer_apply_tag_by_name(buff,
			                                  "blue_foreground",
			                                  &start, &end);

			gtk_text_buffer_insert_at_cursor(buff, "\n", -1);
			gdk_threads_leave();

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}

		case 8:		// GIF_SUCCESS_N_ERROR_MSG
		{
			switch(gifheader->reserved)
			{
			case 101:	// GIF_ERROR_LOGIN_INCORRECT
			{
				gdk_threads_enter();
				message_dialog(GTK_MESSAGE_INFO, "Login Incorrect");
				gdk_threads_leave();

				break;
			}

			case 102:	// GIF_SUCCESS_ADD_CONTACTS
			{
				gdk_threads_enter();
				message_dialog(GTK_MESSAGE_INFO, "Your new contact has been successfully added");
				gdk_threads_leave();

				break;
			}

			case 103:	// GIF_ERROR_ADD_CONTACTS
			{
				gdk_threads_enter();
				message_dialog(GTK_MESSAGE_INFO,"The contact id you entered does not belong to a gchat user");
				gdk_threads_leave();

				break;
			}

			case 104:	// GIF_SUCCESS_DELETE_CONTACTS
			{
				gdk_threads_enter();
				message_dialog(GTK_MESSAGE_INFO,"Deleted. See the new list after you re-login");
				gdk_threads_leave();

				break;
			}

			case 105:	// GIF_ERROR_DELETE_CONTACTS_NOT_A_CONTACT
			{
				gdk_threads_enter();
				message_dialog(GTK_MESSAGE_INFO,"1. The id you entered is not in your contact list");
				gdk_threads_leave();

				break;
			}

			case 106:	// GIF_ERROR_DELETE_CONTACTS_NOT_A_MEMBER
			{
				gdk_threads_enter();
				message_dialog(GTK_MESSAGE_INFO,"2. The id you entered is not in your contact list");
				gdk_threads_leave();

				break;
			}

			}

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}

		case 10:		// GIF_OFFLINE_MESSAGE
		{
			int counter;
			char *message;
			offline_msgs_send_t *omsgs_se;

			gdk_threads_enter();
			GtkWidget *widget;
			GtkListStore *store;
			GtkTreeViewColumn *column;
			GtkCellRenderer *renderer;
			GtkTreeIter iter;
			GdkPixbuf *img;
			gdk_threads_leave();

			if(gifheader->reserved == 1)
			{
				offline_messages_count = 0;

				//creating a model
				gdk_threads_enter();
				store =
				        gtk_list_store_new(OFFLINE_COLUMNS,
				                           GDK_TYPE_PIXBUF,
				                           G_TYPE_STRING,
				                           G_TYPE_STRING, G_TYPE_STRING);
				offline_temp_store = store;
				gdk_threads_leave();

				//creating a view
				gdk_threads_enter();
				offline_tree =
				        gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
				gdk_threads_leave();

				//adding the view to the scrolledwindow
				gdk_threads_enter();
				gtk_container_add(GTK_CONTAINER(scrolledwindow4),
				                  offline_tree);
				gdk_threads_leave();

				//creating first column
				gdk_threads_enter();
				renderer = gtk_cell_renderer_pixbuf_new();
				column =
				        gtk_tree_view_column_new_with_attributes
				        ("New/Old", renderer, "pixbuf", OFFLINE_NEW_PIXMAP, NULL);
				gtk_tree_view_append_column(GTK_TREE_VIEW
				                            (offline_tree), column);
				gdk_threads_leave();

				//creating second column
				gdk_threads_enter();
				renderer = gtk_cell_renderer_text_new();
				g_object_set(G_OBJECT(renderer), "foreground", "red", NULL);
				column =
				        gtk_tree_view_column_new_with_attributes
				        ("From", renderer, "text", OFFLINE_SENDER_TEXT, NULL);
				gtk_tree_view_append_column(GTK_TREE_VIEW
				                            (offline_tree), column);
				gdk_threads_leave();

				//creating third column
				gdk_threads_enter();
				renderer = gtk_cell_renderer_text_new();
				g_object_set(G_OBJECT(renderer), "foreground",
				             "dark green", NULL);
				column =
				        gtk_tree_view_column_new_with_attributes
				        ("Date & Time", renderer, "text",
				         OFFLINE_DATESERIAL_TEXT, NULL);
				gtk_tree_view_append_column(GTK_TREE_VIEW
				                            (offline_tree), column);
				gdk_threads_leave();

				//creating fourth column
				gdk_threads_enter();
				renderer = gtk_cell_renderer_text_new();
				column =
				        gtk_tree_view_column_new_with_attributes
				        ("Message", renderer, "text", OFFLINE_MSG_TEXT, NULL);
				gtk_tree_view_append_column(GTK_TREE_VIEW
				                            (offline_tree), column);
				gdk_threads_leave();
			}

			counter = 0;
			omsgs_se =
			        (offline_msgs_send_t *) malloc(sizeof(offline_msgs_send_t));
			while(counter < (gifheader->length))
			{
				gdk_threads_enter();
				widget = lookup_widget(offline, "butOfflineDelete");
				gtk_widget_set_sensitive(widget, TRUE);
				gdk_threads_leave();

				memcpy(omsgs_se, (gifdata + counter),
				       sizeof(offline_msgs_send_t));
				counter = counter + sizeof(offline_msgs_send_t);

				message = (char *) malloc(omsgs_se->length);
				strncpy(message, (gifdata + counter), omsgs_se->length);
				counter = counter + (omsgs_se->length);

				// setting the status image for online clients and offline clients
				gdk_threads_enter();
				if(omsgs_se->new == 1)
					img = gdk_pixbuf_new_from_file("../pixmaps/ok.png", NULL);
				else
					img =
					        gdk_pixbuf_new_from_file("../pixmaps/kill.png", NULL);
				gdk_threads_leave();

				//Acquire a child iterator
				gdk_threads_enter();
				gtk_list_store_append(store, &iter);
				gtk_list_store_set(store, &iter,
				                   OFFLINE_NEW_PIXMAP, img,
				                   OFFLINE_SENDER_TEXT,
				                   omsgs_se->sender,
				                   OFFLINE_DATESERIAL_TEXT,
				                   omsgs_se->dateserial,
				                   OFFLINE_MSG_TEXT, message, -1);
				gdk_threads_leave();

				offline_messages_count++;

				free(message);
			}
			free(omsgs_se);

			gdk_threads_enter();
			gtk_widget_show(offline_tree);
			gdk_threads_leave();

			if(gifheader->reserved == 1)
			{
				GtkTreeSelection *select;

				// signal handling for "changed" event
				gdk_threads_enter();
				select =
				        gtk_tree_view_get_selection(GTK_TREE_VIEW(offline_tree));
				gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
				g_signal_connect(G_OBJECT(select), "changed",
				                 G_CALLBACK
				                 (gif_select_offline_messages), NULL);
				gdk_threads_leave();
			}

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}
		}
	}
}


GtkWidget* lookup_widget(GtkWidget *widget, const gchar *widget_name)
{
        GtkWidget *parent, *found_widget;

        for (;;)
        {
                if (GTK_IS_MENU (widget))
                        parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
                else
                        parent = widget->parent;
                if (!parent)
                        parent = gtk_object_get_data (GTK_OBJECT (widget), "GladeParentKey");
                if (parent == NULL)
                        break;
                widget = parent;
        }

        found_widget = (GtkWidget*) gtk_object_get_data (GTK_OBJECT (widget),
                        widget_name);
        if (!found_widget)
                g_warning ("Widget not found: %s", widget_name);
        return found_widget;
}

/* This is an internally used function to create pixmaps. */
GtkWidget* create_pixmap(GtkWidget *widget, const gchar *filename)
{
        GtkWidget *pixmap;
        gchar *pathname;

        if (!filename || !filename[0])
                return gtk_image_new ();

        pathname = gnome_program_locate_file (NULL, GNOME_FILE_DOMAIN_APP_PIXMAP,
                                              filename, TRUE, NULL);
        if (!pathname)
        {
                g_warning (_("Couldn't find pixmap file: %s"), filename);
                return gtk_image_new ();
        }

        pixmap = gtk_image_new_from_file (pathname);
        g_free (pathname);
        return pixmap;
}

/* This is an internally used function to create pixmaps. */

GdkPixbuf *create_pixbuf(const gchar *filename)
{
        GdkPixbuf *pixbuf;
        GError *error = NULL;
        pixbuf = gdk_pixbuf_new_from_file(filename, &error);
        if(!pixbuf)
        {
                fprintf(stderr, "%s\n", error->message);
                g_error_free(error);
        }
        return pixbuf;
}

/**
GdkPixbuf* create_pixbuf(const gchar *filename)
{
        gchar *pathname = NULL;
        GdkPixbuf *pixbuf;
        GError *error = NULL;

        if (!filename || !filename[0])
                return NULL;

        pathname = gnome_program_locate_file (NULL, GNOME_FILE_DOMAIN_APP_PIXMAP,
                                              filename, TRUE, NULL);

        if (!pathname)
        {
                g_warning (_("Couldn't find pixmap file: %s"), filename);
                return NULL;
        }

        pixbuf = gdk_pixbuf_new_from_file (pathname, &error);
        if (!pixbuf)
        {
                fprintf (stderr, "Failed to load pixbuf file: %s: %s\n",
                         pathname, error->message);
                g_error_free (error);
        }
        g_free (pathname);
        return pixbuf;
}
*/
