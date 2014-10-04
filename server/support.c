#include "main.h"
#include "gif_defns.h"
#include "main.h"

char pathname[MAX_PATH_LENGTH];

char *get_system_time()
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
