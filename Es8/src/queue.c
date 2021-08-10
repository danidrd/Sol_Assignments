#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <queue.h>

/*--------------MACRO------------*/
#define LOCK(l)		if(pthread_mutex_lock(l) != 0)		{\
	fprintf(stderr, "ERRORE FATALE lock\n");			 \
	pthread_exit((void*)EXIT_FAILURE);					 \
}
#define UNLOCK(l)	if(pthread_mutex_unlock(l) != 0)	{\
	fprintf(stderr, "ERRORE FATALE unlock\n");			 \
	pthread_exit((void*)EXIT_FAILURE);					 \
}
#define WAIT(c,l)	if(pthread_cond_wait(c,l) != 0)		{\
	fprintf(stderr, "ERRORE FATALE wait\n");			 \
	pthread_exit((void*)EXIT_FAILURE);					 \
}
#define SIGNAL(c)	if(pthread_cond_signal(c) != 0 )	{\
	fprintf(stderr, "ERRORE FATALE signal\n");		 \
	pthread_exit((void*)EXIT_FAILURE);					\
}


/*--------------utilities--------------*/

static inline Node_t	*allocNode()		{return malloc(sizeof(Node_t));}
static inline Queue_t	*allocQueue()		{return malloc(sizeof(Queue_t));}
static inline void freeNode(Node_t *node)	{free((void*) node);}
static inline void LockQueue(Queue_t *q)	{LOCK(&q->qlock);}
static inline void UnlockQueue(Queue_t *q)	{UNLOCK(&q->qlock);}
static inline void UnlockQueueAndWait(Queue_t *q)	{WAIT(&q->qcond,&q->qlock);}
static inline void UnlockQueueAndSignal(Queue_t *q)	{SIGNAL(&q->qcond); UNLOCK(&q->qlock);}


/*---------------interfaccia della coda ---------------*/

Queue_t *initQueue(){
	Queue_t *q = allocQueue();
	if(!q) return NULL;
	q->head = allocNode();
	if(!q->head) return NULL;
	q->head->data = NULL;
	q->head->next = NULL;
	q->tail = q->head;
	q->qlen = 0;

	if(pthread_mutex_init(&q->qlock,NULL) != 0){
		perror("mutex init");
		return NULL;
	}

	if(pthread_cond_init(&q->qcond,NULL) != 0){
		perror("mutex cond");
		if(&q->qlock) pthread_mutex_destroy(&q->qlock);
		return NULL;
	}

	return q;
}

void deleteQueue(Queue_t *q){

	while(q->head != q->tail){
		Node_t *p = (Node_t*)q->head;
		q->head = q->head->next;
		freeNode(p);
	}

	if(q->head) freeNode((void *) q->head);
	if(&q->qlock) pthread_mutex_destroy(&q->qlock);
	if(&q->qcond) pthread_cond_destroy(&q->qcond);
	free(q);

}

int push(Queue_t *q,void *data){
	if((q==NULL) || (data == NULL)){ errno = EINVAL; return -1;}

	Node_t *n = allocNode();
	if(!n) return -1;
	n->data = data;
	n->next = NULL;

	LockQueue(q);
	q->tail->next = n;
	q->tail = n;
	q->qlen +=1;
	UnlockQueueAndSignal(q);
	return 0;

}

void *pop(Queue_t *q){
	if(q == NULL){errno = EINVAL; return NULL;}

	LockQueue(q);
	while(q->head == q->tail){
		UnlockQueueAndWait(q);
	}

	//locked
	assert(q->head->next);
	Node_t *n = (Node_t *) q->head;
	void *data = (q->head->next)->data;

	q->head = q->head->next;
	q->qlen -=1;
	assert(q->qlen >= 0);
	UnlockQueue(q);
	freeNode(n);
	return data;

}

unsigned long length(Queue_t *q){
	LockQueue(q);
	unsigned long len = q->qlen;
	UnlockQueue(q);
	return len;
}