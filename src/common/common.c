#include "common.h"

void display_header(gifhdr_t *gifheader)
{
        fputs("The HEADER information is: \n",stdout);
        printf("length: %u\n",gifheader->length);
        printf("receiver: %s\n",gifheader->receiver);
        printf("sender: %s\n",gifheader->sender);
        printf("message type: ");
        switch(gifheader->type)
        {
                case GIF_LOGIN_MSG: fputs("GIF_LOGIN_MSG\n",stdout); break; //用户登录
                case GIF_CALL_MSG: fputs("GIF_CALL_MSG\n",stdout); break; //用户呼叫
                case GIF_ADDRLIST_MSG: fputs("GIF_ADDRLIST_MSG\n",stdout); break; //更新联系人列表信息
                case GIF_ADD_CONTACTS_MSG: fputs("GIF_ADD_CONTACTS_MSG\n",stdout); break; //添加连接信息
                case GIF_DELETE_CONTACTS_MSG: fputs("GIF_DELETE_CONTACTS_MSG\n",stdout); break; //删除连接信息
                case GIF_CHAT_MSG: fputs("GIF_CHAT_MSG\n",stdout); break; //聊天信息
                case GIF_DISCONNECT_MSG: fputs("GIF_DISCONNECT_MSG\n",stdout); break; //用户下线信息
                case GIF_SUCCESS_N_ERROR_MSG: fputs("GIF_SUCCESS_N_ERROR_MSG\n",stdout); break; //消息请求成功或失败信息
                case GIF_OFFLINE_REQUEST_MSG: fputs("GIF_OFFLINE_REQUEST_MSG\n",stdout); break; //发送离线消息
                case GIF_OFFLINE_MSG: fputs("GIF_OFFLINE_MSG\n",stdout); break; //离线消息
                case GIF_OFFLINE_DELETE_MSG: fputs("GIF_OFFLINE_DELETE_MSG\n",stdout); break; //删除离线消息
                default: fputs("Unknown\n",stdout); break;
        }
        printf("reserved: %u\n",gifheader->reserved);
}

void display(char *gifbuffer)
{
        gifhdr_t *gifheader;
        gifheader = (gifhdr_t *)malloc(HEADER_LENGTH);
        memcpy(gifheader,gifbuffer,HEADER_LENGTH);
        display_header(gifheader);
        fprintf(stderr,"%s\n",(gifbuffer+HEADER_LENGTH));
}

/**
根据参数设置得到文件的完整路径名
full_path_name: 必须保证拥有足够等长度来存储路径名
filename: for example:"users.db"
num: 附加后缀的数目
...: 附加后缀列表，必须按续给出
for example:
        filename: users.db
        num: 3
        ...: server/   db/    friends/
        result: $(APP_PATH)/server/db/friends/users.db
$(APP_PATH): /home/jiangxin/temp/gchat/
*/
int get_full_path_name(char *full_path_name, char *filename, int num, ...)
{
        va_list ap;
        int ptr_no = 0;

        char *prefix = COMMON_PATH_PREFIX;
        strcpy(full_path_name,prefix);

        va_start(ap, num);
        while(ptr_no < num)
        {
                strcat(full_path_name, va_arg(ap, char *));
                ptr_no ++;
        }
        va_end(ap);

        strcat(full_path_name,filename);
        return 0;
}
