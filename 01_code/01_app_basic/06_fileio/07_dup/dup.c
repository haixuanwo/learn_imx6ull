#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv)
{
    char buf[10];
    char buf2[10];
    
    if (argc != 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    int fd2 = open(argv[1], O_RDONLY);
    int fd3 = dup(fd);

    printf("fd = %d\n", fd);
    printf("fd2 = %d\n", fd2);
    printf("fd3 = %d\n", fd3);

    if (fd < 0 || fd2 < 0 || fd3 < 0)
    {
        printf("can not open %s\n", argv[1]);
        return -1;
    }

    read(fd, buf, 1);
    read(fd2, buf2, 1);

    printf("data get from fd : %c\n", buf[0]);
    printf("data get from fd2: %c\n", buf2[0]);
    
    read(fd3, buf, 1);
    printf("data get from fd3: %c\n", buf[0]);
    
    return 0;
}

