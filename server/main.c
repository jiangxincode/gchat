#include "main.h"
#include "gif_defns.h"

int main()
{
	user_handle();
	int sockfd, client_sockfd;
	socklen_t length;
	struct sockaddr_in server_addr, client_addr;
	pthread_t pthd;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("error: socket");
		exit(1);
	}

	int reuse = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int)); //重用bind的地址

	server_addr.sin_family = AF_INET; //IPV4因特网域
	server_addr.sin_port = htons(SERVER_PORT); //设置端口号
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //设置IP地址，接收所有网卡收到等数据包

	if((bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)
	{
		perror("error: bind");
		exit(1);
	}

	puts("Gchat server is running, Welcome your connect!");

	length = sizeof(struct sockaddr_in);

	while(1)
	{
		if(listen(sockfd, SOMAXCONN) == -1) //入队等未完成连接的请求数目设为SOMAXCONN
		{
		        perror("error: listen");
		        continue;
		}
		if((client_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &length))  == -1)
		{
			perror("error: accept");
			continue;
		}
		else
		{
			if(pthread_create(&pthd, NULL, (void *)gif_handle_client, (void *)(&client_sockfd)) != 0)
                        {
                                perror("error: pthread_create");
                                continue;
                        }
			if(pthread_detach(pthd) != 0)
                        {
                                perror("error: pthread_detach");
                                continue;
                        }
		}
	}
}

