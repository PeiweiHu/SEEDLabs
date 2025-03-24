all: 
	gcc -m32 -z execstack -o call_shellcode32 call_shellcode.c
	gcc -z execstack -o call_shellcode64 call_shellcode.c

clean:
	rm -f call_shellcode32 call_shellcode64 codefile_32 codefile_64