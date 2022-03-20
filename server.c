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
#define BUFSIZE 500

char suc201[BUFSIZE] = "201 sent\nServer - Mycomputer\nContent-Type - application/txt\nContent-Length - 2000\r\n\r\n";
int bytes;
char recv_buf[BUFSIZE];
char temp[BUFSIZE];
char *headers = "201 sent\nServer - Mycomputer\n";
int conlen;
char contype[] = "Content-Type - text/plain \n";
char linebreak[] = "\r\n\r\n";
			

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
					printf("\nNew connection from %s on port %d \n",
					inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
					}
				}
				else{
					memset(recv_buf,0,BUFSIZE); 
					memset(temp,0,BUFSIZE); 
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
						recv_buf[bytes-1]='\0';
						printf("\nRequest from client %d \n%s",i,recv_buf);
						int k=0,l=0,j;
						char method[10];
						char body[BUFSIZE];
						while(recv_buf[k]!=' '){
							method[k] = recv_buf[k];
							++k;}
						method[k] = '\0';
						if(!(strcmp(method,"POST"))){
							//if the method is post , get the request body
							int x=0;
							while(recv_buf[x]!='\r')
								++x;
							x+=4;
							while(recv_buf[x] != '\0'){
							body[l] = recv_buf[x];
							++x;
							++l;
							}
							
							body[l]='\0';
							for(j = 0; j <= fdmax; j++){
							//checking whether the fd is in the list
								if (FD_ISSET(j, &master)){
								//message should be forwarded only to other clients
								
									if (j != sockfd && j != i) {
									  	// forward the messages to the other clients
										if (send(j, body, sizeof(body), 0) == -1) 	
											printf("Error in sending the messages.\n");	
										}
									else if(i==j){
										//sending the response header to the client which sent the request
										char contlen[30];
										conlen = strlen(body);; //update the content length as length of the msg
										sprintf(contlen,"Content-Length -%d\n",conlen);
										//construct the reponse headers
										strcat(temp,headers); // add the headers
										strcat(temp,contlen);
										strcat(temp, contype);
										strcat(temp,linebreak);
										send(j,temp,sizeof(temp),0);
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
}//main
