#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int count_string_words(char *line){
	int words=0;
	char* token2 = strtok(line, " ");
	while(token2) {
	    token2 = strtok(NULL, " ");
	    words++;
	}

	return words;

}

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
	
	int lw = (l && w) || (!l && !w); //entrambe le opzioni abilitate oppure omesse

	int lines=0,words=0;//righe e parole per il file argv[i]
	int i;
	//Inizio dall'indice dopo scansione getopt
	for(i=optind;i<argc;i++){
		//Per ogni nome file
		//Controllo che esista il file in memoria
		if(access(argv[optind],R_OK) != 0) {
    		fprintf(stderr,"La stringa %s non indica un file readable", argv[i]);
    		return -1;
		}
		FILE *ifp;
		ifp = fopen(argv[i], "r");
		//Lettura
		if(!ifp){
		strerror(errno);
		return -1;
		}
		

		fseek(ifp,0L,SEEK_END);
		int sz = ftell(ifp);
		char s[sz + 2];
		//Necessario per rileggere il file dopo la fseek
		rewind(ifp);
		
		//Restituisco i risultati voluti '-l o -w, oppure entrambi per il file dell'iterazione corrente
		if(lw){
			// Utilizzo entrambe le opzioni
			

			while((fgets(s,sz+2,ifp))!= NULL){
					
				int words_string=0;//parole nella stringa	
					
				if((words_string = count_string_words(s))<=-1){
					//Errore, nessuna parola nella stringa		
					fprintf(stderr,"Errore nella funzione count_string_words");
					return -1;
				}else if(words_string>-1){
					words+=words_string;
					lines++;
				}
			}

			fprintf(stdout,"Opzione -lw abilitata: %d righe/%d parole contate in totale nel file %s\n",lines,words,argv[i]);
			
			
			

		}else if(l){
			//Utilizzo solo opzione l
			
			
			

			while((fgets(s,sz+2,ifp))!= NULL){		
				lines++;			
			}

			fprintf(stdout,"Opzione -l abilitata: %d righe contate nel file %s\n",lines,argv[i]);
			
			
			
		}else if(w){
			//-w 
			

			
			
				
				while((fgets(s,sz+2,ifp))!= NULL){
					
					int words_string=0;//parole nella stringa	
					
					if((words_string = count_string_words(s))<=-1){
						//Errore, nessuna parola nella stringa		
						fprintf(stderr,"Errore nella funzione count_string_words");
						return -1;
					}else if(words_string>-1){
						words+=words_string;
						
					}

				}
				fprintf(stdout,"Opzione -w abilitata: %d parole contate nel file %s\n",words,argv[optind]);
			
			

			
		}
		
		lines = 0 ; 
		words = 0;

	}
	return 0;

	
}