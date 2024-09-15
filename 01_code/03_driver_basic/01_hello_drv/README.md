<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-09-08 16:49:35
 * @LastEditors: Clark
 * @LastEditTime: 2024-09-08 17:03:54
 * @Description: file content
-->

# 注意改kernel的路径

make
cp hello_drv.ko hello_drv_test ~/nfs_rootfs/

在开发板运行
insmod hello_drv.ko
./hello_drv_test -w tanxiaohai
./hello_drv_test -r


驱动程序的核心是 file_operations 结构体：分配、设置、注册它。


以下代码将会在“/sys/class”目录下创建一个子目录“hello_class”：
hello_class = class_create(THIS_MODULE, "hello_class");

以下代码将会在“/sys/class/hello_class”目录下创建一个文件“hello”：
device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello");
