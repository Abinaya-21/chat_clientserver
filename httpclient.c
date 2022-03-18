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

char *headers = "Host - Mycomputer\nContent-Type - application/txt \nContent-Length - 1000\n\n";

void chat_withserver(int sockfd)
{
	char buff[BUFSIZE],user[20],pass[20],ack[100];
	int n;
	while(1) {
		memset(buff,0, sizeof(buff));

		printf("\nEnter the HTTP Request: ");
		fgets(buff,BUFSIZE,stdin);
		
		char *method = strtok(buff," ");
		if(method == "GET")
			strcat(buff,headers);
		else if (method == "POST"){
			strcat(buff,headers);
			printf("\n Enter Username : ");
			n=0;
			while ((user[n++] = getchar()) != '\n')
			;
			strcat(buff,user);
			printf("\n Enter Password : ");
			n=0;
			while ((pass[n++] = getchar()) != '\n')
			;
			strcat(buff,pass);
			
			
		}
		send(sockfd, buff, sizeof(buff),0);
		
		memset(buff,0, sizeof(buff));
		recv(sockfd, buff, sizeof(buff),0);
		printf("From Server : %s", buff);
		
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

	if (connect(clientsocket, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("connected to the server..\n");

	chat_withserver(clientsocket);


	close(clientsocket);
}

