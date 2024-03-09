#ifndef MAIN_H
#define MAIN_H

void gif_handle_client(void *client);
void gif_send_clients_contact_list(char *, int, int);
char *get_system_time();
int user_handle();
int add_user();
int display_user();
int delete_user();
int empty(const char *filename);
int user_exist(const char *filename, const char *new_user);
int password(char *dest);
char *get_passwd(char *passwd);

#endif // MAIN_H
