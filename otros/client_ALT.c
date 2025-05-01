#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h> //epoll
#include <fcntl.h> //epoll flags
#include <netinet/in.h>
#include <errno.h>

#define DEFAULT_PORT 8080

void main()
{
	int n;
	int c;
	int sockfd;
	char buf[256];
	struct sockaddr_in srv_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero((char *)addr, sizeof(struct sockaddr_in));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(DEFAULT_PORT);

	if (connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) 
	{
		perror("connect()");
		exit(1);
	}

	while (1)
	{
		printf("input: ");
		fgets(buf, sizeof(buf), stdin);
		c = strlen(buf) - 1;
		buf[c] = '\0';
		write(sockfd, buf, c + 1);

		bzero(buf, sizeof(buf));
		while (errno != EAGAIN && (n = read(sockfd, buf, sizeof(buf))) > 0) 
		{
			printf("echo: %s\n", buf);
			bzero(buf, sizeof(buf));

			c -= n;
			if (c <= 0) 
			{
				break;
			}
		}
	}
	close(sockfd);
}
