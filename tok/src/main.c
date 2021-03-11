#define _POSIX_C_SOURCE 200112L
#include"tokenizer.h"
#include"tokenizer_r.h"
#include <stdio.h>


int main(int argc, char *argv[]) {
	//char *stringa=strcpy(stringa,argv[1]);
    for(int i=1;i<argc;++i){
    	 tokenizer(argv[i]);
    		
		 //tokenizer_r(stringa);
    }
  
    return 0;
}
