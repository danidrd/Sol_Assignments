#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc,char *argv[]){


	int sz;

	FILE *ifp, *opf;
	ifp = fopen("/etc/passwd" , "r");
	//Lettura
	if(!ifp){
		strerror(errno);
		return -1;
	}
	fseek(ifp,0L,SEEK_END);
	sz = ftell(ifp);
	char s[sz + 2];
	rewind(ifp);

 	opf = fopen(argv[1],"w");
 	//Scrittura
 	if(!opf){
		strerror(errno);
		return -1;
	}

	while((fgets(s,sz+2,ifp))!= NULL){
		//Scrivo su file di output
		//Seleziono il primo token 
		
		char *limit;
		limit = (char*)strchr(s,':');
			
		
		//Trovo l'indice finale del token
		int j=0;
		while(&(s[j]) < limit){
			j++;
		}
		//Trovo la dimensione del token
		int rst = strlen(limit);
		int sztok = strlen(s) - rst;
		int i=0;
		//Scrivo carattere per carattere il login name su opf
		while(i++<sztok){
			fprintf(opf, "%c", s[i-1]);

		}
		fprintf(opf,"\n");


	}

	fclose(ifp);
	fclose(opf);





}