# 使用adb查看手机信息

1. 在设置→关于→连续点击7次版本号，在开发者选项中启动USB调试
2. 数据线与手机连接
3. 解压课程资源中的adb工具，找到adb.exe，在此路径下执行以下命令
    - 查看所有进程
        - adb.exe shell ps
    - 查看指定进程信息
        - adb.exe shell cat /proc/[PID]/stat
        - adb.exe shell cat /proc/[PID]/status
    - 查看其它信息
        - adb.exe devices (查看连接设备)
        - adb.exe shell getprop ro.product.model (查看机型)
        - adb.exe shell dumpsys battery (电池状况)
        - adb.exe shell pm list packages -s (查看系统应用)
        - adb.exe shell pm list packages -3 (查看第三方应用)
    - 查看权限
        - adb.exe shell pm list permissions (查看设备上已经有的权限)
        - adb.exe shell pm list permissions -g (按组查看详细的权限)
        - adb.exe shell pm list permissions -d -g (按组查看危险权限)

# APK重打包实验

1. 查看当前APK的签名
    1. .\jdk-15.0.2\bin\keytool -printcert -jarfile example.apk
2. 查看当前example.apk样式
    1. **通过adb.exe将软件安装到手机：adb.exe install [APK文件路径]**
    2. 在手机上可运行该APP查看样式
3. 修改APK的内容（**加粗的参数为自定义值的参数，但要保持前后一致**）
    1. 解压原APK
        1. .\jdk-15.0.2\bin\java -jar apktool.jar -r d -f **example.apk** -o **out**
    2. 修改NEXT按钮为HOME
        1. 在输出文件：**out**\smali\com\example\helloword\FirstFragment.smali中修改NEXT为HOME
    3. 删除原始签名
        1. 删除out\original\META-INF
    4. 重打包APK
        1. .\jdk-15.0.2\bin\java -jar apktool.jar b **out**/ -o **changed.apk**
    5. 生成签名文件
        1. .\jdk-15.0.2\bin\keytool -genkey -alias **mykey** -keyalg RSA -validity 2000 -keystore **my.keystore**
    6. 重签名
        1. .\jdk-15.0.2\bin\jarsigner -verbose -keystore **my.keystore** -storepass **上一步生成签名输入的密码** -signedjar **resigned.apk** **changed.apk mykey**
    7. 再次查看签名
        1. .\jdk-15.0.2\bin\keytool -printcert -jarfile **resigned.apk**
    8. **在卸载之前安装的APP后**，再次安装新的**resigned.apk**查看样式，NEXT已经变为HOME
