#include<stdio.h>
#include<stdlib.h>

#define CHECK_PTR_EXIT(M,s)  if(M==NULL){printf(s);}								




#define PRINTMAT(M,dimN,dimM)  for(size_t i=0;i<dimN;++i) { for(size_t j=0;j<dimM;++j) {printf("%ld ",M[i+(j*8)]);}printf("\n");}
								
									
 



#define dimN 16
#define dimM  8

#define ELEM(M,i,j)  M[i + (j*dimM)]
int main() {
    long *M = malloc(dimN*dimM*sizeof(long));
    CHECK_PTR_EXIT(M, "malloc"); 
    for(size_t i=0;i<dimN;++i)
	for(size_t j=0;j<dimM;++j)			
	    ELEM(M,i,j) = i+j;    
    
    PRINTMAT(M, dimN, dimM);
    free(M);
    return 0;
}