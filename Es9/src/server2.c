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



#define SOCKNAME "./mysock2"
#define UNIX_PATH_MAX 108
#define SYSCALL(r,c,e)\
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

static void *worker(void *arg){
	long fd_c = (long)arg;int err;
	char buf[N];

	while(true){
		toUpper(buf);
		SYSCALL(err,read(fd_c,buf,N),"read");
		if(err == 0) break;
		SYSCALL(err,write(fd_c,buf,N),"write");
	}

	if(errRead == -1){
		perror("read");
		exit(-1);
	}

	close(fd_c);
	return NULL;
}
void dispatcher(long connfd){
	pthread_attr_t thattr;
	pthread_t thid;

	if(pthread_attr_init(&thattr) != 0){
		fprintf(stderr, "pthread_attr_init\n");
		close(connfd);
		return;
	}

	//setting thread in detached mode

	if(pthread_attr_setdetachstate(&thattr,PTHREAD_CREATE_DETACHED) != 0){
		fprintf(stderr, "pthread_attr_setdetachstate fallita\n");
		pthread_attr_destroy(&thattr);
		close(connfd);
		return;
	}

	if(pthread_create(&thid,&thattr,worker,(void*)connfd) != 0){
		fprintf(stderr, "pthread_create fallita\n");
		pthread_attr_destroy(&thattr);
		close(connfd);
		return;
	}

}



int main(int argc, char *argv[]){

	cleanup();
	atexit(cleanup);
	
	


	int fd_skt,err; 
	pthread_t * tid;DispatcherArgs_t *dat;
	struct sockaddr_un sa;
	strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
	sa.sun_family=AF_UNIX;
	SYSCALL(fd_skt,socket(AF_UNIX,SOCK_STREAM,0),"creando listening socket");
	SYSCALL(err,bind(fd_skt,(struct sockaddr *) &sa,sizeof(sa)),"binding");
	SYSCALL(err,listen(fd_skt,1),"listening");
 	
 	
 	
 	while(true){
 		long fd_c;
 		SYSCALL(fd_c,accept(fd_skt,NULL,0),"accepting connection");

 		dispatcher(fd_c);
 	}
 	

 	
}