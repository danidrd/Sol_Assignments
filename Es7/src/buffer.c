#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>





static int buffer;

int get(){
	return buffer;
}

void put(int* buf){
	printf("The producer produce : %d", *buf=rand());
}



static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


static char bufempty=1;


static void *producer(){
	printf("ciao prod");
	fflush(stdout);
	while(true){
			printf("ciao prod1");

		
		pthread_mutex_lock(&mtx);
		printf("lock\n");
		while(!bufempty){
			pthread_cond_wait(&cond,&mtx);

		}
		put(&buffer);
		printf("%d",buffer);
		bufempty=0;
			
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mtx);
		printf("unlock\n");

	}

	return NULL;
}

static void* consumer(void * arg){
	printf("ciao cons");
	while(true){
	printf("ciao cons 1");

		
		
		pthread_mutex_lock(&mtx);
		printf("lock\n");
		while(bufempty){
			pthread_cond_wait(&cond,&mtx);
		}
		
		printf("The consumer consume :%d\n", get());
		bufempty=1;
		
		
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mtx);
		printf("unlock\n");
	}
	return NULL;
}

int main(void){

	pthread_t tid1,tid2;
	int err1,err2;
	
	if( (err1 = pthread_create(&tid1,NULL,producer,NULL)) != 0){
		errno = err1;
		perror("pthread_create");
		exit(-1);
	}

	if( (err2 = pthread_create(&tid2,NULL,&consumer,NULL)) != 0){
		errno = err2;
		perror("pthread_create");
		
		exit(-1);


	}	

	

	return 0;

}