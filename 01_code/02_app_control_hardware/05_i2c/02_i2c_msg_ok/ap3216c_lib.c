
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "i2cbusses.h"


#define I2C_BUS      0
#define AP3216C_ADDR 0x1e

static int fd;

int ap3216c_init(void)
{
	char buf[100];
	int err;
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msgs[1];
	int nmsgs_sent;
	
	fd = open_i2c_dev(I2C_BUS, buf, sizeof(buf), 0);
	if (fd < 0)
	{
		printf("can not open i2c bus %d\n", I2C_BUS);
		return fd; /* err */
	}

	err = set_slave_addr(fd, AP3216C_ADDR, 1);
	if (err)
	{
		printf("can not set slave addr 0x%x\n", AP3216C_ADDR);
		return err;
	}

	/* reset ap3216c */
	msgs[0].addr  = AP3216C_ADDR;
	msgs[0].flags = 0;      /* 写:0, 读:I2C_M_RD */
	msgs[0].len   = 2;
	msgs[0].buf   = buf;
	buf[0] = 0;
	buf[1] = 4;

	rdwr.msgs = msgs;
	rdwr.nmsgs = 1;
	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);
	if (nmsgs_sent != 1)
	{
		printf("can not reset ap3216c\n");
		return -1;  /* err */
	}

	/* enable ap3216c */
	msgs[0].addr  = AP3216C_ADDR;
	msgs[0].flags = 0;      /* 写:0, 读:I2C_M_RD */
	msgs[0].len   = 2;
	msgs[0].buf   = buf;
	buf[0] = 0;
	buf[1] = 3;

	rdwr.msgs = msgs;
	rdwr.nmsgs = 1;
	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);
	if (nmsgs_sent != 1)
	{
		printf("can not enable ap3216c\n");
		return -1;  /* err */
	}
	
	return 0;
}

int ap3216c_read_light(void)
{
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msgs[2];
	int nmsgs_sent;

	char buf_tx[1];
	char buf_rx[2];

	int light;

	/* 发送寄存器地址 */
	msgs[0].addr  = AP3216C_ADDR;
	msgs[0].flags = 0;      /* 写:0, 读:I2C_M_RD */
	msgs[0].len   = 1;
	msgs[0].buf   = buf_tx;
	buf_tx[0] = 0xc;

	msgs[1].addr  = AP3216C_ADDR;
	msgs[1].flags = I2C_M_RD;      /* 写:0, 读:I2C_M_RD */
	msgs[1].len   = 2;
	msgs[1].buf   = buf_rx;

	rdwr.msgs = msgs;
	rdwr.nmsgs = 2;
	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);
	if (nmsgs_sent != 2)
	{
		printf("can not read ap3216c light\n");
		return -1;  /* err */
	}

	light = (buf_rx[1]<<8) | buf_rx[0];
	
	return light;
}

int ap3216c_read_distance(void)
{
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msgs[2];
	int nmsgs_sent;

	char buf_tx[1];
	char buf_rx[2];

	int distance;

	/* 发送寄存器地址 */
	msgs[0].addr  = AP3216C_ADDR;
	msgs[0].flags = 0;		/* 写:0, 读:I2C_M_RD */
	msgs[0].len   = 1;
	msgs[0].buf   = buf_tx;
	buf_tx[0] = 0xe;

	msgs[1].addr  = AP3216C_ADDR;
	msgs[1].flags = I2C_M_RD;	   /* 写:0, 读:I2C_M_RD */
	msgs[1].len   = 2;
	msgs[1].buf   = buf_rx;

	rdwr.msgs = msgs;
	rdwr.nmsgs = 2;
	nmsgs_sent = ioctl(fd, I2C_RDWR, &rdwr);
	if (nmsgs_sent != 2)
	{
		printf("can not read ap3216c light\n");
		return -1;	/* err */
	}

	distance = ((buf_rx[1] & 0x3F)<<4) | (buf_rx[0] & 0xf);
	
	return distance;
}



