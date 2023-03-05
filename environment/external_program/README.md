# Note

上述实验如果在本书提供的Ubuntu 16.04虚拟机上做的话，得到的只是一个普通的shell，而不是root shell。这是由Ubuntu 16.04的一个保护机制导致的。这已经在第一章的1.5.1节中做了解释。可以通过：

+ sudo ln -sf /bin/zsh /bin/sh

来缓解。实验结束后可以通过`sudo ln -sf /bin/dash /bin/sh`修改回来。
