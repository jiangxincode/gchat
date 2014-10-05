#include "common.h"

void display(gifhdr_t header)
{
        fputs("The HEADER information is: \n",stdout);
        printf("%u\n",header.length);
        printf("%s\n",header.receiver);
        printf("%s\n",header.sender);
        switch(header.type)
        {
                case GIF_LOGIN_MSG: fputs("GIF_LOGIN_MSG",stdout); break; //用户登录
                case GIF_CALL_MSG: fputs("GIF_CALL_MSG",stdout); break; //用户呼叫
                case GIF_ADDRLIST_MSG: fputs("GIF_ADDRLIST_MSG",stdout); break; //更新联系人列表信息
                case GIF_ADD_CONTACTS_MSG: fputs("GIF_ADD_CONTACTS_MSG",stdout); break; //添加连接信息
                case GIF_DELETE_CONTACTS_MSG: fputs("GIF_DELETE_CONTACTS_MSG",stdout); break; //删除连接信息
                case GIF_CHAT_MSG: fputs("GIF_CHAT_MSG",stdout); break; //聊天信息
                case GIF_DISCONNECT_MSG: fputs("GIF_DISCONNECT_MSG",stdout); break; //用户下线信息
                case GIF_SUCCESS_N_ERROR_MSG: fputs("GIF_SUCCESS_N_ERROR_MSG",stdout); break; //消息请求成功或失败信息
                case GIF_OFFLINE_REQUEST_MSG: fputs("GIF_OFFLINE_REQUEST_MSG",stdout); break; //发送离线消息
                case GIF_OFFLINE_MSG: fputs("GIF_OFFLINE_MSG",stdout); break; //离线消息
                case GIF_OFFLINE_DELETE_MSG: fputs("GIF_OFFLINE_DELETE_MSG",stdout); break; //删除离线消息
                default: fputs("Unknown",stdout); break;
        }
        printf("%u\n",header.reserved);
}
