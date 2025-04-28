#include<stdio.h>
#include<stdlib.h>

int main(){
    char *a1 = malloc(0x100);
    char *b1 = malloc(0x20);
    char *c1 = malloc(0x100);
    char *d1 = malloc(0x20);

    free(a1);
    free(b1);
    free(c1);
    free(d1);

    char *a2 = malloc(0x100);
    char *b2 = malloc(0x20);
    char *c2 = malloc(0x100);
    char *d2 = malloc(0x20);

    printf("a1=%x a2=%x\n", a1, a2);
    printf("b1=%x b2=%x\n", b1, b2);
    printf("c1=%x c2=%x\n", c1, c2);
    printf("d1=%x d2=%x\n", d1, d2);
    
    return 0;
}

