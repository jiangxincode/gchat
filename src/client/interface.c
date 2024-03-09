#define CLIENT_APP

#include "../common/common.h"
#include "main.h"

#define GLADE_HOOKUP_OBJECT(component, widget, name) g_object_set_data_full(G_OBJECT(component), name, widget, (GDestroyNotify)g_free)

#define GLADE_HOOKUP_OBJECT_NO_REF(component, widget, name) g_object_set_data(G_OBJECT(component), name, widget)

GtkWidget *createMenuBar(GtkWidget *gifmain)
{
    GtkWidget *menu_bar = gtk_menu_bar_new();

    GtkWidget *connect_menu = gtk_menu_new();
    GtkWidget *connect_menu_item = gtk_menu_item_new_with_mnemonic("_Conn");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(connect_menu_item), connect_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), connect_menu_item);

    GtkWidget *connect = gtk_menu_item_new_with_mnemonic("C_onnect");
    g_signal_connect(G_OBJECT(connect), "activate", G_CALLBACK(on_Connect), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(connect_menu), connect);

    GtkWidget *disconnect = gtk_menu_item_new_with_mnemonic("_Disconnect");
    gtk_widget_set_sensitive(disconnect, FALSE);
    g_signal_connect(G_OBJECT(disconnect), "activate", G_CALLBACK(on_mnuDisconect_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(connect_menu), disconnect);

    GtkWidget *exit = gtk_menu_item_new_with_mnemonic("_Exit");
    g_signal_connect(G_OBJECT(exit), "activate", G_CALLBACK(gtk_main_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(connect_menu), exit);

    GtkWidget *contacts_menu = gtk_menu_new();
    GtkWidget *contacts_menu_item = gtk_menu_item_new_with_mnemonic("Con_tacts");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(contacts_menu_item), contacts_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), contacts_menu_item);

    GtkWidget *add = gtk_menu_item_new_with_mnemonic("_Add");
    gtk_widget_set_sensitive(add, FALSE);
    g_signal_connect(G_OBJECT(add), "activate", G_CALLBACK(on_Add), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(contacts_menu), add);

    GtkWidget *delete = gtk_menu_item_new_with_mnemonic("_Delete");
    gtk_widget_set_sensitive(delete, FALSE);
    g_signal_connect(G_OBJECT(delete), "activate", G_CALLBACK(on_mnuDisconect_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(contacts_menu), delete);

    GtkWidget *offline = gtk_menu_item_new_with_mnemonic("_Offline Messages");
    gtk_widget_set_sensitive(offline, FALSE);
    g_signal_connect(G_OBJECT(offline), "activate", G_CALLBACK(on_Offline), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(contacts_menu), offline);

    GtkWidget *help_menu = gtk_menu_new();
    GtkWidget *help_menu_item = gtk_menu_item_new_with_mnemonic("_Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu_item), help_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_menu_item);

    GtkWidget *about = gtk_menu_item_new_with_label("About");
    g_signal_connect(G_OBJECT(about), "activate", G_CALLBACK(on_mnuAbout_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about);

    GLADE_HOOKUP_OBJECT(gifmain, connect_menu_item, "conn");
    GLADE_HOOKUP_OBJECT(gifmain, connect, "mnuConnect");
    GLADE_HOOKUP_OBJECT(gifmain, disconnect, "mnuDisconect");
    GLADE_HOOKUP_OBJECT(gifmain, exit, "mnuQuit");
    GLADE_HOOKUP_OBJECT(gifmain, contacts_menu_item, "mnuContacts");
    GLADE_HOOKUP_OBJECT(gifmain, add, "mnuAdd");
    GLADE_HOOKUP_OBJECT(gifmain, delete, "mnuDelete");
    GLADE_HOOKUP_OBJECT(gifmain, offline, "mnuOffline");
    GLADE_HOOKUP_OBJECT(gifmain, help_menu_item, "mnuHelp");
    GLADE_HOOKUP_OBJECT(gifmain, about, "mnuAbout");

    return menu_bar;
}

GtkWidget *createToolBar(GtkWidget *gifmain)
{
    GtkWidget *tool_bar = gtk_toolbar_new();
    gtk_container_set_border_width(GTK_CONTAINER(tool_bar), 1);
    gtk_toolbar_set_style(GTK_TOOLBAR(tool_bar), GTK_TOOLBAR_BOTH);

    GtkToolItem *butConnect = gtk_tool_button_new(NULL, "C_onn");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(butConnect), "gtk-jump-to");
    gtk_tool_item_set_tooltip_text(butConnect, "Connect to the server");
    gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), butConnect, -1);

    GtkToolItem *butAdd = gtk_tool_button_new(NULL, "_Add");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(butAdd), "gtk-add");
    gtk_tool_item_set_tooltip_text(butAdd, "Add new contacts");
    gtk_widget_set_sensitive(GTK_WIDGET(butAdd), FALSE);
    gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), butAdd, -1);

    GtkToolItem *butConf = gtk_tool_button_new(NULL, "Con_f");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(butConf), "gnome-stock-scores");
    gtk_tool_item_set_tooltip_text(butConf, "Make a conference");
    gtk_widget_set_sensitive(GTK_WIDGET(butConf), FALSE);
    gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), butConf, -1);

    GtkToolItem *butOffline = gtk_tool_button_new(NULL, "Off_line");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(butOffline), "gnome-stock-mail");
    gtk_tool_item_set_tooltip_text(butOffline, "View offline messages");
    gtk_widget_set_sensitive(GTK_WIDGET(butOffline), FALSE);
    gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), butOffline, -1);

    g_signal_connect(G_OBJECT(butConnect), "clicked", G_CALLBACK(on_Connect), NULL);
    g_signal_connect(G_OBJECT(butAdd), "clicked", G_CALLBACK(on_Add), NULL);
    g_signal_connect(G_OBJECT(butConf), "clicked", G_CALLBACK(on_butConf_clicked), NULL);
    g_signal_connect(G_OBJECT(butOffline), "clicked", G_CALLBACK(on_Offline), NULL);

    GLADE_HOOKUP_OBJECT(gifmain, tool_bar, "toolbar");
    GLADE_HOOKUP_OBJECT(gifmain, butConnect, "butConnect");
    GLADE_HOOKUP_OBJECT(gifmain, butAdd, "butAdd");
    GLADE_HOOKUP_OBJECT(gifmain, butConf, "butConf");
    GLADE_HOOKUP_OBJECT(gifmain, butOffline, "butOffline");

    return tool_bar;
}

GtkWidget *scrolledwindow1;
GtkWidget *scrolledwindow4;

extern GtkWidget *gifmain;

GtkWidget *create_gifmain(void)
{
    GtkWidget *toolbar;
    GtkWidget *appbar;
    char pathname[MAX_PATH_LENGTH];

    gifmain = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(gifmain), "gchat");

    gtk_window_set_position(GTK_WINDOW(gifmain), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(gifmain), 205, 400);

    strcpy(pathname, COMMON_PATH_PREFIX);
    strcat(pathname, "src/client/pixmaps/icon.png");
    gtk_window_set_icon(GTK_WINDOW(gifmain), create_pixbuf(pathname));

    GtkWidget *menu_bar = createMenuBar(gifmain);
    GtkWidget *tool_bar = createToolBar(gifmain);

    scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    appbar = gtk_statusbar_new();

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), tool_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), appbar, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(gifmain), vbox);

    gtk_widget_show_all(gifmain);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(gifmain, gifmain, "gifmain");
    GLADE_HOOKUP_OBJECT(gifmain, scrolledwindow1, "scrolledwindow1");
    GLADE_HOOKUP_OBJECT(gifmain, appbar, "appbar");

    return gifmain;
}

GtkWidget *message_dialog(GtkMessageType type, const gchar *message)
{
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), "gchat - Message");
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return dialog;
}

GtkWidget *create_Authen(void)
{
    GtkWidget *Authen;
    GtkWidget *vbox;
    GtkWidget *hbox1, *hbox2, *hbox3, *hbox4;
    GtkWidget *label1, *label2, *label3;
    GtkWidget *entServip, *entUserid, *entPass;
    GtkWidget *butCancel, *butClear, *butOk;

    Authen = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(Authen), 400, 300);
    gtk_window_set_position(GTK_WINDOW(Authen), GTK_WIN_POS_CENTER);

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox);
    gtk_container_add(GTK_CONTAINER(Authen), vbox);

    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, TRUE, TRUE, 0);

    label1 = gtk_label_new("Server Ip");
    gtk_widget_show(label1);
    gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 0);
    gtk_widget_set_size_request(label1, 60, -2);
    gtk_label_set_justify(GTK_LABEL(label1), GTK_JUSTIFY_LEFT);

    GtkEntryBuffer *entryBuffer = gtk_entry_buffer_new("127.0.0.1", -1);
    entServip = gtk_entry_new_with_buffer(GTK_ENTRY_BUFFER(entryBuffer));
    gtk_widget_show(entServip);
    gtk_box_pack_start(GTK_BOX(hbox1), entServip, TRUE, TRUE, 0);
    gtk_widget_set_tooltip_text(entServip, "Enter server ip");

    hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox2);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, TRUE, TRUE, 0);

    label2 = gtk_label_new("User Id");
    gtk_widget_show(label2);
    gtk_box_pack_start(GTK_BOX(hbox2), label2, FALSE, FALSE, 0);
    gtk_widget_set_size_request(label2, 60, -2);
    gtk_label_set_justify(GTK_LABEL(label2), GTK_JUSTIFY_LEFT);

    entryBuffer = gtk_entry_buffer_new("jiangxin", -1);
    entUserid = gtk_entry_new_with_buffer(entryBuffer);
    gtk_widget_show(entUserid);
    gtk_box_pack_start(GTK_BOX(hbox2), entUserid, TRUE, TRUE, 0);
    gtk_widget_set_tooltip_text(entUserid, "Enter user id");

    hbox3 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox3);
    gtk_box_pack_start(GTK_BOX(vbox), hbox3, TRUE, TRUE, 0);

    label3 = gtk_label_new("Password");
    gtk_widget_show(label3);
    gtk_box_pack_start(GTK_BOX(hbox3), label3, FALSE, FALSE, 0);
    gtk_widget_set_size_request(label3, 60, -2);
    gtk_label_set_justify(GTK_LABEL(label3), GTK_JUSTIFY_LEFT);

    entryBuffer = gtk_entry_buffer_new("123456", -1);
    entPass = gtk_entry_new_with_buffer(entryBuffer);
    gtk_widget_show(entPass);
    gtk_box_pack_start(GTK_BOX(hbox3), entPass, TRUE, TRUE, 0);
    gtk_widget_set_tooltip_text(entPass, "Enter password");
    gtk_entry_set_visibility(GTK_ENTRY(entPass), FALSE);

    hbox4 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox4);
    gtk_box_pack_start(GTK_BOX(vbox), hbox4, TRUE, TRUE, 0);

    butCancel = gtk_button_new_with_label("Cancel");
    gtk_widget_show(butCancel);
    gtk_widget_set_tooltip_text(butCancel, "Cancel window");
    gtk_box_pack_start(GTK_BOX(hbox4), butCancel, TRUE, TRUE, 0);

    butClear = gtk_button_new_with_label("Clear");
    gtk_widget_show(butClear);
    gtk_widget_set_tooltip_text(butClear, "Clear ALL");
    gtk_box_pack_start(GTK_BOX(hbox4), butClear, TRUE, TRUE, 0);

    butOk = gtk_button_new_with_label("OK");
    gtk_widget_show(butOk);
    gtk_widget_set_tooltip_text(butOk, "Connect");
    gtk_box_pack_start(GTK_BOX(hbox4), butOk, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(butCancel), "clicked", G_CALLBACK(on_butCancel_clicked), NULL);
    g_signal_connect(G_OBJECT(butClear), "clicked", G_CALLBACK(on_butClear_clicked), NULL);
    g_signal_connect(G_OBJECT(butOk), "clicked", G_CALLBACK(on_butOk_clicked), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(Authen, Authen, "Authen");
    GLADE_HOOKUP_OBJECT(Authen, hbox1, "hbox1");
    GLADE_HOOKUP_OBJECT(Authen, label1, "label1");
    GLADE_HOOKUP_OBJECT(Authen, entServip, "entServip");
    GLADE_HOOKUP_OBJECT(Authen, hbox2, "hbox2");
    GLADE_HOOKUP_OBJECT(Authen, label2, "label2");
    GLADE_HOOKUP_OBJECT(Authen, entUserid, "entUserid");
    GLADE_HOOKUP_OBJECT(Authen, hbox3, "hbox3");
    GLADE_HOOKUP_OBJECT(Authen, label3, "label3");
    GLADE_HOOKUP_OBJECT(Authen, entPass, "entPass");
    GLADE_HOOKUP_OBJECT(Authen, hbox4, "hbox4");
    GLADE_HOOKUP_OBJECT(Authen, butCancel, "butCancel");
    GLADE_HOOKUP_OBJECT(Authen, butClear, "butClear");
    GLADE_HOOKUP_OBJECT(Authen, butOk, "butOk");

    return Authen;
}

GtkWidget *create_Chat(void)
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

    Chat = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(Chat, 450, 280);
    gtk_window_set_title(GTK_WINDOW(Chat), "gchat - Client");

    vbox2 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox2);
    gtk_container_add(GTK_CONTAINER(Chat), vbox2);

    vbox4 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox4);
    gtk_box_pack_start(GTK_BOX(vbox2), vbox4, TRUE, TRUE, 0);

    scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolledwindow3);
    gtk_box_pack_start(GTK_BOX(vbox4), scrolledwindow3, TRUE, TRUE, 0);
    gtk_widget_set_size_request(scrolledwindow3, -2, 215);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow3), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    txtDisplay = gtk_text_view_new();
    gtk_widget_show(txtDisplay);
    gtk_container_add(GTK_CONTAINER(scrolledwindow3), txtDisplay);
    gtk_container_set_border_width(GTK_CONTAINER(txtDisplay), 4);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(txtDisplay), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(txtDisplay), GTK_WRAP_WORD);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(txtDisplay), FALSE);
    gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(txtDisplay), 2);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(txtDisplay), 5);

    vbox3 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox3);
    gtk_box_pack_start(GTK_BOX(vbox2), vbox3, TRUE, TRUE, 0);

    hbox4 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox4);
    gtk_box_pack_start(GTK_BOX(vbox3), hbox4, TRUE, TRUE, 0);

    scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolledwindow2);
    gtk_box_pack_start(GTK_BOX(hbox4), scrolledwindow2, TRUE, TRUE, 7);
    gtk_widget_set_size_request(scrolledwindow2, 280, 20);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    viewport2 = gtk_viewport_new(NULL, NULL);
    gtk_widget_show(viewport2);
    gtk_container_add(GTK_CONTAINER(scrolledwindow2), viewport2);

    entInput = gtk_entry_new();
    gtk_widget_show(entInput);
    gtk_container_add(GTK_CONTAINER(viewport2), entInput);
    gtk_widget_set_tooltip_text(entInput, "Enter the message to be sent");

    hbuttonbox1 = gtk_hbutton_box_new();
    gtk_widget_show(hbuttonbox1);
    gtk_box_pack_start(GTK_BOX(hbox4), hbuttonbox1, TRUE, TRUE, 0);
    gtk_widget_set_size_request(hbuttonbox1, 40, -2);
    gtk_box_set_spacing(GTK_BOX(hbuttonbox1), 0);

    butSend = gtk_button_new();
    gtk_widget_show(butSend);
    gtk_container_add(GTK_CONTAINER(hbuttonbox1), butSend);
    gtk_widget_set_can_default(butSend, TRUE);
    gtk_widget_set_tooltip_text(butSend, "Click to send the message");

    alignment1 = gtk_alignment_new(0.5, 0.5, 0, 0);
    gtk_widget_show(alignment1);
    gtk_container_add(GTK_CONTAINER(butSend), alignment1);

    hbox5 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(hbox5);
    gtk_container_add(GTK_CONTAINER(alignment1), hbox5);

    image10 = gtk_image_new_from_stock("gnome-stock-mail-snd", GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(image10);
    gtk_box_pack_start(GTK_BOX(hbox5), image10, FALSE, FALSE, 0);

    label8 = gtk_label_new_with_mnemonic("Send");
    gtk_widget_show(label8);
    gtk_box_pack_start(GTK_BOX(hbox5), label8, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label8), GTK_JUSTIFY_LEFT);

    g_signal_connect(G_OBJECT(Chat), "destroy",
                     G_CALLBACK(on_Chat_destroy),
                     NULL);
    g_signal_connect(G_OBJECT(entInput), "activate",
                     G_CALLBACK(on_entInput_activate),
                     NULL);
    g_signal_connect(G_OBJECT(butSend), "clicked",
                     G_CALLBACK(on_butSend_clicked),
                     NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(Chat, Chat, "Chat");
    GLADE_HOOKUP_OBJECT(Chat, vbox2, "vbox2");
    GLADE_HOOKUP_OBJECT(Chat, vbox4, "vbox4");
    GLADE_HOOKUP_OBJECT(Chat, scrolledwindow3, "scrolledwindow3");
    GLADE_HOOKUP_OBJECT(Chat, txtDisplay, "txtDisplay");
    GLADE_HOOKUP_OBJECT(Chat, vbox3, "vbox3");
    GLADE_HOOKUP_OBJECT(Chat, hbox4, "hbox4");
    GLADE_HOOKUP_OBJECT(Chat, scrolledwindow2, "scrolledwindow2");
    GLADE_HOOKUP_OBJECT(Chat, viewport2, "viewport2");
    GLADE_HOOKUP_OBJECT(Chat, entInput, "entInput");
    GLADE_HOOKUP_OBJECT(Chat, hbuttonbox1, "hbuttonbox1");
    GLADE_HOOKUP_OBJECT(Chat, butSend, "butSend");
    GLADE_HOOKUP_OBJECT(Chat, alignment1, "alignment1");
    GLADE_HOOKUP_OBJECT(Chat, hbox5, "hbox5");
    GLADE_HOOKUP_OBJECT(Chat, image10, "image10");
    GLADE_HOOKUP_OBJECT(Chat, label8, "label8");

    return Chat;
}

GtkWidget *create_AddContacts(void)
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

    AddContacts = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(AddContacts), "gchat - Add contacts");
    gtk_widget_set_size_request(AddContacts, 300, 120);
    gtk_window_set_default_size(GTK_WINDOW(AddContacts), 300, 120);
    gtk_window_set_resizable(GTK_WINDOW(AddContacts), FALSE);

    dialog_vbox3 = gtk_dialog_get_content_area(GTK_DIALOG(AddContacts));
    gtk_widget_show(dialog_vbox3);

    vbox6 = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(vbox6);
    gtk_box_pack_start(GTK_BOX(dialog_vbox3), vbox6, TRUE, TRUE, 0);

    label15 = gtk_label_new("");
    gtk_widget_show(label15);
    gtk_box_pack_start(GTK_BOX(vbox6), label15, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label15), GTK_JUSTIFY_LEFT);

    hbox7 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox7);
    gtk_box_pack_start(GTK_BOX(vbox6), hbox7, TRUE, TRUE, 0);

    image11 = create_pixmap(AddContacts, NULL);
    gtk_widget_show(image11);
    gtk_box_pack_start(GTK_BOX(hbox7), image11, TRUE, TRUE, 0);

    label14 = gtk_label_new("Contact's id");
    gtk_widget_show(label14);
    gtk_box_pack_start(GTK_BOX(hbox7), label14, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label14), GTK_JUSTIFY_LEFT);

    image12 = create_pixmap(AddContacts, NULL);
    gtk_widget_show(image12);
    gtk_box_pack_start(GTK_BOX(hbox7), image12, TRUE, TRUE, 0);

    entNewContactsid = gtk_entry_new();
    gtk_widget_show(entNewContactsid);
    gtk_box_pack_start(GTK_BOX(hbox7), entNewContactsid, TRUE, TRUE, 0);

    image13 = create_pixmap(AddContacts, NULL);
    gtk_widget_show(image13);
    gtk_box_pack_start(GTK_BOX(hbox7), image13, TRUE, TRUE, 0);

    label16 = gtk_label_new("");
    gtk_widget_show(label16);
    gtk_box_pack_start(GTK_BOX(vbox6), label16, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL(label16), GTK_JUSTIFY_LEFT);

    dialog_action_area3 = gtk_dialog_get_action_area(GTK_DIALOG(AddContacts));
    gtk_widget_show(dialog_action_area3);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area3), GTK_BUTTONBOX_SPREAD);

    butAddContactsCancel = gtk_button_new_with_label("gtk-cancel");
    gtk_widget_show(butAddContactsCancel);
    gtk_widget_set_can_default(butAddContactsCancel, TRUE);
    gtk_box_pack_start(GTK_BOX(dialog_action_area3), butAddContactsCancel, FALSE, FALSE, 0);

    butAddContactsOk = gtk_button_new_with_label("gtk-ok");
    gtk_widget_show(butAddContactsOk);
    gtk_widget_set_can_default(butAddContactsOk, TRUE);
    gtk_box_pack_start(GTK_BOX(dialog_action_area3), butAddContactsOk, FALSE, FALSE, 0);

    g_signal_connect(butAddContactsCancel, "clicked", G_CALLBACK(on_butAddContactsCancel_clicked), NULL);
    g_signal_connect(butAddContactsOk, "clicked", G_CALLBACK(on_butAddContactsOk_clicked), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(AddContacts, AddContacts, "AddContacts");
    GLADE_HOOKUP_OBJECT_NO_REF(AddContacts, dialog_vbox3, "dialog_vbox3");
    GLADE_HOOKUP_OBJECT(AddContacts, vbox6, "vbox6");
    GLADE_HOOKUP_OBJECT(AddContacts, label15, "label15");
    GLADE_HOOKUP_OBJECT(AddContacts, hbox7, "hbox7");
    GLADE_HOOKUP_OBJECT(AddContacts, image11, "image11");
    GLADE_HOOKUP_OBJECT(AddContacts, label14, "label14");
    GLADE_HOOKUP_OBJECT(AddContacts, image12, "image12");
    GLADE_HOOKUP_OBJECT(AddContacts, entNewContactsid, "entNewContactsid");
    GLADE_HOOKUP_OBJECT(AddContacts, image13, "image13");
    GLADE_HOOKUP_OBJECT(AddContacts, label16, "label16");
    GLADE_HOOKUP_OBJECT_NO_REF(AddContacts, dialog_action_area3, "dialog_action_area3");
    GLADE_HOOKUP_OBJECT(AddContacts, butAddContactsCancel, "butAddContactsCancel");
    GLADE_HOOKUP_OBJECT(AddContacts, butAddContactsOk, "butAddContactsOk");

    return AddContacts;
}

GtkWidget *create_Offline(void)
{
    GtkWidget *Offline;
    GtkWidget *dialog_vbox4;
    GtkWidget *dialog_action_area4;
    GtkWidget *butOfflineDelete;
    GtkWidget *butOfflineClose;

    Offline = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(Offline), "gchat - Offline Messages");
    gtk_widget_set_size_request(Offline, 500, 250);
    gtk_window_set_default_size(GTK_WINDOW(Offline), 350, 200);
    gtk_window_set_resizable(GTK_WINDOW(Offline), FALSE);

    dialog_vbox4 = gtk_dialog_get_content_area(GTK_DIALOG(Offline));
    gtk_widget_show(dialog_vbox4);

    scrolledwindow4 = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_show(scrolledwindow4);
    gtk_box_pack_start(GTK_BOX(dialog_vbox4), scrolledwindow4, TRUE, TRUE, 0);

    dialog_action_area4 = gtk_dialog_get_action_area(GTK_DIALOG(Offline));
    gtk_widget_show(dialog_action_area4);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area4), GTK_BUTTONBOX_EDGE);

    butOfflineDelete = gtk_button_new_with_label("gtk-delete");
    gtk_widget_show(butOfflineDelete);
    gtk_widget_set_sensitive(butOfflineDelete, FALSE);
    gtk_widget_set_can_default(butOfflineDelete, TRUE);
    gtk_box_pack_start(GTK_BOX(dialog_action_area4), butOfflineDelete, FALSE, FALSE, 0);

    butOfflineClose = gtk_button_new_with_label("gtk-close");
    gtk_widget_show(butOfflineClose);
    gtk_widget_set_can_default(butOfflineClose, TRUE);
    gtk_box_pack_start(GTK_BOX(dialog_action_area4), butOfflineClose, FALSE, FALSE, 0);

    g_signal_connect(butOfflineDelete, "clicked", G_CALLBACK(on_butOfflineDelete_clicked), NULL);
    g_signal_connect(butOfflineClose, "clicked", G_CALLBACK(on_butOfflineClose_clicked), NULL);

    /* Store pointers to all widgets, for use by lookup_widget(). */
    GLADE_HOOKUP_OBJECT_NO_REF(Offline, Offline, "Offline");
    GLADE_HOOKUP_OBJECT_NO_REF(Offline, dialog_vbox4, "dialog_vbox4");
    GLADE_HOOKUP_OBJECT(Offline, scrolledwindow4, "scrolledwindow4");
    GLADE_HOOKUP_OBJECT_NO_REF(Offline, dialog_action_area4, "dialog_action_area4");
    GLADE_HOOKUP_OBJECT(Offline, butOfflineDelete, "butOfflineDelete");
    GLADE_HOOKUP_OBJECT(Offline, butOfflineClose, "butOfflineClose");

    return Offline;
}
