#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include <input_manager.h>

int main(int argc, char **argv)
{
	int ret;
	InputEvent event;
	
	InputInit();
	IntpuDeviceInit();

	while (1)
	{
		printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
		ret = GetInputEvent(&event);

		printf("%s %s %d, ret = %d\n", __FILE__, __FUNCTION__, __LINE__, ret);
		if (ret) {
			printf("GetInputEvent err!\n");
			return -1;
		}
		else
		{
			printf("%s %s %d, event.iType = %d\n", __FILE__, __FUNCTION__, __LINE__, event.iType );
			if (event.iType == INPUT_TYPE_TOUCH)
			{
				printf("Type      : %d\n", event.iType);
				printf("iX        : %d\n", event.iX);
				printf("iY        : %d\n", event.iY);
				printf("iPressure : %d\n", event.iPressure);
			}
			else if (event.iType == INPUT_TYPE_NET)
			{
				printf("Type      : %d\n", event.iType);
				printf("str       : %s\n", event.str);
			}
		}
	}
	return 0;	
}


