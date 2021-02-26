#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>


typedef int F_t(char*);

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int arg_n(const char* x){
	if(x!= NULL){
		long number;
		if((number=isNumber(x)) != -1){
			printf("parametro -n = %ld\n", number);
		}else{
			printf("the argument of -n is Not a number!");
			return -1;
		}
	}else return -1;
	
}

int arg_m(const char* x){

	if(x!= NULL){
		long number;
		if((number=isNumber(x)) != -1){
			printf("parametro -m = %ld\n", number);
		}else{
			printf("the argument of -m is Not a number!");
			return -1;
		}
	}else {
		return -1;
	}

}

int arg_o(const char* x){

	if(x!= NULL){
		long number;
		if((number=isNumber(x)) == -1){
			printf("parametro -o = %s\n", x);
		}else{
			printf("Error: the argument of -o is a number, expected string");
			return -1;
		}
	}else return -1;

}

int arg_h(const char* x){

	if(x == "help"){
		printf("nome-programma -n <number> -m <number> -o <string> -h\n");
		exit(0);
	}else return -1;

}

int main(int argc, char* argv[]) {
 
  
  if(argc < 2 ){

	printf("usage: at least one option between :: -n <number>-s <string>-m <number> -h");
	return -1;
  }

  F_t (*V[4]); //Vettore di funzioni 
  //V=malloc(sizeof(F_t)*4);

  
  char* help = NULL;
  int h=1;
	for(h=1;h<argc;h++){
		if(argv[h][0] == '-'){
			if(argv[h][1]){
				char letter = argv[h][1];

				if(letter == 'h'){
					help = "help";
					//printf("nome-programma -n <number> -m <number> -o <string> -h\n");
					//return 0;
				}
			}
		}
	}

 	V[2] = arg_n;
    V[1] = arg_m;
 	V[3] = arg_o;
    V[0] = arg_h;

    
    V[0](help);
    


  int opt;
  while ((opt = getopt(argc,argv, "n:m:o:h")) != -1) {
    switch(opt) {
    case '?': { 
       printf("Errore: opzione non riconosciuta, usa -h per help");
       return -1;
    } break;
    default:
     // invocazione della funzione di gestione passando come parametro l'argomento restituito da getopt
     if (V[opt%4]( (optarg==NULL ? argv[0] : optarg) ) == -1) {
      printf("Errore: parametro dell'opzione errato!");
      return -1;
     }
    }
  }
  return 0; 
}