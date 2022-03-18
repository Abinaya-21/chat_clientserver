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
	
	while(1){
		if((connfd=accept(serversocket,(struct sockaddr*)&cliaddr,&addrlen))<0)
		{
			printf("Error in accepting client\n");
			exit(EXIT_FAILURE);
		}
		printf("Connection accepted Successfully from %s:%d\n",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
		printf("fd: %d\n ",connfd);
		

		inet_ntop(AF_INET,&(cliaddr.sin_addr),clientAddr,CLIADDRLEN);
		if((childpid = fork())==0){
			close(serversocket);
			handle_clients(connfd,clientAddr);
			}
			
			
		close(connfd);
	
	}
	return 0;
	}





