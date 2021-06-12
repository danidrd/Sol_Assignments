#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

static char* buffer;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t full = PTHREAD_COND_INITIALIZER;


static void* producer(void * arg){
	
	while(1){
		
		sleep(1);
		pthread_mutex_lock(&mtx);
		printf("lock");
		pthread_cond_wait(&empty,&mtx);
		buffer = "c";
			
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mtx);
		printf("unlock");

	}
	return;
}

static void* consumer(void * arg){

	
	while(1){

		sleep(1);
		
		pthread_mutex_lock(&mtx);
		printf("lock");
		
			pthread_cond_wait(&full,&mtx);
		
		buffer = NULL;
		
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mtx);
		printf("unlock");
	}
	return;
}

int main(void){

	pthread_t tid1,tid2;
	int err1,err2;


	if( (err1 = pthread_create(&tid1,NULL,&producer,NULL)) != 0){
		errno = err1;
		perror("pthread_create");
		exit(-1);
	}

	if( (err2 = pthread_create(&tid2,NULL,&consumer,NULL)) != 0){
		errno = err2;
		perror("pthread_create");
		exit(-1);
	}	


}