#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc,char *argv[]){

	if(argc<2){
		fprintf(stderr,"usage ./binmat.o <dim> | dim<=512");
		return -1;
	}

	char* e = NULL;
   	long n = strtol(argv[1], &e, 0);

	FILE *fpb,*fp;
	fp = fopen("./opf/mat_dump.txt","w");
	if(!fp){
		strerror(errno);
		return -1;
	}
	fpb = fopen("./opf/mat_dump.dat","wb");
	if(!fpb){
		strerror(errno);
		return -1;
	}

	if(n>512){
		fprintf(stderr,"argv[1] must be less or equal to 512");
		return -2;
	}

	float* M1 = calloc(n*n,sizeof(float));
	
	int i,j;

	for(i=0;i<n;i++){
		for(j=0;j<n;j++){

			M1[(i*n)+j] = (float) (i+j)/2;

			fprintf(fp,"%f ",M1[(i*n)+j]);
			

		}
	}

	fwrite(M1,sizeof(float),n*n,fpb);
	fclose(fpb);
	fclose(fp);
	free(M1);




}