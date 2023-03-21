# 第一次作业

## 实验环境

实验在x86架构的**ubuntu 16.04 64bit**下进行，可通过docker配置环境（也可以通过安装虚拟机）。

```bash
sudo docker pull ubuntu:16.04
```

ubuntu 16.04的python版本较低，可能不兼容较新的pwntools，可以安装anaconda创建合适的python环境：

```bash
# 连接到docker容器后执行如下命令安装anaconda
wget https://repo.anaconda.com/archive/Anaconda3-2022.05-Linux-x86_64.sh
./Anaconda3-2022.05-Linux-x86_64.sh
```

## 题目介绍

+ Pwn1: 栈溢出的利用
    + 目标：获取shell
+ Pwn2: 堆溢出 - safe unlinking绕过（附加题）
    + 目标：获取shell

## 提示

+ IDA、GHIDRA、Radare2等可用于二进制的逆向，通过反汇编/反编译结果发现可能的漏洞点。
+ Pwntools (https://github.com/Gallopsled/pwntools)可用于生成shellcode，便捷地调试、编写漏洞利用脚本。
+ libc database search (https://libc.blukat.me) 可用于通过偏移计算绕过ASLR（非必需）。
+ GOT（Global Offset Table）作用

## 考核要求

+ 提交内容：exp代码，可当场演示讲解，若有简要解题报告write up或PPT可加分。
+ 对于能力强的同学，可以来挑战附加题，getshell成功并提交详细分析报告可加分。

更多问题：hupeiwei AT iie.ac.cn


