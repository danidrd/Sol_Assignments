#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>




int main(int argc, char *argv[]) {

	if(argc < 2 ){

		printf("usage: at least one option between :: -n <number>-s <string>-m <number> -h");
		return -1;
	}



	int opt;

	int h=1;
	for(h=1;h<argc;h++){
		if(argv[h][0] == '-'){
			if(argv[h][1]){
				char letter = argv[h][1];

				if(letter == 'h'){
					printf("nome-programma -n <number> -m <number> -o <string> -h\n");
					return 0;
				}
			}
		}
	}
	

	while((opt = getopt(argc,argv,"n:m:o:h")) != -1){
		
		switch(opt){

			case '?': {
				printf("Errore: parametro %c non riconosciuto\n", opt);
				return -1;
			} break;

			default: {
				
				printf("parametro -%c = %s\n", opt, optarg);
				break;

			}

		}

	}

	return 0;

}