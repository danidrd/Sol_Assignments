#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>


typedef pthread_mutex_t forchetta_t;
typedef struct threadArgs {
	int 		thid;
	int  		N;
	forchetta_t *forks;
} threadArgs_t;

void eat(unsigned int *seed){

	long r = rand_r(seed) % 800000;

	struct timespec t = {0,r};
	nanosleep(&t,NULL);

}

void think(unsigned int *seed){

	long r = rand_r(seed) % 1000000;
	struct timespec t = {0,r};
	nanosleep(&t,NULL);

}

static void *philosopher(void *tidARG){
	
	int myid = ((threadArgs_t*)tidARG)->thid;
	int N = ((threadArgs_t*)tidARG)->N;

	int left = myid % N;
	int right = myid -1;

	forchetta_t* destra = &((threadArgs_t*)tidARG)->forks[right];
	forchetta_t* sinistra = &((threadArgs_t*)tidARG)->forks[left];

	unsigned int seed = myid * time(NULL);

	int mangia =0;
	int pensa =0;

	for (int i = 0; i < 100; ++i)
	{
		think(&seed);
		pensa++;

		if(myid % 2){ //filosofo di indice dispari prende prima la forchetta destra

			pthread_mutex_lock(destra);
			pthread_mutex_lock(sinistra);
			eat(&seed);
			mangia++;
			pthread_mutex_unlock(sinistra);
			pthread_mutex_unlock(destra);

		}else{ //il filosofo di indice pari prende prima la forchetta sinistra

			pthread_mutex_lock(sinistra);
			pthread_mutex_lock(destra);
			eat(&seed);
			mangia++;
			pthread_mutex_unlock(destra);
			pthread_mutex_unlock(sinistra);


		}

		fprintf(stdout, "Filosofo %d : ho mangiato %d e pensato %d volte \n", myid,mangia,pensa);
		fflush(stdout);
	}


	return NULL;
}




int main(int argc, char *argv[]){

	int N = atoi(argv[1]);

	if(argc < 2){

		fprintf(stderr,"usage filosofi 5[>=]");
		return -1;

	}

	if(N<5){

		fprintf(stderr,"Almeno 5 filosofi/thread");
		return -2;

	}

	pthread_t* tid; //Thread filosofi
	tid = malloc(sizeof(pthread_t)*N);
	int* err; //array di errori in parallelo ai thread
	err = malloc(sizeof(int)*N);
	forchetta_t * forks; //array di mutex
	forks = malloc(sizeof(forchetta_t)*N);
	threadArgs_t *tidARGS; // array di struct che contiene puntatore ai mutex ed info per ogni singolo filosofo
	tidARGS = malloc(sizeof(threadArgs_t) * N);



	//Controllo di successo sulle malloc
	if(!forks || !tid || !err || !tidARGS){

		fprintf(stderr,"malloc fallita\n");
		exit(EXIT_FAILURE);

	}

	//Inizializzazione dinamica delle mutex contenute nell-array forks
	for (int i = 0; i < N; ++i)
	{
		if(pthread_mutex_init(&forks[i], NULL) != 0){

			fprintf(stderr, "pthread_mutex_init failed" );
			exit(EXIT_FAILURE);

		}
	}
	//Inizializzazione delle informazioni da passare ai thread i-esimi
	for (int i = 0; i < N; ++i)
	{
		tidARGS[i].thid = (i+1);
		tidARGS[i].N = N;
		tidARGS[i].forks = forks;
	}

	//Creazione dei thread e passaggio delle informazioni attraverso la struttura tidARGS
	for (int i = 0; i < N; ++i)
	{
		if( (err[i] = pthread_create(&tid[i],NULL,philosopher,&tidARGS[i])) != 0 ){

			errno = err[i];
			perror("pthread_create");
			exit(-1);

		}
	}
	
	for (int i = 0; i < N; ++i)
	{
		if(pthread_join(tid[i],NULL) == -1){
			fprintf(stderr, "pthread_join failed");
		}
	}

	free(tid);
	free(err);
	free(forks);
	free(tidARGS);

}