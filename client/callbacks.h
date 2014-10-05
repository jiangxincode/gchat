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

gint delete_event (GtkWidget *widget, GdkEvent *event, gpointer data);

void gif_call_client_for_chat(GtkTreeSelection * selection, gpointer data);
void gif_select_offline_messages(GtkTreeSelection * selection, gpointer data);

void
on_mnuConnect_activate(GtkMenuItem     *menuitem,
                       gpointer         user_data);

void
on_mnuDisconect_activate(GtkMenuItem     *menuitem,
                         gpointer         user_data);

void
on_mnuQuit_activate(GtkMenuItem     *menuitem,
                    gpointer         user_data);

void
on_mnuAbout_activate(GtkMenuItem     *menuitem,
                     gpointer         user_data);

void
on_butConnect_clicked(GtkButton       *button,
                      gpointer         user_data);

void
on_butAdd_clicked(GtkButton       *button,
                  gpointer         user_data);

void
on_butConf_clicked(GtkButton       *button,
                   gpointer         user_data);

void
on_butCancel_clicked(GtkButton       *button,
                     gpointer         user_data);

void
on_butClear_clicked(GtkButton       *button,
                    gpointer         user_data);

void
on_butOk_clicked(GtkButton       *button,
                 gpointer         user_data);

void
on_butSend_clicked(GtkButton       *button,
                   gpointer         user_data);

void
on_entInput_activate(GtkEntry        *entry,
                     gpointer         user_data);

void
on_Chat_destroy(GtkObject       *object,
                gpointer         user_data);

void
on_entInput_activate(GtkEntry        *entry,
                     gpointer         user_data);

void
on_butAddContactsCancel_clicked(GtkButton       *button,
                                gpointer         user_data);

void
on_butAddContactsOk_clicked(GtkButton       *button,
                            gpointer         user_data);

void
on_butOfflineDelete_clicked(GtkButton       *button,
                            gpointer         user_data);

void
on_butOfflineClose_clicked(GtkButton       *button,
                           gpointer         user_data);

void
on_butOffline_clicked(GtkButton       *button,
                      gpointer         user_data);

void
on_mnuAdd_activate(GtkMenuItem     *menuitem,
                   gpointer         user_data);

void
on_mnuDelete_activate(GtkMenuItem     *menuitem,
                      gpointer         user_data);

void
on_mnuOffline_activate(GtkMenuItem     *menuitem,
                       gpointer         user_data);
