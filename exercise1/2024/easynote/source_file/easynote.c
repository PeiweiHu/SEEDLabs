#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void read_input(char *buf,size_t size){
	int ret ;
    ret = read(0,buf,size);
    if(ret <=0){
        puts("Error");
        _exit(-1);
    }	
}

struct note {
	size_t size ;
	char title[8] ;
	int priority;
	char *content ;
};

struct note *notearray[5];

void menu(){
	puts("--------------------------------");
	puts("    Welcome to use Note Book    ");
	puts("--------------------------------");
	puts(" 1. Create a Note               ");
	puts(" 2. Edit a Note                 ");
	puts(" 3. Show a Note                 ");
	puts(" 4. Delete a Note               ");
	puts(" 5. Exit                        ");
	puts("--------------------------------");
	printf("Your choice :");
}

void create_note(){
	int i ;
	char buf[8];
	size_t size = 0;
	int priority = 0;
	for(i = 0 ; i < 5 ; i++){
		if(!notearray[i]){
			notearray[i] = (struct note *)malloc(sizeof(struct note));
			if(!notearray[i]){
				puts("Allocate Error");
				exit(1);
			}
			printf("Please input the size of Note : ");
			read(0,buf,8);
			size = atoi(buf);
			notearray[i]->content = (char *)malloc(size);
			if(!notearray[i]->content){
				puts("Allocate Error");
				exit(2);
			}
			notearray[i]->size = size ;

			printf("Please input the title of Note : ");
			read(0,notearray[i]->title,8);

			printf("Please input the priority of Note : ");
			read(0,buf,8);
			priority = atoi(buf);
			notearray[i]->priority = priority ;

			printf("Please input the content of Note:");
			read_input(notearray[i]->content,size);
			puts("Create successFully");
			break ;
		}
	}
}

void edit_note(){
	int idx ;
	char buf[4];
	printf("Index :");
	read(0,buf,4);
	idx = atoi(buf);
	if(idx < 0 || idx >= 5){
		puts("the idx is invalid!");
		_exit(0);
	}
	if(notearray[idx]){
		printf("Content of note : ");
		read_input(notearray[idx]->content,notearray[idx]->size+1);
		puts("Done !");
	}else{
		puts("No such note !");
	}
}

void show_note(){
	int idx ;
	char buf[4];
	printf("Index :");
	read(0,buf,4);
	idx = atoi(buf);
	if(idx < 0 || idx >= 5){
		puts("Out of bound!");
		_exit(0);
	}
	if(notearray[idx]){
		printf("Size : %ld\nContent : %s\n",notearray[idx]->size,notearray[idx]->content);
		puts("Done !");
	}else{
		puts("No such note !");
	}

}

void delete_note(){
	int idx ;
	char buf[4];
	printf("Index :");
	read(0,buf,4);
	idx = atoi(buf);
	if(idx < 0 || idx >= 5){
		puts("Out of bound!");
		_exit(0);
	}
	if(notearray[idx]){
		free(notearray[idx]->content);
		free(notearray[idx]);
		notearray[idx] = NULL ;
		puts("Done !");	
	}else{
		puts("No such note !");
	}

}


int main(){
	char buf[4];
	setvbuf(stdout,0,2,0);
	setvbuf(stdin,0,2,0);
	while(1){
		menu();
		read(0,buf,4);
		switch(atoi(buf)){
			case 1 :
				create_note();
				break ;
			case 2 :
				edit_note();
				break ;
			case 3 :
				show_note();
				break ;
			case 4 :
				delete_note();
				break ;
			case 5 :
				exit(0);
				break ;
			default :
				puts("Invalid Choice");
				break;
		}

	}
	return 0 ;
}
