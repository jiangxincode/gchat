#include "../common/common.h"
#include "support.h"
#include "callbacks.h"
#include "interface.h"

GtkWidget *gifmain;

int main (int argc, char *argv[])
{
        gdk_threads_init();
        gnome_program_init ("gchat", "0.01", LIBGNOMEUI_MODULE, argc, argv, GNOME_PARAM_NONE);
        create_gifmain (); //对gifmain进行初始化
        gtk_signal_connect (GTK_OBJECT (gifmain), "delete_event", G_CALLBACK (delete_event), NULL);
        gdk_threads_enter();
        gtk_main ();
        gdk_threads_leave();
        return 0;
}

