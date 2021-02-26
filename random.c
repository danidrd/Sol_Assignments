#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define N 1000
#define K1 100
#define K2 120


int main(int argc, char* argv[]) {
	
	long k1,k2,n; 
	if(argc>1){

		char* e = NULL;
   		n = strtol(argv[1], &e, 0);
   		if (e != NULL && *e == (char)0);
   		else{
   			printf("Errore: il primo parametro non è un numero!");
   			return -1;
   		}
   		char* e1 = NULL;
   		k1 = strtol(argv[2], &e1, 0);
   		if (e1 != NULL && *e1 == (char)0);
   		else{
   			printf("Errore: il secondo parametro non è un numero!");
   			return -1;
   		}
   		char* e2 = NULL;
   		k2 = strtol(argv[3], &e2, 0);
   		if (e2 != NULL && *e2 == (char)0);
   		else{
   			printf("Errore: il terzo parametro non è un numero!"); 
   			return -1;
   		}

   		

	}else{
		printf("usage: N K1 K2");
		n = N;
		k1 = K1;
		k2 = K2;
	}


	unsigned int seedp=time(NULL);
	
		//comandi inseriti
		float percentuali[k2-k1];
	
		float contatore[k2-k1];

		int i;

		for(i=0;i<k2-k1;i++){

			contatore[i]=0;
			

		}
		for(i=0;i<n;i++){

			int result = rand_r(&seedp)%k2;
			
			contatore[result]++;

		}
		

		for(i=0;i<k2-k1;i++){
			
			percentuali[i]=(float) (contatore[i] * 100/n)*n;
			float perc = percentuali[i]/n;

			printf("%ld   : %f %%\n",i+k1, perc);

		}


		return 0;

}