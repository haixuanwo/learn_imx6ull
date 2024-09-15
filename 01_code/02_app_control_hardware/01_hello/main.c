
#include <hello.h>  /* 在哪里查找头文件? */
//#include "hello.h"  /* C文件当前目录下查找 */

//int hello(char *name, int age);

/* 执行命令: ./hello weidongshan 
 * argc = 2
 * argv[0] = ./hello
 * argv[1] = weidongshan
 */

int main(int argc, char **argv)
{
	if (argc == 2)
		hello(argv[1], 1);
	else
		hello(0, 1);
	return 0;
}


