#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <time.h>



#define SERVER_PORT 9028

void gif_handle_client(int);
void gif_send_clients_contact_list(char *, int, int);
char *gif_get_system_time();
