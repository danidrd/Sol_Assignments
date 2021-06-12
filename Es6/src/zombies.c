#define _POSIX_C_SOURCE 200112L
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>


int main (int argc,char *argv[]){

	if(argc != 2){
		fprintf(stderr,"usage zombies N");
		exit(EXIT_FAILURE);
	}

	char* e = NULL;
   	long n = strtol(argv[1], &e, 0);
   	if (e != NULL && *e == (char)0);
   	else{
   		printf("Errore: il primo parametro non è un numero!");
   		return -1;
   		};


 	long i = 0 ; 
 	for(i=0;i<n;i++){
 		int pid = fork();
 		if(pid < 0 ){
 			perror("fork");
 			exit(EXIT_FAILURE);
 		}
 		if(pid == 0 )execl("/bin/echo","echo","ciao","ciao","sono","daniele", (char*)NULL);
 	}

 	sleep(20);
 	
}