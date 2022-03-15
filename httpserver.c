#include<stdio.h>
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

void handle_requests(char *method,char *path,int connfd){
	char outstr[BUFSIZE]="";
	if(!(strcmp(method,"GET"))){
		FILE *fp = fopen(path,"r");
		if(fp){
			while(fgets(outstr,BUFSIZE,fp))
				strcat(outstr,outstr);
		}
		fclose(fp);
		printf("%s\n",outstr);
		write(connfd,"success ",sizeof("success"));
		
		
	
	}
}

void handle_clients(int connfd){
	char buffer[BUFSIZE];
	char parse[BUFSIZE];
	while(1){
		memset(buffer,0,BUFSIZE);
		read(connfd,buffer,sizeof(buffer));
		strcpy(parse,buffer);
		printf("client Request:\n %s",parse);
		char *method = strtok(buffer," ");
		printf("Method = %s\n",method);
		int i=0;
		while(buffer[i++]!='/');
		//printf("%d",i);
		//printf("%c",*(buffer+i));
		char *path = strtok(buffer+i," ");
		//printf("path = %s\n",path);
		handle_requests(method,path,connfd);
		//write(connfd,"success ",sizeof("success"));
	}
}
int main(){
	int serversocket,connfd;
	struct sockaddr_in servaddr,cliaddr;
	int addrlen = sizeof(cliaddr);

	//create a socket
	if((serversocket = socket(AF_INET,SOCK_STREAM,0))<=0){
		printf("Error in creating socket!\n");
		exit(EXIT_FAILURE);
	}
	printf("Server socket created succesfully!!\n");
	memset(&servaddr,0,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVERPORT);

	//bind the socket to the IP
	if((bind(serversocket,(struct sockaddr*)&servaddr,sizeof(servaddr)))!=0)
	{
		printf("Error in binding the server socket!\n");
		exit(EXIT_FAILURE);

	}

	//listen for clients
	if ((listen(serversocket,5))!=0){
		
		printf("Error in Listening!\n");
                exit(EXIT_FAILURE);
		
	}
	if((connfd=accept(serversocket,(struct sockaddr*)&cliaddr,&addrlen))<0)
	{
		printf("Error in accepting client\n");
		exit(EXIT_FAILURE);
	}

	handle_clients(connfd);
	close(serversocket);


}
