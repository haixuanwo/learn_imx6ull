<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-09-07 14:55:34
 * @LastEditors: Clark
 * @LastEditTime: 2024-09-07 15:17:19
 * @Description: file content
-->

# 交叉编译libpng

tar Jxf libpng-1.6.37.tar.xz
cd libpng-1.6.37
mkdir tmp
./configure --host=arm-buildroot-linux-gnueabihf --prefix=$PWD/tmp
make -j4
make install

cd tmp
cp -rf include/* /home/book/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/../lib/gcc/arm-buildroot-linux-gnueabihf/7.5.0/include

cp -rf lib/* /home/book/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/../lib/gcc/arm-buildroot-linux-gnueabihf/7.5.0/../../../../arm-buildroot-linux-gnueabihf/lib


# 交叉编译freetype

tar xJf freetype-2.10.2.tar.xz
cd freetype-2.10.2
mkdir tmp
./configure --host=arm-buildroot-linux-gnueabihf --prefix=$PWD/tmp
make -j4
make install

cd tmp
cp include/* -rf /home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/../arm-linux-gnueabihf/libc/usr/include/

cp lib/* -rf /home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/../arm-linux-gnueabihf/libc/usr/lib/




