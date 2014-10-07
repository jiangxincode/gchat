#ifndef MAIN_H
#define MAIN_H
void gif_receive_messages(void *server);	// actually the messages are received from the server

/*
 * This function returns a widget in a component created by Glade.
 * Call it with the toplevel widget in the component (i.e. a window/dialog),
 * or alternatively any widget in the component, and the name of the widget
 * you want returned.
 */
GtkWidget* lookup_widget(GtkWidget *widget, const gchar *widget_name);

/* This is used to create the pixmaps used in the interface. */
GtkWidget* create_pixmap(GtkWidget *widget, const gchar *filename);

/* This is used to create the pixbufs used in the interface. */
GdkPixbuf* create_pixbuf(const gchar *filename);


GtkWidget* create_gifmain(void);
GtkWidget* message_dialog(GtkMessageType type, const gchar* message);
GtkWidget* create_Authen(void);
GtkWidget* create_Chat(void);
GtkWidget* create_AddContacts(void);
GtkWidget* create_Offline(void);


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

void gif_call_client_for_chat(GtkTreeSelection * selection, gpointer data);
void gif_select_offline_messages(GtkTreeSelection * selection, gpointer data);


void on_Connect(GtkWidget *button, gpointer user_data);
void on_Add(GtkWidget *menuitem, gpointer user_data);
void on_mnuDelete_activate(GtkMenuItem * menuitem, gpointer user_data);
void on_mnuDisconect_activate(GtkMenuItem     *menuitem, gpointer         user_data);
void on_mnuAbout_activate(GtkMenuItem     *menuitem, gpointer         user_data);
void on_butConf_clicked(GtkButton       *button, gpointer         user_data);
void on_butCancel_clicked(GtkButton       *button, gpointer         user_data);
void on_butClear_clicked(GtkButton       *button, gpointer         user_data);
void on_butOk_clicked(GtkButton       *button, gpointer         user_data);
void on_butSend_clicked(GtkButton       *button, gpointer         user_data);
void on_entInput_activate(GtkEntry        *entry, gpointer         user_data);
void on_Chat_destroy(GtkObject       *object, gpointer         user_data);
void on_entInput_activate(GtkEntry        *entry, gpointer         user_data);
void on_butAddContactsCancel_clicked(GtkButton       *button, gpointer         user_data);
void on_butAddContactsOk_clicked(GtkButton       *button, gpointer         user_data);
void on_butOfflineDelete_clicked(GtkButton       *button, gpointer         user_data);
void on_butOfflineClose_clicked(GtkButton *button, gpointer user_data);
void on_Offline(GtkWidget *widget, gpointer user_data);

#endif // MAIN_H
