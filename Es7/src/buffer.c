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
	printf("The producer produce : %d", *buf=rand_r(0));
}



static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t full = PTHREAD_COND_INITIALIZER;

static bool is_full=false;
static bool is_empty=true;

static void *producer(){
	printf("ciao prod");
	fflush(stdout);
	while(true){
		
		sleep(1);
		pthread_mutex_lock(&mtx);
		printf("lock\n");
		while(!is_empty){
			pthread_cond_wait(&empty,&mtx);

		}
		put(&buffer);
		is_full=true;
		is_empty=false;		
		pthread_cond_signal(&full);
		pthread_mutex_unlock(&mtx);
		printf("unlock\n");

	}

	return NULL;
}

static void* consumer(void * arg){
	printf("ciao cons");
	while(true){

		sleep(1);
		
		pthread_mutex_lock(&mtx);
		printf("lock\n");
		while(!is_full){
			pthread_cond_wait(&full,&mtx);
		}
		
		printf("The consumer consume :%d\n", get());
		//buffer = NULL;
		is_empty = true;
		is_full = false;
		pthread_cond_signal(&empty);
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