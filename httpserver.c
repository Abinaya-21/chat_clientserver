/*#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<unistd.h>

#define SERVERPORT 9000
#define  BUFSIZE 10000
#define  CLIADDRLEN 100

char error404[] = "404 NOT FOUND";
char *suc200 = "200 OK";
char *suc201 = "201 CREATED";
char *error400 = "400 BAD REQUEST";
char outstr[BUFSIZE];
void get_request(char* path,int connfd)

{
	memset(outstr,0,BUFSIZE);
	
	FILE *fp = fopen(path,"r");
	if(fp){
		//send(connfd,suc200,sizeof(suc200),0);
		while(fgets(outstr,BUFSIZE,fp)){
			
			send(connfd,outstr,sizeof(outstr),0);
		}
		fclose(fp);
	}
		
	else {

		send(connfd,error404,sizeof(error404),0);
	}
	memset(outstr,0,BUFSIZE);		
}

void post_request(char* path,int connfd){
	memset(outstr,0,BUFSIZE);
	recv(connfd,outstr,sizeof(outstr),0);
	FILE *fp;
	fp = fopen(path,"w");
	fprintf(fp,outstr);
	fclose(fp);
	send(connfd,suc201,sizeof(suc201),0);	
	memset(outstr,0,BUFSIZE);
	
}
//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


void put_request(char* path,int connfd){}
void delete_request(char* path,int connfd){}
//void post_request(char* path,int connfd){

//}

void handle_requests(char *method,char *path,int connfd){
	
	if(!(strcmp(method,"GET")))
		get_request(path,connfd);	
	else if(!(strcmp(method,"POST")))
		post_request(path,connfd);
	else if(!(strcmp(method,"PUT")))
		put_request(path,connfd);
	else if(!(strcmp(method,"DELETE")))
		delete_request(path,connfd);	
	else
		write(connfd,error400,sizeof(error400));

}

void handle_clients(int connfd,char clientAddr[CLIADDRLEN]){
	char buffer[BUFSIZE];
	//char parse[BUFSIZE];
	while(1){
		memset(buffer,0,BUFSIZE);
		if((recv(connfd,buffer,sizeof(buffer),0))<0){
			printf("Error reading data from client\n");
			exit(EXIT_FAILURE);
		}
		//strcpy(parse,buffer);
		//printf("client Request:\n %s",parse);
		printf("Client [%s] Request :\n %s",clientAddr,buffer);
		char *method = strtok(buffer," ");
		//printf("Method = %s\n",method);
		int i=0;
		while(buffer[i++]!='/');
		
		char *path = strtok(buffer+i," ");
		
		handle_requests(method,path,connfd);
		
		
	}
}
int main(){
	int serversocket,connfd;
	struct sockaddr_in servaddr,cliaddr;
	int addrlen = sizeof(cliaddr);
	char clientAddr[CLIADDRLEN];
	pid_t childpid;
	int clientlist[2];

	
	if((serversocket = socket(AF_INET,SOCK_STREAM,0))<=0){
		printf("Error in creating Server socket!\n");
		exit(EXIT_FAILURE);
	}
	printf("Server socket created succesfully!!\n");
	memset(&servaddr,0,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVERPORT);

	
	if((bind(serversocket,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0)
	{
		printf("Error in binding the server socket!\n");
		exit(EXIT_FAILURE);

	}
	printf("Bind is successfull!\n");

	
	if ((listen(serversocket,2))!=0){
		
		printf("Error in Listening!\n");
                exit(EXIT_FAILURE);
		
	}
	int i;
	while(i<2){
		if((clientlist[i]=accept(serversocket,(struct sockaddr*)&cliaddr,&addrlen))<0)
		{
			printf("Error in accepting client\n");
			exit(EXIT_FAILURE);
		}
		printf("Connection accepted Successfully from %s:%d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
		printf("fd: %d\n ",clientlist[i]);
		

		inet_ntop(AF_INET,&(cliaddr.sin_addr),clientAddr,CLIADDRLEN);
		//if((childpid = fork())==0){
			//close(serversocket);
			handle_clients(clientlist[i],clientAddr);
			
			
			
		
		//close(connfd);
		++i;
	}*/
	//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
	
#define TRUE 1
#define FALSE 0
#define SERVERPORT 9000
#define  BUFSIZE 10000
	
int main(int argc , char *argv[])
{
	int opt = TRUE;
	int master_socket , addrlen , new_socket , client_socket[2] ,max_clients = 2 , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
		
	char buffer[BUFSIZE];
	//set of socket descriptors
	fd_set readfds;
		
	//a message
	char *ack = "Server accepted the connection \n";
		
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		printf("Error in socket creation\n");
		exit(EXIT_FAILURE);
	}
	printf("master socket: %d\n",master_socket);
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(SERVERPORT );
		
	
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		printf("Error in binding\n");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", SERVERPORT);
		
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		printf("Error in listening");
		exit(EXIT_FAILURE);
	}
		
	//accept the incoming connection
	addrlen = sizeof(address);
		
	while(1)
	{
		//clear the socket set
		FD_ZERO(&readfds);
	
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
			
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			
			if((sd = client_socket[i])>0)
				FD_SET(sd, &readfds);
		
			if(sd > max_sd)
				max_sd = sd;
		}
	
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}
			
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
		
			/*//send new connection greeting message
			if( send(new_socket, message, strlen(message), 0) != strlen(message) )
			{
				perror("send");
			}
			*/
			send(new_socket,ack,sizeof(ack),0);			
				
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
				
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread = read( sd , buffer, BUFSIZE))<= 0)
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i] = 0;
				}
					
				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buffer[valread] = '\0';
					int j=0;
					for(;j<max_clients;j++)
						send(client_socket[i] , buffer , strlen(buffer) , 0 );
				}
			}
		}
	}
		
	return 0;
}


