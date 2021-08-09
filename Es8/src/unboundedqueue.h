#if !defined(BOUNDED_QUEUE_H)
#define BOUNDED_QUEUE_H

#include <pthread.h>

/** Struttura dati coda
*
*/

typedef struct UBQueue
{
	void **buf;
	size_t head;
	size_t tail;
	size_t qsize;
	size_t qlen;

	pthread_mutex_t m;
	pthread_cond_t cfull;
	pthread_cond_t cempty;
} UBQueue_t;

/* Alloca ed inizializza una coda di dimensione \param n.
*  Deve essere chiamata da un solo thread(tipicamente dal main)
*
*  \retval NULL se vi sono problemi nell-allocazione(errno settato)
*  \retval q puntatore alla coda allocata
*/
UBQueue_t *initUBQueue(size_t n);


/** Cancella una coda allocata con initQueue. Deve essere chiamata da un
*	singolo thread.
*
* 	\param q puntatore alla coda da cancellare
*
*/

void deleteUBQueue(UBQueue_t *q,void (*F)(void*));

/** Inserisce un dato nella coda.
 * 	 \param data puntatore al dato da inserire
 * 	 
 * 	 \retval 0 if success
 * 	 \retval -1 se errore(errno settato opportunamente)
*/

int  	push(UBQueue_t *q,void *data);

/** Estrae un dato dalla coda
*	
*	\retval data puntatore al dato estratto
*/

void  	*pop(UBQueue_t *q);

#endif /* BOUNDED_QUEUE_H */