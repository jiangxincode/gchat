#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "gif_defns.c"

int main()
{
	int ch;
	FILE *fp;
	users_t usr;

	while(1)
	{
		printf("Select an option :\n");
		printf("\t1 . Add\n");
		printf("\t2 . Display\n");
		printf("\t3 . Delete\n");
		printf("\t4 . Exit\n");
		printf("Enter ur choice : ");
		scanf("%d", &ch);
		switch(ch)
		{
			case 1:
			{
				char filename[30];
				printf("Enter the name to be added : ");
				scanf("%s",usr.loginid);
				printf("Enter the password : ");
				scanf("%s",usr.password);
				fp = fopen("users.db","a");
				fwrite(&usr, sizeof(users_t), 1, fp);
				fclose(fp);
	
				strcpy(filename, usr.loginid);
				strcat(filename, ".db");
				// creating a file for the new user for storing his contacts list.
				fp = fopen(filename, "w");
				fclose(fp);

				strcpy(filename, usr.loginid);
				strcat(filename, "_as.db");
				// creating a file for the new user for storing the list of users who has this user as their contact.
				fp = fopen(filename, "w");
				fclose(fp);

				strcpy(filename, usr.loginid);
				strcat(filename, "_off.db");
				fp = fopen(filename, "w");
				fclose(fp);

				break;
			}
			case 2:
			{
				printf("\nAvailable Names :");
				fp = fopen("users.db","r");
				while((fread(&usr, sizeof(users_t), 1, fp)) == 1)
					printf("\n%s",usr.loginid);
				fclose(fp);
				printf("\n");
				break;
			}
			case 3:
			{
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
				break;
			}
			case 4:
				exit(0);
			default:
				printf("Sorry. Enter the correct choice.\n");
		}
	}
}


