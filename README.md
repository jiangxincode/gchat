# gchat
    
A IM program runs on Linux, based on gnome, GTK+, C and MySQL.

If you have some advises, please tell me, open a issue, give me a pull request, or send me a email.

+ Aloys
+ jiangxinnju@163.com

## License

LGPL V3, see it on:

http://opensource.org/licenses/lgpl-3.0.html


## Update Log

* Version 0.01-0.XX 2013年完成初步框架。
* Version 1.01 重新开始完善，重组了软件构架，清理了不必要文件，增强了系统鲁棒性，代码等可读性。
* Version 1.02 重新组织了程序结构，主要集中在服务器方面，修复了服务器端用户等添加删除等的潜在bug。
* Version 1.03 对错误定位方式进行了改进。
* Version 1.04 
    * 增加common文件夹，将client和server同时需要的文件进行合并处理。
    * 将gif_defns.h中的内容移到common.h，并因此重组了部分软件结构。
    * 增加void display(gifhdr_t header)函数，用来debug。
* Version 1.05
    * 变更display函数和display_header函数。
    * 修复错误的描述符问题问题。
* Version 1.06
    * 解决文件当前目录的问题。
    * 解决pathname的多线程争用问题。
* Version 1.07
    * 增加图标
* Version 1.08
    * 修复菜单栏不显示问题。
* Version 1.09
    * 精简了部分冗余代码，发现若干bugs。
* Version 1.10
    * 修复了1.09发现等bug
    * 重写了Authon窗口，使用默认值，减少了之后调试等工作量。
* Version 1.11
    * 重写了about窗口。
* Version 1.12
    * 解决了作者声明问题，增加license。
* Version 1.13
    * 解决部分文件路径错误问题。
    * 解决错误信息定位问题。
    * 重写了错误提示框处理函数，可以接受更多错误提示，并修改了相应等调用接口。可读性更高。
* Version 1.14
    * 将client等头文件进行合并，简化了程序结构，并与server保持了统一。
    * 重写了get_full_path_name，合并了服务器端和客户端等操作，并修改了相应的调用接口。
* Version 1.15
    * 修复上一版本引入的bug
    * 使用宏变量代替大多数数字参数，增强了系统可读性。
    * 去除所有编译警告。
* Version 1.16
    * 解决用户密码保密问题。(使用ncurses库)
* Version 1.17
    * 修改本README文件。
    * 删除material文件夹中资料文件。
* Version 1.18
    * 对文件路径长度的定义更加严格。
    * 对线程的使用更加严谨。
    * 删除无用log文件
    
## TODO

* 减少服务器端对用户信息等处理，提高自动化。
* 解决用户处理过程中等海量用户处理问题。
* 解决服务器端用户添加界面问题。
* 当用户使用过程中如果产生错误，不应该停止线程。
* 内存泄漏检测，主要是FILE是否关闭
* 实现聊天室功能
* 中文化完善。
* 实现类似QQ的自动隐藏功能。
* 解决当用户不正常下线时对online.db的处理。
* 当用户输入不合理时要在提交之前进行提醒。
* 添加联系人时如果重复时要提示，并停止添加。
* 打开联系人对话框时需要双击而不是单击。
* gdk线程运行机制。
* MySQL数据库与GTK+的接口。

## Compile and Install

* 编译条件
    * 系统具有gnome开发环境。
    * 使用gcc编译器或者codeblocks等使用gcc为编译器的集成开发环境。
    * 安装有ncurses/mysql库(运行环境和开发环境)

* 使用以下方式加载gnome库：
    * `pkg-config libgnome-2.0 libgnomeui-2.0 --cflags`
    * `pkg-config  libgnome-2.0 libgnomeui-2.0 --libs`
    
* 使用以下方式加载ncurses库：
    * `-lncurses`
    
* 使用以下方式加载MySQL库：
    * `mysql_config --cflags`
    * `mysql_config --libs`

##  Data Files List

* users.db:用户信息数据文件，该文件保存了用户的loginid和password
* online.db:在线用户数据文件，该文件保存了当前在线用户的loginid和socket信息
* $(loginid).db:用户拥有好友文件，该文件保存了当前用户所添加的好友信息
* $(loginid)_as.db:被添加为好友文件，该文件保存了有哪些用户把当前用户添加为好友
* $(loginid)_off.db:离线消息存储文件，该文件保存了其他用户发给当前用户的离线消息



[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/jiangxincode/gchat/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

