#include <dirent.h>
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


//lsdir directory, visita ricorsivamente tutto il sotto sotto-albero e stampa su stdout 
/*
Directory: <nomedir1>
file1     size    permessi
file2     size    permessi
------------------
Directory: <nomedir2>
file3     size    permessi
file4     size    permessi
------------------
*/


int show_dir_content(char *path,int depth,char *cwd){
	char buf[1024];
	struct stat s;
	char *realpaths;

	
	if(depth == 0) realpaths = realpath(path,buf);
	else realpaths = path;
	realpaths[strlen(realpaths)] = '\0';
	
	DIR *d; 
	

	if( stat(realpaths,&s) == 0 ){
		if( s.st_mode & S_IFDIR ){
			//Directory		
			
			if(chdir(realpaths) == -1){
				perror("chdir 2"); exit(EXIT_FAILURE);
			}
			cwd = realpaths;
			d = opendir(realpaths);

		}else if(s.st_mode & S_IFREG){
			//Regular file
			if(depth == 0){
				fprintf(stderr,"Il primo path %s non indica una cartella\n",realpaths);
				return -1;
			}

				char mode[11] = {'-','-','-','-','-','-','-','-','-','-','\0'};

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
							
			printf("Directory : %-s \n %-s\t%-10ld\t%-s \n\n",cwd,realpaths,s.st_size,mode);
			fflush(stdout);			
			return 1;
		}

	}

	
	
	d = opendir(realpaths);
	struct dirent *dir; //directory entries
	
	while((errno=0,dir=readdir(d)) != NULL){
		realpaths = realpath(dir->d_name,buf);
		
		if(!strcmp(dir->d_name,".")  || !strcmp(dir->d_name,"..")){
			continue;
		}

		if(dir->d_type == DT_DIR){
		
		
			if((show_dir_content(realpaths,1,realpaths))==1){

				
				if(chdir("..") == -1){
					perror("chdir, impossibile salire alla dir padre");
				}


		}

		

		}else if(dir->d_type == DT_REG){
		 
		 show_dir_content(realpaths,1,cwd);
		 
		}
		
	}	

	

	if(closedir(d) == -1){
		perror("closing cwd : "); exit(EXIT_FAILURE);

	}
	

	
	return 1;
	
}

//Recursive searching of a filename into a directory(and sub-dir)
int main(int argc, char *argv[]) {

	if(argc!=2){
		fprintf(stderr, "usage: %s startdir\n", argv[0]);
	    exit(EXIT_FAILURE);
	}
	
	if(show_dir_content(argv[1],0,argv[1]) == -1){



	}
	

	return 0;
	

}
