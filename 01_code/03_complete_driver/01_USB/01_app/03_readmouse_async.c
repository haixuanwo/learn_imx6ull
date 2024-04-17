#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libusb.h>

struct usb_mouse {
    struct libusb_device_handle *handle;
    int interface;
    int endpoint;
    unsigned char buf[16];
    int transferred;
    struct libusb_transfer *transfer;
    struct usb_mouse *next;
};

static struct usb_mouse *usb_mouse_list;


void free_usb_mouses(struct usb_mouse *usb_mouse_list)
{
    struct usb_mouse *pnext;
    while (usb_mouse_list)
    {
        pnext = usb_mouse_list->next;
        free(usb_mouse_list);
        usb_mouse_list = pnext;
    }
}


/*  */
int get_usb_mouses(libusb_device **devs, int num_devices, struct usb_mouse **usb_mouse_list)
{
    int err;
    libusb_device *dev;
    int endpoint;
    int interface_num;
    struct libusb_config_descriptor *config_desc;
    struct libusb_device_handle *dev_handle = NULL;
    struct usb_mouse *pmouse;
    struct usb_mouse *list = NULL;
    int mouse_cnt = 0;

    /* for each device, get config descriptor */
    for (int i = 0; i < num_devices; i++) {
        dev = devs[i];

        /* parse interface descriptor, find usb mouse */
        err = libusb_get_config_descriptor(dev, 0, &config_desc);
        if (err) {
            fprintf(stderr, "could not get configuration descriptor\n");
            continue;
        }
        fprintf(stdout, "libusb_get_config_descriptor() ok\n");

        for (int interface = 0; interface < config_desc->bNumInterfaces; interface++) {
            const struct libusb_interface_descriptor *intf_desc = &config_desc->interface[interface].altsetting[0];
            interface_num = intf_desc->bInterfaceNumber;

            if (intf_desc->bInterfaceClass != 3 || intf_desc->bInterfaceProtocol != 2)
                continue;
            else

{
                /* 找到了USB鼠标 */
                fprintf(stdout, "find usb mouse ok\n");
                for (int ep = 0; ep < intf_desc->bNumEndpoints; ep++)
                {
                    if ((intf_desc->endpoint[ep].bmAttributes & 3) == LIBUSB_TRANSFER_TYPE_INTERRUPT ||
                            (intf_desc->endpoint[ep].bEndpointAddress & 0x80) == LIBUSB_ENDPOINT_IN) {
                        /* 找到了输入的中断端点 */
                        fprintf(stdout, "find in int endpoint\n");
                        endpoint = intf_desc->endpoint[ep].bEndpointAddress;

                        /* libusb_open */
                        err = libusb_open(dev, &dev_handle);
                        if (err)
                        {
                            fprintf(stderr, "failed to open usb mouse\n");
                            return -1;
                        }
                        fprintf(stdout, "libusb_open ok\n");

                        /* 记录下来: 放入链表 */
                        pmouse = malloc(sizeof(struct usb_mouse));
                        if (!pmouse)
                        {
                            fprintf(stderr, "can not malloc\n");
                            return -1;
                        }
                        pmouse->endpoint  = endpoint;
                        pmouse->interface = interface_num;
                        pmouse->handle    = dev_handle;
                        pmouse->next      = NULL;

                        if (!list)
                            list = pmouse;
                        else
                        {
                            pmouse->next = list;
                            list = pmouse;
                        }
                        mouse_cnt++;
                        break;
                    }

                }
            }

        }

        libusb_free_config_descriptor(config_desc);
    }

    *usb_mouse_list = list;
    return mouse_cnt;
}


static void mouse_irq(struct libusb_transfer *transfer)
{
    static int count = 0;
    if (transfer->status == LIBUSB_TRANSFER_COMPLETED)
    {
        /* parser data */
        printf("%04d datas: ", count++);
        for (int i = 0; i < transfer->actual_length; i++)
        {
            printf("%02x ", transfer->buffer[i]);
        }
        printf("\n");

    }

    if (libusb_submit_transfer(transfer) < 0)
    {
        fprintf(stderr, "libusb_submit_transfer err\n");
    }
}

int main(int argc, char **argv)
{
    int err;
    libusb_device **devs;
    int num_devices, num_mouse;
    struct usb_mouse *pmouse;

    /* libusb_init */

    err = libusb_init(NULL);
    if (err < 0) {
        fprintf(stderr, "failed to initialise libusb %d - %s\n", err, libusb_strerror(err));
        exit(1);
    }

    /* get device list */
    if ((num_devices = libusb_get_device_list(NULL, &devs)) < 0) {
        fprintf(stderr, "libusb_get_device_list() failed\n");
        libusb_exit(NULL);
        exit(1);
    }
    fprintf(stdout, "libusb_get_device_list() ok\n");

    /* get usb mouse */
    num_mouse = get_usb_mouses(devs, num_devices, &usb_mouse_list);

    if (num_mouse <= 0)
    {
        /* free device list */
        libusb_free_device_list(devs, 1);
        libusb_exit(NULL);
        exit(1);
    }
    fprintf(stdout, "get %d mouses\n", num_mouse);

    /* free device list */
    libusb_free_device_list(devs, 1);

    /* claim interface */
    pmouse = usb_mouse_list;
    while (pmouse)
    {
        libusb_set_auto_detach_kernel_driver(pmouse->handle, 1);
        err = libusb_claim_interface(pmouse->handle, pmouse->interface);
        if (err)
        {
            fprintf(stderr, "failed to libusb_claim_interface\n");
            exit(1);
        }
        pmouse = pmouse->next;
    }
    fprintf(stdout, "libusb_claim_interface ok\n");

    /* for each mouse, alloc transfer, fill transfer, submit transfer */
    pmouse = usb_mouse_list;
    while (pmouse)
    {
        /* alloc transfer */
        pmouse->transfer = libusb_alloc_transfer(0);

        /* fill transfer */
        libusb_fill_interrupt_transfer(pmouse->transfer, pmouse->handle, pmouse->endpoint, pmouse->buf,
            sizeof(pmouse->buf), mouse_irq, pmouse, 0);

        /* submit transfer */
        libusb_submit_transfer(pmouse->transfer);

        pmouse = pmouse->next;
    }

    /* handle events */
    while (1) {
        struct timeval tv = { 5, 0 };
        int r;

        r = libusb_handle_events_timeout(NULL, &tv);
        if (r < 0) {
            fprintf(stderr, "libusb_handle_events_timeout err\n");
            break;
        }
    }


    /* libusb_close */
    pmouse = usb_mouse_list;
    while (pmouse)
    {
        libusb_release_interface(pmouse->handle, pmouse->interface);
        libusb_close(pmouse->handle);
        pmouse = pmouse->next;
    }

    free_usb_mouses(usb_mouse_list);

    libusb_exit(NULL);
}


