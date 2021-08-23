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
#include <signal.h>
#include <aio.h>



#define SOCKNAME "./mysock2"
#define UNIX_PATH_MAX 108
#define SYSCALL(r,c,e)\
	if((r=c)==-1){perror(e);exit(errno); }

#define ec_meno1(s,m)\
	if( (s) == -1){perror(m);exit(EXIT_FAILURE);}

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

	return;
}

static void *worker(void *arg){
	long fd_c = (long)arg;int err;
	char buf[N];

	while(!termina){
		toUpper(buf);
		SYSCALL(err,read(fd_c,buf,N),"read");
		if(err == 0) break;
		SYSCALL(err,write(fd_c,buf,N),"write");
	}


	close(fd_c);
	return NULL;
}

int dispatcher(long connfd){
	pthread_attr_t thattr;
	pthread_t thid;

	sigset_t mask,oldmask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask,SIGQUIT);
	sigaddset(&mask,SIGTERM);
	sigaddset(&mask,SIGHUP);


	if(pthread_sigmask(SIG_BLOCK,&mask,&oldmask) != 0){
		fprintf(stderr, "FATAL ERROR, pthread_sigmask\n");
		close(connfd);
		return;
	}


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
		return ;
	}

	if(pthread_create(&thid,&thattr,worker,(void*)connfd) != 0){
		fprintf(stderr, "pthread_create fallita\n");
		pthread_attr_destroy(&thattr);
		close(connfd);
		return;
	}

	if(pthread_sigmask(SIG_SETMASK,&oldmask,NULL) != 0 ){
		fprintf(stderr, "FATAL ERROR\n");
		close(connfd);
		return;
	}

	return;

}

static int termina = 0;
static int listenfd;

static void *sigHandler(void *arg){
	sigset_t *set = (sigset_t*)arg;

	for( ;; ){
		int sig;
		int r = sigwait(set,&sig);

		if(r != 0){
			errno = r;
			perror("FATAL ERROR sigwait");
			return NULL;
		}

		switch(sig){
			case SIGINT:
			case SIGTERM:
			case SIGQUIT:
				termina =1;

				shutdown(listenfd, SHUT_RDWR);

				return NULL;
			default: ;
		}

	}

	return NULL;
}




int main(int argc, char *argv[]){

	cleanup();
	atexit(cleanup);

	sigset_t mask, oldmask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGTERM);
	sigaddset(&mask, SIGHUP);

	if(pthread_sigmask(SIG_BLOCK,&mask,&oldmask) != 0){
		fprintf(stderr, "FATAL ERROR, pthread_sigmask\n");
		return EXIT_FAILURE;
	}

	//Installo il signal handler per tutti i segnali che mi interessano
	//Gestione dei segnali
	struct sigaction s;
	memset( &s, 0, sizeof(s));
	s.sa_handler=SIG_IGN;
	ec_meno1(sigaction(SIGPIPE,&s,NULL),"sigaction");
	pthread_t sighandler_thread;

	if(pthread_create(&sighandler_thread,NULL,sigHandler,&mask)!= 0){
		fprintf(stderr, "errore nella creazione del signal handler thread\n" );
		abort();
	}
	


	int err,r=0; 
	
	struct sockaddr_un sa;
	memset(&sa,'0',sizeof(sa));
	strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
	sa.sun_family=AF_UNIX;
	SYSCALL(listenfd,socket(AF_UNIX,SOCK_STREAM,0),"creando listening socket");
	SYSCALL(err,bind(listenfd,(struct sockaddr *) &sa,sizeof(sa)),"binding");
	SYSCALL(err,listen(listenfd,1),"listening");
 	
 	if(pthread_sigmask(SIG_SETMASK,&oldmask,NULL) != 0 ){
 		fprintf(stderr, "FATAL ERROR\n");
 		return EXIT_FAILURE;
 	}
 	
 	while(!termina){

 		long fd_c;
 		if(fd_c = accept(listenfd,NULL,0) == -1){
 			if(errno == EINVAL && termina) break; 
 				
 			
 			perror("accept");
 			exit(-1);
 			
 		}

 		dispatcher(fd_c);
 	}
 	
 	
 	cleanup(SOCKNAME);
 	
 	close(listenfd);
 	return 0;

 	
}