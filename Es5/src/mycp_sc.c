#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



#define BUFFERSIZE 256

int main(int argc, char *argv[]) {

    if (argc > 4 || argc < 3) {
	fprintf(stderr, "usage: %s filein fileout [buffersize] \n", argv[0]);
	return -1;
	}

	int fdi,len,fdo;
	FILE *ipf,*opf;

	
	if( (fdi = open(argv[1], O_RDONLY) )== -1){
		strerror(errno);
		exit(EXIT_FAILURE);
	}
	
	if( (fdo = open(argv[2],O_WRONLY) )==-1){
		strerror(errno);
		return -1;
	}

	if(argc==4){
		//buffer size esplicita
		//leggo la dimensione del buffer
		char* e = NULL;
   		long n = strtol(argv[3], &e, 0);
   		if (e != NULL && *e == (char)0);
   		else{
   			printf("Errore: La dimensione passata non è un numero esplicito!");
   			return -1;
   		}
		char buf[n];
		//Continuo con lettura e scrittura
		//Leggo da file di input, scrivo su buffer e scrivo su file di out
		while((len= read(fdi,buf,n)) > 0){
			if( write(fdo,buf,len) == -1){
				perror("%s: write"); strerror(errno); exit(EXIT_FAILURE); //?
			}
		}
		
		close(fdi);
		close(fdo);

		return 0;
	}

	//Caso buffer size implicita, utilizzo valore di default
	char buf[BUFFERSIZE];

	while((len= read(fdi,buf,BUFFERSIZE)) > 0){
			if( write(fdo,buf,len) == -1){
				perror("%s: write"); strerror(errno); exit(EXIT_FAILURE); //?
			}	
	}


	close(fdi);
	close(fdo);
	
	return 0;




}