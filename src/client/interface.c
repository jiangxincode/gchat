#define CLIENT_APP

#include "../common/common.h"
#include "main.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) gtk_object_set_data_full (GTK_OBJECT (component), name,  gtk_widget_ref (widget), (GtkDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) gtk_object_set_data (GTK_OBJECT (component), name, widget)

static GnomeUIInfo conn_menu_uiinfo[] =
{
        {GNOME_APP_UI_ITEM, N_("C_onnect"), NULL, (gpointer) on_Connect, NULL, NULL, 0, NULL, 0, 0, NULL},
        {GNOME_APP_UI_ITEM, N_("_Disconnect"),NULL,(gpointer) on_mnuDisconect_activate, NULL, NULL,0, NULL,0, 0, NULL},
        GNOMEUIINFO_SEPARATOR,
        GNOMEUIINFO_MENU_EXIT_ITEM (gtk_main_quit, NULL),
        GNOMEUIINFO_END
};

static GnomeUIInfo mnuContacts_menu_uiinfo[] =
{
        {GNOME_APP_UI_ITEM, N_("_Add"), NULL, (gpointer) on_Add, NULL, NULL, 0, NULL, 0, 0, NULL},
        {GNOME_APP_UI_ITEM, N_("_Delete"),NULL, (gpointer) on_mnuDelete_activate, NULL, NULL, 0, NULL, 0, 0, NULL},
        GNOMEUIINFO_SEPARATOR,
        {GNOME_APP_UI_ITEM, N_("_Offline Messages"), NULL, (gpointer) on_Offline, NULL, NULL, 0, NULL, 0, 0, NULL},
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

extern GtkWidget *gifmain;

GtkWidget* create_gifmain (void)
{
        GtkWidget *toolbar;
        GtkWidget *tmp_toolbar_icon;
        GtkWidget *butConnect,*butAdd, *butConf, *butOffline;
        GtkWidget *appbar;
        char pathname[MAX_PATH_LENGTH];

        gifmain = gnome_app_new ("gchat", "gchat");
        gtk_window_set_position (GTK_WINDOW (gifmain), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size (GTK_WINDOW (gifmain), 205, 400);

        strcpy(pathname,COMMON_PATH_PREFIX);
        strcat(pathname,"client/pixmaps/icon.png");
        gtk_window_set_icon(GTK_WINDOW(gifmain), create_pixbuf(pathname));

        gnome_app_create_menus (GNOME_APP (gifmain), menubar_uiinfo);
        gtk_widget_set_sensitive (conn_menu_uiinfo[1].widget, FALSE);
        gtk_widget_set_sensitive (mnuContacts_menu_uiinfo[0].widget, FALSE);
        gtk_widget_set_sensitive (mnuContacts_menu_uiinfo[1].widget, FALSE);
        gtk_widget_set_sensitive (mnuContacts_menu_uiinfo[3].widget, FALSE);

        toolbar = gtk_toolbar_new ();
        gtk_widget_show (toolbar);
        gnome_app_add_toolbar (GNOME_APP (gifmain), GTK_TOOLBAR (toolbar), "toolbar",
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
        gnome_app_set_contents (GNOME_APP (gifmain), scrolledwindow1);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

        appbar = gnome_appbar_new (TRUE, TRUE, GNOME_PREFERENCES_NEVER);
        gtk_widget_show (appbar);
        gnome_app_set_statusbar (GNOME_APP (gifmain), appbar);

        gnome_app_install_menu_hints (GNOME_APP (gifmain), menubar_uiinfo);

        gtk_widget_show(gifmain);
        gtk_signal_connect (GTK_OBJECT (butConnect), "clicked", G_CALLBACK (on_Connect), NULL);
        gtk_signal_connect (GTK_OBJECT (butAdd), "clicked", G_CALLBACK (on_Add), NULL);
        gtk_signal_connect (GTK_OBJECT (butConf), "clicked", G_CALLBACK (on_butConf_clicked), NULL);
        gtk_signal_connect (GTK_OBJECT (butOffline), "clicked", G_CALLBACK (on_Offline), NULL);

        /* Store pointers to all widgets, for use by lookup_widget(). */
        GLADE_HOOKUP_OBJECT_NO_REF (gifmain, gifmain, "gifmain");
        GLADE_HOOKUP_OBJECT (gifmain, menubar_uiinfo[0].widget, "conn");
        GLADE_HOOKUP_OBJECT (gifmain, conn_menu_uiinfo[0].widget, "mnuConnect");
        GLADE_HOOKUP_OBJECT (gifmain, conn_menu_uiinfo[1].widget, "mnuDisconect");
        GLADE_HOOKUP_OBJECT (gifmain, conn_menu_uiinfo[2].widget, "separator1");
        GLADE_HOOKUP_OBJECT (gifmain, conn_menu_uiinfo[3].widget, "mnuQuit");
        GLADE_HOOKUP_OBJECT (gifmain, menubar_uiinfo[1].widget, "mnuContacts");
        GLADE_HOOKUP_OBJECT (gifmain, mnuContacts_menu_uiinfo[0].widget, "mnuAdd");
        GLADE_HOOKUP_OBJECT (gifmain, mnuContacts_menu_uiinfo[1].widget, "mnuDelete");
        GLADE_HOOKUP_OBJECT (gifmain, mnuContacts_menu_uiinfo[2].widget, "separator2");
        GLADE_HOOKUP_OBJECT (gifmain, mnuContacts_menu_uiinfo[3].widget, "mnuOffline");
        GLADE_HOOKUP_OBJECT (gifmain, menubar_uiinfo[2].widget, "mnuHelp");
        GLADE_HOOKUP_OBJECT (gifmain, mnuHelp_menu_uiinfo[0].widget, "mnuAbout");
        GLADE_HOOKUP_OBJECT (gifmain, toolbar, "toolbar");
        GLADE_HOOKUP_OBJECT (gifmain, butConnect, "butConnect");
        GLADE_HOOKUP_OBJECT (gifmain, butAdd, "butAdd");
        GLADE_HOOKUP_OBJECT (gifmain, butConf, "butConf");
        GLADE_HOOKUP_OBJECT (gifmain, butOffline, "butOffline");
        GLADE_HOOKUP_OBJECT (gifmain, scrolledwindow1, "scrolledwindow1");
        GLADE_HOOKUP_OBJECT (gifmain, appbar, "appbar");

        return gifmain;
}

GtkWidget* message_dialog (GtkMessageType type, const gchar* message)
{
        GtkWidget *dialog;

        dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%s",message);
        gtk_window_set_title (GTK_WINDOW (dialog), "gchat - Message");
        gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
        gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        return dialog;
}

GtkWidget* create_Authen (void)
{
        GtkWidget *Authen;
        GtkWidget *vbox;
        GtkWidget *hbox1, *hbox2, *hbox3, *hbox4;
        GtkWidget *label1, *label2, *label3;
        GtkWidget *entServip, *entUserid, *entPass;
        GtkWidget *butCancel,*butClear,*butOk;
        GtkTooltips *tooltips;

        Authen = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_default_size(GTK_WINDOW(Authen),400,300);
        gtk_window_set_position(GTK_WINDOW(Authen),GTK_WIN_POS_CENTER);

        tooltips = gtk_tooltips_new ();

        vbox = gtk_vbox_new (FALSE, 0);
        gtk_widget_show (vbox);
        gtk_container_add(GTK_CONTAINER(Authen) , vbox);

        hbox1 = gtk_hbox_new (FALSE, 0);
        gtk_widget_show (hbox1);
        gtk_box_pack_start (GTK_BOX (vbox), hbox1, TRUE, TRUE, 0);

        label1 = gtk_label_new ("Server Ip");
        gtk_widget_show (label1);
        gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);
        gtk_widget_set_usize (label1, 60, -2);
        gtk_label_set_justify (GTK_LABEL (label1), GTK_JUSTIFY_LEFT);


        GtkEntryBuffer *entryBuffer = gtk_entry_buffer_new("127.0.0.1",-1);
        entServip = gtk_entry_new_with_buffer (GTK_ENTRY_BUFFER(entryBuffer));
        gtk_widget_show (entServip);
        gtk_box_pack_start (GTK_BOX (hbox1), entServip, TRUE, TRUE, 0);
        gtk_tooltips_set_tip (tooltips, entServip, "Enter server ip", NULL);

        hbox2 = gtk_hbox_new (FALSE, 0);
        gtk_widget_show (hbox2);
        gtk_box_pack_start (GTK_BOX (vbox), hbox2, TRUE, TRUE, 0);

        label2 = gtk_label_new ("User Id");
        gtk_widget_show (label2);
        gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);
        gtk_widget_set_usize (label2, 60, -2);
        gtk_label_set_justify (GTK_LABEL (label2), GTK_JUSTIFY_LEFT);

        entryBuffer = gtk_entry_buffer_new("jiangxin",-1);
        entUserid = gtk_entry_new_with_buffer (entryBuffer);
        gtk_widget_show (entUserid);
        gtk_box_pack_start (GTK_BOX (hbox2), entUserid, TRUE, TRUE, 0);
        gtk_tooltips_set_tip (tooltips, entUserid, "Enter user id", NULL);

        hbox3 = gtk_hbox_new (FALSE, 0);
        gtk_widget_show (hbox3);
        gtk_box_pack_start (GTK_BOX (vbox), hbox3, TRUE, TRUE, 0);

        label3 = gtk_label_new ("Password");
        gtk_widget_show (label3);
        gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);
        gtk_widget_set_usize (label3, 60, -2);
        gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_LEFT);

        entryBuffer = gtk_entry_buffer_new("123456",-1);
        entPass = gtk_entry_new_with_buffer (entryBuffer);
        gtk_widget_show (entPass);
        gtk_box_pack_start (GTK_BOX (hbox3), entPass, TRUE, TRUE, 0);
        gtk_tooltips_set_tip (tooltips, entPass, "Enter password", NULL);
        gtk_entry_set_visibility (GTK_ENTRY (entPass), FALSE);

        hbox4 = gtk_hbox_new(FALSE, 0);
        gtk_widget_show(hbox4);
        gtk_box_pack_start(GTK_BOX(vbox), hbox4, TRUE, TRUE, 0);

        butCancel = gtk_button_new_with_label("Cancel");
        gtk_widget_show(butCancel);
        gtk_tooltips_set_tip(tooltips,butCancel,"Cancel window", NULL);
        gtk_box_pack_start(GTK_BOX(hbox4),butCancel,TRUE,TRUE,0);

        butClear = gtk_button_new_with_label("Clear");
        gtk_widget_show(butClear);
        gtk_tooltips_set_tip(tooltips,butClear,"Clear ALL", NULL);
        gtk_box_pack_start(GTK_BOX(hbox4),butClear,TRUE,TRUE,0);

        butOk = gtk_button_new_with_label("OK");
        gtk_widget_show(butOk);
        gtk_tooltips_set_tip(tooltips,butOk,"Connect", NULL);
        gtk_box_pack_start(GTK_BOX(hbox4),butOk,TRUE,TRUE,0);

        gtk_signal_connect (GTK_OBJECT (butCancel), "clicked", G_CALLBACK (on_butCancel_clicked), NULL);
        gtk_signal_connect (GTK_OBJECT (butClear), "clicked", G_CALLBACK (on_butClear_clicked), NULL);
        gtk_signal_connect (GTK_OBJECT (butOk), "clicked", G_CALLBACK (on_butOk_clicked), NULL);

        /* Store pointers to all widgets, for use by lookup_widget(). */
        GLADE_HOOKUP_OBJECT_NO_REF (Authen, Authen, "Authen");
        GLADE_HOOKUP_OBJECT (Authen, hbox1, "hbox1");
        GLADE_HOOKUP_OBJECT (Authen, label1, "label1");
        GLADE_HOOKUP_OBJECT (Authen, entServip, "entServip");
        GLADE_HOOKUP_OBJECT (Authen, hbox2, "hbox2");
        GLADE_HOOKUP_OBJECT (Authen, label2, "label2");
        GLADE_HOOKUP_OBJECT (Authen, entUserid, "entUserid");
        GLADE_HOOKUP_OBJECT (Authen, hbox3, "hbox3");
        GLADE_HOOKUP_OBJECT (Authen, label3, "label3");
        GLADE_HOOKUP_OBJECT (Authen, entPass, "entPass");
        GLADE_HOOKUP_OBJECT (Authen, hbox4, "hbox4");
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
                            G_CALLBACK (on_Chat_destroy),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (entInput), "activate",
                            G_CALLBACK (on_entInput_activate),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butSend), "clicked",
                            G_CALLBACK (on_butSend_clicked),
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
                            G_CALLBACK (on_butAddContactsCancel_clicked),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butAddContactsOk), "clicked",
                            G_CALLBACK (on_butAddContactsOk_clicked),
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
                            G_CALLBACK (on_butOfflineDelete_clicked),
                            NULL);
        gtk_signal_connect (GTK_OBJECT (butOfflineClose), "clicked",
                            G_CALLBACK (on_butOfflineClose_clicked),
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
