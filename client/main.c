#include "../common/common.h"
#include "support.h"
#include "callbacks.h"
#include "interface.h"

extern GtkWidget *gifmain;

int main (int argc, char *argv[])
{
        GtkWidget *gifclient;

        gdk_threads_init();

        gnome_program_init ("gchat", "0.01", LIBGNOMEUI_MODULE, argc, argv, GNOME_PARAM_NONE);

        gifclient = create_gifclient ();
        gtk_widget_show (gifclient);

        gifmain = gifclient;

        gtk_signal_connect (GTK_OBJECT (gifclient), "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);

        gdk_threads_enter();
        gtk_main ();
        gdk_threads_leave();

        return 0;
}

