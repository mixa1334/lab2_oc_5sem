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


int main ()
{
    int fdin, fdout;
    char *src, *dst;
    struct stat statbuf;
    int mode = 0x0777;

    /* open the input file */
    if ((fdin = open ("input.txt", O_RDONLY)) < 0)
    {
        perror("open input");
        exit(1);
    }

    /* open/create the output file */
    if ((fdout = open ("output.txt", O_RDWR | O_CREAT | O_TRUNC, mode )) < 0)//edited here
    {
        perror("open output");
        exit(1);
    }

    /* find size of input file */
    if (fstat (fdin,&statbuf) < 0)
    {
        perror("size");
        exit(1);
    }

    /* go to the location corresponding to the last byte */
    if (lseek (fdout, statbuf.st_size - 1, SEEK_SET) == -1)
    {
        perror("location");
        exit(1);
    }

    /* write a dummy byte at the last location */
    if (write (fdout, "", 1) != 1)
    {
        perror("write");
        exit(1);
    }

    /* mmap the input file */
    if ((src = mmap (0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED)
    {
        perror("mmap input");
        exit(1);
    }

    /* mmap the output file */
    if ((dst = mmap (0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0)) == MAP_FAILED)
    {
        perror("mmap output");
        exit(1);
    }

    /* this copies the input file to the output file */
    memcpy (dst, src, statbuf.st_size);
    return 0;
}
