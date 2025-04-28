

可以自己 Build:

```
docker build -t "easynote" .
```

或者加载镜像文件：

```
docker load < pwn_easynote.tar
```

运行：

```
docker run -d -p "0.0.0.0:9999:9999" -h "easynote" -v {/mypath/flag}:/home/ctf/flag --name="easynote" easynote
```



flag 存放位置：/home/ctf/flag

flag 存取方法：更新宿主机上的flag即可修改 docker 内部的 flag(/mypath/flag 为宿主机存取 flag 的位置。)

docker内部服务端口：9999





docker run -d -p "0.0.0.0:31055:9999" -h "easynote" -v {/home/ctf/202102/pwn_easynote/easynote/bin/flag}:/home/ctf/flag --name="easynote" easynote