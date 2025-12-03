
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "get_next_line.h"

int main(void)
{
    int fd;
    fd = open("get_next_line.h" , O_RDONLY);

    char *a = get_next_line(fd);
    while (a)
    {
        printf("%s", a);
        free(a);
        a = get_next_line(fd);
    }
    close(fd);
}
