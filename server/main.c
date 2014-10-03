#include "../gif_defns.h"
#include "support.h"

int main()
{
	int sockfd, client_sockfd;
	socklen_t length;
	struct sockaddr_in servAddr, cliAddr;
	pthread_t pthd;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Cannot create socket");
		exit(0);
	}
	int on = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr))) == -1)
	{
		perror("Cannot bind address");
		exit(0);
	}

	printf("running gchat........\n");

	length = sizeof(struct sockaddr_in);

	while(1)
	{
		listen(sockfd, 5);
		if((client_sockfd = accept(sockfd, (struct sockaddr *) &cliAddr, &length)) < 0)
		{
			perror("Cannot accept clients request");
			continue;
		}
		else
		{
			if(pthread_create( &pthd, NULL, (void *)gif_handle_client, (int *)client_sockfd) != 0)
				perror("Thread creation problem");
			pthread_detach(pthd);
		}
	}
}

