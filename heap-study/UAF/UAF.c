#include <stdio.h>
#include <stdlib.h>
typedef struct note {
  int size;
  char title[12];
  char *content;
  void (*func)(char *str);
} NOTE;
void myprint(char *str) { printf("%s\n", str); }

void main(){
    int op;
    int idx;
    int size;
    struct note* notes[16];

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
        if(idx<0 || idx>=16)
            continue;
        switch(op){
            case 1:
                notes[idx] = (NOTE *)malloc(sizeof(struct note));
                printf("title: \n");
                read(0, notes[idx]->title, 16);
                notes[idx]->title[15] = '\0';
                printf("content size: \n");
                scanf("%d", &size);
                notes[idx]->size = size;
                notes[idx]->content = (char *)malloc(size);
                notes[idx]->func = myprint;
                break;
            case 2:
                if(notes[idx]){
                    free(notes[idx]->content);
                    free(notes[idx]);
                }
                break;
            case 3:
                if(notes[idx])
                    notes[idx]->func(notes[idx]->content);
                break;
            case 4:
                if(notes[idx]){
                    printf("content: \n");
                    read(0, notes[idx]->content, notes[idx]->size);
                }
                break;
            default:
                break;
        }
    }
    return;
}
