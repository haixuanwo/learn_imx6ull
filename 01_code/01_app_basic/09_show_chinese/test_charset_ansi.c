/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-09-07 14:43:35
 * @LastEditors: Clark
 * @LastEditTime: 2024-09-07 14:45:58
 * @Description: file content
 */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *str = "A中";
	int i;

	printf("str's len = %d\n", (int)strlen(str));
	printf("Hex code: ");
	for (i = 0; i < strlen(str); i++)
	{
		printf("%02x ", (unsigned char)str[i]);
	}
	printf("\n");
	return 0;
}