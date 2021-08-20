#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t sigint_flag = 0;
volatile sig_atomic_t sigtstp_flag = 0;

#define ec_meno1(s,m)\
	if( (s) == -1){perror(m);exit(EXIT_FAILURE);}

void gestoreINT(int sig){
	sigint_flag++;

}

void gestoreTSTP(int sig){
	sigtstp_flag++;
	char buf[sizeof(sig_atomic_t)];
	*buf = sigint_flag + '0';
	write(1,buf,sizeof(sig_atomic_t));
}


int main(void){

	struct sigaction s;char buf[128];
	memset( &s, 0, sizeof(s));
	s.sa_handler=gestoreINT;
	ec_meno1(sigaction(SIGINT,&s,NULL),"sigaction");
	s.sa_handler=gestoreTSTP;
	ec_meno1(sigaction(SIGTSTP,&s,NULL),"sigaction");

	while(sigtstp_flag != 3);

	alarm(10);
	fprintf(stdout, "Il processo deve continuare (yes, no)!\n");
	read(0,buf,128);
	if(strncmp(buf,"yes",3) == 0){
		alarm(0);

		fprintf(stdout, "Processo continuato e terminato con successo\n");
		return 0;
	}else if(strncmp(buf,"no",2) == 0 ){
		alarm(0);
		
		fprintf(stdout, "Processo terminato con successo\n");
		return 0;
	}

}