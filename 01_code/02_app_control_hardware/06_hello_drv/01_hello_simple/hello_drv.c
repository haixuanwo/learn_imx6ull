#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/ctype.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/compat.h>

/* 参考: drivers\char\ppdev.c */

static char hello_buf[100] = "www.100ask.net";

static int hello_open(struct inode *inode, struct file *file)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

/* APP : read(fd, buffer, len) */
static ssize_t hello_read(struct file *file, char __user *buf, size_t count,
		       loff_t *ppos)
{
	int err;
	if (count > 100)
		count = 100;
	err = copy_to_user(buf, hello_buf, count);
	return count;
}

/* APP : write(fd, buffer, len) */
static ssize_t hello_write(struct file *file, const char __user *buf,
		   size_t count, loff_t *ppos)
{
	int err;
	if (count > 100)
		count = 100;
	err = copy_from_user(hello_buf, buf, count);
	return count;
}

static int hello_release(struct inode *inode, struct file *file)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

/* 分配/设置/注册 file_operations结构体 */
static const struct file_operations hello_fops = {
	.owner		= THIS_MODULE,
	.read		= hello_read,
	.write		= hello_write,
	.open		= hello_open,
	.release	= hello_release,
};
	
int hello_init(void)
{
	int err;
	err = register_chrdev(88, "100ask_hello_drv", &hello_fops);
	printk("register_chrdev ret = %d\n", err);
	return err;
}

//int init_module(void) __attribute__((alias("hello_init")));

void hello_exit(void)
{
	unregister_chrdev(88, "100ask_hello_drv");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");

