#include <stdio.h>
#include <stdlib.h>

void main(){
    int op;
    int idx;
    int size;
    char* notes[16];
    int sizes[16];
    
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stderr, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);

    while(1){
        printf(" 1. Add   2. Delete\n");
        printf(" 3. Print 4. Edit\n");
        printf("option: \n");
        scanf("%d", &op);
        printf("index: \n");
        scanf("%d", &idx);
        getchar();
        if(idx<0 || idx>=16)
            continue;
        switch(op){
            case 1:
                printf("content size: \n");
                scanf("%d", &size);
                sizes[idx] = size;
                notes[idx] = (char *)malloc(size);
                break;
            case 2:
                if(notes[idx])
                    free(notes[idx]);
                break;
            case 3:
                if(notes[idx])
                    printf("%s", notes[idx]);
                break;
            case 4:
                if(notes[idx]){
                    printf("content: \n");
                    read(0, notes[idx], sizes[idx]);
                }
                break;
            default:
                break;
        }
    }
    return;
}
