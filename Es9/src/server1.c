#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include <util.h>

#define SOCKNAME "./mysock"
#define UNIX_PATH_MAX 108
#define SYSCALL(r,c,e)\
	if((r=c)==-1){perror(e);exit(errno); }
#define N 100

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

void cleanup(){
	unlink(SOCKNAME);
}
int main(int argc, char *argv[]){

	cleanup();

	
	int fd_skt,err,fd_c; char buf[N],buf2[N];
	struct sockaddr_un sa;
	strncpy(sa.sun_path, SOCKNAME, UNIX_PATH_MAX);
	sa.sun_family=AF_UNIX;
	SYSCALL(fd_skt,socket(AF_UNIX,SOCK_STREAM,0),"creando listening socket");
	SYSCALL(err,bind(fd_skt,(struct sockaddr *) &sa,sizeof(sa)),"binding");
	SYSCALL(err,listen(fd_skt,1),"listening");
  accept:	
	SYSCALL(fd_c,accept(fd_skt,NULL,0),"accepting connection");
	while(true){
		
		
		SYSCALL(err,read(fd_c,buf,N),"read");
		if(err == 0) {
			SYSCALL(err,close(fd_c),"close");
			goto accept;
		}
		memset(buf2,'\0',N);
		int n;

		if((n = cmd(buf,buf2)) == -1){
			fprintf(stderr, "Errore nella funzione cmd\n");
			return -1;			
		}
		
		SYSCALL(err,write(fd_c,buf2,N),"write");


		
		

	}

	return 0;
}