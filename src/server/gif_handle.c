#include "../common/common.h"
#include "main.h"

void gif_handle_client(void *client)
{
	int client_sockfd = *(int *)client;
	gifhdr_t *gifheader;
	int rcv_status;
	char *gifdata, *gifbuffer;
	char loginid[COMMON_LENGTH], password[COMMON_LENGTH];
	char pathname[MAX_PATH_LENGTH];

	while(1)
	{
		gifbuffer = (char *) malloc(BUFF_SIZE);
		rcv_status = recv(client_sockfd, gifbuffer, BUFF_SIZE, 0);

		if(rcv_status == -1)
		{
			_DEBUG("recv error");
			pthread_exit(NULL);
		}
		else if(rcv_status == 0) //连接关闭
		{
			printf("%s disconnected.\n", loginid);
			pthread_exit(NULL);
		}
		gifheader = (gifhdr_t *) malloc(sizeof(gifhdr_t));
		if(gifheader == NULL) //分配空间错误
		{
			_DEBUG("malloc error");
			pthread_exit(NULL);
		}
		memcpy(gifheader, gifbuffer, HEADER_LENGTH);
		if((gifheader->length) > 0)
		{
			gifdata = (char *) malloc(gifheader->length);
			memcpy(gifdata , (gifbuffer + HEADER_LENGTH), gifheader->length);
		}
		else
		{
			display_header(gifheader);
		}
		free(gifbuffer);

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

			get_full_path_name(pathname,"users.db",1,"server/db/");
			usersfp = fopen(pathname,"r");
			if(usersfp == NULL)
			{
				_DEBUG("error: open users.db");
				pthread_exit(NULL);
			}
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
				printf("%s - Login Correct\n", loginid);

				strcpy(ousr.loginid, loginid);
				ousr.sockfd = client_sockfd;

				// adding the name to the online users list file
				get_full_path_name(pathname,"online.db",1,"server/db/");
				onlinefp = fopen(pathname, "a+"); //如果不存在则新建，同时既要读又要写
				if(onlinefp == NULL)
				{
					_DEBUG("error: open online.db");
					pthread_exit(NULL);
				}
				fwrite(&ousr, sizeof(online_users_t), 1, onlinefp);

				// 发送该登录用户的联系人状态信息
				gif_send_clients_contact_list(loginid, client_sockfd, 0);

				/**检查以该登录用户为联系人的用户是否在线，如果在，则向其更新在线联系人信息*/
				get_full_path_name(pathname,"_as.db",2,"server/db/",loginid);
				as_contactfp = fopen(pathname, "r");
				if(as_contactfp == NULL)
				{
					_DEBUG("error: _as.db");
					pthread_exit(NULL);
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
			}
			else //用户名或密码不匹配
			{
				printf("%s : %s - Login Incorrect\n", loginid, password);
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
					_DEBUG("error:send GIF_SUCCESS_N_ERROR_MSG");
					pthread_exit(NULL);
				}

				free(gifbufferS);
				free(gifheaderS);
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
			int flag = 0;

			get_full_path_name(pathname,".db",2,"server/db/",gifheader->sender);
			contactsfp = fopen(pathname, "a");
			if(contactsfp == NULL)
			{
				_DEBUG("error: open .db");
				pthread_exit(NULL);
			}

			get_full_path_name(pathname,"users.db",1,"server/db/");
			usersfp = fopen(pathname, "r");
			if(usersfp == NULL)
			{
				_DEBUG("error:open users.db");
				pthread_exit(NULL);
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
				get_full_path_name(pathname,"_as.db",2,"server/db/",gifdata);
				as_contactfp = fopen(pathname, "a");
				if(as_contactfp == NULL)
				{
					_DEBUG("error: open _as.db");
					pthread_exit(NULL);
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
				_DEBUG("error: send GIF_SUCCESS_N_ERROR_MSG");
			}

			free(gifbufferS);
			free(gifheaderS);

			if(flag == 1)
				gif_send_clients_contact_list(gifheader->sender, client_sockfd, 1); //mark应该还要给所添加的用户更新视图

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);

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

			get_full_path_name(pathname,"users.db",1,"server/db/");
			usersfp = fopen(pathname, "r");
			if(usersfp == NULL)
			{
				_DEBUG("A user's contact file");
				pthread_exit(NULL);
			}

			while((fread(&usr, sizeof(users_t), 1, usersfp)) == 1)
			{
				if((strcmp(usr.loginid, gifdata)) == 0)
					flag = 1;
				else
					continue;
			}
			fclose(usersfp);

			get_full_path_name(pathname,".db",2,"server/db/",gifheader->sender);
			contactsfp = fopen(pathname, "r");
			if(contactsfp == NULL)
			{
				_DEBUG("A user's contact file");
				pthread_exit(NULL);
			}

			if(flag == 1) //要删除等联系人在用户数据库中
			{
				int localflag = 0;
				// removing entry in the client's contacts file

				get_full_path_name(pathname_temp,"contacts_delete_temp.db",1,"server/db/");
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
				get_full_path_name(pathname,"_as.db",2,"server/db/",gifdata);
				as_contactfp = fopen(pathname, "r");
				if(as_contactfp == NULL)
				{
					_DEBUG("A user's as_contact file");
					pthread_exit(NULL);
				}

				get_full_path_name(pathname_temp,"as_contacts_delete_temp.db",1,"server/db/");
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
				get_full_path_name(pathname_temp,"contacts_delete_temp.db",1,"server/db/");
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
				_DEBUG("Cannot send message(AddContacts)");
			}

			free(gifbufferS);
			free(gifheaderS);

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);

			break;
		}

		case GIF_CHAT_MSG:
		{
			char *gifbufferS;
			online_users_t ousr;
			FILE *onlinefp;
			int receiving_client_sockfd;
			int flag = 0;

			get_full_path_name(pathname,"online.db",1,"server/db/");

			if((onlinefp = fopen(pathname, "r")) == NULL)
			{

				pthread_exit(NULL);
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
				gifbufferS = (char *)malloc(HEADER_LENGTH + gifheader->length);
				memcpy(gifbufferS, gifheader, HEADER_LENGTH);
				memcpy((gifbufferS + HEADER_LENGTH), gifdata, gifheader->length);

				if((send(receiving_client_sockfd, gifbufferS, (HEADER_LENGTH + gifheader->length), 0)) < 0)
				{
					_DEBUG("Message passing error");
				}

				free(gifbufferS);
			}
			else
			{
				FILE *offlinefp;
				offline_msgs_t omsgs;
				char *dateserial;

				strcpy(omsgs.sender, gifheader->sender);
				strcpy(omsgs.message, gifdata);
				omsgs.new = 1;

				// setting the system date and time to the char pointer variable 'dateserial'(DD MMM YYYY  HH:MM)
				dateserial = get_system_time();
				strcpy(omsgs.dateserial, dateserial);

				get_full_path_name(pathname,"_off.db",2,"server/db/",gifheader->receiver);
				offlinefp = fopen(pathname, "a");
				if(offlinefp == NULL)
				{
					_DEBUG("Offline Messages file");
					pthread_exit(NULL);
				}

				fwrite(&omsgs, sizeof(offline_msgs_t), 1, offlinefp);
				fclose(offlinefp);

				free(dateserial);
			}

			if((gifheader->length) != 0)
				free(gifdata);
			free(gifheader);

			break;
		}

		case GIF_DISCONNECT_MSG:
		{
			online_users_t ousr;
			user_contacts_t usrc;
			FILE *onlinefp, *newfp, *as_contactfp;
			char pathname_temp[MAX_PATH_LENGTH];

			get_full_path_name(pathname,"online.db",1,"server/db/");
			onlinefp = fopen(pathname, "r");
			get_full_path_name(pathname_temp,"newfile.db",1,"server/db/");
			newfp = fopen(pathname_temp, "w");

			rewind(onlinefp);
			while(fread(&ousr, sizeof(online_users_t), 1, onlinefp) == 1)
			{
				if((strcmp(ousr.loginid, gifheader->sender)) != 0)
					fwrite(&ousr, sizeof(online_users_t), 1, newfp);
			}
			fclose(newfp);
			fclose(onlinefp);
			remove(pathname);
			rename(pathname_temp, pathname);

			onlinefp = fopen(pathname, "r");

			// coding for refresing the contacts list of clients who has this just logined client as a contact
			get_full_path_name(pathname_temp,"_as.db",2,"server/db/",gifheader->sender);
			as_contactfp = fopen(pathname, "r");
			if(as_contactfp == NULL)
			{
				_DEBUG("A user's as_contact file");
				pthread_exit(NULL);
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

			break;
		}

		case GIF_OFFLINE_REQUEST_MSG:
		{
			gifhdr_t *gifheaderS;
			char *gifdataS, *gifbufferS;
			offline_msgs_t omsgs;
			offline_msgs_send_t *omsgs_se;
			FILE *offlinefp;
			int counter, i, flag;
			counter = flag = 0;
			i = 1;

			get_full_path_name(pathname,"_off.db",2,"server/db/",gifheader->sender);
			if((offlinefp = fopen(pathname, "r+")) == NULL)
			{
				_DEBUG("A user's offline messages file");
				pthread_exit(NULL);
			}

			gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
			gifdataS = (char *) malloc(BODY_LENGTH);

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

				if((counter + sizeof(offline_msgs_send_t) + omsgs_se->length) <= BODY_LENGTH)
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

					gifbufferS = (char *) malloc(HEADER_LENGTH + (gifheaderS->length));
					memcpy(gifbufferS, gifheaderS, HEADER_LENGTH);
					memcpy((gifbufferS + HEADER_LENGTH), gifdataS, gifheaderS->length);

					if((send(client_sockfd, gifbufferS, (HEADER_LENGTH + gifheaderS->length), 0)) < 0)
					{
						_DEBUG("Error sending Offline Messages");
					}

					free(gifbufferS);
					free(gifdataS);
					free(gifheaderS);

					counter = 0;
					gifheaderS = (gifhdr_t *) malloc(sizeof(gifhdr_t));
					gifdataS = (char *) malloc(BODY_LENGTH);
				}

				free(omsgs_se);
			}
			fclose(offlinefp);

			gifheaderS->type = GIF_OFFLINE_MSG;
			strcpy(gifheaderS->sender, "server");
			strcpy(gifheaderS->receiver, gifheader->sender);
			gifheaderS->length = counter;
			gifheaderS->reserved = i;

			gifbufferS = (char *) malloc(HEADER_LENGTH + (gifheaderS->length));
			memcpy(gifbufferS, gifheaderS, HEADER_LENGTH);
			memcpy((gifbufferS + HEADER_LENGTH), gifdataS, gifheaderS->length);

			if((send(client_sockfd, gifbufferS, (HEADER_LENGTH + gifheaderS->length), 0)) < 0)
			{
				_DEBUG("Error sending Offline Messages");
			}

			free(gifbufferS);
			free(gifdataS);
			free(gifheaderS);

			if(gifheader->length != 0)
				free(gifdata);
			free(gifheader);

			break;
		}

		case GIF_OFFLINE_DELETE_MSG:
		{
			offline_msgs_t omsgs;
			FILE *offlinefp, *newfp;
			char pathname_temp[MAX_PATH_LENGTH];
			get_full_path_name(pathname,"_off.db",2,"server/db/",gifheader->sender);
			offlinefp = fopen(pathname, "r");
			get_full_path_name(pathname_temp,"new_offline_file.db",1,"server/db/");
			newfp = fopen(pathname_temp, "w");

			rewind(offlinefp);
			while(fread(&omsgs, sizeof(offline_msgs_t), 1, offlinefp) == 1)
			{
				if((strcmp(omsgs.message, gifdata)) != 0)
					fwrite(&omsgs, sizeof(offline_msgs_t), 1, newfp);
			}
			fclose(newfp);
			fclose(offlinefp);

			remove(pathname);
			rename(pathname_temp, pathname);

			if(gifheader->length != 0)
				free(gifdata);
			free(gifheader);

			break;
		}
		default :
			break;
		}

	}
}



