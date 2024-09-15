<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-09-07 16:19:21
 * @LastEditors: Clark
 * @LastEditTime: 2024-09-08 09:04:50
 * @Description: file content
-->

https://forums.100ask.net/t/topic/6407

# 查看输入设备信息

ls /dev/input/* -l
ls /dev/event* -l



# 查看输入设备详细信息
cat /proc/bus/input/devices

I: Bus=0019 Vendor=0000 Product=0000 Version=0000
N: Name="20cc000.snvs:snvs-powerkey"
P: Phys=snvs-pwrkey/input0
S: Sysfs=/devices/soc0/soc/2000000.aips-bus/20cc000.snvs/20cc000.snvs:snvs-powerkey/input/input0
U: Uniq=
H: Handlers=kbd event0 evbug
B: PROP=0
B: EV=3
B: KEY=100000 0 0 0

I: Bus=0019 Vendor=0001 Product=0001 Version=0100
N: Name="gpio-keys"
P: Phys=gpio-keys/input0
S: Sysfs=/devices/soc0/gpio-keys/input/input1
U: Uniq=
H: Handlers=kbd event1 evbug
B: PROP=0
B: EV=3
B: KEY=c

# 读取数据
hexdump /dev/input/event0

0001360 125c 66dc b031 000a 0003 0039 ffff ffff
0001370 125c 66dc b031 000a 0001 014a 0000 0000
0001380 125c 66dc b031 000a 0000 0000 0000 0000
0001390 125c 66dc bd3a 000c 0003 0039 0032 0000
00013a0 125c 66dc bd3a 000c 0003 0036 00e3 0000
00013b0 125c 66dc bd3a 000c 0003 0030 0029 0000
00013c0 125c 66dc bd3a 000c 0003 003a 0029 0000
00013d0 125c 66dc bd3a 000c 0001 014a 0001 0000
00013e0 125c 66dc bd3a 000c 0000 0000 0000 0000


# 编译tslib库

tar -Jxf tslib-1.21.tar.xz
cd tslib-1.21
mkdir tmp

./configure --host=arm-buildroot-linux-gnueabihf --prefix=/
make
make install DESTDIR=$PWD/tmp

cd tmp
cp include/* /home/book/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/../lib/gcc/arm-buildroot-linux-gnueabihf/7.5.0/include
cp -d lib/*so* /home/book/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/../lib/gcc/arm-buildroot-linux-gnueabihf/7.5.0/../../../../arm-buildroot-linux-gnueabihf/lib/

关闭Qt的gui
mv /etc/init.d/S07hmi /root
mv /etc/init.d/S05lvgl /root
reboot

运行时需要加载动态库，拷贝到板子上去
cp tslib_install/lib/ts -rfd /lib
cp tslib_install/lib/*so* -d /lib
cp tslib_install/bin/* /bin
cp tslib_install/etc/ts.conf -d /etc
export TSLIB_PLUGINDIR=/lib/ts

测试程序，可在板子上多点触控画图
ts_test_mt


export TSLIB_FBDEVICE=/dev/fb0
export TSLIB_TSDEVICE=/dev/input/event0
export TSLIB_CALIBFILE=/etc/pointercal
export TSLIB_CONFFILE=/etc/ts.conf
export TSLIB_PLUGINDIR=/lib/ts

ts_test_mt





























