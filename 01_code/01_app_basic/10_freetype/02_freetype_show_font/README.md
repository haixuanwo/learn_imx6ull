<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-09-07 15:27:12
 * @LastEditors: Clark
 * @LastEditTime: 2024-09-07 15:43:06
 * @Description: file content
-->

# 编译使用FreeType库的应用程序


cd /home/book/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/arm-buildroot-linux-gnueabihf/sysroot/usr/include
mv freetype2/* ./

arm-buildroot-linux-gnueabihf-gcc -o freetype_show_font freetype_show_font.c -lfreetype

# 在开发板上运行
./freetype_show_font simsun.ttc 24
