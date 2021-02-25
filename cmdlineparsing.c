#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}



int main(int argc, char *argv[]) {

	if(argc < 2  ||  argc> 8){

		printf("usage: at least one option between :: -n <number>-s <string>-m <number> -h");
		return -1;
	}

	long number=-1;
	char* string=NULL;
	int issNumber=0;
	int isString=0;
	char *opzione=NULL;

	int i=0;
	while(++i<argc){

		switch(argv[i][0]){

			default: 
				if(issNumber==2){

					if((number = isNumber(argv[i])) == -1){
						printf("La stringa %s non è un parametro per l'opzione %s\n", argv[i],opzione);
						return -1;
					}
					printf("-n: %ld\n", number);
					issNumber=0;	
					break;

				}

				if(isString==2){

					if((number = isNumber(argv[i])) != -1){
						printf("La stringa %s non è un parametro per l'opzione %s\n", argv[i],opzione);
						return -1;
					}
					printf("-s: %s\n", argv[i]);
					isString=0;	
					break;

				}

				printf("Errore: %s non è un parametro coretto\n", argv[i]);
				return -1;


			case '-': 
			issNumber = 0;
			isString=0;
			string = NULL;
			number = -1;
			opzione = NULL;
			while(*++argv[i]){

				switch(*argv[i]){
					case '-': break;	
					
					case 'n':
						//caso -n10
						opzione = argv[i];
						++argv[i];
						if(*argv[i]){
							
							if((number = isNumber(argv[i])) == -1){
								printf("La stringa %s non è un parametro per l'opzione -%s\n", --argv[i],opzione);
								return -1;
							}
							printf("-n: %ld\n", number);
							issNumber=1;//Devo avvisare lo switch interno che ho letto un numero
							opzione = NULL;

							break;
						}else{
							//caso -n 10
							issNumber = 2;//Devo avvisare entrambi gli switch che la prossima stringa è un numero
							break;
						}

					case 'm':
						
						opzione = argv[i];
						++argv[i];
						//caso -m10
						if(*argv[i]){
							
							if((number = isNumber(argv[i])) == -1){
								printf("La stringa %s non è un parametro per l'opzione -%s\n", --argv[i],opzione);
								return -1;
							}
							printf("-m: %ld\n", number);
							issNumber=1;//Devo avvisare lo switch interno che ho letto un numero
							opzione = NULL;
							break;
						}else{
							//caso -n 10
							issNumber = 2;//Devo avvisare entrambi gli switch che la prossima stringa è un numero
							break;
						}
					case 's':
						opzione = argv[i];
						++argv[i];
						//caso -s"ciao"
						if(*argv[i]){
							
							string = argv[i];
							printf("-s: %s\n", string);
							isString=1;//Devo avvisare lo switch interno che ho letto una stringa
							opzione = NULL;
							break;
						}else{
							//caso -s "ciao"
							isString= 2;//Devo avvisare entrambi gli switch che la prossima stringa è una stringa
							break;
						}

					case 'h':
					default:
						if(issNumber==1){

							break;

						}else if(issNumber == 2){
							break;
						}

						if(isString==1){

							break;

						}else if(isString == 2){
							break;
						}

						printf("Errore: %s non è un parametro coretto\n", --argv[i]);
						return -1;

				}

			}

		}

	}

}