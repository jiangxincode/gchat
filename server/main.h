/**
 *   main.h
 *   include all the common header files
 *   use "lphread" for the link of pthread library
 */


void gif_handle_client(void *client);
void gif_send_clients_contact_list(char *, int, int);
char *get_system_time();
int get_full_path_name(char *full_path_name, char *id_path, char *filename);
int user_handle();
int add_user();
int display_user();
int delete_user();
int empty(const char *filename);
int user_exist(const char *filename, const char *new_user);

#define _DEBUG(msg...) printf("[ %s\n%s\n%d\n]=>",__FILE__, __FUNCTION__, __LINE__); perror(msg)
#define _ERROR(msg...) printf("[ %s\n%d]=>", __FILE__,  __LINE__); perror(msg)
