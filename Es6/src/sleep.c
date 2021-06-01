#define _POSIX_C_SOURCE 200112L
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char * argv[]){

	int pid,status;

	if(argc != 2){
		fprintf(stderr,"usage sleep X (where x is the time)");
		exit(EXIT_FAILURE);
	}

	pid = fork();

	if(pid==0){
   	execl("/bin/sleep", "sleep", argv[1],(char*) NULL);		
	}

	if((waitpid(pid,&status,0) == -1)){
		perror("waitpid");
		exit(EXIT_FAILURE);
	}
   	printf("my pid : %d, my father pid : %d\n",getpid(),getppid());

}