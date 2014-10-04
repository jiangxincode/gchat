#include "main.h"
#include "gif_defns.h"
#include "main.h"

char pathname[MAX_PATH_LENGTH];

char *gif_get_system_time()
{
	time_t *cal_t = 0;
	struct tm *split_t;
	char *sys_date;

	// setting the system date and time to the char pointer variable 'dateserial'(DD MMM YYYY  HH:MM)
	sys_date = (char *) malloc(20);
	time(cal_t);
	split_t = localtime(cal_t);
	strftime(sys_date, 20, "%d %b %Y  %H:%M", split_t);
	return sys_date;
}

int get_full_path_name(char *full_path_name, char *id_path, char *filename)
{
        char *prefix = "db/";
        strcpy(full_path_name,prefix);
        if(id_path != NULL)
        {
                strcat(full_path_name,id_path);
        }
        strcat(full_path_name, filename);
        return 0;
}

/**
检查filename所指定等文件中是否有用户new_user.
如果文件存在该用户，则返回1,
如果new_user为NULL,且文件存在某一用户，则返回1,
如果文件不存在任何用户，则返回0,
其它任何情况均返回-1.
(包括文件不存在，权限不足等)
*/
int is_exist(const char *filename, const char *new_user)
{
        users_t usr;
        FILE *fp;

        if((fp=fopen(filename,"r")) == NULL) //打开错误
        {
                return -1;
        }
        if((fread(&usr, sizeof(users_t),1,fp)) == 0) //无任何用户
        {
                return 0;
        }
        rewind(fp);
        while((fread(&usr, sizeof(users_t),1,fp)) == 1)
        {
                if(new_user == NULL)
                {
                        return 1;
                }
                if(strcmp(usr.loginid,new_user) == 0) //存在该用户
                {
                        return 1;
                }
        }
        return -1;
}
