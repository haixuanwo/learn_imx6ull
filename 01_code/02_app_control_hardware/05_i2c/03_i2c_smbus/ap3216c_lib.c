
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <i2cbusses.h>
#include <i2c/smbus.h>


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
	err = i2c_smbus_write_byte_data(fd, 0, 4);
	if (err)
	{
		printf("reset ap3216c err\n");
		return err;
	}
	
	/* enable ap3216c */
	err = i2c_smbus_write_byte_data(fd, 0, 3);
	if (err)
	{
		printf("enable ap3216c err\n");
		return err;
	}
	
	return 0;
}

int ap3216c_read_light(void)
{
	return i2c_smbus_read_word_data(fd, 0xc);	
}

int ap3216c_read_distance(void)
{

	int val = i2c_smbus_read_word_data(fd, 0xe);	
	char buf_rx[2];
	int distance;

	buf_rx[0] = val & 0xff;
	buf_rx[1] = val >> 8;
	
	distance = ((buf_rx[1] & 0x3F)<<4) | (buf_rx[0] & 0xf);
	
	return distance;
}



