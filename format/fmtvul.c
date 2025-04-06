// fmtvul.c

#include "stdio.h"

void fmtstr(char *str) {
	unsigned int *framep;
	unsigned int *ret;

	// copy ebp into framep
	asm("movl %%ebp, %0" : "=r" (framep));
	ret = framep + 1;

	printf("address of input array: 0x%.8x\n", (unsigned)str);
	printf("value of the frame pointer: 0x%.8x\n", (unsigned)framep);
	printf("value of the return address: 0x%.8x\n", (unsigned)*ret);

	printf(str);
}

int main(int argc, char **argv) {
	FILE *badfile;
	char str[200];

	badfile = fopen("badfile", "rb");
	fread(str, sizeof(char), 200, badfile);
	fmtstr(str);

	return 1;
}