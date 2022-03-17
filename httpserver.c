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
#define  CLIADDRLEN 100

char error404[] = "404 NOT FOUND";
char *suc200 = "200 OK";
char *suc201 = "201 CREATED";
char *error400 = "400 BAD REQUEST";
void get_request(char* path,int connfd)

{
	char outstr[BUFSIZE];
	FILE *fp = fopen(path,"r");
	if(fp){
		while(fgets(outstr,BUFSIZE,fp)){
			write(connfd,outstr,sizeof(outstr));
		}
		fclose(fp);
	}
		
	else {

		write(connfd,error404,sizeof(error404));
	}		
}
//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


void put_request(char* path,int connfd){}
void delete_request(char* path,int connfd){}
void post_request(char* path,int connfd){

}

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
		if((read(connfd,buffer,sizeof(buffer)))<0){
			printf("Error reading data from client\n");
			exit(EXIT_FAILURE);
		}
		//strcpy(parse,buffer);
		//printf("client Request:\n %s",parse);
		printf("Client [%s] Request : %s\n",clientAddr,buffer);
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
	while(1){
		if((connfd=accept(serversocket,(struct 		sockaddr*)&cliaddr,&addrlen))<0)
		{
			printf("Error in accepting client\n");
			exit(EXIT_FAILURE);
		}
		printf("Connection accepted Successfully\n");

	//handle_clients(connfd,serversocket);
		inet_ntop(AF_INET,&(cliaddr.sin_addr),clientAddr,CLIADDRLEN);
		if((childpid = fork())==0){
			close(serversocket);
			handle_clients(connfd,clientAddr);
			
			
		}
		close(connfd);
	}

}
