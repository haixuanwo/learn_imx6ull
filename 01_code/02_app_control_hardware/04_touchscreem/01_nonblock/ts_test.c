#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/input.h>

#if 0
struct input_event {
	struct timeval time;
	__u16 type;
	__u16 code;
	__s32 value;
};
#endif


/*
 * ./ts_test /dev/input/event2
 */

int main(int argc, char **argv)
{
	int fd;
	ssize_t len;
	int rlen = sizeof(struct input_event);
	struct input_event ev;
	
	if (2 != argc)
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDWR|O_NONBLOCK);
	//fd = open(argv[1], O_RDWR);
	if (-1 == fd)
	{
		printf("can not open %s\n", argv[1]);
		return -1;
	}

	while (1)
	{
		len = read(fd, &ev, rlen);
		if (len == rlen)
		{
			printf("Get : type = 0x%x, code = 0x%x, value = 0x%x\n", ev.type, ev.code, ev.value);
		}
	}

	return 0;
}

