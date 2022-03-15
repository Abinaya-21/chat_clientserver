#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include<unistd.h>
#include<arpa/inet.h>
#define BUFSIZE  10000
#define PORT 9000

void chat_withserver(int sockfd)
{
	char buff[BUFSIZE];
	int n;
	while(1) {
		memset(buff,0, sizeof(buff));
		printf("\nEnter the Request: ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		/*if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}*/
	}
}

int main()
{
	int clientsocket, connfd;
	struct sockaddr_in servaddr, cliaddr;

	// socket create and verification
	if((clientsocket = socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("Error in creating client socket!\n");
		exit(EXIT_FAILURE);
	}
	printf("Client Socket successfully created..\n");
	memset(&servaddr,0, sizeof(servaddr));


	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(clientsocket, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	chat_withserver(clientsocket);

	// close the socket
	close(clientsocket);
}

