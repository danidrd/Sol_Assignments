#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>



#define BUFFERSIZE 512

int main(int argc, char *argv[]) {

    if (argc > 4 || argc < 3) {
	fprintf(stderr, "usage: %s filein fileout [buffersize] \n", argv[0]);
	return -1;
	}

	
	FILE *ipf,*opf;

	
	if( (ipf = fopen(argv[1],"r")  ) ==  NULL){
		strerror(errno);
		exit(EXIT_FAILURE);
	}
	
	if( (opf = fopen(argv[2],"w") )== NULL){
		strerror(errno);
		return -1;
	}

	fseek(ipf,0L,SEEK_END);
	int sz = ftell(ipf);
	
	rewind(ipf);

	int len=0;
	int lenstat=0; //Utilizzata per terminazione del while, indica la crescita dei byte letti in totale

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
		while((len = fread(buf,1,n,ipf))  < sz && lenstat<sz) {
			lenstat+=len;
			if( fwrite(buf,1,len,opf) == -1){
			perror("%s: write"); strerror(errno); exit(EXIT_FAILURE); //?
			}
		}
		
		
		
		fclose(ipf);
		fclose(opf);

		return 0;
	}

	//Caso buffer size implicita, utilizzo valore di default
	char buf[BUFFERSIZE];
	if(BUFFERSIZE>=sz){
		//Caso speciale in cui il while non continua
		//Utilizzo sz per leggere tutto il file
		len = fread(buf,1,sz,ipf);
		if( fwrite(buf,1,len,opf) == -1){
		perror("%s: write"); strerror(errno); exit(EXIT_FAILURE); //?
		}

		fclose(ipf);
		fclose(opf);

		return 0;

	}
	while((len = fread(buf,1,BUFFERSIZE,ipf))  < sz && lenstat<sz) {
		lenstat+=len;
		if( fwrite(buf,1,len,opf) == -1){
		perror("%s: write"); strerror(errno); exit(EXIT_FAILURE); //?
		}
	}

		fclose(ipf);
		fclose(opf);
	
	return 0;




}