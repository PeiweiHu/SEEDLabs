# 签到题 signup


## 介绍

本题并无任何解题步骤需要，直接访问题目服务即可获取flag，目的在于教会第一次使用ctfd题板的同学以下基本操作：

- 如何通过nc与题目交互
- 题目的附件是如何被部署为服务的
- 如何查看hints
- 如何提交flag


## 部署

使用`xinetd`来部署题目，用于打包docker镜像的文件位于`signup`目录下：

1. 修改`bin/flag`为你想要的flag值

2. 将编译好的题目二进制文件`signup`放置于`bin/`目录下

3. 修改`ctf.xinetd`的`server_args`使其执行题目二进制文件；修改`port`为容器内某自定义端口（e.g., 10020）；

4. 修改Dockerfile中的`EXPOSE`端口与上面的`port`一致

5. 构建容器镜像并创建容器：

    ```bash
    docker built -t signup .
    docker run -itd -p 31052:10020 --name ctfd_signup signup
    ```

6. 执行`nc localhost 31052`来验证题目是否部署成功


## 题解

直接`nc server_ip 31052`即可看到flag。