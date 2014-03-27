echo "Start install"
tree > /home/jiangxin/temp/before_autogen.log
./autogen.sh
tree > /home/jiangxin/temp/before_configure.log
./configure --prefix=/home/jiangxin/lab03/gchat_client/test/
tree > /home/jiangxin/temp/before_make.log
make
tree > /home/jiangxin/temp/before_make_install.log
make install
tree > /home/jiangxin/temp/after_make_install.log
echo "end"

echo "test"
./test/bin/gchat
