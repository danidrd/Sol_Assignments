#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

typedef int F_t(const void *s1,const void *s2, size_t n);

int confronta(F_t *fun, float *m1, float *m2, size_t size){

	return fun(m1,m2,size);

}

int main(int argc,char *argv[]){

	if(argc<4){
		fprintf(stderr,"usage ./readbinmat.o <pathToTxtMat> <pathToDatMat> <dim>");
		return -1;
	}

	FILE *ifp,*ifpb;
	ifp = fopen(argv[1],"r");
	ifpb = fopen(argv[2],"rb");

	char* e = NULL;
   	long n = strtol(argv[3], &e, 0);
	
	
	float *M1 = calloc(n*n,sizeof(float));;
	float *M2 = calloc(n*n,sizeof(float));;

	//Leggo matrice testuale
	int i=0;
	while (fscanf(ifp, "%f", &M1[i++]) != EOF);

	//Leggo matrice binaria
	if(fread(M2,sizeof(float),n*n,ifpb) == 0){
			printf("errore!\n");
			return -3;
	} 


	printf("memcmp result : %d\n", confronta(&memcmp,M1,M2,n*n*sizeof(float)));
	
	fclose(ifp);
	fclose(ifpb);

}