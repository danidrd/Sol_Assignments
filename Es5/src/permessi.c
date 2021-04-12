
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>




int show_dir_content(char *path){
	
	struct stat s;
	char mode[11] = {'-','-','-','-','-','-','-','-','-','-','\0'};



	//Da aggiungere controlli su stmode per i bit di protezione
	

	if( stat(path,&s) == 0 ){

					if(S_ISDIR(s.st_mode)) mode[0] = 'd';
					if(S_ISCHR(s.st_mode)) mode[0] = 'c';
					if(S_ISBLK(s.st_mode)) mode[0] = 'b';
					if(S_ISLNK(s.st_mode)) mode[0] = 'l';
					if(S_ISFIFO(s.st_mode)) mode[0] = 'p';
					if(S_ISSOCK(s.st_mode)) mode[0] = 's';

					if(S_IRUSR & s.st_mode) mode[1] = 'r';
					if(S_IWUSR & s.st_mode) mode[2] = 'w';
					if(S_IXUSR & s.st_mode) mode[3] = 'x';


					if(S_IRGRP & s.st_mode) mode[4] = 'r';
					if(S_IWGRP & s.st_mode) mode[5] = 'w';
					if(S_IXGRP & s.st_mode) mode[6] = 'x';


					if(S_IROTH & s.st_mode) mode[7] = 'r';
					if(S_IWOTH & s.st_mode) mode[8] = 'w';
					if(S_IXOTH & s.st_mode) mode[9] = 'x';

					struct passwd *pw = getpwuid(s.st_uid);
					struct group *gr = getgrgid(s.st_gid);
				
				printf("File found!\nAbsolute path = %s  Inode = (%-7u)  Tipo del file = %-s  User id = %-s  Group id = %-s  Size of File = %-10ld  Timestamp = %-s\n"
					 ,path,(unsigned)s.st_ino,mode,pw->pw_name,gr->gr_name,s.st_size,ctime(&s.st_mtime));
				fflush(stdout);			
				
				
	}else {

		perror("stat : ");
		return -1;

	}	
	
	return 0;
	
}

//Looking for capabilites for specific dirnames/files
int main(int argc, char *argv[]) {

	if(argc<2){
		fprintf(stderr, "usage: %s file/dirname [file/dirname]\n", argv[0]);
	    exit(EXIT_FAILURE);
	}

	//Controllo che esistano tutti file
	char buf[1024];
	char *realpaths;

	
	while((errno=0, realpaths = realpath(argv[--argc],buf)) && argc>=1){
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