#include "../common/common.h"
#include "main.h"

char *get_system_time()
{
    time_t *cal_t = 0;
    struct tm *split_t;
    char *sys_date;

    // setting the system date and time to the char pointer variable 'dateserial'(DD MMM YYYY  HH:MM)
    sys_date = (char *)malloc(20);
    time(cal_t);
    split_t = localtime(cal_t);
    strftime(sys_date, 20, "%d %b %Y  %H:%M", split_t);
    return sys_date;
}

// type = 0 (sending the contacts list while normal login)
// type = 1(sending the contacts list while refresing)
void gif_send_clients_contact_list(char *client_loginid, int client_sockfd, int type)
{
    char pathname[MAX_PATH_LENGTH];
    gifhdr_t *gifheaderS;
    char *gifdataS, *gifbufferS;
    online_users_t ousr;
    user_contacts_t usrc;
    user_status_t *usrs;
    FILE *contactsfp, *onlinefp;
    int i;

    gifheaderS = (gifhdr_t *)malloc(sizeof(gifhdr_t));
    gifheaderS->type = GIF_ADDRLIST_MSG;
    strcpy(gifheaderS->sender, "server");
    strcpy(gifheaderS->receiver, client_loginid);
    gifheaderS->reserved = 0;
    if (type == 1)
        gifheaderS->reserved = 1;

    gifdataS = (char *)malloc(BODY_LENGTH);
    gifbufferS = (char *)malloc(BUFF_SIZE);

    // 设置该用户所有联系人是否在线
    get_full_path_name(pathname, ".db", 2, "server/db/", client_loginid);
    contactsfp = fopen(pathname, "r");
    if (contactsfp == NULL)
    {
        _DEBUG("A user's contacts file");
        pthread_exit(NULL);
    }

    get_full_path_name(pathname, "online.db", 1, "server/db/");
    onlinefp = fopen(pathname, "r");
    if (onlinefp == NULL) // 如果没有发生异常，此处一定能够打开成功
    {
        _DEBUG("online.db");
        pthread_exit(NULL);
    }
    i = 0;
    usrs = (user_status_t *)malloc(sizeof(user_status_t));
    while ((fread(&usrc, sizeof(user_contacts_t), 1, contactsfp)) == 1)
    {
        strcpy(usrs->loginid, usrc.loginid);
        // initialising status of the user to be offline
        usrs->status = 0;
        rewind(onlinefp);
        while ((fread(&ousr, sizeof(online_users_t), 1, onlinefp)) == 1)
        {
            if ((strcmp(ousr.loginid, usrs->loginid)) == 0)
                usrs->status = 1; // setting status to online
            else
                continue;
        }

        memcpy((gifdataS + (i * sizeof(user_status_t))), usrs, sizeof(user_status_t));
        i++;
    }
    free(usrs);
    fclose(onlinefp);
    fclose(contactsfp);

    gifheaderS->length = (i * (sizeof(user_status_t))); // mark 有可能导致为0
    memcpy(gifbufferS, gifheaderS, HEADER_LENGTH);
    memcpy((gifbufferS + HEADER_LENGTH), gifdataS, gifheaderS->length);

    if ((send(client_sockfd, gifbufferS, (HEADER_LENGTH + gifheaderS->length), 0)) < 0)
    {
        _DEBUG("Error sending message(Address list)");
        pthread_exit(NULL);
    }

    free(gifbufferS);
    free(gifdataS);
    free(gifheaderS);
}

/**
无回显获得密码并显示“*”的子函数
函数返回密码的长度，不包括null
*/
int password(char *dest)
{
    char e = '*';
    int i;
    for (i = 0; i < COMMON_LENGTH; i++)
    {
        dest[i] = getch();
        if (dest[i] == '\n')
        {
            break;
        }
        addch(e);
    }
    dest[i] = '\0';
    return i;
}

char *get_passwd(char *passwd)
{
    int i;
    int pw_length;
    char passwd_tmp[COMMON_LENGTH];

    initscr();
    cbreak();
    nl();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    refresh();

    while (1)
    {
        printw("Input the password: ");
        pw_length = password(passwd);
        for (i = 0; i <= pw_length; i++)
        {
            passwd_tmp[i] = passwd[i]; // null include
        }
        printw("\nInput the password again: ");
        pw_length = password(passwd);
        if ((strcmp(passwd, passwd_tmp)) == 0)
        {
            break;
        }
        else
        {
            continue;
        }
        refresh();
        getchar();
    }
    endwin();
    return passwd;
}
