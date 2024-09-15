#include <stdio.h>

#include "hello.h"

/* 
 * ./hello name
 */

int main(int argc, char **argv)
{
	if (argc == 2)
		hello(argv[1]);
	else
		hello(NULL);

	return 0;
}

