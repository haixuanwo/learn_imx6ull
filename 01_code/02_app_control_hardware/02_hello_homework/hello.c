#include <stdio.h>

#include "hello.h"

void hello(char *name)
{
	if (name)
		printf("hello, %s\n", name);
	else
		printf("hello, nobody\n");
}


