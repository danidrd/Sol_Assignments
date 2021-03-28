#include <stdio.h>
#include <stdlib.h>
int somma(int x,int* saveptr){

	*saveptr+=x;
	return *saveptr;

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
	fprintf(stderr, "use: %s <number> \n", argv[0]);
	return -1;
    }

    long n;
    char* e = NULL;
   		n = strtol(argv[1], &e, 0);
   		if (e != NULL && *e == (char)0);
   		else{
   			printf("Errore: Inserisci il numero di scanf che vuoi eseguire!");
   			return -1;
   		}
   	int saveptr=INIT_VALUE;
   	int i,scan;
   	for(i=0;i<n;i++){

   		printf("Leggi il %d° numero \n",i+1);
   		scanf("%d",&scan);
   		printf("Somma al %d° numero: %d\n",i+1,somma(scan,&saveptr));
   	}

   	return 0;




}