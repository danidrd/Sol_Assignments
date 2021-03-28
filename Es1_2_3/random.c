#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define N 10
#define K1 50
#define K2 60


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
		printf("usage: N K1 K2\n");
		n =(int) N;
		k1 = (int) K1;
		k2 = (int) K2;
	}

	if(argc>1){
		unsigned int seedp=time(NULL);
	
		//comandi inseriti
		float percentuali[k2-k1];
	
		long contatore[k2-k1];

		int i;

		for(i=0;i<k2-k1;i++){

			contatore[i]=0;
			

		}
		for(i=0;i<n;i++){

			int result =  rand_r(&seedp)%(k2-k1+1);	

			contatore[result]++;

		}
		
		

		for(i=0;i<k2-k1;i++){
			
			percentuali[i]=(float) (contatore[i] * 100)/n;
			float perc = percentuali[i];

			printf("%ld   : %f %%\n",i+k1, perc);

		}

	}else{

		unsigned int seedp=time(NULL);
	
		//comandi inseriti
		float percentuali[K2-K1];
	
		long contatore[K2-K1];

		int i;

		for(i=0;i<K2-K1;i++){

			contatore[i]=0;
			

		}
		for(i=0;i<N;i++){

			int result =  rand_r(&seedp)%(K2-K1+1);	

			contatore[result]++;

		}
		

		for(i=0;i<K2-K1;i++){
			
			percentuali[i]=(float) (contatore[i] * 100)/N;
			float perc = percentuali[i];

			printf("%d  : %f %%\n",i+K1, perc);

		}


	}

		return 0;

}