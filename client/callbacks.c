#include "main.h"
#include "support.h"
#include "callbacks.h"
#include "interface.h"
#include "gif_defns.h"

void gif_receive_messages(int);	// actually the messages are received from the server
static void gif_call_client_for_chat(GtkTreeSelection * selection,
                                     gpointer data);
static void gif_select_offline_messages(GtkTreeSelection * selection,
                                        gpointer data);

extern GtkWidget *gifmain;
extern GtkWidget *scrolledwindow1;
extern GtkWidget *scrolledwindow4;

GtkWidget *authen;
GtkWidget *offline;
//the treeview widget used to display the contacts list for the client
GtkWidget *tree;
//the treeview widget used to display the offline messages
GtkWidget *offline_tree;
GtkWidget *chat_window[100];

GtkListStore *offline_temp_store;
GtkTreeIter offline_selected_iter;
GtkTreeModel *offline_selected_model;

contacts_chat_window_id_t *head;
struct sockaddr_in servAddr;
int sockfd;
char client_loginid[25];
int offline_messages_count;

enum
{
	CONTACTS_COLUMN_TEXT,
	STATUS_COLUMN_PIXMAP
};

enum
{
	OFFLINE_NEW_PIXMAP,
	OFFLINE_SENDER_TEXT,
	OFFLINE_DATESERIAL_TEXT,
	OFFLINE_MSG_TEXT,
	OFFLINE_COLUMNS
};

void
on_mnuConnect_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	head = NULL;
	authen = create_Authen();
	gtk_widget_show(authen);
}


void
on_mnuDisconect_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *widget;
	contacts_chat_window_id_t *ptr;
	char *gifbufferS, *errormsg;
	gifhdr_t *gifheaderS;
	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
	gifheaderS->type = GIF_DISCONNECT_MSG;
	strcpy(gifheaderS->sender, client_loginid);
	strcpy(gifheaderS->receiver, "server");
	gifheaderS->length = 0;
	gifheaderS->reserved = 0;

	gifbufferS = (char *) malloc(sizeof(gifhdr_t));
	memcpy(gifbufferS, gifheaderS, sizeof(gifhdr_t));
	if((send(sockfd, gifbufferS, 32, 0)) < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}

	free(gifbufferS);
	free(gifheaderS);

	// closing all the currently opened windows
	for(ptr = head; ptr != NULL; ptr = ptr->next)
	{
		if(ptr->window_open == 1)
			gtk_widget_destroy(chat_window[ptr->window_id]);
	}

	// freeing the allocated spaces for linked list
	while(head != NULL)
	{
		ptr = head;
		head = head->next;
		free(ptr);
	}

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

	close(sockfd);
}


void
on_mnuQuit_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_main_quit();
}


void
on_mnuAbout_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_widget_show(create_About());
}


void
on_butConnect_clicked(GtkButton * button, gpointer user_data)
{
	head = NULL;
	authen = create_Authen();
	gtk_widget_show(authen);
}


void
on_butAdd_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_show(create_AddContacts());
}


void
on_butConf_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_show(create_msgbox("info", "Under Construction"));
}


void
on_butOffline_clicked(GtkButton * button, gpointer user_data)
{
	gifhdr_t *gifheaderS;
	char *gifbufferS, *errormsg;

	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
	gifbufferS = (char *) malloc(sizeof(gifhdr_t));
	gifheaderS->type = GIF_OFFLINE_REQUEST_MSG;
	gifheaderS->length = 0;
	strcpy(gifheaderS->sender, client_loginid);
	strcpy(gifheaderS->receiver, "server");
	gifheaderS->reserved = 0;

	memcpy(gifbufferS, gifheaderS, 32);

	if((send(sockfd, gifbufferS, 32, 0)) < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}

	free(gifbufferS);
	free(gifheaderS);

	offline = create_Offline();
	gtk_widget_show(offline);
}


void
on_butCancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(authen);
}


void
on_butClear_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *loginid = lookup_widget(GTK_WIDGET(button), "entUserid");
	GtkWidget *password = lookup_widget(GTK_WIDGET(button), "entPass");
	GtkWidget *servip = lookup_widget(GTK_WIDGET(button), "entServip");

	gtk_entry_set_text(GTK_ENTRY(loginid), "");
	gtk_entry_set_text(GTK_ENTRY(password), "");
	gtk_entry_set_text(GTK_ENTRY(servip), "");
}


void
on_butOk_clicked(GtkButton * button, gpointer user_data)
{
	pthread_t pthd;
	gifhdr_t *gifheaderS;
	char *gifdataS, *gifbufferS, *errormsg;
	GtkWidget *loginid = lookup_widget(GTK_WIDGET(button), "entUserid");
	GtkWidget *password = lookup_widget(GTK_WIDGET(button), "entPass");
	GtkWidget *servip = lookup_widget(GTK_WIDGET(button), "entServip");

	// initialising the client's loginid(global variable) to be used somewhere
	strcpy(client_loginid, gtk_entry_get_text(GTK_ENTRY(loginid)));

	//initialising the server details
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, gtk_entry_get_text(GTK_ENTRY(servip)),
	          &servAddr.sin_addr);

	//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
		return;
	}
	//connect to the server
	if((connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)))
	                < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
		return;
	}

	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
	gifheaderS->type = GIF_LOGIN_MSG;
	strcpy(gifheaderS->sender, gtk_entry_get_text(GTK_ENTRY(loginid)));
	strcpy(gifheaderS->receiver, "server");
	gifheaderS->reserved = 0;

	gifdataS =
	        (char *) malloc(strlen(gtk_entry_get_text(GTK_ENTRY(loginid))) +
	                        strlen(gtk_entry_get_text(GTK_ENTRY(password))) + 5);
	strcpy(gifdataS, gtk_entry_get_text(GTK_ENTRY(loginid)));
	strcat(gifdataS, "#*&");
	strcat(gifdataS, gtk_entry_get_text(GTK_ENTRY(password)));
	gifheaderS->length = strlen(gifdataS) + 1;

	gifbufferS = (char *) malloc(32 + gifheaderS->length);
	memcpy(gifbufferS, gifheaderS, 32);
	memcpy((gifbufferS + 32), gifdataS, gifheaderS->length);

	if((send(sockfd, gifbufferS, (32 + gifheaderS->length), 0)) < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}

	free(gifheaderS);
	free(gifdataS);
	free(gifbufferS);

	if(pthread_create
	                (&pthd, NULL, (void *) gif_receive_messages, (void *) &sockfd) != 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}
	pthread_detach(pthd);

	gtk_widget_destroy(authen);
}

void
gif_receive_messages(server_sockfd)
int server_sockfd;
{
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
			gtk_widget_show(create_msgbox("error", errormsg));
			gdk_threads_leave();
			pthread_cancel(pthd);
			return;
		}
		else if(rcv_status == 0)
		{
			contacts_chat_window_id_t *ptr;
			GtkWidget *widget;

			gdk_threads_enter();
			gtk_widget_show(create_msgbox("error", "Server Disconnected"));
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
				gtk_widget_show(create_msgbox
				                ("error", "Login Incorrect"));
				gdk_threads_leave();

				break;
			}

			case 102:	// GIF_SUCCESS_ADD_CONTACTS
			{
				gdk_threads_enter();
				gtk_widget_show(create_msgbox
				                ("info",
				                 "Your new contact has been successfully added"));
				gdk_threads_leave();

				break;
			}

			case 103:	// GIF_ERROR_ADD_CONTACTS
			{
				gdk_threads_enter();
				gtk_widget_show(create_msgbox
				                ("error",
				                 "The contact id you entered does not belong to a gchat user"));
				gdk_threads_leave();

				break;
			}

			case 104:	// GIF_SUCCESS_DELETE_CONTACTS
			{
				gdk_threads_enter();
				gtk_widget_show(create_msgbox
				                ("info",
				                 "Deleted. See the new list after you re-login"));
				gdk_threads_leave();

				break;
			}

			case 105:	// GIF_ERROR_DELETE_CONTACTS_NOT_A_CONTACT
			{
				gdk_threads_enter();
				gtk_widget_show(create_msgbox
				                ("error",
				                 "1. The id you entered is not in your contact list"));
				gdk_threads_leave();

				break;
			}

			case 106:	// GIF_ERROR_DELETE_CONTACTS_NOT_A_MEMBER
			{
				gdk_threads_enter();
				gtk_widget_show(create_msgbox
				                ("error",
				                 "2. The id you entered is not in your contact list"));
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

static void
gif_select_offline_messages(GtkTreeSelection * selection, gpointer data)
{
	if(gtk_tree_selection_get_selected
	                (selection, &offline_selected_model, &offline_selected_iter))
	{
		//gtk_list_store_remove(offline_temp_store, &iter);
		//gtk_tree_model_get (model, &iter, OFFLINE_MSG_TEXT, &offline_selected_msg, -1);
		//g_print(offline_selected_msg);
	}
}


static void
gif_call_client_for_chat(GtkTreeSelection * selection, gpointer data)
{
	contacts_chat_window_id_t *ptr;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *selected_name;

	if(gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_model_get(model, &iter, CONTACTS_COLUMN_TEXT,
		                   &selected_name, -1);

		if((strcmp(selected_name, "Available")) != 0
		                && (strcmp(selected_name, "Not Available")) != 0)
		{
			for(ptr = head; ptr != NULL; ptr = ptr->next)
			{
				if((strcmp(ptr->contacts_name, selected_name)) == 0)
					break;
				else
					continue;
			}

			if(ptr->window_open == 0)
			{
				GtkWidget *widget;
				GtkTextBuffer *buff;
				ptr->window_open = 1;	// since the window is gonna be opened
				chat_window[ptr->window_id] = create_Chat();
				widget =
				        lookup_widget(chat_window[ptr->window_id], "entInput");
				gtk_window_set_focus(GTK_WINDOW
				                     (chat_window[ptr->window_id]), widget);
				gtk_window_set_title(GTK_WINDOW
				                     (chat_window[ptr->window_id]),
				                     selected_name);
				gtk_widget_show(chat_window[ptr->window_id]);

				// creating tags(foreground color) for the buffer
				widget =
				        lookup_widget(chat_window[ptr->window_id], "txtDisplay");
				buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
				gtk_text_buffer_create_tag(buff, "red_foreground",
				                           "foreground", "red", NULL);
				gtk_text_buffer_create_tag(buff, "blue_foreground",
				                           "foreground", "blue", NULL);
			}
		}
		g_free(selected_name);
	}
}


void
on_butSend_clicked(GtkButton * button, gpointer user_data)
{
	gifhdr_t *gifheaderS;
	char title[20];
	char *gifdataS, *gifbufferS, *errormsg;
	GtkWidget *window, *input_text, *display_text;
	GtkTextBuffer *buff;
	GtkTextIter start, end;
	int line_no;

	window = lookup_widget(GTK_WIDGET(button), "Chat");
	input_text = lookup_widget(GTK_WIDGET(button), "entInput");
	display_text = lookup_widget(GTK_WIDGET(button), "txtDisplay");

	//title = gtk_object_get_data(GTK_OBJECT(window), "data");
	strcpy(title, gtk_window_get_title(GTK_WINDOW(window)));

	buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(display_text));

	gtk_text_buffer_insert_at_cursor(buff, client_loginid, -1);
	gtk_text_buffer_insert_at_cursor(buff, "  :  ", -1);
	gtk_text_buffer_insert_at_cursor(buff,
	                                 gtk_entry_get_text(GTK_ENTRY
	                                                 (input_text)), -1);

	// setting the color(red) for client's name in the display text box
	line_no = gtk_text_buffer_get_line_count(buff);
	gtk_text_buffer_get_iter_at_line(buff, &start, line_no);
	gtk_text_buffer_get_iter_at_line_offset(buff, &end, line_no,
	                                        strlen(client_loginid));
	gtk_text_buffer_apply_tag_by_name(buff, "red_foreground", &start, &end);

	gtk_text_buffer_insert_at_cursor(buff, "\n", -1);

	// the coding for passing the message to the other client through the server
	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
	gifheaderS->type = GIF_CHAT_MSG;
	strcpy(gifheaderS->sender, client_loginid);
	strcpy(gifheaderS->receiver, title);
	gifheaderS->reserved = 0;

	gifdataS =
	        (char *) malloc(strlen(gtk_entry_get_text(GTK_ENTRY(input_text))));
	strcpy(gifdataS, gtk_entry_get_text(GTK_ENTRY(input_text)));
	gifheaderS->length = strlen(gifdataS) + 1;

	gifbufferS = (char *) malloc(32 + gifheaderS->length);
	memcpy(gifbufferS, gifheaderS, 32);
	memcpy((gifbufferS + 32), gifdataS, gifheaderS->length);

	if((send(sockfd, gifbufferS, (32 + gifheaderS->length), 0)) < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}
	// clearing the input entry box
	gtk_entry_set_text(GTK_ENTRY(input_text), "");
	gtk_window_set_focus(GTK_WINDOW(window), input_text);

	free(gifbufferS);
	free(gifdataS);
	free(gifheaderS);
}


void
on_Chat_destroy(GtkObject * object, gpointer user_data)
{
	contacts_chat_window_id_t *ptr;
	GtkWidget *window;
	char title[20];

	window = lookup_widget(GTK_WIDGET(object), "Chat");
	strcpy(title, gtk_window_get_title(GTK_WINDOW(window)));

	for(ptr = head; ptr != NULL; ptr = ptr->next)
	{
		if((strcmp(ptr->contacts_name, title)) == 0)
			break;
		else
			continue;
	}

	// setting the chat window state to 'close'
	ptr->window_open = 0;
}


void
on_entInput_activate(GtkEntry * entry, gpointer user_data)
{
	gifhdr_t *gifheaderS;
	char title[20];
	char *gifdataS, *gifbufferS, *errormsg;
	GtkWidget *window, *input_text, *display_text;
	GtkTextBuffer *buff;
	GtkTextIter start, end;
	int line_no;

	window = lookup_widget(GTK_WIDGET(entry), "Chat");
	input_text = lookup_widget(GTK_WIDGET(entry), "entInput");
	display_text = lookup_widget(GTK_WIDGET(entry), "txtDisplay");

	//title = gtk_object_get_data(GTK_OBJECT(window), "data");
	strcpy(title, gtk_window_get_title(GTK_WINDOW(window)));

	buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(display_text));

	gtk_text_buffer_insert_at_cursor(buff, client_loginid, -1);
	gtk_text_buffer_insert_at_cursor(buff, "  :  ", -1);
	gtk_text_buffer_insert_at_cursor(buff,
	                                 gtk_entry_get_text(GTK_ENTRY
	                                                 (input_text)), -1);

	// setting the color for client's name in the display text box
	line_no = gtk_text_buffer_get_line_count(buff);
	gtk_text_buffer_get_iter_at_line(buff, &start, line_no);
	gtk_text_buffer_get_iter_at_line_offset(buff, &end, line_no,
	                                        strlen(client_loginid));
	gtk_text_buffer_apply_tag_by_name(buff, "red_foreground", &start, &end);

	gtk_text_buffer_insert_at_cursor(buff, "\n", -1);

	// the coding for passing the message to the other client through the server
	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
	gifheaderS->type = GIF_CHAT_MSG;
	strcpy(gifheaderS->sender, client_loginid);
	strcpy(gifheaderS->receiver, title);
	gifheaderS->reserved = 0;

	gifdataS =
	        (char *) malloc(strlen(gtk_entry_get_text(GTK_ENTRY(input_text))));
	strcpy(gifdataS, gtk_entry_get_text(GTK_ENTRY(input_text)));
	gifheaderS->length = strlen(gifdataS) + 1;

	gifbufferS = (char *) malloc(32 + gifheaderS->length);
	memcpy(gifbufferS, gifheaderS, 32);
	memcpy((gifbufferS + 32), gifdataS, gifheaderS->length);

	if((send(sockfd, gifbufferS, (32 + gifheaderS->length), 0)) < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}
	// clearing the input entry box
	gtk_entry_set_text(GTK_ENTRY(input_text), "");

	free(gifbufferS);
	free(gifdataS);
	free(gifheaderS);
}


void
on_butAddContactsOk_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *widget, *contactsid;
	widget = lookup_widget(GTK_WIDGET(button), "AddContacts");
	contactsid = lookup_widget(GTK_WIDGET(button), "entNewContactsid");
	gifhdr_t *gifheaderS;
	char *gifdataS, *gifbufferS, *errormsg;

	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));

	if(strcmp
	                (gtk_window_get_title(GTK_WINDOW(widget)),
	                 "gchat - Add contacts") == 0)
		gifheaderS->type = GIF_ADD_CONTACTS_MSG;
	else
		gifheaderS->type = GIF_DELETE_CONTACTS_MSG;

	strcpy(gifheaderS->sender, client_loginid);
	strcpy(gifheaderS->receiver, "server");
	gifheaderS->reserved = 0;

	gifdataS =
	        (char *) malloc(strlen(gtk_entry_get_text(GTK_ENTRY(contactsid))));
	strcpy(gifdataS, gtk_entry_get_text(GTK_ENTRY(contactsid)));
	gifheaderS->length = strlen(gifdataS) + 1;

	gifbufferS = (char *) malloc(32 + gifheaderS->length);
	memcpy(gifbufferS, gifheaderS, 32);
	memcpy((gifbufferS + 32), gifdataS, gifheaderS->length);

	if((send(sockfd, gifbufferS, (32 + gifheaderS->length), 0)) < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}

	free(gifheaderS);
	free(gifdataS);
	free(gifbufferS);

	gtk_widget_destroy(widget);
}


void
on_butAddContactsCancel_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *widget;
	widget = lookup_widget(GTK_WIDGET(button), "AddContacts");
	gtk_widget_destroy(widget);
}


void
on_butOfflineDelete_clicked(GtkButton * button, gpointer user_data)
{
	gifhdr_t *gifheaderS;
	char *gifdataS, *gifbufferS, *errormsg;
	gchar *offline_selected_msg;

	gtk_tree_model_get(offline_selected_model, &offline_selected_iter,
	                   OFFLINE_MSG_TEXT, &offline_selected_msg, -1);

	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
	gifheaderS->type = GIF_OFFLINE_DELETE_MSG;
	strcpy(gifheaderS->sender, client_loginid);
	strcpy(gifheaderS->receiver, "server");
	gifheaderS->reserved = 0;

	gifdataS = (char *) malloc(strlen(offline_selected_msg));
	strcpy(gifdataS, offline_selected_msg);
	gifheaderS->length = strlen(gifdataS) + 1;

	gifbufferS = (char *) malloc(32 + gifheaderS->length);
	memcpy(gifbufferS, gifheaderS, 32);
	memcpy((gifbufferS + 32), gifdataS, gifheaderS->length);

	if((send(sockfd, gifbufferS, (32 + gifheaderS->length), 0)) < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}

	free(gifheaderS);
	free(gifdataS);
	free(gifbufferS);

	gtk_list_store_remove(offline_temp_store, &offline_selected_iter);
	offline_messages_count--;
	if(offline_messages_count == 0)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
	}
}


void
on_butOfflineClose_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(offline);
}


void
on_mnuAdd_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_widget_show(create_AddContacts());
}


void
on_mnuDelete_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *widget;
	widget = create_AddContacts();
	gtk_widget_show(widget);
	gtk_window_set_title(GTK_WINDOW(widget), "gchat - Delete contacts");
}


void
on_mnuOffline_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gifhdr_t *gifheaderS;
	char *gifbufferS, *errormsg;

	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
	gifbufferS = (char *) malloc(sizeof(gifhdr_t));
	gifheaderS->type = GIF_OFFLINE_REQUEST_MSG;
	gifheaderS->length = 0;
	strcpy(gifheaderS->sender, client_loginid);
	strcpy(gifheaderS->receiver, "server");
	gifheaderS->reserved = 0;

	memcpy(gifbufferS, gifheaderS, 32);

	if((send(sockfd, gifbufferS, 32, 0)) < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}

	free(gifbufferS);
	free(gifheaderS);

	offline = create_Offline();
	gtk_widget_show(offline);
}


/*
GtkWidget* create_msgbox (const gchar*, const gchar*);

GtkWidget*
create_msgbox (const gchar* msg_type, const gchar* message)
{
  GtkWidget *msgbox;
  GtkWidget *dialog_vbox1;
  GtkWidget *dialog_action_area1;
  GtkWidget *button4;

  msgbox = gnome_message_box_new (message,
                              msg_type, NULL);
*/
