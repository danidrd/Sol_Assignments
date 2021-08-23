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
	long *fd_c = (long*)arg;int err;
	char buf[N];

	while(!sig_flag){
		
		
			
			SYSCALL(err,read(*fd_c,buf,N),"read");
			if(err == 0 || sig_flag) break;
			toUpper(buf);
			SYSCALL(err,write(*fd_c,buf,N),"write");
		
		
	}


	close(*fd_c);
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
		return -1;
	}


	if(pthread_attr_init(&thattr) != 0){
		fprintf(stderr, "pthread_attr_init\n");
		close(connfd);
		return -1;
	}

	//setting thread in detached mode

	if(pthread_attr_setdetachstate(&thattr,PTHREAD_CREATE_DETACHED) != 0){
		fprintf(stderr, "pthread_attr_setdetachstate fallita\n");
		pthread_attr_destroy(&thattr);
		close(connfd);
		return -1;
	}

	if(pthread_create(&thid,&thattr,worker,(void*)connfd) != 0){
		fprintf(stderr, "pthread_create fallita\n");
		pthread_attr_destroy(&thattr);
		close(connfd);
		return -1;
	}

	if(pthread_sigmask(SIG_SETMASK,&oldmask,NULL) != 0 ){
		fprintf(stderr, "FATAL ERROR\n");
		close(connfd);
		return -1;
	}

	return 0;

}

volatile sig_atomic_t sig_flag = 0;

static void sighandler(int useless){
	sig_flag = 1;
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
	//Blocco la ricezione di segnali prima dell-installazione del signal handler
	if(pthread_sigmask(SIG_BLOCK,&mask,&oldmask) != 0){
		fprintf(stderr, "FATAL ERROR, pthread_sigmask\n");
		return EXIT_FAILURE;
	}

	//Installo il signal handler per tutti i segnali che mi interessano
	//Gestione dei segnali
	struct sigaction s;
	memset( &s, 0, sizeof(s));
	s.sa_handler=sighandler;
	ec_meno1(sigaction(SIGINT,&s,NULL),"sigaction");
	ec_meno1(sigaction(SIGTERM,&s,NULL),"sigaction");
	ec_meno1(sigaction(SIGQUIT,&s,NULL),"sigaction");
	ec_meno1(sigaction(SIGHUP,&s,NULL),"sigaction");
	s.sa_handler=SIG_IGN;
	ec_meno1(sigaction(SIGPIPE,&s,NULL),"sigaction");
	


	int fd_skt,err,r=0; 
	
	struct sockaddr_un sa;
	memset(&sa,'0',sizeof(sa));
	strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
	sa.sun_family=AF_UNIX;
	SYSCALL(fd_skt,socket(AF_UNIX,SOCK_STREAM,0),"creando listening socket");
	SYSCALL(err,bind(fd_skt,(struct sockaddr *) &sa,sizeof(sa)),"binding");
	SYSCALL(err,listen(fd_skt,1),"listening");
 	
 	if(pthread_sigmask(SIG_SETMASK,&oldmask,NULL) != 0 ){
 		fprintf(stderr, "FATAL ERROR\n");
 		return EXIT_FAILURE;
 	}
 	
 	while(!sig_flag){

 		long fd_c;
 		if(!sig_flag  && (fd_c = accept(fd_skt,NULL,0)) == -1){
 			if(errno == EINTR){ //Se accept interrotta da segnale
 				if(sig_flag) break;
 			}else {
 				perror("accept");
 				r = EXIT_FAILURE;
 			}
 		}

 		if(dispatcher(fd_c)< 0){
 			r = EXIT_FAILURE;
 			break;
 		}
 	}
 	
 	cleanup();

 	close(fd_skt);
 	return r;

 	
}