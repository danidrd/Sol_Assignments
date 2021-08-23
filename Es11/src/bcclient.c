#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <util.h>
#include <conn.h>

int main(int argc, char *argv[]){
	struct sockaddr_un serv_addr;
	int sockfd;
	SYSCALL_EXIT("socket", sockfd, socket(AF_UNIX, SOCK_STREAM, 0), "socket", "");
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sun_family = AF_UNIX;
	strncpy(serv_addr.sun_path, SOCKNAME, strlen(SOCKNAME) + 1);

	int notused,n;

	SYSCALL_EXIT("connect", notused, connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)), "connect", "");

	char buf[BUFSIZE];

	do{
		memset(buf, '\0', BUFSIZE);
		if(fgets(buf, BUFSIZE-1, stdin) == NULL) break;
		if(strncmp(buf, "quit", 4) == 0) break;

		int len = strlen(buf);
		

		if( (n=writen(sockfd, &len, sizeof(int))) == -1){
			perror("write1");
			break;
		}

		if( (n=writen(sockfd, buf, strlen(buf))) == -1 ){
			perror("write2");
			break;
		}

		if( (n=readn(sockfd,&len,sizeof(int))) ==-1 ){
			perror("read1");
			break;
		}

		if( (n=readn(sockfd,buf,len)) == -1){
			perror("read2");
			break;
		}

		fprintf(stdout, " Risultato: %s\n", buf);
	}while(1);

	close(sockfd);
	return 0;

}