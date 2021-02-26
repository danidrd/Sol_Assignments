#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
	fprintf(stderr, "use: %s stringa1 stringa2\n", argv[0]);
	return -1;
    }

    char *saveptr[4];
    char *saveptr2[4];
    char* token1 = strtok_r(argv[1], " ",saveptr);

    while (token1) {
	printf("%s\n", token1);
	char* token2 = strtok_r(argv[2], " ",saveptr2);
	while(token2) {
	    printf("%s\n", token2);
	    token2 = strtok_r(NULL, " ",saveptr2);
	}
	token1 = strtok_r(NULL, " ",saveptr);
    }
    return 0;
}