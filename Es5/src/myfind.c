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




int show_dir_content(const char *path, const char *filename, size_t len,int depth){
	char buf[1024];
	struct stat s;
	char *realpaths;
	int isopen=0;

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
			isopen = 1;
			d = opendir(realpaths);

		}else if(s.st_mode & S_IFREG){
			//Regular file

			char *last = strrchr(path,'/');
			if(last != NULL){

				if(strcmp(last+1,filename) == 0){
				printf("File found!\n Absolute path = %s\n",path);
				fflush(stdout);			
				return 1;
				}

			}

			printf("File not found!\n Absolute path = %s\n",path);
			fflush(stdout);
			return -1;

		}

	}

	
	
	if(!isopen) d = opendir(realpaths);
	struct dirent *dir; //directory entries
	
	while((errno=0,dir=readdir(d)) != NULL){
		realpaths = realpath(dir->d_name,buf);
		
		if(!strcmp(dir->d_name,".")  || !strcmp(dir->d_name,"..")){
			continue;
		}

		if(dir->d_type == DT_DIR){
		
		
		if((show_dir_content(realpaths,filename,len,1))==1){

			
			if(chdir("..") == .1){
				perror("chdir, impossibile salire alla dir padre");
			}


		}

		

		}else if(dir->d_type == DT_REG){
		 
		 show_dir_content(realpaths,filename,len,1);
		 
		}
		
	}	

	

	if(closedir(d) == -1){
		perror("closing cwd : "); exit(EXIT_FAILURE);

	}
	

	
	return 1;
	
}

//Recursive searching of a filename into a directory(and sub-dir)
int main(int argc, char *argv[]) {

	if(argc!=3){
		fprintf(stderr, "usage: %s startdir filename\n", argv[0]);
	    exit(EXIT_FAILURE);
	}
	
	show_dir_content(argv[1],argv[2],strlen(argv[2]),0);
	

	return 0;
	

}