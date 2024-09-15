#include <stdio.h>



int hello(char *name, int age)
{
	if (name == 0)
	{
		printf("hello, nobody\n");
		return -1;
	}
	else
	{
		printf("hello, %s, are you %d years old?\n", name, age);
		return 0;
	}
}


