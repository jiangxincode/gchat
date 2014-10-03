#include "main.h"
#include "user_handle.h"
#include "gif_defns.h"

extern char *path;
extern char pathname[MAX_PATH_LENGTH];

int add_user()
{
	FILE *fp;
	users_t usr;
	printf("Enter the name to be added : ");
	scanf("%s",usr.loginid);
	printf("Enter the password : ");
	scanf("%s",usr.password);

	strcpy(pathname,path);
	strcat(pathname,"users.db");

	fp = fopen(pathname,"a");
	fwrite(&usr, sizeof(users_t), 1, fp);
	fclose(fp);

	strcpy(pathname,path);
	strcat(pathname, usr.loginid);
	strcat(pathname, ".db");

	// creating a file for the new user for storing his contacts list.
	fp = fopen(pathname, "w");
	fclose(fp);

	strcpy(pathname,path);
	strcat(pathname,usr.loginid);
	strcat(pathname, "_as.db");

	// creating a file for the new user for storing the list of users who has this user as their contact.
	fp = fopen(pathname, "w");
	fclose(fp);

	strcpy(pathname,path);
	strcat(pathname,usr.loginid);
	strcat(pathname, "_off.db");

	fp = fopen(pathname, "w");
	fclose(fp);
	return 0;
}

int display_user()
{
	FILE *fp;
	users_t usr;
	printf("\nAvailable Names :");
	fp = fopen("users.db","r");
	while((fread(&usr, sizeof(users_t), 1, fp)) == 1)
		printf("\n%s",usr.loginid);
	fclose(fp);
	printf("\n");
	return 0;
}

int delete_user()
{
	FILE *fp;
	users_t usr;
	FILE *fp1;
	char name[20], filename[25];
	int flag = 0;
	printf("Enter the name to be deleted : ");
	scanf("%s", name);
	fp = fopen("users.db", "r");
	fp1 = fopen("tempfile.db", "w");
	rewind(fp);
	while((fread(&usr, sizeof(users_t), 1, fp)) == 1)
	{
		if((strcmp(usr.loginid, name)) != 0)
			fwrite(&usr, sizeof(users_t), 1, fp1);
		else
			flag = 1;
	}
	fclose(fp1);
	fclose(fp);
	remove("users.db");
	rename("tempfile.db", "users.db");
	if(flag ==1)
	{
		strcpy(filename, name);
		strcat(filename, ".db");
		remove(filename);

		strcpy(filename, name);
		strcat(filename, "_as.db");
		remove(filename);

		strcpy(filename, name);
		strcat(filename, "_off.db");
		remove(filename);

		printf("Deleted!!\n");
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
		printf("Enter your choice : ");
		scanf("%d", &ch);
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
			printf("Sorry. Enter the correct choice.\n");
		}
	}
}

