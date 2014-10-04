# gchat #
    
* VERAION:    **0.01**
* AUTHOR:    **jiangxin**
* Email:    **jiangxinnju@163.com**
* Copyright (c) 2014, **jiangxin**

一个Linux上等聊天工具。

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of the jiangxin nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



* Version 0.01-0.XX 2013年完成初步框架。
* Version 1.01 重新开始完善，重组了软件构架，清理了不必要文件，增强了系统鲁棒性，代码等可读性。
* Version 1.02 重新组织了程序结构，主要集中在服务器方面，修复了服务器端用户等添加删除等的潜在bug。
* Version 1.03 对错误定位方式进行了改进。


将要修复等问题：
错误信息定位问题
错误的描述符问题
用户密码保密问题
减少服务器端对用户信息等处理，提高自动化
解决用户处理过程中等海量用户处理问题
解决服务器端用户添加界面问题

<img src="" />


～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

数据文件说明：

* users.db:用户信息数据文件，该文件保存了用户的loginid和password
* online.db:在线用户数据文件，该文件保存了当前在线用户的loginid和socket信息
* $(loginid).db:用户拥有好友文件，该文件保存了当前用户所添加的好友信息
* $(loginid)_as.db:被添加为好友文件，该文件保存了有哪些用户把当前用户添加为好友
* $(loginid)_off.db:离线消息存储文件，该文件保存了其他用户发给当前用户的离线消息

