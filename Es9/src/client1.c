#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define SOCKNAME "./mysock"
#define UNIX_PATH_MAX 108
#define SYSCALL(r,c,e)\
	if((r=c)==-1){perror(e);exit(errno); }
#define N 100


int main(int argc, char *argv[]){

	int fd_skt,err; char buf[N];
	struct sockaddr_un sa;
	strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
	sa.sun_family=AF_UNIX;

	SYSCALL(fd_skt,socket(AF_UNIX,SOCK_STREAM,0),"creating socket");
	while(connect(fd_skt, (struct sockaddr*) &sa, sizeof(sa)) == -1){
		if(errno == ENOENT) sleep(1);
		else exit(EXIT_FAILURE);
	}

	while(true){
		char line[N];

		memset(line,'\0',N);
		if(fgets(line,N, stdin) == NULL ){
			perror("fgets");
			return -1;
		}

		if(strncmp(line,"quit",4) == 0) break;

		SYSCALL(err,write(fd_skt,line,N),"write client");

		SYSCALL(err,read(fd_skt,buf,N),"read client");
		

		fprintf(stdout, "Operazione : %s", line);
		fprintf(stdout, "Risultato : %s\n", buf);
	}



}