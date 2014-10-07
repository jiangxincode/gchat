#include "../common/common.h"
#include "main.h"
#include "main.h"

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



/**
无回显获得密码并显示“*”的子函数
函数返回密码的长度，不包括null
*/
int  password(char *dest)
{
	char e='*';
	int i;
	for(i=0; i<COMMON_LENGTH; i++)
	{
		dest[i]=getch();
		if(dest[i]=='\n')
		{
			break;
		}
		addch(e);
	}
	dest[i] = '\0';
	return i;
}

char* get_passwd(char *passwd)
{
	int i;
	int pw_length;
	char passwd_tmp[COMMON_LENGTH];

        initscr();
	cbreak();
	nl();
	noecho();
	intrflush(stdscr,FALSE);
	keypad(stdscr,TRUE);
	refresh();

	while(1)
	{
		printw("Input the password: ");
		pw_length=password(passwd);
		for(i=0; i<=pw_length; i++)
		{
			passwd_tmp[i]=passwd[i]; //null include
		}
		printw("\nInput the password again: ");
		pw_length=password(passwd);
		if((strcmp(passwd,passwd_tmp)) == 0)
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

