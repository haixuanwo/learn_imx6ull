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
#include <linux/platform_device.h>
#include <linux/of.h>

/* 参考: drivers\char\ppdev.c 
 *       drivers\char\ipmi\ipmi_powernv.c
 */

static char hello_buf[100] = "ver2";

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

static int hello_release(struct inode *inode, struct file *file)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

/* 分配/设置/注册 file_operations结构体 */
static const struct file_operations hello_fops = {
	.owner		= THIS_MODULE,
	.read		= hello_read,
	.open		= hello_open,
	.release	= hello_release,
};


static int hello_probe(struct platform_device *pdev)
{
	int err;
	int len;
	const char *vers; /* vers is a pointer, point to "const char"  */

	/* 从匹配的platform_device里得到版本信息
	 * 用来设置: hello_buf
	 */

	err = of_property_read_string_index(pdev->dev.of_node, "version",
				      0, &vers);

	if (err)
	{
		vers = "no version";
	}

	len = strlen(vers) + 1;
	if (len > 100)
		len = 100;
	strncpy(hello_buf, vers, len);
	hello_buf[len - 1] = '\0';
	
	err = register_chrdev(88, "100ask_hello_drv", &hello_fops);
	printk("register_chrdev ret = %d\n", err);
	return err;
}

static int hello_remove(struct platform_device *pdev)
{
	unregister_chrdev(88, "100ask_hello_drv");
	return 0;
}

static const struct of_device_id hello_dt_ids[] = {
	{ .compatible = "100ask,hello", },
	{ /* sentinel */ }
};


/* 分配/设置/注册一个 platform_driver */
static struct platform_driver hello_driver = {
	.driver = {
		.name		    = "100ask_hello",
		.of_match_table = hello_dt_ids,	
	},
	.probe	= hello_probe,
	.remove	= hello_remove,
};

int hello_init(void)
{
	int err;
	err = platform_driver_register(&hello_driver);	
	return err;
}

//int init_module(void) __attribute__((alias("hello_init")));

void hello_exit(void)
{
	platform_driver_unregister(&hello_driver);	
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");

