#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


int main()
{
    int fd;
    struct stat mystat;
    void *pmap;

    fd = open("input.txt", O_RDWR);
    if(fd == -1)
    {
        perror("open");
        exit(1);
    }

    if(fstat(fd, &mystat) < 0)
    {
        perror("fstat");
        close(fd);
        exit(1);
    }

    pmap = mmap(0, mystat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(pmap == MAP_FAILED)
    {
        perror("map");
        close(fd);
        exit(1);
    }

    printf("%s", (char*)pmap);

    strncpy(pmap, "wtf", 3);

    printf("%s", (char*)pmap);

    close(fd);

	return 0;
}