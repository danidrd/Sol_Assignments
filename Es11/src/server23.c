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
#include <signal.h>
#include <assert.h>


#include <message.h>
#include <threadpool.h>
#include <conn.h>
#include <util.h>

static void toup(char *str){
	char *p = str;
	while(*p != '\0'){
		*p = (islower(*p)?toupper(*p):*p);
		++p;
	}
}

int update(fd_set set, int fdmax){
	for(int i=(fdmax-1);i>=0;--i){
		if(FD_ISSET(i,&set)) return i;
	}

	return -1;
}

//Funzione eseguita dal worker thread del pool
//Gestisce un intera connessione di un client


static void threadF(void *arg){
	assert(arg);
	long* args = (long*)arg;
	long connfd = args[0];
	long *termina = (long*)(args[1]);
	int pipe_request = (int) (args[2]);
	free(arg);
	printf("threadF first %d\n", pipe_request);
	fd_set set, tmpset;
	FD_ZERO(&set);
	FD_SET(connfd, &set);

	do{
		tmpset = set;
		int r;

		//ogni tanto controllo se devo terminare
		struct timeval timeout = {0,100000}; // 100 ms

		if( (r = select(connfd+1, &tmpset, NULL, NULL, &timeout)) < 0 ){
			perror("select");
			break;
		}
		//Timeout terminato, nessun fd controllo se devo terminare
		if(r == 0){
			if(*termina) break;
			continue;
		}

		msg_t str;
		int n;

		if( (n=readn(connfd, &str.len, sizeof(int))) == -1 ){
			perror("read1");
			*termina = 1;
			break;
		}

		if(n == 0 ) break;

		str.str =  calloc((str.len),sizeof(char));
		if(!str.str){
			perror("calloc");
			fprintf(stderr, "Memoria esaurita....\n");
			break;
		}

		if( (n=readn(connfd, str.str, str.len * sizeof(char))) == -1 ){
			perror("read2");
			free(str.str);
			*termina = 1;
			break;
		}

		toup(str.str);

		if( (n=writen(connfd, &str.len, sizeof(int))) == -1){
			perror("write1");
			free(str.str);
			*termina = 1;
			break;
		}

		if( (n=writen(connfd, str.str, str.len * sizeof(char))) == -1 ){
			perror("write2");
			free(str.str);
			*termina = 1;
			break;
		}
		free(str.str);

	*termina = 1;
	}while(*termina == 0);

	int m;
	close(connfd);
	write(pipe_request, &connfd, sizeof(int)); 
	printf("threadF %d\n", connfd );
	return;
	
	

}


#define SYSCALL(r,c,e)	\
	if((r=c)==-1){perror(e);goto _exit; }
#define N 100

void cleanup(){
	unlink(SOCKNAME);
}


typedef struct sigHandler_t {
	sigset_t *set; //Set dei segnali da gestire(mascherati)
	int 	signal_pipe; //Descrittore di scrittura di una pipe senza nome
}sigHandler_t;

static void *sigHandler(void *arg){
	sigset_t *set = ((sigHandler_t*)arg)->set;
	int fd_pipe	  = ((sigHandler_t*)arg)->signal_pipe;

	for( ;; ){
		int sig;
		int r = sigwait(set,&sig);

		if(r != 0){
			errno = r;
			perror("FATAL ERROR sigwait");
			return NULL;
		}

		switch(sig){
			case SIGTERM:
			case SIGINT:
			case SIGQUIT:
				

				close(fd_pipe); //notifico il listener thread della ricezione del segnale

				return NULL;
			default: ;
		}

	}

	return NULL;
}



static void usage(const char *argv0){
	fprintf(stderr, "usage : %s threadsInPool\n",argv0);
}

static void checkargs(int argc, char *argv[]){

	if(argc != 2){
		usage(argv[0]);
		_exit(EXIT_FAILURE);
	}

	if((int)strtol(argv[1], NULL, 10) <= 0){
		fprintf(stderr, "threadsInPool must be greater than zero\n\n");
		usage(argv[0]);
		_exit(EXIT_FAILURE);
	}

}

int main(int argc, char *argv[]){
	checkargs(argc,argv);

	int threadsInPool = (int) atoi(argv[1]);

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGTERM);

	if(pthread_sigmask(SIG_BLOCK,&mask,NULL) != 0){
		fprintf(stderr, "FATAL ERROR\n");
		goto _exit;
	}

	//Ignoro SIGPIPE per evitare di essere terminato da una scrittura su un socket
	struct sigaction s;
	memset(&s,0,sizeof(s));
	s.sa_handler=SIG_IGN;

	if( (sigaction(SIGPIPE, &s, NULL)) == -1){
		perror("sigaction");
		goto _exit;
	}

	//La pipe viene utilizzata come canale di comunicazione tra il signalhandler thread 
	//ed il thread listener per notificare la terminazione

	int signal_pipe[2];
	if(pipe(signal_pipe) == -1){
		perror("pipe");
		goto _exit;
	}

	pthread_t sighandler_thread;
	sigHandler_t handlerArgs = { &mask, signal_pipe[1] };
	
	if(pthread_create(&sighandler_thread,NULL,sigHandler,&handlerArgs) != 0){
		fprintf(stderr, "Errore nella creazione del signal handler thread!\n");
		goto _exit;
	}

	int listenfd;

	if( (listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1 ){
		perror("socket");
		goto _exit;
	}

	struct sockaddr_un serv_addr;
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);

	if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
		perror("bind");
		goto _exit;
	}

	if(listen(listenfd, MAXBACKLOG) == -1){
		perror("listen");
		goto _exit;
	}

	threadpool_t *pool = NULL;

	pool 	= createThreadPool(threadsInPool,threadsInPool);
	if(!pool){
		fprintf(stderr, "ERRORE FATALE NELLA CREAZIONE DEL THREADPOOL\n");
		goto _exit;
	}
 	
 	fd_set set,tmpset;
 	FD_ZERO(&set);
 	FD_ZERO(&tmpset);

 	FD_SET(listenfd, &set); //Aggiungo il listener fd al master set
 	FD_SET(signal_pipe[0], &set); //Aggiungo il descrittore di lettura della signalpipe
 	
 	int fdmax = (listenfd > signal_pipe[0]) ? listenfd : signal_pipe[0];

 	volatile long termina = 0;
 	int pipe_request[2];
	if(pipe(pipe_request) == -1){
		 perror("pipe");
		 goto _exit;
	}
	FD_SET(pipe_request[0], &set);
	fdmax = (fdmax > pipe_request[0])?fdmax:pipe_request[0];
 	while(!termina){
 		//Copio il set nella variabile tempooranea per la select
 		tmpset = set;
 		
 		printf("mi blocco su select\n");
 		fflush(stdout);
 		if(select(fdmax + 1, &tmpset, NULL, NULL, NULL) == -1){
 			perror("select");
 			goto _exit;
 		}else{
 			//Cerchiamo di capire da quale fd abbiamo ricevuto una richiesta
 			for (int fd = 0; fd<= fdmax ; ++fd)
 			{
 				if(FD_ISSET(fd,&tmpset)){
 					long connfd;
 					if(fd == listenfd){ // socket connect pronto
 						SYSCALL(connfd,accept(listenfd,NULL,0),"accepting connect");
 						
 						
 						long* args = malloc(3 * sizeof(long));
 						if(!args){
 							perror("FATAL ERROR malloc");
 							goto _exit;
 						}
 						args[0] = connfd;
 						args[1] = (long) &termina;
 						args[2] = (long) pipe_request[1]; //Passo al threadF anche la pipe di richiesta chiusura servizio

 						int r = addToThreadPool(pool, threadF, (void *)args);
 						if(r == 0) continue; // aggiunto con successo
 						if(r<0){
 							fprintf(stderr, "FATAL ERROR , adding thread to the pool\n" );;
 						}else{//Coda pendenti piena
 							fprintf(stderr, "SERVER TOO BUSY\n");
 						}
 						printf("passo da qui\n");
 						free(args);
 						close(connfd);
 						continue;
 						
 					}else if(fd == signal_pipe[0]){ 
 							//ricevuto un segnale, esco ed inizio il protocollo di terminazione
 							printf("passo da signal_pipe\n");
 							fflush(stdout);
 							termina = 1;
 							break;
 					}else if(fd == pipe_request[0]){
 							//Ricevuto segnale di terminazione worker
 							int fd_c;
 							read(pipe_request[0],&fd_c,sizeof(int));
 							printf("main %d\n", fd_c);
 							fflush(stdout);
 							FD_SET(fd_c, &set);
 							fdmax = (fdmax > fd_c) ? fdmax : fd_c;
 							continue;
 					}else {
 						//Socket I/O pronto
 						printf("passo da socket IO pronto\n");
 						FD_CLR(fd, &tmpset);
 						fdmax = update(set,fdmax);
 						long* args = malloc(3 * sizeof(long));
 						if(!args){
 							perror("FATAL ERROR malloc");
 							goto _exit;
 						}
 						args[0] = fd;
 						args[1] = (long) &termina;
 						args[2] = (long) pipe_request[1]; //Passo al threadF anche la pipe di richiesta chiusura servizio

 						int r = addToThreadPool(pool, &threadF, (void *)args);
 						if(r == 0) continue; // aggiunto con successo
 						if(r<0){
 							fprintf(stderr, "FATAL ERROR , adding thread to the pool\n" );;
 						}else{//Coda pendenti piena
 							fprintf(stderr, "SERVER TOO BUSY\n");
 						}

 						free(args);
 						close(fd);
 						continue;
 					}
 				}else continue;
 			}
 		}
 	}
 	printf("ciao!!\n");
 	destroyThreadPool(pool,0);

 	//Attendo la terminazione del signal handler thread
 	printf("in attesa di pthread_join\n");
 	pthread_join(sighandler_thread, NULL);
 	
 	cleanup();
 	return 0;

  _exit:
 	cleanup();
 	printf("passo da _exit\n");
 	return -1;
}