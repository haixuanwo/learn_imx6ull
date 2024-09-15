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


/* 参考 :drivers/char/ipmi/ipmi_msghandler.c  */

static struct resource hello_resources[] = {
	{
		.name	= "ver123",
		.start	= 0,
		.end	= 3,
		.flags	= IORESOURCE_MEM,
	}, 
};


static void	hello_dev_release(struct device *dev)
{
}

/* 分配/设置/注册 platform_device */
static struct platform_device hello_device = {
	.name			= "100ask_hello",
	.id			= -1,
	.dev = {
		.release = hello_dev_release,
	},
	.num_resources		= ARRAY_SIZE(hello_resources),
	.resource		= hello_resources,
};

int hello_dev_init(void)
{
	int err;
	err = platform_device_register(&hello_device);	
	return err;
}

//int init_module(void) __attribute__((alias("hello_init")));

void hello_dev_exit(void)
{
	platform_device_unregister(&hello_device);	
}


module_init(hello_dev_init);
module_exit(hello_dev_exit);
MODULE_LICENSE("GPL");

