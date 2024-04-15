<!--
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-13 21:43:49
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-13 21:53:37
 * @Description: file content
-->

# 1 driver 驱动

static struct file_operations hello_drv = {
	.owner	 = THIS_MODULE,
	.open    = hello_drv_open,
	.read    = hello_drv_read,
	.write   = hello_drv_write,
	.release = hello_drv_close,
};

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













