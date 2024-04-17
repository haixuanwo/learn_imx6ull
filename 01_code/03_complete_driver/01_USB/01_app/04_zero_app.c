#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libusb.h>

#define DRIVER_VENDOR_NUM	0x0525		/* NetChip */
#define DRIVER_PRODUCT_NUM	0xa4a0		/* Linux-USB "Gadget Zero" */


int get_bulk_endpoint(libusb_device *dev, int *in_ep, int *out_ep, int *in_ep_maxlen)
{
    struct libusb_config_descriptor *config;
    const struct libusb_endpoint_descriptor *ep;
    int r;
	int iface_idx;
    int found = 0;

    r = libusb_get_active_config_descriptor(dev, &config);
    if (r < 0) {
        printf("could not retrieve active config descriptor");
        return LIBUSB_ERROR_OTHER;
    }

	{
		const struct libusb_interface *iface = &config->interface[0];
		int altsetting_idx = 0;

		const struct libusb_interface_descriptor *altsetting
			= &iface->altsetting[altsetting_idx];
		int ep_idx;

		for (ep_idx = 0; ep_idx < altsetting->bNumEndpoints; ep_idx++) {
			const struct libusb_endpoint_descriptor *ep =
				&altsetting->endpoint[ep_idx];

            if ((ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK)
            {
    			if (ep->bEndpointAddress & LIBUSB_ENDPOINT_IN)
    			{
    				*in_ep = ep->bEndpointAddress;
                    *in_ep_maxlen = ep->wMaxPacketSize;
                    found++;
    			}
                else
                {
                    *out_ep = ep->bEndpointAddress;
                    found++;
                }
            }
		}
	}

    libusb_free_config_descriptor(config);
    return (found == 2) ? 0 : -1;

}

void PrintUsage(char *name)
{
    printf("Usage:\n");
    printf("%s -l : list bConfigurationValue of all configs\n", name);
    printf("%s -s <bConfigurationValue> : select config\n", name);
    printf("%s -wstr <string> : write string\n", name);
    printf("%s -rstr : read string\n", name);
    printf("%s -w <val1 val2 ....> : write bytes\n", name);
    printf("%s -r : read 32 bytes\n", name);
}

int main(int argc, char **argv)
{
    int err = 0;
    libusb_device *dev, **devs;
    int num_devices;
    int endpoint;
    int interface_num = 0;
    int found = 0;
    int transferred;
    int count = 0;
    unsigned char buffer[1024];
    struct libusb_config_descriptor *config_desc;
    struct libusb_device_handle *dev_handle = NULL;
    int i;
    int in_ep, out_ep;
    int in_ep_maxlen;


    if (argc == 1)
    {
        PrintUsage(argv[0]);
        return 0;
    }

    /* libusb_init */
    err = libusb_init(NULL);
    if (err < 0) {
        fprintf(stderr, "failed to initialise libusb %d - %s\n", err, libusb_strerror(err));
        exit(1);
    }


    /* open device */
    dev_handle = libusb_open_device_with_vid_pid(NULL, DRIVER_VENDOR_NUM, DRIVER_PRODUCT_NUM);
	if (!dev_handle) {
		printf("can not open zero device\n");
		return -1;
	}

	dev = libusb_get_device(dev_handle);
    /* 想选择某一个配置, 先知道它的bConfigurationValue */
    if (!strcmp(argv[1], "-l"))
    {
        for (i = 0; i < 255; i++)
        {
            /* parse interface descriptor, find usb mouse */
            err = libusb_get_config_descriptor(dev, i, &config_desc);
            if (err) {
                //fprintf(stderr, "could not get configuration descriptor\n");
                break;
            }
            printf("config %d: bConfigurationValue = %d\n", i, config_desc->bConfigurationValue);
            libusb_free_config_descriptor(config_desc);
        }
        return 0;
    }

    /* 想选择某一个配置 */
    if (!strcmp(argv[1], "-s") && (argc == 3))
    {
        i = strtoul(argv[2], NULL, 0);
        libusb_set_auto_detach_kernel_driver(dev_handle, 0);
        libusb_detach_kernel_driver(dev_handle, 0);
        //libusb_release_interface(dev_handle, 0);
        err = libusb_set_configuration(dev_handle, i);
        if (err) {
            fprintf(stderr, "could not set configuration as %d, err = %d\n", i, err);
            return -1;
        }
        return 0;
    }

    err = libusb_get_configuration(dev_handle, &i);
    fprintf(stdout, "current config: %d\n", i);


    /* 想读写数据需要得到endpoint */
    err = get_bulk_endpoint(dev, &in_ep, &out_ep, &in_ep_maxlen);
    if (err) {
        fprintf(stderr, "could not get bulk endpoints\n");
        goto exit;
    }
    fprintf(stdout, "in_ep = 0x%x, out_ep = 0x%x\n", in_ep, out_ep);

    /* claim interface */
    libusb_set_auto_detach_kernel_driver(dev_handle, 1);
    err = libusb_claim_interface(dev_handle, interface_num);
    if (err)
    {
        fprintf(stderr, "failed to libusb_claim_interface\n");
        goto exit;
    }


    /* write string */
    if (!strcmp(argv[1], "-wstr") && (argc == 3))
    {
        memset(buffer, 0, 32);
        strncpy(buffer, argv[2], 32);

    	err = libusb_bulk_transfer(dev_handle, out_ep,
    							buffer, 32, &transferred, 1000);
        if (err) {
            fprintf(stderr, "libusb_bulk_transfer err = %d\n", err);
            goto exit;
        }

        if (transferred != 32)
        {
            fprintf(stderr, "transferred != 32\n");
        }

        goto exit;
    }

    /* read string */
    if (!strcmp(argv[1], "-rstr"))
    {
        memset(buffer, 0, 32);

    	err = libusb_bulk_transfer(dev_handle, in_ep,
    							buffer, 32, &transferred, 1000);
        if (err) {
            fprintf(stderr, "libusb_bulk_transfer err = %d\n", err);
            goto exit;
        }

        if (transferred != 32)
        {
            fprintf(stderr, "transferred != 32\n");
        }

        printf("Read string: %s\n", buffer);

        goto exit;
    }


    /* write datas */
    if (!strcmp(argv[1], "-w") && (argc >= 3))
    {
        memset(buffer, 0, 32);
        /* argv[2],... */
        for (i = 2; i < argc; i++)
            buffer[i-2] = strtoul(argv[i], NULL, 0);

    	err = libusb_bulk_transfer(dev_handle, out_ep,
    							buffer, argc - 2, &transferred, 1000);
        if (err) {
            fprintf(stderr, "libusb_bulk_transfer err = %d\n", err);
            goto exit;
        }

        if (transferred != argc - 2)
        {
            fprintf(stderr, "transferred != %d\n", argc - 2);
        }

        goto exit;
    }


    /* read datas */
    if (!strcmp(argv[1], "-r")) /* 读Source/Sink这个配置里的端点时, 它一次性返回512字节的数据 */
    {
        memset(buffer, 0, 1024);

    	err = libusb_bulk_transfer(dev_handle, in_ep,
    							buffer, in_ep_maxlen, &transferred, 1000);
        if (err) {
            fprintf(stderr, "libusb_bulk_transfer err = %d\n", err);
            goto exit;
        }

        if (transferred != in_ep_maxlen)
        {
            fprintf(stderr, "transferred != in_ep_maxlen\n");
        }

        printf("Read datas: \n");
        for (i = 0; i < transferred; i++)
        {
            printf("%02x ", buffer[i]);
            if ((i+1) % 16 == 0)
                printf("\n");
        }

        printf("\n");

        goto exit;
    }


exit:
    /* libusb_close */
    libusb_release_interface(dev_handle, interface_num);
    libusb_close(dev_handle);
    libusb_exit(NULL);
    return err;
}


