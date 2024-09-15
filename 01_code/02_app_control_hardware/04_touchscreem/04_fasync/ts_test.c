#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/input.h>
#include <sys/select.h>
#include <signal.h>

#if 0
struct input_event {
	struct timeval time;
	__u16 type;
	__u16 code;
	__s32 value;
};
#endif

int fd;

void my_sig_function(int sig)
{
	ssize_t len;
	int rlen = sizeof(struct input_event);
	struct input_event ev;

	while (1)
	{
		len = read(fd, &ev, rlen);
		if (len == rlen)
		{
			printf("Get : type = 0x%x, code = 0x%x, value = 0x%x\n", ev.type, ev.code, ev.value);
		}
		else
		{
			break;
		}
	}
}


/*
 * ./ts_test /dev/input/event2
 */

int main(int argc, char **argv)
{
	ssize_t len;
	int rlen = sizeof(struct input_event);
	struct input_event ev;

	unsigned int flags;

	
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

	/* 3. 我收到信号后"做什么" */
	signal(SIGIO, my_sig_function);

	/* 1. 告诉小孩:  你醒了之后通知"谁" */
	fcntl(fd, F_SETOWN, getpid());
	
	/* 2. 使能异步通知, 告诉小孩:  你醒了之后要"发信号" */
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | FASYNC);

		   
	while (1)
	{
		printf("read book\n");
		sleep(10);
	}

	return 0;
}

