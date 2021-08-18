#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <util.h>

void processo1(int in, int out, const char arg[]){
	fprintf(stderr, "Processo1 %d started\n", getpid());

	dup2(in,0);//close(0);dup(in);
	dup2(out,1);

	//Chiudo tutti i descrittori che non uso prima di chiamare la exec

	close(in);
	close(out);


	execl("./dec", "dec", arg, NULL);

	perror("execl");
	exit(errno);
}

void processo2(int in, int out){
	fprintf(stderr, "Processo1 %d started\n", getpid());

	dup2(in,0);//close(0);dup(in);
	dup2(out,1);

	//Chiudo tutti i descrittori che non uso prima di chiamare la exec

	close(in);
	close(out);


	execl("./dec", "dec", NULL);

	perror("execl");
	exit(errno);
}

int main(int argc, char *argv[]){
	int x;
	if(argc==2){
		x = atoi(argv[1]);
		if(x<0) {
			fprintf(stderr, "x deve essere positivo!\n");
			return -1;
		}
	}else{
		fprintf(stderr, "usa : %s n\n",argv[0]);
		return EXIT_FAILURE;
	}

	int canale12[2]; //canale da processo 1 a 2
	int canale21[2]; //canale da processo 2 a 1
	int r;
	SYSCALL_RETURN(pipe, r, pipe(canale12),"pipe1","");
	SYSCALL_RETURN(pipe, r, pipe(canale21), "pipe2", "");

	if(fork() == 0){
		//primo figlio, chiudo scrittura canale21 e lettura canale 12
		close(canale21[1]);
		close(canale12[0]);
		processo1(canale21[0],canale12[1],argv[1]);
	} else{
		//Secondo figlio
		close(canale12[1]);
		close(canale21[0]);
		if(fork() == 0){
			processo2(canale12[0],canale21[1]);
		}else{
			//Processo padre, non usa pipe quindi chiude tutti i descrittori che non usa
			close(canale12[0]);
			close(canale12[1]);
			close(canale21[0]);
			close(canale21[1]);
		}
	}

//attendo la terminazione del processo figlio
	int status;
	SYSCALL_RETURN(waitpid, r, waitpid(-1,NULL,0),"waitpid1","");
	fprintf(stderr, "Processo %d terminato, status = %d\n",r ,WEXITSTATUS(status));
	SYSCALL_RETURN(waitpid, r, waitpid(-1,NULL,0), "waitpid2", "");
	fprintf(stderr, "Processo %d terminato, status = %d\n",r ,WEXITSTATUS(status));
	return 0;


}