#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#include <boundedqueue.h>

/**
* @file boundedqueue.c
* @brief File di implementazione dell-interfaccia per la coda di dimensione finita
*/

/*-------------Macro------------*/


#define LOCK(l)		if(pthread_mutex_lock(l)!=0)		{ \
	fprintf(stderr, "FATAL ERROR pthread_mutex_lock");	  \
	pthread_exit((void*)EXIT_FAILURE);					  \
}\

#define UNLOCK(l)		if(pthread_mutex_unlock(l)!=0)		{ \
	fprintf(stderr, "FATAL ERROR pthread_mutex_unlock");	  \
	pthread_exit((void*)EXIT_FAILURE);					  \
}

#define WAIT(c,l)	if(pthread_cond_wait(c,l)!= 0)		{\
	fprintf(stderr, "FATAL ERROR pthread_cond_wait");	 \
	pthread_exit((void*)EXIT_FAILURE);					  \
}

#define SIGNAL(c)	if(pthread_cond_signal(c)!=0)	{	\
	fprintf(stderr, "FATAL ERROR pthread_cond_signal");	 \
	pthread_exit((void*)EXIT_FAILURE);					  \
}


/*--------------- funzioni di  utilita'-----------------*/

//qui assumiamo per semplicita' che le mutex non siano mai di 
//tipo robust mutex (pthread_mutex_lock(3)) per cui possono
//di fatto ritornare solo EINVAL se la mutex non e' stata inizializzata

static inline void LockQueue(BQueue_t *q)	{LOCK(&q->m);}
static inline void UnlockQueue(BQueue_t *q)	{UNLOCK(&q->m);}
static inline void WaitToProduce(BQueue_t *q)	{WAIT(&q->cfull,&q->m);}
static inline void WaitToConsume(BQueue_t *q)	{WAIT(&q->cempty,&q->m);}
static inline void SignalProducer(BQueue_t *q)	{SIGNAL(&q->cfull);}
static inline void SignalConsumer(BQueue_t *q)	{SIGNAL(&q->cempty);}

/*------------------Interfaccia della coda -----------------*/

BQueue_t *initBQueue(size_t n){

	BQueue_t *q = (BQueue_t*) calloc(sizeof(BQueue_t),1);
	if(!q){perror("malloc");return NULL;}
	q->buf = calloc(sizeof(void*),n);
	if(!q->buf){
		perror("malloc buf");
		goto error;
	}
	if (pthread_mutex_init(&q->m,NULL) != 0)
	{
		perror("pthread_mutex_init");
		goto error;
	}
	if (pthread_cond_init(&q->cfull,NULL) != 0)
	{
		perror("pthread_cond_init cfull");
		goto error;
	}
	if (pthread_cond_init(&q->cempty,NULL) != 0)
	{
		perror("pthread_cond_init cempty");
		goto error;
	}

	q->head = q->tail = 0;
	q->qlen = 0;
	q->qsize = n;
	return q;

  error:
	if(!q) return NULL;
	int myerrno = errno;
	if(q->buf) free(q->buf);
	if(&q->m) pthread_mutex_destroy(&q->m);
	if(&q->cfull) pthread_cond_destroy(&q->cfull);
	if(&q->cempty) pthread_cond_destroy(&q->cempty);
	free(q);
	errno = myerrno;
	return NULL;  
}

void deleteBQueue(BQueue_t * q, void (*F)(void *)){

	if(!q){
		errno = EINVAL;
		return;
	}
	if(F){
		void *data = NULL;
		while((data = pop(q))) F(data);
	}

	if(q->buf) free(q->buf);
	if(&q->m) pthread_mutex_destroy(&q->m);
	if(&q->cfull) pthread_cond_destroy(&q->cfull);
	if(&q->cempty) pthread_cond_destroy(&q->cempty);
	free(q);

}

int push(BQueue_t *q,void *data){
	if(!q || !data){
		errno = EINVAL;
		return -1;
	}

	LockQueue(q);
	while(q->qlen == q->qsize) WaitToProduce(q);
	assert(q->buf[q->tail] == NULL);
	q->buf[q->tail] = data;
	q->tail += (q->tail+1 >= q->qsize) ? (1-q->qsize) : 1;
	q->qlen += 1;

	SignalConsumer(q);
	UnlockQueue(q);
	return 0;
}

void *pop(BQueue_t *q){

	if(!q){
		errno = EINVAL;
		return NULL;
	}

	LockQueue(q);
	while(q->qlen == 0)WaitToConsume(q);
	void *data = q->buf[q->head];
	q->buf[q->head] = NULL;
	q->head+= (q->head+1 >= q->qsize) ? (1-q->qsize) : 1;
	q->qlen -= 1;
	assert(q->qlen>=0);

	SignalProducer(q);
	UnlockQueue(q);
	return data;

}