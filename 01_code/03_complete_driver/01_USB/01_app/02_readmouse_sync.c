#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libusb.h>

int main(int argc, char **argv)
{
    int err;
    libusb_device *dev, **devs;
    int num_devices;
    int endpoint;
    int interface_num;
    int found = 0;
    int transferred;
    int count = 0;
    unsigned char buffer[16];
    struct libusb_config_descriptor *config_desc;
    struct libusb_device_handle *dev_handle = NULL;

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

        for (int interface = 0; interface < config_desc->bNumInterfaces; interface++)
        {
            const struct libusb_interface_descriptor *intf_desc = &config_desc->interface[interface].altsetting[0];
            interface_num = intf_desc->bInterfaceNumber;

            if (intf_desc->bInterfaceClass != 3 || intf_desc->bInterfaceProtocol != 2)
            {
                continue;
            }
            else
            {
                /* 找到了USB鼠标 */
                fprintf(stdout, "find usb mouse ok\n");
                for (int ep = 0; ep < intf_desc->bNumEndpoints; ep++)
                {
                    if ((intf_desc->endpoint[ep].bmAttributes & 3) == LIBUSB_TRANSFER_TYPE_INTERRUPT ||
                            (intf_desc->endpoint[ep].bEndpointAddress & 0x80) == LIBUSB_ENDPOINT_IN)
                    {
                        /* 找到了输入的中断端点 */
                        fprintf(stdout, "find in int endpoint\n");
                        endpoint = intf_desc->endpoint[ep].bEndpointAddress;
                        found = 1;
                        break;
                    }
                }
            }

            if (found)
                break;
        }

        libusb_free_config_descriptor(config_desc);

        if (found)
            break;
    }

    if (!found)
    {
        /* free device list */
        libusb_free_device_list(devs, 1);
        libusb_exit(NULL);
        exit(1);
    }

    if (found)
    {
        /* libusb_open */
        err = libusb_open(dev, &dev_handle);
        if (err)
        {
            fprintf(stderr, "failed to open usb mouse\n");
            exit(1);
        }
        fprintf(stdout, "libusb_open ok\n");
    }

    /* free device list */
    libusb_free_device_list(devs, 1);

    /* claim interface */
    libusb_set_auto_detach_kernel_driver(dev_handle, 1);
    err = libusb_claim_interface(dev_handle, interface_num);
    if (err)
    {
        fprintf(stderr, "failed to libusb_claim_interface\n");
        exit(1);
    }
    fprintf(stdout, "libusb_claim_interface ok\n");

    /* libusb_interrupt_transfer */
    while (1)
    {
        err = libusb_interrupt_transfer(dev_handle, endpoint, buffer, 16, &transferred, 5000);
        if (!err) {
            /* parser data */
            printf("%04d datas: ", count++);
            for (int i = 0; i < transferred; i++)
            {
                printf("%02x ", buffer[i]);
            }
            printf("\n");
        } else if (err == LIBUSB_ERROR_TIMEOUT){
            fprintf(stderr, "libusb_interrupt_transfer timout\n");
        } else {
            fprintf(stderr, "libusb_interrupt_transfer err : %d\n", err);
            //exit(1);
        }

    }

    /* libusb_close */
    libusb_release_interface(dev_handle, interface_num);
    libusb_close(dev_handle);
    libusb_exit(NULL);
}


