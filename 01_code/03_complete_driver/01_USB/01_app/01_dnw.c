/*
 *      To send file through USB for s3c2410/s3c2440's bootloader, vivi/u-boot
 *
 *      Copyright (C) 2006 thisway.diy@163.com
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include "libusb.h"

#define DMA_BUF_SIZE	0x8000
#define	DNW_DEVICE_IDVENDOR		0x5345
#define	DNW_DEVICE_IDPRODUCT	0x1234


const char version[] = "DNW on linux for www.100ask.net, v2.0 with libusb1.0";
const char author[]  = "thisway.diy@163.com";
	
void printusage(char *name)
{
	printf("Usage: %s <file> [address], send file to USB device\n", name);
	printf("       %s -v, show the version\n", name);
	printf("examples:\n");
	printf("1. %s vmlinus\n", name);
	printf("2. %s yaffs_image 0x30000000\n", name);
}


int main(int argc, char *argv[])
{
	int i;

	int first = 1;
	
	int fd_file;

	struct stat stat_buf;

	libusb_device_handle *hdev = NULL;


	unsigned int ret;
	int status;
	
	unsigned int num_remain = 2;
	unsigned int num_read = 0;
	unsigned int num_write = 0;
	unsigned int num_read_total = 0;
	unsigned int num_write_total = 0;
	unsigned int num_has_write_total = 0;

	int transferred;

	unsigned int download_addr = 0x30000000;
	unsigned short check_sum = 0;

	unsigned char *buf = 0;

	struct timeval time_start;
	struct timeval time_end;
	unsigned int time_us;

	if (argc == 2 && strcmp(argv[1], "-v") == 0)
	{
		printf("Version: %s\n", version);
		printf("Complie Time: %s, %s\n", __DATE__, __TIME__);
		printf("Author: %s\n", author);
		return 0;
	}

	if (argc < 2 || argc > 3)
	{
		printusage(argv[0]);
		return -1;
	}

	/* open the device using libusb */
	status = libusb_init(NULL);
	if (status < 0) {
		printf("libusb_init() failed: %s\n", libusb_error_name(status));
		return -1;
	}

	hdev = libusb_open_device_with_vid_pid(NULL, (uint16_t)DNW_DEVICE_IDVENDOR, (uint16_t)DNW_DEVICE_IDPRODUCT);
	if (hdev == NULL) {
		printf("libusb_open() failed\n");
		goto err3;
	}

	/* We need to claim the first interface */
	libusb_set_auto_detach_kernel_driver(hdev, 1);
	status = libusb_claim_interface(hdev, 0);
	if (status != LIBUSB_SUCCESS) {
		libusb_close(hdev);
		printf("libusb_claim_interface failed: %s\n", libusb_error_name(status));
		goto err2;
	}
	
	
	fd_file = open(argv[1], O_RDONLY);
	if (fd_file < 0) 
	{
		printf("open %s failed!\n", argv[2]);
		goto err1;
	}

	if (fstat(fd_file, &stat_buf))
	{
		printf("Can't get status of file %s\n", argv[2]);
		goto err;
	}

	printf("Size of file: %d byte(s)\n", (unsigned int)stat_buf.st_size);

	/* FORMAT: <ADDR(DATA):4>+<SIZE(n+10):4>+<DATA:n>+<CS:2> */
	num_write_total = stat_buf.st_size + 10;

	if (argc == 3)
	{
		download_addr = (unsigned int)strtoul((const char *)argv[2], NULL, 0);
	}

	printf("Address of bootloader to receive data: 0x%x\n", download_addr);

	buf = malloc(DMA_BUF_SIZE);
	if (!buf)
	{
		printf("Can't alloc memory\n");
		goto err;
	}

	memcpy(buf, &download_addr, 4);
	memcpy(buf + 4, &num_write_total, 4);

	gettimeofday(&time_start, 0);

	while (num_read_total < stat_buf.st_size)
	{
		num_read = read(fd_file, buf + (first ? 8 : 0), DMA_BUF_SIZE - (first ? 8 : 0));

		if (num_read)
		{
			num_read_total += num_read;
			for (i = 0; i < num_read; i++)
			{
				check_sum += (unsigned short)buf[i];
			}
		}
		else
		{
			printf("Can't read file, num_read = %d, num_read_total = %d\n", num_read, num_read_total);
			goto err;
		}

		num_write = num_read + (first ? 8 : 0);
		
		if (num_read_total == stat_buf.st_size)
		{
			for (i = 0; i < 2; i++)
			{
				if (num_write < DMA_BUF_SIZE)
				{
					buf[num_write++] = (unsigned char)(check_sum >> (i*8));
					num_remain--;
				}
			}
		}

/*
int libusb_bulk_transfer 	( 	struct libusb_device_handle *  	dev_handle,
		unsigned char  	endpoint,
		unsigned char *  	data,
		int  	length,
		int *  	transferred,
		unsigned int  	timeout 
	) 	

*/
		ret =  libusb_bulk_transfer(hdev, 0x03, buf, num_write, &transferred, 3000);

		if ((ret != 0) || (transferred != num_write))
		{
			printf("Error, can't send to the board: num_write = %d, transferred = %d, ret = %d\n", num_write, transferred, ret);
			goto err;			
		}
		
		num_has_write_total += num_write;

		gettimeofday(&time_end, 0);

		time_us = (time_end.tv_sec - time_start.tv_sec) * 1000000 +(time_end.tv_usec - time_start.tv_usec);

		printf("\rTx: %d/%d, Percentage: %.1f%%, Speed: %.1fKB/s, Time: %.1fS", num_has_write_total, num_write_total, ((float)num_has_write_total)/((float)num_write_total)*100., ((float)num_has_write_total/1024)/((float)time_us/1000000), time_us/1000000.);

		if (first)
		{
			first = 0;
		}
		
	}		

	if (num_remain == 2)
	{
		buf[0] = (unsigned char)check_sum;
		buf[1] = (unsigned char)(check_sum >> 8);
	}
	else if (num_remain == 1)
	{
		buf[0] = (unsigned char)(check_sum >> 8);
	}

	if (num_remain)
	{
		num_write = num_remain;
		ret =  libusb_bulk_transfer(hdev, 0x03, buf, num_write, &transferred, 3000);

		if ((ret != 0) || (transferred != num_write))
		{
			printf("Error, can't send to the board: num_write = %d, ret = %d\n", num_write, ret);
			goto err;
		}
		
		num_has_write_total += num_write;
	}

	gettimeofday(&time_end, 0);

	time_us = (time_end.tv_sec - time_start.tv_sec) * 1000000 +(time_end.tv_usec - time_start.tv_usec);

	printf("\rTx: %d/%d, Percentage: %.1f%%, Speed: %.1fKB/s, Time: %.1fS\n", num_has_write_total, num_write_total, ((float)num_has_write_total)/((float)num_write_total)*100., ((float)num_has_write_total/1024)/((float)time_us/1000000), time_us/1000000.);

err:
	close(fd_file);
err1:
	libusb_release_interface(hdev, 0);
err2:
	libusb_close(hdev);
err3:
	libusb_exit(NULL);
	return 0;
}

