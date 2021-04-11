
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#include <time.h>




int show_dir_content(char *path){
	
	struct stat s;
	

	//Da aggiungere controlli su stmode per i bit di protezione
	

	if( stat(path,&s) == 0 ){
				//Modificare stampa e gestire uid e guid, ctime va bene
				printf("File found!\nAbsolute path = %s  Inode = (%-7u)  Tipo del file =   User id =   Group id =   Size of File = %-10ld  Timestamp = %-s\n"
					 ,path,(unsigned)s.st_ino,/*s.st_mode,s.st_uid,s.st_gid,*/s.st_size,ctime(&s.st_mtime));
				fflush(stdout);			
				
				
	}else {

		perror("stat : ");
		return -1;

	}	
	
	return 0;
	
}

//Recursive searching of a filename into a directory(and sub-dir)
int main(int argc, char *argv[]) {

	if(argc<2){
		fprintf(stderr, "usage: %s file/dirname [file/dirname]\n", argv[0]);
	    exit(EXIT_FAILURE);
	}

	//Controllo che esistano tutti file
	char buf[1024];
	char *realpaths;

	
	while((errno=0, realpaths = realpath(argv[--argc],buf))){
		if(realpaths==NULL){
			perror("realpath : ");
			fprintf(stderr,"Il file/directory %s non esiste / realpath\n",argv[argc]);
			continue;
		}
		realpaths[strlen(realpaths)] = '\0';
		if((show_dir_content(realpaths))==-1){
			fprintf(stderr,"Il file/directory %s non esiste o problemi con stat\n",realpaths);
			continue;
		}
	}
	
	

	return 0;
	

}