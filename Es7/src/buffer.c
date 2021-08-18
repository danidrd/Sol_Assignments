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
	printf("The producer produce : %d\n", *buf=rand());
}




static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static char bufempty=1;


static void *producer(){
	fflush(stdout);
	while(true){

		
		pthread_mutex_lock(&mtx);
		printf("lock %d \n",gettid());
		while(!bufempty){
			pthread_cond_wait(&cond,&mtx);

		}
		put(&buffer);
		
		bufempty=0;
			
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mtx);
		printf("unlock, %d \n",gettid());

	}

	return NULL;
}

static void* consumer(void * arg){
	
	while(true){
	

		
		
		pthread_mutex_lock(&mtx);
		printf("lock %d \n",gettid());
		while(bufempty){
			pthread_cond_wait(&cond,&mtx);
		}
		
		printf("The consumer consume :%d\n", get());
		bufempty=1;
		
		
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mtx);
		printf("unlock, %d \n",gettid());
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