#include "../common/common.h"
#include "support.h"
#include "callbacks.h"
#include "interface.h"



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

GtkWidget *gifmain;

gint delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
        gtk_main_quit ();
        return (FALSE);
}

void on_mnuConnect_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	head = NULL;
	authen = create_Authen();
	gtk_widget_show(authen);
}


void on_mnuDisconect_activate(GtkMenuItem * menuitem, gpointer user_data)
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


void on_mnuQuit_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_main_quit();
}


void on_mnuAbout_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_widget_show(create_About());
}


void on_butConnect_clicked(GtkButton * button, gpointer user_data)
{
	head = NULL;
	authen = create_Authen();
	gtk_widget_show(authen);
}


void on_butAdd_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_show(create_AddContacts());
}


void on_butConf_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_show(create_msgbox("info", "Under Construction"));
}


void on_butOffline_clicked(GtkButton * button, gpointer user_data)
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


void on_butCancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(authen);
}


void on_butClear_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *loginid = lookup_widget(GTK_WIDGET(button), "entUserid");
	GtkWidget *password = lookup_widget(GTK_WIDGET(button), "entPass");
	GtkWidget *servip = lookup_widget(GTK_WIDGET(button), "entServip");

	gtk_entry_set_text(GTK_ENTRY(loginid), "");
	gtk_entry_set_text(GTK_ENTRY(password), "");
	gtk_entry_set_text(GTK_ENTRY(servip), "");
}


void on_butOk_clicked(GtkButton * button, gpointer user_data)
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
	inet_pton(AF_INET, gtk_entry_get_text(GTK_ENTRY(servip)),&servAddr.sin_addr);

	//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
		return;
	}
	//connect to the server
	if((connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)))< 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
		return;
	}

	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
	gifheaderS->type = GIF_LOGIN_MSG;
	strcpy(gifheaderS->sender, gtk_entry_get_text(GTK_ENTRY(loginid))); //mark
	strcpy(gifheaderS->receiver, "server"); //mark
	gifheaderS->reserved = 0;


	gifdataS =(char *) malloc(strlen(gtk_entry_get_text(GTK_ENTRY(loginid))) +
	                          strlen(gtk_entry_get_text(GTK_ENTRY(password))) + 4);
	strcpy(gifdataS, gtk_entry_get_text(GTK_ENTRY(loginid)));
	strcat(gifdataS, "#*&");
	strcat(gifdataS, gtk_entry_get_text(GTK_ENTRY(password)));
	gifheaderS->length = strlen(gifdataS) + 1;


	gifbufferS = (char *) malloc(HEADER_LENGTH + gifheaderS->length);
	memcpy(gifbufferS, gifheaderS, HEADER_LENGTH);
	memcpy((gifbufferS + HEADER_LENGTH), gifdataS, gifheaderS->length);

	if((send(sockfd, gifbufferS, (HEADER_LENGTH + gifheaderS->length), 0)) < 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}

	free(gifheaderS);
	free(gifdataS);
	free(gifbufferS);

	if(pthread_create(&pthd, NULL, (void *) gif_receive_messages, (void *) &sockfd) != 0)
	{
		errormsg = strerror(errno);
		gtk_widget_show(create_msgbox("error", errormsg));
	}
	pthread_detach(pthd);

	gtk_widget_destroy(authen);
}


void
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


void
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
