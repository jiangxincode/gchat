#include "../common/common.h"
#include "support.h"
#include "callbacks.h"
#include "interface.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) gtk_object_set_data_full (GTK_OBJECT (component), name,  gtk_widget_ref (widget), (GtkDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) gtk_object_set_data (GTK_OBJECT (component), name, widget)

static GnomeUIInfo conn_menu_uiinfo[] =
{
        {GNOME_APP_UI_ITEM, N_("C_onnect"), NULL, (gpointer) on_mnuConnect_activate, NULL, NULL, 0, NULL, 0, 0, NULL},
        {GNOME_APP_UI_ITEM, N_("_Disconnect"),NULL,(gpointer) on_mnuDisconect_activate, NULL, NULL,0, NULL,0, 0, NULL},
        GNOMEUIINFO_SEPARATOR,
        GNOMEUIINFO_MENU_EXIT_ITEM (on_mnuQuit_activate, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo mnuContacts_menu_uiinfo[] =
{
        {GNOME_APP_UI_ITEM, N_("_Add"), NULL, (gpointer) on_mnuAdd_activate, NULL, NULL, 0, NULL, 0, 0, NULL},
        {GNOME_APP_UI_ITEM, N_("_Delete"),NULL, (gpointer) on_mnuDelete_activate, NULL, NULL, 0, NULL, 0, 0, NULL},
        GNOMEUIINFO_SEPARATOR,
        {GNOME_APP_UI_ITEM, N_("_Offline Messages"), NULL, (gpointer) on_mnuOffline_activate, NULL, NULL, 0, NULL, 0, 0, NULL},
        GNOMEUIINFO_END
};

static GnomeUIInfo mnuHelp_menu_uiinfo[] =
{
        GNOMEUIINFO_MENU_ABOUT_ITEM (on_mnuAbout_activate, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo menubar_uiinfo[] =
{
        {GNOME_APP_UI_SUBTREE, N_("_Conn"),NULL,conn_menu_uiinfo, NULL, NULL, 0, NULL, 0, 0, NULL},
        {GNOME_APP_UI_SUBTREE, N_("Con_tacts"),NULL,mnuContacts_menu_uiinfo, NULL, NULL, 0, NULL, 0, 0, NULL},
        GNOMEUIINFO_MENU_HELP_TREE (mnuHelp_menu_uiinfo),
        GNOMEUIINFO_END
};

GtkWidget *scrolledwindow1;
GtkWidget *scrolledwindow4;

GtkWidget* create_gifclient (void)
{
        GtkWidget *gifclient;
        GtkWidget *toolbar;
        GtkWidget *tmp_toolbar_icon;
        GtkWidget *butConnect,*butAdd, *butConf, *butOffline;
        GtkWidget *appbar;
        char pathname[MAX_PATH_LENGTH];

        gifclient = gnome_app_new ("gchat", "gchat");
        gtk_window_set_position (GTK_WINDOW (gifclient), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size (GTK_WINDOW (gifclient), 205, 400);

        strcpy(pathname,COMMON_PATH_PREFIX);
        strcat(pathname,"client/pixmaps/icon.png");
        gtk_window_set_icon(GTK_WINDOW(gifclient), create_pixbuf(pathname));

        gnome_app_create_menus (GNOME_APP (gifclient), menubar_uiinfo);
        gtk_widget_set_sensitive (conn_menu_uiinfo[1].widget, FALSE);
        gtk_widget_set_sensitive (mnuContacts_menu_uiinfo[0].widget, FALSE);
        gtk_widget_set_sensitive (mnuContacts_menu_uiinfo[1].widget, FALSE);
        gtk_widget_set_sensitive (mnuContacts_menu_uiinfo[3].widget, FALSE);
        gtk_widget_show(gifclient);

        toolbar = gtk_toolbar_new ();
        gtk_widget_show (toolbar);
        gnome_app_add_toolbar (GNOME_APP (gifclient), GTK_TOOLBAR (toolbar), "toolbar",
                               BONOBO_DOCK_ITEM_BEH_EXCLUSIVE,
                               BONOBO_DOCK_TOP, 1, 0, 0);
        gtk_container_set_border_width (GTK_CONTAINER (toolbar), 1);
        gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_BOTH);

        tmp_toolbar_icon = gtk_image_new_from_stock ("gtk-jump-to", gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar)));
        butConnect = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                        GTK_TOOLBAR_CHILD_BUTTON,
                        NULL,
                        _("C_onn"),
                        _("Connect to the server"), NULL,
                        tmp_toolbar_icon, NULL, NULL);
        gtk_label_set_use_underline (GTK_LABEL (((GtkToolbarChild*) (g_list_last (GTK_TOOLBAR (toolbar)->children)->data))->label), TRUE);
        gtk_widget_show (butConnect);

        tmp_toolbar_icon = gtk_image_new_from_stock ("gtk-add", gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar)));
        butAdd = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                             GTK_TOOLBAR_CHILD_BUTTON,
                                             NULL,
                                             _("_Add"),
                                             _("Add new contacts"), NULL,
                                             tmp_toolbar_icon, NULL, NULL);
        gtk_label_set_use_underline (GTK_LABEL (((GtkToolbarChild*) (g_list_last (GTK_TOOLBAR (toolbar)->children)->data))->label), TRUE);
        gtk_widget_show (butAdd);
        gtk_widget_set_sensitive (butAdd, FALSE);

        tmp_toolbar_icon = gtk_image_new_from_stock ("gnome-stock-scores", gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar)));
        butConf = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                                              GTK_TOOLBAR_CHILD_BUTTON,
                                              NULL,
                                              _("Con_f"),
                                              _("Make a conference"), NULL,
                                              tmp_toolbar_icon, NULL, NULL);
        gtk_label_set_use_underline (GTK_LABEL (((GtkToolbarChild*) (g_list_last (GTK_TOOLBAR (toolbar)->children)->data))->label), TRUE);
        gtk_widget_show (butConf);
        gtk_widget_set_sensitive (butConf, FALSE);

        tmp_toolbar_icon = gtk_image_new_from_stock ("gnome-stock-mail", gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar)));
        butOffline = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar),
                        GTK_TOOLBAR_CHILD_BUTTON,
                        NULL,
                        _("Off_line"),
                        _("View offline messages"), NULL,
                        tmp_toolbar_icon, NULL, NULL);
        gtk_label_set_use_underline (GTK_LABEL (((GtkToolbarChild*) (g_list_last (GTK_TOOLBAR (toolbar)->children)->data))->label), TRUE);
        gtk_widget_show (butOffline);
        gtk_widget_set_sensitive (butOffline, FALSE);

        scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
        gtk_widget_show (scrolledwindow1);
        gnome_app_set_contents (GNOME_APP (gifclient), scrolledwindow1);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

        appbar = gnome_appbar_new (TRUE, TRUE, GNOME_PREFERENCES_NEVER);
        gtk_widget_show (appbar);
        gnome_app_set_statusbar (GNOME_APP (gifclient), appbar);

        gnome_app_install_menu_hints (GNOME_APP (gifclient), menubar_uiinfo);
        gtk_signal_connect (GTK_OBJECT (butConnect), "clicked",
                            GTK_SIGNAL_FUNC (on_butConnect_clicked),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butAdd), "clicked",
                            GTK_SIGNAL_FUNC (on_butAdd_clicked),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butConf), "clicked",
                            GTK_SIGNAL_FUNC (on_butConf_clicked),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butOffline), "clicked",
                            GTK_SIGNAL_FUNC (on_butOffline_clicked),
                            NULL);

        /* Store pointers to all widgets, for use by lookup_widget(). */
        GLADE_HOOKUP_OBJECT_NO_REF (gifclient, gifclient, "gifclient");
        GLADE_HOOKUP_OBJECT (gifclient, menubar_uiinfo[0].widget, "conn");
        GLADE_HOOKUP_OBJECT (gifclient, conn_menu_uiinfo[0].widget, "mnuConnect");
        GLADE_HOOKUP_OBJECT (gifclient, conn_menu_uiinfo[1].widget, "mnuDisconect");
        GLADE_HOOKUP_OBJECT (gifclient, conn_menu_uiinfo[2].widget, "separator1");
        GLADE_HOOKUP_OBJECT (gifclient, conn_menu_uiinfo[3].widget, "mnuQuit");
        GLADE_HOOKUP_OBJECT (gifclient, menubar_uiinfo[1].widget, "mnuContacts");
        GLADE_HOOKUP_OBJECT (gifclient, mnuContacts_menu_uiinfo[0].widget, "mnuAdd");
        GLADE_HOOKUP_OBJECT (gifclient, mnuContacts_menu_uiinfo[1].widget, "mnuDelete");
        GLADE_HOOKUP_OBJECT (gifclient, mnuContacts_menu_uiinfo[2].widget, "separator2");
        GLADE_HOOKUP_OBJECT (gifclient, mnuContacts_menu_uiinfo[3].widget, "mnuOffline");
        GLADE_HOOKUP_OBJECT (gifclient, menubar_uiinfo[2].widget, "mnuHelp");
        GLADE_HOOKUP_OBJECT (gifclient, mnuHelp_menu_uiinfo[0].widget, "mnuAbout");
        GLADE_HOOKUP_OBJECT (gifclient, toolbar, "toolbar");
        GLADE_HOOKUP_OBJECT (gifclient, butConnect, "butConnect");
        GLADE_HOOKUP_OBJECT (gifclient, butAdd, "butAdd");
        GLADE_HOOKUP_OBJECT (gifclient, butConf, "butConf");
        GLADE_HOOKUP_OBJECT (gifclient, butOffline, "butOffline");
        GLADE_HOOKUP_OBJECT (gifclient, scrolledwindow1, "scrolledwindow1");
        GLADE_HOOKUP_OBJECT (gifclient, appbar, "appbar");

        return gifclient;
}

GtkWidget* create_msgbox (const gchar* msg_type, const gchar* message)
{
        GtkWidget *msgbox;
        GtkWidget *dialog_vbox1;
        GtkWidget *dialog_action_area1;
        GtkWidget *button4;

        msgbox = gnome_message_box_new (message, msg_type, NULL);
        gtk_window_set_title (GTK_WINDOW (msgbox), _("gchat - Message"));
        gtk_window_set_position (GTK_WINDOW (msgbox), GTK_WIN_POS_CENTER);
        gtk_window_set_resizable (GTK_WINDOW (msgbox), FALSE);

        dialog_vbox1 = GNOME_DIALOG (msgbox)->vbox;
        gtk_widget_show (dialog_vbox1);

        dialog_action_area1 = GNOME_DIALOG (msgbox)->action_area;
        gtk_widget_show (dialog_action_area1);
        gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

        gnome_dialog_append_button (GNOME_DIALOG (msgbox), "gtk-ok");
        button4 = GTK_WIDGET (g_list_last (GNOME_DIALOG (msgbox)->buttons)->data);
        gtk_widget_show (button4);
        GTK_WIDGET_SET_FLAGS (button4, GTK_CAN_DEFAULT);

        /* Store pointers to all widgets, for use by lookup_widget(). */
        GLADE_HOOKUP_OBJECT_NO_REF (msgbox, msgbox, "msgbox");
        GLADE_HOOKUP_OBJECT_NO_REF (msgbox, dialog_vbox1, "dialog_vbox1");
        GLADE_HOOKUP_OBJECT_NO_REF (msgbox, dialog_action_area1, "dialog_action_area1");
        GLADE_HOOKUP_OBJECT (msgbox, button4, "button4");

        return msgbox;
}

GtkWidget* create_About (void)
{
        const gchar *authors[] =
        {
                "Tommy Zhang <nig.daemon@gmail.com>",
                NULL
        };
        const gchar *documenters[] = { NULL };
        /* TRANSLATORS: Replace this string with your names, one name per line. */
        gchar *translators = _("translator_credits");
        GtkWidget *About;

        if (!strcmp (translators, "translator_credits"))
                translators = NULL;
        gchar *VERSION = "0.01";
        About = gnome_about_new ("gchat", VERSION,
                                 _("Copyright(c) 2003 SAP"),
                                 _("A `GNOME chat interface' to communicate with people. "),
                                 authors,
                                 documenters,
                                 translators,
                                 NULL);

        /* Store pointers to all widgets, for use by lookup_widget(). */
        GLADE_HOOKUP_OBJECT_NO_REF (About, About, "About");

        return About;
}

GtkWidget* create_Authen (void)
{
        GtkWidget *Authen;
        GtkWidget *dialog_vbox2;
        GtkWidget *vbox1;
        GtkWidget *label4;
        GtkWidget *hbox1;
        GtkWidget *image1;
        GtkWidget *label1;
        GtkWidget *image2;
        GtkWidget *entServip;
        GtkWidget *image3;
        GtkWidget *label5;
        GtkWidget *hbox2;
        GtkWidget *image6;
        GtkWidget *label2;
        GtkWidget *image5;
        GtkWidget *entUserid;
        GtkWidget *image4;
        GtkWidget *label6;
        GtkWidget *hbox3;
        GtkWidget *image7;
        GtkWidget *label3;
        GtkWidget *image8;
        GtkWidget *entPass;
        GtkWidget *image9;
        GtkWidget *label7;
        GtkWidget *dialog_action_area2;
        GtkWidget *butCancel;
        GtkWidget *butClear;
        GtkWidget *butOk;
        GtkTooltips *tooltips;

        tooltips = gtk_tooltips_new ();

        Authen = gnome_dialog_new (_("gchat - Authentication"), NULL);
        gtk_widget_set_usize (Authen, 350, 220);
        gtk_tooltips_set_tip (tooltips, Authen, _("Enter server ip"), NULL);
        gtk_window_set_resizable (GTK_WINDOW (Authen), FALSE);

        dialog_vbox2 = GNOME_DIALOG (Authen)->vbox;
        gtk_widget_show (dialog_vbox2);

        vbox1 = gtk_vbox_new (FALSE, 0);
        gtk_widget_show (vbox1);
        gtk_box_pack_start (GTK_BOX (dialog_vbox2), vbox1, TRUE, TRUE, 0);

        label4 = gtk_label_new ("");
        gtk_widget_show (label4);
        gtk_box_pack_start (GTK_BOX (vbox1), label4, FALSE, FALSE, 0);
        gtk_label_set_justify (GTK_LABEL (label4), GTK_JUSTIFY_LEFT);

        hbox1 = gtk_hbox_new (FALSE, 0);
        gtk_widget_show (hbox1);
        gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

        image1 = create_pixmap (Authen, NULL);
        gtk_widget_show (image1);
        gtk_box_pack_start (GTK_BOX (hbox1), image1, TRUE, TRUE, 0);

        label1 = gtk_label_new (_("Server Ip"));
        gtk_widget_show (label1);
        gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);
        gtk_widget_set_usize (label1, 60, -2);
        gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);

        image2 = create_pixmap (Authen, NULL);
        gtk_widget_show (image2);
        gtk_box_pack_start (GTK_BOX (hbox1), image2, TRUE, TRUE, 0);

        entServip = gtk_entry_new ();
        gtk_widget_show (entServip);
        gtk_box_pack_start (GTK_BOX (hbox1), entServip, TRUE, TRUE, 0);
        gtk_tooltips_set_tip (tooltips, entServip, _("Enter server ip"), NULL);

        image3 = create_pixmap (Authen, NULL);
        gtk_widget_show (image3);
        gtk_box_pack_start (GTK_BOX (hbox1), image3, TRUE, TRUE, 0);

        label5 = gtk_label_new ("");
        gtk_widget_show (label5);
        gtk_box_pack_start (GTK_BOX (vbox1), label5, FALSE, FALSE, 0);
        gtk_label_set_justify (GTK_LABEL (label5), GTK_JUSTIFY_LEFT);

        hbox2 = gtk_hbox_new (FALSE, 0);
        gtk_widget_show (hbox2);
        gtk_box_pack_start (GTK_BOX (vbox1), hbox2, TRUE, TRUE, 0);

        image6 = create_pixmap (Authen, NULL);
        gtk_widget_show (image6);
        gtk_box_pack_start (GTK_BOX (hbox2), image6, TRUE, TRUE, 0);

        label2 = gtk_label_new (_("User Id"));
        gtk_widget_show (label2);
        gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);
        gtk_widget_set_usize (label2, 60, -2);
        gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

        image5 = create_pixmap (Authen, NULL);
        gtk_widget_show (image5);
        gtk_box_pack_start (GTK_BOX (hbox2), image5, TRUE, TRUE, 0);

        entUserid = gtk_entry_new ();
        gtk_widget_show (entUserid);
        gtk_box_pack_start (GTK_BOX (hbox2), entUserid, TRUE, TRUE, 0);
        gtk_tooltips_set_tip (tooltips, entUserid, _("Enter user id"), NULL);

        image4 = create_pixmap (Authen, NULL);
        gtk_widget_show (image4);
        gtk_box_pack_start (GTK_BOX (hbox2), image4, TRUE, TRUE, 0);

        label6 = gtk_label_new ("");
        gtk_widget_show (label6);
        gtk_box_pack_start (GTK_BOX (vbox1), label6, FALSE, FALSE, 0);
        gtk_label_set_justify (GTK_LABEL (label6), GTK_JUSTIFY_LEFT);

        hbox3 = gtk_hbox_new (FALSE, 0);
        gtk_widget_show (hbox3);
        gtk_box_pack_start (GTK_BOX (vbox1), hbox3, TRUE, TRUE, 0);

        image7 = create_pixmap (Authen, NULL);
        gtk_widget_show (image7);
        gtk_box_pack_start (GTK_BOX (hbox3), image7, TRUE, TRUE, 0);

        label3 = gtk_label_new (_("Password"));
        gtk_widget_show (label3);
        gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
        gtk_widget_set_usize (label3, 60, -2);
        gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

        image8 = create_pixmap (Authen, NULL);
        gtk_widget_show (image8);
        gtk_box_pack_start (GTK_BOX (hbox3), image8, TRUE, TRUE, 0);

        entPass = gtk_entry_new ();
        gtk_widget_show (entPass);
        gtk_box_pack_start (GTK_BOX (hbox3), entPass, TRUE, TRUE, 0);
        gtk_tooltips_set_tip (tooltips, entPass, _("Enter password"), NULL);
        gtk_entry_set_visibility (GTK_ENTRY (entPass), FALSE);

        image9 = create_pixmap (Authen, NULL);
        gtk_widget_show (image9);
        gtk_box_pack_start (GTK_BOX (hbox3), image9, TRUE, TRUE, 0);

        label7 = gtk_label_new ("");
        gtk_widget_show (label7);
        gtk_box_pack_start (GTK_BOX (vbox1), label7, FALSE, FALSE, 0);
        gtk_label_set_justify (GTK_LABEL (label7), GTK_JUSTIFY_LEFT);

        dialog_action_area2 = GNOME_DIALOG (Authen)->action_area;
        gtk_widget_show (dialog_action_area2);
        gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area2), GTK_BUTTONBOX_SPREAD);

        gnome_dialog_append_button (GNOME_DIALOG (Authen), "gtk-cancel");
        butCancel = GTK_WIDGET (g_list_last (GNOME_DIALOG (Authen)->buttons)->data);
        gtk_widget_show (butCancel);
        GTK_WIDGET_SET_FLAGS (butCancel, GTK_CAN_DEFAULT);
        gtk_tooltips_set_tip (tooltips, butCancel, _("Cancel window"), NULL);

        gnome_dialog_append_button (GNOME_DIALOG (Authen), "gtk-clear");
        butClear = GTK_WIDGET (g_list_last (GNOME_DIALOG (Authen)->buttons)->data);
        gtk_widget_show (butClear);
        GTK_WIDGET_SET_FLAGS (butClear, GTK_CAN_DEFAULT);
        gtk_tooltips_set_tip (tooltips, butClear, _("Clear all"), NULL);

        gnome_dialog_append_button (GNOME_DIALOG (Authen), "gtk-ok");
        butOk = GTK_WIDGET (g_list_last (GNOME_DIALOG (Authen)->buttons)->data);
        gtk_widget_show (butOk);
        GTK_WIDGET_SET_FLAGS (butOk, GTK_CAN_DEFAULT);
        gtk_tooltips_set_tip (tooltips, butOk, _("Connect"), NULL);

        gtk_signal_connect (GTK_OBJECT (butCancel), "clicked",
                            GTK_SIGNAL_FUNC (on_butCancel_clicked),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butClear), "clicked",
                            GTK_SIGNAL_FUNC (on_butClear_clicked),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butOk), "clicked",
                            GTK_SIGNAL_FUNC (on_butOk_clicked),
                            NULL);

        /* Store pointers to all widgets, for use by lookup_widget(). */
        GLADE_HOOKUP_OBJECT_NO_REF (Authen, Authen, "Authen");
        GLADE_HOOKUP_OBJECT_NO_REF (Authen, dialog_vbox2, "dialog_vbox2");
        GLADE_HOOKUP_OBJECT (Authen, vbox1, "vbox1");
        GLADE_HOOKUP_OBJECT (Authen, label4, "label4");
        GLADE_HOOKUP_OBJECT (Authen, hbox1, "hbox1");
        GLADE_HOOKUP_OBJECT (Authen, image1, "image1");
        GLADE_HOOKUP_OBJECT (Authen, label1, "label1");
        GLADE_HOOKUP_OBJECT (Authen, image2, "image2");
        GLADE_HOOKUP_OBJECT (Authen, entServip, "entServip");
        GLADE_HOOKUP_OBJECT (Authen, image3, "image3");
        GLADE_HOOKUP_OBJECT (Authen, label5, "label5");
        GLADE_HOOKUP_OBJECT (Authen, hbox2, "hbox2");
        GLADE_HOOKUP_OBJECT (Authen, image6, "image6");
        GLADE_HOOKUP_OBJECT (Authen, label2, "label2");
        GLADE_HOOKUP_OBJECT (Authen, image5, "image5");
        GLADE_HOOKUP_OBJECT (Authen, entUserid, "entUserid");
        GLADE_HOOKUP_OBJECT (Authen, image4, "image4");
        GLADE_HOOKUP_OBJECT (Authen, label6, "label6");
        GLADE_HOOKUP_OBJECT (Authen, hbox3, "hbox3");
        GLADE_HOOKUP_OBJECT (Authen, image7, "image7");
        GLADE_HOOKUP_OBJECT (Authen, label3, "label3");
        GLADE_HOOKUP_OBJECT (Authen, image8, "image8");
        GLADE_HOOKUP_OBJECT (Authen, entPass, "entPass");
        GLADE_HOOKUP_OBJECT (Authen, image9, "image9");
        GLADE_HOOKUP_OBJECT (Authen, label7, "label7");
        GLADE_HOOKUP_OBJECT_NO_REF (Authen, dialog_action_area2, "dialog_action_area2");
        GLADE_HOOKUP_OBJECT (Authen, butCancel, "butCancel");
        GLADE_HOOKUP_OBJECT (Authen, butClear, "butClear");
        GLADE_HOOKUP_OBJECT (Authen, butOk, "butOk");
        GLADE_HOOKUP_OBJECT_NO_REF (Authen, tooltips, "tooltips");

        return Authen;
}

GtkWidget* create_Chat (void)
{
        GtkWidget *Chat;
        GtkWidget *vbox2;
        GtkWidget *vbox4;
        GtkWidget *scrolledwindow3;
        GtkWidget *txtDisplay;
        GtkWidget *vbox3;
        GtkWidget *hbox4;
        GtkWidget *scrolledwindow2;
        GtkWidget *viewport2;
        GtkWidget *entInput;
        GtkWidget *hbuttonbox1;
        GtkWidget *butSend;
        GtkWidget *alignment1;
        GtkWidget *hbox5;
        GtkWidget *image10;
        GtkWidget *label8;
        GtkTooltips *tooltips;

        tooltips = gtk_tooltips_new ();

        Chat = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_widget_set_usize (Chat, 450, 280);
        gtk_window_set_title (GTK_WINDOW (Chat), _("gchat - Client"));

        vbox2 = gtk_vbox_new (FALSE, 0);
        gtk_widget_show (vbox2);
        gtk_container_add (GTK_CONTAINER (Chat), vbox2);

        vbox4 = gtk_vbox_new (FALSE, 0);
        gtk_widget_show (vbox4);
        gtk_box_pack_start (GTK_BOX (vbox2), vbox4, TRUE, TRUE, 0);

        scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
        gtk_widget_show (scrolledwindow3);
        gtk_box_pack_start (GTK_BOX (vbox4), scrolledwindow3, TRUE, TRUE, 0);
        gtk_widget_set_usize (scrolledwindow3, -2, 215);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

        txtDisplay = gtk_text_view_new ();
        gtk_widget_show (txtDisplay);
        gtk_container_add (GTK_CONTAINER (scrolledwindow3), txtDisplay);
        gtk_container_set_border_width (GTK_CONTAINER (txtDisplay), 4);
        gtk_text_view_set_editable (GTK_TEXT_VIEW (txtDisplay), FALSE);
        gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (txtDisplay), GTK_WRAP_WORD);
        gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (txtDisplay), FALSE);
        gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW (txtDisplay), 2);
        gtk_text_view_set_left_margin (GTK_TEXT_VIEW (txtDisplay), 5);

        vbox3 = gtk_vbox_new (FALSE, 0);
        gtk_widget_show (vbox3);
        gtk_box_pack_start (GTK_BOX (vbox2), vbox3, TRUE, TRUE, 0);

        hbox4 = gtk_hbox_new (FALSE, 0);
        gtk_widget_show (hbox4);
        gtk_box_pack_start (GTK_BOX (vbox3), hbox4, TRUE, TRUE, 0);

        scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
        gtk_widget_show (scrolledwindow2);
        gtk_box_pack_start (GTK_BOX (hbox4), scrolledwindow2, TRUE, TRUE, 7);
        gtk_widget_set_usize (scrolledwindow2, 280, 20);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

        viewport2 = gtk_viewport_new (NULL, NULL);
        gtk_widget_show (viewport2);
        gtk_container_add (GTK_CONTAINER (scrolledwindow2), viewport2);

        entInput = gtk_entry_new ();
        gtk_widget_show (entInput);
        gtk_container_add (GTK_CONTAINER (viewport2), entInput);
        gtk_tooltips_set_tip (tooltips, entInput, _("Enter the message to be sent"), NULL);

        hbuttonbox1 = gtk_hbutton_box_new ();
        gtk_widget_show (hbuttonbox1);
        gtk_box_pack_start (GTK_BOX (hbox4), hbuttonbox1, TRUE, TRUE, 0);
        gtk_widget_set_usize (hbuttonbox1, 40, -2);
        gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox1), 0);

        butSend = gtk_button_new ();
        gtk_widget_show (butSend);
        gtk_container_add (GTK_CONTAINER (hbuttonbox1), butSend);
        GTK_WIDGET_SET_FLAGS (butSend, GTK_CAN_DEFAULT);
        gtk_tooltips_set_tip (tooltips, butSend, _("Click to send the message"), NULL);

        alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
        gtk_widget_show (alignment1);
        gtk_container_add (GTK_CONTAINER (butSend), alignment1);

        hbox5 = gtk_hbox_new (FALSE, 2);
        gtk_widget_show (hbox5);
        gtk_container_add (GTK_CONTAINER (alignment1), hbox5);

        image10 = gtk_image_new_from_stock ("gnome-stock-mail-snd", GTK_ICON_SIZE_BUTTON);
        gtk_widget_show (image10);
        gtk_box_pack_start (GTK_BOX (hbox5), image10, FALSE, FALSE, 0);

        label8 = gtk_label_new_with_mnemonic (_("Send"));
        gtk_widget_show (label8);
        gtk_box_pack_start (GTK_BOX (hbox5), label8, FALSE, FALSE, 0);
        gtk_label_set_justify (GTK_LABEL (label8), GTK_JUSTIFY_LEFT);

        gtk_signal_connect (GTK_OBJECT (Chat), "destroy",
                            GTK_SIGNAL_FUNC (on_Chat_destroy),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (entInput), "activate",
                            GTK_SIGNAL_FUNC (on_entInput_activate),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butSend), "clicked",
                            GTK_SIGNAL_FUNC (on_butSend_clicked),
                            NULL);

        /* Store pointers to all widgets, for use by lookup_widget(). */
        GLADE_HOOKUP_OBJECT_NO_REF (Chat, Chat, "Chat");
        GLADE_HOOKUP_OBJECT (Chat, vbox2, "vbox2");
        GLADE_HOOKUP_OBJECT (Chat, vbox4, "vbox4");
        GLADE_HOOKUP_OBJECT (Chat, scrolledwindow3, "scrolledwindow3");
        GLADE_HOOKUP_OBJECT (Chat, txtDisplay, "txtDisplay");
        GLADE_HOOKUP_OBJECT (Chat, vbox3, "vbox3");
        GLADE_HOOKUP_OBJECT (Chat, hbox4, "hbox4");
        GLADE_HOOKUP_OBJECT (Chat, scrolledwindow2, "scrolledwindow2");
        GLADE_HOOKUP_OBJECT (Chat, viewport2, "viewport2");
        GLADE_HOOKUP_OBJECT (Chat, entInput, "entInput");
        GLADE_HOOKUP_OBJECT (Chat, hbuttonbox1, "hbuttonbox1");
        GLADE_HOOKUP_OBJECT (Chat, butSend, "butSend");
        GLADE_HOOKUP_OBJECT (Chat, alignment1, "alignment1");
        GLADE_HOOKUP_OBJECT (Chat, hbox5, "hbox5");
        GLADE_HOOKUP_OBJECT (Chat, image10, "image10");
        GLADE_HOOKUP_OBJECT (Chat, label8, "label8");
        GLADE_HOOKUP_OBJECT_NO_REF (Chat, tooltips, "tooltips");

        return Chat;
}

GtkWidget* create_AddContacts (void)
{
        GtkWidget *AddContacts;
        GtkWidget *dialog_vbox3;
        GtkWidget *vbox6;
        GtkWidget *label15;
        GtkWidget *hbox7;
        GtkWidget *image11;
        GtkWidget *label14;
        GtkWidget *image12;
        GtkWidget *entNewContactsid;
        GtkWidget *image13;
        GtkWidget *label16;
        GtkWidget *dialog_action_area3;
        GtkWidget *butAddContactsCancel;
        GtkWidget *butAddContactsOk;

        AddContacts = gnome_dialog_new (_("gchat - Add contacts"), NULL);
        gtk_widget_set_usize (AddContacts, 300, 120);
        gtk_window_set_default_size (GTK_WINDOW (AddContacts), 300, 120);
        gtk_window_set_resizable (GTK_WINDOW (AddContacts), FALSE);

        dialog_vbox3 = GNOME_DIALOG (AddContacts)->vbox;
        gtk_widget_show (dialog_vbox3);

        vbox6 = gtk_vbox_new (FALSE, 0);
        gtk_widget_show (vbox6);
        gtk_box_pack_start (GTK_BOX (dialog_vbox3), vbox6, TRUE, TRUE, 0);

        label15 = gtk_label_new ("");
        gtk_widget_show (label15);
        gtk_box_pack_start (GTK_BOX (vbox6), label15, FALSE, FALSE, 0);
        gtk_label_set_justify (GTK_LABEL (label15), GTK_JUSTIFY_LEFT);

        hbox7 = gtk_hbox_new (FALSE, 0);
        gtk_widget_show (hbox7);
        gtk_box_pack_start (GTK_BOX (vbox6), hbox7, TRUE, TRUE, 0);

        image11 = create_pixmap (AddContacts, NULL);
        gtk_widget_show (image11);
        gtk_box_pack_start (GTK_BOX (hbox7), image11, TRUE, TRUE, 0);

        label14 = gtk_label_new (_("Contact's id"));
        gtk_widget_show (label14);
        gtk_box_pack_start (GTK_BOX (hbox7), label14, FALSE, FALSE, 0);
        gtk_label_set_justify (GTK_LABEL (label14), GTK_JUSTIFY_LEFT);

        image12 = create_pixmap (AddContacts, NULL);
        gtk_widget_show (image12);
        gtk_box_pack_start (GTK_BOX (hbox7), image12, TRUE, TRUE, 0);

        entNewContactsid = gtk_entry_new ();
        gtk_widget_show (entNewContactsid);
        gtk_box_pack_start (GTK_BOX (hbox7), entNewContactsid, TRUE, TRUE, 0);

        image13 = create_pixmap (AddContacts, NULL);
        gtk_widget_show (image13);
        gtk_box_pack_start (GTK_BOX (hbox7), image13, TRUE, TRUE, 0);

        label16 = gtk_label_new ("");
        gtk_widget_show (label16);
        gtk_box_pack_start (GTK_BOX (vbox6), label16, FALSE, FALSE, 0);
        gtk_label_set_justify (GTK_LABEL (label16), GTK_JUSTIFY_LEFT);

        dialog_action_area3 = GNOME_DIALOG (AddContacts)->action_area;
        gtk_widget_show (dialog_action_area3);
        gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area3), GTK_BUTTONBOX_SPREAD);

        gnome_dialog_append_button (GNOME_DIALOG (AddContacts), "gtk-cancel");
        butAddContactsCancel = GTK_WIDGET (g_list_last (GNOME_DIALOG (AddContacts)->buttons)->data);
        gtk_widget_show (butAddContactsCancel);
        GTK_WIDGET_SET_FLAGS (butAddContactsCancel, GTK_CAN_DEFAULT);

        gnome_dialog_append_button (GNOME_DIALOG (AddContacts), "gtk-ok");
        butAddContactsOk = GTK_WIDGET (g_list_last (GNOME_DIALOG (AddContacts)->buttons)->data);
        gtk_widget_show (butAddContactsOk);
        GTK_WIDGET_SET_FLAGS (butAddContactsOk, GTK_CAN_DEFAULT);

        gtk_signal_connect (GTK_OBJECT (butAddContactsCancel), "clicked",
                            GTK_SIGNAL_FUNC (on_butAddContactsCancel_clicked),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butAddContactsOk), "clicked",
                            GTK_SIGNAL_FUNC (on_butAddContactsOk_clicked),
                            NULL);

        /* Store pointers to all widgets, for use by lookup_widget(). */
        GLADE_HOOKUP_OBJECT_NO_REF (AddContacts, AddContacts, "AddContacts");
        GLADE_HOOKUP_OBJECT_NO_REF (AddContacts, dialog_vbox3, "dialog_vbox3");
        GLADE_HOOKUP_OBJECT (AddContacts, vbox6, "vbox6");
        GLADE_HOOKUP_OBJECT (AddContacts, label15, "label15");
        GLADE_HOOKUP_OBJECT (AddContacts, hbox7, "hbox7");
        GLADE_HOOKUP_OBJECT (AddContacts, image11, "image11");
        GLADE_HOOKUP_OBJECT (AddContacts, label14, "label14");
        GLADE_HOOKUP_OBJECT (AddContacts, image12, "image12");
        GLADE_HOOKUP_OBJECT (AddContacts, entNewContactsid, "entNewContactsid");
        GLADE_HOOKUP_OBJECT (AddContacts, image13, "image13");
        GLADE_HOOKUP_OBJECT (AddContacts, label16, "label16");
        GLADE_HOOKUP_OBJECT_NO_REF (AddContacts, dialog_action_area3, "dialog_action_area3");
        GLADE_HOOKUP_OBJECT (AddContacts, butAddContactsCancel, "butAddContactsCancel");
        GLADE_HOOKUP_OBJECT (AddContacts, butAddContactsOk, "butAddContactsOk");

        return AddContacts;
}

GtkWidget* create_Offline (void)
{
        GtkWidget *Offline;
        GtkWidget *dialog_vbox4;
        GtkWidget *dialog_action_area4;
        GtkWidget *butOfflineDelete;
        GtkWidget *butOfflineClose;

        Offline = gnome_dialog_new (_("gchat - Offline Messages"), NULL);
        gtk_widget_set_usize (Offline, 500, 250);
        gtk_window_set_default_size (GTK_WINDOW (Offline), 350, 200);
        gtk_window_set_resizable (GTK_WINDOW (Offline), FALSE);

        dialog_vbox4 = GNOME_DIALOG (Offline)->vbox;
        gtk_widget_show (dialog_vbox4);

        scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
        gtk_widget_show (scrolledwindow4);
        gtk_box_pack_start (GTK_BOX (dialog_vbox4), scrolledwindow4, TRUE, TRUE, 0);

        dialog_action_area4 = GNOME_DIALOG (Offline)->action_area;
        gtk_widget_show (dialog_action_area4);
        gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area4), GTK_BUTTONBOX_EDGE);

        gnome_dialog_append_button (GNOME_DIALOG (Offline), "gtk-delete");
        butOfflineDelete = GTK_WIDGET (g_list_last (GNOME_DIALOG (Offline)->buttons)->data);
        gtk_widget_show (butOfflineDelete);
        gtk_widget_set_sensitive (butOfflineDelete, FALSE);
        GTK_WIDGET_SET_FLAGS (butOfflineDelete, GTK_CAN_DEFAULT);

        gnome_dialog_append_button (GNOME_DIALOG (Offline), "gtk-close");
        butOfflineClose = GTK_WIDGET (g_list_last (GNOME_DIALOG (Offline)->buttons)->data);
        gtk_widget_show (butOfflineClose);
        GTK_WIDGET_SET_FLAGS (butOfflineClose, GTK_CAN_DEFAULT);

        gtk_signal_connect (GTK_OBJECT (butOfflineDelete), "clicked",
                            GTK_SIGNAL_FUNC (on_butOfflineDelete_clicked),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butOfflineClose), "clicked",
                            GTK_SIGNAL_FUNC (on_butOfflineClose_clicked),
                            NULL);

        /* Store pointers to all widgets, for use by lookup_widget(). */
        GLADE_HOOKUP_OBJECT_NO_REF (Offline, Offline, "Offline");
        GLADE_HOOKUP_OBJECT_NO_REF (Offline, dialog_vbox4, "dialog_vbox4");
        GLADE_HOOKUP_OBJECT (Offline, scrolledwindow4, "scrolledwindow4");
        GLADE_HOOKUP_OBJECT_NO_REF (Offline, dialog_action_area4, "dialog_action_area4");
        GLADE_HOOKUP_OBJECT (Offline, butOfflineDelete, "butOfflineDelete");
        GLADE_HOOKUP_OBJECT (Offline, butOfflineClose, "butOfflineClose");

        return Offline;
}
