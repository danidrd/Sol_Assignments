#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <sys/select.h>

#include <conn.h>
#include <message.h>

static void toup(char *str){
	char *p = str;
	while(*p != '\0'){
		*p = (islower(*p)?toupper(*p):*p);
		++p;
	}
}



//Funzione eseguita dal worker thread del pool
//Gestisce un intera connessione di un client


void threadF(void *arg){
	assert(arg);
	long* args = (long*)arg;
	long connfd = args[0];
	long *termina = (long*)(args[1]);
	int pipe_request = (int) (args[2]);
	free(arg);

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
	if((m=writen(pipe_request,&connfd,sizeof(int))) == -1){
		perror("pipe write");
	}
	

}