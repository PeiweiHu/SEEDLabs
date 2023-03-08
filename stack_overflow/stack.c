#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int foo(char *str) {
    char buffer[100];

	strcpy(buffer, str);
	return 1;
} 

int main(int argc, char **argv) {
	char str[400];
	FILE *badfile;

	badfile = fopen("badfile", "r");
	fread(str, sizeof(char), 300, badfile);
	foo(str);

	printf("Returned Properly\n");
	return 1;
}
