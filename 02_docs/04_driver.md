<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-13 21:43:49
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-15 16:33:16
 * @Description: file content
-->

# 1 driver 驱动

Linux 驱动 = 软件框架 + 硬件操作

GPIO 功能：
- 输入输出
- 防抖动
- 中断
- 唤醒


static struct file_operations hello_drv = {
	.owner	 = THIS_MODULE,
	.open    = hello_drv_open,
	.read    = hello_drv_read,
	.write   = hello_drv_write,
	.release = hello_drv_close,
};

## 怎么编写驱动程序
- 确定主设备号，也可以让内核分配
- 定义file_operations结构体
- 实现对应的drv_open/drv_read/drv_write等函数，填入file_operations结构体
- 把file_operations结构体告诉内核：
- 注册驱动程序入口函数register_chrdev
- 卸载驱动程序出口函数unregister_chrdev
- 其他完善：提供设备信息，自动创建设备节点：class_create, device_create

copy_to_user
copy_from_user

register_chrdev
class_create
device_create

device_destroy
class_destroy
unregister_chrdev

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");

# 2
alloc_chrdev_region
cdev_init
cdev_add
cdev_del

# 3

GPIO5_GDIR = ioremap(0x020AC004, 4);

struct inode *inode = file_inode(file);
int minor = iminor(inode);













