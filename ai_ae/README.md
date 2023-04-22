# 智能软件系统安全I

## 环境配置（课前准备）

由于大部分同学使用Windows系统，本文档介绍如何在Windows系统下进行相关环境配置。

### 安装Anaconda 5.2.0

1. 打开`https://repo.anaconda.com/archive`，下载其中的文件`Anaconda3-5.2.0-Windows-x86_64.exe`。
2. 下载成功后双击文件，按照提示进行安装
3. 安装成功后打开Anaconda Prompt程序，继续下列步骤：
   1. 创建新的python环境：`conda create -n softsec python==3.6.13`
   2. 激活创建的虚拟环境：`conda activate softsec`
   3. 安装jupyter：`conda install jupyter`
   4. 安装相关依赖：`pip install opencv-python==4.3.0.38`, `pip install tensorflow==1.14`

**注意：**

1. 若出现`ERROR: Could not build wheels for XXX which use PEP 517 and cannot be installed directly`错误，可通过更新pip版本解决：`python -m pip install --upgrade pip`。
2. 注意安装的opencv-python版本为4.3.0.38，否则可能出现`ERROR: Could not build wheels for ... pyproject.toml-based projects`错误。
3. 包含`pip install`的命令，可以通过添加选项`-i https://pypi.tuna.tsinghua.edu.cn/simple`来提高下载速度。

## 实验 - FGSM算法

在Anaconda Prompt中输入jupyter notebook启动程序，打开课程资源中的`fgsm.ipynb`，运行代码进行FGSM对抗攻击。打开`classify.ipynb`，修改两个png图片的文件路径，可以查看模型对于两类样本的预测结果。

## 实验 - 对抗样本防御

在jupyter notebook中打开`adversarial_training.ipynb`并执行代码，可以看到随着训练的进行两类准确率的变化。