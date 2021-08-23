#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>


/**
 *@file threadpool.h
 *@brief interfaccia per il threadPool
 */

/**
 *@struct taskfun_t
 *@brief generico task che un thread del threadpool deve eseguire
 *
 *@var fun puntatore alla funzione da eseguire
 *@ arg Argomento della funzione
 */

typedef struct taskfun_t{
	void (*fun)(void *);
	void *arg;
}taskfun_t;

/**
 *@struct threadpool
 *@brief rappresentazione dell-oggetto threadpool
 */

typedef struct threadpool_t{
	pthread_mutex_t lock; //Mutua esclusione nell-accesso all' oggetto
	pthread_cond_t cond; //Usata per notificare un worker thread
	pthread_t 	* threads; //Array di worker id
	int numthreads; // numero di thread(size dell'array threads)
	taskfun_t *pending_queue; // coda interna per task pendenti
	int queue_size; // Massima size della coda, puo' essere -1 indicando che non si vogliono task pendenti
	int taskonthefly; // Task attualmente in esecuzione
	int head,tail; //Riferimenti della coda
	int count; //numero di task nella coda dei task pendenti
	int exiting; // se > 0 e' iniziato il protocollo di uscita, se 1 il thread aspetta che non ci siano piu' lavori in coda

}threadpool_t;

/**
 *@function createThreadPool
 *@brief Crea un oggetto thread pool.
 *@param numthreads ee' il numero di thread del pool 
 *@param pending_size e' la size delle richieste che possono essere pendenti.
 * Questo parametro e' 0 se si vuole utilizzare un modello per il pool con 1 thread 1 richiesta, cioe'
 * non ci sono richieste pendenti 
 *
 *
 *@return un nuovo threadpool oppure NULL ed errno settato opportunamente
 */

threadpool_t * createThreadPool(int numthreads, int pending_size);

/**
 *@function destroyThreadPool
 *@brief stoppa tutti i thread e distrugge l-oggetto pool
 *
 *@param pool oggetto da liberare
 *@param force se 1 forza l'uscita immediatamente di tutti i thread e libera subito le risorse
 *,se 0 aspetta che i thread finiscano tutti e soli i lavori pendendi senza accettarne altri
 *
 *@return 0 in caso di successo, < 0 in caso di falalimento ed errno settato
 */

int destroyThreadPool(threadpool_t *pool, int force);

/**
 * @function addTaskToThreadPool
 * @brief aggiunge un task al pool, se ci sono thread liberi il task viene assegnato ad uno
 * di questi, se non ci sono thread liberi e pending_size> 0 allora si cerca di inserire il task
 * come task pendente. Se non c'e' posto nella coda interna allora la chiamata fallisce
 * 
 * @param pool oggetto thread pool
 * @param fun funzione da eseguire per eseguire il task
 * @param arg argomento della funzione
 * 
 * @return 0 se successo, 1 se non ci sono thread disponibili e/o la coda e' piena, -1 in caso di fallimento
 * , errno settato opportunamente
 * 
 */

int addToThreadPool(threadpool_t * pool, void (*fun)(void *), void *arg);

/**
 * @function spawnThread
 * @brief lancia un thread che esegue la funzione fun passata come parametro, il thread viene 
 * lanciato in modalita' detached e non fa parte del pool
 * @param fun funzione da eseguire per eseguire il task
 * @param arg argomento della funzione
 * @return 0 se successo , -1 in caso di fallimento, errno viene settato opportunamente
 * 
 */

 int spawnThread(void (*fun)(void *), void *arg);

 #endif /* THREADPOOL_H */ 
