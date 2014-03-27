
// gchat macros
//
#define GIF_LOGIN_MSG 1
#define GIF_CALL_MSG 2
#define GIF_ADDRLIST_MSG 3
#define GIF_ADD_CONTACTS_MSG 4
#define GIF_DELETE_CONTACTS_MSG 5
#define GIF_CHAT_MSG 6
#define GIF_DISCONNECT_MSG 7
#define GIF_SUCCESS_N_ERROR_MSG 8
#define GIF_OFFLINE_REQUEST_MSG 9
#define GIF_OFFLINE_MSG 10
#define GIF_OFFLINE_DELETE_MSG 11

// macros user with Success and Error messages
#define GIF_ERROR_LOGIN_INCORRECT 101
#define GIF_SUCCESS_ADD_CONTACTS 102
#define GIF_ERROR_ADD_CONTACTS 103
#define GIF_SUCCESS_DELETE_CONTACTS 104
#define GIF_ERROR_DELETE_CONTACTS_NOT_A_CONTACT 105
#define GIF_ERROR_DELETE_CONTACTS_NOT_A_MEMBER 106



// gchat header
//
typedef struct _gifhdr_t
{
	unsigned int type;
	unsigned int length;
	char sender[10];
	char receiver[10];
	unsigned int reserved;
}gifhdr_t;


// gchat, user status structure
//
typedef struct _user_status_t
{	
	char loginid[20];
	unsigned int status;
}user_status_t;


// gchat, the structure used in the linked list to maintain the individual contact person's window id
//
typedef struct _contacts_chat_window_id_t
{
	char contacts_name[20];
	unsigned int window_id;
	unsigned int window_open;	//used as boolean variable(opened or not)
	struct _contacts_chat_window_id_t *next;
}contacts_chat_window_id_t;


// gchat, structure used while sending the offline messages
//
typedef struct _offline_msgs_send_t
{
	char sender[20];
	char dateserial[20];
	unsigned int new;
	unsigned int length;
}offline_msgs_send_t;

