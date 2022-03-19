#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
	
#define BUFSIZE 2000
#define SERVERPORT 5500

char send_buf[BUFSIZE]; // a buffer to store the sending mesaages
char recv_buf[BUFSIZE]; // buffer to store received messages
char temp[BUFSIZE];
int bytes;	
char *headers = "POST /client HTTP/1.1\nHost - Mycomputer\nContent-Type - application/txt\nContent-Length - 2000\r\n\r\n";

	
int main()
{
	fd_set master;
	fd_set read_fds;
	int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	FD_ZERO(&master); // zeroing out the master
        FD_ZERO(&read_fds); //zeroing out the read file descriptors
        FD_SET(0, &master); //set the fd:0 in master
	
	//creating a socket for the client 
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket creation failed.\n");
		exit(EXIT_FAILURE);
	}
	
	
	server_addr.sin_family = AF_INET; // assigning internet protocol family
	server_addr.sin_port = htons(SERVERPORT); // assigning the port number
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //assigning the ip address	
	
	//connecting the client socket to server socket
	if(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("Connection to server failed.\n");
		exit(EXIT_FAILURE);
	}
	
        FD_SET(sockfd, &master); // set the client file descriptor in master
	fdmax = sockfd; //initially assigning the max file descriptor as client socket's file descriptor.
	
	
	//do until the client is alive
	while(1){
		read_fds = master;
		
		//checking which file descriptor is ready using select system call
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			printf("Error in selecting File descriptors\n");
			exit(EXIT_FAILURE);
		}
		
		for(i=0; i <= fdmax; i++ )
			//checking whether socket fd i is in the active read fds set
			if(FD_ISSET(i, &read_fds)){
				//if present
				if (i == 0){
					memset(send_buf,BUFSIZE,0);//clearing the send buffer
					memset(temp,BUFSIZE,0);//clearing the temp buffer
					fgets(temp, BUFSIZE, stdin); // get the message from standard input;
					strcat(send_buf,headers);
					strcat(send_buf,temp);
					
					send(sockfd, send_buf, strlen(send_buf), 0);//send the message 
					memset(send_buf,BUFSIZE,0);
					memset(temp,BUFSIZE,0);//clearing the temp buffer
				}
				else {
					//printf("%d \n",i);
					
					memset(recv_buf , 0, BUFSIZE);
					bytes = recv(sockfd, recv_buf, BUFSIZE, 0); // receive the message
					recv_buf[bytes] = '\0'; //terminate the string
					//removing the HTTP response headers
					char *body = strstr(recv_buf,"\r\n\r\n");
					body = body+4;
					printf("%s" , body);
					memset(recv_buf, 0, BUFSIZE);//clearing the recv_buffer
					fflush(stdout); //clear the standard output
				}
			}
	}
	
	close(sockfd);//close the socket after communication
	return 0;
}
