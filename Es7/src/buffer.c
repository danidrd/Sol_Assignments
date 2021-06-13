#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

static char* buffer;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t full = PTHREAD_COND_INITIALIZER;

static int is_full;
static int is_empty;

static void* producer(void * arg){
	
	while(1){
		printf("ciao");
		sleep(1);
		pthread_mutex_lock(&mtx);
		printf("lock");
		while(!is_empty){
			pthread_cond_wait(&empty,&mtx);

		}
		buffer = "c";
		is_full=1;		
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mtx);
		printf("unlock");

	}
	return;
}

static void* consumer(void * arg){
	printf("ciao consumer");
	
	while(1){

		sleep(1);
		
		pthread_mutex_lock(&mtx);
		printf("lock");
		while(!is_full){
			pthread_cond_wait(&full,&mtx);
		}
		
		
		buffer = NULL;
		is_empty = 1;
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mtx);
		printf("unlock");
	}
	return;
}

int main(void){

	pthread_t tid1,tid2;
	int err1,err2;

	printf("ciaoc");
	if( (err1 = pthread_create(&tid1,NULL,&producer,NULL)) != 0){
		errno = err1;
		perror("pthread_create");
		exit(-1);
	}
	printf("ciaob");
	if( (err2 = pthread_create(&tid2,NULL,&consumer,NULL)) != 0){
		errno = err2;
		perror("pthread_create");
		exit(-1);
	}	


}