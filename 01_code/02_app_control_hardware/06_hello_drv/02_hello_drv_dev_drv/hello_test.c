#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


/* ./hello_test r
 */
int main(int argc, char **argv)
{
	int fd;
	char app_buf[100];
	int len = 0;
	
	if (argc == 1)
	{
		printf("Usage:\n");
		printf("%s r          - read version from hello drv\n", argv[0]);
		return -1;
	}

	/* open */
	fd = open("/dev/abcxyz", O_RDWR);
	if (fd < 0)
	{
		printf("open /dev/abcxyz err\n");
		return -1;
	}

	if ((argc == 2) && !strcmp(argv[1], "r"))
	{
		/* read */
		len = read(fd, app_buf, 100);
		app_buf[len - 1] = '\0';

		/* printf */
		printf("get str from drv: %s\n", app_buf);
		
	}

	//close(fd);
	return 0;
}

