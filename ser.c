#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
	
#define SERVERPORT 5500
#define BUFSIZE 2000

char suc201[BUFSIZE] = "HTTP/1.1 201 sent\nServer - Mycomputer\nContent-Type - application/txt\nContent-Length - 2000\r\n\r\n";

int bytes;
char recv_buf[BUFSIZE];
char temp[BUFSIZE];
char buf[BUFSIZE];

			

int main()
{
	fd_set master;
	fd_set read_fds; // data structure for holding file descriptors
	int fdmax, i,sockfd=0;
	struct sockaddr_in serv_addr, client_addr;
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	//creating server socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Socket creation failed.\n");
		exit(EXIT_FAILURE);
	}
		
	serv_addr.sin_family = AF_INET; // assigning internet protol family
	serv_addr.sin_port = htons(SERVERPORT); //assigning the port number
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //assigning any ip address
		
	FD_SET(sockfd, &master); // set the file desc of server socket in master set
	
	fdmax = sockfd; //initially assigning the max file descriptor as cerver socket's file descriptor.
	
	//binding the server socket with the ip address
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) {
		printf("Error in binding the socket.\n");
		exit(EXIT_FAILURE);
	}
	
	//listening to the socket 
	
	if (listen(sockfd, 5) == -1) {
		printf("Error in listening\n");
		exit(EXIT_FAILURE);
	}
	printf("Waiting for Connection.\n");
	
	while(1){
		read_fds = master;
		
		//using select checking which file desc in readfds is active
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			printf("Error in selecting File descriptors\n");
			exit(EXIT_FAILURE);
		}
		
		for (i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				//checking whether i is in the read fds set
				if (i == sockfd) {//if is same as the serversocket {

					socklen_t addrlen;
					int newsockfd; //file descriptor for new connection
	
					addrlen = sizeof(struct sockaddr_in);
					//accepting the new client
					if((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen)) < 0) {
						printf("Error in accepting the new connection\n");
						exit(EXIT_FAILURE);
					}
					else {
						// adding the new file descriptor in the set 
						FD_SET(newsockfd, &master);
						if(newsockfd > fdmax){
							fdmax = newsockfd; // updating the maximum val of file descriptor
						}
					printf("New connection from %s on port %d \n",
					inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
					}
				}
				else{
					if ((bytes = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
						if (bytes == 0) 
							printf("connection from %s on port %d is disconnected. \n",
					inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
						else {
							printf("Error in receiving\n");
						}
					close(i); // close the connection
					FD_CLR(i, &master); // clear the file descriptor in the set
					}	
					else { 
						// forward te messages to the other clients
						int j;
						for(j = 0; j <= fdmax; j++){
							//checking whether the fd is in the list
							if (FD_ISSET(j, &master)){
								//message should be forwarded only to other clients
								if (j != sockfd && j != i) {
									//removing http headers and sending the body
									int k=0,l=0;
									char method[10];
									while(recv_buf[k]!=' '){
										method[k] = recv_buf[k];
										++k;}
									method[k] = '\0';
									printf("Received from Client on socket: %d\n  %s\n",i,recv_buf);
									if(!(strcmp(method,"POST"))){
									
										memset(temp,0,BUFSIZE);
										
										char *body = strstr(recv_buf,"\r\n\r\n");
										body = body+4;
										//memset(recv_buf,0,BUFSIZE);
										strcat(temp,suc201);
										strcat(temp,body);
										printf("Sent to client on socket %d\n %s",j,temp);
										
										if (send(j, temp, strlen(temp), 0) == -1) 
											printf("Error in sending the messages.\n");
									
										memset(recv_buf,0,BUFSIZE);
																				
									}
									else{
										
									}	
			
								}
							}
							
						}
		
					}
				}
			}
		}
	}
	return 0;
}
