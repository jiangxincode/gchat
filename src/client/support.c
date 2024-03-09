#include "../common/common.h"
#include "main.h"

extern GtkWidget *gifmain;
extern GtkWidget *scrolledwindow1;
extern GtkWidget *scrolledwindow4;

extern GtkWidget *offline;
// the treeview widget used to display the contacts list for the client
extern GtkWidget *tree;
// the treeview widget used to display the offline messages
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
    char *gifdata, *gifbuffer;
    char PATHNAME[MAX_PATH_LENGTH];

    pthread_t pthd = pthread_self();

    while (1)
    {
        gifbuffer = (char *)malloc(BUFF_SIZE);
        rcv_status = recv(server_sockfd, gifbuffer, BUFF_SIZE, 0);

        if (rcv_status < 0) // 发生错误
        {
            message_dialog(GTK_MESSAGE_INFO, strerror(errno));
            pthread_cancel(pthd); // 非正常退出
        }
        else if (rcv_status == 0) // 断开连接
        {
            contacts_chat_window_id_t *ptr;
            GtkWidget *widget;

            message_dialog(GTK_MESSAGE_INFO, "Server Disconnected");

            while (head != NULL) // freeing the allocated spaces for linked list
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

            pthread_cancel(pthd);
        }

        gifheader = (gifhdr_t *)malloc(sizeof(gifhdr_t));
        memcpy(gifheader, gifbuffer, HEADER_LENGTH);
        if ((gifheader->length) > 0)
        {
            gifdata = (char *)malloc(gifheader->length);
            memcpy(gifdata, (gifbuffer + HEADER_LENGTH), gifheader->length);
        }
        else
        {
            gifdata = NULL;
        }

        switch (gifheader->type)
        {
        case GIF_ADDRLIST_MSG:
        {
            contacts_chat_window_id_t *ptr;
            user_status_t *usrs;
            int i, counter;

            GtkWidget *widget;
            GtkTreeStore *store;
            GtkTreeViewColumn *column;
            GtkCellRenderer *renderer;
            GtkTreeIter parent_iter, child_iter;
            GdkPixbuf *img;
            GtkTreeSelection *select;

            if (gifheader->reserved == 1)
            {
                // removing the tree from the container for refresing
                gtk_container_remove(GTK_CONTAINER(scrolledwindow1), tree);

                while (head != NULL) // freeing the allocated spaces for linked list
                {
                    ptr = head;
                    head = head->next;
                    free(ptr);
                }
            }

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

            // creating a model
            store = gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);

            // creating a view
            tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

            // adding the view to the scrolledwindow
            gtk_container_add(GTK_CONTAINER(scrolledwindow1), tree);

            // creating first column
            renderer = gtk_cell_renderer_pixbuf_new();
            column = gtk_tree_view_column_new_with_attributes("Status", renderer, "pixbuf", STATUS_COLUMN_PIXMAP, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

            // creating second column
            renderer = gtk_cell_renderer_text_new();
            column = gtk_tree_view_column_new_with_attributes("Contacts", renderer, "text", CONTACTS_COLUMN_TEXT, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

            // Acquire a top-level iterator
            gtk_tree_store_append(store, &parent_iter, NULL);
            if (gifheader->length != 0)
            {
                gtk_tree_store_set(store, &parent_iter, CONTACTS_COLUMN_TEXT, "Available", -1);
            }
            else
            {
                gtk_tree_store_set(store, &parent_iter, CONTACTS_COLUMN_TEXT, "Not Available", -1);
            }

            i = counter = 0;
            usrs = (user_status_t *)malloc(sizeof(user_status_t));
            while (counter < (gifheader->length))
            {
                contacts_chat_window_id_t *ptr;

                memcpy(usrs, (gifdata + (i * sizeof(user_status_t))), sizeof(user_status_t));

                // setting the window id for each contacts and constructing a linked list
                ptr = (contacts_chat_window_id_t *)malloc(sizeof(contacts_chat_window_id_t));
                strcpy(ptr->contacts_name, usrs->loginid);
                ptr->window_id = i;
                ptr->window_open = 0;
                if (head == NULL)
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
                if (usrs->status == 1)
                {
                    get_full_path_name(PATHNAME, "ok.png", 1, "client/pixmaps");
                    img = gdk_pixbuf_new_from_file(PATHNAME, NULL);
                }
                else
                {
                    get_full_path_name(PATHNAME, "kill.png", 1, "client/pixmaps");
                    img = gdk_pixbuf_new_from_file(PATHNAME, NULL);
                }

                // Acquire a child iterator
                gtk_tree_store_append(store, &child_iter, &parent_iter);
                gtk_tree_store_set(store, &child_iter, CONTACTS_COLUMN_TEXT, usrs->loginid, STATUS_COLUMN_PIXMAP, img, -1);

                i++;
                counter = i * sizeof(user_status_t);
            }
            free(usrs);

            gtk_tree_view_expand_all(GTK_TREE_VIEW(tree));
            gtk_widget_show(tree);

            // signal handling for "changed" event
            select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
            gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
            g_signal_connect(G_OBJECT(select), "changed", G_CALLBACK(gif_call_client_for_chat), NULL);

            if ((gifheader->length) != 0)
            {
                free(gifdata);
            }
            free(gifheader);
            free(gifbuffer);

            break;
        }

        case GIF_CHAT_MSG:
        {
            contacts_chat_window_id_t *ptr;
            GtkWidget *display_text;
            GtkTextBuffer *buff;
            GtkTextIter start, end;
            int line_no;

            for (ptr = head; ptr != NULL; ptr = ptr->next)
            {
                if ((strcmp(ptr->contacts_name, gifheader->sender)) == 0)
                    break;
                else
                    continue;
            }

            if (ptr->window_open == 0)
            {
                GtkWidget *widget;
                GtkTextBuffer *buff;
                ptr->window_open = 1; // since the window is gonna be opened

                chat_window[ptr->window_id] = create_Chat();
                widget =
                    lookup_widget(chat_window[ptr->window_id], "entInput");
                gtk_window_set_focus(GTK_WINDOW(chat_window[ptr->window_id]), widget);
                gtk_window_set_title(GTK_WINDOW(chat_window[ptr->window_id]),
                                     gifheader->sender);
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

            display_text =
                lookup_widget(chat_window[ptr->window_id], "txtDisplay");
            buff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(display_text));
            gtk_text_buffer_insert_at_cursor(buff, gifheader->sender, -1);
            gtk_text_buffer_insert_at_cursor(buff, "  :  ", -1);
            gtk_text_buffer_insert_at_cursor(buff, gifdata, -1);

            // setting the color(blue) for client's name in the display text box
            line_no = gtk_text_buffer_get_line_count(buff);
            gtk_text_buffer_get_iter_at_line(buff, &start, line_no);
            gtk_text_buffer_get_iter_at_line_offset(buff, &end,
                                                    line_no,
                                                    strlen(gifheader->sender));
            gtk_text_buffer_apply_tag_by_name(buff,
                                              "blue_foreground",
                                              &start, &end);

            gtk_text_buffer_insert_at_cursor(buff, "\n", -1);

            if ((gifheader->length) != 0)
                free(gifdata);
            free(gifheader);
            free(gifbuffer);

            break;
        }

        case GIF_SUCCESS_N_ERROR_MSG:
        {
            switch (gifheader->reserved)
            {
            case GIF_ERROR_LOGIN_INCORRECT:
            {
                message_dialog(GTK_MESSAGE_INFO, "Login Incorrect");
                break;
            }

            case GIF_SUCCESS_ADD_CONTACTS:
            {
                message_dialog(GTK_MESSAGE_INFO, "Your new contact has been successfully added");
                break;
            }

            case GIF_ERROR_ADD_CONTACTS:
            {
                message_dialog(GTK_MESSAGE_INFO, "The contact id you entered does not belong to a gchat user");
                break;
            }

            case GIF_SUCCESS_DELETE_CONTACTS:
            {
                message_dialog(GTK_MESSAGE_INFO, "Deleted. See the new list after you re-login");
                break;
            }

            case GIF_ERROR_DELETE_CONTACTS_NOT_A_CONTACT:
            {
                message_dialog(GTK_MESSAGE_INFO, "1. The id you entered is not in your contact list");
                break;
            }

            case GIF_ERROR_DELETE_CONTACTS_NOT_A_MEMBER:
            {
                message_dialog(GTK_MESSAGE_INFO, "2. The id you entered is not in your contact list");
                break;
            }
            }

            if ((gifheader->length) != 0)
            {
                free(gifdata);
            }
            free(gifheader);
            free(gifbuffer);

            break;
        }

        case GIF_OFFLINE_MSG:
        {
            int counter;
            char *message;
            offline_msgs_send_t *omsgs_se;

            GtkWidget *widget;
            GtkListStore *store;
            GtkTreeViewColumn *column;
            GtkCellRenderer *renderer;
            GtkTreeIter iter;
            GdkPixbuf *img;

            if (gifheader->reserved == 1) // 刷新联系人在线状态
            {
                offline_messages_count = 0;

                // creating a model
                store = gtk_list_store_new(OFFLINE_COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
                offline_temp_store = store;

                // creating a view
                offline_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));

                // adding the view to the scrolledwindow
                gtk_container_add(GTK_CONTAINER(scrolledwindow4), offline_tree);

                // creating first column
                renderer = gtk_cell_renderer_pixbuf_new();
                column = gtk_tree_view_column_new_with_attributes("New/Old", renderer, "pixbuf", OFFLINE_NEW_PIXMAP, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(offline_tree), column);

                // creating second column
                renderer = gtk_cell_renderer_text_new();
                g_object_set(G_OBJECT(renderer), "foreground", "red", NULL);
                column = gtk_tree_view_column_new_with_attributes("From", renderer, "text", OFFLINE_SENDER_TEXT, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(offline_tree), column);

                // creating third column
                renderer = gtk_cell_renderer_text_new();
                g_object_set(G_OBJECT(renderer), "foreground", "dark green", NULL);
                column = gtk_tree_view_column_new_with_attributes("Date & Time", renderer, "text", OFFLINE_DATESERIAL_TEXT, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(offline_tree), column);

                // creating fourth column
                renderer = gtk_cell_renderer_text_new();
                column = gtk_tree_view_column_new_with_attributes("Message", renderer, "text", OFFLINE_MSG_TEXT, NULL);
                gtk_tree_view_append_column(GTK_TREE_VIEW(offline_tree), column);
            }
            else
            {
                _DEBUG("error: condition");
                pthread_cancel(pthd);
                return;
            }

            counter = 0;
            omsgs_se =
                (offline_msgs_send_t *)malloc(sizeof(offline_msgs_send_t));
            while (counter < (gifheader->length))
            {
                widget = lookup_widget(offline, "butOfflineDelete");
                gtk_widget_set_sensitive(widget, TRUE);

                memcpy(omsgs_se, (gifdata + counter),
                       sizeof(offline_msgs_send_t));
                counter = counter + sizeof(offline_msgs_send_t);

                message = (char *)malloc(omsgs_se->length);
                strncpy(message, (gifdata + counter), omsgs_se->length);
                counter = counter + (omsgs_se->length);

                // setting the status image for online clients and offline clients
                if (omsgs_se->new == 1)
                {
                    get_full_path_name(PATHNAME, "ok.png", 1, "client/pixmaps");
                    img = gdk_pixbuf_new_from_file(PATHNAME, NULL);
                }
                else
                {
                    get_full_path_name(PATHNAME, "kill.png", 1, "client/pixmaps");
                    img = gdk_pixbuf_new_from_file(PATHNAME, NULL);
                }

                // Acquire a child iterator
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter,
                                   OFFLINE_NEW_PIXMAP, img,
                                   OFFLINE_SENDER_TEXT,
                                   omsgs_se->sender,
                                   OFFLINE_DATESERIAL_TEXT,
                                   omsgs_se->dateserial,
                                   OFFLINE_MSG_TEXT, message, -1);

                offline_messages_count++;

                free(message);
            }
            free(omsgs_se);

            gtk_widget_show(offline_tree);

            if (gifheader->reserved == 1)
            {
                GtkTreeSelection *select;

                // signal handling for "changed" event
                select =
                    gtk_tree_view_get_selection(GTK_TREE_VIEW(offline_tree));
                gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
                g_signal_connect(G_OBJECT(select), "changed",
                                 G_CALLBACK(gif_select_offline_messages), NULL);
            }

            if ((gifheader->length) != 0)
                free(gifdata);
            free(gifheader);
            free(gifbuffer);

            break;
        }
        }
    }
}

GtkWidget *lookup_widget(GtkWidget *widget, const gchar *widget_name)
{
    GtkWidget *parent, *found_widget;

    for (;;)
    {
        if (GTK_IS_MENU(widget))
            parent = gtk_menu_get_attach_widget(GTK_MENU(widget));
        else
            parent = gtk_widget_get_parent(widget);
        if (!parent)
            parent = g_object_get_data(G_OBJECT(widget), "GladeParentKey");
        if (parent == NULL)
            break;
        widget = parent;
    }

    found_widget = (GtkWidget *)g_object_get_data(G_OBJECT(widget),
                                                  widget_name);
    if (!found_widget)
        g_warning("Widget not found: %s", widget_name);
    return found_widget;
}

/* This is an internally used function to create pixmaps. */
GtkWidget *create_pixmap(GtkWidget *widget, const gchar *filename)
{
    GtkWidget *pixmap;
    gchar *pathname = NULL;

    if (!filename || !filename[0])
        return gtk_image_new();

    pathname = g_find_program_in_path(filename);

    if (!pathname)
    {
        g_warning("Couldn't find pixmap file: %s", filename);
        return gtk_image_new();
    }

    pixmap = gtk_image_new_from_file(pathname);
    g_free(pathname);
    return pixmap;
}

/* This is an internally used function to create pixmaps. */

GdkPixbuf *create_pixbuf(const gchar *filename)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if (!pixbuf)
    {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }
    return pixbuf;
}
