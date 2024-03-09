#include "../common/common.h"
#include "main.h"

GtkWidget *gifmain;

int main (int argc, char *argv[])
{
        gdk_threads_init();
        gtk_init (&argc, &argv);
        create_gifmain (); //对gifmain进行初始化
        g_signal_connect (G_OBJECT (gifmain), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
        gtk_main ();
        return 0;
}

