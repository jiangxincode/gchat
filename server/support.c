#include "gif_defns.h"
#include "support.h"

void gif_handle_client(int client_sockfd)
{
	gifhdr_t *gifheader;
	int rcv_status;
	char *gifdata, *gifbuffer;
	char loginid[20], password[20];
	char *path = "db/";
	char pathname[MAX_PATH_LENGTH];

	pthread_t pthd = pthread_self(); //获取自身等线程ID

	while(1)
	{
		gifbuffer = (char *) malloc(1024);
		rcv_status = recv(client_sockfd, gifbuffer, 1024, 0);

		if(rcv_status == -1)
		{
			perror("error: recv");
			pthread_cancel(pthd);
			return;
		}
		else if(rcv_status == 0)
		{
			printf("%s disconnected.\n", loginid); //mark，此时还没有获得loginid
			pthread_cancel(pthd);
			return;
		}
		gifheader = (gifhdr_t *) malloc(sizeof(gifhdr_t));
		memcpy(gifheader, gifbuffer, HEADER_LENGTH);
		if((gifheader->length) > 0)
		{
			gifdata = (char *) malloc(gifheader->length);
			memcpy(gifdata , (gifbuffer + HEADER_LENGTH), gifheader->length);
		}

		switch(gifheader->type)
		{
		/** S means 'Send'.
		 * The datatypes sufficed with 'S' related to send function
		 */
		case GIF_LOGIN_MSG:
		{
			users_t usr;
			gifhdr_t *gifheaderS;
			int loginlength = 0, passlength = 0, flag = 0;
			char *ptr, *gifbufferS;
			FILE *usersfp;

			strcpy(pathname,path);
			strcat(pathname,"users.db");
			usersfp = fopen(pathname,"r");

			if(usersfp == NULL)
			{
				perror("error: open users.db");
				pthread_cancel(pthd);
				return;
			}

			ptr = (char *)strstr(gifdata, "#*&");
			loginlength = ptr - gifdata; //获取loginid的长度
			ptr = ptr + 3;
			while(*ptr != '\0')
			{
				passlength++; //获取password的长度
				ptr++;
			}

			strncpy(loginid, gifdata, loginlength);
			strncpy(password, (gifdata + loginlength + 3), passlength);
			*(loginid + loginlength) = '\0';
			*(password + passlength) = '\0';

			if((gifheader->length) != 0)
			{
				free(gifdata);
			}
			free(gifheader);
			free(gifbuffer);

			while((fread(&usr, sizeof(users_t), 1, usersfp)) == 1) //读取users.db，匹配用户名和密码
			{
				if(((strcmp(usr.loginid,loginid)) == 0) && ((strcmp(usr.password,password)) == 0))
					flag = 1;
				else
					continue;
			}
			fclose(usersfp);

			if(flag == 1) //用户名和密码匹配
			{
				online_users_t ousr;
				user_contacts_t usrc;
				FILE *onlinefp, *as_contactfp;
				//char filename[30]; //mark

				printf("%s - Login Correct\n", loginid);

				strcpy(pathname,path);
				strcat(pathname,"online.db");
				onlinefp = fopen(pathname, "a+");
				if(onlinefp == NULL)
				{
					perror("error: open online.db");
					pthread_cancel(pthd);
					return;
				}

				strcpy(ousr.loginid, loginid);
				ousr.sockfd = client_sockfd;

				// adding the name to the online users list file
				fwrite(&ousr, sizeof(online_users_t), 1, onlinefp);

				// funcion for sending the contacts list to the client
				gif_send_clients_contact_list(loginid, client_sockfd, 0);

				// coding for refresing the contacts list of clients who has this just logined client as a contact
				strcpy(pathname,path);
				strcat(pathname, loginid);
				strcat(pathname, "_as.db");
				as_contactfp = fopen(pathname, "r");
				if(as_contactfp == NULL)
				{
					perror("error: _as.db");
					pthread_cancel(pthd);
					return;
				}

				/**检查以该登录用户为联系人的用户是否在线，如果在，则向其更新在线联系人信息*/
				while((fread(&usrc, sizeof(user_contacts_t), 1, as_contactfp)) == 1)
				{
					rewind(onlinefp);
					while((fread(&ousr, sizeof(online_users_t), 1, onlinefp)) == 1)
					{
						if((strcmp(usrc.loginid, ousr.loginid)) == 0)
						{
							gif_send_clients_contact_list(ousr.loginid, ousr.sockfd, 1);
						}
					}
				}
				fclose(as_contactfp);
				fclose(onlinefp);
			}
			else //用户名或密码不匹配
			{
				printf("%s : %s - Login Incorrect\n", loginid, password);

				// unauthorised person
				gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
				gifheaderS->type = GIF_SUCCESS_N_ERROR_MSG;
				strcpy(gifheaderS->sender, "server");
				strcpy(gifheaderS->receiver, loginid);
				gifheaderS->length = 0;
				gifheaderS->reserved = GIF_ERROR_LOGIN_INCORRECT;

				gifbufferS = (char *) malloc(sizeof(gifhdr_t));
				memcpy(gifbufferS, gifheaderS, sizeof(gifhdr_t));

				if((send(client_sockfd, gifbufferS, (HEADER_LENGTH + gifheaderS->length), 0)) < 0)
				{
					perror("error:send GIF_SUCCESS_N_ERROR_MSG");
				}

				free(gifbufferS);
				free(gifheaderS);

				pthread_cancel(pthd);
			}
			break;
		}

		case GIF_ADD_CONTACTS_MSG:
		{
			users_t usr;
			user_contacts_t usrc;
			FILE *contactsfp, *usersfp, *as_contactfp;
			gifhdr_t *gifheaderS;
			char *gifbufferS;
			//char filename[25]; //mark
			int flag = 0;

			strcpy(pathname,path);
			strcpy(pathname, gifheader->sender);
			strcat(pathname, ".db");
			contactsfp = fopen(pathname, "a");
			if(contactsfp == NULL)
			{
				perror("error: open .db");
				pthread_cancel(pthd);
				return;
			}

			usersfp = fopen("users.db", "r");
			if(usersfp == NULL)
			{
				perror("error:open users.db");
				pthread_cancel(pthd);
				return;
			}

			while((fread(&usr, sizeof(users_t), 1, usersfp)) == 1)
			{
				if((strcmp(usr.loginid, gifdata)) == 0)
					flag = 1;
				else
					continue;
			}
			fclose(usersfp);

			if(flag == 1) //在数据库中找到该登录用户想要添加为联系人等用户
			{
				// making entry in the client's contacts file who added
				strcpy(usrc.loginid, gifdata);
				fwrite(&usrc, sizeof(user_contacts_t), 1, contactsfp);
				fclose(contactsfp);

				// making entry in the client's as_contact file whose name is added
				strcpy(pathname, path);
				strcat(pathname, gifdata);
				strcat(pathname, "_as.db");
				as_contactfp = fopen(pathname, "a");
				if(as_contactfp == NULL)
				{
					perror("error: open _as.db");
					pthread_cancel(pthd);
					return;
				}
				strcpy(usrc.loginid, gifheader->sender);
				fwrite(&usrc, sizeof(user_contacts_t), 1, as_contactfp);
				fclose(as_contactfp);

				gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
				gifheaderS->type = GIF_SUCCESS_N_ERROR_MSG;
				strcpy(gifheaderS->sender, "server");
				strcpy(gifheaderS->receiver, gifheader->sender);
				gifheaderS->length = 0;
				gifheaderS->reserved = GIF_SUCCESS_ADD_CONTACTS;

				gifbufferS = (char *) malloc(sizeof(gifhdr_t));
				memcpy(gifbufferS, gifheaderS, sizeof(gifhdr_t));

			}
			else //在数据库中没有找到该登录用户想要添加为联系人等用户
			{
				gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
				gifheaderS->type = GIF_SUCCESS_N_ERROR_MSG;
				strcpy(gifheaderS->sender, "server");
				strcpy(gifheaderS->receiver, gifheader->sender);
				gifheaderS->length = 0;
				gifheaderS->reserved = GIF_ERROR_ADD_CONTACTS;

				gifbufferS = (char *) malloc(sizeof(gifhdr_t));
				memcpy(gifbufferS, gifheaderS, sizeof(gifhdr_t));
			}

			if((send(client_sockfd, gifbufferS, (HEADER_LENGTH + gifheaderS->length), 0)) < 0)
			{
				perror("error: send GIF_SUCCESS_N_ERROR_MSG");
			}

			free(gifbufferS);
			free(gifheaderS);

			if(flag == 1)
				gif_send_clients_contact_list(gifheader->sender, client_sockfd, 1); //mark应该还要给所添加的用户更新视图

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}

		case GIF_DELETE_CONTACTS_MSG:
		{
			users_t usr;
			user_contacts_t usrc;
			FILE *contactsfp, *usersfp, *as_contactfp, *newfp;
			gifhdr_t *gifheaderS;
			char *gifbufferS;
			int flag = 0;

			char pathname_temp[MAX_PATH_LENGTH];

			strcpy(pathname,path);
			strcat(pathname,"users.db");
			usersfp = fopen(pathname, "r");
			if(usersfp == NULL)
			{
				perror("A user's contact file");
				pthread_cancel(pthd);
				return;
			}

			while((fread(&usr, sizeof(users_t), 1, usersfp)) == 1)
			{
				if((strcmp(usr.loginid, gifdata)) == 0)
					flag = 1;
				else
					continue;
			}
			fclose(usersfp);

			strcpy(pathname,path);
			strcat(pathname, gifheader->sender);
			strcat(pathname, ".db");
			contactsfp = fopen(pathname, "r");
			if(contactsfp == NULL)
			{
				perror("A user's contact file");
				pthread_cancel(pthd);
				return;
			}

			if(flag == 1) //要删除等联系人在用户数据库中
			{
				int localflag = 0;
				// removing entry in the client's contacts file

				strcpy(pathname_temp,path);
				strcat(pathname_temp,"contacts_delete_temp.db");
				newfp = fopen(pathname_temp, "w");
				while(fread(&usrc, sizeof(user_contacts_t), 1, contactsfp) == 1)
				{
					if(strcmp(usrc.loginid, gifdata) != 0)
						fwrite(&usrc, sizeof(user_contacts_t), 1, newfp);
					else
						localflag = 1;
				}
				fclose(newfp);
				fclose(contactsfp);
				remove(pathname);
				rename(pathname_temp, pathname);

				// removing entry in the client's as_contact file whose name is deleted
				strcpy(pathname,path);
				strcat(pathname, gifdata);
				strcat(pathname, "_as.db");
				as_contactfp = fopen(pathname, "r");
				if(as_contactfp == NULL)
				{
					perror("A user's as_contact file");
					exit(0);
				}

				strcpy(pathname_temp,path);
				strcat(pathname_temp,"as_contacts_delete_temp.db");
				newfp = fopen(pathname_temp, "w");
				while(fread(&usrc, sizeof(user_contacts_t), 1, as_contactfp) == 1)
				{
					if(strcmp(usrc.loginid, gifheader->sender) != 0)
						fwrite(&usrc, sizeof(user_contacts_t), 1, newfp);
				}
				fclose(newfp);
				fclose(as_contactfp);
				remove(pathname);
				rename(pathname_temp, pathname);

				gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
				gifheaderS->type = GIF_SUCCESS_N_ERROR_MSG;
				strcpy(gifheaderS->sender, "server");
				strcpy(gifheaderS->receiver, gifheader->sender);
				gifheaderS->length = 0;

				if(localflag == 1)
					gifheaderS->reserved = GIF_SUCCESS_DELETE_CONTACTS;
				else
					gifheaderS->reserved = GIF_ERROR_DELETE_CONTACTS_NOT_A_CONTACT;

				gifbufferS = (char *) malloc(sizeof(gifhdr_t));
				memcpy(gifbufferS, gifheaderS, sizeof(gifhdr_t));
			}
			else  //要删除等联系人不在用户数据库中
			{
				int localflag = 0;
				// removing entry in the client's contacts file
                                strcpy(pathname_temp,path);
                                strcat(pathname_temp,"contacts_delete_temp.db");
				newfp = fopen(pathname_temp, "w");
				while(fread(&usrc, sizeof(user_contacts_t), 1, contactsfp) == 1)
				{
					if(strcmp(usrc.loginid, gifdata) != 0)
						fwrite(&usrc, sizeof(user_contacts_t), 1, newfp);
					else
						localflag = 1;
				}
				fclose(newfp);
				fclose(contactsfp);
				remove(pathname);
				rename(pathname_temp, pathname);

				gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
				gifheaderS->type = GIF_SUCCESS_N_ERROR_MSG;
				strcpy(gifheaderS->sender, "server");
				strcpy(gifheaderS->receiver, gifheader->sender);
				gifheaderS->length = 0;

				if(localflag == 1)
					gifheaderS->reserved = GIF_SUCCESS_DELETE_CONTACTS;
				else
					gifheaderS->reserved = GIF_ERROR_DELETE_CONTACTS_NOT_A_MEMBER;

				gifbufferS = (char *) malloc(sizeof(gifhdr_t));
				memcpy(gifbufferS, gifheaderS, sizeof(gifhdr_t));
			}

			if((send(client_sockfd, gifbufferS, (HEADER_LENGTH + gifheaderS->length), 0)) < 0)
			{
				perror("Cannot send message(AddContacts)");
			}

			free(gifbufferS);
			free(gifheaderS);

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}

		case 6: //GIF_CHAT_MSG
		{
			char *gifbufferS;
			online_users_t ousr;
			FILE *onlinefp;
			int receiving_client_sockfd;
			int flag = 0;

			onlinefp = fopen("online.db", "r");
			if(onlinefp == NULL)
			{
				perror("online.db");
				exit(0);
			}

			// retreving the socket file descriptor for the receiving client from the online.db file
			while((fread(&ousr, sizeof(online_users_t), 1, onlinefp)) == 1)
			{
				if((strcmp(ousr.loginid, gifheader->receiver)) == 0)
				{
					receiving_client_sockfd = ousr.sockfd;
					flag = 1;
				}
				else
					continue;
			}
			fclose(onlinefp);

			if(flag == 1)
			{
				gifbufferS = (char *)malloc(32 + gifheader->length);
				memcpy(gifbufferS, gifheader, 32);
				memcpy((gifbufferS + 32), gifdata, gifheader->length);

				if((send(receiving_client_sockfd, gifbufferS, (32 + gifheader->length), 0)) < 0)
				{
					perror("Message passing error");
				}

				free(gifbufferS);
			}
			else
			{
				FILE *offlinefp;
				offline_msgs_t omsgs;
				char *dateserial, filename[30];

				strcpy(omsgs.sender, gifheader->sender);
				strcpy(omsgs.message, gifdata);
				omsgs.new = 1;

				// setting the system date and time to the char pointer variable 'dateserial'(DD MMM YYYY  HH:MM)
				dateserial = gif_get_system_time();
				strcpy(omsgs.dateserial, dateserial);

				strcpy(filename, gifheader->receiver);
				strcat(filename, "_off.db");
				offlinefp = fopen(filename, "a");
				if(offlinefp == NULL)
				{
					perror("Offline Messages file");
					exit(0);
				}

				fwrite(&omsgs, sizeof(offline_msgs_t), 1, offlinefp);
				fclose(offlinefp);

				free(dateserial);
			}

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}

		case 7: //GIF_DISCONNECT_MSG
		{
			online_users_t ousr;
			user_contacts_t usrc;
			FILE *onlinefp, *newfp, *as_contactfp;
			char filename[30];

			onlinefp = fopen("online.db", "r");
			newfp = fopen("newfile.db", "w");

			rewind(onlinefp);
			while(fread(&ousr, sizeof(online_users_t), 1, onlinefp) == 1)
			{
				if((strcmp(ousr.loginid, gifheader->sender)) != 0)
					fwrite(&ousr, sizeof(online_users_t), 1, newfp);
			}
			fclose(newfp);
			fclose(onlinefp);
			remove("online.db");
			rename("newfile.db", "online.db");

			onlinefp = fopen("online.db", "r");

			// coding for refresing the contacts list of clients who has this just logined client as a contact
			strcpy(filename, gifheader->sender);
			strcat(filename, "_as.db");
			as_contactfp = fopen(filename, "r");
			if(as_contactfp == NULL)
			{
				perror("A user's as_contact file");
				exit(0);
			}

			while((fread(&usrc, sizeof(user_contacts_t), 1, as_contactfp)) == 1)
			{
				rewind(onlinefp);
				while((fread(&ousr, sizeof(online_users_t), 1, onlinefp)) == 1)
				{
					if((strcmp(usrc.loginid, ousr.loginid)) == 0)
					{
						gif_send_clients_contact_list(ousr.loginid, ousr.sockfd, 1);
					}
				}
			}
			fclose(as_contactfp);
			fclose(onlinefp);

			if(gifheader->length != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}

		case 9: // GIF_OFFLINE_REQUEST_MSG
		{
			gifhdr_t *gifheaderS;
			char *gifdataS, *gifbufferS, filename[30];
			offline_msgs_t omsgs;
			offline_msgs_send_t *omsgs_se;
			FILE *offlinefp;
			int counter, i, flag;
			counter = flag = 0;
			i = 1;

			strcpy(filename, gifheader->sender);
			strcat(filename, "_off.db");
			if((offlinefp = fopen(filename, "r+")) == NULL)
			{
				perror("A user's offline messages file");
				exit(0);
			}

			gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
			gifdataS = (char *) malloc(992);

			while((fread(&omsgs, sizeof(offline_msgs_t), 1, offlinefp)) == 1)
			{
				omsgs_se = (offline_msgs_send_t *) malloc(sizeof(offline_msgs_send_t));

				strcpy(omsgs_se->sender, omsgs.sender);
				strcpy(omsgs_se->dateserial, omsgs.dateserial);
				omsgs_se->new = omsgs.new;
				omsgs_se->length = strlen(omsgs.message) + 1;

				// making the read message to be old
				if(omsgs.new == 1)
				{
					omsgs.new = 0;
					fseek(offlinefp, -(sizeof(offline_msgs_t)), SEEK_CUR);
					fwrite(&omsgs, sizeof(offline_msgs_t), 1, offlinefp);
				}

				if((counter + sizeof(offline_msgs_send_t) + omsgs_se->length) <= 992)
				{
					memcpy((gifdataS + counter), omsgs_se, sizeof(offline_msgs_send_t));
					counter = counter + sizeof(offline_msgs_send_t);
					strncpy((gifdataS + counter), omsgs.message, omsgs_se->length);
					counter = counter + (omsgs_se->length);
				}
				else
				{
					gifheaderS->type = GIF_OFFLINE_MSG;
					strcpy(gifheaderS->sender, "server");
					strcpy(gifheaderS->receiver, gifheader->sender);
					gifheaderS->length = counter;
					gifheaderS->reserved = i;

					i++;

					gifbufferS = (char *) malloc(32 + (gifheaderS->length));
					memcpy(gifbufferS, gifheaderS, 32);
					memcpy((gifbufferS + 32), gifdataS, gifheaderS->length);

					if((send(client_sockfd, gifbufferS, (32 + gifheaderS->length), 0)) < 0)
					{
						perror("Error sending Offline Messages");
					}

					free(gifbufferS);
					free(gifdataS);
					free(gifheaderS);

					counter = 0;
					gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
					gifdataS = (char *) malloc(992);
				}

				free(omsgs_se);
			}
			fclose(offlinefp);

			gifheaderS->type = GIF_OFFLINE_MSG;
			strcpy(gifheaderS->sender, "server");
			strcpy(gifheaderS->receiver, gifheader->sender);
			gifheaderS->length = counter;
			gifheaderS->reserved = i;

			gifbufferS = (char *) malloc(32 + (gifheaderS->length));
			memcpy(gifbufferS, gifheaderS, 32);
			memcpy((gifbufferS + 32), gifdataS, gifheaderS->length);

			if((send(client_sockfd, gifbufferS, (32 + gifheaderS->length), 0)) < 0)
			{
				perror("Error sending Offline Messages");
			}

			free(gifbufferS);
			free(gifdataS);
			free(gifheaderS);

			if(gifheader->length != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}

		case 11: // GIF_OFFLINE_DELETE_MSG
		{
			offline_msgs_t omsgs;
			FILE *offlinefp, *newfp;
			char filename[30];
			strcpy(filename, gifheader->sender);
			strcat(filename, "_off.db");
			offlinefp = fopen(filename, "r");
			newfp = fopen("new_offline_file.db", "w");

			rewind(offlinefp);
			while(fread(&omsgs, sizeof(offline_msgs_t), 1, offlinefp) == 1)
			{
				if((strcmp(omsgs.message, gifdata)) != 0)
					fwrite(&omsgs, sizeof(offline_msgs_t), 1, newfp);
			}
			fclose(newfp);
			fclose(offlinefp);

			remove(filename);
			rename("new_offline_file.db", filename);

			if(gifheader->length != 0)
				free(gifdata);
			free(gifheader);
			free(gifbuffer);

			break;
		}
		}
	}
}

void gif_send_clients_contact_list(char *client_loginid, int client_sockfd, int type)
// type = 0 (sending the contacts list while normal login)
// type = 1(sending the contacts list while refresing)
{
	gifhdr_t *gifheaderS;
	char *gifdataS, *gifbufferS;
	online_users_t ousr;
	user_contacts_t usrc;
	user_status_t *usrs;
	FILE *contactsfp, *onlinefp;
	char filename[25];
	int i;

	strcpy(filename,client_loginid);
	strcat(filename,".db");
	contactsfp = fopen(filename,"r");
	if(contactsfp == NULL)
	{
		perror("A user's contacts file");
		exit(0);
	}

	onlinefp = fopen("online.db", "r");
	if(onlinefp == NULL)
	{
		perror("online.db");
		exit(0);
	}

	gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
	gifheaderS->type = GIF_ADDRLIST_MSG;
	strcpy(gifheaderS->sender, "server");
	strcpy(gifheaderS->receiver, client_loginid);
	gifheaderS->reserved = 0;
	if(type == 1)
		gifheaderS->reserved = 1;

	gifdataS = (char *) malloc(992);
	gifbufferS = (char *) malloc(1024);
	usrs = (user_status_t *) malloc(sizeof(user_status_t));

	i = 0;
	while((fread(&usrc, sizeof(user_contacts_t), 1, contactsfp)) == 1)
	{
		strcpy(usrs->loginid, usrc.loginid);
		// initialising status of the user to be offline
		usrs->status = 0;

		rewind(onlinefp);
		while((fread(&ousr, sizeof(online_users_t), 1, onlinefp)) == 1)
		{
			if((strcmp(ousr.loginid, usrs->loginid)) == 0)
				usrs->status = 1;	// setting status to online
			else
				continue;
		}

		memcpy((gifdataS + (i * sizeof(user_status_t))), usrs, sizeof(user_status_t));
		i++;
	}
	fclose(onlinefp);
	fclose(contactsfp);

	gifheaderS->length = (i * (sizeof(user_status_t)));
	memcpy(gifbufferS, gifheaderS, 32);
	memcpy((gifbufferS + 32), gifdataS, gifheaderS->length);

	if((send(client_sockfd, gifbufferS, (32 + gifheaderS->length), 0)) < 0)
	{
		perror("Error sending message(Address list)");
	}

	free(usrs);
	free(gifbufferS);
	free(gifdataS);
	free(gifheaderS);
}

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

int adduser()
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
