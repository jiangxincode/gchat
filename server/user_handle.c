#include "main.h"
#include "gif_defns.h"
#include "main.h"

extern char pathname[MAX_PATH_LENGTH];

/**
添加用户
*/
int add_user()
{
	FILE *fp;
	users_t usr;
	printf("Enter the name to be added : \n");
	if((scanf("%s",usr.loginid)) == EOF)
	{
		fprintf(stderr,"No Users\n");
		return 0;
	}
	printf("Enter the password : \n");
	if((scanf("%s",usr.password)) == EOF)
	{
		fprintf(stderr,"No Users\n");
		return 0;
	}

	get_full_path_name(pathname,NULL,"users.db");
	if(is_exist(pathname,usr.loginid) == 1) //用户已存在
        {
                fprintf(stderr,"User exists\n");
                return 0;
        }
	if((fp = fopen(pathname,"a")) == NULL) //打开文件错误
        {
                fprintf(stderr,"%s",strerror(errno));
                return 0;
        }
	fwrite(&usr, sizeof(users_t), 1, fp); //添加用户
	fclose(fp);

	// creating a file for the new user for storing his contacts list.
	get_full_path_name(pathname,usr.loginid,".db");
	fp = fopen(pathname, "w");
	fclose(fp);

	// creating a file for the new user for storing the list of users who has this user as their contact.
	get_full_path_name(pathname,usr.loginid,"_as.db");
	fp = fopen(pathname, "w");
	fclose(fp);

	get_full_path_name(pathname,usr.loginid,"_off.db");
	fp = fopen(pathname, "w");
	fclose(fp);
	return 0;
}

int display_user()
{
	FILE *fp;
	users_t usr;
	get_full_path_name(pathname,NULL,"users.db");

	int temp = is_exist(pathname,NULL);
	if((temp == -1) || (temp == 0))
        {
                fprintf(stderr,"No Users\n");
                return 0;
        }
        fprintf(stderr,"Available Names :\n");
        if((fp=fopen(pathname,"r")) == NULL) //打开错误
        {
                return 0;
        }
	while((fread(&usr, sizeof(users_t), 1, fp)) == 1)
        {
                printf("%s\n",usr.loginid);
        }
	fclose(fp);
	return 0;
}

int delete_user()
{
	FILE *fp;
	users_t usr;
	FILE *fp_temp;
	char name[NAME_LANGTH];
	char pathname_temp[MAX_PATH_LENGTH];
	int flag = 0;
	printf("Enter the name to be deleted : \n");
	if((scanf("%s",name)) == EOF)
	{
		fprintf(stderr,"%s",strerror(errno));
		return 0;
	}
	get_full_path_name(pathname,NULL,"users.db");
	if((fp=fopen(pathname,"r")) == NULL)
        {
                fprintf(stderr,"No Users\n");
                return 0;
        }
	get_full_path_name(pathname_temp,NULL,"tempfile.db");
	if((fp_temp=fopen(pathname_temp,"w"))== NULL)
        {
                fprintf(stderr,"%s",strerror(errno));
                return 0;
        }
	while((fread(&usr, sizeof(users_t), 1, fp)) == 1)
	{
		if((strcmp(usr.loginid, name)) != 0)
			fwrite(&usr, sizeof(users_t), 1, fp_temp);
		else
			flag = 1;
	}
	fclose(fp_temp);
	fclose(fp);
	remove(pathname);
	rename(pathname_temp, pathname);
	if(flag ==1)
	{
		get_full_path_name(pathname,name,".db");
		remove(pathname);

		get_full_path_name(pathname,name,"_as.db");
		remove(pathname);

		get_full_path_name(pathname,name,"_off.db");
		remove(pathname);
		printf("Deleted\n");
	}
	else
		printf("The name you entered does not exist\n");
	return 0;
}
int user_handle()
{
	int ch;
	while(1)
	{
		printf("Select an option :\n");
		printf("\t1 . Add\n");
		printf("\t2 . Display\n");
		printf("\t3 . Delete\n");
		printf("\t4 . Exit\n");
		printf("\t5 . Go on\n");
		printf("Enter your choice : ");
		if((scanf("%d",&ch)) == EOF)
		{
			perror("error: in scanf");
			exit(1);
		}
		switch(ch)
		{
		case 1:
			add_user();
			break;
		case 2:
			display_user();
			break;
		case 3:
			delete_user();
			break;
		case 4:
			exit(0);
		default:
			return 0;
		}
	}
}

