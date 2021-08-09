#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <boundedqueue.h>

static pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;



static BQueue_t *buf1;
static BQueue_t *buf2;


static void *readerLines(void *arg){
	FILE *ifp;
	ifp = fopen(arg,"r");
	if(!ifp){
		fprintf(stderr, "errore in apertura file");
		exit(EXIT_FAILURE);
	}

	fseek(ifp,0L,SEEK_END);
	int sz = ftell(ifp);
	char s[sz + 2];
	rewind(ifp);

	while(fgets(s,sz+2,ifp) != NULL){
		char * line = malloc(sizeof(s));
		strcpy(line,s);
		push(buf1,line);
	}
	
	
}

static void *tokenizerLines(void *arg){
	
	while(buf1->qlen > 0){
		char *line = (char*) pop(buf1);
		
		char *token = strtok(line," ");
		while(token){
			char *word = malloc(strlen(token)+1);
			

			strcpy(word,token);
			push(buf2,word);
			token = strtok(NULL," ");
		}
		free(line);

	}	

}

static void *printerThread(void *arg){

	
	while(buf2->qlen > 0){

		char *word = (char*) pop(buf2);
		fprintf(stdout, "%s\n",word);
		free(word);

	}


}


int main(int argc, char *argv[]){
	
	if(argc!=2){

		fprintf(stderr, "usage pipe filename");
		exit(EXIT_FAILURE);

	}

	
	

	int err;
	pthread_t tid,tid2,tid3;
	buf1 = initBQueue(10);
	buf2 = initBQueue(100);

	if( (err = pthread_create(&tid,NULL,readerLines,argv[1])) != 0 ){

		errno = err;
		perror("pthread_create");
		exit(-1);


	}

	if( (err = pthread_create(&tid2,NULL,tokenizerLines,NULL)) ){

		errno = err;
		perror("pthread_create");
		exit(-1);

	}

	if( (err = pthread_create(&tid3,NULL,printerThread,NULL)) ){

		errno = err;
		perror("pthread_create");
		exit(-1);

	}

	if(pthread_join(tid,NULL) == -1){
			fprintf(stderr, "pthread_join failed");
	}
	if(pthread_join(tid2,NULL) == -1){
			fprintf(stderr, "pthread_join failed");
	}
	if(pthread_join(tid3,NULL) == -1){
			fprintf(stderr, "pthread_join failed");
	}

	deleteBQueue(buf1,NULL);
	deleteBQueue(buf2,NULL);	




}