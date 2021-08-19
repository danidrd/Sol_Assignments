#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/select.h>


#define SOCKNAME "./mysock3"
#define UNIX_PATH_MAX 108
#define SYSCALL(r,c,e)	\
	if((r=c)==-1){perror(e);exit(errno); }
#define N 100

void cleanup(){
	unlink(SOCKNAME);
}

void toUpper(char *buf){
	for (int i = 0; i < strlen(buf); ++i)
	{
		if(islower(buf[i])){
			buf[i] = (char) toupper((int) buf[i]);
		}
	}

	return buf;
}

int update(fd_set set, int fdmax){
	for(int i=(fdmax-1);i>=0;--i){
		if(FD_ISSET(i,&set)) return i;
	}

	return -1;
}

int main(int argc, char *argv[]){

	cleanup();
	atexit(cleanup);
	
	int fd_skt,fd_c,fd_num=0,fd,err; 
	char buf[N]; fd_set set, rdset,wrset;int nread;
	struct sockaddr_un sa;
	strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
	sa.sun_family=AF_UNIX;
	SYSCALL(fd_skt,socket(AF_UNIX,SOCK_STREAM,0),"creando listening socket");
	SYSCALL(err,bind(fd_skt,(struct sockaddr *) &sa,sizeof(sa)),"binding");
	SYSCALL(err,listen(fd_skt,1),"listening");
 	
 	
 	if(fd_skt > fd_num) fd_num = fd_skt;
 	FD_ZERO(&set);
 	FD_SET(fd_skt,&set);

 	while(true){
 		rdset = set;
 		if(select(fd_num + 1, &rdset, &wrset, NULL, NULL) == -1){
 			fprintf(stderr, "errore nella select\n");
 			exit(-1);
 		}else{
 			for (int fd = 0; fd<= fd_num ; ++fd)
 			{
 				if(FD_ISSET(fd,&rdset)){
 					if(fd == fd_skt){ // socket connect pronto
 						SYSCALL(fd_c,accept(fd_skt,NULL,0),"accepting connect");
 						FD_SET(fd_c,&set);
 						if(fd_c > fd_num) fd_num = fd_c;
 					}else{ // socket i/o pronto
 						SYSCALL(err,read(fd,buf,N),"reading");
 						if(err == 0){ //EOF client finito
 							FD_CLR(fd,&set);
 							fd_num = update(set,fd_num);
 							close(fd);
 						}
 						toUpper(buf);
 						if(write(fd,buf,N) == -1){
 						perror("write");
 						FD_CLR(fd,&set);
 						close(fd);
 						}
 					}
 				}
 			}
 		}
 	}
 	

 	
}