#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <unboundedqueue.h>

static UBQueue_t buf;
static int no_msg_per_producer;

static void *producers_f(void * arg){

}

static void *consumers_f(void * arg){


}


int main(int argc,char **argv){

	if(argc != 4){
		fprintf(stderr, "usage %s producers consumers messages\n", argv[0] );
	}

	int M = atoi(argv[1]);
	int N = atoi(argv[2]);
	int K = atoi(argv[3]);

	if(K % M == 0){
		no_msg_per_producer = K/M;
	}else{
		no_msg_per_producer = (int) K/M;
	}

	pthread_t *producers,*consumers;

	producers = malloc(sizeof(pthread_t)*M);
	consumers = malloc(sizeof(pthread_t)*N);

	for (int i = 0; i < M; ++i)
	{
		int err;
		if( (err = pthread_create(&producers[i],NULL,producers_f,NULL)) != 0 ){

		errno = err;
		perror("pthread_create");
		exit(-1);


		}
	}

	for (int i = 0; i < N; ++i)
	{
		int err;
		if( (err = pthread_create(&consumers[i],NULL,consumers_f,NULL)) != 0 ){

		errno = err;
		perror("pthread_create");
		exit(-1);


		}
	}

	for (int i = 0; i < M; ++i)
	{
		if(pthread_join(producers[i],NULL) == -1){
			fprintf(stderr, "pthread_join failed");
		}
	}

	for (int i = 0; i < N; ++i)
	{
		if(pthread_join(consumers[i],NULL) == -1){
			fprintf(stderr, "pthread_join failed");
		}
	}

}