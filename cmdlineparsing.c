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

	if(argc < 2 ){

		printf("usage: at least one option between :: -n <number>-s <string>-m <number> -h");
		return -1;
	}



	long number=-1;
	char* string=NULL;
	
	char *opzione=NULL;
	int h=1;
	for(h=1;h<argc;h++){
		if(argv[h][0] == '-'){
			if(argv[h][1]){
				char letter = argv[h][1];

				if(letter == 'h'){
					printf("nome-programma -n <number> -m <number> -s<string> -h\n");
					return 0;
				}
			}
		}
	}


	int i=0;
	int j=0;
	while(++i<argc){
	  		
			
		switch(argv[i][0]){
			


			case '-':
			
			string = NULL;
			number = -1;
			opzione = NULL;
			j=0; 
			
			while(argv[i][++j]){

				switch(argv[i][j]){
					case '-': break;	
					
					case 'n':
						//caso -n10
						opzione = &argv[i][j];
						
						if(argv[i][++j]){
							
							if((number = isNumber(&argv[i][j])) == -1){
								printf("La stringa %s non è un parametro per l'opzione %c\n", &argv[i][j],*opzione);
								printf("sto in n");
								return -1;
							}
							printf("-n: %ld\n", number);
							
							opzione = NULL;

							
						}else{
							//caso -m 10
							if(argv[i+1]){
								if(argv[i+1][0] != '-'){
									

									if((number = isNumber(argv[i+1])) != -1){
										printf("-%c: %ld",*opzione,number);
									}else{
										printf("Errore: non ho trovato l'argomento per -n\n");
										return -1;

									}

								}else{
									printf("Errore: non ho trovato l'argomento per -n\n");
									return -1;
								}
							}else{
								printf("Errore: non ho trovato l'argomento per -n\n");
								return -1;

							}
							
							
						}
						

						break;

					case 'm':
						
							//caso -m10
						opzione = &argv[i][j];
						
						if(argv[i][++j]){
							
							if((number = isNumber(&argv[i][j])) == -1){
								printf("La stringa %s non è un parametro per l'opzione %c\n", &argv[i][j],*opzione);
								printf("sto in m");
								return -1;
							}
							printf("-m: %ld\n", number);
							
							opzione = NULL;

							
						}else{
							//caso -m 10
							if(argv[i+1]){
								if(argv[i+1][0] != '-'){

									if((number = isNumber(argv[i+1])) != -1){
										printf("-%c: %ld",*opzione,number);
									}else{
										printf("Errore: non ho trovato l'argomento per -m\n");
										return -1;

									}

								}else{
									printf("Errore: non ho trovato l'argomento per -m\n");
									return -1;
								}
							}else{
								printf("Errore: non ho trovato l'argomento per -m\n");
								return -1;

							}
							
							
						}
						

						break;

					case 's':
						opzione = &argv[i][j];
						
						//caso -s"ciao"
						if(argv[i][++j]){
							
							string = &argv[i][j];
							printf("-%c: %s\n", *opzione,string);
							
						
							opzione = NULL;
							
						}else if(argv[++i][0] != '-'){
							char* stringa = argv[i];
							printf("-%c: %s\n", *opzione,stringa);
							
							break;

						}else{
							printf("Errore: non ho trovato l'argomento per il comando -%c\n", *opzione);
							return -1;
						}


						break;

					case 'h':
					default:
					
					
							
						
						

					break;
						

				}

			}
			break;
			

		}

	}

}