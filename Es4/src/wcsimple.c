#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char *argv[]){

	if(argc<2){
		fprintf(stderr,"usage wcsimple [-l -w] file1 [file2 file3 ...]");
		return -1;
	}


	int opt,l=0,w=0;
	
	while((opt = getopt(argc,argv,"lw")) != -1){
		
		switch(opt){

			case '?': {
				printf("Errore: parametro %c non riconosciuto\n", opt);
				return -1;
			} break;

			default: {
				if (opt == 'l') l = 1;
				if (opt == 'w') w = 1;
				printf("parametro -%c = %s\n", opt, optarg);
				break;

			}

		}

	}
	
	int lw = l && w; //entrambe le opzioni abilitate, se vero

	
	int i;
	//Inizio dall'indice dopo scansione getopt
	for(i=optind;i<argc;i++){
		//Per ogni nome file
		//Controllo che esista il file in memoria
		if( access( argv[optind], R_OK ) != 0 ) {
    		fprintf(stderr,"La stringa %s non indica un file readable", argv[optind]);
    		return -1;
		} 

		//Restituisco i risultati voluti '-l o -w, oppure entrambi' per quel file
		if(lw){
			// Utilizzo entrambe le opzioni
			printf("sono in lw");
		}else if(l){
			//Utilizzo solo opzione l
			//-l basta contare le line del file con fgets (esempio in passwd.c)
			printf("sono in l");
		}else if(w){
			//-w scrivere funzione che conta parole di ogni line
			printf("sono in w");
		}
		


	}

	return 0;
}