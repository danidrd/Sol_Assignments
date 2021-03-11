#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// esempio di tokenizzazione di stringhe con strtok_r

void tokenizer_r(char *stringa) {
   
    int j = 0;
	while(j++ < strlen(stringa)){
		if(stringa[j-1] == '\0'){
			stringa[j-1] = ' ';
		}
	}

	 char *tmpstr;
    char *token = (char*)strtok_r(stringa, " ", &tmpstr);
    while (token) {
      printf("%s\n", token);
      token = (char*)strtok_r(NULL, " ", &tmpstr);
    }
}
