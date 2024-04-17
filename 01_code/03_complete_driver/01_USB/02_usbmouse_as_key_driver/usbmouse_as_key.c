
/* 参考: drivers\hid\usbhid\usbmouse.c */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/input.h>
#include <linux/hid.h>

struct usb_mouse_as_key_desc {
    struct usb_device *dev;
    struct usb_interface *intf;
    const struct usb_device_id *id;
    int pipe, maxp;
    int bInterval;
    void *data_buffer;
    dma_addr_t data_dma;
    struct urb *urb;
};

/* 1. 构造usb_driver 
 * 1.1 id_table : 能支持哪些设备
 * 1.2 probe : 记录某些信息, 分配/设置/注册input_dev, 也许"分配/填充/提交URB"
 */

static struct usb_device_id usb_mouse_as_key_id_table [] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_MOUSE), .driver_info = (kernel_ulong_t)"it is a mouse", },
	{ }	/* Terminating entry */
};


static void usb_mouse_as_key_irq(struct urb *urb)
{
	struct input_dev *dev = urb->context;
    struct usb_mouse_as_key_desc *desc = input_get_drvdata(dev);
	signed char *data = desc->data_buffer;
	int status;

    //printk("%s %s %d, urb->status = %d\n", __FILE__, __FUNCTION__, __LINE__, urb->status);

	switch (urb->status) {
	case 0:			/* success */
		break;
	case -ECONNRESET:	/* unlink */
	case -ENOENT:
	case -ESHUTDOWN:
		return;
	/* -EPIPE:  should clear the halt */
	default:		/* error */
		goto resubmit;
	}

//    printk("data[0] = 0x%x\n", data[1]);
	input_report_key(dev, KEY_L, data[1] & 0x01);
	input_report_key(dev, KEY_S, data[1] & 0x02);
	input_report_key(dev, KEY_ENTER, data[1] & 0x04);

	input_sync(dev);
resubmit:
	status = usb_submit_urb (urb, GFP_ATOMIC);
    //printk("%s %s %d, status = %d\n", __FILE__, __FUNCTION__, __LINE__, status);
}


static int usb_mouse_as_key_open(struct input_dev *dev)
{
    struct urb *urb;
    struct usb_mouse_as_key_desc *desc = input_get_drvdata(dev);
    int err;
    
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    /* 分配/填充/提交 URB */
    urb = usb_alloc_urb(0, GFP_KERNEL);

    desc->urb = urb;

    usb_fill_int_urb(urb,
				     desc->dev,
				     desc->pipe,
				     desc->data_buffer,
				     (desc->maxp > 8 ? 8 : desc->maxp),
				     usb_mouse_as_key_irq,
				     dev,
				     desc->bInterval);
    
	urb->transfer_dma = desc->data_dma;
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    err = usb_submit_urb (urb, GFP_ATOMIC);
    
    printk("%s %s %d, err = %d\n", __FILE__, __FUNCTION__, __LINE__, err);
    
    return err;
}

static void usb_mouse_as_key_close(struct input_dev *dev)
{
    struct usb_mouse_as_key_desc *desc = input_get_drvdata(dev);
    
    /* 取消/释放 URB */
    usb_kill_urb(desc->urb);

    usb_free_urb(desc->urb);
}


static int usb_mouse_as_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    struct usb_device *dev = interface_to_usbdev(intf);
    struct input_dev *input_dev;
    struct usb_mouse_as_key_desc *desc;
    int error;
	struct usb_host_interface *interface;
	struct usb_endpoint_descriptor *endpoint;
	int pipe, maxp;

    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    /* 1. 记录设备信息:  intf */
	interface = intf->cur_altsetting;

	if (interface->desc.bNumEndpoints != 1)
		return -ENODEV;

	endpoint = &interface->endpoint[0].desc;
	if (!usb_endpoint_is_int_in(endpoint))
		return -ENODEV;

	pipe = usb_rcvintpipe(dev, endpoint->bEndpointAddress);
	maxp = usb_maxpacket(dev, pipe, usb_pipeout(pipe));

    desc = kmalloc(sizeof(struct usb_mouse_as_key_desc), GFP_KERNEL);
    desc->dev  = dev;
    desc->intf = intf;
    desc->id   = id;
    desc->pipe = pipe;
    desc->maxp = maxp;
    desc->bInterval = endpoint->bInterval;
    desc->data_buffer = usb_alloc_coherent(dev, maxp, GFP_ATOMIC, &desc->data_dma);
    printk("%s %s %d, desc->data_buffer = 0x%x\n", __FILE__, __FUNCTION__, __LINE__, desc->data_buffer);

    /* 2. 分配/设置/注册input_dev 
     * 2.1 能产生哪类事件
     * 2.2 能产生这类事件里哪些些事件: L/S/ENTER
     * 2.3 设置函数, 比如open
     * 2.4 在open函数里: 分配/填充/提交 URB
     * 2.5 URB的回调函数: 解析数据, 上报input_event
     */

    input_dev = devm_input_allocate_device(&intf->dev);
    
    input_set_drvdata(input_dev, desc);

	/* set 1: which type event ? */	
	__set_bit(EV_KEY, input_dev->evbit);

	/* set 2: which event ? */	
	__set_bit(KEY_L, input_dev->keybit);
	__set_bit(KEY_S, input_dev->keybit);
	__set_bit(KEY_ENTER, input_dev->keybit);

    /* set 3: open */
    input_dev->open  = usb_mouse_as_key_open;
    input_dev->close = usb_mouse_as_key_close;

	error = input_register_device(input_dev);

    usb_set_intfdata(intf, input_dev);
    
    return 0;
}

static void usb_mouse_as_key_disconnect(struct usb_interface *intf)
{
	struct input_dev *input_dev = usb_get_intfdata (intf);
    struct usb_mouse_as_key_desc *desc = input_get_drvdata(input_dev);

    usb_free_coherent(desc->dev, desc->maxp, desc->data_buffer, desc->data_dma);
    kfree(desc);
        
    input_unregister_device(input_dev);
    usb_set_intfdata(intf, NULL);
}


static struct usb_driver usb_mouse_as_key_driver = {
	.name		= "usbmouse_as_key",
	.probe		= usb_mouse_as_key_probe,
	.disconnect	= usb_mouse_as_key_disconnect,
	.id_table	= usb_mouse_as_key_id_table,
};

/* 入口函数  / 出口函数 */
// module_usb_driver(usb_mouse_as_key_driver);
static int __init usb_mouse_as_key__init(void)
{
    printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return usb_register(&usb_mouse_as_key_driver);
}

static void __exit usb_mouse_as_key__init_exit(void)
{ 
	usb_deregister(&usb_mouse_as_key_driver);
}

module_init(usb_mouse_as_key__init);
module_exit(usb_mouse_as_key__init_exit);


/* 2. 注册usb_driver */

MODULE_LICENSE("GPL");


