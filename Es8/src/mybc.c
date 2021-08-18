#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>

#include <util.h>

#define MAX_LINE_SIZE 1024

int cmd(const char str[],char *buf){

	int tobc[2];
	int frombc[2];

	int r;
	SYSCALL_RETURN(pipe, r, pipe(tobc), "pipe1", "");
	SYSCALL_RETURN(pipe, r, pipe(frombc), "pipe2", "");

	//Duplico il padre in modo da ottenere due copie delle pipe
	if(fork() == 0){
		//Chiudo scrittura su tobc e lettura su frombc
		SYSCALL_RETURN(close, r, close(tobc[1]), "close", "");
		SYSCALL_RETURN(close, r, close(frombc[0]), "close", "");

		//Redirigo stdin su tobc
		SYSCALL_RETURN(dup2, r, dup2(tobc[0],0), "dup2 child", "stdin");
		//Redirigo sia stdout che stderr sulla stessa pipe
		SYSCALL_RETURN(dup2, r, dup2(frombc[1],1), "dup2 child", "stdout");
		SYSCALL_RETURN(dup2, r, dup2(frombc[1],2), "dup2 child", "stderr");

		
		//Chiamo la execl in attesa di ricevere dati in input dal processo padre
		execl("/usr/bin/bc", "bc", "-lq", NULL);
		perror("execl");
		return -1;
	}

	//Chiudo i descrittori che non uso
	//Chiudo lettura su tobc e scrittura su frombc

	SYSCALL_RETURN(close, r, close(tobc[0]), "close", "");
	SYSCALL_RETURN(close, r, close(frombc[1]),"close","");

	int n;

	//Scrivo sulla pipe tobc che sara' in attesa di ricevere input
	SYSCALL_RETURN(write, n, write(tobc[1], (char*)str,strlen(str)), "write", "");
	//Leggo il risultato o l'errore
	SYSCALL_RETURN(read, n, read(frombc[0], buf, BUFSIZE), "readn", "");
	//Chiudo lo stdin di bc cosi da farlo terminare
	SYSCALL_RETURN(close, r, close(tobc[1]), "close", "");
	SYSCALL_RETURN(wait, r, wait(NULL), "wait", "");
	return n;
}

int main(int argc,char * argv[]){
	char result[BUFSIZE];

	while(true){
		char line[MAX_LINE_SIZE];

		memset(line,'\0',MAX_LINE_SIZE);
		if(fgets(line,MAX_LINE_SIZE, stdin) == NULL ){
			perror("fgets");
			return -1;
		}

		if(strncmp(line,"exit",4) == 0) break;

		int n;

		if((n = cmd(line,result)) < 0){
			fprintf(stderr, "Errore nell'esecuzione del comando\n");
			break;
		}

		result[n] = '\0';

		fprintf(stdout, "Operazione : %s", line);
		fprintf(stdout, "Risultato : %s\n", result);
	}
}