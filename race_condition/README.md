# Race Condition 实验

## 1. 准备实验程序(vulp.c)

```c

```

按照如下命令对其进行编译并设置为setuid程序

```bash
gcc vulp.c -o vulp
sudo chown root vulp
sudo chmod 4755 vulp

# 关闭保护措施
# for ubuntu 16.04
sudo sysctl -w fs.protected_symlinks=0
# for ubuntu 12.04
sudo sysctl -w kernel.yama.protected_sticky_symlinks=0
```

## 2. 设置将要添加到/etc/passwd的用户行信息(passwd_input)

```bash
echo "test:U6aMy0Wojraho:0:0:test:/root:/bin/bash" > passwd_input
```

## 3. 设置攻击进程来不断更改链接的指向(attack_process.c)

```c

```

将其编译：`gcc -o attack_process attack_process.c`

## 4. 编写脚本不断触发实验程序，实现race condtion

```bash

```

## 5. 执行攻击

```bash
# 在窗口1执行attach_process来不断变换链接指向
./attack_process

# 在窗口2不断触发实验程序
bash target_process.sh

# 等待几分钟，会出现提示：STOP... The passwd file has been changed

# 查看效果
cat /etc/passwd
```

# Dirty Cow （在Ubuntu 12.04上进行）

## 1. 添加一个测试用户testcow

```bash
sudo adduser testcow
```

## 2. 准备攻击程序

```c

```

将其进行编译：`gcc dirty_cow.c -lpthread`生成a.out。

## 3. 执行攻击

```bash
# 执行如下程序几秒后手动ctrl+c结束
./a.out

# 查看结果
cat /etc/passwd
```
