#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include <gnome.h>

#include <ncurses.h>

// gchat macros
//


#define MAX_PATH_LENGTH (sysconf(_PC_PATH_MAX)+sysconf(_PC_NAME_MAX)) //the max length of the pathname(filename included)
#define SERVER_PORT 9028
#define COMMON_LENGTH 20
#define BUFF_SIZE 1024
#define COMMON_PATH_PREFIX "/home/jiangxin/temp/gchat/"


#define GIF_LOGIN_MSG 1			//用户登录
#define GIF_CALL_MSG 2			//用户呼叫
#define GIF_ADDRLIST_MSG 3		//更新联系人列表信息
#define GIF_ADD_CONTACTS_MSG 4		//添加连接信息
#define GIF_DELETE_CONTACTS_MSG 5	//删除连接信息
#define GIF_CHAT_MSG 6			//聊天信息
#define GIF_DISCONNECT_MSG 7		//用户下线信息
#define GIF_SUCCESS_N_ERROR_MSG 8	//消息请求成功或失败信息
#define GIF_OFFLINE_REQUEST_MSG 9	//发送离线消息
#define GIF_OFFLINE_MSG 10		//离线消息
#define GIF_OFFLINE_DELETE_MSG 11	//删除离线消息

// macros user with Success and Error messages
#define GIF_ERROR_LOGIN_INCORRECT 101		//错误的登录信息
#define GIF_SUCCESS_ADD_CONTACTS 102		//添加连接信息成功
#define GIF_ERROR_ADD_CONTACTS 103		//添加连接信息错误
#define GIF_SUCCESS_DELETE_CONTACTS 104		//删除连接信息成功
#define GIF_ERROR_DELETE_CONTACTS_NOT_A_CONTACT 105	//用户离线
#define GIF_ERROR_DELETE_CONTACTS_NOT_A_MEMBER 106	//无指定用户

// ginterface header
//
typedef struct _gifhdr_t
{
	unsigned int type;	//消息类型
	unsigned int length;	//消息的长度，以字节为单位
	char sender[COMMON_LENGTH];	//发送方
	char receiver[COMMON_LENGTH];	//接收方
	unsigned int reserved;
}gifhdr_t;


// ginterface, user profile structure
//
typedef struct _users_t
{
	char loginid[COMMON_LENGTH];	//用户名
	char password[COMMON_LENGTH];	//密码
}users_t;


// ginterface, online users profile structure
//
typedef struct _online_users_t
{
	char loginid[COMMON_LENGTH];	//用户名
	int sockfd;		//在线用户客户机的套接字
}online_users_t;


// ginterface, user contacts file structure
//
typedef struct _user_contacts_t
{
	char loginid[COMMON_LENGTH];
}user_contacts_t;


// ginterface, user status structure
//
typedef struct _user_status_t
{
	char loginid[COMMON_LENGTH];
	unsigned int status;
}user_status_t;


// ginterface, offline messages structure used while storing the messages in the file
//
typedef struct _offline_msgs_t
{
	char sender[COMMON_LENGTH];
	char dateserial[COMMON_LENGTH];
	unsigned int new;
	char message[BUFF_SIZE];
}offline_msgs_t;


// ginterface, structure used while sending the offline messages
//
typedef struct _offline_msgs_send_t
{
	char sender[COMMON_LENGTH];
	char dateserial[COMMON_LENGTH];
	unsigned int new;
	unsigned int length;
}offline_msgs_send_t;


// ginterface, the structure used in the linked list to maintain the individual contact person's window id
//
typedef struct _contacts_chat_window_id_t
{
        char contacts_name[COMMON_LENGTH];
        unsigned int window_id;
        unsigned int window_open;	//used as boolean variable(opened or not)
        struct _contacts_chat_window_id_t *next;
} contacts_chat_window_id_t;

#define HEADER_LENGTH (sizeof(gifhdr_t)) //the length of the gif_hdr_t
#define BODY_LENGTH (BUFF_SIZE-HEADER_LENGTH)

#define _DEBUG(msg...) printf("\nFILE: %s\nFUNCTION: %s\nLINE: %d\n" ,__FILE__, __FUNCTION__, __LINE__); perror(msg)
#define _ERROR(msg...) printf("\nFILE: %s\nFUNCTION: %s\nLINE: %d\n" ,__FILE__, __FUNCTION__, __LINE__); perror(msg)

void display_header(gifhdr_t *gifheader);
void display(char *gifbuffer);
int get_full_path_name(char *full_path_name, char *filename, int num, ...);
