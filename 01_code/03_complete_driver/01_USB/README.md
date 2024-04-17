<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-17 14:46:54
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-17 15:40:50
 * @Description: file content
-->

# Ubuntu下编译libusb-1.0.21.tar.bz2

```$ tar -jxf libusb-1.0.21.tar.bz2 ```
```$ cd libusb-1.0.21 ```
```$ mkdir build ```
```$ ./configure --enable-udev=no --prefix=`pwd`/build ```
```$ make ```
```$ make install ```


- dnw：USB数据传输
- mouse
- serial：串口通信
- adb
