#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
	fprintf(stderr, "use: %s stringa1 stringa2\n", argv[0]);
	return -1;
    }

    int arg2len=strlen(argv[2]);


    char *saveptr[4];
    
    char* token1 = strtok_r(argv[1], " ",saveptr);

    while (token1) {
	printf("%s\n", token1);
	char* token2 = strtok(argv[2], " ");
	while(token2) {
	    printf("%s\n", token2);
	    token2 = strtok(NULL, " ");
	}
	int j = 0;
	while(j++ < arg2len){
		if(argv[2][j-1] == '\0'){
			argv[2][j-1] = ' ';
		}
	}

	token1 = strtok_r(NULL, " ",saveptr);
    }
    return 0;
}