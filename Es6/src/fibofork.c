#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* 
 * Calcola ricorsivamente il numero di Fibonacci dell'argomento 'n'.
 * La soluzione deve effettuare fork di processi con il vincolo che 
 * ogni processo esegua 'doFib' al più una volta.  
 * Se l'argomento doPrint e' 1 allora la funzione stampa il numero calcolato 
 * prima di passarlo al processo padre. 
 */
static void doFib(int n, int doPrint);


int main(int argc, char *argv[]) {
    // questo programma puo' calcolare i numeri di Fibonacci solo fino a 13.  
    const int NMAX=13;
    int arg;
    
    if(argc != 2){
	fprintf(stderr, "Usage: %s <num>\n", argv[0]);
	return EXIT_FAILURE;
    }
    arg = atoi(argv[1]);
    if(arg <= 0 || arg > NMAX){
	fprintf(stderr, "num deve essere compreso tra 1 e 13\n");
	return EXIT_FAILURE;
    }   
    doFib(arg, 1);
    return 0;
}

static void doFib(int n, int doPrint){
    
    int m = n;
    int pid1,pid2;

    if(n>=2){

        if((pid1 = fork()) == 0 ){

            doFib(n-1,0);

        }else{
            if(pid1 == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if((pid2 = fork()) == 0 ){
                doFib(n-2,0);
            }else{

                if(pid2 == -1){
                    perror("fork");
                    exit(-1);
                }

                int status;
                if((waitpid(pid1,&status,0)) == -1){
                    perror("waitpid");
                    exit(EXIT_FAILURE);
                }
                m = WEXITSTATUS(status);
                if((waitpid(pid2,&status,0)) == -1){
                    perror("waitpid");
                    exit(EXIT_FAILURE);
                }
                m+= WEXITSTATUS(status);
            }


        }

    } 

    if(doPrint) printf("m=%d\n",m);
    return exit(m);
}